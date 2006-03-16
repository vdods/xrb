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
// DataFileValue
// ///////////////////////////////////////////////////////////////////////////

DataFileValue::DataFileValue (AllocationTracker *const owner_allocation_tracker)
{
    ASSERT1(owner_allocation_tracker != NULL)
    m_owner_allocation_tracker = owner_allocation_tracker;
    AddAllocation(this);
}

DataFileValue::~DataFileValue ()
{
    if (m_owner_allocation_tracker != NULL)
        RemoveAllocation(this);
}

void DataFileValue::FprintAllocations (
    FILE *const fptr,
    AllocationTracker const &allocation_tracker)
{
    fprintf(
        fptr,
        "DataFileValue::FprintAllocations(); number of allocations = %u\n",
        allocation_tracker.size());
    for (AllocationTrackerConstIterator it = allocation_tracker.begin(),
                                        it_end = allocation_tracker.end();
         it != it_end;
         ++it)
    {
        DataFileValue const *value = *it;
        ASSERT1(value != NULL)
        fprintf(fptr, "\t%p: %s\n", value, typeid(value).name());
    }
}

void DataFileValue::DeleteAllocations (
    AllocationTracker *const allocation_tracker)
{
    ASSERT1(allocation_tracker != NULL)
    for (AllocationTrackerIterator it = allocation_tracker->begin(),
                                   it_end = allocation_tracker->end();
         it != it_end;
         ++it)
    {
        DataFileValue const *value = *it;
        ASSERT1(value != NULL)
        Delete(value);
    }
    allocation_tracker->clear();
}

void DataFileValue::RemoveFromOwnerAllocationTracker (
    DataFileValue const *const value)
{
    RemoveAllocation(value);
}

void DataFileValue::AddAllocation (DataFileValue const *const value)
{
    ASSERT1(value != NULL)
    ASSERT1(value->m_owner_allocation_tracker != NULL)
    ASSERT1(
        value->m_owner_allocation_tracker->find(value) ==
        value->m_owner_allocation_tracker->end())
    value->m_owner_allocation_tracker->insert(value);
}

void DataFileValue::RemoveAllocation (DataFileValue const *const value)
{
    ASSERT1(value != NULL)
    ASSERT1(value->m_owner_allocation_tracker != NULL)
    ASSERT1(
        value->m_owner_allocation_tracker->find(value) !=
        value->m_owner_allocation_tracker->end())
    value->m_owner_allocation_tracker->erase(value);
    value->m_owner_allocation_tracker = NULL;
}

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
            formatter.ContinueLine("\\%c", Util::GetEscapeCharacter(*string));
        else
            formatter.ContinueLine("%c", *string);
    }

    formatter.ContinueLine("\"");
}

// ///////////////////////////////////////////////////////////////////////////
// DataFileKeyValuePair
// ///////////////////////////////////////////////////////////////////////////

