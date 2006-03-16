// ///////////////////////////////////////////////////////////////////////////
// xrb_matrix2.cpp by Victor Dods, created 2005/05/27
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_matrix2.h"

/** @file xrb_matrix2.cpp
  * Contains instances of ms_identity for each instantiation of Matrix2, as
  * well as implementations of other Matrix2-related global functions.
  * @brief implementation file for matrix2.h.
  */

namespace Xrb
{

// this template <> syntax seems to be necessary because otherwise the
// compiler doesn't think any template parameters are specified

template <> 
FloatMatrix2 const FloatMatrix2::ms_identity(
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f);

void FprintMatrix2 (FILE *fptr,
                    FloatMatrix2 const &matrix)
{
    matrix.Fprint(fptr, "%g");
}

} // end of namespace Xrb
