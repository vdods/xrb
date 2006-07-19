// ///////////////////////////////////////////////////////////////////////////
// xrb_datafilevalue.h by Victor Dods, created 2005/07/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_DATAFILEVALUE_H_)
#define _XRB_DATAFILEVALUE_H_

#include "xrb.h"

#include <map>
#include <stdio.h>
#include <string>
#include <vector>

#include "xrb_util.h"
#include "xrb_indentformatter.h"

namespace Xrb
{

/*
temp DataFileValue notes - turn into real doxygen docs later

data types:
    boolean        - true, false
    [un]signed int - binary, octal, decimal or hexidecimal
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
being a key/value pair with the path/filename as the identifier, and the
file's contents as a structure.  therefore, a complete path can be specified
to any individual piece of data in the file (similar to xpath).  arrays can
be indexed using unsigned decimal integers (instead of element names (e.g.
"arrayname|3").  because the root node identifier is a filesystem path and may
contain slashes, a different character will be used to delimit each element
in the datapath.  the datapath delimiting character will be the pipe character
'|'.  the pipe character will therefore not be allowed in filenames in this
context.  example path:

    |/usr/data/map.dat|entities|enemies|0|name

this path returns the name of the 0th element of enemies (indices are 0-based),
which is a member of the entities structure, in the file /usr/data/map.dat
data paths should never end in a '|' character.

//////////////////////////////////////////////////////////////////////////////

creating DataFileValue ASTs from in-program data:

in a valid data path, all elements before the last in the sequence are
guaranteed to be "container" elements (meaning they can contain other
elements).  these containers are DataFileArray and DataFileStructure.  a
DataFileKeyPair can technically contain another value, but in this context,
it merely represents an element name in the data path.

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

DataFileValue
    DataFileBoolean
    DataFileInteger
    DataFileFloat
    DataFileCharacter
    DataFileString
    DataFileKeyPair
    DataFileArray
    DataFileStructure

class containment:

DataFileKeyPair (contains a string for the key)
    DataFileValue *

DataFileArray
    vector<DataFileValue *>

DataFileStructure
    map<DataFileKeyPair>

*/

enum DataFileElementType
{
    DAT_BOOLEAN = 0,
    DAT_INTEGER,
    DAT_FLOAT,
    DAT_CHARACTER,
    DAT_STRING,
    DAT_KEY_PAIR,
    DAT_ARRAY,
    DAT_STRUCTURE,
    DAT_NO_TYPE,

    DAT_COUNT
}; // end of enum DataFileElementType

enum NumericSign
{
    NEGATIVE = 0,
    POSITIVE = 1
}; // end of enum NumericSign

std::string const &GetDataFileElementTypeString (DataFileElementType data_file_element_type);

/** A data file is a human-readable text file which functions as a generalized
  * storage medium.  The file is organized up into sets of potentially nested
  * primitives.
  *
  * The primitive types are
  *     <ul>
  *     <li>Boolean</li>
  *     <li>Integer (signed and unsigned)</li>
  *     <li>Floating-point value</li>
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
  * See xrb_datafilescanner.lpp and xrb_datafileparser.ypp for technical
  * details.
  *
  * @brief Baseclass of all the DataFile container classes.
  */
class DataFileValue
{
public:

    virtual ~DataFileValue () { }

    virtual DataFileElementType GetElementType () const = 0;

    virtual void Print (IndentFormatter &formatter) const = 0;
    virtual void PrintAST (IndentFormatter &formatter) const = 0;

protected:

    DataFileValue () { }

    virtual DataFileValue const *GetPathElement (
        std::string const &path,
        Uint32 start) const = 0;

    // sort of a kludgey way for these to call GetPathElement
    // on other objects, but then again, fuck it.
    friend class DataFileKeyPair;
    friend class DataFileArray;
    friend class DataFileStructure;
}; // end of class DataFileValue

// ///////////////////////////////////////////////////////////////////////////
// DataFileLeafValue
// ///////////////////////////////////////////////////////////////////////////

class DataFileLeafValue : public DataFileValue
{
public:

    DataFileLeafValue ()
        :
        DataFileValue()
    { }
    virtual ~DataFileLeafValue () = 0;

protected:

