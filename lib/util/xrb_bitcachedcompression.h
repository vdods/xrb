// ///////////////////////////////////////////////////////////////////////////
// xrb_bitcachedcompression.h by Victor Dods, created 2005/05/21
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_BITCACHEDCOMPRESSION_H_)
#define _XRB_BITCACHEDCOMPRESSION_H_

#include "xrb.h"

#include "xrb_bitcache.h"

namespace Xrb
{

class Huffman;
class Serializer;

class BitCachedCompression : public BitCache
{
public:

    BitCachedCompression (
        Huffman const *huffman,
        Uint32 cache_size_in_bytes,
        Endian::Endianness endianness);
    virtual ~BitCachedCompression ();

    inline Serializer *GetAttachedSerializer () const { return m_serializer; }

    void AttachSerializer (Serializer *serializer);
    void DetachSerializer ();

protected:

    virtual Uint32 FlushBytes (
        Uint8 const *bytes_to_flush,
        Uint32 number_of_bytes_to_flush) const;
    virtual Uint32 RenewBytes (
        Uint8 *bytes_to_renew,
        Uint32 number_of_bytes_to_renew) const;

private:

    /// The Huffman en/decoder to use for the de/compression.
    Huffman const *m_huffman;
    /// The serializer being used to do the actual reads/writes.
    Serializer *m_serializer;
}; // end of class BitCachedCompression

} // end of namespace Xrb

#endif // !defined(_XRB_BITCACHEDCOMPRESSION_H_)
