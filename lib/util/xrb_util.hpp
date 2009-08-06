// ///////////////////////////////////////////////////////////////////////////
// xrb_util.hpp by Victor Dods, created 2005/02/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_UTIL_HPP_)
#define _XRB_UTIL_HPP_

#include "xrb.hpp"

#include <string>

#include "xrb_enums.hpp"

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
    /** @brief If c is an uppercase letter, returns it in lowercase, otherwise returns c.
      * @param c The character to convert to lowercase.
      */
    char Lowercase (char c);
    /** @brief If c is a lowercase letter, returns it in uppercase, otherwise returns c.
      * @param c The character to convert to uppercase.
      */
    char Uppercase (char c);
    /** @brief Converts all alphabetic characters in the string to lowercase.
      */
    void MakeLowercase (std::string *str);
    /** @brief Converts all alphabetic characters in the string to uppercase.
      */
    void MakeUppercase (std::string *str);
    /** @brief Returns the ASCII code of the given character as it would be
      *        interpreted with the SHIFT key held down.
      */
    char ShiftedAscii (char c);

    /** @brief Returns true iff the given character needs to be escaped to be
      *        properly represented in a C-style character literal.
      */
    bool CharacterLiteralCharNeedsEscaping (char c);
    /** @brief Returns true iff the given character needs to be escaped to be
      *        properly represented in a C-style string literal.
      */
    bool StringLiteralCharNeedsEscaping (char c);
    /** For example, EscapeCode('\0') returns '0'.
      *
      * This function returns the given character if it is not an
      * escapable character (e.g. EscapeCode('j') returns 'j'.
      *
      * EscapedChar(EscapeCode(c)) should return c.
      *
      * @brief Returns the escape character code for escaped characters.
      */
    char EscapeCode (char c);
    /** For example, EscapedChar('0') returns '\0'.
      *
      * This function returns the given character if it is not an
      * escapable character (e.g. EscapedChar('h') returns 'h'.
      *
      * EscapeCode(EscapedChar(c)) should return c.
      *
      * @brief Returns the escaped character for the given escape code.
      */
    char EscapedChar (char c);

    /** For example, CharacterLiteral('\n') returns std::string("'\\n'"),
      * while CharacterLiteral('j') returns std::string("'j'").
      * @brief Returns the single-quote-surrounded C-style character literal
               for the given character in the form of a std::string.
      */
    std::string CharacterLiteral (char c);
    /** For example, StringLiteral("eat shit\t\"and die\".")
      * returns std::string("\"eat shit\\t\\\"and die\\\".\"").
      * @brief Returns the double-quote-surrounded C-style string literal
      *        for the given text in the form of a std::string.
      */
    std::string StringLiteral (std::string const &text);

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
    /** The extra parameter will return a pointer to the portion of the string
      * after the numeric value.
      * @brief Returns the floating point value parsed from the given text.
      */
    inline Float TextToFloat (char const *const text, char const **const end)
    {
        // the const_cast BS is because strtof/d's second parameter type is wrong
    #if defined(WIN32)
        return static_cast<Float>(strtod(text, const_cast<char **>(end)));
    #else
        return strtof(text, const_cast<char **>(end));
    #endif
    }

    /** @brief Returns textual representations of the IOError enums.
      */
    char const *IOErrorString (IOError error);
} // end of namespace Util

} // end of namespace Xrb

#endif // !defined(_XRB_UTIL_HPP_)

