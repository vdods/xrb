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

#if XRB_PLATFORM == XRB_PLATFORM_SDL
    #include "SDL_stdinc.h" // for the integer types
#else
    #include <stdint.h> // for the integer types
#endif

namespace Xrb
{

#if XRB_PLATFORM == XRB_PLATFORM_SDL
// use the SDL int types
using ::Sint8;
using ::Uint8;
using ::Sint16;
using ::Uint16;
using ::Sint32;
using ::Uint32;
#if defined(XRB_ENABLE_64BIT_INTEGER_TYPES)
using ::Sint64;
using ::Uint64;
#endif
#else
// these types are predicated on the SDL integer types.  the correctness
// of their sizes is checked in xrb_compiletimeasserts.cpp
typedef int8_t   Sint8;
typedef uint8_t  Uint8;
typedef int16_t  Sint16;
typedef uint16_t Uint16;
typedef int32_t  Sint32;
typedef uint32_t Uint32;
#if defined(XRB_ENABLE_64BIT_INTEGER_TYPES)
typedef int64_t  Sint64;
typedef uint64_t Uint64;
#endif
#endif

// for using floats or doubles
typedef float Float;

// StateMachine input value typedef
typedef Uint8 StateMachineInput;

} // end of namespace Xrb

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

#if defined(XRB_ENABLE_64BIT_INTEGER_TYPES)
// lower/upper bound on Sint64 values
#define SINT64_LOWER_BOUND static_cast<Xrb::Sint64>(0x8000000000000000LL)
#define SINT64_UPPER_BOUND static_cast<Xrb::Sint64>(0x7FFFFFFFFFFFFFFFLL)
// lower/upper bound on Uint64 values
#define UINT64_LOWER_BOUND static_cast<Xrb::Uint64>(0x0000000000000000LL)
#define UINT64_UPPER_BOUND static_cast<Xrb::Uint64>(0xFFFFFFFFFFFFFFFFLL)
#endif

#endif // !defined(_XRB_TYPES_HPP_)

