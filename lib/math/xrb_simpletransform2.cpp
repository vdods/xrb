// ///////////////////////////////////////////////////////////////////////////
// xrb_simpletransform2.cpp by Victor Dods, created 2005/05/27
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_simpletransform2.h"

/** @file xrb_simpletransform2.cpp
  * Contains instances of ms_identity for each instantiation of
  * SimpleTransform2, as well as implementations of other
  * SimpleTransform2-related global functions.
  * @brief implementation file for xrb_simpletransform2.h.
  */

namespace Xrb
{

void FprintSimpleTransform2 (
    FILE *fptr,
    FloatSimpleTransform2 const &simple_transform)
{
    simple_transform.Fprint(fptr, "%g");
}

} // end of namespace Xrb
