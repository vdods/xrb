// ///////////////////////////////////////////////////////////////////////////
// xrb_ntuple.h by Victor Dods, created 2005/06/12
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_NTUPLE_H_)
#define _XRB_NTUPLE_H_

#include "xrb.h"

#include "xrb_enums.h"

/** @file xrb_ntuple.h
  * Also contains convenience typedefs for specific types of n-tuples.
  * @brief Defines a very efficient yet generalized n-tuple class
  *        (which are more or less vectors, but without the specialized
  *        operators).
  */

namespace Xrb
{

// ///////////////////////////////////////////////////////////////////////////
// NTuple - generalized type ordered value set of arbitrary size
// ///////////////////////////////////////////////////////////////////////////

/** The type must be a regular assignable type (not a type with possibly a
  * vtable, and definitely not a type which requires deep copying to perform
  * assignment).  This is because if XRB_NTUPLE_USES_MEMCPY is defined,
  * memcpy() is used to increase efficiency on certain operations.  Also,
  * if XRB_NTUPLE_USES_MEMCMP is defined, memcmp() is used on certain
  * operations.  The default behavior is to iterate over each component
  * for operations.
  * @brief Generalized n-tuple class geared towards high speed efficiency.
  */
template <typename T, Uint32 size>
class NTuple
{
public:

    /// The array which stores the n-tuple's components
    T m[size];

    // ///////////////////////////////////////////////////////////////////////
    // constructors and destructor
    // ///////////////////////////////////////////////////////////////////////

    /** Does no initialization for the sake of efficiency.
      * @brief Default constructor.
      */
    inline NTuple ()
    {
    }
    /** I gave up trying for partial template specialization (or whatever it's
      * called) to specify the two-parameter constructor for only
      * NTuple<T, 2>, and settled for asserting that @c size is 2.
      * @brief Constructs an n-tuple with the specified ordered pair.
      * @param component0 The value to assign to the first component (m[0]).
      * @param component1 The value to assign to the second component (m[1]).
      */
    inline NTuple (
        T const component0,
        T const component1)
    {
        ASSERT1(size == 2)
        m[0] = component0;
        m[1] = component1;
    }
    /** I gave up trying for partial template specialization (or whatever it's
      * called) to specify the two-parameter constructor for only
      * NTuple<T, 3>, and settled for asserting that @c size is 3.
      * @brief Constructs an n-tuple with the specified ordered triplet.
      * @param component0 The value to assign to the first component (m[0]).
      * @param component1 The value to assign to the second component (m[1]).
      * @param component2 The value to assign to the third component (m[2]).
      */
    inline NTuple (
        T const component0,
        T const component1,
        T const component2)
    {
        ASSERT1(size == 3)
        m[0] = component0;
        m[1] = component1;
        m[2] = component2;
    }
    /** I gave up trying for partial template specialization (or whatever it's
      * called) to specify the two-parameter constructor for only
      * NTuple<T, 4>, and settled for asserting that @c size is 4.
      * @brief Constructs an n-tuple with the specified ordered quadruplet.
      * @param component0 The value to assign to the first component (m[0]).
      * @param component1 The value to assign to the second component (m[1]).
      * @param component2 The value to assign to the third component (m[2]).
      * @param component3 The value to assign to the fourth component (m[3]).
      */
    inline NTuple (
        T const component0,
        T const component1,
        T const component2,
        T const component3)
    {
        ASSERT1(size == 4)
        m[0] = component0;
        m[1] = component1;
        m[2] = component2;
        m[3] = component3;
    }
    /** @brief Constructs an n-tuple from an array of component values.
      * @param components The array of ordered components to construct this
      *                   n-tuple with.  Must be at least @c size
      *                   elements long.
      */
    inline NTuple (T const *const components)
    {
        ASSERT1(components != NULL)
    #if defined(XRB_NTUPLE_USES_MEMCPY)
        memcpy(m, components, sizeof(T) * size);
    #else
        for (Uint32 i = 0; i < size; ++i)
            m[i] = components[i];
    #endif
    }
    /** @brief Copy constructor.
      * @param source The n-tuple to copy.
      */
    inline NTuple (NTuple<T, size> const &source)
    {
    #if defined(XRB_NTUPLE_USES_MEMCPY)
        memcpy(m, source.m, sizeof(T) * size);
    #else
        for (Uint32 i = 0; i < size; ++i)
            m[i] = source.m[i];
    #endif
    }
    /** No cleanup is necessary.
      * @brief Destructor.
      */
    inline ~NTuple ()
    {
    }

