// ///////////////////////////////////////////////////////////////////////////
// xrb_parse_datafile_value.hpp by Victor Dods, created 2005/07/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_PARSE_DATAFILE_VALUE_HPP_)
#define _XRB_PARSE_DATAFILE_VALUE_HPP_

#include "xrb.hpp"

#include <map>
#include <stdio.h>
#include <string>
#include <vector>

#include "xrb_util.hpp"
#include "xrb_indentformatter.hpp"

namespace Xrb {
namespace Parse {
namespace DataFile {

/*
temp DataFile::Value notes - turn into real doxygen docs later

data types:
    boolean        - true, false
    [un]signed int - binary, octal, decimal or hexadecimal
    floating point - IEEE floating point (of the type given by Float)
    character      - ascii character
    string         - string of ascii characters

atomic language prototypes:
    identifier     - C-language identifier [_a-zA-Z][_a-zA-Z0-9]*

conglomerate data types:
    key/value pair - identifier/value
    array          - comma-delimited list of homogenous values,
                     all enclosed inside [ ].  possible element types are:
                     boolean, int, float, character, string, key/value pair,
                     array, and no elements.  array nesting indicates
                     multiple dimensions.  if the array contains key/value
                     pairs, the key identifiers do not have to be unique.
    structure      - semicolon-delimited list (semicolon after each key/value
                     pair) of key/value pairs, with all keys having names
                     unique to that structure, all enclosed inside { }

the format of a data file is the inside of a structure -- semicolon-delimited
list (semicolon after each key/value pair) of key/value pairs.  e.g.

    mass 5.6;
    elasticity 0.311;
    vertices [ [0, 0], [1, 0], [1, 1], [0, 1] ];
    properties
    {
        name "spiny norman";
        is_enemy true;
        AI_level 18;
    };
    array_of_structs
    [
        {
            name "bob";
            punch_strength 8.7;
        },
        {
            name "joe";
            punch_strength 7.72;
        },
        {
            name "frankie";
            punch_strength 1.453e8;
        }
    ];

the data file is parsed and stored as a tree structure, with the root node
being a key/value pair with the path/path as the identifier, and the
file's contents as a structure.  therefore, a complete path can be specified
to any individual piece of data in the file (similar to xpath).  arrays can
be indexed using unsigned decimal integers (instead of element names (e.g.
"arrayname|3").  because the root node identifier is a filesystem path and may
contain slashes, a different character will be used to delimit each element
in the datapath.  the datapath delimiting character will be the pipe character
'|'.  the pipe character will therefore not be allowed in paths in this
context.  example path:

    |/usr/data/map.dat|entities|enemies|0|name

this path returns the name of the 0th element of enemies (indices are 0-based),
which is a member of the entities structure, in the file /usr/data/map.dat
data paths should never end in a '|' character.

//////////////////////////////////////////////////////////////////////////////

creating Value ASTs from in-program data:

in a valid data path, all elements before the last in the sequence are
guaranteed to be "container" elements (meaning they can contain other
elements).  these containers are Array and Structure.  a KeyPair can
technically contain another value, but in this context, it merely represents
an element name in the data path.

therefore, writing to any arbitrary data path can be done without needing to
create the necessary container classes leading up to its leaf value.  more
specifically, any arbitrary (valid) path can imply the requirement of a
certain type of container element, and thusly each container along the path
can be created.

examples:

    SetPathElement("|map|name", "Blood Bowl");
    SetPathElement("|map|timelimit", 15);
    SetPathElement("|map|max_players", 8);
    SetPathElement("|map|entities|+|name", "THE ERADICATOR!");
    SetPathElement("|map|entities|$|mass", 175.65);
    SetPathElement("|map|entities|$|position|+", 0.0);
    SetPathElement("|map|entities|$|position|+", 15.0);
    SetPathElement("|map|entities|$|position|+", 0.3);
    SetPathElement("|map|entities|+|name", "Spiny Norman");
    SetPathElement("|map|entities|$|mass", 6.2e15);
    SetPathElement("|map|entities|$|position|+", 124.24);
    SetPathElement("|map|entities|$|position|+", 6547.0);
    SetPathElement("|map|entities|$|position|+", -2240.3);

the resulting AST is:

    map
    {
        name "Blood Bowl";
        timelimit 15;
        max_players 8;
        entities
        [
            {
                name "THE ERADICATOR";
                mass 175.65;
                position [ 0.0, 15.0, 0.3 ];
            },
            {
                name "Spiny Norman";
                mass 6.2e15;
                position [ 124.24, 6547.0, -2240.3 ];
            }
        ];
    };

notice that entities is an array.  the + element indicates that a new array
element should be appended.  the $ element specifies the last (highest index)
element.


//////////////////////////////////////////////////////////////////////////////

random notes:

class hierarchy:

Value
    Boolean
    SignedInteger
    UnsignedInteger
    Floaty
    Character
    String
    KeyPair
    Array
    Structure

class containment:

KeyPair (contains a string for the key)
    Value *

Array
    vector<Value *>

Structure
    map<KeyPair>

more random notes:

SetPathElement on structure
{
    if (at end of path) return ERROR;
    if (at last char of path && last char == '|') return INVALID_PATH;
    ASSERT(ParentElementNodeType() == structure);
    get key string
    if (!IsValidKey(key)) return INVALID_KEY;
    if (key exists in this structure) return matching_key_pair->SetPathElement(value);
    temp_key_pair = new keypair();
    retval = temp_key_pair->SetPathElement(value);
    if (retval != NULL) add temp_key_pair
    return retval;
}

*/

class Boolean;
class SignedInteger;
class UnsignedInteger;
class Floaty;
class Character;
class String;
class Array;
class Structure;

enum ElementType
{
    ET_BOOLEAN = 0,
    ET_SIGNED_INTEGER,
    ET_UNSIGNED_INTEGER,
    ET_FLOATY,
    ET_CHARACTER,
    ET_STRING,
    ET_KEY_PAIR,
    ET_ARRAY,
    ET_STRUCTURE,
    ET_NO_TYPE,

