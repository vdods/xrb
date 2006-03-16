// ///////////////////////////////////////////////////////////////////////////
// xrb.h by Victor Dods, created 2004/06/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_H_)
#define _XRB_H_

// EVERY SOURCE FILE (H AND CPP) SHOULD INCLUDE THIS FILE
// EVERY SOURCE FILE (H AND CPP) SHOULD INCLUDE THIS FILE
// EVERY SOURCE FILE (H AND CPP) SHOULD INCLUDE THIS FILE

#if defined(HAVE_CONFIG_H)
    #include "config.h"
#endif // defined(HAVE_CONFIG_H)

#include "SDL.h"
#include "SDL_opengl.h"
#include "xrb_assert.h"
#include "xrb_debug.h"
#include "xrb_delete.h"
#include "xrb_singletons.h"

#if defined(WIN32)
    #if defined(DELETE)
        #undef DELETE
    #endif
#endif // defined(WIN32)

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

/** Contains all classes, functions, enumerations, and typedefs that libxrb
  * uses, with the exception of a few things (notably, mathematical operator
  * overloads for things like Vector and Matrix2).
  * @brief Container namespace for the XuqRijBuh game engine.
  */
namespace Xrb
{

// for using floats or doubles
typedef float Float;

/** If @c x @c == @c y, @c y is preferred.
  * @brief Macro to choose the minimum of two values.
  */
template <typename T>
inline T Min (T const x, T const y)
{
    return (x < y) ? x : y;
}

/** If @c x @c == @c y, @c y is preferred.
  * @brief Macro to choose the maximum of two values.
  */
template <typename T>
inline T Max (T const x, T const y)
{
    return (x > y) ? x : y;
}

/** Iff @c x is within @c epsilon of @c y, this evaluates to true.
  * @brief Near-equality checking.
  */
template <typename T>
inline bool Epsilon (T const x, T const y, T const epsilon)
{
    return x - y >= -epsilon && x - y <= epsilon;
}

/** @brief Does a static_cast, but ASSERT1s that the analogous
  * dynamic_cast is not NULL.
  */
template <typename CastToType, typename CastFromType>
inline CastToType DStaticCast (CastFromType cast_from)
{
    ASSERT1(cast_from == NULL || dynamic_cast<CastToType>(cast_from) != NULL)
    return static_cast<CastToType>(cast_from);
}

} // end of namespace Xrb

/** Iff @c x is true, evaluates to the ASCII string "true", otherwise "false".
  * @brief Handy boolean-to-string macro.
  */
#define BOOL_TO_STRING(x) ((x) ? "true" : "false")
/** @brief Convenience macro which preprocesses the source-code identifier into
  *        an ASCII immutable string constant.
  */
#define STRINGIFY(identifier) #identifier

#endif // !defined(_XRB_H_)