    // ///////////////////////////////////////////////////////////////////////
    // overloaded operators
    // ///////////////////////////////////////////////////////////////////////

    /** @brief Assignment operator.
      * @param operand The n-tuple to assign.
      */
    inline void operator = (NTuple<T, size> const &operand)
    {
    #if defined(XRB_NTUPLE_USES_MEMCPY)
        memcpy(m, operand.m, sizeof(T) * size);
    #else
        for (Uint32 i = 0; i < size; ++i)
            m[i] = operand.m[i];
    #endif
    }
    /** @brief Equality operator.
      * @param operand The n-tuple to test equality against.
      * @return True iff each component of operand is equal to the
      *         corresponding component of this n-tuple.
      */
    inline bool operator == (NTuple<T, size> const &operand) const
    {
    #if defined(XRB_NTUPLE_USES_MEMCMP)
        return memcmp(m, operand.m, sizeof(T) * size) == 0;
    #else
        for (Uint32 i = 0; i < size; ++i)
            if (m[i] != operand.m[i])
                return false;
        return true;
    #endif
    }
    /** @brief Inequality operator.
      * @param operand The n-tuple to test inequality against.
      * @return True iff any component of operand is unequal to the
      *         corresponding component of this n-tuple.
      */
    inline bool operator != (NTuple<T, size> const &operand) const
    {
    #if defined(XRB_NTUPLE_USES_MEMCMP)
        return memcmp(m, operand.m, sizeof(T) * size) != 0;
    #else
        for (Uint32 i = 0; i < size; ++i)
            if (m[i] != operand.m[i])
                return true;
        return false;
    #endif
    }
    /** Array index referencing.  Emulates the syntax of a normal array,
      * giving a more appealing usage (n-tuple[1] instead of n-tuple.m[1]).
      * @brief Bracket operator with const qualification.
      * @param index The index of the component to reference.
      * @return A const reference to the desired n-tuple component.
      */
    inline T const &operator [] (Uint32 const index) const
    {
        ASSERT3(index < size);
        return m[index];
    }
    /** Array index referencing.  Emulates the syntax of a normal array,
      * giving a more appealing usage (n-tuple[1] instead of n-tuple.m[1]).
      * @brief Bracket operator.
      * @param index The index of the component to reference.
      * @return A non-const reference to the desired n-tuple component.
      */
    inline T &operator [] (Uint32 const index)
    {
        ASSERT3(index < size);
        return m[index];
    }
    /** Performs vector addition on this and the given n-tuple, storing
      * the result in this n-tuple.
      * @brief In-place addition operator.
      * @param operand The n-tuple to add to this n-tuple.
      */
    inline void operator += (NTuple<T, size> const &operand)
    {
        for (Uint32 i = 0; i < size; ++i)
            m[i] += operand.m[i];
    }
    /** Performs vector subtraction on this and the given n-tuple, storing
      * the result in this n-tuple.
      * @brief In-place addition operator.
      * @param operand The n-tuple to subtract from this n-tuple.
      */
    inline void operator -= (NTuple<T, size> const &operand)
    {
        for (Uint32 i = 0; i < size; ++i)
            m[i] -= operand.m[i];
    }
    /** Performs scalar multiplication of the components of @c operand
      * on each respective component of this n-tuple, storing the
      * result in this n-tuple.
      * @brief In-place multiplication operator.
      * @param operand The n-tuple to component-wise multiply this
      *                n-tuple by.
      */
    inline void operator *= (NTuple<T, size> const &operand)
    {
        for (Uint32 i = 0; i < size; ++i)
            m[i] *= operand.m[i];
    }
    /** Performs scalar multiplication on this n-tuple by the given scalar
      * storing the result in this n-tuple.
      * @brief In-place multiplication operator.
      * @param operand The scalar value to multiply this n-tuple by.
      */
    inline void operator *= (T const operand)
    {
        for (Uint32 i = 0; i < size; ++i)
            m[i] *= operand;
    }
    /** Performs scalar division of the components of this n-tuple
      * by each respective component of @c operand, storing the
      * result in this n-tuple.
      * @brief In-place division operator.
      * @param operand The n-tuple to component-wise divide this
      *                n-tuple by.
      * @note There is no protection here against dividing by zero.
      */
    inline void operator /= (NTuple<T, size> const &operand)
    {
        for (Uint32 i = 0; i < size; ++i)
            m[i] /= operand.m[i];
    }
    /** Performs scalar division on this n-tuple by the given scalar
      * storing the result in this n-tuple.
      * @brief In-place division operator.
      * @param operand The scalar value to divide this n-tuple by.
      * @note There is no protection here against dividing by zero.
      */
    inline void operator /= (T const operand)
    {
        for (Uint32 i = 0; i < size; ++i)
            m[i] /= operand;
    }
    /** @brief Unary negation operator.
      * @return The arithmatic negative of this n-tuple.
      */
    inline NTuple<T, size> operator - () const
    {
        NTuple<T, size> retval;
        for (Uint32 i = 0; i < size; ++i)
            retval.m[i] = -m[i];
        return retval;
    }

