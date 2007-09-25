// ///////////////////////////////////////////////////////////////////////////
// xrb_color.cpp by Victor Dods, created 2004/07/02
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_color.h"

namespace Xrb
{

Color const Color::ms_opaque_white(1.0f, 1.0f, 1.0f, 1.0f);
Color const Color::ms_opaque_black(0.0f, 0.0f, 0.0f, 1.0f);
Color const Color::ms_transparent_black(0.0f, 0.0f, 0.0f, 0.0f);

Color const &Color::ms_identity_mask_color(ms_opaque_white);
Color const &Color::ms_identity_bias_color(ms_transparent_black);

} // end of namespace Xrb
