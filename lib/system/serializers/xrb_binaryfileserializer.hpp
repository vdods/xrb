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

// implements the Serializer interface for binary files.
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

    std::string const m_path;
    Endian::Endianness m_file_endianness;
    Uint32 m_max_allowable_array_size;
    FILE *m_fptr;
}; // end of class BinaryFileSerializer

} // end of namespace Xrb

#endif // !defined(_XRB_BINARYFILESERIALIZER_HPP_)
