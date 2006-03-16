// ///////////////////////////////////////////////////////////////////////////
// xrb_validator.h by Victor Dods, created 2005/06/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_VALIDATOR_H_)
#define _XRB_VALIDATOR_H_

#include "xrb.h"

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
    ValueType const m_comparison_value;
    ValueType const m_smallest_valid_value;

    GreaterThanValidator (
        ValueType const comparison_value,
        ValueType const smallest_valid_value)
        :
        m_comparison_value(comparison_value),
        m_smallest_valid_value(smallest_valid_value)
    { }

    virtual bool GetIsValid (ValueType value) const
    {
        return value > m_comparison_value;
    }
    virtual ValueType Validate (ValueType value) const
    {
        return value > m_comparison_value ?
               value :
               m_smallest_valid_value;
    }
}; // end of struct GreaterThanValidator<ValueType>

// ///////////////////////////////////////////////////////////////////////////
// GreatorOrEqualValidator<ValueType>
// ///////////////////////////////////////////////////////////////////////////

template <typename ValueType>
struct GreaterOrEqualValidator : public Validator<ValueType>
{
    ValueType const m_comparison_value;

    GreaterOrEqualValidator (
        ValueType const comparison_value)
        :
        m_comparison_value(comparison_value)
    { }

    virtual bool GetIsValid (ValueType value) const
    {
        return value >= m_comparison_value;
    }
    virtual ValueType Validate (ValueType value) const
    {
        return value >= m_comparison_value ?
               value :
               m_comparison_value;
    }
}; // end of struct GreaterOrEqualValidator<ValueType>

// ///////////////////////////////////////////////////////////////////////////
// LessThanValidator<ValueType>
// ///////////////////////////////////////////////////////////////////////////

template <typename ValueType>
struct LessThanValidator : public Validator<ValueType>
{
    ValueType const m_comparison_value;
    ValueType const m_largest_valid_value;

    LessThanValidator (
        ValueType const comparison_value,
        ValueType const largest_valid_value)
        :
        m_comparison_value(comparison_value),
        m_largest_valid_value(largest_valid_value)
    { }

    virtual bool GetIsValid (ValueType value) const
    {
        return value < m_comparison_value;
    }
    virtual ValueType Validate (ValueType value) const
    {
        return value < m_comparison_value ?
               value :
               m_largest_valid_value;
    }
}; // end of struct LessThanValidator<ValueType>

// ///////////////////////////////////////////////////////////////////////////
// LessOrEqualValidator<ValueType>
// ///////////////////////////////////////////////////////////////////////////

template <typename ValueType>
struct LessOrEqualValidator : public Validator<ValueType>
{
    ValueType const m_comparison_value;

    LessOrEqualValidator (
        ValueType const comparison_value)
        :
        m_comparison_value(comparison_value)
    { }

    virtual bool GetIsValid (ValueType value) const
    {
        return value <= m_comparison_value;
    }
    virtual ValueType Validate (ValueType value) const
    {
        return value <= m_comparison_value ?
               value :
               m_comparison_value;
    }
}; // end of struct LessOrEqualValidator<ValueType>

// ///////////////////////////////////////////////////////////////////////////
// RangeValidator<ValueType>
// ///////////////////////////////////////////////////////////////////////////

template <typename ValueType>
struct RangeValidator : public Validator<ValueType>
{
    ValueType const m_lower_bound;
    ValueType const m_upper_bound;

    RangeValidator (
        ValueType const lower_bound,
        ValueType const upper_bound)
        :
        m_lower_bound(lower_bound),
        m_upper_bound(upper_bound)
    {
        ASSERT1(lower_bound <= upper_bound)
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
}; // end of struct RangeValidator<ValueType>

} // end of namespace Xrb

#endif // !defined(_XRB_VALIDATOR_H_)

