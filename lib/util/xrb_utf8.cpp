// ///////////////////////////////////////////////////////////////////////////
// xrb_utf8.cpp by Victor Dods, created 2006/06/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_utf8.hpp"

namespace Xrb
{

/*
range:
[0x0,0xD7FF],[0xE000,0x10FFFF]

00000000 0xxxxxxx               0xxxxxxx
00000yyy yyxxxxxx               110yyyyy 10xxxxxx
zzzzyyyy yyxxxxxx               1110zzzz 10yyyyyy 10xxxxxx
000uuuzz zzzzyyyy yyxxxxxx      11110uuu 10zzzzzz 10yyyyyy 10xxxxxx

00-7F
C2-DF       80-BF
E0          A0-BF
E1-EC       80-BF       80-BF
ED          80-9F       80-BF
EE-EF       80-BF       80-BF
F0          90-BF       80-BF       80-BF
F1-F3       80-BF       80-BF       80-BF
F4          80-8F       80-BF       80-BF
*/

char const *UTF8::GetNextCharacter (char const *current)
{
    ASSERT1(current != NULL);

    Uint8 const *bytes = reinterpret_cast<Uint8 const *>(current);
    if (*bytes == 0x00)
        return current;
    if (*bytes < 0x80)
        return current+1;
    if ((*bytes&0xE0) == 0xC0)
        return current+2;
    if ((*bytes&0xF0) == 0xE0)
        return current+3;
    if ((*bytes&0xF8) == 0xF0)
        return current+4;

    // reaching here signifies a really ill-formed character
    return current;
}

bool UTF8::GetAreCharactersEqual (char const *const c0, char const *const c1)
{
    return c0 != NULL && c1 != NULL && GetUnicode(c0) == GetUnicode(c1);
}

Uint32 UTF8::GetUnicode (char const *sequence)
{
    ASSERT1(sequence != NULL);

    Uint8 const *bytes = reinterpret_cast<Uint8 const *>(sequence);
    // quickly dispatch regular ASCII (one-byte sequences).
    if (*bytes < 0x80)
        return static_cast<Uint32>(*bytes);

    Uint32 retval;
    if (*bytes < 0xC2)
        return 0xFFFFFFFF;
    else if (*bytes < 0xE0)
    {
        // two-byte sequences
        retval = *(bytes++)&0x1F;
        retval <<= 6;
        if (*bytes < 0x80 || *bytes > 0xBF)
            return 0xFFFFFFFF;
        retval |= *bytes&0x3F;
    }
    else if (*bytes == 0xE0)
    {
        // two-byte sequences
        retval = *(bytes++)&0x1F;
        retval <<= 6;
        if (*bytes < 0xA0 || *bytes > 0xBF)
            return 0xFFFFFFFF;
        retval |= *(bytes++)&0x3F;
        retval <<= 6;
        if (*bytes < 0x80 || *bytes > 0xBF)
            return 0xFFFFFFFF;
        retval |= *bytes&0x3F;
    }
    else if (*bytes == 0xED)
    {
        // three-byte sequences
        retval = *(bytes++)&0x0F;
        retval <<= 6;
        if (*bytes < 0x80 || *bytes > 0x9F)
            return 0xFFFFFFFF;
        retval |= *(bytes++)&0x3F;
        retval <<= 6;
        if (*bytes < 0x80 || *bytes > 0xBF)
            return 0xFFFFFFFF;
        retval |= *bytes&0x3F;
    }
    else if (*bytes < 0xF0)
    {
        // three-byte sequences
        retval = *(bytes++)&0x0F;
        retval <<= 6;
        if (*bytes < 0x80 || *bytes > 0xBF)
            return 0xFFFFFFFF;
        retval |= *(bytes++)&0x3F;
        retval <<= 6;
        if (*bytes < 0x80 || *bytes > 0xBF)
            return 0xFFFFFFFF;
        retval |= *bytes&0x3F;
    }
    else if (*bytes == 0xF0)
    {
        // four-byte sequences
        retval = *(bytes++)&0x07;
        retval <<= 6;
        if (*bytes < 0x90 || *bytes > 0xBF)
            return 0xFFFFFFFF;
        retval |= *(bytes++)&0x3F;
        retval <<= 6;
        if (*bytes < 0x80 || *bytes > 0xBF)
            return 0xFFFFFFFF;
        retval |= *(bytes++)&0x3F;
        retval <<= 6;
        if (*bytes < 0x80 || *bytes > 0xBF)
            return 0xFFFFFFFF;
        retval |= *bytes&0x3F;
    }
    else if (*bytes <= 0xF4)
    {
        // four-byte sequences
        retval = *(bytes++)&0x07;
        retval <<= 6;
        if (*bytes < 0x80 || *bytes > 0xBF)
            return 0xFFFFFFFF;
        retval |= *(bytes++)&0x3F;
        retval <<= 6;
        if (*bytes < 0x80 || *bytes > 0xBF)
            return 0xFFFFFFFF;
        retval |= *(bytes++)&0x3F;
        retval <<= 6;
        if (*bytes < 0x80 || *bytes > 0xBF)
            return 0xFFFFFFFF;
        retval |= *bytes&0x3F;
    }
    else
        return 0xFFFFFFFF;

    return retval;
}

void UTF8::AppendSequence (std::string *const dest, Uint32 const unicode)
{
    ASSERT1(dest != NULL);

    if (unicode < 0x80)
    {
        *dest += static_cast<char>(unicode);
    }
    else if (unicode < 0x800)
    {
        *dest += static_cast<char>(0xC0|(unicode>>6));
        *dest += static_cast<char>(0x80|(unicode&0x3F));
    }
    else if (unicode < 0xD800)
    {
        *dest += static_cast<char>(0xE0|(unicode>>12));
        *dest += static_cast<char>(0x80|((unicode>>6)&0x3F));
        *dest += static_cast<char>(0x80|(unicode&0x3F));
    }
    else if (unicode < 0xE000)
    {
        // ill-formed unicode scalar
    }
    else if (unicode < 0x10000)
    {
        *dest += static_cast<char>(0xE0|(unicode>>12));
        *dest += static_cast<char>(0x80|((unicode>>6)&0x3F));
        *dest += static_cast<char>(0x80|(unicode&0x3F));
    }
    else if (unicode < 0x110000)
    {
        *dest += static_cast<char>(0xF0|(unicode>>18));
        *dest += static_cast<char>(0x80|((unicode>>12)&0x3F));
        *dest += static_cast<char>(0x80|((unicode>>6)&0x3F));
        *dest += static_cast<char>(0x80|(unicode&0x3F));
    }
    else
    {
        // ill-formed unicode scalar
    }
}

} // end of namespace Xrb