    virtual DataFileValue const *GetPathElement (
        std::string const &path,
        Uint32 start) const;
}; // end of class DataFileLeafValue

// ///////////////////////////////////////////////////////////////////////////
// DataFileBoolean
// ///////////////////////////////////////////////////////////////////////////

class DataFileBoolean : public DataFileLeafValue
{
public:

    DataFileBoolean (bool value)
        :
        DataFileLeafValue(),
        m_value(value)
    { }

    inline bool GetValue () const { return m_value; }

    virtual DataFileElementType GetElementType () const { return DAT_BOOLEAN; }

    virtual void Print (IndentFormatter &formatter) const
    {
        formatter.BeginLine("%s", BOOL_TO_STRING(m_value));
    }
    virtual void PrintAST (IndentFormatter &formatter) const;

private:

    bool const m_value;
}; // end of class DataFileBoolean

// ///////////////////////////////////////////////////////////////////////////
// DataFileInteger
// ///////////////////////////////////////////////////////////////////////////

class DataFileInteger : public DataFileLeafValue
{
public:

    DataFileInteger (Uint32 value)
        :
        DataFileLeafValue(),
        m_value(value),
        m_is_signed(false)
    { }
    DataFileInteger (Sint32 value)
        :
        DataFileLeafValue(),
        m_value(value),
        m_is_signed(true)
    { }

    inline bool GetIsSigned () const { return m_is_signed; }
    inline Sint32 GetSignedValue () const { return static_cast<Sint32>(m_value); }
    inline Uint32 GetUnsignedValue () const { return m_value; }

    virtual DataFileElementType GetElementType () const { return DAT_INTEGER; }

    void Sign (NumericSign sign);

    virtual void Print (IndentFormatter &formatter) const
    {
        if (m_is_signed)
            formatter.BeginLine("%+d", static_cast<Sint32>(m_value));
        else
            formatter.BeginLine("%u", m_value);
    }
    virtual void PrintAST (IndentFormatter &formatter) const;

private:

    Uint32 m_value;
    bool m_is_signed;
}; // end of class DataFileInteger

// ///////////////////////////////////////////////////////////////////////////
// DataFileFloat
// ///////////////////////////////////////////////////////////////////////////

class DataFileFloat : public DataFileLeafValue
{
public:

    DataFileFloat (Float value)
        :
        DataFileLeafValue(),
        m_value(value)
    { }

    inline Float GetValue () const { return m_value; }

    virtual DataFileElementType GetElementType () const { return DAT_FLOAT; }

    void Sign (NumericSign sign);

    virtual void Print (IndentFormatter &formatter) const
    {
        formatter.BeginLine("%e", m_value);
    }
    virtual void PrintAST (IndentFormatter &formatter) const;

private:

    Float m_value;
}; // end of class DataFileFloat

// ///////////////////////////////////////////////////////////////////////////
// DataFileCharacter
// ///////////////////////////////////////////////////////////////////////////

class DataFileCharacter : public DataFileLeafValue
{
public:

    DataFileCharacter (char value)
        :
        DataFileLeafValue(),
        m_value(value)
    { }

    inline char GetValue () const { return m_value; }

    virtual DataFileElementType GetElementType () const { return DAT_CHARACTER; }

    virtual void Print (IndentFormatter &formatter) const
    {
        formatter.BeginLine("%s", Util::GetCharacterLiteral(m_value).c_str());
    }
    virtual void PrintAST (IndentFormatter &formatter) const;

private:

    char const m_value;
}; // end of class DataFileCharacter

// ///////////////////////////////////////////////////////////////////////////
// DataFileString
// ///////////////////////////////////////////////////////////////////////////

class DataFileString : public DataFileLeafValue
{
public:

    DataFileString ()
        :
        DataFileLeafValue()
    { }
    DataFileString (std::string const &value)
        :
        DataFileLeafValue(),
        m_value(value)
    { }

    inline std::string const &GetValue () const { return m_value; }

    inline void AppendString (std::string const &string) { m_value += string; }
    inline void AppendCharacter (char const character) { m_value += character; }

    virtual DataFileElementType GetElementType () const { return DAT_STRING; }

    virtual void Print (IndentFormatter &formatter) const;
    virtual void PrintAST (IndentFormatter &formatter) const;

private:

