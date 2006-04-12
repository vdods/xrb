// ///////////////////////////////////////////////////////////////////////////
// xrb_types.h by Victor Dods, created 2006/04/10
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_TYPES_H_)
#define _XRB_TYPES_H_

// don't include xrb.h here, because xrb.h includes this file.

// lower/upper bound on Sint8 values
#define SINT8_LOWER_BOUND static_cast<Sint8>(0x80)
#define SINT8_UPPER_BOUND static_cast<Sint8>(0x7F)
// lower/upper bound on Uint8 values
#define UINT8_LOWER_BOUND static_cast<Uint8>(0x00)
#define UINT8_UPPER_BOUND static_cast<Uint8>(0xFF)
// lower/upper bound on Sint16 values
#define SINT16_LOWER_BOUND static_cast<Sint16>(0x8000)
#define SINT16_UPPER_BOUND static_cast<Sint16>(0x7FFF)
// lower/upper bound on Uint16 values
#define UINT16_LOWER_BOUND static_cast<Uint16>(0x0000)
#define UINT16_UPPER_BOUND static_cast<Uint16>(0xFFFF)
// lower/upper bound on Sint32 values
#define SINT32_LOWER_BOUND static_cast<Sint32>(0x80000000)
#define SINT32_UPPER_BOUND static_cast<Sint32>(0x7FFFFFFF)
// lower/upper bound on Uint32 values
#define UINT32_LOWER_BOUND static_cast<Uint32>(0x00000000)
#define UINT32_UPPER_BOUND static_cast<Uint32>(0xFFFFFFFF)

namespace Xrb
{

// for using floats or doubles
typedef float Float;

// StateMachine input value typedef
typedef Uint8 StateMachineInput;

} // end of namespace Xrb

#endif // !defined(_XRB_TYPES_H_)

