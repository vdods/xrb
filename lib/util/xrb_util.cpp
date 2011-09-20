// ///////////////////////////////////////////////////////////////////////////
// xrb_util.cpp by Victor Dods, created 2005/02/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_util.hpp"

#include <stdarg.h>
#include <string.h>

#include "xrb_emptystring.hpp"

namespace Xrb
{

void Util::ReplaceAllInString (
    std::string *string,
    std::string const &string_to_replace,
    std::string const &string_to_replace_with)
{
    ASSERT1(string != NULL);
    ASSERT1(string_to_replace.length() > 0);
    Uint32 offset = 0;
    while (1)
    {
        Uint32 pos = static_cast<Uint32>(string->find(string_to_replace, offset));
        if (pos >= string->length())
            break;
        string->replace(pos, string_to_replace.length(), string_to_replace_with);
        offset = pos + string_to_replace_with.length();
    }
}

std::string Util::StringPrintf (char const *format, ...)
{
    ASSERT1(format != NULL);

    // temp buffer stuff until i figure out a better way
    char buffer[0x1000];
    va_list list;

    va_start(list, format);
    vsnprintf(buffer, 0x1000, format, list);
    va_end(list);

    return std::string(buffer);
}

void StringPrintf (std::string *const string, char const *const format, ...)
{
    ASSERT1(string != NULL);
    ASSERT1(format != NULL);

    // temp buffer stuff until i figure out a better way
    char buffer[0x1000];
    va_list list;

    va_start(list, format);
    vsnprintf(buffer, 0x1000, format, list);
    va_end(list);

    *string = buffer;
}
/*
void Util::StringCopy (char *destination, char const *source, Uint32 destination_size)
{
    ASSERT1(destination != NULL);
    ASSERT1(source != NULL);

    strncpy(destination, source, destination_size);
    destination[destination_size-1] = '\0';
}
*/
char *Util::StringDuplicate (char const *string_to_duplicate)
{
    ASSERT1(string_to_duplicate != NULL);

    Uint32 length = strlen(string_to_duplicate);
    char *duplicated_string = new char[length + 1];
    memcpy(duplicated_string, string_to_duplicate, length + 1);
    ASSERT1(duplicated_string[length] == '\0');

    return duplicated_string;
}

char Util::Lowercase (char c)
{
    if (c >= 'A' && c <= 'Z')
        return c += 'a' - 'A';
    else
        return c;
}

char Util::Uppercase (char c)
{
    if (c >= 'a' && c <= 'z')
        return c += 'A' - 'a';
    else
        return c;
}

std::string Util::Lowercase (std::string const &str)
{
    std::string retval(str);
    MakeLowercase(retval);
    return retval;
}

std::string Util::Uppercase (std::string const &str)
{
    std::string retval(str);
    MakeUppercase(retval);
    return retval;
}

void Util::MakeLowercase (std::string &str)
{
    for (Uint32 i = 0; i < str.length(); ++i)
        if (str[i] >= 'A' && str[i] <= 'Z')
            str[i] += 'a' - 'A';
}

void Util::MakeUppercase (std::string &str)
{
    for (Uint32 i = 0; i < str.length(); ++i)
        if (str[i] >= 'a' && str[i] <= 'z')
            str[i] += 'A' - 'a';
}

char Util::ShiftedAscii (char c)
{
    static char const ascii_to_shifted_ascii[128] =
    {
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
        '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
         ' ',  '!', '\'',  '#',  '$',  '%',  '&',  '"',
         '(',  ')',  '*',  '+',  '<',  '_',  '>',  '?',
         ')',  '!',  '@',  '#',  '$',  '%',  '^',  '&',
         '*',  '(',  ':',  ':',  '<',  '+',  '>',  '?',
         '@',  'a',  'b',  'c',  'd',  'e',  'f',  'g',
         'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',
         'p',  'q',  'r',  's',  't',  'u',  'v',  'w',
         'x',  'y',  'z',  '{',  '|',  '}',  '^',  '_',
         '~',  'A',  'B',  'C',  'D',  'E',  'F',  'G',
         'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O',
         'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',
         'X',  'Y',  'Z',  '{',  '|',  '}',  '~', '\0',
    };

    return (c >= 0) ? ascii_to_shifted_ascii[(Uint32)c] : '\0';
}

bool Util::CharacterLiteralCharNeedsEscaping (char c)
{
    // TODO: make lookup table? (decide what to do for non-ascii chars)

    // '\a' '\b' '\t' '\n' '\v' '\f' and '\r' are all contiguous in ASCII
    return (c >= '\a' && c <= '\r') || c == '\0' || c == '\n' || c == '\\' || c == '\'';
}

bool Util::StringLiteralCharNeedsEscaping (char c)
{
    // TODO: make lookup table? (decide what to do for non-ascii chars)

    // '\a' '\b' '\t' '\n' '\v' '\f' and '\r' are all contiguous in ASCII
    return (c >= '\a' && c <= '\r') || c == '\0' || c == '\n' || c == '\\' || c == '\"';
}

char Util::EscapeCode (char c)
{
    // TODO: make lookup table? (decide what to do for non-ascii chars)

    switch (c)
    {
        case '\0': return '0';
        case '\a': return 'a';
        case '\b': return 'b';
        case '\t': return 't';
        case '\n': return 'n';
        case '\v': return 'v';
        case '\f': return 'f';
        case '\r': return 'r';
        default  : return c;
    }
}

char Util::EscapedChar (char c)
{
    // TODO: make lookup table? (decide what to do for non-ascii chars)

    switch (c)
    {
        case '0': return '\0';
        case 'a': return '\a';
        case 'b': return '\b';
        case 't': return '\t';
        case 'n': return '\n';
        case 'v': return '\v';
        case 'f': return '\f';
        case 'r': return '\r';
        default : return c;
    }
}

std::string Util::CharacterLiteral (char c)
{
    std::string retval("'");
    if (CharacterLiteralCharNeedsEscaping(c))
        retval += '\\', retval += EscapeCode(c);
    else
        retval += c;
    retval += '\'';
    return retval;
}

std::string Util::StringLiteral (std::string const &text)
{
    std::string retval("\"");
    for (std::string::const_iterator it = text.begin(),
                                     it_end = text.end();
         it != it_end;
         ++it)
    {
        if (StringLiteralCharNeedsEscaping(*it))
            retval += '\\', retval += EscapeCode(*it);
        else
            retval += *it;
    }
    retval += '"';
    return retval;
}

std::string Util::DirectoryPortion (std::string const &path)
{
    std::string::size_type last_slash = path.find_last_of("/");
    if (last_slash == std::string::npos)
        return g_empty_string;
    else
        return path.substr(0, last_slash+1);
}

std::string Util::FilenamePortion (std::string const &path)
{
    std::string::size_type last_slash = path.find_last_of("/");
    if (last_slash == std::string::npos)
        return path;
    else
        return path.substr(last_slash+1);
}

} // end of namespace Xrb