    ET_COUNT
}; // end of enum ElementType

enum NumericSign
{
    NEGATIVE = 0,
    POSITIVE = 1
}; // end of enum NumericSign

std::string const &ElementTypeString (ElementType element_type);

/** A data file is a human-readable text file which functions as a generalized
  * storage medium.  The file is organized up into sets of potentially nested
  * primitives.
  *
  * The primitive types are
  *     <ul>
  *     <li>Boolean</li>
  *     <li>SignedInteger</li>
  *     <li>UnsignedInteger</li>
  *     <li>Floaty</li>
  *     <li>Character</li>
  *     <li>String</li>
  *     <li>Keypair</li>
  *     <li>Array</li>
  *     <li>Structure</li>
  *     </ul>
  *
  * The basic data types (boolean, integer, floating point, character, string)
  * are equivalent to C++ builtin types (where string is stored using an STL
  * basic_string).  The significance of the key/value pair is to give the
  * ability to give a name to a particular value.  The structure is used
  * to store nested key/value pairs (where the values can be anything except
  * key/value pairs).  The array is used to store nested values (where the
  * values can be any of the above).
  *
  * Here is an example of a data file.
  *
  * @code
  * blah - do later
  * @endcode
  *
  * @brief Baseclass of all the DataFile container classes.
  */
class Value
{
public:

    virtual ~Value () { }

    virtual ElementType GetElementType () const = 0;
    Value const *PathElement (std::string const &path) const { return SubpathElement(path, 0); }

    // these methods will throw a string describing the path or type mismatch error
    bool PathElementBoolean (std::string const &path) const throw (std::string);
    Sint32 PathElementSignedInteger (std::string const &path) const throw (std::string);
    Uint32 PathElementUnsignedInteger (std::string const &path) const throw (std::string);
    Float PathElementFloaty (std::string const &path) const throw (std::string);
    char PathElementCharacter (std::string const &path) const throw (std::string);
    std::string const &PathElementString (std::string const &path) const throw (std::string);
    Array const *PathElementArray (std::string const &path) const throw (std::string);
    Structure const *PathElementStructure (std::string const &path) const throw (std::string);

    virtual void Print (IndentFormatter &formatter) const = 0;
    virtual void PrintAST (IndentFormatter &formatter) const = 0;

protected:

    virtual Value const *SubpathElement (
        std::string const &path,
        Uint32 start) const = 0;

    // sort of a kludgey way for these to call SubpathElement
    // on other objects, but then again, fuck it.
    friend class KeyPair;
    friend class Array;
    friend class Structure;
}; // end of class Value

// ///////////////////////////////////////////////////////////////////////////
// LeafValue
// ///////////////////////////////////////////////////////////////////////////

class LeafValue : public Value
{
public:

    LeafValue ()
        :
        Value()
    { }
    virtual ~LeafValue () = 0;

protected:

    virtual Value const *SubpathElement (
        std::string const &path,
        Uint32 start) const;
}; // end of class LeafValue

// ///////////////////////////////////////////////////////////////////////////
// Boolean
// ///////////////////////////////////////////////////////////////////////////

class Boolean : public LeafValue
{
public:

