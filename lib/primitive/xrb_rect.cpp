// ///////////////////////////////////////////////////////////////////////////
// xrb_rect.cpp by Victor Dods, created 2005/04/22
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_rect.h"

namespace Xrb
{

void Fprint (
    FILE *fptr,
    FloatRect const &rect,
    bool const show_size_instead_of_top_right,
    bool add_newline)
{
    rect.Fprint(fptr, "%g", show_size_instead_of_top_right, add_newline);
}

void Fprint (
    FILE *fptr,
    Sint32Rect const &rect,
    bool const show_size_instead_of_top_right,
    bool add_newline)
{
    rect.Fprint(fptr, "%d", show_size_instead_of_top_right, add_newline);
}

} // end of namespace Xrb
