// ///////////////////////////////////////////////////////////////////////////
// xrb_vector.hpp by Victor Dods, created 2005/04/22
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_VECTOR_HPP_)
#define _XRB_VECTOR_HPP_

#include "xrb.hpp"

#include "xrb_enums.hpp"
#include "xrb_serializer.hpp"

namespace Xrb {

// this is necessary to avoid a circular header inclusion nightmare
namespace Math {
    Float Sqrt (Float x);
} // end of namespace Math

// ///////////////////////////////////////////////////////////////////////////
// Vector - generalized type vector with arbitrary dimension
// ///////////////////////////////////////////////////////////////////////////

/** The scalar template type given for each instantiation of Vector must
  * have certain operations defined.  Namely, addition, subtraction,
  * unary negation, multiplication, division and a square root function.
  *
  * The type must be a regular assignable type (not a type with possibly a
  * vtable, and definitely not a type which requires deep copying to perform
  * assignment).  This is because if XRB_VECTOR_USES_MEMCPY is defined,
  * memcpy() is used to increase efficiency on certain operations.  Also,
  * if XRB_VECTOR_USES_MEMCMP is defined, memcmp() is used on certain
  * operations.  The default behavior is to iterate over each component
  * for operations.
  *
  * @brief Generalized vector class geared towards high speed efficiency.
  * @note For each instantiation of Vector, there must be a corresponding
  *       instance of ms_zero.
  */
template <typename T, Uint32 dimension>
class Vector
{
public:

    /** This member serves as the type-specific zero vector for use in
      * the IsZero and IsNonZero functions, as well as providing a fast,
      * convenient and elegant way to refer to possibly the most commonly
      * used vector value.
      * @brief The zero vector.
      */
    static Vector<T, dimension> const ms_zero;

    /// The array which stores the vector's components
    T m[dimension];

    // ///////////////////////////////////////////////////////////////////////
    // constructors and destructor
    // ///////////////////////////////////////////////////////////////////////

    /** Does no initialization for the sake of efficiency.
      * @brief Default constructor.
      */
    Vector ()
    {
    }
    /** Does/can not use any memcpy/memset functions.
      * @brief Constructs a Vector, setting each component to @c fill_with.
      * @param fill_with The value to set each component to.
      */
    explicit Vector (T const fill_with)
    {
        for (Uint32 i = 0; i < dimension; ++i)
            m[i] = fill_with;
    }
    /** I gave up trying for partial template specialization (or whatever it's
      * called) to specify the two-parameter constructor for only
      * Vector<T, 2>, and settled for asserting that @c dimension is 2.
      * @brief Constructs a vector with the specified ordered pair.
      * @param component0 The value to assign to the first component (m[0]).
      * @param component1 The value to assign to the second component (m[1]).
      */
    Vector (T const component0, T const component1)
    {
        ASSERT1(dimension == 2);
        m[0] = component0;
        m[1] = component1;
    }
    /** I gave up trying for partial template specialization (or whatever it's
      * called) to specify the three-parameter constructor for only
      * Vector<T, 3>, and settled for asserting that @c dimension is 3.
      * @brief Constructs a vector with the specified ordered triplet.
      * @param component0 The value to assign to the first component (m[0]).
      * @param component1 The value to assign to the second component (m[1]).
      * @param component2 The value to assign to the third component (m[2]).
      */
    Vector (T const component0, T const component1, T const component2)
    {
        ASSERT1(dimension == 3);
        m[0] = component0;
        m[1] = component1;
        m[2] = component2;
    }
    /** @brief Constructs a vector from an array of component values.
      * @param components The array of ordered components to construct this
      *                   vector with.  Must be at least @c dimension
      *                   elements long.
      */
    Vector (T const *const components)
    {
        ASSERT1(components != NULL);
    #if defined(XRB_VECTOR_USES_MEMCPY)
        memcpy(m, components, sizeof(T) * dimension);
    #else
        for (Uint32 i = 0; i < dimension; ++i)
            m[i] = components[i];
    #endif
    }
    /** @brief Copy constructor.
      * @param source The vector to copy.
      */
    Vector (Vector<T, dimension> const &source)
    {
    #if defined(XRB_VECTOR_USES_MEMCPY)
        memcpy(m, source.m, sizeof(T) * dimension);
    #else
        for (Uint32 i = 0; i < dimension; ++i)
            m[i] = source.m[i];
    #endif
    }
    /** No cleanup is necessary.
      * @brief Destructor.
      */
    ~Vector ()
    {
    }

