// ///////////////////////////////////////////////////////////////////////////
// xrb_linecalculator.h by Victor Dods, created 2005/07/21
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_LINECALCULATOR_H_)
#define _XRB_LINECALCULATOR_H_

#include "xrb.h"

namespace Xrb
{

/**
  * @brief Simply contains the parsing function(s) for the Line Calculator.
  */
namespace LineCalculator
{
    /** Divide-by-zero and domain errors are properly protected against,
      * though the result will be NaN (the floating point representation of
      * not-a-number).
      * @brief Parses an algebraic expression from a string, returning the
      *        calculated value as a Float.
      */
    Float Parse (char const *string);
}

} // end of namespace Xrb

#endif // !defined(_XRB_LINECALCULATOR_H_)

