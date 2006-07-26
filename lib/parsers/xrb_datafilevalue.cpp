// ///////////////////////////////////////////////////////////////////////////
// xrb_datafilevalue.cpp by Victor Dods, created 2005/07/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_datafilevalue.h"

#include <sstream>

namespace Xrb
{

std::string const &GetDataFileElementTypeString (DataFileElementType data_file_element_type)
{
    static std::string const s_data_file_element_type_string[DAT_COUNT] =
    {
        "DAT_BOOLEAN",
        "DAT_SINT32",
        "DAT_UINT32",
        "DAT_FLOAT",
        "DAT_CHARACTER",
        "DAT_STRING",
        "DAT_KEY_PAIR",
        "DAT_ARRAY",
        "DAT_STRUCTURE",
        "DAT_NO_TYPE"
    };

    ASSERT1(data_file_element_type < DAT_COUNT)
    return s_data_file_element_type_string[data_file_element_type];
}

// ///////////////////////////////////////////////////////////////////////////
// DataFileValue
// ///////////////////////////////////////////////////////////////////////////

bool DataFileValue::GetPathElementBoolean (std::string const &path) const
{
    DataFileBoolean const *value = dynamic_cast<DataFileBoolean const *>(GetPathElement(path));
    if (value == NULL)
        throw std::string("path error");
    return value->GetValue();
}

Sint32 DataFileValue::GetPathElementSint32 (std::string const &path) const
{
    DataFileSint32 const *value = dynamic_cast<DataFileSint32 const *>(GetPathElement(path));
    if (value == NULL)
        throw std::string("path error");
    return value->GetValue();
}

Uint32 DataFileValue::GetPathElementUint32 (std::string const &path) const
{
    DataFileUint32 const *value = dynamic_cast<DataFileUint32 const *>(GetPathElement(path));
    if (value == NULL)
        throw std::string("path error");
    return value->GetValue();
}

Float DataFileValue::GetPathElementFloat (std::string const &path) const
{
    DataFileFloat const *value = dynamic_cast<DataFileFloat const *>(GetPathElement(path));
    if (value == NULL)
        throw std::string("path error");
    return value->GetValue();
}

char DataFileValue::GetPathElementCharacter (std::string const &path) const
{
    DataFileCharacter const *value = dynamic_cast<DataFileCharacter const *>(GetPathElement(path));
    if (value == NULL)
        throw std::string("path error");
    return value->GetValue();
}

std::string const &DataFileValue::GetPathElementString (std::string const &path) const
{
    DataFileString const *value = dynamic_cast<DataFileString const *>(GetPathElement(path));
    if (value == NULL)
        throw std::string("path error");
    return value->GetValue();
}

DataFileArray const *DataFileValue::GetPathElementArray (std::string const &path) const
{
    return dynamic_cast<DataFileArray const *>(GetPathElement(path));
}

DataFileStructure const *DataFileValue::GetPathElementStructure (std::string const &path) const
{
    return dynamic_cast<DataFileStructure const *>(GetPathElement(path));
}

// ///////////////////////////////////////////////////////////////////////////
// DataFileLeafValue
// ///////////////////////////////////////////////////////////////////////////

DataFileLeafValue::~DataFileLeafValue () { }

DataFileValue const *DataFileLeafValue::GetSubpathElement (
    std::string const &path,
    Uint32 const start) const
{
    ASSERT1(start <= path.length())

    if (start >= path.length())
        return this;

    fprintf(
        stderr,
        "GetPathElement error: in path \"%s\" - element type %s can not have subelements (subpath \"%s\")\n",
        path.c_str(),
        GetDataFileElementTypeString(GetElementType()).c_str(),
        path.c_str() + start);
    return NULL;
}

// ///////////////////////////////////////////////////////////////////////////
// DataFileBoolean
// ///////////////////////////////////////////////////////////////////////////

void DataFileBoolean::PrintAST (IndentFormatter &formatter) const
{
    formatter.EndLine("DAT_BOOLEAN - %s", BOOL_TO_STRING(m_value));
}

// ///////////////////////////////////////////////////////////////////////////
// DataFileSint32
// ///////////////////////////////////////////////////////////////////////////

void DataFileSint32::PrintAST (IndentFormatter &formatter) const
{
    formatter.EndLine("DAT_SINT32 - %+d", m_value);
}

// ///////////////////////////////////////////////////////////////////////////
// DataFileUint32
// ///////////////////////////////////////////////////////////////////////////

void DataFileUint32::PrintAST (IndentFormatter &formatter) const
{
    formatter.EndLine("DAT_UINT32 - %u", m_value);
}

// ///////////////////////////////////////////////////////////////////////////
// DataFileFloat
// ///////////////////////////////////////////////////////////////////////////

void DataFileFloat::Sign (NumericSign const sign)
{
    ASSERT1(sign == NEGATIVE || sign == POSITIVE)
    if (sign == NEGATIVE)
        m_value = -m_value;
}

void DataFileFloat::PrintAST (IndentFormatter &formatter) const
{
    formatter.EndLine("DAT_FLOAT - %g", m_value);
}

// ///////////////////////////////////////////////////////////////////////////
// DataFileCharacter
// ///////////////////////////////////////////////////////////////////////////

void DataFileCharacter::PrintAST (IndentFormatter &formatter) const
{
    formatter.EndLine("DAT_CHARACTER - '%c'", m_value);
}

// ///////////////////////////////////////////////////////////////////////////
// DataFileString
// ///////////////////////////////////////////////////////////////////////////

void DataFileString::Print (IndentFormatter &formatter) const
{
    formatter.ContinueLine("%s", Util::GetStringLiteral(m_value).c_str());
}

void DataFileString::PrintAST (IndentFormatter &formatter) const
{
    formatter.EndLine("DAT_STRING - %s", Util::GetStringLiteral(m_value).c_str());
}

// ///////////////////////////////////////////////////////////////////////////
// DataFileContainer
// ///////////////////////////////////////////////////////////////////////////

bool DataFileContainer::SetPathElementBoolean (std::string const &path, bool const value)
{
    return SetSubpathElement(path, 0, new DataFileBoolean(value));
}

bool DataFileContainer::SetPathElementSint32 (std::string const &path, Sint32 const value)
{
    return SetSubpathElement(path, 0, new DataFileSint32(value));
}

bool DataFileContainer::SetPathElementUint32 (std::string const &path, Uint32 const value)
{
    return SetSubpathElement(path, 0, new DataFileUint32(value));
}

bool DataFileContainer::SetPathElementFloat (std::string const &path, Float const value)
{
    return SetSubpathElement(path, 0, new DataFileFloat(value));
}

bool DataFileContainer::SetPathElementCharacter (std::string const &path, char const value)
{
    return SetSubpathElement(path, 0, new DataFileCharacter(value));
}

bool DataFileContainer::SetPathElementString (std::string const &path, std::string const &value)
{
    return SetSubpathElement(path, 0, new DataFileString(value));
}

DataFileContainer::NodeType DataFileContainer::GetParentElementNodeType (std::string const &path, Uint32 start) const
{
    ASSERT1(start <= path.length())

    if (start >= path.length())
        return NT_LEAF;

    if (path[start] != '|')
    {
        fprintf(
            stderr,
            "SetPathElement error: in path \"%s\" - invalid subpath \"%s\" - expected '|' prefix\n",
            path.c_str(),
            path.c_str() + start);
        return NT_PATH_ERROR;
    }

    ++start;
    if (start < path.length())
    {
        if (path[start] == '|')
            return NT_PATH_ERROR;
        if (path[start] >= '0' && path[start] <= '9')
        {
            do ++start; while (start < path.length() && path[start] >= '0' && path[start] <= '9');
            if (start >= path.length() || path[start] == '|')
                return NT_ARRAY;
            else
            {
                fprintf(
                    stderr,
                    "SetPathElement error: in path \"%s\" - invalid array index\n",
                    path.c_str());
                return NT_PATH_ERROR;
            }
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
    {
        fprintf(
            stderr,
            "SetPathElement error: in path \"%s\" - path can not end with '|' character\n",
            path.c_str());
        return NT_PATH_ERROR;
    }
}

// ///////////////////////////////////////////////////////////////////////////
// DataFileKeyPair
// ///////////////////////////////////////////////////////////////////////////

void DataFileKeyPair::Print (IndentFormatter &formatter) const
{
    ASSERT1(m_value != NULL)
    if (m_value->GetElementType() == DAT_STRUCTURE)
    {
        formatter.PrintLine("%s\n{", m_key.c_str());
        formatter.Indent();
        m_value->Print(formatter);
        formatter.Unindent();
        formatter.BeginLine("}");
    }
    else if (m_value->GetElementType() == DAT_ARRAY)
    {
        DataFileArray const *array = DStaticCast<DataFileArray const *>(m_value);
        ASSERT1(array != NULL)
        if (array->GetShouldBeFormattedInline())
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

void DataFileKeyPair::PrintAST (IndentFormatter &formatter) const
{
    formatter.EndLine("DAT_KEY_PAIR");
    formatter.Indent();
    formatter.EndLine("key  : %s", m_key.c_str());
    formatter.BeginLine("value: ");
    ASSERT1(m_value != NULL)
    m_value->PrintAST(formatter);
    formatter.Unindent();
}

DataFileValue const *DataFileKeyPair::GetSubpathElement (
    std::string const &path,
    Uint32 const start) const
{
    ASSERT1(start <= path.length())

//     fprintf(stderr, "DataFileKeyPair::GetSubpathElement(\"%s\");\n", path.c_str()+start);

    if (start >= path.length())
        return GetValue();

    if (path[start] != '|')
    {
        fprintf(
            stderr,
            "GetPathElement error: in path \"%s\" - invalid subpath \"%s\" - expected '|' prefix\n",
            path.c_str(),
            path.c_str() + start);
        return NULL;
    }

    return GetValue()->GetSubpathElement(path, start);
}

bool DataFileKeyPair::SetSubpathElement (
    std::string const &path,
    Uint32 const start,
    DataFileLeafValue *const value)
{
    ASSERT1(start <= path.length())
    ASSERT1(value != NULL)

    switch (GetParentElementNodeType(path, start))
    {
        case NT_LEAF:
            if (m_value != NULL)
                Delete(m_value);
            m_value = value;
            return true;

        case NT_ARRAY:
            // if the key pair's value is an array, call SetSubpathElement on it.
            if (m_value != NULL && m_value->GetElementType() == DAT_ARRAY)
                return DStaticCast<DataFileContainer *>(m_value)->SetSubpathElement(path, start, value);
            // otherwise, attempt to replace it, but only replace if SetSubpathElement succeeds.
            else
            {
                // create an array to replace m_value
                DataFileValue *array = new DataFileArray();
                // only delete and replace m_value if the SetSubpathElement worked
                if (static_cast<DataFileContainer *>(array)->SetSubpathElement(path, start, value))
                {
                    Delete(m_value);
                    m_value = array;
                    return true;
                }
                // otherwise delete the recently created array
                else
                {
                    Delete(array);
                    return false;
                }
            }
            ASSERT1(false && "this should never happen")
            return false;

        case NT_STRUCTURE:
            // if the key pair's value is an structure, call SetSubpathElement on it.
            if (m_value != NULL && m_value->GetElementType() == DAT_STRUCTURE)
                return DStaticCast<DataFileContainer *>(m_value)->SetSubpathElement(path, start, value);
            // otherwise, attempt to replace it, but only replace if SetSubpathElement succeeds.
            else
            {
                // create an structure to replace m_value
                DataFileValue *structure = new DataFileStructure();
                // only delete and replace m_value if the SetSubpathElement worked
                if (static_cast<DataFileContainer *>(structure)->SetSubpathElement(path, start, value))
                {
                    Delete(m_value);
                    m_value = structure;
                    return true;
                }
                // otherwise delete the recently created structure
                else
                {
                    Delete(structure);
                    return false;
                }
            }
            ASSERT1(false && "this should never happen")
            return false;

        default: return false;
    }
}

// ///////////////////////////////////////////////////////////////////////////
// DataFileArray
// ///////////////////////////////////////////////////////////////////////////

DataFileArray::~DataFileArray ()
{
    for (ElementVectorIterator it = m_element_vector.begin(),
                               it_end = m_element_vector.end();
         it != it_end;
         ++it)
    {
        DataFileValue const *value = *it;
        ASSERT1(value != NULL)
        Delete(value);
    }
}

bool DataFileArray::GetShouldBeFormattedInline () const
{
    ElementVectorConstIterator it = m_element_vector.begin();
    // arrays with elements that aren't arrays or structures
    // should not be inlined
    if (it != m_element_vector.end())
    {
        DataFileValue const *value = *it;
        ASSERT1(value != NULL)
        ASSERT1(value->GetElementType() != DAT_KEY_PAIR)
        return value->GetElementType() != DAT_ARRAY && value->GetElementType() != DAT_STRUCTURE;
    }
    else
        // empty arrays should be inlined.
        return true;
}

DataFileElementType DataFileArray::GetArrayElementType () const
{
    return m_element_vector.empty() ? DAT_NO_TYPE : m_element_vector[0]->GetElementType();
}

DataFileElementType DataFileArray::GetUltimateArrayElementType () const
{
    if (m_element_vector.empty())
        return DAT_NO_TYPE;

    if (m_element_vector[0]->GetElementType() == DAT_ARRAY)
        return DStaticCast<DataFileArray const *>(m_element_vector[0])->GetUltimateArrayElementType();

    return m_element_vector[0]->GetElementType();
}

Uint32 DataFileArray::GetDimensionCount () const
{
    if (GetArrayElementType() == DAT_ARRAY)
        return 1 + DStaticCast<DataFileArray const *>(m_element_vector[0])->GetDimensionCount();
    else
        return 1;
}

void DataFileArray::AppendValue (DataFileValue *const value)
{
    ASSERT1(value != NULL)

    // make sure that the value being appended is the same type
    // as the first value in the array, or the array is empty.
    ElementVectorIterator it = m_element_vector.begin();
    if (it != m_element_vector.end())
    {
        DataFileValue const *first_element_value = *it;
        ASSERT1(first_element_value != NULL)
        if (value->GetElementType() != first_element_value->GetElementType())
        {
            std::ostringstream out;
            out << "cannot add a " << GetDataFileElementTypeString(value->GetElementType())
                << " to an array with element type "
                << GetDataFileElementTypeString(first_element_value->GetElementType());
            throw out.str();
        }
        else if (value->GetElementType() == DAT_ARRAY &&
                    !GetDoesMatchDimensionAndType(
                        DStaticCast<DataFileArray const *>(value),
                        DStaticCast<DataFileArray const *>(first_element_value)))
        {
            throw std::string("sibling elements in nested arrays must be of identical dimension and type");
        }
    }

    // add the new value onto the end of the array.
    m_element_vector.push_back(value);
}

void DataFileArray::Print (IndentFormatter &formatter) const
{
    bool inlined_array = GetShouldBeFormattedInline();

    if (inlined_array)
        formatter.BeginLine("[ ");
    else
    {
        formatter.PrintLine("[");
        formatter.Indent();
    }

    ElementVectorConstIterator it_test;
    for (ElementVectorConstIterator it = m_element_vector.begin(),
                                  it_end = m_element_vector.end();
         it != it_end;
         ++it)
    {
        DataFileValue const *value = *it;
        ASSERT1(value != NULL)
        ASSERT1(value->GetElementType() != DAT_KEY_PAIR)

        if (value->GetElementType() == DAT_STRUCTURE)
        {
            ASSERT1(!inlined_array)
            formatter.PrintLine("{");
            formatter.Indent();
        }

        value->Print(formatter);

        if (value->GetElementType() == DAT_STRUCTURE)
        {
            ASSERT1(!inlined_array)
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

void DataFileArray::PrintAST (IndentFormatter &formatter) const
{
    formatter.EndLine(
        "DAT_ARRAY - %u dimensions - %u element(s) of type %s",
        GetDimensionCount(),
        m_element_vector.size(),
        GetDataFileElementTypeString(GetArrayElementType()).c_str());
    formatter.Indent();
    for (Uint32 i = 0; i < m_element_vector.size(); ++i)
    {
        formatter.BeginLine("[%3u]: ", i);
        ASSERT1(m_element_vector[i] != NULL)
        m_element_vector[i]->PrintAST(formatter);
    }
    formatter.Unindent();
}

DataFileValue const *DataFileArray::GetSubpathElement (
    std::string const &path,
    Uint32 start) const
{
    ASSERT1(start <= path.length())

//     fprintf(stderr, "DataFileArray::GetSubpathElement(\"%s\");\n", path.c_str()+start);

    if (start >= path.length())
        return this;

    if (path[start] != '|')
    {
        fprintf(
            stderr,
            "GetPathElement error: in path \"%s\" - invalid subpath \"%s\" - expected '|' prefix\n",
            path.c_str(),
            path.c_str() + start);
        return NULL;
    }

    ++start;
    Uint32 key_delim = Min(path.length(), static_cast<Uint32>(path.find_first_of("|", start)));

    if (start == key_delim)
    {
        fprintf(
            stderr,
            "GetPathElement error: in path \"%s\" - missing array index\n",
            path.c_str());
        return NULL;
    }

    Uint32 array_index = 0;
    for (Uint32 i = start; i < key_delim; ++i)
    {
        if (path[i] >= '0' && path[i] <= '9')
            array_index = 10 * array_index + path[i] - '0';
        else
        {
            fprintf(
                stderr,
                "GetPathElement error: in path \"%s\" - invalid array index \"%s\"\n",
                path.c_str(),
                path.substr(start, key_delim-start).c_str());
            return NULL;
        }
    }

    if (array_index >= m_element_vector.size())
    {
        fprintf(
            stderr,
            "GetPathElement error: in path \"%s\" - out of bounds array index \"%s\"\n",
            path.c_str(),
            path.substr(start, key_delim-start).c_str());
        return NULL;
    }

    ASSERT1(key_delim < UINT32_UPPER_BOUND)
    return m_element_vector[array_index]->GetSubpathElement(path, key_delim);
}

bool DataFileArray::SetSubpathElement (
    std::string const &path,
    Uint32 start,
    DataFileLeafValue *const value)
{
    ASSERT1(start <= path.length())
    ASSERT1(value != NULL)

    if (start >= path.length())
    {
        fprintf(stderr, "SetPathElement error: in path \"%s\" - can't assign a value to an array itself\n", path.c_str());
        return false;
    }

    if (path[start] != '|')
    {
        fprintf(stderr, "SetPathElement error: in path \"%s\" - invalid subpath \"%s\" - expected '|' prefix\n", path.c_str(), path.c_str() + start);
        return false;
    }

    ASSERT1(GetParentElementNodeType(path, start) == NT_ARRAY)

    ++start;
    Uint32 key_delim = Min(path.length(), static_cast<Uint32>(path.find_first_of("|", start)));
    NodeType element_type = GetParentElementNodeType(path, key_delim);

    // determine what array index is being referenced
    Uint32 array_index = UINT32_UPPER_BOUND;
    bool create_new_element = false;
    if (key_delim-start == 1 && (path[start] == '+' || path[start] == '$'))
    {
        if (path[start] == '+')
        {
            create_new_element = true;
        }
        else if (path[start] == '$')
        {
            if (m_element_vector.empty())
            {
                fprintf(stderr, "SetPathElement error: in path \"%s\" - $ can not be used on array with no elements\n", path.c_str());
                return false;
            }
            array_index = m_element_vector.size()-1;
        }
        else
        {
            ASSERT1(false && "this should never happen")
        }
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
            if (array_index > 429496729 || array_index == 429496729 && c > '5')
            {
                array_index = UINT32_UPPER_BOUND;
                break;
            }
            else
                array_index = 10 * array_index + (c - '0');
            ++i;
        }
        if (c < '0' || c > '9')
        {
            fprintf(stderr, "SetPathElement error: in path \"%s\" - invalid array index in subpath \"%s\"\n", path.c_str(), path.c_str()+start);
            return false;
        }
    }

    // this is pedantic, but hey.
    ASSERT1(m_element_vector.size() < UINT32_UPPER_BOUND)

    if (create_new_element)
    {
        DataFileValue *element;
        switch (element_type)
        {
            case NT_LEAF:
                element = value;
                break;

            case NT_ARRAY:
                element = new DataFileArray();
                if (!static_cast<DataFileContainer *>(element)->SetSubpathElement(path, key_delim, value))
                {
                    Delete(element);
                    return false;
                }
                break;

            case NT_STRUCTURE:
                element = new DataFileStructure();
                if (!static_cast<DataFileContainer *>(element)->SetSubpathElement(path, key_delim, value))
                {
                    Delete(element);
                    return false;
                }
                break;

            default: return false;
        }

        // if there are existing elements, check type and dimension
        if (!m_element_vector.empty())
        {
            DataFileValue const *first_element = m_element_vector[0];
            ASSERT1(first_element != NULL)
            if (element->GetElementType() != first_element->GetElementType())
            {
                fprintf(stderr, "SetPathElement error: in path \"%s\" - mismatch: array element type %s, assignment type %s\n", path.c_str(), GetDataFileElementTypeString(first_element->GetElementType()).c_str(), GetDataFileElementTypeString(element->GetElementType()).c_str());
                Delete(element);
                return false;
            }
            else if (element->GetElementType() == DAT_ARRAY &&
                    (DStaticCast<DataFileArray const *>(first_element)->GetDimensionCount() !=
                    DStaticCast<DataFileArray const *>(element)->GetDimensionCount()
                    ||
                    static_cast<DataFileArray const *>(first_element)->GetUltimateArrayElementType() !=
                    static_cast<DataFileArray const *>(element)->GetUltimateArrayElementType()))
            {
                fprintf(stderr, "SetPathElement error: in path \"%s\" - mismatch: array depth %u/type %s, assignment depth %u/type %s\n", path.c_str(), static_cast<DataFileArray const *>(first_element)->GetDimensionCount(), GetDataFileElementTypeString(static_cast<DataFileArray const *>(first_element)->GetUltimateArrayElementType()).c_str(), static_cast<DataFileArray const *>(element)->GetDimensionCount(), GetDataFileElementTypeString(static_cast<DataFileArray const *>(element)->GetUltimateArrayElementType()).c_str());
                Delete(element);
                return false;
            }
        }

        m_element_vector.push_back(element);
        return true;
    }
    else if (array_index >= m_element_vector.size())
    {
        fprintf(stderr, "SetPathElement error: in path \"%s\" - array index %u out of bounds\n", path.c_str(), array_index);
        return false;
    }
    else
    {
        ASSERT1(element_type == NT_LEAF || element_type == NT_ARRAY || element_type == NT_STRUCTURE)
        ASSERT1(!m_element_vector.empty())
        ASSERT1(m_element_vector[array_index] != NULL)
        DataFileValue *&element = m_element_vector[array_index];

        if (element_type == NT_LEAF && value->GetElementType() != element->GetElementType()
            ||
            element_type == NT_ARRAY && element->GetElementType() != DAT_ARRAY
            ||
            element_type == NT_STRUCTURE && element->GetElementType() != DAT_STRUCTURE)
        {
            DataFileElementType assignment_type = DAT_NO_TYPE;
            if (element_type == NT_LEAF)
                assignment_type = value->GetElementType();
            else if (element_type == NT_ARRAY)
                assignment_type = DAT_ARRAY;
            else if (element_type == NT_STRUCTURE)
                assignment_type = DAT_STRUCTURE;

            ASSERT1(assignment_type != DAT_NO_TYPE)
            fprintf(stderr, "SetPathElement error: in path \"%s\" - mismatch: array element type %s, assignment type %s\n", path.c_str(), GetDataFileElementTypeString(element->GetElementType()).c_str(), GetDataFileElementTypeString(assignment_type).c_str());
            return false;
        }

        if (element_type == NT_LEAF)
        {
            Delete(element);
            element = value;
            return true;
        }
        else if (element_type == NT_ARRAY || element_type == NT_STRUCTURE)
        {
            return DStaticCast<DataFileContainer *>(element)->SetSubpathElement(path, key_delim, value);
        }
        else
        {
            ASSERT1(false && "this should never happen")
            return false;
        }
    }
}

std::string DataFileArray::GetDimensionAndTypeString () const
{
    std::ostringstream out;

    out << GetDataFileElementTypeString(GetUltimateArrayElementType());

    DataFileValue const *value = this;
    while (value != NULL && value->GetElementType() == DAT_ARRAY)
    {
        out << '[' << DStaticCast<DataFileArray const *>(value)->m_element_vector.size() << ']';
        if (DStaticCast<DataFileArray const *>(value)->m_element_vector.empty())
            value = NULL;
        else
            value = DStaticCast<DataFileArray const *>(value)->m_element_vector[0];
    }

    return out.str();
}

bool DataFileArray::GetDoesMatchDimensionAndType (
    DataFileArray const *const array0,
    DataFileArray const *const array1)
{
    ASSERT1(array0 != NULL)
    ASSERT1(array1 != NULL)

    if (array0->m_element_vector.size() != array1->m_element_vector.size())
        return false;

    if (array0->m_element_vector.empty())
        return true;

    if (array0->m_element_vector[0]->GetElementType() != array1->m_element_vector[0]->GetElementType())
        return false;

    if (array0->m_element_vector[0]->GetElementType() == DAT_ARRAY)
        return
            GetDoesMatchDimensionAndType(
                DStaticCast<DataFileArray const *>(array0->m_element_vector[0]),
                DStaticCast<DataFileArray const *>(array1->m_element_vector[0]));

    return true;
}

// ///////////////////////////////////////////////////////////////////////////
// DataFileStructure
// ///////////////////////////////////////////////////////////////////////////

DataFileStructure::~DataFileStructure ()
{
    for (MemberMapIterator it = m_member_map.begin(),
                           it_end = m_member_map.end();
         it != it_end;
         ++it)
    {
        DataFileKeyPair const *key_pair = it->second;
        ASSERT1(key_pair != NULL)
        Delete(key_pair);
    }
}

DataFileValue const *DataFileStructure::GetValue (std::string const &key) const
{
    ASSERT1(key.length() > 0)
    MemberMapConstIterator it = m_member_map.find(key);
    if (it == m_member_map.end())
        return NULL;
    else
        return it->second->GetValue();
}

void DataFileStructure::AddKeyPair (std::string const &key, DataFileValue *value)
{
    ASSERT1(!key.empty())
    ASSERT1(value != NULL)

    if (!GetIsValidKey(key))
    {
        std::ostringstream out;
        out << "key \"" << key << "\" contains invalid characters";
        throw out.str();
    }

    if (m_member_map.find(key) == m_member_map.end())
        m_member_map[key] = new DataFileKeyPair(key, value);
    else
    {
        std::ostringstream out;
        out << "collision with key \"" << key << "\"";
        throw out.str();
    }
}

void DataFileStructure::AddKeyPair (DataFileKeyPair *const key_pair)
{
    ASSERT1(key_pair != NULL)
    ASSERT1(!key_pair->GetKey().empty())
    ASSERT1(key_pair->GetValue() != NULL)

    if (!GetIsValidKey(key_pair->GetKey()))
    {
        std::ostringstream out;
        out << "key \"" << key_pair->GetKey() << "\" contains invalid characters";
        throw out.str();
    }

    if (m_member_map.find(key_pair->GetKey()) == m_member_map.end())
        m_member_map[key_pair->GetKey()] = key_pair;
    else
    {
        std::ostringstream out;
        out << "collision with key \"" << key_pair->GetKey() << "\"";
        throw out.str();
    }
}

void DataFileStructure::Print (IndentFormatter &formatter) const
{
    for (MemberMapConstIterator it = m_member_map.begin(),
                                it_end = m_member_map.end();
         it != it_end;
         ++it)
    {
        DataFileKeyPair const *key_pair = it->second;
        ASSERT1(key_pair != NULL)
        key_pair->Print(formatter);
        formatter.EndLine(";");
    }
}

void DataFileStructure::PrintAST (IndentFormatter &formatter) const
{
    formatter.EndLine("DAT_STRUCTURE - %u element(s)", m_member_map.size());
    formatter.Indent();
    for (MemberMapConstIterator it = m_member_map.begin(),
                                it_end = m_member_map.end();
         it != it_end;
         ++it)
    {
        DataFileKeyPair const *key_pair = it->second;
        ASSERT1(key_pair != NULL)
        key_pair->PrintAST(formatter);
    }
    formatter.Unindent();
}

DataFileValue const *DataFileStructure::GetSubpathElement (
    std::string const &path,
    Uint32 start) const
{
    ASSERT1(start <= path.length())

//     fprintf(stderr, "DataFileStructure::GetSubpathElement(\"%s\");\n", path.c_str()+start);

    if (start >= path.length())
        return this;

    if (path[start] != '|')
    {
        fprintf(stderr, "GetPathElement error: in path \"%s\" - invalid subpath \"%s\" - expected '|' prefix\n", path.c_str(), path.c_str() + start);
        return NULL;
    }

    ++start;
    Uint32 key_delim = Min(path.length(), static_cast<Uint32>(path.find_first_of("|", start)));
    std::string key(path.substr(start, key_delim-start));
    MemberMapConstIterator it = m_member_map.find(key);
    if (it == m_member_map.end())
    {
        fprintf(stderr, "GetPathElement error: in path \"%s\" - unmatched element \"%s\"\n", path.c_str(), key.c_str());
        return NULL;
    }
    else
    {
        ASSERT1(key_delim < UINT32_UPPER_BOUND)
        return it->second->GetValue()->GetSubpathElement(path, key_delim);
    }
}

bool DataFileStructure::SetSubpathElement (
    std::string const &path,
    Uint32 start,
    DataFileLeafValue *const value)
{
    ASSERT1(start <= path.length())
    ASSERT1(value != NULL)

    if (start >= path.length())
    {
        fprintf(stderr, "SetPathElement error: in path \"%s\" - can't assign a value to a structure itself\n", path.c_str());
        return false;
    }

    if (path[start] != '|')
    {
        fprintf(stderr, "SetPathElement error: in path \"%s\" - invalid subpath \"%s\" - expected '|' prefix\n", path.c_str(), path.c_str() + start);
        return false;
    }

    ASSERT1(GetParentElementNodeType(path, start) == NT_STRUCTURE)

    ++start;
    Uint32 key_delim = Min(path.length(), static_cast<Uint32>(path.find_first_of("|", start)));
    ASSERT1(key_delim < UINT32_UPPER_BOUND)

    NodeType element_type = GetParentElementNodeType(path, key_delim);
    if (element_type == NT_PATH_ERROR)
    {
        fprintf(stderr, "SetPathElement error: in path \"%s\" - type mismatch before subpath \"%s\"\n", path.c_str(), path.c_str() + start);
        return false;
    }

    std::string key(path.substr(start, key_delim-start));
    if (!GetIsValidKey(key))
    {
        fprintf(stderr, "SetPathElement error: in path \"%s\" - invalid key \"%s\"\n", path.c_str(), key.c_str());
        return false;
    }

    MemberMapConstIterator it = m_member_map.find(key);
    if (it == m_member_map.end())
    {
        DataFileKeyPair *key_pair = new DataFileKeyPair(key, NULL);
        if (static_cast<DataFileContainer *>(key_pair)->SetSubpathElement(path, key_delim, value))
        {
            m_member_map[key] = key_pair;
            return true;
        }
        else
        {
            Delete(key_pair);
            return false;
        }
    }
    else
    {
        return static_cast<DataFileContainer *>(it->second)->SetSubpathElement(path, key_delim, value);
    }
}

bool DataFileStructure::GetIsValidKey (std::string const &key)
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

} // end of namespace Xrb