    Boolean (bool value)
        :
        LeafValue(),
        m_value(value)
    { }

    bool Get () const { return m_value; }

    virtual ElementType GetElementType () const { return ET_BOOLEAN; }

    virtual void Print (IndentFormatter &formatter) const
    {
        formatter.BeginLine("%s", BOOL_TO_STRING(m_value));
    }
    virtual void PrintAST (IndentFormatter &formatter) const;

private:

    bool const m_value;
}; // end of class Boolean

// ///////////////////////////////////////////////////////////////////////////
// SignedInteger
// ///////////////////////////////////////////////////////////////////////////

class SignedInteger : public LeafValue
{
public:

    SignedInteger (Sint32 value)
        :
        LeafValue(),
        m_value(value)
    { }

    Sint32 Get () const { return m_value; }

    virtual ElementType GetElementType () const { return ET_SIGNED_INTEGER; }

    virtual void Print (IndentFormatter &formatter) const { formatter.BeginLine("%+d", m_value); }
    virtual void PrintAST (IndentFormatter &formatter) const;

private:

    Sint32 m_value;
}; // end of class SignedInteger

// ///////////////////////////////////////////////////////////////////////////
// UnsignedInteger
// ///////////////////////////////////////////////////////////////////////////

class UnsignedInteger : public LeafValue
{
public:

    UnsignedInteger (Uint32 value)
        :
        LeafValue(),
        m_value(value)
    { }

    Uint32 Get () const { return m_value; }

    virtual ElementType GetElementType () const { return ET_UNSIGNED_INTEGER; }

    virtual void Print (IndentFormatter &formatter) const { formatter.BeginLine("%u", m_value); }
    virtual void PrintAST (IndentFormatter &formatter) const;

private:

    Uint32 m_value;
}; // end of class UnsignedInteger

// ///////////////////////////////////////////////////////////////////////////
// Floaty
// ///////////////////////////////////////////////////////////////////////////

class Floaty : public LeafValue
{
public:

    Floaty (Float value)
        :
        LeafValue(),
        m_value(value)
    { }

    Float Get () const { return m_value; }

    virtual ElementType GetElementType () const { return ET_FLOATY; }

    virtual void Print (IndentFormatter &formatter) const
    {
        formatter.BeginLine("%e", m_value);
    }
    virtual void PrintAST (IndentFormatter &formatter) const;

private:

    Float m_value;
}; // end of class Floaty

// ///////////////////////////////////////////////////////////////////////////
// Character
// ///////////////////////////////////////////////////////////////////////////

class Character : public LeafValue
{
public:

    Character (char value)
        :
        LeafValue(),
        m_value(value)
    { }

    char Get () const { return m_value; }

    virtual ElementType GetElementType () const { return ET_CHARACTER; }

    void Escape ();

    virtual void Print (IndentFormatter &formatter) const
    {
        formatter.BeginLine("%s", Util::CharacterLiteral(m_value).c_str());
    }
    virtual void PrintAST (IndentFormatter &formatter) const;

private:

    char m_value;
}; // end of class Character

// ///////////////////////////////////////////////////////////////////////////
// String
// ///////////////////////////////////////////////////////////////////////////

class String : public LeafValue
{
public:

    String ()
        :
        LeafValue()
    { }
    String (std::string const &value)
        :
        LeafValue(),
        m_value(value)
    { }

    std::string const &Get () const { return m_value; }

    void AppendString (std::string const &string) { m_value += string; }
    void AppendCharacter (char const character) { m_value += character; }

    virtual ElementType GetElementType () const { return ET_STRING; }

    virtual void Print (IndentFormatter &formatter) const;
    virtual void PrintAST (IndentFormatter &formatter) const;

private:

    std::string m_value;
}; // end of class String

// ///////////////////////////////////////////////////////////////////////////
// Container
// ///////////////////////////////////////////////////////////////////////////

class Container : public Value
{
public:

    Container ()
        :
        Value()
    { }

    void SetPathElementBoolean (std::string const &path, bool value) throw(std::string);
    void SetPathElementSignedInteger (std::string const &path, Sint32 value) throw(std::string);
    void SetPathElementUnsignedInteger (std::string const &path, Uint32 value) throw(std::string);
    void SetPathElementFloaty (std::string const &path, Float value) throw(std::string);
    void SetPathElementCharacter (std::string const &path, char value) throw(std::string);
    void SetPathElementString (std::string const &path, std::string const &value) throw(std::string);

protected:

