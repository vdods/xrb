// ///////////////////////////////////////////////////////////////////////////
// xrb_bitcache.hpp by Victor Dods, created 2005/05/19
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_BITCACHE_HPP_)
#define _XRB_BITCACHE_HPP_

#include "xrb.hpp"

#include "xrb_endian.hpp"
#include "xrb_enums.hpp"

namespace Xrb
{

// for doing bit-packed caching in memory with abstract buffer flushing/renewing
class BitCache
{
public:

    BitCache (Uint32 cache_size_in_bytes, Endian::Endianness endianness);
    virtual ~BitCache ();

    // ///////////////////////////////////////////////////////////////////////
    // state accessors
    // ///////////////////////////////////////////////////////////////////////

    /** @brief Get the endianness of this BitCache.
      * @return The endianness (word storage byte order) of this BitCache.
      * @note Will not change the error value.
      */
    inline Endian::Endianness GetEndianness () const
    {
        return m_endianness;
    }
    /** @brief Get the open state of the BitCache.
      * @return True iff the BitCache is open for input/output.
      * @note Will not change the error value.
      */
    inline bool IsOpen () const
    {
        return m_is_open;
    }
    /** @brief Get the I/O direction of the BitCache.
      * @return @c IOD_READ if reading, @c IOD_WRITE if writing,
      *         or IOD_NONE if neither is applicable (e.g. if the cache is
      *         not currently open).
      * @note Will not change the error value.
      */
    inline IODirection GetIODirection () const
    {
        return m_io_direction;
    }
    /** @brief Get the end-of-cache condition.
      * @return True iff the cache is at the end (e.g. end of file)
      * @note Will not change the error value.
      */
    bool IsAtEnd () const;
    /** This is necessary because IsAtEnd() doesn't work when a set
      * of data doesn't end exactly on a byte boundary.
      * @brief Get the less-than-one-byte-left-in-in-the-cache condition.
      * @return True iff the cache has fewer than 8 bits left.
      * @note Will not change the error value.
      */
    bool HasFewerThan8BitsLeft () const;
    /** @brief Get the error state of the most recently called procedure.
      * @return The most recent error state value.
      * @note Will not change the error value.
      */
    inline IOError GetError () const
    {
        return m_error;
    }

    Uint32 ReadUnsignedBits (Uint32 bit_count);
    void WriteUnsignedBits (Uint32 value, Uint32 bit_count);

    Sint32 ReadSignedBits (Uint32 bit_count);
    void WriteSignedBits (Sint32 value, Uint32 bit_count);

    bool ReadBool ();
    void WriteBool (bool value);

    void Read1ByteWordFromCache (void *destination);
    void Write1ByteWordToCache (void *source);

    void Read2ByteWordFromCache (void *destination);
    void Write2ByteWordToCache (void *source);

    void Read4ByteWordFromCache (void *destination);
    void Write4ByteWordToCache (void *source);

    void Read8ByteWordFromCache (void *destination);
    void Write8ByteWordToCache (void *source);

    void ReadBytes (Uint8 *destination, Uint32 destination_size);
    void WriteBytes (Uint8 const *source, Uint32 source_size);

    Uint32 ReadBufferString (char *destination, Uint32 destination_length);
    // this might need a "bool write_null_char" parameter.
    Uint32 WriteBufferString (char const *source, Uint32 source_length);

    // this makes virtual calls and may not be used in the destructor.
    void FlushWriteCache ();

protected:

    inline void SetError (IOError error) const { m_error = error; }

    // ///////////////////////////////////////////////////////////////////////
    // procedures
    // ///////////////////////////////////////////////////////////////////////

    inline void IncrementCacheByteIndex (Uint32 requested_byte_count)
    {
        ASSERT1(IsCacheBitIndexOnByteBoundary());
        ASSERT1(CacheByteIndex() + requested_byte_count
                <=
                m_next_available_cache_byte_index);
        m_cache_bit_index += requested_byte_count << 3;
    }
    inline void IncrementCacheBitIndex (Uint32 requested_bit_count)
    {
        ASSERT1(m_cache_bit_index + requested_bit_count
                <=
                GetNextAvailableCacheBitIndex());
        m_cache_bit_index += requested_bit_count;
    }

