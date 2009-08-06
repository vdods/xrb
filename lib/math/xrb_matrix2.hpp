// ///////////////////////////////////////////////////////////////////////////
// xrb_matrix2.hpp by Victor Dods, created 2005/05/27
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_MATRIX2_HPP_)
#define _XRB_MATRIX2_HPP_

#include "xrb.hpp"

#include "xrb_simpletransform2.hpp"
#include "xrb_util.hpp"
#include "xrb_vector.hpp"
#include "xrb_math.hpp"

namespace Xrb
{

//
// a matrix is
//
// [ A B X ]
// [ C D Y ]
// [ 0 0 1 ]
//

// note: T must be a plainly assignable type (no virtual tables or anything)
template <typename T>
class Matrix2
{
public:

    static Matrix2<T> const ms_identity;

    enum Component
    {
        A = 0,
        B,
        X,
        C,
        D,
        Y,

        COMPONENT_COUNT
    };

    T m[COMPONENT_COUNT];

    // ///////////////////////////////////////////////////////////////////////
    // constructors and destructor
    // ///////////////////////////////////////////////////////////////////////

    inline Matrix2 () { }
    inline Matrix2 (
        T a, T b, T x,
        T c, T d, T y)
    {
        m[A] = a;
        m[B] = b;
        m[X] = x;
        m[C] = c;
        m[D] = d;
        m[Y] = y;
    }
    inline Matrix2 (T const *const components)
    {
        ASSERT1(components != NULL);
    #if defined(XRB_MATRIX2_USES_MEMCPY)
        memcpy(m, components, sizeof(T) * COMPONENT_COUNT);
    #else
        for (Uint32 i = 0; i < COMPONENT_COUNT; ++i)
            m[i] = components[i];
    #endif
    }
    inline Matrix2 (Matrix2<T> const &source)
    {
    #if defined(XRB_MATRIX2_USES_MEMCPY)
        memcpy(m, source.m, sizeof(T) * COMPONENT_COUNT);
    #else
        for (Uint32 i = 0; i < COMPONENT_COUNT; ++i)
            m[i] = source.m[i];
    #endif
    }
    inline Matrix2 (SimpleTransform2<T> const &source)
    {
        m[A] = source[SimpleTransform2<T>::R];
        m[B] = static_cast<T>(0);
        m[X] = source[SimpleTransform2<T>::X];

        m[C] = static_cast<T>(0);
        m[D] = source[SimpleTransform2<T>::S];
        m[Y] = source[SimpleTransform2<T>::Y];
    }
    inline ~Matrix2 () { }

    // ///////////////////////////////////////////////////////////////////////
    // overloaded operators
    // ///////////////////////////////////////////////////////////////////////

    inline T const &operator [] (Component const component) const
    {
        ASSERT3(A == 0);
        ASSERT1(component < COMPONENT_COUNT);
        return m[component];
    }
    inline T &operator [] (Component const component)
    {
        ASSERT3(A == 0);
        ASSERT1(component < COMPONENT_COUNT);
        return m[component];
    }
    // multiplying operand on the left side of this matrix
    inline void operator *= (Matrix2<T> const &operand)
    {
        T b, x, c;
        b = operand.m[A]*m[B] + operand.m[B]*m[D];
        x = operand.m[A]*m[X] + operand.m[B]*m[Y] + operand.m[X];
        c = operand.m[C]*m[A] + operand.m[D]*m[C];
        m[A] = operand.m[A]*m[A] + operand.m[B]*m[C];
        m[D] = operand.m[C]*m[B] + operand.m[D]*m[D];
        m[Y] = operand.m[C]*m[X] + operand.m[D]*m[Y] + operand.m[Y];
        m[B] = b;
        m[X] = x;
        m[C] = c;
    }
    // multiplying operand on the left side of this matrix
    inline void operator *= (SimpleTransform2<T> const &operand)
    {
        m[A] *= operand[SimpleTransform2<T>::R];
        m[B] *= operand[SimpleTransform2<T>::R];
        m[X] = m[X]*operand[SimpleTransform2<T>::R] + operand[SimpleTransform2<T>::X];
        m[C] *= operand[SimpleTransform2<T>::S];
        m[D] *= operand[SimpleTransform2<T>::S];
        m[Y] = m[Y]*operand[SimpleTransform2<T>::S] + operand[SimpleTransform2<T>::Y];
    }
    inline void operator = (Matrix2<T> const &operand)
    {
    #if defined(XRB_MATRIX2_USES_MEMCPY)
        memcpy(m, operand.m, sizeof(T) * COMPONENT_COUNT);
    #else
        for (Uint32 i = 0; i < COMPONENT_COUNT; ++i)
            m[i] = operand.m[i];
    #endif
    }
    inline void operator = (SimpleTransform2<T> const &operand)
    {
        m[A] = operand[SimpleTransform2<T>::R];
        m[B] = static_cast<T>(0);
        m[X] = operand[SimpleTransform2<T>::X];

        m[C] = static_cast<T>(0);
        m[D] = operand[SimpleTransform2<T>::S];
        m[Y] = operand[SimpleTransform2<T>::Y];
    }