    std::string m_value;
}; // end of class DataFileString

// ///////////////////////////////////////////////////////////////////////////
// DataFileKeyPair
// ///////////////////////////////////////////////////////////////////////////

class DataFileKeyPair : public DataFileValue
{
public:

    DataFileKeyPair (std::string const &key, DataFileValue const *value)
        :
        DataFileValue (),
        m_key(key),
        m_value(value)
    {
        ASSERT1(m_key.length() > 0)
    }
    virtual ~DataFileKeyPair ()
    {
        delete m_value;
    }

    inline std::string const &GetKey () const { return m_key; }
    inline DataFileValue const *GetValue () const
    {
        ASSERT1(m_value != NULL && "you shouldn't call this after StealValue()")
        return m_value;
    }

    inline DataFileValue const *StealValue ()
    {
        DataFileValue const *value = m_value;
        m_value = NULL;
        return value;
    }

    virtual DataFileElementType GetElementType () const { return DAT_KEY_PAIR; }
    inline DataFileValue const *GetPathElement (std::string const &path) const { return GetPathElement(path, 0); }

    virtual void Print (IndentFormatter &formatter) const;
    virtual void PrintAST (IndentFormatter &formatter) const;

protected:

    virtual DataFileValue const *GetPathElement (
        std::string const &path,
        Uint32 start) const;

private:

    std::string const m_key;
    DataFileValue const *m_value;

    // sort of a kludgey way to call GetPathElement
    // on keypairs, but then again, fuck it.
    friend class DataFileStructure;
}; // end of class DataFileKeyPair

// ///////////////////////////////////////////////////////////////////////////
// DataFileArray
// ///////////////////////////////////////////////////////////////////////////

class DataFileArray : public DataFileValue
{
public:

    DataFileArray ()
        :
        DataFileValue()
    { }
    virtual ~DataFileArray ();

    bool GetShouldBeFormattedInline () const;
    DataFileElementType GetArrayElementType () const;
    DataFileElementType GetUltimateArrayElementType () const;
    Uint32 GetDimensionCount () const;
    inline Uint32 GetElementCount () const { return m_element_vector.size(); }
    inline DataFileValue const *GetElement (Uint32 index) const
    {
        return index < m_element_vector.size() ? m_element_vector[index] : NULL;
    }

    void AppendValue (DataFileValue const *value);

    virtual DataFileElementType GetElementType () const { return DAT_ARRAY; }

    virtual void Print (IndentFormatter &formatter) const;
    virtual void PrintAST (IndentFormatter &formatter) const;

protected:

    virtual DataFileValue const *GetPathElement (
        std::string const &path,
        Uint32 start) const;

private:

    static bool GetDoesMatchDimensionAndType (DataFileArray const *array0, DataFileArray const *array1);

    typedef std::vector<DataFileValue const *> ElementVector;
    typedef ElementVector::iterator ElementVectorIterator;
    typedef ElementVector::const_iterator ElementVectorConstIterator;

    ElementVector m_element_vector;
}; // end of class DataFileArray

// ///////////////////////////////////////////////////////////////////////////
// DataFileStructure
// ///////////////////////////////////////////////////////////////////////////

class DataFileStructure : public DataFileValue
{
public:

    DataFileStructure ()
        :
        DataFileValue()
    { }
    virtual ~DataFileStructure ();

    DataFileValue const *GetValue (std::string const &key) const;
    inline DataFileValue const *GetPathElement (std::string const &path) const { return GetPathElement(path, 0); }

    void AddKeyPair (DataFileKeyPair *key_value_pair);

    virtual DataFileElementType GetElementType () const { return DAT_STRUCTURE; }

    virtual void Print (IndentFormatter &formatter) const;
    virtual void PrintAST (IndentFormatter &formatter) const;

protected:

    virtual DataFileValue const *GetPathElement (
        std::string const &path,
        Uint32 start) const;

private:

    typedef std::map<std::string, DataFileKeyPair const *> MemberMap;
    typedef MemberMap::iterator MemberMapIterator;
    typedef MemberMap::const_iterator MemberMapConstIterator;

    MemberMap m_member_map;
}; // end of class DataFileStructure

} // end of namespace Xrb

#endif // !defined(_XRB_DATAFILEVALUE_H_)

