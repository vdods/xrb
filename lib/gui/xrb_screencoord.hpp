// ///////////////////////////////////////////////////////////////////////////
// xrb_screencoord.hpp by Victor Dods, created 2005/05/31
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_SCREENCOORD_HPP_)
#define _XRB_SCREENCOORD_HPP_

#include "xrb.hpp"

#include "xrb_rect.hpp"
#include "xrb_simpletransform2.hpp"
#include "xrb_vector.hpp"

namespace Xrb
{

typedef Sint32 ScreenCoord;
typedef Vector<ScreenCoord, 2> ScreenCoordVector2;
typedef Rect<ScreenCoord> ScreenCoordRect;
typedef SimpleTransform2<ScreenCoord> ScreenCoordSimpleTransform2;

} // end of namespace Xrb

// lower/upper bound on ScreenCoord values
#define SCREENCOORD_LOWER_BOUND SINT32_LOWER_BOUND
#define SCREENCOORD_UPPER_BOUND SINT32_UPPER_BOUND

#endif // !defined(_XRB_SCREENCOORD_HPP_)