    // ///////////////////////////////////////////////////////////////////////
    // accessors
    // ///////////////////////////////////////////////////////////////////////

    inline T Determinant () const
    {
        return m[A] * m[D] - m[B] * m[C];
    }
    inline Matrix2<T> Inverse () const
    {
        Matrix2<T> retval;
        T determinant = m[A] * m[D] - m[B] * m[C];
        T negative_determinant = -determinant;
        if (determinant != negative_determinant) // equivalent to determinant != 0
        {
            retval.m[A] = m[D] / determinant;
            retval.m[B] = m[B] / negative_determinant;
            retval.m[X] = (m[B]*m[Y] - m[X]*m[D]) / determinant;
            retval.m[C] = m[C] / negative_determinant;
            retval.m[D] = m[A] / determinant;
            retval.m[Y] = (m[X]*m[C] - m[A]*m[Y]) / determinant;
        }
        else
        {
        #if defined(XRB_MATRIX2_USES_MEMCPY)
            memcpy(retval.m, ms_identity.m, sizeof(T) * COMPONENT_COUNT);
        #else
            for (Uint32 i = 0; i < COMPONENT_COUNT; ++i)
                retval.m[i] = ms_identity.m[i];
        #endif
        }
        return retval;
    }

    // ///////////////////////////////////////////////////////////////////////
    // modifiers
    // ///////////////////////////////////////////////////////////////////////

    inline void Invert ()
    {
        T determinant = m[A]*m[D] - m[B]*m[C];
        T negative_determinant = -determinant;
        if (determinant != negative_determinant) // equivalent to determinant != 0
        {
            T a, x, c;
            a = m[D] / determinant;
            x = (m[B]*m[Y] - m[X]*m[D]) / determinant;
            c = m[C] / negative_determinant;
            m[B] /= negative_determinant;
            m[D] = m[A] / determinant;
            m[Y] = (m[X]*m[C] - m[A]*m[Y]) / determinant;
            m[A] = a;
            m[X] = x;
            m[C] = c;
        }
        else
        {
        #if defined(XRB_MATRIX2_USES_MEMCPY)
            memcpy(m, ms_identity.m, sizeof(T) * COMPONENT_COUNT);
        #else
            for (Uint32 i = 0; i < COMPONENT_COUNT; ++i)
                m[i] = ms_identity.m[i];
        #endif
        }
    }

    inline void Rotate (T const angle)
    {
        Rotate(Math::Cos(angle), Math::Sin(angle));
    }
    inline void Rotate (T const cos_value, T const sin_value)
    {
        T a, b, x;
        a = m[A]*cos_value - m[C]*sin_value;
        b = m[B]*cos_value - m[D]*sin_value;
        x = m[X]*cos_value - m[Y]*sin_value;
        m[C] = m[A]*sin_value + m[C]*cos_value;
        m[D] = m[B]*sin_value + m[D]*cos_value;
        m[Y] = m[X]*sin_value + m[Y]*cos_value;
        m[A] = a;
        m[B] = b;
        m[X] = x;
    }
    inline void Scale (Vector<T, 2> const &scale_factors)
    {
        m[A] *= scale_factors[Dim::X];
        m[B] *= scale_factors[Dim::X];
        m[X] *= scale_factors[Dim::X];
        m[C] *= scale_factors[Dim::Y];
        m[D] *= scale_factors[Dim::Y];
        m[Y] *= scale_factors[Dim::Y];
    }
    inline void Scale (T const r, T const s)
    {
        m[A] *= r;
        m[B] *= r;
        m[X] *= r;
        m[C] *= s;
        m[D] *= s;
        m[Y] *= s;
    }
    inline void Scale (T const scale_factor)
    {
        m[A] *= scale_factor;
        m[B] *= scale_factor;
        m[X] *= scale_factor;
        m[C] *= scale_factor;
        m[D] *= scale_factor;
        m[Y] *= scale_factor;
    }
    inline void Translate (Vector<T, 2> const &translation)
    {
        m[X] += translation[Dim::X];
        m[Y] += translation[Dim::Y];
    }
    inline void Translate (T const x, T const y)
    {
        m[X] += x;
        m[Y] += y;
    }

    inline void SetComponents (
        T const a, T const b, T const x,
        T const c, T const d, T const y)
    {
        m[A] = a;
        m[B] = b;
        m[X] = x;
        m[C] = c;
        m[D] = d;
        m[Y] = y;
    }

    // ///////////////////////////////////////////////////////////////////////
    // procedures
    // ///////////////////////////////////////////////////////////////////////

