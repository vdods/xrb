// ///////////////////////////////////////////////////////////////////////////
// xrb_bitcache.cpp by Victor Dods, created 2005/05/19
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_bitcache.h"

#include <string.h>

namespace Xrb
{

BitCache::BitCache (
    Uint32 const cache_size_in_bytes,
    Endian::Endianness const endianness)
    :
    m_working_cache_size_in_bytes(cache_size_in_bytes),
    m_working_cache_size_in_bits(m_working_cache_size_in_bytes * 8),
    m_actual_cache_size_in_bytes(m_working_cache_size_in_bytes + 1),
    m_actual_cache_size_in_bits(m_actual_cache_size_in_bytes * 8)
{
    ASSERT1(m_working_cache_size_in_bytes > 0)
    ASSERT1(m_working_cache_size_in_bits > m_working_cache_size_in_bytes && "Numerical overflow resulting from requested cache size")
    ASSERT1(m_actual_cache_size_in_bytes > m_working_cache_size_in_bytes && "Numerical overflow resulting from requested cache size")
    ASSERT1(m_actual_cache_size_in_bits > m_actual_cache_size_in_bytes && "Numerical overflow resulting from requested cache size")

    m_cache = new Uint8[m_actual_cache_size_in_bytes];

    SetIsOpen(false);
    SetIODirection(IOD_NONE);
    SetError(IOE_NONE);
}

BitCache::~BitCache ()
{
    ASSERT1(!GetIsOpen() && "You must close a previously opened BitCache before it is destroyed.")
    ASSERT1(GetIODirection() == IOD_NONE)
    ASSERT1(m_cache != NULL)
    ASSERT1(m_working_cache_size_in_bytes > 0)
    ASSERT1(m_working_cache_size_in_bits > 0)
    ASSERT1(m_actual_cache_size_in_bytes > 0)
    ASSERT1(m_actual_cache_size_in_bits > 0)

    Delete(m_cache);
    m_cache_bit_index = 0;
    m_next_available_cache_byte_index = 0;
}

bool BitCache::GetIsAtEnd () const
{
    if (GetCacheByteIndex() < m_next_available_cache_byte_index)
        return false;

    switch (GetIODirection())
    {
        case IOD_READ:
            EnsureReadCacheHasEnoughBitsAvailable(1);
            return GetError() == IOE_IS_AT_END;

        case IOD_WRITE:
            EnsureWriteCacheHasEnoughBitsAvailable(1);
            return GetError() == IOE_IS_AT_END;

        default:
            ASSERT0(false && "Invalid IODirection")
            return false;
    }
}

bool BitCache::GetHasFewerThan8BitsLeft () const
{
    switch (GetIODirection())
    {
        case IOD_READ:
            EnsureReadCacheHasEnoughBitsAvailable(8);
            return GetError() == IOE_IS_AT_END ||
                   GetError() == IOE_INSUFFICIENT_AVAILABLE_DATA;

        case IOD_WRITE:
            EnsureWriteCacheHasEnoughBitsAvailable(8);
            return GetError() == IOE_IS_AT_END ||
                   GetError() == IOE_INSUFFICIENT_STORAGE;

        default:
            ASSERT0(false && "Invalid IODirection")
            return false;
    }
}

Uint32 BitCache::ReadUnsignedBits (Uint32 const bit_count)
{
    ASSERT1(GetIsOpen())
    ASSERT1(GetIODirection() == IOD_READ)
    ASSERT1(bit_count <= 32)

    EnsureReadCacheHasEnoughBitsAvailable(bit_count);
    if (GetError() != IOE_NONE)
        return 0;

    Uint32 retval = 0;
    Uint32 bits_actually_transferred = 0;
    Uint8 *source = m_cache + GetCacheByteIndex();

    while (bits_actually_transferred < bit_count)
    {
        Uint32 amount_to_shift_retval =
            Min(8 - ((m_cache_bit_index + bits_actually_transferred) & 7),
                bit_count - bits_actually_transferred);
        Uint32 amount_to_shift_source =
            8 - Min(
                ((m_cache_bit_index + bits_actually_transferred) & 7) +
                (bit_count - bits_actually_transferred),
                static_cast<Uint32>(8));
        Uint32 source_value =
            static_cast<Uint32>(*source) >> amount_to_shift_source;
        Uint32 source_mask =
            (1 << amount_to_shift_retval) - 1;

        retval <<= amount_to_shift_retval;
        retval |= source_value & source_mask;

        bits_actually_transferred += amount_to_shift_retval;
        ++source;
    }

    // the bit packed writing algorithm is implicitly big-endian --

    IncrementCacheBitIndex(bit_count);
    return retval;
}

void BitCache::WriteUnsignedBits (Uint32 const value, Uint32 const bit_count)
{
    ASSERT1(GetIsOpen())
    ASSERT1(GetIODirection() == IOD_WRITE)
    ASSERT1(bit_count <= 32)

    EnsureWriteCacheHasEnoughBitsAvailable(bit_count);
    if (GetError() != IOE_NONE)
        return;

    // the bit packed writing algorithm is implicitly big-endian

    Uint32 bits_actually_transferred = 0;
    Uint8 *source = m_cache + GetCacheByteIndex();

    while (bits_actually_transferred < bit_count)
    {
        Uint32 bits_to_transfer_in_this_byte =
            Min(8 - ((m_cache_bit_index + bits_actually_transferred) & 7),
                bit_count - bits_actually_transferred);
        Uint32 amount_to_shift_source =
            8 - Min(
                ((m_cache_bit_index + bits_actually_transferred) & 7) +
                (bit_count - bits_actually_transferred),
                static_cast<Uint32>(8));
        Uint8 source_write_mask =
            static_cast<Uint8>((1 << bits_to_transfer_in_this_byte) - 1);
        Uint8 value_to_write =
            static_cast<Uint8>(value >> (bit_count -
                                         bits_actually_transferred -
                                         bits_to_transfer_in_this_byte));
        *source >>= amount_to_shift_source;
        *source &= ~source_write_mask;
        *source |= value_to_write & source_write_mask;
        *source <<= amount_to_shift_source;

        bits_actually_transferred += bits_to_transfer_in_this_byte;
        ++source;
    }

    IncrementCacheBitIndex(bit_count);
    ASSERT1(GetError() == IOE_NONE)
}

Sint32 BitCache::ReadSignedBits (Uint32 const bit_count)
{
    Uint32 retval = ReadUnsignedBits(bit_count);
    if (GetError() != IOE_NONE)
        return 0;

    // extend the sign bit if necessary
    if (bit_count < 32 && (retval & (1 << (bit_count - 1))) != 0)
        retval |= ~((1 << bit_count) - 1);

    ASSERT1(GetError() == IOE_NONE)
    return static_cast<Sint32>(retval);
}

void BitCache::WriteSignedBits (Sint32 const value, Uint32 const bit_count)
{
    WriteUnsignedBits(static_cast<Uint32>(value), bit_count);
}

bool BitCache::ReadBool ()
{
    ASSERT1(GetIsOpen())
    ASSERT1(GetIODirection() == IOD_READ)

    EnsureReadCacheHasEnoughBitsAvailable(1);
    if (GetError() != IOE_NONE)
        return false;

    Uint32 bit_to_query = 7 - (m_cache_bit_index & 7);
    Uint8 bit_mask = static_cast<Uint8>(1 << bit_to_query);
    bool retval = (m_cache[GetCacheByteIndex()] & bit_mask) ? true : false;

    IncrementCacheBitIndex(1);
    ASSERT1(GetError() == IOE_NONE)
    return retval;
}

void BitCache::WriteBool (bool const value)
{
    ASSERT1(GetIsOpen())
    ASSERT1(GetIODirection() == IOD_WRITE)

    EnsureWriteCacheHasEnoughBitsAvailable(1);
    if (GetError() != IOE_NONE)
        return;

    Uint32 bit_to_set = 7 - (m_cache_bit_index & 7);
    Uint8 bit_mask = static_cast<Uint8>(1 << bit_to_set);
    if (value)
        m_cache[GetCacheByteIndex()] |= bit_mask;
    else
        m_cache[GetCacheByteIndex()] &= ~bit_mask;

    IncrementCacheBitIndex(1);
    ASSERT1(GetError() == IOE_NONE)
}

void BitCache::Read1ByteWordFromCache (void *const destination)
{
    ASSERT1(GetIsOpen())
    ASSERT1(GetIODirection() == IOD_READ)
    ASSERT1(destination != NULL)
    Uint32 const byte_count = 1;
    EnsureCacheIsOnByteBoundary();
    EnsureReadCacheHasEnoughBytesAvailable(byte_count);
    if (GetError() != IOE_NONE)
        return;
    for (Uint8 *write_to = static_cast<Uint8 *>(destination),
               *read_from = m_cache + GetCacheByteIndex(),
               i = 0;
         i < byte_count;
         ++write_to, ++read_from, ++i)
    {
        *write_to = *read_from;
    }
    Endian::ConvertGivenToMachine1ByteWord(GetEndianness(), destination);
    IncrementCacheByteIndex(byte_count);
    ASSERT1(GetError() == IOE_NONE)
}

void BitCache::Write1ByteWordToCache (void *const source)
{
    ASSERT1(GetIsOpen())
    ASSERT1(GetIODirection() == IOD_WRITE)
    ASSERT1(source != NULL)
    Uint32 const byte_count = 1;
    EnsureCacheIsOnByteBoundary();
    EnsureWriteCacheHasEnoughBytesAvailable(byte_count);
    if (GetError() != IOE_NONE)
        return;
    Endian::ConvertMachineToGiven1ByteWord(source, GetEndianness());
    for (Uint8 *write_to = m_cache + GetCacheByteIndex(),
               *read_from = static_cast<Uint8 *>(source),
               i = 0;
            i < byte_count;
            ++write_to, ++read_from, ++i)
    {
        *write_to = *read_from;
    }
    IncrementCacheByteIndex(byte_count);
    ASSERT1(GetError() == IOE_NONE)
}

void BitCache::Read2ByteWordFromCache (void *const destination)
{
    ASSERT1(GetIsOpen())
    ASSERT1(GetIODirection() == IOD_READ)
    ASSERT1(destination != NULL)
    Uint32 const byte_count = 2;
    EnsureCacheIsOnByteBoundary();
    EnsureReadCacheHasEnoughBytesAvailable(byte_count);
    if (GetError() != IOE_NONE)
        return;
    for (Uint8 *write_to = static_cast<Uint8 *>(destination),
               *read_from = m_cache + GetCacheByteIndex(),
               i = 0;
            i < byte_count;
            ++write_to, ++read_from, ++i)
    {
        *write_to = *read_from;
    }
    Endian::ConvertGivenToMachine2ByteWord(GetEndianness(), destination);
    IncrementCacheByteIndex(byte_count);
    ASSERT1(GetError() == IOE_NONE)
}

void BitCache::Write2ByteWordToCache (void *const source)
{
    ASSERT1(GetIsOpen())
    ASSERT1(GetIODirection() == IOD_WRITE)
    ASSERT1(source != NULL)
    Uint32 const byte_count = 2;
    EnsureCacheIsOnByteBoundary();
    EnsureWriteCacheHasEnoughBytesAvailable(byte_count);
    if (GetError() != IOE_NONE)
        return;
    Endian::ConvertMachineToGiven2ByteWord(source, GetEndianness());
    for (Uint8 *write_to = m_cache + GetCacheByteIndex(),
               *read_from = static_cast<Uint8 *>(source),
               i = 0;
            i < byte_count;
            ++write_to, ++read_from, ++i)
    {
        *write_to = *read_from;
    }
    IncrementCacheByteIndex(byte_count);
    ASSERT1(GetError() == IOE_NONE)
}

void BitCache::Read4ByteWordFromCache (void *const destination)
{
    ASSERT1(GetIsOpen())
    ASSERT1(GetIODirection() == IOD_READ)
    ASSERT1(destination != NULL)
    Uint32 const byte_count = 4;
    EnsureCacheIsOnByteBoundary();
    EnsureReadCacheHasEnoughBytesAvailable(byte_count);
    if (GetError() != IOE_NONE)
        return;
    for (Uint8 *write_to = static_cast<Uint8 *>(destination),
               *read_from = m_cache + GetCacheByteIndex(),
               i = 0;
            i < byte_count;
            ++write_to, ++read_from, ++i)
    {
        *write_to = *read_from;
    }
    Endian::ConvertGivenToMachine4ByteWord(GetEndianness(), destination);
    IncrementCacheByteIndex(byte_count);
    ASSERT1(GetError() == IOE_NONE)
}

void BitCache::Write4ByteWordToCache (void *const source)
{
    ASSERT1(GetIsOpen())
    ASSERT1(GetIODirection() == IOD_WRITE)
    ASSERT1(source != NULL)
    Uint32 const byte_count = 4;
    EnsureCacheIsOnByteBoundary();
    EnsureWriteCacheHasEnoughBytesAvailable(byte_count);
    if (GetError() != IOE_NONE)
        return;
    Endian::ConvertMachineToGiven4ByteWord(source, GetEndianness());
    for (Uint8 *write_to = m_cache + GetCacheByteIndex(),
               *read_from = static_cast<Uint8 *>(source),
               i = 0;
            i < byte_count;
            ++write_to, ++read_from, ++i)
    {
        *write_to = *read_from;
    }
    IncrementCacheByteIndex(byte_count);
    ASSERT1(GetError() == IOE_NONE)
}

void BitCache::Read8ByteWordFromCache (void *const destination)
{
    ASSERT1(GetIsOpen())
    ASSERT1(GetIODirection() == IOD_READ)
    ASSERT1(destination != NULL)
    Uint32 const byte_count = 8;
    EnsureCacheIsOnByteBoundary();
    EnsureReadCacheHasEnoughBytesAvailable(byte_count);
    if (GetError() != IOE_NONE)
        return;
    for (Uint8 *write_to = static_cast<Uint8 *>(destination),
               *read_from = m_cache + GetCacheByteIndex(),
               i = 0;
            i < byte_count;
            ++write_to, ++read_from, ++i)
    {
        *write_to = *read_from;
    }
    Endian::ConvertGivenToMachine8ByteWord(GetEndianness(), destination);
    IncrementCacheByteIndex(byte_count);
    ASSERT1(GetError() == IOE_NONE)
}

void BitCache::Write8ByteWordToCache (void *const source)
{
    ASSERT1(GetIsOpen())
    ASSERT1(GetIODirection() == IOD_WRITE)
    ASSERT1(source != NULL)
    Uint32 const byte_count = 8;
    EnsureCacheIsOnByteBoundary();
    EnsureWriteCacheHasEnoughBytesAvailable(byte_count);
    if (GetError() != IOE_NONE)
        return;
    Endian::ConvertMachineToGiven8ByteWord(source, GetEndianness());
    for (Uint8 *write_to = m_cache + GetCacheByteIndex(),
               *read_from = static_cast<Uint8 *>(source),
               i = 0;
            i < byte_count;
            ++write_to, ++read_from, ++i)
    {
        *write_to = *read_from;
    }
    IncrementCacheByteIndex(byte_count);
    ASSERT1(GetError() == IOE_NONE)
}

void BitCache::ReadBytes (
    Uint8 *const destination,
    Uint32 const destination_size)
{
    ASSERT1(destination != NULL)
    ASSERT1(destination_size > 0)
    ASSERT1(destination_size <= m_working_cache_size_in_bytes)

    EnsureCacheIsOnByteBoundary();
    EnsureReadCacheHasEnoughBytesAvailable(destination_size);
    if (GetError() != IOE_NONE)
        return;

    memcpy(destination, m_cache + GetCacheByteIndex(), destination_size);
    IncrementCacheByteIndex(destination_size);
    ASSERT1(GetError() == IOE_NONE)
}

void BitCache::WriteBytes (
    Uint8 const *const source,
    Uint32 const source_size)
{
    ASSERT1(source != NULL)
    ASSERT1(source_size > 0)
    ASSERT1(source_size <= m_working_cache_size_in_bytes)

    EnsureCacheIsOnByteBoundary();
    EnsureWriteCacheHasEnoughBytesAvailable(source_size);
    if (GetError() != IOE_NONE)
        return;

    memcpy(m_cache + GetCacheByteIndex(), source, source_size);
    IncrementCacheByteIndex(source_size);
    ASSERT1(GetError() == IOE_NONE)
}

Uint32 BitCache::ReadBufferString (
    char *const destination,
    Uint32 const destination_length)
{
    ASSERT1(GetIsOpen())
    ASSERT1(GetIODirection() == IOD_READ)
    ASSERT1(destination != NULL)
    ASSERT1(destination_length > 0)
    ASSERT1(destination_length <= m_working_cache_size_in_bytes)

    EnsureCacheIsOnByteBoundary();
    EnsureReadCacheHasEnoughBytesAvailable(destination_length);
    // IOE_INSUFFICIENT_AVAILABLE_DATA is acceptable here because we don't
    // know how much data is actually going to be read in -- we have to cache
    // up the maximum possible amount first, possibly risking said error.
    // we just have to make sure that we check for the end of the cache
    // while iterating.
    if (GetError() != IOE_NONE &&
        GetError() != IOE_INSUFFICIENT_AVAILABLE_DATA)
    {
        *destination = '\0';
        return 0;
    }

    char *destination_it = destination;
    Uint32 chars_read_count = 0;
    // copy the chars into the buffer
    while (GetCacheByteIndex() < m_next_available_cache_byte_index &&
           chars_read_count < destination_length - 1 &&
           m_cache[GetCacheByteIndex()] != '\0')
    {
        *destination_it = m_cache[GetCacheByteIndex()];
        ++destination_it;
        ++chars_read_count;
        IncrementCacheByteIndex(1);
    }
    // write the null char
    ASSERT1(destination + chars_read_count == destination_it)
    *destination_it = '\0';
    if (m_cache[GetCacheByteIndex()] != '\0')
    {
        IncrementCacheByteIndex(1);
        SetError(IOE_OVERSIZED_STRING);
    }
    else if (GetCacheByteIndex() == m_next_available_cache_byte_index)
    {
        SetError(IOE_INSUFFICIENT_AVAILABLE_DATA);
    }
    else
    {
        IncrementCacheByteIndex(1);
        SetError(IOE_NONE);
    }

    return chars_read_count;
}

Uint32 BitCache::WriteBufferString (
    char const *const source,
    Uint32 const source_length)
{
    ASSERT1(GetIsOpen())
    ASSERT1(GetIODirection() == IOD_WRITE)
    ASSERT1(source_length > 0)
    ASSERT1(source_length <= m_working_cache_size_in_bytes)
    ASSERT1(source != NULL)

    EnsureCacheIsOnByteBoundary();
    EnsureWriteCacheHasEnoughBytesAvailable(source_length);
    if (GetError() != IOE_NONE)
        return 0;

    char const *source_it = source;
    char *destination =
        reinterpret_cast<char *>(m_cache + GetCacheByteIndex());
    Uint32 chars_written_count = 0;
    // copy the chars into the cache
    while (chars_written_count < source_length - 1 && *source_it != '\0')
    {
        *destination = *source_it;
        ++source_it;
        ++destination;
        ++chars_written_count;
    }
    // write the null char
    ASSERT1(source + chars_written_count == source_it)
    *destination = '\0';
    ++chars_written_count;
    if (*source_it != '\0')
        SetError(IOE_OVERSIZED_STRING);
    else
        SetError(IOE_NONE);

    IncrementCacheByteIndex(chars_written_count);
    // the last char is the null-terminating char, so the string
    // length is one less than that.
    return chars_written_count - 1;
}

void BitCache::FlushWriteCache ()
{
    if (GetCacheByteIndex() > 0)
    {
        EnsureCacheIsOnByteBoundary();
        Uint32 bytes_written = FlushBytes(m_cache, GetCacheByteIndex());
        if (bytes_written == GetCacheByteIndex())
            SetError(IOE_NONE);
        else
            SetError(IOE_INSUFFICIENT_STORAGE);

        m_cache_bit_index -= bytes_written << 3;
    }
    else
        SetError(IOE_NONE);
}

void BitCache::EnsureReadCacheHasEnoughBitsAvailable (
    Uint32 const requested_bit_count) const
{
    ASSERT1(GetIODirection() == IOD_READ)
    ASSERT1(requested_bit_count > 0)
    ASSERT1(requested_bit_count <= m_working_cache_size_in_bits)

    // if we already have enough cached bits available, set IOE_NONE and return.
    if (m_cache_bit_index + requested_bit_count
        <=
        (m_next_available_cache_byte_index << 3))
    {
        SetError(IOE_NONE);
        return;
    }

    // shift the important bytes over
    memmove(m_cache,
            m_cache + GetCacheByteIndex(),
            m_next_available_cache_byte_index - GetCacheByteIndex());
    m_next_available_cache_byte_index -= GetCacheByteIndex();
    m_cache_bit_index &= 7;

    Uint32 bytes_read =
        RenewBytes(m_cache + m_next_available_cache_byte_index,
                   m_actual_cache_size_in_bytes - m_next_available_cache_byte_index);

    m_next_available_cache_byte_index += bytes_read;
    ASSERT1(m_next_available_cache_byte_index <= m_actual_cache_size_in_bytes)

    // set IOE_NONE if there are enough bytes available to fill the request.
    if (m_cache_bit_index + requested_bit_count
        <=
        (m_next_available_cache_byte_index << 3))
    {
        SetError(IOE_NONE);
    }
    // otherwise there's not enough data left.  set appropriate error.
    else if (m_next_available_cache_byte_index < m_actual_cache_size_in_bytes &&
             m_cache_bit_index == m_next_available_cache_byte_index << 3)
        SetError(IOE_IS_AT_END);
    else
        SetError(IOE_INSUFFICIENT_AVAILABLE_DATA);
}

void BitCache::EnsureReadCacheHasEnoughBytesAvailable (
    Uint32 const requested_byte_count) const
{
    ASSERT1(GetIsCacheBitIndexOnByteBoundary())
    EnsureReadCacheHasEnoughBitsAvailable(requested_byte_count << 3);
}

void BitCache::EnsureWriteCacheHasEnoughBitsAvailable (
    Uint32 const requested_bit_count) const
{
    ASSERT1(GetIODirection() == IOD_WRITE)
    ASSERT1(requested_bit_count > 0)
    ASSERT1(requested_bit_count <= m_working_cache_size_in_bits)

    // if we already have enough cached bits available, set IOE_NONE and return.
    if (m_cache_bit_index + requested_bit_count <= m_actual_cache_size_in_bits)
    {
        SetError(IOE_NONE);
        return;
    }

    // write out the "used" bytes.
    Uint32 bytes_written = FlushBytes(m_cache, GetCacheByteIndex());
    if (bytes_written == GetCacheByteIndex())
        SetError(IOE_NONE);
    else
        SetError(IOE_INSUFFICIENT_STORAGE);

    // shift the unflushed cache data over
    memmove(m_cache, m_cache + bytes_written, m_actual_cache_size_in_bytes);
    // offset the cache bit index, preserving the within-byte bit offset.
    m_cache_bit_index -= bytes_written << 3;
}

void BitCache::EnsureWriteCacheHasEnoughBytesAvailable (
    Uint32 const requested_byte_count) const
{
    ASSERT1(GetIsCacheBitIndexOnByteBoundary())
    EnsureWriteCacheHasEnoughBitsAvailable(requested_byte_count << 3);
}

void BitCache::OpenForReading ()
{
    ASSERT1(!GetIsOpen())
    ASSERT1(GetIODirection() == IOD_NONE)

    m_cache_bit_index = 0;
    m_next_available_cache_byte_index = 0;
    SetIsOpen(true);
    SetIODirection(IOD_READ);
    SetError(IOE_NONE);
}

void BitCache::OpenForWriting ()
{
    ASSERT1(!GetIsOpen())
    ASSERT1(GetIODirection() == IOD_NONE)

    m_cache_bit_index = 0;
    m_next_available_cache_byte_index = m_actual_cache_size_in_bytes + 1;
    // check for overflow
    ASSERT1(m_next_available_cache_byte_index > m_actual_cache_size_in_bytes)

    SetIsOpen(true);
    SetIODirection(IOD_WRITE);
    SetError(IOE_NONE);
}

void BitCache::Close ()
{
    ASSERT1(GetIsOpen())
    ASSERT1(GetIODirection() == IOD_READ ||
            GetIODirection() == IOD_WRITE)

    if (GetIODirection() == IOD_WRITE)
        FlushWriteCache();

    SetIsOpen(false);
    SetIODirection(IOD_NONE);
    SetError(IOE_NONE);
}

} // end of namespace Xrb
