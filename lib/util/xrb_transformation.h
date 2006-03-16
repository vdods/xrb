// ///////////////////////////////////////////////////////////////////////////
// xrb_transformation.h by Victor Dods, created 2006/01/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_TRANSFORMATION_H_)
#define _XRB_TRANSFORMATION_H_

#include "xrb.h"

namespace Xrb
{

/** These functions are for use as transformation functions in
  * SignalSender/SignalReceiver connections.
  * @brief Contains various transformation functions.
  */
namespace Transformation
{

/// Returns the logical negation of @c value.
inline bool BooleanNegation (bool value)
{
    return !value;
}

} // end of namespace Transformation

} // end of namespace Xrb

#endif // !defined(_XRB_TRANSFORMATION_H_)