    inline void EnsureCacheIsOnByteBoundary ()
    {
        // round up to the nearest multiple of 8
        m_cache_bit_index = ((m_cache_bit_index + 7) >> 3) << 3;
    }

    void EnsureReadCacheHasEnoughBitsAvailable (Uint32 requested_bit_count) const;
    void EnsureReadCacheHasEnoughBytesAvailable (Uint32 requested_byte_count) const;

    void EnsureWriteCacheHasEnoughBitsAvailable (Uint32 requested_bit_count) const;
    void EnsureWriteCacheHasEnoughBytesAvailable (Uint32 requested_byte_count) const;
    
    void OpenForReading ();
    void OpenForWriting ();

    // this makes virtual calls and may not be used in the destructor.
    void Close ();

    virtual Uint32 FlushBytes (
        Uint8 const *bytes_to_flush,
        Uint32 number_of_bytes_to_flush) const = 0;
    virtual Uint32 RenewBytes (
        Uint8 *bytes_to_renew,
        Uint32 number_of_bytes_to_renew) const = 0;

private:

    // ///////////////////////////////////////////////////////////////////////
    // private accessors
    // ///////////////////////////////////////////////////////////////////////

    inline bool IsCacheEmpty () const
    {
        return m_cache_bit_index == 0;
    }
    inline bool IsCacheBitIndexOnByteBoundary () const
    {
        return (m_cache_bit_index & 7) == 0;
    }
    // this is m_cache_bit_index divided by 8, rounded up.
    inline Uint32 GetUsedCacheByteCount () const
    {
        return (m_cache_bit_index + 7) >> 3;
    }
    inline Uint32 CacheByteIndex () const
    {
        return m_cache_bit_index >> 3;
    }
    inline Uint32 GetNextAvailableCacheBitIndex () const
    {
        return m_next_available_cache_byte_index << 3;
    }

    // ///////////////////////////////////////////////////////////////////////
    // private modifiers
    // ///////////////////////////////////////////////////////////////////////

    inline void SetIsOpen (bool is_open) { m_is_open = is_open; }
    inline void SetIODirection (IODirection io_direction)
    {
        m_io_direction = io_direction;
    }

    /// The endianness of this BitCache.
    Endian::Endianness m_endianness;
    /// The current open state.
    bool m_is_open;
    /// The current IO direction.
    IODirection m_io_direction;
    /// The most recent error state, which must be set by each procedure call.
    mutable IOError m_error;

    /// The requested (working) cache size in bytes.
    Uint32 const m_working_cache_size_in_bytes;
    /// The requested (working) cache size in bits.
    Uint32 const m_working_cache_size_in_bits;
    /// The actual cache size in bytes (the size of the allocated m_cache).
    Uint32 const m_actual_cache_size_in_bytes;
    /// The actual cache size in bits.
    Uint32 const m_actual_cache_size_in_bits;
    /// The memory buffer where the bits are cached.
    mutable Uint8 *m_cache;
    /// Bit index into the cache.  The least-significant 3 bits are the bit
    /// index, and the rest are the byte index.  The index indicates the
    /// next available bit to be written.  Index 0 is the most significant bit
    /// in byte 0 of m_cache, index 7 is the least significant bit of byte 0,
    /// index 8 is the most significant bit of byte 1, etc.
    mutable Uint32 m_cache_bit_index;
    /// The index of the next available byte in the cache.  This is necessary
    /// because when a read stream is almost through reading, its cache may not
    /// fill up on the last cache cycle).
    mutable Uint32 m_next_available_cache_byte_index;
}; // end of class BitCache

} // end of namespace Xrb

#endif // !defined(_XRB_BITCACHE_HPP_)
