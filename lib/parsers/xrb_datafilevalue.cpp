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

#include <typeinfo>

namespace Xrb
{

// ///////////////////////////////////////////////////////////////////////////
// DataFileString
// ///////////////////////////////////////////////////////////////////////////

void DataFileString::Print (IndentFormatter &formatter) const
{
    formatter.ContinueLine("\"");

    ASSERT1(m_value.c_str() != NULL)
    for (char const *string = m_value.c_str();
         *string != '\0';
         ++string)
    {
        if (Util::GetDoesCharacterNeedEscaping(*string))
            formatter.ContinueLine("\\%c", Util::GetEscapedCharacterBase(*string));
        else
            formatter.ContinueLine("%c", *string);
    }

    formatter.ContinueLine("\"");
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
        ASSERT1(value->GetElementType() != DAT_STRUCTURE)
        return value->GetElementType() != DAT_ARRAY && value->GetElementType() != DAT_KEY_PAIR;
    }
    else
        // empty arrays should be inlined.
        return true;
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
        if (value->GetElementType() != first_element_value->GetElementType()
            ||
            value->GetElementType() == DAT_ARRAY &&
            !GetDoesMatchDimensionAndType(
                DStaticCast<DataFileArray const *>(value),
                DStaticCast<DataFileArray const *>(first_element_value)))
        {
            throw "non-homogeneous array";
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
        ASSERT1(value->GetElementType() != DAT_STRUCTURE)

        value->Print(formatter);

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

bool DataFileArray::GetDoesMatchDimensionAndType (DataFileArray const *const array0, DataFileArray const *const array1)
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
    for (MemberSetIterator it = m_member_set.begin(),
                           it_end = m_member_set.end();
         it != it_end;
         ++it)
    {
        DataFileKeyPair const *key_pair = *it;
        ASSERT1(key_pair != NULL)
        Delete(key_pair);
    }
}

DataFileValue const *DataFileStructure::GetValue (std::string const &key) const
{
    ASSERT1(key.length() > 0)
    DataFileKeyPair key_pair_search(key, NULL);
    MemberSetConstIterator it = m_member_set.find(&key_pair_search);
    if (it == m_member_set.end())
        return NULL;
    else
        return (*it)->GetValue();
}

void DataFileStructure::AddKeyPair (DataFileKeyPair *const key_pair)
{
    ASSERT1(key_pair != NULL)
    ASSERT1(key_pair->GetKey().length() > 0)
    ASSERT1(key_pair->GetValue() != NULL)

    if (m_member_set.find(key_pair) != m_member_set.end())
        throw "DataFileStructure::AddKeyPair(); key collision";
    else
        m_member_set.insert(key_pair);
}

void DataFileStructure::Print (IndentFormatter &formatter) const
{
    for (MemberSetConstIterator it = m_member_set.begin(),
                                it_end = m_member_set.end();
         it != it_end;
         ++it)
    {
        DataFileKeyPair const *key_pair = *it;
        ASSERT1(key_pair != NULL)
        key_pair->Print(formatter);
        formatter.EndLine(";");
    }
}

} // end of namespace Xrb