    // ///////////////////////////////////////////////////////////////////////
    // overloaded operators
    // ///////////////////////////////////////////////////////////////////////

    /** @brief Assignment operator.
      * @param operand The vector to assign.
      */
    void operator = (Vector<T, dimension> const &operand)
    {
    #if defined(XRB_VECTOR_USES_MEMCPY)
        memcpy(m, operand.m, sizeof(T) * dimension);
    #else
        for (Uint32 i = 0; i < dimension; ++i)
            m[i] = operand.m[i];
    #endif
    }
    /** @brief Equality operator.
      * @param operand The vector to test equality against.
      * @return True iff each component of operand is equal to the
      *         corresponding component of this vector.
      */
    bool operator == (Vector<T, dimension> const &operand) const
    {
    #if defined(XRB_VECTOR_USES_MEMCMP)
        return memcmp(m, operand.m, sizeof(T) * dimension) == 0;
    #else
        for (Uint32 i = 0; i < dimension; ++i)
            if (m[i] != operand.m[i])
                return false;
        return true;
    #endif
    }
    /** @brief Inequality operator.
      * @param operand The vector to test inequality against.
      * @return True iff any component of operand is unequal to the
      *         corresponding component of this vector.
      */
    bool operator != (Vector<T, dimension> const &operand) const
    {
    #if defined(XRB_VECTOR_USES_MEMCMP)
        return memcmp(m, operand.m, sizeof(T) * dimension) != 0;
    #else
        for (Uint32 i = 0; i < dimension; ++i)
            if (m[i] != operand.m[i])
                return true;
        return false;
    #endif
    }
    /** Array index referencing.  Emulates the syntax of a normal array,
      * giving a more appealing usage (vector[1] instead of vector.m[1]).
      * @brief Bracket operator with const qualification.
      * @param index The index of the component to reference.
      * @return A const reference to the desired vector component.
      */
    T const &operator [] (Uint32 const index) const
    {
        ASSERT3(index < dimension);
        return m[index];
    }
    /** Array index referencing.  Emulates the syntax of a normal array,
      * giving a more appealing usage (vector[1] instead of vector.m[1]).
      * @brief Bracket operator.
      * @param index The index of the component to reference.
      * @return A non-const reference to the desired vector component.
      */
    T &operator [] (Uint32 const index)
    {
        ASSERT3(index < dimension);
        return m[index];
    }
    /** Performs vector addition on this and the given vector, storing
      * the result in this vector.
      * @brief In-place addition operator.
      * @param operand The vector to add to this vector.
      */
    void operator += (Vector<T, dimension> const &operand)
    {
        for (Uint32 i = 0; i < dimension; ++i)
            m[i] += operand.m[i];
    }
    /** Performs vector subtraction on this and the given vector, storing
      * the result in this vector.
      * @brief In-place addition operator.
      * @param operand The vector to subtract from this vector.
      */
    void operator -= (Vector<T, dimension> const &operand)
    {
        for (Uint32 i = 0; i < dimension; ++i)
            m[i] -= operand.m[i];
    }
    /** Performs scalar multiplication of the components of @c operand
      * on each respective component of this vector, storing the
      * result in this vector.
      * @brief In-place multiplication operator.
      * @param operand The vector to component-wise multiply this
      *                vector by.
      */
    void operator *= (Vector<T, dimension> const &operand)
    {
        for (Uint32 i = 0; i < dimension; ++i)
            m[i] *= operand.m[i];
    }
    /** Performs scalar multiplication on this vector by the given scalar
      * storing the result in this vector.
      * @brief In-place multiplication operator.
      * @param operand The scalar value to multiply this vector by.
      */
    void operator *= (T const &operand)
    {
        for (Uint32 i = 0; i < dimension; ++i)
            m[i] *= operand;
    }
    /** Performs scalar division of the components of this vector
      * by each respective component of @c operand, storing the
      * result in this vector.
      * @brief In-place division operator.
      * @param operand The vector to component-wise divide this
      *                vector by.
      * @note There is no protection here against dividing by zero.
      */
    void operator /= (Vector<T, dimension> const &operand)
    {
        for (Uint32 i = 0; i < dimension; ++i)
            m[i] /= operand.m[i];
    }
    /** Performs scalar division on this vector by the given scalar
      * storing the result in this vector.
      * @brief In-place division operator.
      * @param operand The scalar value to divide this vector by.
      * @note There is no protection here against dividing by zero.
      */
    void operator /= (T const &operand)
    {
        for (Uint32 i = 0; i < dimension; ++i)
            m[i] /= operand;
    }
    /** @brief Unary negation operator.
      * @return The arithmatic negative of this vector.
      */
    Vector<T, dimension> operator - () const
    {
        Vector<T, dimension> retval;
        for (Uint32 i = 0; i < dimension; ++i)
            retval.m[i] = -m[i];
        return retval;
    }

