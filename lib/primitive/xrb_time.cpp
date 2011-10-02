// ///////////////////////////////////////////////////////////////////////////
// xrb_time.cpp by Victor Dods, created 2011/09/29
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_time.hpp"

#include <limits>

namespace Xrb {

// compile time asserts to check for the necessary properties of std::numeric_limits<double>
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(DOUBLE_HAS_QUIET_NAN_CHECK, std::numeric_limits<double>::has_quiet_NaN)
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(DOUBLE_HAS_INFINITY_CHECK, std::numeric_limits<double>::has_infinity)

// definition of the static members
Time const Time::ms_beginning_of(0.0);
Time const Time::ms_invalid(std::numeric_limits<double>::quiet_NaN());
Time const Time::ms_negative_infinity(-std::numeric_limits<double>::infinity());
Time const Time::ms_positive_infinity( std::numeric_limits<double>::infinity());

} // end of namespace Xrb
