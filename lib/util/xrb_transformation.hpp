// ///////////////////////////////////////////////////////////////////////////
// xrb_transformation.hpp by Victor Dods, created 2006/01/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_TRANSFORMATION_HPP_)
#define _XRB_TRANSFORMATION_HPP_

#include "xrb.hpp"

#include <string>

namespace Xrb
{

/** These functions are for use as transformation functions in
  * SignalSender/SignalReceiver connections.
  * @brief Contains various transformation functions.
  */
namespace Transformation
{

/// Returns the logical negation of @c value.
bool BooleanNegation (bool value);

/// Returns @c value with uppercase letters changed to lowercase.
std::string Lowercase (std::string value);

/// Returns @c value with lowercase letters changed to uppercase.
std::string Uppercase (std::string value);

} // end of namespace Transformation

} // end of namespace Xrb

#endif // !defined(_XRB_TRANSFORMATION_HPP_)

