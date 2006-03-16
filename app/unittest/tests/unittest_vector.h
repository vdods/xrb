// ///////////////////////////////////////////////////////////////////////////
// vectorunittest.h by Victor Dods, created 2005/05/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_VECTORUNITTEST_H_)
#define _VECTORUNITTEST_H_

#include "../defines.h"

#include "unittest.h"

#include "../vector.h"

class VectorUnitTest : public UnitTest
{
public:

    VectorUnitTest ();
    virtual ~VectorUnitTest ();

    virtual void Initialize ();

protected:

    void AtomConstruction ();
    void AtomAssignment ();
    void AtomComparisonOperators ();
    void AtomClassOperatorBracket ();
    void AtomInplaceAddition ();
    void AtomInplaceSubtraction ();
    void AtomInplaceMultiplication ();
    void AtomInplaceDivision ();
    void AtomNegation ();
    void AtomLength ();
    void AtomNormalization ();
    void AtomZeroComparison ();
    void AtomFillWith ();
    void AtomSetComponents ();
    void AtomGlobalOperators ();
    void AtomAddition ();
    void AtomSubtraction ();
    void AtomProduct ();
    void AtomDivision ();
    void AtomDotProduct ();
    void AtomCrossProduct ();
    void AtomOperatorProperties ();

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

    FloatVector2 const m_a;
    FloatVector2 const m_b;
    Float const m_s;
}; // end of class VectorUnitTest

#endif // !defined(_VECTORUNITTEST_H_)

