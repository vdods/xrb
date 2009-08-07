// ///////////////////////////////////////////////////////////////////////////
// xrb_bitarray.hpp by Victor Dods, created 2005/05/02
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_BITARRAY_HPP_)
#define _XRB_BITARRAY_HPP_

#include "xrb.hpp"

#include <stdio.h>
#include <string.h>

namespace Xrb
{

template <Uint32 bit_count>
class BitArray
{
public:

    // these values will be computed for each template instantiation of
    // BitArray, and will be computed at compile-time, thus they are 
    // compile-time constants (i.e. no run-time computation is necessary).
    enum
    {
        BIT_COUNT = bit_count,
        HIGHEST_BIT_INDEX = bit_count - 1,
        
        WORD_COUNT = (bit_count - 1) / 32 + 1,
        HIGHEST_WORD_INDEX = (bit_count - 1) / 32,

        // these don't really need to be in this class, but whateva.
        WORD_SIZE_IN_BITS = sizeof(Uint32) * 8,
        WORD_SIZE_IN_BYTES = sizeof(Uint32),
    };

    // really only used for the constructor which fills the bitarray
    // with 1s or 0s.  this is necessary to avoid ambiguity with a
    // constructor which takes an integer.
    enum BitValue
    {
        ZERO = 0,
        ONE = 1
    }; // end of enum BitValue

    // convenient way to specify a bit array of all zeros
    static BitArray<bit_count> const ms_zero;
    // convenient way to specify a bit array of all ones
    static BitArray<bit_count> const ms_one;

    inline BitArray ()
    {
        ASSERT1(bit_count > 0);
        m_words[HIGHEST_WORD_INDEX] &= USED_BIT_MASK;
        ASSERT1(AreUnusedBitsZeroed());
    }
    inline BitArray (BitValue const fill_with)
    {
        ASSERT1(bit_count > 0);
        SetAllBits((fill_with == ONE) ? true : false);
        ASSERT1(AreUnusedBitsZeroed());
    }
    inline BitArray (Uint32 const assign_to_least_significant_word)
    {
        ASSERT1(bit_count > 0);
        // zero out all but the least significant word
    #if defined(XRB_BITARRAY_USES_MEMSET)
        if (WORD_COUNT > 1)
            memset(reinterpret_cast<Uint8 *>(m_words + 1),
                   0x00,
                   (WORD_COUNT - 1) * sizeof(Uint32));
    #else
        if (WORD_COUNT > 1)
            for (Uint32 i = 1; i < WORD_COUNT; ++i)
                m_words[i] = 0x00000000;
    #endif
        // assign the least significant word
        m_words[0] = assign_to_least_significant_word;
        m_words[HIGHEST_WORD_INDEX] &= USED_BIT_MASK;
        ASSERT1(AreUnusedBitsZeroed());
    }
    inline BitArray (BitArray<bit_count> const &source)
    {
        ASSERT1(bit_count > 0);
    #if defined(XRB_BITARRAY_USES_MEMCPY)
        memcpy(reinterpret_cast<Uint8 *>(m_words),
               reinterpret_cast<Uint8 const *>(source.m_words),
               WORD_COUNT * sizeof(Uint32));
    #else
        for (Uint32 i = 0; i < WORD_COUNT; ++i)
            m_words[i] = source.m_words[i];
    #endif
        ASSERT1(AreUnusedBitsZeroed());
    }
    inline ~BitArray ()
    {
        ASSERT1(AreUnusedBitsZeroed());
    }

    inline bool Bit (Uint32 const bit_index) const
    {
        ASSERT1(AreUnusedBitsZeroed());
        ASSERT1(bit_index < bit_count);

        Uint32 major_index = bit_index >> 5; // same as / 32
        Uint32 minor_index = bit_index & 31; // same as % 32

        Uint32 bit_mask = 1 << minor_index;
        return (m_words[major_index] & bit_mask) != 0;
    }
    inline Uint32 Word (Uint32 const word_index) const
    {
        ASSERT1(AreUnusedBitsZeroed());
        ASSERT1(word_index <= HIGHEST_WORD_INDEX);
        return m_words[word_index];
    }

