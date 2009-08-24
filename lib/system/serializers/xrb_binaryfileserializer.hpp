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

#include <stdio.h>

#include "xrb_endian.hpp"
#include "xrb_serializer.hpp"

namespace Xrb
{

/** An instance of BinaryFileSerializer can either read or write, but not both
  * simultaneously.  Construction of a BinaryFileSerializer instance is what
  * opens a file, destruction is what * closes it.  Exceptions are thrown to
  * indicate error (see @ref Serializer ).
  *
  * Endianness is handled in the following way.  When opening a file to write,
  * the first byte is automatically written to indicate the endianness of the
  * ensuing data.  0xFF and 0x00 indicate little and big endian (the values
  * written by Write<bool>(true) and Write<bool>(false) respectively).
  *
  * All writes are done in the endianness of the machine doing the writing.
  * The reads are what do endian-switching when necessary.  This is easier
  * to implement, since the read buffer is a convenient place to do in-place
  * endian-switching, as opposed to creating temporary buffers for write-time
  * endian-switching.
  * 
  * @brief Serializer implementation for binary files.
  */
class BinaryFileSerializer : public Serializer
{
public:

    static Uint32 const ms_longest_allowable_sized_buffer_initial_value = 0x10000;

    BinaryFileSerializer (std::string const &path, IODirection direction) throw(Exception);
    virtual ~BinaryFileSerializer () throw();

    // Serializer interface methods
    virtual bool IsAtEnd () const throw(Exception);
    virtual Uint32 MaxAllowableArraySize () const throw() { return m_max_allowable_array_size; }
    virtual void ReadRawWords (void *dest, Uint32 word_size, Uint32 word_count) throw(Exception);
    virtual void WriteRawWords (void const *source, Uint32 word_size, Uint32 word_count) throw(Exception);

    std::string const &Path () const { return m_path; }
    Endian::Endianness FileEndianness () const { return m_file_endianness; }

    void SetMaxAllowableArraySize (Uint32 max_allowable_array_size) { m_max_allowable_array_size = max_allowable_array_size; }

private:

    // can't touch me, ha ha!  the call to Serializer's
    // constructor is arbitrary and irrelevant.
    BinaryFileSerializer () : Serializer(IOD_READ) { ASSERT1(false && "don't use me"); }

    std::string const m_path;
    Endian::Endianness m_file_endianness;
    Uint32 m_max_allowable_array_size;
    FILE *m_fptr;
}; // end of class BinaryFileSerializer

} // end of namespace Xrb

#endif // !defined(_XRB_BINARYFILESERIALIZER_HPP_)
