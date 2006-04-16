// ///////////////////////////////////////////////////////////////////////////
// xrb_compiletimeasserts.cpp by Victor Dods, created 2006/04/16
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb.h"

namespace Xrb
{

GLOBAL_SCOPE_COMPILE_TIME_ASSERT(SIZEOF_CHAR_CHECK, sizeof(char) == 1)
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(SIZEOF_SINT8_CHECK, sizeof(Sint8) == 1)
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(SIZEOF_UINT8_CHECK, sizeof(Uint8) == 1)
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(SIZEOF_SINT16_CHECK, sizeof(Sint16) == 2)
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(SIZEOF_UINT16_CHECK, sizeof(Uint16) == 2)
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(SIZEOF_SINT32_CHECK, sizeof(Sint32) == 4)
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(SIZEOF_UINT32_CHECK, sizeof(Uint32) == 4)
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(SIZEOF_FLOAT_CHECK, sizeof(float) == 4)
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(SIZEOF_DOUBLE_CHECK, sizeof(double) == 8)

} // end of namespace Xrb