    inline void SetBit (Uint32 const bit_index, bool const value)
    {
        ASSERT1(bit_index < bit_count);

        Uint32 major_index = bit_index >> 5; // same as / 32
        Uint32 minor_index = bit_index & 31; // same as % 32

        Uint32 bit_mask = 1 << minor_index;
        m_words[major_index] ^= bit_mask;
    }
    inline void SetAllBits (bool const value)
    {
        if (value)
        {
        #if defined(XRB_BITARRAY_USES_MEMSET)
            memset(reinterpret_cast<Uint8 *>(m_words),
                   0xFF,
                   WORD_COUNT * sizeof(Uint32));
        #else
            for (Uint32 i = 0; i < WORD_COUNT; ++i)
                m_words[i] = 0xFFFFFFFF;
        #endif
            m_words[HIGHEST_WORD_INDEX] &= USED_BIT_MASK;
        }
        else
        {
        #if defined(XRB_BITARRAY_USES_MEMSET)
            memset(reinterpret_cast<Uint8 *>(m_words),
                   0x00,
                   WORD_COUNT * sizeof(Uint32));
        #else
            for (Uint32 i = 0; i < WORD_COUNT; ++i)
                m_words[i] = 0x00000000;
        #endif
        }
        ASSERT1(AreUnusedBitsZeroed());
    }
    inline void SetWord (Uint32 const word_index, Uint32 const value)
    {
        ASSERT1(word_index <= HIGHEST_WORD_INDEX);
        m_words[word_index] = value;
        m_words[HIGHEST_WORD_INDEX] &= USED_BIT_MASK;
        ASSERT1(AreUnusedBitsZeroed());
    }

    inline void Negate ()
    {
        for (Uint32 i = 0; i < WORD_COUNT; ++i)
            m_words[i] = ~m_words[i];
        m_words[HIGHEST_WORD_INDEX] &= USED_BIT_MASK;
        ASSERT1(AreUnusedBitsZeroed());
    }

    // assignment operator
    inline void operator = (Uint32 const right_operand)
    {
        // zero out all but the least significant word
    #if defined(XRB_BITARRAY_USES_MEMSET)
        if (WORD_COUNT > 1)
            memset(reinterpret_cast<Uint8 *>(m_words + 1),
                   0x00,
                   (WORD_COUNT - 1) * sizeof(Uint32));
    #else
        if (WORD_COUNT > 1)
            for (Uint32 i = 1; i < WORD_COUNT; ++i)
                m_words[i] = 0x00000000;
    #endif
        // assign the least significant word
        m_words[0] = right_operand;
        m_words[HIGHEST_WORD_INDEX] &= USED_BIT_MASK;
        ASSERT1(AreUnusedBitsZeroed());
    }
    // assignment operator
    inline void operator = (BitArray<bit_count> const &right_operand)
    {
    #if defined(XRB_BITARRAY_USES_MEMCPY)
        memcpy(reinterpret_cast<Uint8 *>(m_words),
               reinterpret_cast<Uint8 const *>(right_operand.m_words),
               WORD_COUNT * sizeof(Uint32));
    #else
        for (Uint32 i = 0; i < WORD_COUNT; ++i)
            m_words[i] = right_operand.m_words[i];
    #endif
        ASSERT1(AreUnusedBitsZeroed());
    }

    // equality test operator
    inline bool operator == (BitArray const &right_operand) const
    {
        ASSERT1(AreUnusedBitsZeroed());
        ASSERT1(right_operand.AreUnusedBitsZeroed());
    #if defined(XRB_BITARRAY_USES_MEMCMP)
        return 0 == memcmp(reinterpret_cast<Uint8 const *>(m_words),
                           reinterpret_cast<Uint8 const *>(right_operand.m_words),
                           WORD_COUNT);
    #else
        for (Uint32 i = 0; i < WORD_COUNT; ++i)
            if (m_words[i] != right_operand.m_words[i])
                return false;
        return true;
    #endif
    }
    // inequality test operator
    inline bool operator != (BitArray const &right_operand) const
    {
        ASSERT1(AreUnusedBitsZeroed());
        ASSERT1(right_operand.AreUnusedBitsZeroed());
    #if defined(XRB_BITARRAY_USES_MEMCMP)
        return 0 != memcmp(reinterpret_cast<Uint8 const *>(m_words),
                           reinterpret_cast<Uint8 const *>(right_operand.m_words),
                           WORD_COUNT);
    #else
        for (Uint32 i = 0; i < WORD_COUNT; ++i)
            if (m_words[i] != right_operand.m_words[i])
                return true;
        return false;
    #endif
    }

