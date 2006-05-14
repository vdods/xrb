// ///////////////////////////////////////////////////////////////////////////
// xrb_math.h by Victor Dods, created 2005/06/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_MATH_H_)
#define _XRB_MATH_H_

#include "xrb.h"

#if defined(WIN32)
    // this is for _finite()
    #include <float.h>
#endif // defined(WIN32)
#include <math.h>
#include <stdlib.h>

#include "xrb_vector.h"

namespace Xrb
{

/** There are also a set of "fast" versions of some of the functions, which
  * provide fast, but less-accurate results of the respective function.
  *
  * All angles in XuqRijBuh are in degrees (unless otherwise specified), for
  * representational convenience.
  *
  * @brief Contains mathematical functions.
  */
namespace Math
{

    /** @brief Converts degrees to radians.
      */
    inline Float Radians (Float const angle)
    {
        static Float const conversion_factor = static_cast<Float>(M_PI) / 180.0f;
        return angle * conversion_factor;
    }
    /** @brief Converts radians to degrees.
      */
    inline Float Degrees (Float const radians)
    {
        static Float const conversion_factor = 180.0f / static_cast<Float>(M_PI);
        return radians * conversion_factor;
    }
    /** This should only be used on positional angles, not velocity angles.
      * @brief Returns an equivalent angle in the range [-180, 180].
      */
    Float GetCanonicalAngle (Float angle);

    /** @brief Returns the sine of the given angle.
      */
    inline Float Sin (Float const angle)
    {
        return sinf(Radians(angle));
    }
    /** @brief Returns the arcsine of the given value.
      */
    inline Float Asin (Float const value)
    {
        return Degrees(asinf(value));
    }
    /** @brief Returns the cosine of the given angle.
      */
    inline Float Cos (Float const angle)
    {
        return cosf(Radians(angle));
    }
    /** @brief Returns the arccosine of the given value.
      */
    inline Float Acos (Float const value)
    {
        return Degrees(acosf(value));
    }
    /** @brief Returns the tangent of the given angle.
      */
    inline Float Tan (Float const angle)
    {
        return tanf(Radians(angle));
    }
    /** @brief Returns the arctangent of the given angle.
      */
    inline Float Atan (Float const value)
    {
        return Degrees(atanf(value));
    }
    /** @brief Returns the angle that the given vector is making
      *        with the positive X axis.
      */
    inline Float Atan (FloatVector2 const &vector)
    {
        return Degrees(atan2f(vector[Dim::Y], vector[Dim::X]));
    }
    /** This is used when @c x may be zero.
      * @brief Returns the arctangent of the ratio given by @c y/x.
      */
    inline Float Atan2 (Float const y, Float const x)
    {
        return Degrees(atan2f(y, x));
    }
    /** @brief Returns the unit vector which makes the given angle
      *        with the positive X axis.
      */
    inline FloatVector2 UnitVector (Float const angle)
    {
        Float angle_in_radians = Radians(angle);
        return FloatVector2(cosf(angle_in_radians), sinf(angle_in_radians));
    }
    /** @brief Returns the positive square root of @c x.
      */
    inline Float Sqrt (Float const x)
    {
        return sqrtf(x);
    }
    /** @brief Returns @c base raised to the @c exponent power.
      */
    inline Float Pow (Float const base, Float const exponent)
    {
        return powf(base, exponent);
    }
    /** @brief Returns @c base raised to the integer @c exponent power.
      */
    Float PowInt (Float base, Uint32 exponent);
    /** @brief Returns @c x if @x is non-negative, otherwise @c -x.
      */
    inline Float Abs (Float x)
    {
        return (x >= 0.0f) ? x : -x;
    }
    /** @brief Returns a random Float within the range [lower_bound, upper_bound].
      */
    Float RandomFloat (Float lower_bound, Float upper_bound);
    /** The default parameter values are the lower and upper bounds of the
      * Uint16 type 0 and 65535 respectively.
      * @brief Returns a random Uint16 within the range [lower_bound, upper_bound].
      */
    Uint16 RandomUint16 (Uint16 lower_bound = 0, Uint16 upper_bound = 65535);
    /** @brief Returns a random Float within 0.0f and 360.0f (an angle).
      */
    inline Float RandomAngle ()
    {
        return RandomFloat(static_cast<Float>(0), static_cast<Float>(360));
    }
    /** @brief Returns the constant "e" to the given exponent.
      */
    inline Float ExpBaseE (Float const exponent)
    {
        return expf(exponent);
    }
    /** @brief Returns 10 raised to the given exponent.
      */
    inline Float ExpBase10 (Float const exponent)
    {
        return powf(static_cast<Float>(10), exponent);
    }
    /** @brief Returns the natural logarithm of @c x.
      */
    inline Float LogBaseE (Float const x)
    {
        return logf(x);
    }
    /** @brief Returns the base-10 logarithm of @c x.
      */
    inline Float LogBase10 (Float const x)
    {
        return log10f(x);
    }
    /** If @c x is zero, then zero will be returned (even though there are
      * no bits set in the integer representation of zero.
      * @brief Returns the zero-based index of the highest bit in @c x
      *        (an unsigned, 32 bit value).
      */
    Uint32 HighestBitIndex (Uint32 x);
    /** @brief Returns the floating-point representation of not-a-number.
      */
    inline Float Nan ()
    {
        // hacky type way to get the IEEE single precision floating
        // point representation of not-a-number.
    #if defined(WORDS_BIGENDIAN)
        static Uint8 const s_nan_bytes[8] = { 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
        static Float const s_nan = *reinterpret_cast<Float const *>(s_nan_bytes);
    #else // !defined(WORDS_BIGENDIAN)
        static Uint8 const s_nan_bytes[8] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F };
        static Float const s_nan = *reinterpret_cast<Float const *>(s_nan_bytes + 8 - sizeof(Float));
    #endif // !defined(WORDS_BIGENDIAN)

        return s_nan;
    }
    /** @brief Returns true iff the number is a normal finite number.  i.e.
      *        not NaN or Inf
      */
    inline bool IsFinite (Float const x)
    {
    #if defined(WIN32)
        return _finite(static_cast<double>(x)) != 0;
    #else // !defined(WIN32)
        return isfinite(x);
    #endif
    }

