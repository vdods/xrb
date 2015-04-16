// ///////////////////////////////////////////////////////////////////////////
// xrb_endian.hpp by Victor Dods, created 2005/05/14
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined (_XRB_ENDIAN_HPP_)
#define _XRB_ENDIAN_HPP_

#include "xrb.hpp"

namespace Xrb {
namespace Endianness {
    
/// Enum values for specifying little and big endian, as well as this machine's endianness.
enum Value
{
    LITTLE = 0,
    BIG,

    // The target's endianness is set from the value in config.h
    #if defined(WORDS_BIGENDIAN)
    OF_TARGET = BIG,
    #else // !defined(WORDS_BIGENDIAN)
    OF_TARGET = LITTLE,
    #endif // !defined(WORDS_BIGENDIAN)
}; // end of enum Endianness::Value

} // end of namespace Endianness

/// Type-unsafe template functions for byte-order switching.  Use @c Word<T> instead.
template <Uint32 word_size>
struct WordOfSize
{
    /// @brief Switches the byte-order of a single word.
    /// @param word Must point to a piece of memory containing at least word_size bytes.
    static void SwitchByteOrder (Uint8 *word)
    {
        ASSERT1(word != NULL);
        ASSERT0(word_size % 2 == 0); // TODO: compile-time assert
        // there's probably some slick bit op way to do this, but you can K.I.S.S. my ass!
        Uint8 temp;
        for (Uint8 *left = word,
                   *right = word + word_size - 1;
             left < right;
             ++left, --right)
        {
            temp = *left;
            *left = *right;
            *right = temp;
        }
    }
    /// @brief Switches the byte-order of the first @c word_count elements in the given array.
    /// @param array Must point to a piece of memory containing at least word_size*word_count bytes.
    /// @param word_count The number of words to byte-order switch.
    static void SwitchByteOrder (Uint8 *array, Uint32 word_count)
    {
        ASSERT1(array != NULL);
        for (Uint32 i = 0; i < word_count; ++i)
            SwitchByteOrder<word_size>(array[i*word_size]);
    }
}; // end of struct WordOfSize<T>

/// Partial template specialization for the 1-byte case (in which byte-order switching is a no-op).
template <>
struct WordOfSize<1>
{
    static void SwitchByteOrder (Uint8 *word) { }
    static void SwitchByteOrder (Uint8 *array, Uint32 word_count) { }
}; // end of struct WordOfSize<1>

/// Type-safe template functions for byte-order switching.
template <typename T>
struct Word
{
    /// Switches the byte-order of a single word.
    static void SwitchByteOrder (T &word)
    {
        WordOfSize<sizeof(T)>::SwitchByteOrder(reinterpret_cast<Uint8 *>(&word));
    }
    /// @brief Switches the byte-order of the first @c word_count elements in the given array.
    /// @param array Must point to an array having at least @c word_count elements.
    static void SwitchByteOrder (T *array, Uint32 word_count)
    {
        WordOfSize<sizeof(T)>::SwitchByteOrder(reinterpret_cast<Uint8 *>(array), word_count);
    }
}; // end of struct Word<T>

/// Type-unsafe function for switching the byte-order of a word of a given size.
inline void SwitchByteOrder (Uint8 *word, Uint32 word_size)
{
    ASSERT1(word != NULL);
    ASSERT1(word_size > 0);
    
    if (word_size == 1)
        return;

    ASSERT1(word_size % 2 == 0);
    
    // there's probably some slick bit op way to do this, but you can K.I.S.S. my ass!
    Uint8 temp;
    for (Uint8 *left = word,
               *right = word + word_size - 1;
         left < right;
         ++left, --right)
    {
        temp = *left;
        *left = *right;
        *right = temp;
    }
}

/// Type-unsafe function for switching the byte-order of each word of a given size in an array.
inline void SwitchByteOrder (Uint8 *array, Uint32 word_size, Uint32 word_count)
{
    ASSERT1(array != NULL);
    ASSERT1(word_size > 0);
    
    if (word_size == 1)
        return; // no-op

    ASSERT1(word_size % 2 == 0);

    for (Uint32 i = 0; i < word_count; ++i)
        SwitchByteOrder(array+i*word_size, word_size);
}

} // end of namespace Xrb

#endif // !defined(_XRB_ENDIAN_HPP_)