    // ///////////////////////////////////////////////////////////////////////
    // accessors
    // ///////////////////////////////////////////////////////////////////////

    /** Uses a square root function specific to Vector<T, dimension, Float>,
      * and thus for integral scalar types, the actual vector length after
      * this procedure may not be 1.
      * @brief Returns the length of this vector.
      * @return The length of this vector.
      */
    T Length () const
    {
        return Math::Sqrt(*this | *this);
    }
    /** This is as close as you can come to the length of a vector without
      * needing a square root function.
      * @brief Returns the square of the length of this vector.
      * @return The square of the length of this vector.
      */
    T LengthSquared () const
    {
        return *this | *this;
    }
    /** Uses a square root function specific to Vector<T, dimension, Float>,
      * and thus for integral scalar types, the actual vector length after
      * this procedure may not be 1.
      * @brief Returns the normalized version of this vector.
      * @return The normalized version of this vector.
      */
    Vector<T, dimension> Normalization () const
    {
        Vector<T, dimension> retval(*this);
        retval.Normalize();
        return retval;
    }
    /** @brief Test for equality with the zero vector.
      * @return True iff each component of the zero vector is equal to the
      *         corresponding component of this vector.
      */
    bool IsZero () const
    {
    #if defined(XRB_VECTOR_USES_MEMCMP)
        return memcmp(m, ms_zero.m, sizeof(T) * dimension) == 0;
    #else
        for (Uint32 i = 0; i < dimension; ++i)
            if (m[i] != ms_zero.m[i])
                return false;
        return true;
    #endif
    }

    /** Performs static_cast on each of this Vector's components
      * to convert the values to the desired.
      * @brief The Vector equivalent to static_cast.
      */
    template <typename U>
    Vector<U, dimension> StaticCast () const
    {
        Vector<U, dimension> retval;
        for (Uint32 i = 0; i < dimension; ++i)
            retval.m[i] = static_cast<U>(m[i]);
        return retval;
    }

    // ///////////////////////////////////////////////////////////////////////
    // modifiers
    // ///////////////////////////////////////////////////////////////////////

