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
class Color : public NTuple<ColorCoord, 4>
{
public:

    static Color const ms_white;
    static Color const ms_black;
    static Color const ms_transparent;

    // efficient no-init constructor
    Color () { }
    // constructs an opaque RGB color (fully opaque alpha channel)
    Color (ColorCoord r, ColorCoord g, ColorCoord b)
        :
        NTuple<ColorCoord, 4>(r, g, b, 1.0f)
    { }
    // constructs an RGBA color
    Color (ColorCoord r, ColorCoord g, ColorCoord b, ColorCoord a)
        :
        NTuple<ColorCoord, 4>(r, g, b, a)
    { }
    // copy constructor
    Color (NTuple<ColorCoord, 4> const &c)
        :
        NTuple<ColorCoord, 4>(c)
    { }
}; // end of class Color
// typedef NTuple<ColorCoord, 4> Color;

} // end of namespace Xrb

#endif // !defined(_XRB_COLOR_H_)