    void Fprint (
        FILE *const fptr,
        char const *const component_printf_format) const
    {
        ASSERT1(fptr != NULL);
        ASSERT1(component_printf_format != NULL);

        fprintf(fptr, "Matrix2:\n\t[");
        fprintf(fptr, component_printf_format, m[A]);
        fprintf(fptr, ", ");
        fprintf(fptr, component_printf_format, m[B]);
        fprintf(fptr, ", ");
        fprintf(fptr, component_printf_format, m[X]);
        fprintf(fptr, "]\n\t[");
        fprintf(fptr, component_printf_format, m[C]);
        fprintf(fptr, ", ");
        fprintf(fptr, component_printf_format, m[D]);
        fprintf(fptr, ", ");
        fprintf(fptr, component_printf_format, m[Y]);
        fprintf(fptr, "]\n");
    }
}; // end of class Matrix2

/** This templatized definition will take care of defining the static
  * @c ms_identity member matrix, assuming that @c static_cast<T>(0) and
  * @c static_cast<T>(1) work.
  * @brief Templatized static definition of the @c ms_identity matrix.
  */
template <typename T>
Matrix2<T> const Matrix2<T>::ms_identity(
    static_cast<T>(1), static_cast<T>(0), static_cast<T>(0),
    static_cast<T>(0), static_cast<T>(1), static_cast<T>(0));

// ///////////////////////////////////////////////////////////////////////////
// Operator overloads
// ///////////////////////////////////////////////////////////////////////////

// matrix * matrix
template <typename T>
inline Matrix2<T> operator * (
    Matrix2<T> const &left_operand,
    Matrix2<T> const &right_operand)
{
    Matrix2<T> retval(right_operand);
    retval *= left_operand;
    return retval;
}

// simpletransform * matrix
template <typename T>
inline Matrix2<T> operator * (
    SimpleTransform2<T> const &left_operand,
    Matrix2<T> const &right_operand)
{
    Matrix2<T> retval(right_operand);
    retval *= left_operand;
    return retval;
}

// matrix * simpletransform
template <typename T>
inline Matrix2<T> operator * (
    Matrix2<T> const &left_operand,
    SimpleTransform2<T> const &right_operand)
{
    Matrix2<T> retval;
    retval[Matrix2<T>::A] =
        left_operand[Matrix2<T>::A] * right_operand[SimpleTransform2<T>::R];
    retval[Matrix2<T>::B] =
        left_operand[Matrix2<T>::B] * right_operand[SimpleTransform2<T>::S];
    retval[Matrix2<T>::X] =
        left_operand[Matrix2<T>::A] * right_operand[SimpleTransform2<T>::X] +
        left_operand[Matrix2<T>::B] * right_operand[SimpleTransform2<T>::Y] +
        left_operand[Matrix2<T>::X];
    retval[Matrix2<T>::C] =
        left_operand[Matrix2<T>::C] * right_operand[SimpleTransform2<T>::R];
    retval[Matrix2<T>::D] =
        left_operand[Matrix2<T>::D] * right_operand[SimpleTransform2<T>::S];
    retval[Matrix2<T>::Y] =
        left_operand[Matrix2<T>::C] * right_operand[SimpleTransform2<T>::X] +
        left_operand[Matrix2<T>::D] * right_operand[SimpleTransform2<T>::Y] +
        left_operand[Matrix2<T>::Y];
    return retval;
}

// vector *= matrix
template <typename T>
inline void operator *= (
    Vector<T, 2> &assignee,
    Matrix2<T> const &operand)
{
    T temp =
        operand[Matrix2<T>::A] * assignee[Dim::X] +
        operand[Matrix2<T>::B] * assignee[Dim::Y] +
        operand[Matrix2<T>::X];
    assignee[Dim::Y] =
        operand[Matrix2<T>::C] * assignee[Dim::X] +
        operand[Matrix2<T>::D] * assignee[Dim::Y] +
        operand[Matrix2<T>::Y];
    assignee[Dim::X] = temp;
}

// matrix * vector
template <typename T>
inline Vector<T, 2> operator * (
    Matrix2<T> const &left_operand,
    Vector<T, 2> const &right_operand)
{
    Vector<T, 2> retval(right_operand);
    retval *= left_operand;
    return retval;
}

// ///////////////////////////////////////////////////////////////////////////
// convenience typedefs for Matrix2 of different types,
// pre-made representations of the identity for each typedef,
// and format-specific Fprint functions for each typedef.
// ///////////////////////////////////////////////////////////////////////////

/** FloatMatrix2
  * @brief Convenience typedef for a Matrix2<Float>.
  */
typedef Matrix2<Float> FloatMatrix2;

/** This is a convenience function to provide a default
  * printf format to FloatMatrix2::Fprint.
  * @brief Prints the given FloatMatrix2 to the given file stream.
  * @param fptr The file stream to print to.
  * @param matrix The FloatMatrix2 to print.
  */
void Fprint (FILE *fptr, FloatMatrix2 const &matrix);

} // end of namespace Xrb
    
#endif // !defined(_XRB_MATRIX2_HPP_)

