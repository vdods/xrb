// ///////////////////////////////////////////////////////////////////////////
// xrb_color.h by Victor Dods, created 2004/07/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_COLOR_H_)
#define _XRB_COLOR_H_

#include "xrb.h"

#include <stdio.h>

#include "xrb_ntuple.h"

namespace Xrb
{

typedef Float ColorCoord;
typedef NTuple<ColorCoord, 4> Color;

} // end of namespace Xrb

#endif // !defined(_XRB_COLOR_H_)

