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
        "DAT_INTEGER",
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
// DataFileLeafValue
// ///////////////////////////////////////////////////////////////////////////

DataFileLeafValue::~DataFileLeafValue () { }

DataFileValue const *DataFileLeafValue::GetPathElement (
    std::string const &path,
    Uint32 const start) const
{
    ASSERT1(start <= path.length())

    if (start >= path.length())
        return this;

    fprintf(
        stderr,
        "data path error: in path \"%s\" - element type %s can not have subelements (subpath \"%s\")\n",
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
// DataFileInteger
// ///////////////////////////////////////////////////////////////////////////

void DataFileInteger::Sign (NumericSign const sign)
{
    ASSERT1(!m_is_signed && "you should only call this method on an unsigned value")
    m_is_signed = true;

    ASSERT1(sign == NEGATIVE || sign == POSITIVE)
    if (sign == POSITIVE)
    {
        if (m_value > static_cast<Uint32>(SINT32_UPPER_BOUND))
        {
            std::ostringstream out;
            out << "signed integer value +" << m_value << " is not representable in 32 bits";
            throw out.str();
        }
        else
        {
            // nothing needs to be done to convert the value to signed
        }
    }
    else
    {
        if (m_value > static_cast<Uint32>(SINT32_LOWER_BOUND))
        {
            std::ostringstream out;
            out << "signed integer value -" << m_value << " is not representable in 32 bits";
            throw out.str();
        }
        else
        {
            // bitwise negation (one's compliment negative) is = -x-1.
            // therefore we add one to get -x.
            m_value = ~m_value + 1;
        }
    }
}

void DataFileInteger::PrintAST (IndentFormatter &formatter) const
{
    if (m_is_signed)
        formatter.EndLine("DAT_INTEGER - %+d", static_cast<Sint32>(m_value));
    else
        formatter.EndLine("DAT_INTEGER - %u", m_value);
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

DataFileValue const *DataFileKeyPair::GetPathElement (
    std::string const &path,
    Uint32 const start) const
{
    ASSERT1(start <= path.length())

//     fprintf(stderr, "DataFileKeyPair::GetPathElement(\"%s\");\n", path.c_str()+start);

    if (start >= path.length())
        return GetValue();

    if (path[start] != '|')
    {
        fprintf(
            stderr,
            "data path error: in path \"%s\" - invalid subpath \"%s\" - expected '|' prefix\n",
            path.c_str(),
            path.c_str() + start);
        return NULL;
    }

    return GetValue()->GetPathElement(path, start);
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

void DataFileArray::AppendValue (DataFileValue const *const value)
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

DataFileValue const *DataFileArray::GetPathElement (
    std::string const &path,
    Uint32 start) const
{
    ASSERT1(start <= path.length())

//     fprintf(stderr, "DataFileArray::GetPathElement(\"%s\");\n", path.c_str()+start);

    if (start >= path.length())
        return this;

    if (path[start] != '|')
    {
        fprintf(
            stderr,
            "data path error: in path \"%s\" - invalid subpath \"%s\" - expected '|' prefix\n",
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
            "data path error: in path \"%s\" - missing array index\n",
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
                "data path error: in path \"%s\" - invalid array index \"%s\"\n",
                path.c_str(),
                path.substr(start, key_delim-start).c_str());
            return NULL;
        }
    }

    if (array_index >= m_element_vector.size())
    {
        fprintf(
            stderr,
            "data path error: in path \"%s\" - out of bounds array index \"%s\"\n",
            path.c_str(),
            path.substr(start, key_delim-start).c_str());
        return NULL;
    }

    ASSERT1(key_delim < UINT32_UPPER_BOUND)
    return m_element_vector[array_index]->GetPathElement(path, key_delim);
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

void DataFileStructure::AddKeyPair (DataFileKeyPair *const key_pair)
{
    ASSERT1(key_pair != NULL)
    ASSERT1(key_pair->GetKey().length() > 0)
    ASSERT1(key_pair->GetValue() != NULL)

    if (m_member_map.find(key_pair->GetKey()) == m_member_map.end())
        m_member_map[key_pair->GetKey()] = key_pair;
    else
    {
        std::ostringstream out;
        out << "collision with key " << key_pair->GetKey();
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

DataFileValue const *DataFileStructure::GetPathElement (
    std::string const &path,
    Uint32 start) const
{
    ASSERT1(start <= path.length())

//     fprintf(stderr, "DataFileStructure::GetPathElement(\"%s\");\n", path.c_str()+start);

    if (start >= path.length())
        return this;

    if (path[start] != '|')
    {
        fprintf(
            stderr,
            "data path error: in path \"%s\" - invalid subpath \"%s\" - expected '|' prefix\n",
            path.c_str(),
            path.c_str() + start);
        return NULL;
    }

    ++start;
    Uint32 key_delim = Min(path.length(), static_cast<Uint32>(path.find_first_of("|", start)));
    std::string key(path.substr(start, key_delim-start));
    MemberMapConstIterator it = m_member_map.find(key);
    if (it == m_member_map.end())
    {
        fprintf(
            stderr,
            "data path error: in path \"%s\" - unmatched element \"%s\"\n",
            path.c_str(),
            key.c_str());
        return NULL;
    }
    else
    {
        ASSERT1(key_delim < UINT32_UPPER_BOUND)
        return it->second->GetPathElement(path, key_delim);
    }
}

} // end of namespace Xrb

