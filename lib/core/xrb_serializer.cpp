// ///////////////////////////////////////////////////////////////////////////
// xrb_serializer.cpp by Victor Dods, created 2005/02/10
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_serializer.h"

#include <string.h>

namespace Xrb
{

Serializer::Serializer ()
    :
    m_is_open(false),
    m_io_direction(IOD_NONE),
    m_error(IOE_NONE)
{
}

Serializer::~Serializer ()
{
    ASSERT1(!GetIsOpen() && "A Serializer must not be in an open state upon destruction.")
    ASSERT1(GetIODirection() == IOD_NONE)
}

Uint32 Serializer::ReadString (char **const destination)
{
    ASSERT1(destination != NULL)

    char buffer[MAX_SUPPORTED_STRING_BUFFER_SIZE];
    Uint32 const read_string_length =
        ReadBufferString(buffer, MAX_SUPPORTED_STRING_BUFFER_SIZE);
    if (GetError() == IOE_NONE)
    {
        *destination = new char[read_string_length + 1];
        memcpy(*destination, buffer, read_string_length + 1);
    }
    return read_string_length;
}

Uint32 Serializer::WriteString (char const *const source)
{
    Uint32 actual_string_length = static_cast<Uint32>(strlen(source));
    Uint32 written_string_length =
        WriteBufferString(
            source,
            Min(actual_string_length + 1, static_cast<Uint32>(MAX_SUPPORTED_STRING_BUFFER_SIZE)));
    return written_string_length;
}

std::string Serializer::ReadStdString (Uint32 *const string_length)
{
    char buffer[MAX_SUPPORTED_STRING_BUFFER_SIZE];
    Uint32 const read_string_length =
        ReadBufferString(buffer, MAX_SUPPORTED_STRING_BUFFER_SIZE);
    std::string retval(buffer);
    if (string_length)
        *string_length = read_string_length;
    return retval;
}

void Serializer::WriteStdString (std::string const &source, Uint32 *const string_length)
{
    Uint32 written_string_length =
        WriteBufferString(
            source.c_str(), 
            static_cast<Uint32>(source.length()+1));
    if (string_length)
        *string_length = written_string_length;
}

} // end of namespace Xrb
