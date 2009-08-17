// ///////////////////////////////////////////////////////////////////////////
// xrb_types.hpp by Victor Dods, created 2006/04/10
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_TYPES_HPP_)
#define _XRB_TYPES_HPP_

// don't include xrb.h here, because xrb.h includes this file.

// lower/upper bound on Sint8 values
#define SINT8_LOWER_BOUND static_cast<Xrb::Sint8>(0x80)
#define SINT8_UPPER_BOUND static_cast<Xrb::Sint8>(0x7F)
// lower/upper bound on Uint8 values
#define UINT8_LOWER_BOUND static_cast<Xrb::Uint8>(0x00)
#define UINT8_UPPER_BOUND static_cast<Xrb::Uint8>(0xFF)
// lower/upper bound on Sint16 values
#define SINT16_LOWER_BOUND static_cast<Xrb::Sint16>(0x8000)
#define SINT16_UPPER_BOUND static_cast<Xrb::Sint16>(0x7FFF)
// lower/upper bound on Uint16 values
#define UINT16_LOWER_BOUND static_cast<Xrb::Uint16>(0x0000)
#define UINT16_UPPER_BOUND static_cast<Xrb::Uint16>(0xFFFF)
// lower/upper bound on Sint32 values
#define SINT32_LOWER_BOUND static_cast<Xrb::Sint32>(0x80000000)
#define SINT32_UPPER_BOUND static_cast<Xrb::Sint32>(0x7FFFFFFF)
// lower/upper bound on Uint32 values
#define UINT32_LOWER_BOUND static_cast<Xrb::Uint32>(0x00000000)
#define UINT32_UPPER_BOUND static_cast<Xrb::Uint32>(0xFFFFFFFF)

namespace Xrb
{

// these types are predicated on the SDL integer types.  the correctness
// of their sizes is checked in xrb_compiletimeasserts.cpp
typedef char            Sint8;
typedef unsigned char   Uint8;
typedef short           Sint16;
typedef unsigned short  Uint16;
typedef int             Sint32;
typedef unsigned int    Uint32;

// for using floats or doubles
typedef float Float;

// StateMachine input value typedef
typedef Uint8 StateMachineInput;

} // end of namespace Xrb

#endif // !defined(_XRB_TYPES_HPP_)

