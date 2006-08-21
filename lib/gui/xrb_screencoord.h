// ///////////////////////////////////////////////////////////////////////////
// xrb_screencoord.h by Victor Dods, created 2005/05/31
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_SCREENCOORD_H_)
#define _XRB_SCREENCOORD_H_

#include "xrb.h"

#include "xrb_rect.h"
#include "xrb_simpletransform2.h"
#include "xrb_vector.h"

namespace Xrb
{

typedef Sint32 ScreenCoord;
typedef Vector<ScreenCoord, 2> ScreenCoordVector2;
typedef Rect<ScreenCoord> ScreenCoordRect;
typedef SimpleTransform2<ScreenCoord> ScreenCoordSimpleTransform2;

} // end of namespace Xrb

#endif // !defined(_XRB_SCREENCOORD_H_)

