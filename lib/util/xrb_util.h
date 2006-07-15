// ///////////////////////////////////////////////////////////////////////////
// xrb_util.h by Victor Dods, created 2005/02/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_UTIL_H_)
#define _XRB_UTIL_H_

#include "xrb.h"

#include <string>

#include "xrb_enums.h"

namespace Xrb
{

/** These functions are more or less for convenience, and don't provide any
  * significant algorithmic computation.
  * @brief Contains various utility functions.
  */
namespace Util
{
    /** @brief Find and replace all instances of @c string_to_replace inside
      *        @c string, using @c string_to_replace_with.
      */
    void ReplaceAllInString (
        std::string *string,
        std::string const &string_to_replace,
        std::string const &string_to_replace_with);
    /** @brief Performs printf into an STL string and returns it.
      */
    std::string StringPrintf (char const *format, ...);
    /** @brief Performs printf into the provided STL string.
      */
    void StringPrintf (std::string *string, char const *format, ...);
    /*
    void StringCopy (
        char *destination,
        char const *source,
        Uint32 destination_size);
    */
    /** The returned string is a NULL-terminated ASCII string, of exactly
      * the length of @c string_to_duplicate.
      * @brief Returns a newly allocated copy of the given string.
      */
    char *StringDuplicate (char const *string_to_duplicate);
    /** @brief Converts all alphabetic characters in the string to lowercase.
      */
    void MakeLowercase (std::string *str);
    /** @brief Converts all alphabetic characters in the string to uppercase.
      */
    void MakeUppercase (std::string *str);
    /** @brief Returns the ASCII code of the given character as it would be
      *        interpreted with the SHIFT key held down.
      */
    char GetShiftedAscii (char c);

    /** @brief Returns true iff the given character needs to be escaped to be
      *        properly represented in a quoted string.
      */
    bool GetDoesCharacterNeedEscaping (char c);
    /** For example, GetEscapedCharacter('\0') returns '0'.
      *
      * This function returns the given character if it is not an
      * escapable character.
      *
      * @brief Returns the escape character code for escapable characters.
      */
    char GetEscapedCharacter (char c);
    /** For example, GetEscapedCharacter('0') returns '\0'.
      *
      * This function returns the given character if it is not an
      * escapable character.
      *
      * @brief Returns the escaped character for the escaped character codes.
      */
    char GetEscapedCharacterBase (char c);
    /** For example, GetEscapedCharacterString('\n') returns the ascii string
      * "\\n" (a backslash followed by a lowercase 'n').  if a character does
      * not need to be escaped, then a string containing that single character
      * will be returned.
      * @brief Returns the string-literal representation of the given character.
      */
    std::string GetEscapedCharacterString (char c);
    /** @brief Runs the string's characters through GetEscapedCharacterString.
      */
    std::string GetEscapedString (std::string const &str);

    /** @brief Returns the signed, base 10 integer value parsed from the
      *        given text.
      */
    inline Sint32 TextToSint32 (char const *const text)
    {
        return static_cast<Sint32>(strtol(text, NULL, 10));
    }
    /** @brief Returns the unsigned, base 10 integer value parsed from
      *        the given text.
      */
    inline Uint32 TextToUint32 (char const *const text)
    {
        return static_cast<Uint32>(strtoul(text, NULL, 10));
    }
    /** @brief Returns the floating point value parsed from the given text.
      */
    inline Float TextToFloat (char const *const text)
    {
    #if defined(WIN32)
        return static_cast<Float>(strtod(text, NULL));
    #else
        return strtof(text, NULL);
    #endif
    }

    /** @brief Returns textual representations of the IOError enums.
      */
    char const *GetIOErrorString (IOError error);
} // end of namespace Util

} // end of namespace Xrb

#endif // !defined(_XRB_UTIL_H_)

