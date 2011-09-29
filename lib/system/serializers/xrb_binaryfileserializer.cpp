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

#include "xrb_endian.hpp"
#include "xrb_filesystem.hpp"

namespace Xrb {

BinaryFileSerializer::BinaryFileSerializer (std::string const &path, IODirection io_direction) throw(Exception)
    :
    Serializer(),
    m_path(path),
    m_is_readable(io_direction == IOD_READ || io_direction == IOD_READ_AND_WRITE),
    m_is_writable(io_direction == IOD_WRITE || io_direction == IOD_READ_AND_WRITE),
    m_stream(*new std::fstream())
{
    if (io_direction != IOD_READ && io_direction != IOD_WRITE && io_direction != IOD_READ_AND_WRITE)
        throw Exception(FORMAT("io_direction value is an invalid IODirection"));

    m_stream.exceptions(std::ios_base::failbit|std::ios_base::badbit);
    try {
        std::ios_base::openmode mode = std::ios_base::binary;
        if (m_is_readable)
            mode |= std::ios_base::in;
        if (m_is_writable)
            mode |= std::ios_base::out;
        // TODO: add option to truncate file contents (std::ios_base::trunc)
        std::string os_path(Singleton::FileSystem().OsPath(path, m_is_writable));
        m_stream.open(os_path.c_str(), mode);
    } catch (std::ios_base::failure const &f) {
        delete &m_stream; // throwing during construction causes this object to go out of scope, so m_stream is also.
        throw Exception(FORMAT("error while opening file for " <<
                               (m_is_readable ? "reading" : "") <<
                               (m_is_readable && m_is_writable ? " and " : "") <<
                               (m_is_writable ? "writing" : "")));
    }

    switch (io_direction)
    {
        case IOD_READ:
            m_file_endianness = (Read<bool>() ? Endianness::LITTLE : Endianness::BIG);
            break;

        case IOD_WRITE:
            m_file_endianness = Endianness::OF_TARGET;
            Write<bool>(m_file_endianness == Endianness::LITTLE);
            break;

        case IOD_READ_AND_WRITE:
            ASSERT0(false && "not implemented yet");
            break;

        default:
            ASSERT1(false && "this should never happen");
            break;
    }
}

BinaryFileSerializer::~BinaryFileSerializer () throw()
{
    m_stream.close();
    delete &m_stream; // this BinaryFileSerializer is about to go out of scope, so m_stream is also.
}

bool BinaryFileSerializer::IsAtEnd () const throw(Exception)
{
    if (m_is_writable)
        return false;
    
    m_stream.peek(); // cause an i/o operation so the eof flag could be triggered
    return m_stream.eof();
}

void BinaryFileSerializer::ReaderSeek (Sint32 offset, SeekRelativeTo relative_to) throw(Exception)
{
    try {
        std::ios_base::seekdir dir;
        switch (relative_to)
        {
            case FROM_BEGINNING:        dir = std::ios_base::beg; break;
            case FROM_CURRENT_POSITION: dir = std::ios_base::cur; break;
            case FROM_END:              dir = std::ios_base::end; break;
            default: ASSERT1(false && "this should never happen");
        }
        m_stream.seekg(offset, dir);
    } catch (std::ios_base::failure const &f) {
        throw Exception(FORMAT("error during call to ReaderSeek"));
    }
}

void BinaryFileSerializer::WriterSeek (Sint32 offset, SeekRelativeTo relative_to) throw(Exception)
{
    try {
        std::ios_base::seekdir dir;
        switch (relative_to)
        {
            case FROM_BEGINNING:        dir = std::ios_base::beg; break;
            case FROM_CURRENT_POSITION: dir = std::ios_base::cur; break;
            case FROM_END:              dir = std::ios_base::end; break;
            default: ASSERT1(false && "this should never happen");
        }
        m_stream.seekp(offset, dir);
    } catch (std::ios_base::failure const &f) {
        throw Exception(FORMAT("error during call to WriterSeek"));
    }
} 

bool IsAPowerOfTwo (Uint32 value) { return value != 0 && (value & (value - 1)) == 0; }

void BinaryFileSerializer::ReadRawWords (Uint8 *dest, Uint32 word_size, Uint32 word_count) throw(Exception)
{
    ASSERT1(word_size > 0 && "you silly human!");
    ASSERT1(IsAPowerOfTwo(word_size) && "you're probably trying to read/write a struct, aren't you?");

    if (!IsReadable())
        throw Exception("this Serializer is not readable");

    try {
        m_stream.read(reinterpret_cast<char *>(dest), word_size*word_count);
    } catch (std::ios_base::failure const &f) {
        throw Exception(FORMAT("could not read " << word_count << " words each of size " << word_size << " (hit EOF prematurely, or possibly another error occurred)"));
    }

    if (m_file_endianness != Endianness::OF_TARGET)
        SwitchByteOrder(dest, word_size, word_count);
}

void BinaryFileSerializer::WriteRawWords (Uint8 const *source, Uint32 word_size, Uint32 word_count) throw(Exception)
{
    ASSERT1(word_size > 0 && "you silly human!");
    ASSERT1(IsAPowerOfTwo(word_size) && "you're probably trying to read/write a struct, aren't you?");

    if (!IsWritable())
        throw Exception("this Serializer is not writable");

    try {
        m_stream.write(reinterpret_cast<char const *>(source), word_size*word_count);
    } catch (std::ios_base::failure const &f) {
        throw Exception(FORMAT("could not write " << word_count << " words each of size " << word_size));
    }
}
    
} // end of namespace Xrb
