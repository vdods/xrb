// ///////////////////////////////////////////////////////////////////////////
// xrb_delete.h by Victor Dods, created 2005/06/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_DELETE_H_)
#define _XRB_DELETE_H_

// don't include xrb.h here, because xrb.h includes this file.

#include "xrb_assert.h"

#if !defined(NULL)
    #define NULL static_cast<void *>(0)
#endif // !defined(NULL)

namespace Xrb
{

/** In debug builds, the pointer is checked for the special "bad" value
  * 0xFBADFACE, deleted, then assigned the special hex value 0xFBADFACE,
  * ensuring that double-deletions are caught, and that it's obvious what's
  * been deleted while inspecting the values of pointers in a debugger.
  * @todo Add memory stomping to this.
  * @brief Performs @c delete on the given pointer with a bit of extra
  *        debugging goodness for safety.
  */
template <typename T>
inline void Delete (T *&pointer)
{
    ASSERT1(
        pointer != reinterpret_cast<T *>(0xFBADFACE) &&
        "This pointer has already been deleted")
    delete pointer;
#if DEBUG_LEVEL > 0
    pointer = reinterpret_cast<T *>(0xFBADFACE);
#endif // DEBUG_LEVEL > 0
}

/** In debug builds, the pointer is checked for the special "bad" value
  * 0xFBADFACE, deleted, then assigned the special hex value 0xFBADFACE,
  * ensuring that double-deletions are caught, and that it's obvious what's
  * been deleted while inspecting the values of pointers in a debugger.
  * @todo Add memory stomping to this.
  * @brief Performs @c delete on the given pointer-to-a-const-value with a
  *        bit of extra debugging goodness for safety.
  */
template <typename T>
inline void Delete (T const *&pointer)
{
    ASSERT1(
        pointer != reinterpret_cast<T const *>(0xFBADFACE) &&
        "This pointer has already been deleted")
    delete pointer;
#if DEBUG_LEVEL > 0
    pointer = reinterpret_cast<T const *>(0xFBADFACE);
#endif // DEBUG_LEVEL > 0
}

/** In debug builds, the pointer is checked for the special "bad" value
  * 0xFBADFACE, delete[]d, then assigned the special hex value 0xFBADFACE,
  * ensuring that double-deletions are caught, and that it's obvious what's
  * been deleted while inspecting the values of pointers in a debugger.
  * @todo Add memory stomping to this.
  * @brief Performs @c delete[] on the given pointer with a bit of extra
  *        debugging goodness for safety.
  */
template <typename T>
inline void DeleteArray (T *&pointer)
{
    ASSERT1(
        pointer != reinterpret_cast<T *>(0xFBADFACE) &&
        "This pointer has already been deleted")
    delete[] pointer;
#if DEBUG_LEVEL > 0
    pointer = reinterpret_cast<T *>(0xFBADFACE);
#endif // DEBUG_LEVEL > 0
}

/** In debug builds, the pointer is checked for the special "bad" value
  * 0xFBADFACE, delete[]d, then assigned the special hex value 0xFBADFACE,
  * ensuring that double-deletions are caught, and that it's obvious what's
  * been deleted while inspecting the values of pointers in a debugger.
  * @todo Add memory stomping to this.
  * @brief Performs @c delete[] on the given pointer-to-a-const-value with a
  *        bit of extra debugging goodness for safety.
  */
template <typename T>
inline void DeleteArray (T const *&pointer)
{
    ASSERT1(
        pointer != reinterpret_cast<T const *>(0xFBADFACE) &&
        "This pointer has already been deleted")
    delete[] pointer;
#if DEBUG_LEVEL > 0
    pointer = reinterpret_cast<T const *>(0xFBADFACE);
#endif // DEBUG_LEVEL > 0
}

/** This deletion function should be used when the pointer needs to be reset
  * to NULL because NULL indicates some sentinel condition.
  * @todo Add memory stomping to this.
  * @brief Performs @c delete on the given pointer and then assigns @c NULL
  *        to the pointer.
  */
template <typename T>
inline void DeleteAndNullify (T *&pointer)
{
    delete pointer;
    pointer = NULL;
}

/** This deletion function should be used when the pointer needs to be reset
  * to NULL because NULL indicates some sentinel condition.
  * @todo Add memory stomping to this.
  * @brief Performs @c delete on the given pointer-to-a-const-value and then
  *        assigns @c NULL to the pointer.
  */
template <typename T>
inline void DeleteAndNullify (T const *&pointer)
{
    delete pointer;
    pointer = NULL;
}

/** This deletion function should be used when the pointer needs to be reset
  * to NULL because NULL indicates some sentinel condition.
  * @todo Add memory stomping to this.
  * @brief Performs @c delete[] on the given pointer and then assigns @c NULL
  *        to the pointer.
  */
template <typename T>
inline void DeleteArrayAndNullify (T *&pointer)
{
    delete[] pointer;
    pointer = NULL;
}

/** This deletion function should be used when the pointer needs to be reset
  * to NULL because NULL indicates some sentinel condition.
  * @todo Add memory stomping to this.
  * @brief Performs @c delete[] on the given pointer-to-a-const-value and then
  *        assigns @c NULL to the pointer.
  */
template <typename T>
inline void DeleteArrayAndNullify (T const *&pointer)
{
    delete[] pointer;
    pointer = NULL;
}

} // end of namespace Xrb

#endif // !defined(_XRB_DELETE_H_)