void DataFileKeyValuePair::Print (IndentFormatter &formatter) const
{
    ASSERT1(m_value != NULL)
    if (m_value->GetType() == T_STRUCTURE)
    {
        formatter.PrintLine("%s\n{", m_key.c_str());
        formatter.Indent();
        m_value->Print(formatter);
        formatter.Unindent();
        formatter.BeginLine("}");
    }
    else if (m_value->GetType() == T_LIST)
    {
        DataFileList const *list = DStaticCast<DataFileList const *>(m_value);
        if (list->GetShouldBeFormattedInline())
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
// DataFileStructure
// ///////////////////////////////////////////////////////////////////////////

DataFileStructure::DataFileStructure (
    AllocationTracker *const owner_allocation_tracker)
    :
    DataFileValue(owner_allocation_tracker)
{
    // make sure that GetNextValue() can't be called before GetFirstValue().
    m_iteration_is_valid = false;
}

DataFileStructure::~DataFileStructure ()
{
    for (MemberMapIterator it = m_member_map.begin(),
                           it_end = m_member_map.end();
         it != it_end;
         ++it)
    {
        DataFileKeyValuePair const *key_value_pair = it->second;
        ASSERT1(key_value_pair != NULL)
        ASSERT1(!key_value_pair->GetIsOwnedByAllocationTracker())
        Delete(key_value_pair);
    }
}

DataFileKeyValuePair const *DataFileStructure::GetValue (
    std::string const &key) const
{
    ASSERT1(key.length() > 0)
    MemberMapConstIterator it = m_member_map.find(key);
    MemberMapConstIterator it_end = m_member_map.end();
    if (it == it_end)
        throw "DataFileStructure::GetValue(); key not found";
    else
    {
        DataFileKeyValuePair const *key_value_pair = it->second;
        ASSERT1(key_value_pair != NULL)
        return key_value_pair;
    }
}

DataFileKeyValuePair const *DataFileStructure::GetFirstKeyValuePair () const
{
    // set the iteration-is-valid flag to true.
    m_iteration_is_valid = true;

    // initialize m_iterator to the first list element.
    m_it = m_member_map.begin();
    m_it_end = m_member_map.end();
    // if the iterator is the end, return null.
    if (m_it == m_it_end)
        return NULL;
    // otherwise, return the iterator's contained value.
    DataFileKeyValuePair const *key_value_pair = m_it->second;
    ASSERT1(key_value_pair != NULL)
    return key_value_pair;
}

DataFileKeyValuePair const *DataFileStructure::GetNextKeyValuePair () const
{
    // prohibit continuing an iteration across a list change
    ASSERT1(m_iteration_is_valid)
    // if the iterator is at the end, return null.
    if (m_it == m_it_end)
        return NULL;
    // increment the iterator.
    ++m_it;
    // if the iterator is at the end, return null.
    if (m_it == m_it_end)
        return NULL;
    // otherwise, return the iterator's contained value.
    DataFileKeyValuePair const *key_value_pair = m_it->second;
    ASSERT1(key_value_pair != NULL)
    return key_value_pair;
}

void DataFileStructure::AddKeyValuePair (
    DataFileKeyValuePair const *key_value_pair)
{
    ASSERT1(key_value_pair != NULL)
    ASSERT1(GetHasSameOwnerAllocationTracker(key_value_pair))
    ASSERT1(key_value_pair->GetKey().length() > 0)
    ASSERT1(key_value_pair->GetValue() != NULL)

    // invalidate any in-progress iteration.
    m_iteration_is_valid = false;

    MemberMapConstIterator it = m_member_map.find(key_value_pair->GetKey());
    MemberMapConstIterator it_end = m_member_map.end();
    if (it != it_end)
        throw "DataFileStructure::AddKeyValuePair(); key collision";
    else
    {
        m_member_map[key_value_pair->GetKey()] = key_value_pair;
        RemoveAllocation(key_value_pair);
    }
}

void DataFileStructure::Print (IndentFormatter &formatter) const
{
    for (MemberMapConstIterator it = m_member_map.begin(),
                                it_end = m_member_map.end();
         it != it_end;
         ++it)
    {
        DataFileKeyValuePair const *key_value_pair = it->second;
        ASSERT1(key_value_pair != NULL)
        key_value_pair->Print(formatter);
        formatter.EndLine(";");
    }
}

// ///////////////////////////////////////////////////////////////////////////
// DataFileList
// ///////////////////////////////////////////////////////////////////////////

DataFileList::DataFileList (AllocationTracker *const owner_allocation_tracker)
    :
    DataFileValue(owner_allocation_tracker)
{
    // make sure that GetNextValue() can't be called before GetFirstValue().
    m_iteration_is_valid = false;
}

DataFileList::~DataFileList ()
{
    ASSERT1(GetIsListOfOneType())

    for (ElementListIterator it = m_element_list.begin(),
                             it_end = m_element_list.end();
         it != it_end;
         ++it)
    {
        DataFileValue const *value = *it;
        ASSERT1(value != NULL)
        ASSERT1(!value->GetIsOwnedByAllocationTracker())
        Delete(value);
    }
}

DataFileValue::Type DataFileList::GetUltimateType () const
{
    ElementListConstIterator it = m_element_list.begin();
    ElementListConstIterator it_end = m_element_list.end();
    if (it != it_end)
    {
        DataFileValue const *value = *it;
        ASSERT1(value != NULL)
        if (value->GetType() == T_LIST)
        {
            DataFileList const *list = DStaticCast<DataFileList const *>(value);
            return list->GetUltimateType();
        }
        else
            return value->GetType();
    }
    else
        return T_NO_LIST_ELEMENTS;
}

Uint32 DataFileList::GetListRecursionLevel () const
{
    ElementListConstIterator it = m_element_list.begin();
    ElementListConstIterator it_end = m_element_list.end();
    if (it != it_end)
    {
        DataFileValue const *value = *it;
        return 1 + value->GetListRecursionLevel();
    }
    else
        return 1;
}

bool DataFileList::GetShouldBeFormattedInline () const
{
    ElementListConstIterator it = m_element_list.begin();
    ElementListConstIterator it_end = m_element_list.end();
    // lists with elements that aren't lists or structures
    // should not be inlined
    if (it != it_end)
    {
        DataFileValue const *value = *it;
        ASSERT1(value != NULL)
        ASSERT1(value->GetType() != T_STRUCTURE)
        return
            value->GetType() != T_LIST &&
            value->GetType() != T_KEY_VALUE_PAIR;
    }
    else
        // empty lists should be inlined.
        return true;
}

DataFileValue const *DataFileList::GetFirstValue () const
{
    // set the iteration-is-valid flag to true.
    m_iteration_is_valid = true;

    // initialize m_iterator to the first list element.
    m_it = m_element_list.begin();
    m_it_end = m_element_list.end();
    // if the iterator is the end, return null.
    if (m_it == m_it_end)
        return NULL;
    // otherwise, return the iterator's contained value.
    DataFileValue const *value = *m_it;
    ASSERT1(value != NULL)
    return value;
}

DataFileValue const *DataFileList::GetNextValue () const
{
    // prohibit continuing an iteration across a list change
    ASSERT1(m_iteration_is_valid)
    // if the iterator is at the end, return null.
    if (m_it == m_it_end)
        return NULL;
    // increment the iterator.
    ++m_it;
    // if the iterator is at the end, return null.
    if (m_it == m_it_end)
        return NULL;
    // otherwise, return the iterator's contained value.
    DataFileValue const *value = *m_it;
    ASSERT1(value != NULL)
    return value;
}

void DataFileList::AppendValue (DataFileValue const *const value)
{
    ASSERT1(value != NULL)
    ASSERT1(GetHasSameOwnerAllocationTracker(value))

    // make sure that the value being appended is the same type
    // as the first value in the list, or the list is empty.
    ElementListIterator it = m_element_list.begin();
    ElementListIterator it_end = m_element_list.end();
    if (it != it_end)
    {
        DataFileValue const *first_element_value = *it;
        ASSERT1(first_element_value != NULL)
        if (value->GetListRecursionLevel() != first_element_value->GetListRecursionLevel() ||
            value->GetType() != first_element_value->GetType() ||
            value->GetUltimateType() != first_element_value->GetUltimateType())
        {
            throw "non-homogeneous list";
        }
    }

    // invalidate any in-progress iteration.
    m_iteration_is_valid = false;
    // add the new value onto the end of the list.
    m_element_list.push_back(value);
    // remove the value from its owner allocation tracker
    RemoveAllocation(value);
}

void DataFileList::Print (IndentFormatter &formatter) const
{
    bool inlined_list = GetShouldBeFormattedInline();

    if (inlined_list)
        formatter.BeginLine("[ ");
    else
    {
        formatter.PrintLine("[");
        formatter.Indent();
    }
        
    ElementListConstIterator it_test;
    for (ElementListConstIterator it = m_element_list.begin(),
                                  it_end = m_element_list.end();
         it != it_end;
         ++it)
    {
        DataFileValue const *value = *it;
        ASSERT1(value != NULL)
        ASSERT1(value->GetType() != T_STRUCTURE)
        
        value->Print(formatter);

        it_test = it;
        ++it_test;
        if (inlined_list)
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

    if (inlined_list)
        formatter.ContinueLine("]");
    else
    {
        formatter.Unindent();
        formatter.BeginLine("]");
    }
}

bool DataFileList::GetIsListOfOneType () const
{
    ElementListConstIterator it = m_element_list.begin();
    ElementListConstIterator it_end = m_element_list.end();
    // if the list is empty, return trivially true.
    if (it == it_end)
        return true;

    DataFileValue const *value = *it;
    Uint32 first_element_list_recursion_level = value->GetListRecursionLevel();
    Type first_element_type = value->GetType();
    Type first_element_ultimate_type = value->GetUltimateType();
    ++it;

    // if any of the elements is not the same type, then return false.
    while (it != it_end)
    {
        value = *it;
        if (value->GetListRecursionLevel() != first_element_list_recursion_level ||
            value->GetType() != first_element_type ||
            value->GetUltimateType() != first_element_ultimate_type)
        {
            return false;
        }
        ++it;
    }

    // if all elements are the same type, then return true.
    return true;
}

} // end of namespace Xrb

