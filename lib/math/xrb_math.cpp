// ///////////////////////////////////////////////////////////////////////////
// xrb_math.cpp by Victor Dods, created 2005/06/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_math.h"

#include <stdlib.h>

namespace Xrb
{

Float Math::GetCanonicalAngle (Float const angle)
{
    Float retval = angle;

    if (retval < -180.0f)
        retval -= 360.0f * static_cast<Sint32>((retval - 180.0f) / 360.0f);
    else if (retval > 180.0f)
        retval -= 360.0f * static_cast<Sint32>((retval + 180.0f) / 360.0f);

    return retval;
}

Float Math::PowInt (Float const base, Uint32 const exponent)
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

Float Math::RandomFloat (Float const lower_bound, Float const upper_bound)
{
    ASSERT1(lower_bound <= upper_bound)
    static Float const s_rand_max = static_cast<Float>(RAND_MAX);
    return
        (upper_bound - lower_bound) *
        static_cast<Float>(rand()) /
        s_rand_max
        +
        lower_bound;
}

Uint16 Math::RandomUint16 (Uint16 const lower_bound, Uint16 const upper_bound)
{
    // WIN32's RAND_MAX is 65535.  TODO: implement a mersenne twister
#if !defined(WIN32)
    ASSERT1(RAND_MAX >= 65536) // TODO: somehow turn this into a compile time check or something
#endif
    ASSERT1(lower_bound <= upper_bound)
    Uint16 range = upper_bound + 1 - lower_bound;
    if (range == 0)
        return static_cast<Uint16>(rand());
    else
        return static_cast<Uint16>(rand()) % range + lower_bound;
}

Uint32 Math::HighestBitIndex (Uint32 const x)
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

Float Math::FastSin (Float const angle)
{
    ASSERT0(false && "Not implemented yet");
    return Sin(angle);
}

Float Math::FastAsin (Float const value)
{
    ASSERT0(false && "Not implemented yet");
    return Asin(value);
}

Float Math::FastCos (Float const angle)
{
    ASSERT0(false && "Not implemented yet");
    return Cos(angle);
}

Float Math::FastAcos (Float const value)
{
    ASSERT0(false && "Not implemented yet");
    return Acos(value);
}

Float Math::FastTan (Float const angle)
{
    ASSERT0(false && "Not implemented yet");
    return Tan(angle);
}

Float Math::FastAtan (Float const value)
{
    ASSERT0(false && "Not implemented yet");
    return Atan(value);
}

Float Math::FastAtan (FloatVector2 const &vector)
{
    ASSERT0(false && "Not implemented yet");
    return Atan(vector);
}

Float Math::FastAtan2 (Float const y, Float const x)
{
    ASSERT0(false && "Not implemented yet");
    return Atan2(y, x);
}

FloatVector2 Math::FastUnitVector (Float angle)
{
    ASSERT0(false && "Not implemented yet");
    return UnitVector(angle);
}

Float Math::FastSqrt (Float const x)
{
    ASSERT0(false && "Not implemented yet");
    return Sqrt(x);
}

Float Math::FastPow (Float const base, Float const exponent)
{
    ASSERT0(false && "Not implemented yet");
    return Pow(base, exponent);
}

} // end of namespace Xrb