    /** @brief Sets each component in this vector to the specified value.
      * @param fill_with The value to assign to each vector component.
      */
    void FillWith (T const fill_with)
    {
        for (Uint32 i = 0; i < dimension; ++i)
            m[i] = fill_with;
    }
    /** The in-place negation of this vector, or to put another way,
      * @code ms_zero - *this @endcode (or at least it better equal that!).
      * @brief Sets this vector to the negative of itself.
      */
    void Negate ()
    {
        for (Uint32 i = 0; i < dimension; ++i)
            m[i] = -m[i];
    }
    /** Uses a square root function specific to Vector<T, dimension, Float>,
      * and thus for integral scalar types, the actual vector length after
      * this procedure may not be 1.
      * @brief Sets this vector's length to 1, while preserving its direction.
      */
    void Normalize ()
    {
        operator/=(Math::Sqrt(*this | *this));
    }
    /** I gave up trying for partial template specialization (or whatever it's
      * called) to specify the two-parameter SetComponents for only
      * Vector<T, 2>, and settled for asserting that the dimension is 2.
      * @brief Assigns the specified ordered pair to this vector.
      * @param component0 The value to assign to the first component (m[0]).
      * @param component1 The value to assign to the second component (m[1]).
      */
    void SetComponents (T const component0, T const component1)
    {
        ASSERT1(dimension == 2);
        m[0] = component0;
        m[1] = component1;
    }
    /** I gave up trying for partial template specialization (or whatever it's
      * called) to specify the three-parameter SetComponents for only
      * Vector<T, 3>, and settled for asserting that the dimension is 3.
      * @brief Assigns the specified ordered triplet to this vector.
      * @param component0 The value to assign to the first component (m[0]).
      * @param component1 The value to assign to the second component (m[1]).
      * @param component2 The value to assign to the third component (m[2]).
      */
    void SetComponents (T const component0, T const component1, T const component2)
    {
        ASSERT1(dimension == 3);
        m[0] = component0;
        m[1] = component1;
        m[2] = component2;
    }
    /** @brief Assigns the specified ordered array of components to the components
      *        of this vector.
      * @param components The array of ordered components to assign
      *                   to this vector.  Must be at least @c dimension
      *                   elements long.
      */
    void SetComponents (T const *const components)
    {
        ASSERT1(components != NULL);
    #if defined(XRB_VECTOR_USES_MEMCPY)
        memcpy(m, components, sizeof(T) * dimension);
    #else
        for (Uint32 i = 0; i < dimension; ++i)
            m[i] = components[i];
    #endif
    }
    /** Performs static_cast on each of the operand vector's components
      * to convert the values.
      * @brief The Vector equivalent to vector1 = static_cast<T>(vector2).
      * @param source The vector to assign.
      */
    template <typename U>
    void StaticCastAssign (Vector<U, dimension> const &source)
    {
        for (Uint32 i = 0; i < dimension; ++i)
            m[i] = static_cast<T>(source.m[i]);
    }
}; // end of template <typename T> class Vector

/** This templatized definition will take care of defining the static
  * @c ms_zero member vector, assuming that @c static_cast<T>(0) work.
  * @brief Templatized static definition of the @c ms_zero vector.
  */
template <typename T, Uint32 dimension>
Vector<T, dimension> const Vector<T, dimension>::ms_zero(static_cast<T>(0));

// ///////////////////////////////////////////////////////////////////////////
// Operator overloads
// ///////////////////////////////////////////////////////////////////////////

/** Performs vector addition (a commutative operation) on the given vectors.
  * @brief Global addition operator.
  * @param l The vector on the left side of the operation.
  * @param r The vector on the right side of the operation.
  * @return The vector sum of the given vectors.
  */
template <typename T, Uint32 dimension>
Vector<T, dimension> operator + (Vector<T, dimension> const &l, Vector<T, dimension> const &r)
{
    Vector<T, dimension> retval;
    for (Uint32 i = 0; i < dimension; ++i)
        retval.m[i] = l.m[i] + r.m[i];
    return retval;
}

/** Performs vector subtraction (a non-commutative operation) on the given vectors.
  * @brief Global subtraction operator.
  * @param l The vector on the left side of the operation.
  * @param r The vector on the right side of the operation.
  * @return The vector difference of the given vectors.
  */
template <typename T, Uint32 dimension>
Vector<T, dimension> operator - (Vector<T, dimension> const &l, Vector<T, dimension> const &r)
{
    Vector<T, dimension> retval;
    for (Uint32 i = 0; i < dimension; ++i)
        retval.m[i] = l.m[i] - r.m[i];
    return retval;
}

/** Performs scalar component-wise multiplication (a commutative operation)
  * of the vector operands.
  * @brief Global multiplication operator.
  * @param l The vector on the left side of the operation.
  * @param r The vector on the right side of the operation.
  * @return The vector which contains the component-wise products of the
  *         operands' components.
  */
template <typename T, Uint32 dimension>
Vector<T, dimension> operator * (Vector<T, dimension> const &l, Vector<T, dimension> const &r)
{
    Vector<T, dimension> retval;
    for (Uint32 i = 0; i < dimension; ++i)
        retval.m[i] = l.m[i] * r.m[i];
    return retval;
}

/** Performs scalar multiplication (a commutative operation) of the scalar
  * l on the vector r.
  * @brief Global multiplication operator.
  * @param l The scalar on the left side of the operation.
  * @param r The vector on the right side of the operation.
  * @return The scalar multiple of the given scalar and vector.
  */
template <typename T, Uint32 dimension, typename U>
Vector<T, dimension> operator * (U l, Vector<T, dimension> const &r)
{
    Vector<T, dimension> retval;
    for (Uint32 i = 0; i < dimension; ++i)
        retval.m[i] = static_cast<T>(l) * r.m[i];
    return retval;
}

/** Performs scalar multiplication (a commutative operation) of the scalar
  * r on the vector l.
  * @brief Global multiplication operator.
  * @param l The vector on the left side of the operation.
  * @param r The scalar on the right side of the operation.
  * @return The scalar multiple of the given scalar and vector.
  */
template <typename T, Uint32 dimension, typename U>
Vector<T, dimension> operator * (Vector<T, dimension> const &l, U r)
{
    Vector<T, dimension> retval;
    for (Uint32 i = 0; i < dimension; ++i)
        retval.m[i] = l.m[i] * static_cast<T>(r);
    return retval;
}

/** Performs scalar component-wise division (a non-commutative operation)
  * of the vector operands.
  * @brief Global division operator.
  * @param l The vector on the left side of the operation.
  * @param r The vector on the right side of the operation.
  * @return The vector which contains the component-wise quotients of the
  *         operands' components.
  * @note There is no protection here against dividing by zero.
  */
template <typename T, Uint32 dimension>
Vector<T, dimension> operator / (Vector<T, dimension> const &l, Vector<T, dimension> const &r)
{
    Vector<T, dimension> retval;
    for (Uint32 i = 0; i < dimension; ++i)
        retval.m[i] = l.m[i] / r.m[i];
    return retval;
}

/** Performs scalar component-wise division (a non-commutative operation)
  * of the scalar operand by the vector operand components.
  * @brief Global division operator.
  * @param l The scalar on the left side of the operation.
  * @param r The vector on the right side of the operation.
  * @return The vector which contains the component-wise quotients of the
  *         left operand over each of the right operand's components.
  * @note There is no protection here against dividing by zero.
  */
template <typename T, Uint32 dimension, typename U>
Vector<T, dimension> operator / (U l, Vector<T, dimension> const &r)
{
    Vector<T, dimension> retval;
    for (Uint32 i = 0; i < dimension; ++i)
        retval.m[i] = static_cast<T>(l) / r.m[i];
    return retval;
}

/** Performs scalar division (a non-commutative operation) of the vector
  * l by the scalar r.
  * @brief Global division operator.
  * @param l The vector on the left side of the operation.
  * @param r The scalar on the right side of the operation.
  * @return The scalar quotient of the given scalar and vector.
  * @note There is no protection here against dividing by zero.
  */
template <typename T, Uint32 dimension, typename U>
Vector<T, dimension> operator / (Vector<T, dimension> const &l, U r)
{
    Vector<T, dimension> retval;
    for (Uint32 i = 0; i < dimension; ++i)
        retval.m[i] = l.m[i] / static_cast<T>(r);
    return retval;
}

/** Performs the Euclidean-inner-product (a commutative operation) on
  * the two given vectors.
  * @brief The dot product.
  * @param l The vector on the left side of the operation.
  * @param r The vector on the right side of the operation.
  * @return The sum of the multiplication of each component of the vector
  *         l with the corresponding component of the vector
  *         r.
  */
template <typename T, Uint32 dimension>
T operator | (Vector<T, dimension> const &l, Vector<T, dimension> const &r)
{
    T retval = l.m[0] * r.m[0];
    for (Uint32 i = 1; i < dimension; ++i)
        retval += l.m[i] * r.m[i];
    return retval;
}

/** Performs the scalar cross product (a non-commutative operation) on
  * the two given vectors.  The resulting value can be thought of as the
  * Z component of the cross product of two vectors contained completely
  * in the X/Y plane.
  * @brief The scalar cross product.
  * @param l The vector on the left side of the operation.
  * @param r The vector on the right side of the operation.
  * @return The product of the lengths of the given vectors, multiplied
  *         by the sine of the angle between them.
  */
template <typename T>
T operator & (Vector<T, 2> const &l, Vector<T, 2> const &r)
{
    return l.m[0] * r.m[1] -
           l.m[1] * r.m[0];
}

/** Performs the 3D vector cross product (a non-commutative operation) on
  * the two given vectors.
  * @brief The 3D vector cross product.
  * @param l The vector on the left side of the operation.
  * @param r The vector on the right side of the operation.
  * @return A 3D vector which is perpendicular to both of the given vectors.
  *         The length is given by the product of the given vectors,
  *         multiplied by the sine of the angle between them.
  */
template <typename T>
Vector<T, 3> operator & (Vector<T, 3> const &l, Vector<T, 3> const &r)
{
    return Vector<T, 3>(
        l.m[1] * r.m[2] -
        l.m[2] * r.m[1],
        l.m[2] * r.m[0] -
        l.m[0] * r.m[2],
        l.m[0] * r.m[1] -
        l.m[1] * r.m[0]);
}

/** Requires that T have a unary negation operator defined.
  * This function can be thought of as the 3D cross product of the Z unit
  * vector and @c source (keeping in mind that the coordinate system is
  * right-handed).
  * @brief Returns a 2D vector which is rotated 90 degrees counterclockwise.
  * @param source The vector to base the rotation from.
  * @return A 2D vector which is perpendicular to source such that
  *         @code source & PerpendicularVector2<T>(source) @endcode is
  *         positive.
  */
template <typename T>
Vector<T, 2> PerpendicularVector2 (Vector<T, 2> const &source)
{
    return Vector<T, 2>(-source[1], source[0]);
}

template <typename T, Uint32 dimension>
std::ostream &operator << (std::ostream &stream, Vector<T, dimension> const &v)
{
    stream << "Vector" << dimension << ": (";
    for (Uint32 i = 0; i < dimension; ++i)
    {
        stream << v[i];
        if (i < dimension-1)
            stream << ", ";
    }
    return stream << ')';
}

// ///////////////////////////////////////////////////////////////////////////
// partial template specialization to allow Serializer::ReadAggregate and
// Serializer::WriteAggregate on Vector<T,dimension>
// ///////////////////////////////////////////////////////////////////////////

template <typename T, Uint32 dimension>
struct Aggregate<Vector<T,dimension> >
{
    static void Read (Serializer &serializer, Vector<T,dimension> &dest)
    {
        serializer.ReadArray<T>(dest.m, LENGTHOF(dest.m));
    }
    static void Write (Serializer &serializer, Vector<T,dimension> const &source)
    {
        serializer.WriteArray<T>(source.m, LENGTHOF(source.m));
    }
};

/// Convenience typedef for a 2-dimensional Float vector.
typedef Vector<Float, 2> FloatVector2;

} // end of namespace Xrb
    
#endif // !defined(_XRB_VECTOR_HPP_)
