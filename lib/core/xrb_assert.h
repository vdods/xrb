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

// assert macro which is ALWAYS compiled in
#define ASSERT0(x) { assert(x); }

// normal assert macro, should be used generously in non-speed-critical code
#if DEBUG_LEVEL >= 1
    #define ASSERT1(x) { assert(x); }
#else // DEBUG_LEVEL < 1
    #define ASSERT1(x) { }
#endif // DEBUG_LEVEL < 1

// pedantic assert macro (asserts which are there just for correctness' sake,
// but are extremely unlikely to fail).  to be used in speed-critical code
// sections.
#if DEBUG_LEVEL >= 2
    #define ASSERT2(x) { assert(x); }
#else // DEBUG_LEVEL < 2
    #define ASSERT2(x) { }
#endif // DEBUG_LEVEL < 2

// EXTREMELY pedantic assert macro (asserts which are there just for
// correctness' sake, but are extremely fucking unlikely to fail).  to be used
// in the most speed-critical code sections.
#if DEBUG_LEVEL >= 3
    #define ASSERT3(x) { assert(x); }
#else // DEBUG_LEVEL < 3
    #define ASSERT3(x) { }
#endif // DEBUG_LEVEL < 3

// assert which is used in Engine2::Entity code for debugging NaNs.
#if defined(XRB_NAN_SANITY_CHECK)
    #define ASSERT_NAN_SANITY_CHECK(x) { assert(x); }
#else // !defined(XRB_NAN_SANITY_CHECK)
    #define ASSERT_NAN_SANITY_CHECK(x) { }
#endif // !defined(XRB_NAN_SANITY_CHECK)

#endif // !defined(_XRB_ASSERT_H_)

