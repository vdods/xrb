// ///////////////////////////////////////////////////////////////////////////
// xrb_time.hpp by Victor Dods, created 2011/09/29
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_TIME_HPP_)
#define _XRB_TIME_HPP_

#include "xrb.hpp"

namespace Xrb {

/// @brief Class for storing absolute time values (locations in time) with high precision in the units of seconds.
/// @details Time deltas (differences between absolute time values) will be expressed as Float values for higher
/// efficiency (in memory and performance).
//
/// Time is an affine space [type] (a linear space with no preferred point or 'origin') with associated vector
/// space [type] (a linear space with origin) Time::Delta.  However the "beginning of time" (see
/// @c Time::ms_beginning_of) is a distinguished point which does not play the role of the origin in a vector
/// space.  It is taken to be zero so that Time values indicate the number of seconds since time began.
class Time
{
public:

    /// @brief Sentinel value for the 'beginning of time'.
    /// @details It is taken to be zero so that Time values indicate the number of seconds since time began.
    static Time const ms_beginning_of;
    /// @brief Sentinel value for an 'invalid' time.
    /// @details It is taken to be not-a-number (see @c std::numeric_limits<double>).
    static Time const ms_invalid;
    /// @brief Sentinel value for the earliest possible Time.
    /// @details It is taken to be negative infinity (see @c std::numeric_limits<double>). This value should have
    /// the property of being less than or equal to every valid Time value.
    static Time const ms_negative_infinity;
    /// @brief Sentinel value for the latest possible Time.
    /// @details It is taken to be positive infinity (see @c std::numeric_limits<double>). This value should have
    /// the property of being greater than or equal to every valid Time value.
    static Time const ms_positive_infinity;
    
    /// This is the resulting type of the difference between two Time values.
    /// Time deltas are assumed to be relatively close to zero.
    typedef Float Delta;

    /// Default constructor leaves the value uninitialized.
    Time () { }
    /// Construct a Time value given the number of seconds since the beginning of time (however that is defined).
    explicit Time (double seconds) : m_seconds(seconds) { }
    /// Copy constructor.
    Time (Time const &t) : m_seconds(t.m_seconds) { }

    /// Assignment operator.
    void operator = (Time t) { m_seconds = t.m_seconds; }

    bool operator == (Time t) const { return m_seconds == t.m_seconds; }
    bool operator != (Time t) const { return m_seconds != t.m_seconds; }
    bool operator <  (Time t) const { return m_seconds <  t.m_seconds; }
    bool operator <= (Time t) const { return m_seconds <= t.m_seconds; }
    bool operator >  (Time t) const { return m_seconds >  t.m_seconds; }
    bool operator >= (Time t) const { return m_seconds >= t.m_seconds; }

    /// @brief Returns true if this Time value is valid (i.e. not equal to the Time value stored in ms_invalid).
    /// @details Infinite values are considered valid.
    bool IsValid () const { return m_seconds != ms_invalid.m_seconds; }
    /// @brief Returns true if this Time value is finite (i.e. not equal to either of the infinities).
    bool IsFinite () const { return m_seconds != ms_negative_infinity.m_seconds && m_seconds != ms_positive_infinity.m_seconds; }
    /// Accessor for the double-valued number of seconds since time began.
    double AsDouble () const { return m_seconds; }

    void operator += (Delta dt) { m_seconds += dt; }
    void operator -= (Delta dt) { m_seconds -= dt; }

    void Print (std::ostream &stream) const { stream << "Time(" << m_seconds << "s)"; }

private:

    double m_seconds;
}; // end of class Time

/// Gives a new Time value which is offset from t by dt.
inline Time operator + (Time t, Time::Delta dt)
{
    return Time(t.AsDouble() + dt);
}

/// Gives a new Time value which is offset from t by dt.
inline Time operator + (Time::Delta dt, Time t)
{
    return Time(dt + t.AsDouble());
}

/// Gives the time delta between the two Time values.
inline Time::Delta operator - (Time t0, Time t1)
{
    return Time::Delta(t0.AsDouble() - t1.AsDouble());
}

/// Given a new Time value which is offset from t by -dt.
inline Time operator - (Time t, Time::Delta dt)
{
    return Time(t.AsDouble() - dt);
}

inline std::ostream &operator << (std::ostream &stream, Time t)
{
    t.Print(stream);
    return stream;
}

} // end of namespace Xrb

#endif // !defined(_XRB_TIME_HPP_)
