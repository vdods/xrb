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

#include <list>
#include <map>
#include <set>
#include <stdio.h>
#include <string>

#include "xrb_util.h"
#include "xrb_indentformatter.h"

namespace Xrb
{

// ///////////////////////////////////////////////////////////////////////////
// DataFileValue
// ///////////////////////////////////////////////////////////////////////////

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
  *     <li>Key/value pair</li>
  *     <li>Structure</li>
  *     <li>List</li>
  *     </ul>
  *
  * The basic data types (boolean, integer, floating point, character, string)
  * are equivalent to C++ builtin types (where string is stored using an STL
  * basic_string).  The significance of the key/value pair is to give the
  * ability to give a name to a particular value.  The structure is used
  * to store nested key/value pairs (where the values can be anything except
  * key/value pairs).  The list is used to store nested values (where the
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

    typedef std::set<DataFileValue const *> AllocationTracker;
    typedef AllocationTracker::const_iterator AllocationTrackerConstIterator;
    typedef AllocationTracker::iterator AllocationTrackerIterator;

    enum DataType
    {
        T_BOOLEAN = 0,
        T_INTEGER,
        T_FLOAT,
        T_CHARACTER,
        T_STRING,
        T_KEY_VALUE_PAIR,
        T_STRUCTURE,
        T_LIST,
        T_NO_LIST_ELEMENTS,

        T_TYPE_COUNT
    }; // end of enum DataFileValue::DataType

    DataFileValue (AllocationTracker *owner_allocation_tracker);
    virtual ~DataFileValue ();

    virtual DataType GetDataType () const = 0;
    virtual DataType GetUltimateType () const = 0;
    virtual Uint32 GetListRecursionLevel () const = 0;

    inline bool GetIsOwnedByAllocationTracker () const
    {
        return m_owner_allocation_tracker != NULL;
    }
    inline bool GetHasSameOwnerAllocationTracker (
        DataFileValue const *const value) const
    {
        ASSERT1(m_owner_allocation_tracker != NULL)
        ASSERT1(value->m_owner_allocation_tracker != NULL)
        return m_owner_allocation_tracker == value->m_owner_allocation_tracker;
    }

    virtual void Print (IndentFormatter &formatter) const = 0;

    static void FprintAllocations (
        FILE *fptr,
        AllocationTracker const &allocation_tracker);
    static void DeleteAllocations (
        AllocationTracker *allocation_tracker);
    static void RemoveFromOwnerAllocationTracker (DataFileValue const *value);

protected:

    static void AddAllocation (DataFileValue const *value);
    static void RemoveAllocation (DataFileValue const *value);

private:

    mutable AllocationTracker *m_owner_allocation_tracker;
}; // end of class DataFileValue

// ///////////////////////////////////////////////////////////////////////////
// DataFileBoolean
// ///////////////////////////////////////////////////////////////////////////

class DataFileBoolean : public DataFileValue
{
public:

    DataFileBoolean (
        bool const value,
        AllocationTracker *const owner_allocation_tracker)
        :
        DataFileValue (owner_allocation_tracker)
    {
        m_value = value;
    }
    virtual ~DataFileBoolean () { }

    virtual DataType GetDataType () const
    {
        return T_BOOLEAN;
    }
    virtual DataType GetUltimateType () const
    {
        return T_BOOLEAN;
    }
    virtual Uint32 GetListRecursionLevel () const
    {
        return 0;
    }

    inline bool GetValue () const
    {
        return m_value;
    }

    virtual void Print (IndentFormatter &formatter) const
    {
        formatter.BeginLine("%s", BOOL_TO_STRING(m_value));
    }

private:

    bool m_value;
}; // end of class DataFileBoolean

// ///////////////////////////////////////////////////////////////////////////
// DataFileInteger
// ///////////////////////////////////////////////////////////////////////////

class DataFileInteger : public DataFileValue
{
public:

    DataFileInteger (
        Uint32 const value,
        AllocationTracker *const owner_allocation_tracker)
        :
        DataFileValue (owner_allocation_tracker)
    {
        m_was_constructed_as_unsigned = true;
        m_value = value;
    }
    DataFileInteger (
        Sint32 const value,
        AllocationTracker *const owner_allocation_tracker)
        :
        DataFileValue (owner_allocation_tracker)
    {
        m_was_constructed_as_unsigned = false;
        m_value = static_cast<Uint32>(value);
    }
    virtual ~DataFileInteger () { }

