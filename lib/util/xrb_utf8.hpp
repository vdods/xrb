// ///////////////////////////////////////////////////////////////////////////
// xrb_utf8.hpp by Victor Dods, created 2006/06/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_UTF8_HPP_)
#define _XRB_UTF8_HPP_

#include "xrb.hpp"

#include <string>

namespace Xrb
{

/** @brief Contains functions for manipulating UTF8-encoded strings.
  */
namespace UTF8
{

    /** Because a UTF8-encdoded character can be made up of a sequence of
      * bytes, this function provides a convenient way to "increment the
      * string pointer" (to get the next character in the string).  If
      * @c current points to the end of the string (the NULL char), then
      * the value @c current is returned.
      * @brief Returns the next character in the UTF8-encdoded string.
      * @param current A pointer to the character to be skipped over.  May not
      *                be NULL.
      * @note Returns @c current if the first byte of the sequence is
      *       not well-formed (does not do full well-formedness checking).
      */
    char const *NextCharacter (char const *current);
    /** If either @c c0 or @c c1 are NULL or ill-formed, return false.
      * @brief Returns true if the given UTF8-encoded characters are equal.
      */
    bool AreCharactersEqual (char const *c0, char const *c1);
    /** @brief Returns the 32-bit unicode for the given UTF8-encoded character.
      * @param sequence A pointer to the character to be decoded.  May not be NULL.
      * @note Returns 0xFFFFFFFF if the character is not well-formed.
      */
    Uint32 GetUnicode (char const *sequence);
    /** If @c unicode is not a valid Unicode scalar, nothing will be appended
      * to @c *dest .
      * @brief UTF8-encodes the given unicode character to @c dest (by appending).
      */
    void AppendSequence (std::string *dest, Uint32 unicode);

} // end of namespace UTF8

} // end of namespace Xrb

#endif // !defined(_XRB_UTF8_HPP_)