    enum NodeType
    {
        NT_LEAF = 0,
        NT_ARRAY,
        NT_STRUCTURE
    }; // end of enum Container::NodeType

    static NodeType ParentElementNodeType (std::string const &path, Uint32 start) throw(std::string);

    virtual void SetSubpathElement (
        std::string const &path,
        Uint32 start,
        LeafValue *value) throw(std::string) = 0;

    friend class KeyPair;
    friend class Array;
    friend class Structure;
}; // end of class Container

// ///////////////////////////////////////////////////////////////////////////
// KeyPair
// ///////////////////////////////////////////////////////////////////////////

class KeyPair : public Container
{
public:

    KeyPair (std::string const &key, Value *value)
        :
        Container(),
        m_key(key),
        m_value(value)
    {
        ASSERT1(m_key.length() > 0);
    }
    virtual ~KeyPair ()
    {
        delete m_value;
    }

    std::string const &GetKey () const { return m_key; }
    Value *GetValue () const { return m_value; }

    virtual ElementType GetElementType () const { return ET_KEY_PAIR; }

    virtual void Print (IndentFormatter &formatter) const;
    virtual void PrintAST (IndentFormatter &formatter) const;

protected:

    virtual Value const *SubpathElement (
        std::string const &path,
        Uint32 start) const;

    virtual void SetSubpathElement (
        std::string const &path,
        Uint32 start,
        LeafValue *value) throw(std::string);

private:

    std::string const m_key;
    Value *m_value;
}; // end of class KeyPair

// ///////////////////////////////////////////////////////////////////////////
// Array
// ///////////////////////////////////////////////////////////////////////////

class Array : public Container
{
public:

    Array ()
        :
        Container()
    { }
    virtual ~Array ();

    bool ShouldBeFormattedInline () const;
    ElementType ArrayElementType () const;
    ElementType UltimateArrayElementType () const;
    Uint32 DimensionCount () const;
    Uint32 ElementCount () const { return m_element_vector.size(); }
    Value *Element (Uint32 index) const { return index < m_element_vector.size() ? m_element_vector[index] : NULL; }
    bool BooleanElement (Uint32 index) const throw (std::string);
    Sint32 SignedIntegerElement (Uint32 index) const throw (std::string);
    Uint32 UnsignedIntegerElement (Uint32 index) const throw (std::string);
    Float FloatyElement (Uint32 index) const throw (std::string);
    char CharacterElement (Uint32 index) const throw (std::string);
    std::string const &StringElement (Uint32 index) const throw (std::string);
    Array const *ArrayElement (Uint32 index) const throw (std::string);
    Structure const *StructureElement (Uint32 index) const throw (std::string);

    void AppendValue (Value *value);

    virtual ElementType GetElementType () const { return ET_ARRAY; }

    virtual void Print (IndentFormatter &formatter) const;
    virtual void PrintAST (IndentFormatter &formatter) const;

protected:

    virtual Value const *SubpathElement (
        std::string const &path,
        Uint32 start) const;

    virtual void SetSubpathElement (
        std::string const &path,
        Uint32 start,
        LeafValue *value) throw(std::string);

private:

    std::string DimensionAndTypeString () const;
    static bool DoesMatchDimensionAndType (Array const *array0, Array const *array1);

    typedef std::vector<Value *> ElementVector;

    ElementVector m_element_vector;
}; // end of class Array

// ///////////////////////////////////////////////////////////////////////////
// Structure
// ///////////////////////////////////////////////////////////////////////////

class Structure : public Container
{
public:

    Structure ()
        :
        Container()
    { }
    virtual ~Structure ();

    Value const *GetValue (std::string const &key) const;

    void AddKeyPair (std::string const &key, Value *value);
    void AddKeyPair (KeyPair *key_value_pair);

    virtual ElementType GetElementType () const { return ET_STRUCTURE; }

    virtual void Print (IndentFormatter &formatter) const;
    virtual void PrintAST (IndentFormatter &formatter) const;

protected:

    virtual Value const *SubpathElement (
        std::string const &path,
        Uint32 start) const;

    virtual void SetSubpathElement (
        std::string const &path,
        Uint32 start,
        LeafValue *value) throw(std::string);

private:

    static bool IsValidKey (std::string const &key);

    typedef std::map<std::string, KeyPair *> MemberMap;

    MemberMap m_member_map;
}; // end of class Structure

} // end of namespace DataFile
} // end of namespace Parse
} // end of namespace Xrb

#endif // !defined(_XRB_DATAFILEVALUE_HPP_)