    virtual DataType GetDataType () const
    {
        return T_INTEGER;
    }
    virtual DataType GetUltimateType () const
    {
        return T_INTEGER;
    }
    virtual Uint32 GetListRecursionLevel () const
    {
        return 0;
    }

    inline bool GetIsUnsigned () const
    {
        return m_was_constructed_as_unsigned;
    }
    inline Sint32 GetSignedValue () const
    {
        return static_cast<Sint32>(m_value);
    }
    inline Uint32 GetUnsignedValue () const
    {
        return m_value;
    }

    virtual void Print (IndentFormatter &formatter) const
    {
        if (m_was_constructed_as_unsigned)
            formatter.BeginLine("%u", m_value);
        else
            formatter.BeginLine("%d", static_cast<Sint32>(m_value));
    }

private:

    bool m_was_constructed_as_unsigned;
    Uint32 m_value;
}; // end of class DataFileInteger

// ///////////////////////////////////////////////////////////////////////////
// DataFileFloat
// ///////////////////////////////////////////////////////////////////////////

class DataFileFloat : public DataFileValue
{
public:

    DataFileFloat (
        Float const value,
        AllocationTracker *const owner_allocation_tracker)
        :
        DataFileValue (owner_allocation_tracker)
    {
        m_value = value;
    }
    virtual ~DataFileFloat () { }

    virtual DataType GetDataType () const
    {
        return T_FLOAT;
    }
    virtual DataType GetUltimateType () const
    {
        return T_FLOAT;
    }
    virtual Uint32 GetListRecursionLevel () const
    {
        return 0;
    }

    inline Float GetValue () const
    {
        return m_value;
    }

    virtual void Print (IndentFormatter &formatter) const
    {
        formatter.BeginLine("%e", m_value);
    }

private:

    Float m_value;
}; // end of class DataFileFloat

// ///////////////////////////////////////////////////////////////////////////
// DataFileCharacter
// ///////////////////////////////////////////////////////////////////////////

class DataFileCharacter : public DataFileValue
{
public:

    DataFileCharacter (
        char const value,
        AllocationTracker *const owner_allocation_tracker)
        :
        DataFileValue (owner_allocation_tracker)
    {
        m_value = value;
    }
    virtual ~DataFileCharacter () { }

    virtual DataType GetDataType () const
    {
        return T_CHARACTER;
    }
    virtual DataType GetUltimateType () const
    {
        return T_CHARACTER;
    }
    virtual Uint32 GetListRecursionLevel () const
    {
        return 0;
    }

    inline char GetValue () const
    {
        return m_value;
    }

    virtual void Print (IndentFormatter &formatter) const
    {
        if (Util::GetDoesCharacterNeedEscaping(m_value))
            formatter.BeginLine("'\\%c'", Util::GetEscapeCharacter(m_value));
        else
            formatter.BeginLine("'%c'", m_value);
    }

private:

    char m_value;
}; // end of class DataFileCharacter

// ///////////////////////////////////////////////////////////////////////////
// DataFileString
// ///////////////////////////////////////////////////////////////////////////

class DataFileString : public DataFileValue
{
public:

    DataFileString (AllocationTracker *const owner_allocation_tracker)
        :
        DataFileValue (owner_allocation_tracker)
    {
    }
    DataFileString (
        std::string const &value,
        AllocationTracker *const owner_allocation_tracker)
        :
        DataFileValue (owner_allocation_tracker)
    {
        m_value = value;
    }
    virtual ~DataFileString () { }

    virtual DataType GetDataType () const
    {
        return T_STRING;
    }
    virtual DataType GetUltimateType () const
    {
        return T_STRING;
    }
    virtual Uint32 GetListRecursionLevel () const
    {
        return 0;
    }

    inline std::string const &GetValue () const
    {
        return m_value;
    }

    inline void AppendString (std::string const &string)
    {
        m_value += string;
    }
    inline void AppendCharacter (char const character)
    {
        m_value += character;
    }

    virtual void Print (IndentFormatter &formatter) const;

private:

    std::string m_value;
}; // end of class DataFileString

// ///////////////////////////////////////////////////////////////////////////
// DataFileKeyValuePair
// ///////////////////////////////////////////////////////////////////////////

class DataFileKeyValuePair : public DataFileValue
{
public:

    DataFileKeyValuePair (
        std::string const &key,
        DataFileValue const *const value,
        AllocationTracker *const owner_allocation_tracker)
        :
        DataFileValue (owner_allocation_tracker)
    {
        ASSERT1(key.length() > 0)
        ASSERT1(value != NULL)
        ASSERT1(GetHasSameOwnerAllocationTracker(value))
        m_key = key;
        m_value = value;
        RemoveAllocation(m_value);
    }
    virtual ~DataFileKeyValuePair ()
    {
        ASSERT1(m_value != NULL)
        ASSERT1(!m_value->GetIsOwnedByAllocationTracker())
        Delete(m_value);
    }

    virtual DataType GetDataType () const
    {
        return T_KEY_VALUE_PAIR;
    }
    virtual DataType GetUltimateType () const
    {
        return T_KEY_VALUE_PAIR;
    }
    virtual Uint32 GetListRecursionLevel () const
    {
        return 0;
    }

    inline std::string const &GetKey () const
    {
        ASSERT1(m_key.length() > 0)
        return m_key;
    }
    inline DataFileValue const *GetValue () const
    {
        ASSERT1(m_value != NULL)
        return m_value;
    }

    virtual void Print (IndentFormatter &formatter) const;

private:

    std::string m_key;
    DataFileValue const *m_value;
}; // end of class DataFileKeyValuePair

// ///////////////////////////////////////////////////////////////////////////
// DataFileStructure
// ///////////////////////////////////////////////////////////////////////////

class DataFileStructure : public DataFileValue
{
public:

    DataFileStructure (AllocationTracker *owner_allocation_tracker);
    virtual ~DataFileStructure ();

    virtual DataType GetDataType () const
    {
        return T_STRUCTURE;
    }
    virtual DataType GetUltimateType () const
    {
        return T_STRUCTURE;
    }
    virtual Uint32 GetListRecursionLevel () const
    {
        return 0;
    }

    // this function does not invalidate iteration.
    DataFileKeyValuePair const *GetValue (std::string const &key) const;
    // returns the first value in the structure, or NULL if the structure is empty.
    DataFileKeyValuePair const *GetFirstKeyValuePair () const;
    // returns the next value in the structure, or NULL if at the end of the structure.
    DataFileKeyValuePair const *GetNextKeyValuePair () const;

    void AddKeyValuePair (DataFileKeyValuePair const *key_value_pair);
    virtual void Print (IndentFormatter &formatter) const;

private:

    typedef std::map<std::string, DataFileKeyValuePair const *> MemberMap;
    typedef MemberMap::iterator MemberMapIterator;
    typedef MemberMap::const_iterator MemberMapConstIterator;

    MemberMap m_member_map;
    mutable MemberMapConstIterator m_it;
    mutable MemberMapConstIterator m_it_end;
    mutable bool m_iteration_is_valid;
}; // end of class DataFileStructure

// ///////////////////////////////////////////////////////////////////////////
// DataFileList
// ///////////////////////////////////////////////////////////////////////////

class DataFileList : public DataFileValue
{
public:

    DataFileList (AllocationTracker *owner_allocation_tracker);
    virtual ~DataFileList ();

    virtual DataType GetDataType () const
    {
        return T_LIST;
    }
    virtual DataType GetUltimateType () const;
    virtual Uint32 GetListRecursionLevel () const;
    bool GetShouldBeFormattedInline () const;

    // returns the first value in the list, or NULL if the list is empty.
    DataFileValue const *GetFirstValue () const;
    // returns the next value in the list, or NULL if at the end of the list.
    DataFileValue const *GetNextValue () const;

    void AppendValue (DataFileValue const *value);
    virtual void Print (IndentFormatter &formatter) const;

private:

    bool GetIsListOfOneType () const;

    typedef std::list<DataFileValue const *> ElementList;
    typedef ElementList::iterator ElementListIterator;
    typedef ElementList::const_iterator ElementListConstIterator;

    ElementList m_element_list;
    mutable ElementListConstIterator m_it;
    mutable ElementListConstIterator m_it_end;
    mutable bool m_iteration_is_valid;
}; // end of class DataFileList

} // end of namespace Xrb

#endif // !defined(_XRB_DATAFILEVALUE_H_)

