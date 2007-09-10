// ///////////////////////////////////////////////////////////////////////////
// xrb_bitcachedcompression.cpp by Victor Dods, created 2005/05/21
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_bitcachedcompression.h"

#include "xrb_huffman.h"
#include "xrb_serializer.h"

namespace Xrb
{

BitCachedCompression::BitCachedCompression (
    Huffman const *huffman,
    Uint32 cache_size_in_bytes,
    Endian::Endianness endianness)
    :
    BitCache(cache_size_in_bytes, endianness)
{
    ASSERT1(huffman != NULL);
    m_huffman = huffman;
}

BitCachedCompression::~BitCachedCompression ()
{
    m_huffman = 0;
}

void BitCachedCompression::AttachSerializer (Serializer *serializer)
{
    ASSERT1(!GetIsOpen());
    ASSERT1(serializer != NULL);
    ASSERT1(serializer->GetIsOpen());
    ASSERT1(serializer->GetIODirection() == IOD_READ ||
            serializer->GetIODirection() == IOD_WRITE);

    m_serializer = serializer;

    if (m_serializer->GetIODirection() == IOD_READ)
        BitCache::OpenForReading();
    else
        BitCache::OpenForWriting();
}

void BitCachedCompression::DetachSerializer ()
{
    ASSERT1(GetIsOpen());
    ASSERT1(m_serializer != NULL);
    ASSERT1(m_serializer->GetIsOpen());
    ASSERT1(m_serializer->GetIODirection() == IOD_READ ||
            m_serializer->GetIODirection() == IOD_WRITE);

    BitCache::Close();

    m_serializer = NULL;
}

Uint32 BitCachedCompression::FlushBytes (
    Uint8 const *bytes_to_flush,
    Uint32 number_of_bytes_to_flush) const
{
    ASSERT1(bytes_to_flush != NULL);
    ASSERT1(number_of_bytes_to_flush > 0);
    ASSERT1(GetIsOpen());
    ASSERT1(GetIODirection() == IOD_WRITE);
    ASSERT1(m_serializer != NULL);

    Uint32 retval = m_huffman->EncodeBytes(bytes_to_flush,
                                           number_of_bytes_to_flush,
                                           *m_serializer);
    SetError(m_serializer->GetError());
    return retval;
}

Uint32 BitCachedCompression::RenewBytes (
    Uint8 *bytes_to_renew,
    Uint32 number_of_bytes_to_renew) const
{
    ASSERT1(bytes_to_renew != NULL);
    ASSERT1(number_of_bytes_to_renew > 0);
    ASSERT1(GetIsOpen());
    ASSERT1(GetIODirection() == IOD_READ);
    ASSERT1(m_serializer != NULL);

    Uint32 retval = m_huffman->DecodeBytes(bytes_to_renew,
                                           number_of_bytes_to_renew,
                                           *m_serializer);
    SetError(m_serializer->GetError());
    return retval;
}

} // end of namespace Xrb
