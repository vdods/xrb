// ///////////////////////////////////////////////////////////////////////////
// xrb_binaryfileserializer by Victor Dods, created 2005/05/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_binaryfileserializer.hpp"

#include <errno.h>
#include <sstream>

#include "xrb_endian.hpp"

namespace Xrb
{

BinaryFileSerializer::BinaryFileSerializer (std::string const &path, IODirection direction) throw(Exception)
    :
    Serializer(direction),
    m_path(path),
    m_max_allowable_array_size(ms_longest_allowable_sized_buffer_initial_value),
    m_fptr(NULL)
{
    ASSERT1(Direction() == IOD_READ || Direction() == IOD_WRITE);
    m_fptr = fopen(path.c_str(), (Direction() == IOD_READ ? "rb" : "wb"));
    if (m_fptr == NULL)
        throw Exception(FORMAT("error while opening path \"" << path << "\" (errno = " << errno << ')'));

    if (Direction() == IOD_READ)
        m_file_endianness = (Read<bool>() ? Endian::LITTLE : Endian::BIG);
    else
    {
        m_file_endianness = Endian::MACHINE;
        Write<bool>(m_file_endianness == Endian::LITTLE);
    }
}

BinaryFileSerializer::~BinaryFileSerializer () throw()
{
    if (m_fptr != NULL)
        fclose(m_fptr); // ignore errors
}

bool BinaryFileSerializer::IsAtEnd () const throw(Exception)
{
    // since the constructor throws if m_fptr is NULL, it is impossible for
    // m_fptr to be NULL in anything except the constructor or destructor.
    ASSERT1(m_fptr != NULL);

    // there were problems with feof() only returning the existing EOF state,
    // instead of checking if we were actually at the end of file.  so we'll
    // just try to read a char.  if we're at EOF, return true, otherwise put
    // the char back and pretend like nothing happened.
    int c = fgetc(m_fptr);
    if (c == EOF)
        return true;
    else
    {
        ungetc(c, m_fptr);
        return false;
    }
}

bool IsAPowerOfTwo (Uint32 value) { return value != 0 && (value & (value - 1)) == 0; }

void BinaryFileSerializer::ReadRawWords (void *dest, Uint32 word_size, Uint32 word_count) throw(Exception)
{
    ASSERT1(word_size > 0 && "you silly human!");
    ASSERT1(IsAPowerOfTwo(word_size) && "you're probably trying to read/write a struct, aren't you?");

    if (Direction() == IOD_WRITE)
        throw Exception("can't read from a IOD_WRITE Serializer");

    // since the constructor throws if m_fptr is NULL, it is impossible for
    // m_fptr to be NULL in anything except the constructor or destructor.
    ASSERT1(m_fptr != NULL);
    size_t words_read = fread(dest, word_size, word_count, m_fptr);
    if (words_read != word_count || ferror(m_fptr) != 0)
        throw Exception(FORMAT("error while reading (path \"" << m_path << "\")"));

    if (m_file_endianness != Endian::MACHINE && word_size > 1)
    {
        // switch the endianness of the words in the read-in buffer if necessary.
        for (Uint32 word = 0; word < word_count; ++word)
        {
            // there's probably some slick bit op way to do this, but you can K.I.S.S. my ass!
            Uint8 temp;
            Uint8 *left = reinterpret_cast<Uint8 *>(dest);
            Uint8 *right = reinterpret_cast<Uint8 *>(dest) + word_size - 1;
            for (Uint32 i = 0; i < word_size/2; ++i, ++left, --right)
            {
                temp = *left;
                *left = *right;
                *right = temp;
            }
            ASSERT1(left == right+1);
        }
    }
}

void BinaryFileSerializer::WriteRawWords (void const *source, Uint32 word_size, Uint32 word_count) throw(Exception)
{
    ASSERT1(word_size > 0 && "you silly human!");
    ASSERT1(IsAPowerOfTwo(word_size) && "you're probably trying to read/write a struct, aren't you?");

    if (Direction() == IOD_READ)
        throw Exception("can't write to a IOD_READ Serializer");

    // since the constructor throws if m_fptr is NULL, it is impossible for
    // m_fptr to be NULL in anything except the constructor or destructor.
    ASSERT1(m_fptr != NULL);
    size_t words_written = fwrite(source, word_size, word_count, m_fptr);
    if (words_written != word_count)
        throw Exception(FORMAT("error while writing (path \"" << m_path << "\")"));
}

} // end of namespace Xrb
