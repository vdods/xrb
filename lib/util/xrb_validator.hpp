// ///////////////////////////////////////////////////////////////////////////
// xrb_validator.hpp by Victor Dods, created 2005/06/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_VALIDATOR_HPP_)
#define _XRB_VALIDATOR_HPP_

#include "xrb.hpp"

namespace Xrb
{

// ///////////////////////////////////////////////////////////////////////////
// Validator<ValueType>
// ///////////////////////////////////////////////////////////////////////////

template <typename ValueType>
struct Validator
{
    virtual ~Validator () { }

    virtual bool GetIsValid (ValueType value) const = 0;
    virtual ValueType Validate (ValueType value) const = 0;
}; // end of struct Validator<ValueType>

// ///////////////////////////////////////////////////////////////////////////
// GreaterThanValidator<ValueType>
// ///////////////////////////////////////////////////////////////////////////

template <typename ValueType>
struct GreaterThanValidator : public Validator<ValueType>
{
    ValueType const m_lower_bound;
    ValueType const m_lowest_valid_value;

    GreaterThanValidator (
        ValueType const lower_bound,
        ValueType const lowest_valid_value)
        :
        m_lower_bound(lower_bound),
        m_lowest_valid_value(lowest_valid_value)
    { }

    virtual bool GetIsValid (ValueType value) const
    {
        return value > m_lower_bound;
    }
    virtual ValueType Validate (ValueType value) const
    {
        return value > m_lower_bound ?
               value :
               m_lowest_valid_value;
    }
}; // end of struct GreaterThanValidator<ValueType>

// ///////////////////////////////////////////////////////////////////////////
// GreatorOrEqualValidator<ValueType>
// ///////////////////////////////////////////////////////////////////////////

template <typename ValueType>
struct GreaterOrEqualValidator : public Validator<ValueType>
{
    ValueType const m_lower_bound;

    GreaterOrEqualValidator (
        ValueType const lower_bound)
        :
        m_lower_bound(lower_bound)
    { }

    virtual bool GetIsValid (ValueType value) const
    {
        return value >= m_lower_bound;
    }
    virtual ValueType Validate (ValueType value) const
    {
        return value >= m_lower_bound ?
               value :
               m_lower_bound;
    }
}; // end of struct GreaterOrEqualValidator<ValueType>

// ///////////////////////////////////////////////////////////////////////////
// LessThanValidator<ValueType>
// ///////////////////////////////////////////////////////////////////////////

template <typename ValueType>
struct LessThanValidator : public Validator<ValueType>
{
    ValueType const m_upper_bound;
    ValueType const m_highest_valid_value;

    LessThanValidator (
        ValueType const upper_bound,
        ValueType const highest_valid_value)
        :
        m_upper_bound(upper_bound),
        m_highest_valid_value(highest_valid_value)
    { }

    virtual bool GetIsValid (ValueType value) const
    {
        return value < m_upper_bound;
    }
    virtual ValueType Validate (ValueType value) const
    {
        return value < m_upper_bound ?
               value :
               m_highest_valid_value;
    }
}; // end of struct LessThanValidator<ValueType>

// ///////////////////////////////////////////////////////////////////////////
// LessOrEqualValidator<ValueType>
// ///////////////////////////////////////////////////////////////////////////

template <typename ValueType>
struct LessOrEqualValidator : public Validator<ValueType>
{
    ValueType const m_upper_bound;

    LessOrEqualValidator (
        ValueType const upper_bound)
        :
        m_upper_bound(upper_bound)
    { }

    virtual bool GetIsValid (ValueType value) const
    {
        return value <= m_upper_bound;
    }
    virtual ValueType Validate (ValueType value) const
    {
        return value <= m_upper_bound ?
               value :
               m_upper_bound;
    }
}; // end of struct LessOrEqualValidator<ValueType>

// ///////////////////////////////////////////////////////////////////////////
// RangeInclusiveValidator<ValueType>
// ///////////////////////////////////////////////////////////////////////////

template <typename ValueType>
struct RangeInclusiveValidator : public Validator<ValueType>
{
    ValueType const m_lower_bound;
    ValueType const m_upper_bound;

    RangeInclusiveValidator (
        ValueType const lower_bound,
        ValueType const upper_bound)
        :
        m_lower_bound(lower_bound),
        m_upper_bound(upper_bound)
    {
        ASSERT1(lower_bound <= upper_bound);
    }

    virtual bool GetIsValid (ValueType value) const
    {
        return m_lower_bound <= value &&
               value <= m_upper_bound;
    }
    virtual ValueType Validate (ValueType value) const
    {
        if (value < m_lower_bound)
            return m_lower_bound;
        else if (value > m_upper_bound)
            return m_upper_bound;
        else
            return value;
    }
}; // end of struct RangeInclusiveValidator<ValueType>

// ///////////////////////////////////////////////////////////////////////////
// RangeExclusiveValidator<ValueType>
// ///////////////////////////////////////////////////////////////////////////

template <typename ValueType>
struct RangeExclusiveValidator : public Validator<ValueType>
{
    ValueType const m_lower_bound;
    ValueType const m_lowest_valid_value;
    ValueType const m_highest_valid_value;
    ValueType const m_upper_bound;

    RangeExclusiveValidator (
        ValueType const lower_bound,
        ValueType const lowest_valid_value,
        ValueType const highest_valid_value,
        ValueType const upper_bound)
        :
        m_lower_bound(lower_bound),
        m_lowest_valid_value(lowest_valid_value),
        m_highest_valid_value(highest_valid_value),
        m_upper_bound(upper_bound)
    {
        ASSERT1(lower_bound < lowest_valid_value);
        ASSERT1(lowest_valid_value < highest_valid_value);
        ASSERT1(highest_valid_value < upper_bound);
    }

    virtual bool GetIsValid (ValueType value) const
    {
        return m_lower_bound <= value &&
               value <= m_upper_bound;
    }
    virtual ValueType Validate (ValueType value) const
    {
        if (value < m_lower_bound)
            return m_lowest_valid_value;
        else if (value > m_upper_bound)
            return m_highest_valid_value;
        else
            return value;
    }
}; // end of struct RangeExclusiveValidator<ValueType>

} // end of namespace Xrb

#endif // !defined(_XRB_VALIDATOR_HPP_)

