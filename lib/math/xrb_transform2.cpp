// ///////////////////////////////////////////////////////////////////////////
// xrb_transform2.cpp by Victor Dods, created 2005/05/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_transform2.h"

/** @file xrb_transform2.cpp
  * Contains instances of ms_identity, ms_zero and ms_one for each
  * instantiation of Transform2, as well as implementations of other
  * Transform2-related global functions.
  * @brief implementation file for xrb_transform2.h.
  */

namespace Xrb
{

void Fprint (FILE *fptr, FloatTransform2 const &transform, bool add_newline)
{
    transform.Fprint(fptr, "%g", add_newline);
}

} // end of namespace Xrb
