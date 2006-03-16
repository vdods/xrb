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

// this template <> syntax seems to be necessary because otherwise the
// compiler doesn't think any template parameters are specified

template <>
FloatTransform2 const FloatTransform2::ms_identity(
    Vector<Float, 2>(0.0f, 0.0f),
    Vector<Float, 2>(1.0f, 1.0f),
    0.0f,
    true); // the post-translate value for ms_identity is arbitrary
template <>
Float const FloatTransform2::ms_zero = 0.0f;
template <>
Float const FloatTransform2::ms_one = 1.0f;

void FprintTransform2 (FILE *fptr,
                       FloatTransform2 const &transform,
                       bool add_newline)
{
    transform.Fprint(fptr, "%g", add_newline);
}

} // end of namespace Xrb
