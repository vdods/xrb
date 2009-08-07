// ///////////////////////////////////////////////////////////////////////////
// xrb.hpp by Victor Dods, created 2004/06/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_HPP_)
#define _XRB_HPP_

// EVERY SOURCE FILE (H AND CPP) SHOULD INCLUDE THIS FILE
// EVERY SOURCE FILE (H AND CPP) SHOULD INCLUDE THIS FILE
// EVERY SOURCE FILE (H AND CPP) SHOULD INCLUDE THIS FILE

#if defined(HAVE_CONFIG_H)
    #include "config.h"
#endif // defined(HAVE_CONFIG_H)

#include "SDL.h"
#include "SDL_opengl.h"
#include "xrb_assert.hpp"
#include "xrb_debug.hpp"
#include "xrb_delete.hpp"
#include "xrb_singleton.hpp"
#include "xrb_types.hpp"

#if defined(WIN32)
    #if defined(DELETE)
        #undef DELETE
    #endif
    #if defined(ERROR)
        #undef ERROR
    #endif
#endif // defined(WIN32)

/** Contains all classes, functions, enumerations, and typedefs that libxrb
  * uses, with the exception of a few things (notably, mathematical operator
  * overloads for things like Vector and Matrix2).
  * @brief Container namespace for the XuqRijBuh game engine.
  */
namespace Xrb
{

/** The template type @c T must allow static_cast-ing from the integer value
  * 0, and must have a less-than operator defined.
  * @brief Returns the absolute value of @c x.
  */
template <typename T>
inline T Abs (T const &x)
{
    return (x < static_cast<T>(0)) ? -x : x;
}

/** The template type @c T must have a less-than operator defined.
  * @brief Returns the minimum of two values.
  */
template <typename T>
inline T const &Min (T const &x, T const &y)
{
    return (x < y) ? x : y;
}

/** The template type @c T must have a less-than operator defined.
  * @brief Returns the maximum of two values.
  */
template <typename T>
inline T const &Max (T const &x, T const &y)
{
    return (x < y) ? y : x;
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
    ASSERT1(cast_from == NULL || dynamic_cast<CastToType>(cast_from) != NULL);
    return static_cast<CastToType>(cast_from);
}

/** This function is used to avoid placing the ugliness in code at large.
  * @brief Convenience function for erasing the end of a container
  * whose erase method does not accept reverse_iterator
  * @param container The STL container (e.g. set, map, etc) for which
  *                  to remove its rbegin().
  */
template <typename ContainerType>
inline void StlContainerEraseRBegin (ContainerType &container)
{
    ASSERT1(!container.empty());
    container.erase(typename ContainerType::iterator((++container.rbegin()).base()));
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

#endif // !defined(_XRB_HPP_)

