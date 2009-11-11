// ///////////////////////////////////////////////////////////////////////////
// xrb_parse_datafile_value.cpp by Victor Dods, created 2005/07/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_parse_datafile_value.hpp"

#include <sstream>

#include "xrb_parse_util.hpp"

#define THROW_STRING(x) \
{ \
    std::ostringstream out; \
    out << x; \
    throw out.str(); \
}

namespace Xrb {
namespace Parse {
namespace DataFile {

std::string const &ElementTypeString (ElementType element_type)
{
    static std::string const s_element_type_string[ET_COUNT] =
    {
        "ET_BOOLEAN",
        "ET_SIGNED_INTEGER",
        "ET_UNSIGNED_INTEGER",
        "ET_FLOATY",
        "ET_CHARACTER",
        "ET_STRING",
        "ET_KEY_PAIR",
        "ET_ARRAY",
        "ET_STRUCTURE",
        "ET_NO_TYPE"
    };

    ASSERT1(element_type < ET_COUNT);
    return s_element_type_string[element_type];
}

// ///////////////////////////////////////////////////////////////////////////
// Value
// ///////////////////////////////////////////////////////////////////////////

bool Value::PathElementBoolean (std::string const &path) const throw (std::string)
{
    Boolean const *value = dynamic_cast<Boolean const *>(PathElement(path));
    if (value == NULL)
        THROW_STRING("element is not a Boolean")
    return value->Get();
}

Sint32 Value::PathElementSignedInteger (std::string const &path) const throw (std::string)
{
    SignedInteger const *value = dynamic_cast<SignedInteger const *>(PathElement(path));
    if (value == NULL)
        THROW_STRING("element is not a SignedInteger")
    return value->Get();
}

Uint32 Value::PathElementUnsignedInteger (std::string const &path) const throw (std::string)
{
    UnsignedInteger const *value = dynamic_cast<UnsignedInteger const *>(PathElement(path));
    if (value == NULL)
        THROW_STRING("element is not a Uint32")
    return value->Get();
}

Float Value::PathElementFloaty (std::string const &path) const throw (std::string)
{
    Floaty const *value = dynamic_cast<Floaty const *>(PathElement(path));
    if (value == NULL)
        THROW_STRING("element is not a Float")
    return value->Get();
}

char Value::PathElementCharacter (std::string const &path) const throw (std::string)
{
    Character const *value = dynamic_cast<Character const *>(PathElement(path));
    if (value == NULL)
        THROW_STRING("element is not a Character")
    return value->Get();
}

std::string const &Value::PathElementString (std::string const &path) const throw (std::string)
{
    String const *value = dynamic_cast<String const *>(PathElement(path));
    if (value == NULL)
        THROW_STRING("element is not a String")
    return value->Get();
}

Array const *Value::PathElementArray (std::string const &path) const throw (std::string)
{
    Array const *value = dynamic_cast<Array const *>(PathElement(path));
    if (value == NULL)
        THROW_STRING("element is not an Array")
    return value;
}

Structure const *Value::PathElementStructure (std::string const &path) const throw (std::string)
{
    Structure const *value = dynamic_cast<Structure const *>(PathElement(path));
    if (value == NULL)
        THROW_STRING("element is not a Structure")
    return value;
}

// ///////////////////////////////////////////////////////////////////////////
// LeafValue
// ///////////////////////////////////////////////////////////////////////////

LeafValue::~LeafValue () { }

Value const *LeafValue::SubpathElement (
    std::string const &path,
    Uint32 const start) const
{
    ASSERT1(start <= path.length());

    if (start < path.length())
        THROW_STRING("error: in path \"" << path << "\" - element type " <<
                     ElementTypeString(GetElementType()) <<
                     " can not have subelements (subpath \"" << &path[start] << "\")")
    return this;
}

// ///////////////////////////////////////////////////////////////////////////
// Boolean
// ///////////////////////////////////////////////////////////////////////////

void Boolean::PrintAST (IndentFormatter &formatter) const
{
    formatter.EndLine("ET_BOOLEAN: %s", BOOL_TO_STRING(m_value));
}

// ///////////////////////////////////////////////////////////////////////////
// SignedInteger
// ///////////////////////////////////////////////////////////////////////////

void SignedInteger::PrintAST (IndentFormatter &formatter) const
{
    formatter.EndLine("ET_SIGNED_INTEGER: %+d", m_value);
}

// ///////////////////////////////////////////////////////////////////////////
// UnsignedInteger
// ///////////////////////////////////////////////////////////////////////////

void UnsignedInteger::PrintAST (IndentFormatter &formatter) const
{
    formatter.EndLine("ET_UNSIGNED_INTEGER: %u", m_value);
}

// ///////////////////////////////////////////////////////////////////////////
// Floaty
// ///////////////////////////////////////////////////////////////////////////

void Floaty::PrintAST (IndentFormatter &formatter) const
{
    formatter.EndLine("ET_FLOATY: %g", m_value);
}

// ///////////////////////////////////////////////////////////////////////////
// Character
// ///////////////////////////////////////////////////////////////////////////

void Character::Escape ()
{
    m_value = EscapedChar(m_value);
}

void Character::PrintAST (IndentFormatter &formatter) const
{
    formatter.EndLine("ET_CHARACTER: '%c'", m_value);
}

// ///////////////////////////////////////////////////////////////////////////
// String
// ///////////////////////////////////////////////////////////////////////////

void String::Print (IndentFormatter &formatter) const
{
    formatter.ContinueLine("%s", Util::StringLiteral(m_value).c_str());
}

void String::PrintAST (IndentFormatter &formatter) const
{
    formatter.EndLine("ET_STRING: %s", Util::StringLiteral(m_value).c_str());
}

// ///////////////////////////////////////////////////////////////////////////
// Container
// ///////////////////////////////////////////////////////////////////////////

void Container::SetPathElementBoolean (std::string const &path, bool const value) throw(std::string)
{
    SetSubpathElement(path, 0, new Boolean(value));
}

void Container::SetPathElementSignedInteger (std::string const &path, Sint32 const value) throw(std::string)
{
    SetSubpathElement(path, 0, new SignedInteger(value));
}

void Container::SetPathElementUnsignedInteger (std::string const &path, Uint32 const value) throw(std::string)
{
    SetSubpathElement(path, 0, new UnsignedInteger(value));
}

void Container::SetPathElementFloaty (std::string const &path, Float const value) throw(std::string)
{
    SetSubpathElement(path, 0, new Floaty(value));
}

void Container::SetPathElementCharacter (std::string const &path, char const value) throw(std::string)
{
    SetSubpathElement(path, 0, new Character(value));
}

void Container::SetPathElementString (std::string const &path, std::string const &value) throw(std::string)
{
    SetSubpathElement(path, 0, new String(value));
}

Container::NodeType Container::ParentElementNodeType (std::string const &path, Uint32 start) throw(std::string)
{
    ASSERT1(start <= path.length());

    if (start >= path.length())
        return NT_LEAF;

    if (path[start] != '|')
        THROW_STRING("invalid subpath \"" << path.c_str() + start << "\" - expected '|' prefix")

    ++start;
    if (start < path.length())
    {
        if (path[start] == '|')
            THROW_STRING("unexpected '|' character starting with subpath \"" << path.c_str() + start << "\"")
        if (path[start] >= '0' && path[start] <= '9')
        {
            do ++start; while (start < path.length() && path[start] >= '0' && path[start] <= '9');
            if (start >= path.length() || path[start] == '|')
                return NT_ARRAY;
            else
                THROW_STRING("invalid array index starting with subpath \"" << path.c_str() + start << "\"")
        }
        else if ((path[start] == '+' || path[start] == '$') &&
                 (start+1 >= path.length() || path[start+1] == '|'))
        {
            return NT_ARRAY;
        }
        else
            return NT_STRUCTURE;
    }
    else
        THROW_STRING("path can not end with '|' character")
}

// ///////////////////////////////////////////////////////////////////////////
// KeyPair
// ///////////////////////////////////////////////////////////////////////////

void KeyPair::Print (IndentFormatter &formatter) const
{
    ASSERT1(m_value != NULL);
    if (m_value->GetElementType() == ET_STRUCTURE)
    {
        formatter.PrintLine("%s\n{", m_key.c_str());
        formatter.Indent();
        m_value->Print(formatter);
        formatter.Unindent();
        formatter.BeginLine("}");
    }
    else if (m_value->GetElementType() == ET_ARRAY)
    {
        Array const *array = DStaticCast<Array const *>(m_value);
        ASSERT1(array != NULL);
        if (array->ShouldBeFormattedInline())
            formatter.BeginLine("%s ", m_key.c_str());
        else
            formatter.PrintLine("%s", m_key.c_str());
        m_value->Print(formatter);
    }
    else
    {
        formatter.BeginLine("%s ", m_key.c_str());
        m_value->Print(formatter);
    }
}

void KeyPair::PrintAST (IndentFormatter &formatter) const
{
    formatter.EndLine("ET_KEY_PAIR");
    formatter.Indent();
    formatter.EndLine("key  : %s", m_key.c_str());
    formatter.BeginLine("value: ");
    ASSERT1(m_value != NULL);
    m_value->PrintAST(formatter);
    formatter.Unindent();
}

Value const *KeyPair::SubpathElement (
    std::string const &path,
    Uint32 const start) const
{
    ASSERT1(start <= path.length());

//     fprintf(stderr, "KeyPair::SubpathElement(\"%s\");\n", path.c_str()+start);

    if (start >= path.length())
        return GetValue();

    if (path[start] != '|')
        THROW_STRING("invalid subpath \"" << &path[start] << "\" - expected '|' prefix")

    return GetValue()->SubpathElement(path, start);
}

void KeyPair::SetSubpathElement (
    std::string const &path,
    Uint32 const start,
    LeafValue *const value) throw(std::string)
{
    ASSERT1(start <= path.length());
    ASSERT1(value != NULL);

    switch (ParentElementNodeType(path, start))
    {
        case NT_LEAF:
            if (m_value != NULL)
                Delete(m_value);
            m_value = value;
            return;

        case NT_ARRAY:
            // if the key pair's value is an array, call SetSubpathElement on it.
            if (m_value != NULL && m_value->GetElementType() == ET_ARRAY)
                return DStaticCast<Container *>(m_value)->SetSubpathElement(path, start, value);
            // otherwise, attempt to replace it, but only replace if SetSubpathElement succeeds.
            else
            {
                // create an array to replace m_value
                Value *array = new Array();
                try {
                    static_cast<Container *>(array)->SetSubpathElement(path, start, value);
                } catch (std::string const &exception) {
                    // if the call failed, delete the just-created array.
                    Delete(array);
                    throw exception;
                }
                // otherwise delete the existing value and replace it
                // with the just-created array
                Delete(m_value);
                m_value = array;
                return;
            }

        case NT_STRUCTURE:
            // if the key pair's value is an structure, call SetSubpathElement on it.
            if (m_value != NULL && m_value->GetElementType() == ET_STRUCTURE)
                return DStaticCast<Container *>(m_value)->SetSubpathElement(path, start, value);
            // otherwise, attempt to replace it, but only replace if SetSubpathElement succeeds.
            else
            {
                // create an structure to replace m_value
                Value *structure = new Structure();
                try {
                    static_cast<Container *>(structure)->SetSubpathElement(path, start, value);
                } catch (std::string const &exception) {
                    // if the call failed, delete the just-created structure
                    Delete(structure);
                    throw exception;
                }
                // otherwise delete the existing value and replace it
                // with the just-created structure
                Delete(m_value);
                m_value = structure;
                return;
            }
    }
}

// ///////////////////////////////////////////////////////////////////////////
// Array
// ///////////////////////////////////////////////////////////////////////////

Array::~Array ()
{
    for (ElementVector::iterator it = m_element_vector.begin(),
                                 it_end = m_element_vector.end();
         it != it_end;
         ++it)
    {
        Value const *value = *it;
        ASSERT1(value != NULL);
        Delete(value);
    }
}

bool Array::ShouldBeFormattedInline () const
{
    ElementVector::const_iterator it = m_element_vector.begin();
    // arrays with elements that aren't arrays or structures
    // should not be inlined
    if (it != m_element_vector.end())
    {
        Value const *value = *it;
        ASSERT1(value != NULL);
        ASSERT1(value->GetElementType() != ET_KEY_PAIR);
        return value->GetElementType() != ET_ARRAY && value->GetElementType() != ET_STRUCTURE;
    }
    else
        // empty arrays should be inlined.
        return true;
}

ElementType Array::ArrayElementType () const
{
    return m_element_vector.empty() ? ET_NO_TYPE : m_element_vector[0]->GetElementType();
}

ElementType Array::UltimateArrayElementType () const
{
    if (m_element_vector.empty())
        return ET_NO_TYPE;

    if (m_element_vector[0]->GetElementType() == ET_ARRAY)
        return DStaticCast<Array const *>(m_element_vector[0])->UltimateArrayElementType();

    return m_element_vector[0]->GetElementType();
}

Uint32 Array::DimensionCount () const
{
    if (ArrayElementType() == ET_ARRAY)
        return 1 + DStaticCast<Array const *>(m_element_vector[0])->DimensionCount();
    else
        return 1;
}

bool Array::BooleanElement (Uint32 index) const throw (std::string)
{
    Boolean const *value = dynamic_cast<Boolean const *>(Element(index));
    if (value == NULL)
        THROW_STRING("element " << index << " is not a Boolean")
    return value->Get();
}

Sint32 Array::SignedIntegerElement (Uint32 index) const throw (std::string)
{
    SignedInteger const *value = dynamic_cast<SignedInteger const *>(Element(index));
    if (value == NULL)
        THROW_STRING("element " << index << " is not a SignedInteger")
    return value->Get();
}

Uint32 Array::UnsignedIntegerElement (Uint32 index) const throw (std::string)
{
    UnsignedInteger const *value = dynamic_cast<UnsignedInteger const *>(Element(index));
    if (value == NULL)
        THROW_STRING("element " << index << " is not a Uint32")
    return value->Get();
}

Float Array::FloatyElement (Uint32 index) const throw (std::string)
{
    Floaty const *value = dynamic_cast<Floaty const *>(Element(index));
    if (value == NULL)
        THROW_STRING("element " << index << " is not a Float")
    return value->Get();
}

char Array::CharacterElement (Uint32 index) const throw (std::string)
{
    Character const *value = dynamic_cast<Character const *>(Element(index));
    if (value == NULL)
        THROW_STRING("element " << index << " is not a Character")
    return value->Get();
}

std::string const &Array::StringElement (Uint32 index) const throw (std::string)
{
    String const *value = dynamic_cast<String const *>(Element(index));
    if (value == NULL)
        THROW_STRING("element " << index << " is not a String")
    return value->Get();
}

Array const *Array::ArrayElement (Uint32 index) const throw (std::string)
{
    Array const *value = dynamic_cast<Array const *>(Element(index));
    if (value == NULL)
        THROW_STRING("element " << index << " is not an Array")
    return value;
}

Structure const *Array::StructureElement (Uint32 index) const throw (std::string)
{
    Structure const *value = dynamic_cast<Structure const *>(Element(index));
    if (value == NULL)
        THROW_STRING("element " << index << " is not a Structure")
    return value;
}

void Array::AppendValue (Value *const value)
{
    ASSERT1(value != NULL);

    // make sure that the value being appended is the same type
    // as the first value in the array, or the array is empty.
    ElementVector::iterator it = m_element_vector.begin();
    if (it != m_element_vector.end())
    {
        Value const *first_element_value = *it;
        ASSERT1(first_element_value != NULL);
        if (value->GetElementType() != first_element_value->GetElementType())
            THROW_STRING("cannot add a " <<
                         ElementTypeString(value->GetElementType()) <<
                         " to an array with element type " <<
                         ElementTypeString(first_element_value->GetElementType()))
        else if (value->GetElementType() == ET_ARRAY &&
                    !DoesMatchDimensionAndType(
                        DStaticCast<Array const *>(value),
                        DStaticCast<Array const *>(first_element_value)))
            THROW_STRING("sibling elements in nested arrays must be of identical dimension and type")
    }

    // add the new value onto the end of the array.
    m_element_vector.push_back(value);
}

void Array::Print (IndentFormatter &formatter) const
{
    bool inlined_array = ShouldBeFormattedInline();

    if (inlined_array)
        formatter.BeginLine("[ ");
    else
    {
        formatter.PrintLine("[");
        formatter.Indent();
    }

    ElementVector::const_iterator it_test;
    for (ElementVector::const_iterator it = m_element_vector.begin(),
                                       it_end = m_element_vector.end();
         it != it_end;
         ++it)
    {
        Value const *value = *it;
        ASSERT1(value != NULL);
        ASSERT1(value->GetElementType() != ET_KEY_PAIR);

        if (value->GetElementType() == ET_STRUCTURE)
        {
            ASSERT1(!inlined_array);
            formatter.PrintLine("{");
            formatter.Indent();
        }

        value->Print(formatter);

        if (value->GetElementType() == ET_STRUCTURE)
        {
            ASSERT1(!inlined_array);
            formatter.Unindent();
            formatter.BeginLine("}");
        }

        it_test = it;
        ++it_test;
        if (inlined_array)
        {
            if (it_test != it_end)
                formatter.ContinueLine(", ");
            else
                formatter.ContinueLine(" ");
        }
        else
        {
            if (it_test != it_end)
                formatter.EndLine(", ");
            else
                formatter.EndLine("");
        }
    }

    if (inlined_array)
        formatter.ContinueLine("]");
    else
    {
        formatter.Unindent();
        formatter.BeginLine("]");
    }
}

void Array::PrintAST (IndentFormatter &formatter) const
{
    formatter.EndLine(
        "ET_ARRAY: %u dimensions - %u element(s) of type %s",
        DimensionCount(),
        m_element_vector.size(),
        ElementTypeString(ArrayElementType()).c_str());
    formatter.Indent();
    for (Uint32 i = 0; i < m_element_vector.size(); ++i)
    {
        formatter.BeginLine("[%3u]: ", i);
        ASSERT1(m_element_vector[i] != NULL);
        m_element_vector[i]->PrintAST(formatter);
    }
    formatter.Unindent();
}

Value const *Array::SubpathElement (
    std::string const &path,
    Uint32 start) const
{
    ASSERT1(start <= path.length());

//     fprintf(stderr, "Array::SubpathElement(\"%s\");\n", path.c_str()+start);

    if (start >= path.length())
        return this;

    if (path[start] != '|')
        THROW_STRING("invalid subpath \"" << &path[start] << "\" - expected '|' prefix")

    ++start;
    Uint32 key_delim = Min(path.length(), path.find_first_of("|", start));

    if (start == key_delim)
        THROW_STRING("missing array index")

    Uint32 array_index = 0;
    for (Uint32 i = start; i < key_delim; ++i)
    {
        if (path[i] >= '0' && path[i] <= '9')
            array_index = 10 * array_index + path[i] - '0';
        else
            THROW_STRING("invalid array index \"" << path.substr(start, key_delim-start) << "\"")
    }

    if (array_index >= m_element_vector.size())
        THROW_STRING("out of bounds array index \"" << path.substr(start, key_delim-start) << "\"")

    ASSERT1(key_delim < UINT32_UPPER_BOUND);
    return m_element_vector[array_index]->SubpathElement(path, key_delim);
}

void Array::SetSubpathElement (
    std::string const &path,
    Uint32 start,
    LeafValue *const value) throw(std::string)
{
    ASSERT1(start <= path.length());
    ASSERT1(value != NULL);

    if (start >= path.length())
        THROW_STRING("can't assign a value to an array itself - subpath \"" << path.c_str() + start << "\"")

    if (path[start] != '|')
        THROW_STRING("invalid subpath \"" << path.c_str() + start << "\" - expected '|' prefix")

    ASSERT1(ParentElementNodeType(path, start) == NT_ARRAY);

    ++start;
    Uint32 key_delim = Min(path.length(), path.find_first_of("|", start));
    NodeType element_type = ParentElementNodeType(path, key_delim);

    // determine what array index is being referenced
    Uint32 array_index = UINT32_UPPER_BOUND;
    bool create_new_element = false;
    if (key_delim-start == 1 && (path[start] == '+' || path[start] == '$'))
    {
        if (path[start] == '+')
            create_new_element = true;
        else if (path[start] == '$')
        {
            if (m_element_vector.empty())
                THROW_STRING("$ can not be used on array with no elements - subpath \"" << path.c_str() + start << "\"")
            array_index = m_element_vector.size()-1;
        }
        else
            ASSERT1(false && "this should never happen");
    }
    // check for array index
    else
    {
        Uint32 i = start;
        array_index = 0;
        char c = path[i];
        while (i < key_delim && (c = path[i], c >= '0' && c <= '9'))
        {
            // check for overflow
            if (array_index > 429496729 || (array_index == 429496729 && c > '5'))
            {
                array_index = UINT32_UPPER_BOUND;
                break;
            }
            else
                array_index = 10 * array_index + (c - '0');
            ++i;
        }
        if (c < '0' || c > '9')
            THROW_STRING("invalid array index in subpath \"" << path.c_str() + start << "\"")
    }

    // this is pedantic, but hey.
    ASSERT1(m_element_vector.size() < UINT32_UPPER_BOUND);

    if (create_new_element)
    {
        Value *element;
        switch (element_type)
        {
            case NT_LEAF:
                element = value;
                break;

            case NT_ARRAY:
                element = new Array();
                try {
                    static_cast<Container *>(element)->SetSubpathElement(path, key_delim, value);
                } catch (std::string const &exception) {
                    Delete(element);
                    throw exception;
                }
                break;

            case NT_STRUCTURE:
                element = new Structure();
                try {
                    static_cast<Container *>(element)->SetSubpathElement(path, key_delim, value);
                } catch (std::string const &exception) {
                    Delete(element);
                    throw exception;
                }
                break;
        }

        // if there are existing elements, check type and dimension
        if (!m_element_vector.empty())
        {
            Value const *first_element = m_element_vector[0];
            ASSERT1(first_element != NULL);
            if (element->GetElementType() != first_element->GetElementType())
            {
                std::ostringstream out;
                out << "mismatch: array element type " << ElementTypeString(first_element->GetElementType())
                    << ", assignment type " << ElementTypeString(element->GetElementType());
                Delete(element);
                throw out.str();
            }
            else if (element->GetElementType() == ET_ARRAY &&
                    (DStaticCast<Array const *>(first_element)->DimensionCount() !=
                     DStaticCast<Array const *>(element)->DimensionCount()
                     ||
                     static_cast<Array const *>(first_element)->UltimateArrayElementType() !=
                     static_cast<Array const *>(element)->UltimateArrayElementType()))
            {
                std::ostringstream out;
                out << "mismatch: array depth " << static_cast<Array const *>(first_element)->DimensionCount()
                    << "/type " << ElementTypeString(static_cast<Array const *>(first_element)->UltimateArrayElementType())
                    << ", assignment depth " << static_cast<Array const *>(element)->DimensionCount()
                    << "/type " << ElementTypeString(static_cast<Array const *>(element)->UltimateArrayElementType());
                Delete(element);
                throw out.str();
            }
        }

        m_element_vector.push_back(element);
        return;
    }
    else if (array_index >= m_element_vector.size())
        THROW_STRING("array index " << array_index << " out of bounds in subpath \"" << path.c_str() + start << "\"")
    else
    {
        ASSERT1(element_type == NT_LEAF || element_type == NT_ARRAY || element_type == NT_STRUCTURE);
        ASSERT1(!m_element_vector.empty());
        ASSERT1(m_element_vector[array_index] != NULL);
        Value *&element = m_element_vector[array_index];

        if ((element_type == NT_LEAF && value->GetElementType() != element->GetElementType())
            ||
            (element_type == NT_ARRAY && element->GetElementType() != ET_ARRAY)
            ||
            (element_type == NT_STRUCTURE && element->GetElementType() != ET_STRUCTURE))
        {
            ElementType assignment_type = ET_NO_TYPE;
            if (element_type == NT_LEAF)
                assignment_type = value->GetElementType();
            else if (element_type == NT_ARRAY)
                assignment_type = ET_ARRAY;
            else if (element_type == NT_STRUCTURE)
                assignment_type = ET_STRUCTURE;

            ASSERT1(assignment_type != ET_NO_TYPE);
            THROW_STRING("mismatch: array element type " << ElementTypeString(element->GetElementType()) << ", assignment type " << ElementTypeString(assignment_type))
        }

        if (element_type == NT_LEAF)
        {
            Delete(element);
            element = value;
            return;
        }
        else if (element_type == NT_ARRAY || element_type == NT_STRUCTURE)
        {
            return DStaticCast<Container *>(element)->SetSubpathElement(path, key_delim, value);
        }
        else
        {
            ASSERT1(false && "this should never happen");
            return;
        }
    }
}

std::string Array::DimensionAndTypeString () const
{
    std::ostringstream out;

    out << ElementTypeString(UltimateArrayElementType());

    Value const *value = this;
    while (value != NULL && value->GetElementType() == ET_ARRAY)
    {
        out << '[' << DStaticCast<Array const *>(value)->m_element_vector.size() << ']';
        if (DStaticCast<Array const *>(value)->m_element_vector.empty())
            value = NULL;
        else
            value = DStaticCast<Array const *>(value)->m_element_vector[0];
    }

    return out.str();
}

bool Array::DoesMatchDimensionAndType (
    Array const *const array0,
    Array const *const array1)
{
    ASSERT1(array0 != NULL);
    ASSERT1(array1 != NULL);

    if (array0->m_element_vector.size() != array1->m_element_vector.size())
        return false;

    if (array0->m_element_vector.empty())
        return true;

    if (array0->m_element_vector[0]->GetElementType() != array1->m_element_vector[0]->GetElementType())
        return false;

    if (array0->m_element_vector[0]->GetElementType() == ET_ARRAY)
        return
            DoesMatchDimensionAndType(
                DStaticCast<Array const *>(array0->m_element_vector[0]),
                DStaticCast<Array const *>(array1->m_element_vector[0]));

    return true;
}

// ///////////////////////////////////////////////////////////////////////////
// Structure
// ///////////////////////////////////////////////////////////////////////////

Structure::~Structure ()
{
    for (MemberMap::iterator it = m_member_map.begin(),
                           it_end = m_member_map.end();
         it != it_end;
         ++it)
    {
        KeyPair const *key_pair = it->second;
        ASSERT1(key_pair != NULL);
        Delete(key_pair);
    }
}

Value const *Structure::GetValue (std::string const &key) const
{
    ASSERT1(key.length() > 0);
    MemberMap::const_iterator it = m_member_map.find(key);
    if (it == m_member_map.end())
        return NULL;
    else
        return it->second->GetValue();
}

void Structure::AddKeyPair (std::string const &key, Value *value)
{
    ASSERT1(!key.empty());
    ASSERT1(value != NULL);

    if (!IsValidKey(key))
        THROW_STRING("key \"" << key << "\" contains invalid characters")

    if (m_member_map.find(key) == m_member_map.end())
        m_member_map[key] = new KeyPair(key, value);
    else
        THROW_STRING("collision with key \"" << key << "\"")
}

void Structure::AddKeyPair (KeyPair *const key_pair)
{
    ASSERT1(key_pair != NULL);
    ASSERT1(!key_pair->GetKey().empty());
    ASSERT1(key_pair->GetValue() != NULL);

    if (!IsValidKey(key_pair->GetKey()))
        THROW_STRING("key \"" << key_pair->GetKey() << "\" contains invalid characters")

    if (m_member_map.find(key_pair->GetKey()) == m_member_map.end())
        m_member_map[key_pair->GetKey()] = key_pair;
    else
        THROW_STRING("collision with key \"" << key_pair->GetKey() << "\"")
}

void Structure::Print (IndentFormatter &formatter) const
{
    for (MemberMap::const_iterator it = m_member_map.begin(),
                                it_end = m_member_map.end();
         it != it_end;
         ++it)
    {
        KeyPair const *key_pair = it->second;
        ASSERT1(key_pair != NULL);
        key_pair->Print(formatter);
        formatter.EndLine(";");
    }
}

void Structure::PrintAST (IndentFormatter &formatter) const
{
    formatter.EndLine("ET_STRUCTURE: %u element(s)", m_member_map.size());
    formatter.Indent();
    for (MemberMap::const_iterator it = m_member_map.begin(),
                                it_end = m_member_map.end();
         it != it_end;
         ++it)
    {
        KeyPair const *key_pair = it->second;
        ASSERT1(key_pair != NULL);
        key_pair->PrintAST(formatter);
    }
    formatter.Unindent();
}

Value const *Structure::SubpathElement (
    std::string const &path,
    Uint32 start) const
{
    ASSERT1(start <= path.length());

//     fprintf(stderr, "Structure::SubpathElement(\"%s\");\n", path.c_str()+start);

    if (start >= path.length())
        return this;

    if (path[start] != '|')
        THROW_STRING("invalid subpath \"" << &path[start] << "\" - expected '|' prefix")

    ++start;
    Uint32 key_delim = Min(path.length(), path.find_first_of("|", start));
    std::string key(path.substr(start, key_delim-start));
    MemberMap::const_iterator it = m_member_map.find(key);
    if (it == m_member_map.end())
        THROW_STRING("unmatched element \"" << key << "\"")
    else
    {
        ASSERT1(key_delim < UINT32_UPPER_BOUND);
        return it->second->GetValue()->SubpathElement(path, key_delim);
    }
}

void Structure::SetSubpathElement (
    std::string const &path,
    Uint32 start,
    LeafValue *const value) throw(std::string)
{
    ASSERT1(start <= path.length());
    ASSERT1(value != NULL);

    if (start >= path.length())
        THROW_STRING("can't assign a value to a structure itself")

    if (path[start] != '|')
        THROW_STRING("invalid subpath \"" << path.c_str() + start << "\" - expected '|' prefix")

    ASSERT1(ParentElementNodeType(path, start) == NT_STRUCTURE);

    ++start;
    Uint32 key_delim = Min(path.length(), path.find_first_of("|", start));
    ASSERT1(key_delim < UINT32_UPPER_BOUND);

    std::string key(path.substr(start, key_delim-start));
    if (!IsValidKey(key))
        THROW_STRING("invalid key \"" << key << "\"")

    MemberMap::const_iterator it = m_member_map.find(key);
    if (it == m_member_map.end())
    {
        KeyPair *key_pair = new KeyPair(key, NULL);
        try {
            static_cast<Container *>(key_pair)->SetSubpathElement(path, key_delim, value);
        } catch (std::string const &exception) {
            Delete(key_pair);
            throw exception;
        }
        m_member_map[key] = key_pair;
        return;
    }
    else
        return static_cast<Container *>(it->second)->SetSubpathElement(path, key_delim, value);
}

bool Structure::IsValidKey (std::string const &key)
{
    char c = key[0];
    if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && c != '_')
        return false;

    for (Uint32 i = 1; i < key.length(); ++i)
    {
        c = key[i];
        if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && c != '_' && (c < '0' || c > '9'))
            return false;
    }

    return true;
}

} // end of namespace DataFile
} // end of namespace Parse
} // end of namespace Xrb

