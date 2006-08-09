// ///////////////////////////////////////////////////////////////////////////
// xrb_assert.h by Victor Dods, created 2005/06/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ASSERT_H_)
#define _XRB_ASSERT_H_

// don't include xrb.h here, because xrb.h includes this file.

#include "xrb_debug.h"

#include <assert.h>

// ///////////////////////////////////////////////////////////////////////////
// compile-time assert macro
// ///////////////////////////////////////////////////////////////////////////

/// @cond IGNORE_THIS
template <bool condition> struct ThisCompileErrorIsActuallyAFailedCompileTimeAssert;

template <>
struct ThisCompileErrorIsActuallyAFailedCompileTimeAssert<true>
{
    enum { BLAH };
};
/// @endcond

// this assert is intended to be used in the global scope and will produce a
// compile error on conditions that are decidable during compilation
// (e.g. "sizeof(char) == 1").  each call to this macro must supply an assert
// name that is unique to that source file (e.g. CHAR_SIZE_CHECK).
#define GLOBAL_SCOPE_COMPILE_TIME_ASSERT(assert_name, x) \
namespace { inline int assert_name () { return ThisCompileErrorIsActuallyAFailedCompileTimeAssert<static_cast<bool>(x)>::BLAH; } }

// this assert is intended to be used within the body of a function/method
// and will produce a compile error on conditions that are decidable during
// compilation (e.g. "sizeof(something) == 4").
#define CODE_SCOPE_COMPILE_TIME_ASSERT(x) \
ThisCompileErrorIsActuallyAFailedCompileTimeAssert<static_cast<bool>(x)>::BLAH;

// ///////////////////////////////////////////////////////////////////////////
// run-time assert macros
// ///////////////////////////////////////////////////////////////////////////

// assert macro which is ALWAYS compiled in
#define ASSERT0(x) { assert(x); }

// normal assert macro, should be used generously in non-speed-critical code
#if XRB_DEBUG_LEVEL >= 1
    #define ASSERT1(x) { assert(x); }
#else // XRB_DEBUG_LEVEL < 1
    #define ASSERT1(x) { }
#endif // XRB_DEBUG_LEVEL < 1

// pedantic assert macro (asserts which are there just for correctness' sake,
// but are extremely unlikely to fail).  to be used in speed-critical code
// sections.
#if XRB_DEBUG_LEVEL >= 2
    #define ASSERT2(x) { assert(x); }
#else // XRB_DEBUG_LEVEL < 2
    #define ASSERT2(x) { }
#endif // XRB_DEBUG_LEVEL < 2

// EXTREMELY pedantic assert macro (asserts which are there just for
// correctness' sake, but are extremely fucking unlikely to fail).  to be used
// in the most speed-critical code sections.
#if XRB_DEBUG_LEVEL >= 3
    #define ASSERT3(x) { assert(x); }
#else // XRB_DEBUG_LEVEL < 3
    #define ASSERT3(x) { }
#endif // XRB_DEBUG_LEVEL < 3

// assert which is used in Engine2::Entity code for debugging NaNs.
#if defined(XRB_NAN_SANITY_CHECK)
    #define ASSERT_NAN_SANITY_CHECK(x) { assert(x); }
#else // !defined(XRB_NAN_SANITY_CHECK)
    #define ASSERT_NAN_SANITY_CHECK(x) { }
#endif // !defined(XRB_NAN_SANITY_CHECK)

#endif // !defined(_XRB_ASSERT_H_)

