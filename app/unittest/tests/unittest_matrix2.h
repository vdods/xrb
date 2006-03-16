// ///////////////////////////////////////////////////////////////////////////
// matrix2unittest.h by Victor Dods, created 2005/05/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_MATRIX2UNITTEST_H_)
#define _MATRIX2UNITTEST_H_

#include "../defines.h"

#include "unittest.h"

#include "../matrix2.h"
#include "../simpletransform2.h"
#include "../vector.h"

class Matrix2UnitTest : public UnitTest
{
public:

    Matrix2UnitTest ();
    virtual ~Matrix2UnitTest ();

    virtual void Initialize ();

protected:

    void AtomStaticIdentity ();
    void AtomConstructByComponents ();
    void AtomConstructByComponentArray ();
    void AtomConstructByMatrix2 ();
    void AtomConstructBySimpleTransform2 ();
    void AtomClassOperatorBracket ();
    void AtomClassOperatorInplaceMultiplyMatrix2 ();
    void AtomClassOperatorInplaceMultiplySimpleTransform2 ();
    void AtomClassOperatorAssignmentMatrix2 ();
    void AtomClassOperatorAssignmentSimpleTransform2 ();
    void AtomDeterminant ();
    void AtomInverse ();
    void AtomRotate ();
    void AtomScale ();
    void AtomTranslate ();
    void AtomGlobalOperators ();
    void AtomLinearity ();

private:

    template <typename T, Uint32 const dimension>
    void TestVectorEquality (
        Vector<T, dimension> const &vector0,
        char const *vector0_name,
        Vector<T, dimension> const &vector1,
        char const *vector1_name,
        char const *component_printf_format,
        T epsilon) const
    {
        Uint32 const printf_format_buffer_size = 0x1000;
        char printf_format_buffer[printf_format_buffer_size];
        snprintf(
            printf_format_buffer,
            printf_format_buffer_size,
            "component %%u of %%s (%s) == component %%u of %%s (%s)",
            component_printf_format,
            component_printf_format);
        for (Uint32 i = 0; i < dimension; ++i)
            Test(Epsilon(vector0.m[i], vector1.m[i], epsilon),
                 printf_format_buffer,
                 i,
                 vector0_name,
                 vector0.m[i],
                 i,
                 vector1_name,
                 vector1.m[i]);
    }

    template <typename T>
    void TestMatrix2Equality (
        Matrix2<T> const &matrix0,
        char const *matrix0_name,
        Matrix2<T> const &matrix1,
        char const *matrix1_name,
        char const *component_printf_format,
        T epsilon) const
    {
        Uint32 const printf_format_buffer_size = 0x1000;
        char printf_format_buffer[printf_format_buffer_size];
        snprintf(
            printf_format_buffer,
            printf_format_buffer_size,
            "component %%c of %%s (%s) == component %%c of %%s (%s)",
            component_printf_format,
            component_printf_format);
        struct ComponentNameAndValue
        {
            char m_name;
            typename Matrix2<T>::Component m_value;
        };
        ComponentNameAndValue names_and_values[Matrix2<T>::COMPONENT_COUNT] =
        {
            {'A', Matrix2<T>::A},
            {'B', Matrix2<T>::B},
            {'X', Matrix2<T>::X},
            {'C', Matrix2<T>::C},
            {'D', Matrix2<T>::D},
            {'Y', Matrix2<T>::Y}
        };
        for (Uint32 i = 0; i < Matrix2<T>::COMPONENT_COUNT; ++i)
            Test(Epsilon(matrix0.m[Matrix2<T>::A],
                         matrix1.m[Matrix2<T>::A],
                         epsilon),
                 printf_format_buffer,
                 names_and_values[i].m_name,
                 matrix0_name,
                 matrix0.m[names_and_values[i].m_value],
                 names_and_values[i].m_name,
                 matrix1_name,
                 matrix1.m[names_and_values[i].m_value]);
    }

    FloatMatrix2 const m_matrix0;
    Float const m_matrix0_determinant;
    FloatMatrix2 const m_matrix0_inverse;

    FloatMatrix2 const m_matrix1;
    Float const m_matrix1_determinant;
    FloatMatrix2 const m_matrix1_inverse;

    FloatMatrix2 const m_product_of_m0_and_m1;
    FloatMatrix2 const m_product_of_m1_and_m0;

    FloatSimpleTransform2 const m_simpletransform0;
    FloatMatrix2 const m_product_of_m0_and_st0;
    FloatMatrix2 const m_product_of_st0_and_m0;

    Float const m_angle;
    FloatMatrix2 const m_matrix_rotated;
    FloatVector2 const m_vector_rotated;

    FloatMatrix2 const m_matrix_scaled_nonuniformly;
    FloatVector2 const m_scale_factors;

    FloatMatrix2 const m_matrix_scaled_uniformly;
    Float const m_scale_factor;

    FloatMatrix2 const m_matrix_translated;
    FloatVector2 const m_translation;
}; // end of class Matrix2UnitTest

#endif // !defined(_MATRIX2UNITTEST_H_)