    // in-place bitwise inclusive OR
    inline void operator |= (BitArray const &right_operand)
    {
        ASSERT1(AreUnusedBitsZeroed());
        ASSERT1(right_operand.AreUnusedBitsZeroed());
        for (Uint32 i = 0; i <= HIGHEST_WORD_INDEX; ++i)
            m_words[i] |= right_operand.m_words[i];
        ASSERT1(AreUnusedBitsZeroed());
    }
    // in-place bitwise AND
    inline void operator &= (BitArray const &right_operand)
    {
        ASSERT1(AreUnusedBitsZeroed());
        ASSERT1(right_operand.AreUnusedBitsZeroed());
        for (Uint32 i = 0; i <= HIGHEST_WORD_INDEX; ++i)
            m_words[i] &= right_operand.m_words[i];
        ASSERT1(AreUnusedBitsZeroed());
    }
    // in-place bitwise exclusive OR
    inline void operator ^= (BitArray const &right_operand)
    {
        ASSERT1(AreUnusedBitsZeroed());
        ASSERT1(right_operand.AreUnusedBitsZeroed());
        for (Uint32 i = 0; i <= HIGHEST_WORD_INDEX; ++i)
            m_words[i] ^= right_operand.m_words[i];
        ASSERT1(AreUnusedBitsZeroed());
    }

    // in-place shift left operator
    inline void operator <<= (Uint32 const right_operand)
    {
        ASSERT1(AreUnusedBitsZeroed());
        if (right_operand == 0)
            return;
        Uint32 i;
        Uint32 word_delta = right_operand >> 5; // same as / 32;
        Uint32 minor_shift = right_operand & 31; // same as % 32
        if (minor_shift == 0)
        {
            for (i = HIGHEST_WORD_INDEX; i >= word_delta; --i)
                m_words[i] = m_words[i-word_delta];
            for (i = word_delta - 1; i < HIGHEST_WORD_INDEX; --i)
                m_words[i] = 0;
        }
        else
        {
            for (i = HIGHEST_WORD_INDEX; i > word_delta; --i)
                m_words[i] = (m_words[i-word_delta] << minor_shift) |
                             (m_words[i-1-word_delta] >> (32 - minor_shift));
            m_words[i] = m_words[i-word_delta] << minor_shift;
            for (i = word_delta - 1; i < HIGHEST_WORD_INDEX; --i)
                m_words[i] = 0;
        }
        m_words[HIGHEST_WORD_INDEX] &= USED_BIT_MASK;
        ASSERT1(AreUnusedBitsZeroed());
    }
    // in-place shift right operator
    inline void operator >>= (Uint32 const right_operand)
    {
        ASSERT1(AreUnusedBitsZeroed());
        if (right_operand == 0)
            return;
        Uint32 i;
        Uint32 word_delta = right_operand >> 5; // same as / 32;
        Uint32 minor_shift = right_operand & 31; // same as % 32;
        if (minor_shift == 0)
        {
            for (i = 0; i <= HIGHEST_WORD_INDEX - word_delta; ++i)
                m_words[i] = m_words[i+word_delta];
            for (i = HIGHEST_WORD_INDEX - word_delta + 1; i <= HIGHEST_WORD_INDEX; ++i)
                m_words[i] = 0;
        }
        else
        {
            for (i = 0; i < HIGHEST_WORD_INDEX - word_delta; ++i)
                m_words[i] = (m_words[i+1+word_delta] << (32 - minor_shift)) |
                             (m_words[i+word_delta] >> minor_shift);
            m_words[i] = m_words[i+word_delta] >> minor_shift;
            for (i = HIGHEST_WORD_INDEX - word_delta + 1; i <= HIGHEST_WORD_INDEX; ++i)
                m_words[i] = 0;
        }
        ASSERT1(AreUnusedBitsZeroed());
    }
    // bitwise negation
    inline BitArray<bit_count> operator ~ ()
    {
        BitArray<bit_count> retval(*this);
        retval.Negate();
        return retval;
    }

    void Fprint (FILE *fptr, Uint32 chunk_size = 8, bool add_newline = true) const;
    void FprintAsWords (FILE *fptr, bool add_newline = true) const;

private:

    enum
    {
        // this is done solely to ensure that the left-shift for the
        // INTERMEDIATE_BIT_MASK value doesn't produce the warning
        // "left shift count >= width of type".
        QUASI_HIGHEST_BIT_INDEX = ((HIGHEST_BIT_INDEX % 32) > 30) ? 30 : (HIGHEST_BIT_INDEX % 32),
        // this value is common to the equations for USED_BIT_MASK
        // and UNUSED_BIT_MASK.
        INTERMEDIATE_BIT_MASK = ((static_cast<Uint32>(1) << (QUASI_HIGHEST_BIT_INDEX + 1)) - 1),
    
        // gives the bit mask of bits that are actually used in the most
        // significant word.  should be exactly equal to ~UNUSED_BIT_MASK.
        USED_BIT_MASK = (bit_count % 32 == 0) ? 0xFFFFFFFF : INTERMEDIATE_BIT_MASK,
        
        // gives the bit mask of bits that are not used in the most
        // significant word.  should be exactly equal to ~USED_BIT_MASK.
        UNUSED_BIT_MASK = (bit_count % 32 == 0) ? 0x00000000 : ~INTERMEDIATE_BIT_MASK
    };

    // function only used as a diagnostic to assert that the possibly
    // unused high bits on the last m_words element are zeroed (which
    // is a condition for correct operation).
    inline bool AreUnusedBitsZeroed () const
    {
        return (m_words[HIGHEST_WORD_INDEX] & UNUSED_BIT_MASK) == 0;
    }

    // contains the bits (least significant word first)
    Uint32 m_words[WORD_COUNT];
}; // end of class BitArray

template <Uint32 bit_count>
BitArray<bit_count> const BitArray<bit_count>::ms_zero(ZERO);

template <Uint32 bit_count>
BitArray<bit_count> const BitArray<bit_count>::ms_one(ONE);

template <Uint32 bit_count>
void BitArray<bit_count>::Fprint (
    FILE *const fptr,
    Uint32 const chunk_size,
    bool const add_newline) const
{
    fprintf(fptr, "BitArray (%u) = (", bit_count);
    for (Uint32 i = HIGHEST_BIT_INDEX; i <= HIGHEST_BIT_INDEX; --i)
    {
        fputc(Bit(i) ? '1' : '0', fptr);
        if (i != 0 &&
            chunk_size != 0 &&
            i % chunk_size == 0)
        {
            fputc(' ', fptr);
        }
    }
    fprintf(fptr, ")%c", add_newline ? '\n' : '\0');
}

template <Uint32 bit_count>
void BitArray<bit_count>::FprintAsWords (
    FILE *const fptr,
    bool const add_newline) const
{
    fprintf(fptr, "BitArray (%u) words = (", bit_count);
    for (Uint32 i = HIGHEST_WORD_INDEX; i <= HIGHEST_WORD_INDEX; --i)
    {
        fprintf(fptr, "%08X", Word(i));
        if (i > 0)
            fputc(' ', fptr);
    }
    fprintf(fptr, ")%c", add_newline ? '\n' : '\0');
}

template <Uint32 bit_count>
inline BitArray<bit_count> operator | (
    BitArray<bit_count> const &left_operand,
    BitArray<bit_count> const &right_operand)
{
    BitArray<bit_count> retval(left_operand);
    retval |= right_operand;
    return retval;
}

template <Uint32 bit_count>
inline BitArray<bit_count> operator & (
    BitArray<bit_count> const &left_operand,
    BitArray<bit_count> const &right_operand)
{
    BitArray<bit_count> retval(left_operand);
    retval &= right_operand;
    return retval;
}

template <Uint32 bit_count>
inline BitArray<bit_count> operator ^ (
    BitArray<bit_count> const &left_operand,
    BitArray<bit_count> const &right_operand)
{
    BitArray<bit_count> retval(left_operand);
    retval ^= right_operand;
    return retval;
}

template <Uint32 bit_count>
inline BitArray<bit_count> operator << (
    BitArray<bit_count> const &left_operand,
    Uint32 const right_operand)
{
    BitArray<bit_count> retval(left_operand);
    retval <<= right_operand;
    return retval;
}

template <Uint32 bit_count>
inline BitArray<bit_count> operator >> (
    BitArray<bit_count> const &left_operand,
    Uint32 const right_operand)
{
    BitArray<bit_count> retval(left_operand);
    retval >>= right_operand;
    return retval;
}

} // end of namespace Xrb

#endif // !defined(_XRB_BITARRAY_HPP_)
