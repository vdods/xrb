// ///////////////////////////////////////////////////////////////////////////
// xrb_math.cpp by Victor Dods, created 2005/06/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_math.hpp"

#include <stdlib.h>

namespace Xrb
{

namespace Math
{

Float CanonicalAngle (Float angle)
{
    Float retval = angle;

    if (retval < -180.0f)
        retval -= 360.0f * static_cast<Sint32>((retval - 180.0f) / 360.0f);
    else if (retval > 180.0f)
        retval -= 360.0f * static_cast<Sint32>((retval + 180.0f) / 360.0f);

    return retval;
}

Float Pow (Float base, Uint32 exponent)
{
    Float retval = 1.0f;
    Uint32 bit = 1;
    Float accumulator = base;
    while (bit <= exponent)
    {
        if ((bit & exponent) != 0)
            retval *= accumulator;
        accumulator *= accumulator;
        bit <<= 1;
    }
    return retval;
}

Uint32 PowInt (Uint32 base, Uint32 exponent)
{
    ASSERT1((base > 0 || exponent > 0) && "0^0 is undefined");
    Uint32 retval = 1;
    Uint32 bit = 1;
    Uint32 accumulator = base;
    while (bit <= exponent)
    {
        if ((bit & exponent) != 0)
            retval *= accumulator;
        accumulator *= accumulator;
        bit <<= 1;
    }
    return retval;
}

Uint32 LogInt (Uint32 base, Uint32 argument)
{
    ASSERT1(base > 0 && argument > 0 && "invalid base or argument");
    Uint32 retval = 0;
    while (argument >= base)
    {
        ++retval;
        argument /= base;
    }
    return retval;
}

Float RandomFloat (Float lower_bound, Float upper_bound)
{
    ASSERT1(lower_bound <= upper_bound);
    static Float const s_rand_max = static_cast<Float>(RAND_MAX);
    return
        (upper_bound - lower_bound) *
        static_cast<Float>(rand()) /
        s_rand_max
        +
        lower_bound;
}

Uint16 RandomUint16 (Uint16 lower_bound, Uint16 upper_bound)
{
    // WIN32's RAND_MAX is 65535.  TODO: implement a mersenne twister
#if !defined(WIN32)
    ASSERT1(RAND_MAX >= 65536); // TODO: somehow turn this into a compile time check or something
#endif
    ASSERT1(lower_bound <= upper_bound);
    Uint16 range = upper_bound + 1 - lower_bound;
    if (range == 0)
        return static_cast<Uint16>(rand());
    else
        return static_cast<Uint16>(rand()) % range + lower_bound;
}

Uint32 HighestBitIndex (Uint32 x)
{
    static Uint32 const s_highest_bit_index_table[0x10] =
    {
        0, 0, 1, 1,
        2, 2, 2, 2,
        3, 3, 3, 3,
        3, 3, 3, 3
    };
    if ((x & 0xFFFF0000) != 0)
        if ((x & 0xFF000000) != 0)
            if ((x & 0xF0000000) != 0)
                return 28 + s_highest_bit_index_table[x >> 28];
            else
                return 24 + s_highest_bit_index_table[x >> 24];
        else
            if ((x & 0x00F00000) != 0)
                return 20 + s_highest_bit_index_table[x >> 20];
            else
                return 16 + s_highest_bit_index_table[x >> 16];
    else
        if ((x & 0x0000FF00) != 0)
            if ((x & 0x0000F000) != 0)
                return 12 + s_highest_bit_index_table[x >> 12];
            else
                return 8 + s_highest_bit_index_table[x >> 8];
        else
            if ((x & 0x000000F0) != 0)
                return 4 + s_highest_bit_index_table[x >> 4];
            else
                return s_highest_bit_index_table[x];
}

} // end of namespace Math

} // end of namespace Xrb