    // ///////////////////////////////////////////////////////////////////////
    // accessors
    // ///////////////////////////////////////////////////////////////////////

    /** Performs static_cast on each of this NTuple's components
      * to convert the values to the desired.
      * @brief The NTuple equivalent to static_cast.
      */
    template <typename U>
    inline NTuple<U, size> StaticCast () const
    {
        NTuple<U, size> retval;
        for (Uint32 i = 0; i < size; ++i)
            retval.m[i] = static_cast<U>(m[i]);
        return retval;
    }

    // ///////////////////////////////////////////////////////////////////////
    // modifiers
    // ///////////////////////////////////////////////////////////////////////

    /** @brief Sets each component in this n-tuple to the specified value.
      * @param fill_with The value to assign to each n-tuple component.
      */
    inline void FillWith (T const fill_with)
    {
        for (Uint32 i = 0; i < size; ++i)
            m[i] = fill_with;
    }
    /** The in-place negation of this n-tuple, or to put another way,
      * @code ms_zero - *this @endcode (or at least it better equal that!).
      * @brief Sets this n-tuple to the negative of itself.
      */
    inline void Negate ()
    {
        for (Uint32 i = 0; i < size; ++i)
            m[i] = -m[i];
    }
    /** I gave up trying for partial template specialization (or whatever it's
      * called) to specify the two-parameter constructor for only
      * NTuple<T, 2>, and settled for asserting that the size is 2.
      * @brief Assigns the specified ordered pair to this n-tuple.
      * @param component0 The value to assign to the first component (m[0]).
      * @param component1 The value to assign to the second component (m[1]).
      */
    inline void SetComponents (
        T const component0,
        T const component1)
    {
        ASSERT1(size == 2);
        m[0] = component0;
        m[1] = component1;
    }
    /** I gave up trying for partial template specialization (or whatever it's
      * called) to specify the three-parameter constructor for only
      * NTuple<T, 3>, and settled for asserting that the size is 3.
      * @brief Assigns the specified ordered triplet to this n-tuple.
      * @param component0 The value to assign to the first component (m[0]).
      * @param component1 The value to assign to the second component (m[1]).
      * @param component2 The value to assign to the third component (m[2]).
      */
    inline void SetComponents (
        T const component0,
        T const component1,
        T const component2)
    {
        ASSERT1(size == 3);
        m[0] = component0;
        m[1] = component1;
        m[2] = component2;
    }
    /** I gave up trying for partial template specialization (or whatever it's
      * called) to specify the three-parameter constructor for only
      * NTuple<T, 4>, and settled for asserting that the size is 4.
      * @brief Assigns the specified ordered triplet to this n-tuple.
      * @param component0 The value to assign to the first component (m[0]).
      * @param component1 The value to assign to the second component (m[1]).
      * @param component2 The value to assign to the third component (m[2]).
      * @param component3 The value to assign to the fourth component (m[3]).
      */
    inline void SetComponents (
        T const component0,
        T const component1,
        T const component2,
        T const component3)
    {
        ASSERT1(size == 4);
        m[0] = component0;
        m[1] = component1;
        m[2] = component2;
        m[3] = component3;
    }
    /** @brief Assigns the specified ordered array of components to the components
      *        of this n-tuple.
      * @param components The array of ordered components to assign
      *                   to this n-tuple.  Must be at least @c size
      *                   elements long.
      */
    inline void SetComponents (T const *const components)
    {
        ASSERT1(components != NULL);
    #if defined(XRB_NTUPLE_USES_MEMCPY)
        memcpy(m, components, sizeof(T) * size);
    #else
        for (Uint32 i = 0; i < size; ++i)
            m[i] = components[i];
    #endif
    }
    /** Performs static_cast on each of the operand n-tuple's components
      * to convert the values.
      * @brief The NTuple equivalent to n-tuple1 = static_cast<T>(n-tuple2).
      * @param source The n-tuple to assign.
      */
    template <typename U>
    inline void StaticCastAssign (NTuple<U, size> const &source)
    {
        for (Uint32 i = 0; i < size; ++i)
            m[i] = static_cast<T>(source.m[i]);
    }

