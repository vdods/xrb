// ///////////////////////////////////////////////////////////////////////////
// xrb_debug.h by Victor Dods, created 2005/07/03
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_DEBUG_H_)
#define _XRB_DEBUG_H_

// don't include xrb.h here, because xrb.h includes this file.

// debugging level (determines which asserts are compiled in/out)
#if !defined(DEBUG_LEVEL)
    #define DEBUG_LEVEL 3
#endif // !defined(DEBUG_LEVEL)

// make sure NDEBUG is not defined (used to compile out assert())
#if defined(NDEBUG)
    #undef NDEBUG
#endif // defined(NDEBUG)

// macro which allows an expression to easily be compiled out
// for non debug builds (for DEBUG_LEVEL < 1)
#if DEBUG_LEVEL >= 1
    #define DEBUG_EXPRESSION1(x) x;
#else // DEBUG_LEVEL < 1
    #define DEBUG_EXPRESSION1(x) ;
#endif // DEBUG_LEVEL < 1

// macro which allows an expression to easily be compiled out
// for non debug builds (for DEBUG_LEVEL < 3)
#if DEBUG_LEVEL >= 2
    #define DEBUG_EXPRESSION2(x) x;
#else // DEBUG_LEVEL < 2
    #define DEBUG_EXPRESSION2(x) ;
#endif // DEBUG_LEVEL < 2

// macro which allows an expression to easily be compiled out
// for non debug builds (for DEBUG_LEVEL < 3)
#if DEBUG_LEVEL >= 3
    #define DEBUG_EXPRESSION3(x) x;
#else // DEBUG_LEVEL < 3
    #define DEBUG_EXPRESSION3(x) ;
#endif // DEBUG_LEVEL < 3

#endif // !defined(_XRB_DEBUG_H_)