    /** @brief Returns the sine of the given angle, with lower precision.
      */
    Float FastSin (Float angle);
    /** @brief Returns the arcsine of the given value, with lower precision.
      */
    Float FastAsin (Float value);
    /** @brief Returns the cosine of the given angle, with lower precision.
      */
    Float FastCos (Float angle);
    /** @brief Returns the arccosine of the given value, with lower precision.
      */
    Float FastAcos (Float value);
    /** @brief Returns the tangent of the given angle, with lower precision.
      */
    Float FastTan (Float angle);
    /** @brief Returns the arctangent of the given value, with lower
      *        precision.
      */
    Float FastAtan (Float value);
    /** @brief Returns the angle that the given vector is making
      *        with the positive X axis, with lower precision.
      */
    Float FastAtan (FloatVector2 const &vector);
    /** This is used when @c x may be zero.
      * @brief Returns the arctangent of the ratio given by @c y/x, with
      *        lower precision.
      */
    Float FastAtan2 (Float y, Float x);
    /** @brief Returns the unit vector which makes the given angle
      *        with the positive X axis, with lower precision.
      */
    FloatVector2 FastUnitVector (Float angle);
    /** @brief Returns the positive square root of @c x, with lower precision.
      */
    Float FastSqrt (Float x);
    /** @brief Returns @c base raised to the @c exponent power, with lower
      *        precision.
      */
    Float FastPow (Float base, Float exponent);

    /** @brief Returns the floating-point representation of the nearest
      *        integer less than or equal to @c x.
      */
    inline Float Floor (Float const x)
    {
        return floorf(x);
    }
    /** @brief Returns the floating-point representation of the nearest
      *        integer greater than or equal to @c x.
      */
    inline Float Ceiling (Float const x)
    {
        return ceilf(x);
    }
    /** @brief Returns the floating-point representation of the integer
      *        nearest to @c x.
      */
    inline Float Round (Float const x)
    {
        return floorf(x + 0.5f);
    }
} // end of namespace Math

} // end of namespace Xrb

#endif // !defined(_XRB_MATH_H_)