    // ///////////////////////////////////////////////////////////////////////
    // procedures
    // ///////////////////////////////////////////////////////////////////////

    /** The format of each n-tuple component is specified by
      * component_printf_format, which should be the printf format string
      * for a single n-tuple component.  This value must be passed in because
      * the printf format is not known in the general case.
      * @brief Prints the size and components of this n-tuple to the given
      *        file stream.
      * @param fptr The file stream to print to (e.g. stderr).
      * @param component_printf_format The printf format string for a single
      *                                n-tuple component.
      * @param add_newline Indicates if a newline should be printed as the
      *                    last character (default value is true).
      */
    void Fprint (FILE *const fptr,
                 char const *const component_printf_format,
                 bool const add_newline = true) const
    {
        fprintf(fptr, "%uTuple = (", size);
        Uint32 i;
        for (i = 0; i < size-1; ++i)
        {
            fprintf(fptr, component_printf_format, m[i]);
            fprintf(fptr, ", ");
        }
        ASSERT1(i == size-1);
        fprintf(fptr, component_printf_format, m[i]);
        fprintf(fptr, ")%c", add_newline ? '\n' : '\0');
    }

}; // end of class NTuple

/** Performs n-tuple addition (a commutative operation) on the given n-tuples.
  * @brief Global addition operator.
  * @param left_operand The n-tuple on the left side of the operation.
  * @param right_operand The n-tuple on the right side of the operation.
  * @return The n-tuple sum of the given n-tuples.
  */
template <typename T, Uint32 size>
inline NTuple<T, size> operator + (
    NTuple<T, size> const &left_operand,
    NTuple<T, size> const &right_operand)
{
    NTuple<T, size> retval;
    for (Uint32 i = 0; i < size; ++i)
        retval.m[i] = left_operand.m[i] + right_operand.m[i];
    return retval;
}

/** Performs n-tuple subtraction (a non-commutative operation) on the given n-tuples.
  * @brief Global subtraction operator.
  * @param left_operand The n-tuple on the left side of the operation.
  * @param right_operand The n-tuple on the right side of the operation.
  * @return The n-tuple difference of the given n-tuples.
  */
template <typename T, Uint32 size>
inline NTuple<T, size> operator - (
    NTuple<T, size> const &left_operand,
    NTuple<T, size> const &right_operand)
{
    NTuple<T, size> retval;
    for (Uint32 i = 0; i < size; ++i)
        retval.m[i] = left_operand.m[i] - right_operand.m[i];
    return retval;
}

/** Performs scalar component-wise multiplication (a commutative operation)
  * of the n-tuple operands.
  * @brief Global multiplication operator.
  * @param left_operand The n-tuple on the left side of the operation.
  * @param right_operand The n-tuple on the right side of the operation.
  * @return The n-tuple which contains the component-wise products of the
  *         operands' components.
  */
template <typename T, Uint32 size>
inline NTuple<T, size> operator * (
    NTuple<T, size> const &left_operand,
    NTuple<T, size> const &right_operand)
{
    NTuple<T, size> retval;
    for (Uint32 i = 0; i < size; ++i)
        retval.m[i] = left_operand.m[i] * right_operand.m[i];
    return retval;
}

/** Performs scalar multiplication (a commutative operation) of the scalar
  * left_operand on the n-tuple right_operand.
  * @brief Global multiplication operator.
  * @param left_operand The scalar on the left side of the operation.
  * @param right_operand The n-tuple on the right side of the operation.
  * @return The scalar multiple of the given scalar and n-tuple.
  */
template <typename T, Uint32 size>
inline NTuple<T, size> operator * (
    T const left_operand,
    NTuple<T, size> const &right_operand)
{
    NTuple<T, size> retval;
    for (Uint32 i = 0; i < size; ++i)
        retval.m[i] = left_operand * right_operand.m[i];
    return retval;
}

/** Performs scalar multiplication (a commutative operation) of the scalar
  * right_operand on the n-tuple left_operand.
  * @brief Global multiplication operator.
  * @param left_operand The n-tuple on the left side of the operation.
  * @param right_operand The scalar on the right side of the operation.
  * @return The scalar multiple of the given scalar and n-tuple.
  */
template <typename T, Uint32 size>
inline NTuple<T, size> operator * (
    NTuple<T, size> const &left_operand,
    T const right_operand)
{
    NTuple<T, size> retval;
    for (Uint32 i = 0; i < size; ++i)
        retval.m[i] = left_operand.m[i] * right_operand;
    return retval;
}

/** Performs scalar component-wise division (a non-commutative operation)
  * of the n-tuple operands.
  * @brief Global division operator.
  * @param left_operand The n-tuple on the left side of the operation.
  * @param right_operand The n-tuple on the right side of the operation.
  * @return The n-tuple which contains the component-wise quotients of the
  *         operands' components.
  * @note There is no protection here against dividing by zero.
  */
template <typename T, Uint32 size>
inline NTuple<T, size> operator / (
    NTuple<T, size> const &left_operand,
    NTuple<T, size> const &right_operand)
{
    NTuple<T, size> retval;
    for (Uint32 i = 0; i < size; ++i)
        retval.m[i] = left_operand.m[i] / right_operand.m[i];
    return retval;
}

/** Performs scalar component-wise division (a non-commutative operation)
  * of the scalar operand by the n-tuple operand components.
  * @brief Global division operator.
  * @param left_operand The scalar on the left side of the operation.
  * @param right_operand The n-tuple on the right side of the operation.
  * @return The n-tuple which contains the component-wise quotients of the
  *         left operand over each of the right operand's components.
  * @note There is no protection here against dividing by zero.
  */
template <typename T, Uint32 size>
inline NTuple<T, size> operator / (
    T const left_operand,
    NTuple<T, size> const &right_operand)
{
    NTuple<T, size> retval;
    for (Uint32 i = 0; i < size; ++i)
        retval.m[i] = left_operand / right_operand.m[i];
    return retval;
}

/** Performs scalar division (a non-commutative operation) of the n-tuple
  * left_operand by the scalar right_operand.
  * @brief Global division operator.
  * @param left_operand The n-tuple on the left side of the operation.
  * @param right_operand The scalar on the right side of the operation.
  * @return The scalar quotient of the given scalar and n-tuple.
  * @note There is no protection here against dividing by zero.
  */
template <typename T, Uint32 size>
inline NTuple<T, size> operator / (
    NTuple<T, size> const &left_operand,
    T const right_operand)
{
    NTuple<T, size> retval;
    for (Uint32 i = 0; i < size; ++i)
        retval.m[i] = left_operand.m[i] / right_operand;
    return retval;
}

// ///////////////////////////////////////////////////////////////////////////
// convenience typedefs for n-tuples of different types and dimensions,
// and format-specific Fprint functions for each typedef.
// ///////////////////////////////////////////////////////////////////////////

/** Bool2
  * @brief Convenience typedef for a 2-dimensional Bool vector.
  */
typedef NTuple<bool, 2> Bool2;
/** Alignment2
  * @brief Convenience typedef for a 2-dimensional Alignment vector.
  */
typedef NTuple<Alignment, 2> Alignment2;

/** This is a convenience function to provide a default printf format to
  * NTuple::Fprint.
  * @brief Prints the given Bool2 to the given file stream.
  * @param fptr The file stream to print to.
  * @param ntuple The Bool2 to print.
  * @param add_newline Indicates if a newline should be printed at the
  *                    end of the formatted output.
  */
void Fprint (
    FILE *fptr,
    Bool2 const &ntuple,
    bool add_newline = true);
/** This is a convenience function to provide a default printf format to
  * NTuple::Fprint.
  * @brief Prints the given Alignment2 to the given file stream.
  * @param fptr The file stream to print to.
  * @param ntuple The Alignment2 to print.
  * @param add_newline Indicates if a newline should be printed at the
  *                    end of the formatted output.
  */
void Fprint (
    FILE *fptr,
    Alignment2 const &ntuple,
    bool add_newline = true);

} // end of namespace Xrb

#endif // !defined(_XRB_NTUPLE_H_)

