// ///////////////////////////////////////////////////////////////////////////
// transform2unittest.hpp by Victor Dods, created 2005/05/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_TRANSFORM2UNITTEST_HPP_)
#define _TRANSFORM2UNITTEST_HPP_

#include "../defines.hpp"

#include "unittest.hpp"

#include "../transform2.hpp"

class Transform2UnitTest : public UnitTest
{
public:

    Transform2UnitTest ();
    virtual ~Transform2UnitTest ();

    virtual void Initialize ();

protected:

    void AtomConstructors ();
    void AtomAssignmentOperator ();
    void AtomModifiers ();

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

    template <typename T>
    void TestTransform2Equality (
        Transform2<T> const &transform0,
        char const *transform0_name,
        Transform2<T> const &transform1,
        char const *transform1_name,
        char const *component_printf_format,
        T epsilon) const
    {
        Uint32 const printf_format_buffer_size = 0x1000;
        char printf_format_buffer[printf_format_buffer_size];
        snprintf(
            printf_format_buffer,
            printf_format_buffer_size,
            "%%s %%c of %%s (%s) == %%s %%c of %%s (%s)",
            component_printf_format,
            component_printf_format);
        for (Uint32 i = 0; i < 2; ++i)
            Test(Epsilon(transform0.GetTranslation()[i],
                         transform1.GetTranslation()[i],
                         epsilon),
                 printf_format_buffer,
                 "translation",
                 i == 0 ? 'X' : 'Y',
                 transform0_name,
                 transform0.GetTranslation()[i],
                 "translation",
                 i == 0 ? 'X' : 'Y',
                 transform1_name,
                 transform1.GetTranslation()[i]);
        for (Uint32 i = 0; i < 2; ++i)
            Test(Epsilon(transform0.GetScaleFactors()[i],
                         transform1.GetScaleFactors()[i],
                         epsilon),
                 printf_format_buffer,
                 "scale factors",
                 i == 0 ? 'X' : 'Y',
                 transform0_name,
                 transform0.GetScaleFactors()[i],
                 "scale factors",
                 i == 0 ? 'X' : 'Y',
                 transform1_name,
                 transform1.GetScaleFactors()[i]);
        snprintf(
            printf_format_buffer,
            printf_format_buffer_size,
            "%%s of %%s (%s) == %%s of %%s (%s)",
            component_printf_format,
            component_printf_format);
        Test(Epsilon(transform0.GetAngle(),
                     transform1.GetAngle(),
                     epsilon),
             printf_format_buffer,
             "angle",
             transform0_name,
             transform0.GetAngle(),
             "angle",
             transform1_name,
             transform1.GetAngle());
        Test(transform0.GetPostTranslate() == transform1.GetPostTranslate(),
             "%s of %s (%s) == %s of %s (%s)",
             "post-translate",
             transform0_name,
             BOOL_TO_STRING(transform0.GetPostTranslate()),
             "post-translate",
             transform1_name,
             BOOL_TO_STRING(transform1.GetPostTranslate()));
    }
}; // end of class Transform2UnitTest

#endif // !defined(_TRANSFORM2UNITTEST_HPP_)

