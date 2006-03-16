// ///////////////////////////////////////////////////////////////////////////
// xrb_vector.cpp by Victor Dods, created 2005/04/22
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_vector.h"

/** @file xrb_vector.cpp
  * Contains instances of ms_zero for each instantiation of Vector, as
  * well as implementations of other Vector-related global functions.
  * @brief implementation file for vector.h.
  */

namespace Xrb
{

// this template <> syntax seems to be necessary because otherwise the
// compiler doesn't think any template parameters are specified

template <>  
FloatVector2 const FloatVector2::ms_zero(0.0, 0.0);
template <>  
Uint32Vector2 const Uint32Vector2::ms_zero(0, 0);
template <>  
Sint32Vector2 const Sint32Vector2::ms_zero(0, 0);

void FprintVector (FILE *const fptr,
                   FloatVector2 const &vector,
                   bool const add_newline)
{
    vector.Fprint(fptr, "%g", add_newline);
}

void FprintVector (FILE *const fptr,
                   Uint32Vector2 const &vector,
                   bool const add_newline)
{
    vector.Fprint(fptr, "%u", add_newline);
}

void FprintVector (FILE *const fptr,
                   Sint32Vector2 const &vector,
                   bool const add_newline)
{
    vector.Fprint(fptr, "%d", add_newline);
}

} // end of namespace Xrb
