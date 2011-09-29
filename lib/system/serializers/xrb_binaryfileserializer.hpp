// ///////////////////////////////////////////////////////////////////////////
// xrb_binaryfileserializer.hpp by Victor Dods, created 2005/05/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_BINARYFILESERIALIZER_HPP_)
#define _XRB_BINARYFILESERIALIZER_HPP_

#include "xrb.hpp"

#include <fstream>

#include "xrb_endian.hpp"
#include "xrb_serializer.hpp"

namespace Xrb {

/// @brief Serializer implementation for binary files.
/// @details Construction of a BinaryFileSerializer instance is what opens a file,
/// destruction is what closes it.  Exceptions are thrown to indicate error (see @ref Serializer ).
///
/// Endianness is handled in the following way.  When opening a file to write, the
/// first byte is automatically written to indicate the endianness of the ensuing 
/// data.  0xFF and 0x00 indicate little and big endian (the values written by 
/// @code Write<bool>(true) @endcode and @code Write<bool>(false) @endcode respectively).
///
/// All writes are done in the endianness of the machine doing the writing.
/// The reads are what do endian-switching when necessary.  This is easier
/// to implement, since the read buffer is a convenient place to do in-place
/// endian-switching, as opposed to creating temporary buffers for write-time
/// endian-switching.
class BinaryFileSerializer : public Serializer
{
public:

    /// Attempts to open the given file (as an OS path) using the given IODirection.
    BinaryFileSerializer (std::string const &path, IODirection io_direction) throw(Exception);
    virtual ~BinaryFileSerializer () throw();

    std::string const &Path () const { return m_path; }

    // Serializer interface methods
    virtual bool IsReadable () const throw() { return m_is_readable; }
    virtual bool IsWritable () const throw() { return m_is_writable; }
    virtual bool IsReaderSeekable () const throw() { return m_is_readable; }
    virtual bool IsWriterSeekable () const throw() { return m_is_writable; }
    virtual bool IsAtEnd () const throw(Exception);
    virtual void ReaderSeek (Sint32 offset, SeekRelativeTo relative_to = FROM_BEGINNING) throw(Exception);
    virtual void WriterSeek (Sint32 offset, SeekRelativeTo relative_to = FROM_BEGINNING) throw(Exception);

protected:

    // Serializer interface methods
    virtual void ReadRawWords (Uint8 *dest, Uint32 word_size, Uint32 word_count) throw(Exception);
    virtual void WriteRawWords (Uint8 const *source, Uint32 word_size, Uint32 word_count) throw(Exception);

private:

    std::string const m_path;
    bool const m_is_readable;
    bool const m_is_writable;
    Endianness::Value m_file_endianness;
    Uint32 m_max_allowable_array_size;
    std::fstream &m_stream;
}; // end of class BinaryFileSerializer

} // end of namespace Xrb

#endif // !defined(_XRB_BINARYFILESERIALIZER_HPP_)
