// ///////////////////////////////////////////////////////////////////////////
// simpletransform2unittest.hpp by Victor Dods, created 2005/05/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_SIMPLETRANSFORM2UNITTEST_HPP_)
#define _SIMPLETRANSFORM2UNITTEST_HPP_

#include "../defines.hpp"

#include "unittest.hpp"

#include "../simpletransform2.hpp"

class SimpleTransform2UnitTest : public UnitTest
{
public:

    SimpleTransform2UnitTest ();
    virtual ~SimpleTransform2UnitTest ();

    virtual void Initialize ();

protected:

    void AtomConstructByComponents ();
    void AtomCopyConstructor ();
    void AtomOperatorBracket ();
    void AtomInplaceMultiplySimpleTransform2 ();
    void AtomMultiplySimpleTransform2 ();
    void AtomInplaceMultiplyVector2 ();
    void AtomMultiplyVector2 ();
    void AtomInverse ();

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
    void TestSimpleTransform2Equality (
        SimpleTransform2<T> const &simple_transform0,
        char const *simple_transform0_name,
        SimpleTransform2<T> const &simple_transform1,
        char const *simple_transform1_name,
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
        for (Uint32 i = 0; i < 2; ++i)
            Test(Epsilon(simple_transform0.m_scale_factors[i],
                         simple_transform1.m_scale_factors[i],
                         epsilon),
                 printf_format_buffer,
                 i == 0 ? 'R' : 'S',
                 simple_transform0_name,
                 simple_transform0.m_scale_factors[i],
                 i == 0 ? 'R' : 'S',
                 simple_transform1_name,
                 simple_transform1.m_scale_factors[i]);
        for (Uint32 i = 0; i < 2; ++i)
            Test(Epsilon(simple_transform0.m_translation[i],
                         simple_transform1.m_translation[i],
                         epsilon),
                 printf_format_buffer,
                 i == 0 ? 'X' : 'Y',
                 simple_transform0_name,
                 simple_transform0.m_translation[i],
                 i == 0 ? 'X' : 'Y',
                 simple_transform1_name,
                 simple_transform1.m_translation[i]);
    }

    FloatSimpleTransform2 const m_transform0;
    FloatSimpleTransform2 const m_transform1;
    FloatSimpleTransform2 const m_product_of_t0_and_t1;
    FloatSimpleTransform2 const m_product_of_t1_and_t0;
    FloatSimpleTransform2 const m_transform0_inverse;
    FloatSimpleTransform2 const m_transform1_inverse;

    FloatVector2 const m_one_one_multiplied_by_t0;
    FloatVector2 const m_one_one_multiplied_by_t1;
}; // end of class SimpleTransform2UnitTest

#endif // !defined(_SIMPLETRANSFORM2UNITTEST_HPP_)

