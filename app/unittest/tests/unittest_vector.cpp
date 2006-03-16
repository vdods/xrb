// ///////////////////////////////////////////////////////////////////////////
// vectorunittest.cpp by Victor Dods, created 2005/05/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "vectorunittest.h"

#include <math.h>

VectorUnitTest::VectorUnitTest ()
    :
    UnitTest("Vector"),
    m_a(3.5f, 8.9f),
    m_b(-1.4f, 34.78f),
    m_s(13.8f)
{
    RegisterAtom(&VectorUnitTest::AtomConstruction);
    RegisterAtom(&VectorUnitTest::AtomAssignment);
    RegisterAtom(&VectorUnitTest::AtomComparisonOperators);
    RegisterAtom(&VectorUnitTest::AtomClassOperatorBracket);
    RegisterAtom(&VectorUnitTest::AtomInplaceAddition);
    RegisterAtom(&VectorUnitTest::AtomInplaceSubtraction);
    RegisterAtom(&VectorUnitTest::AtomInplaceMultiplication);
    RegisterAtom(&VectorUnitTest::AtomInplaceDivision);
    RegisterAtom(&VectorUnitTest::AtomNegation);
    RegisterAtom(&VectorUnitTest::AtomLength);
    RegisterAtom(&VectorUnitTest::AtomNormalization);
    RegisterAtom(&VectorUnitTest::AtomZeroComparison);
    RegisterAtom(&VectorUnitTest::AtomFillWith);
    RegisterAtom(&VectorUnitTest::AtomSetComponents);
    RegisterAtom(&VectorUnitTest::AtomAddition);
    RegisterAtom(&VectorUnitTest::AtomSubtraction);
    RegisterAtom(&VectorUnitTest::AtomProduct);
    RegisterAtom(&VectorUnitTest::AtomDivision);
    RegisterAtom(&VectorUnitTest::AtomDotProduct);
    RegisterAtom(&VectorUnitTest::AtomCrossProduct);
    RegisterAtom(&VectorUnitTest::AtomOperatorProperties);
}

VectorUnitTest::~VectorUnitTest ()
{
}

void VectorUnitTest::Initialize ()
{
}

void VectorUnitTest::AtomConstruction ()
{
    {
        PrintSubsectionHeader("Checking construction by components");
        FloatVector2 constructed(2.4f, 8.7f);
        FloatVector2 precomputed;
        precomputed.m[Dim::X] = 2.4f;
        precomputed.m[Dim::Y] = 8.7f;
        TestVectorEquality(
            constructed,
            "constructed vector",
            precomputed,
            "precomputed vector",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader("Checking construction by component array");
        Float component_array[2] = {2.4f, 8.7f};
        FloatVector2 constructed(component_array);
        FloatVector2 precomputed;
        precomputed.m[Dim::X] = 2.4f;
        precomputed.m[Dim::Y] = 8.7f;
        TestVectorEquality(
            constructed,
            "constructed vector",
            precomputed,
            "precomputed vector",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader("Checking copy constructor");
        FloatVector2 constructed(FloatVector2::ms_zero);
        TestVectorEquality(
            constructed,
            "constructed vector",
            FloatVector2::ms_zero,
            "precomputed vector",
            "%f",
            0.0f);
    }
}

void VectorUnitTest::AtomAssignment ()
{
    PrintSubsectionHeader("Checking assignment operator");
    FloatVector2 source(666.0f, 667.0f);
    FloatVector2 assignee;
    assignee = source;
    TestVectorEquality(
        assignee,
        "assignee",
        source,
        "source",
        "%f",
        0.0f);
}

void VectorUnitTest::AtomComparisonOperators ()
{
    FloatVector2 a(FloatVector2::ms_zero);
    FloatVector2 b(FloatVector2::ms_zero);
    FloatVector2 c(1.0f, 6.0f);

    PrintSubsectionHeader("Checking equality operator");
    Test(a == b,
         "the zero vector == the zero vector");
    Test(!(a == c),
         "!(the zero vector == a nonzero vector)");

    PrintSubsectionHeader("Checking inequality operator");
    Test(a != c,
         "the zero vector != a non zero vector");
    Test(!(a != b),
         "!(the zero vector != the zero vector)");
}

void VectorUnitTest::AtomClassOperatorBracket ()
{
    PrintSubsectionHeader("Checking operator[]");

    FloatVector2 test_vector(65.75f, 1035.6125);
    TestEquality(
        test_vector.m[Dim::X],
        "test_vector.m[Dim::X]",
        test_vector[Dim::X],
        "test_vector[Dim::X]",
        "%f");
    TestEquality(
        test_vector.m[Dim::Y],
        "test_vector.m[Dim::Y]",
        test_vector[Dim::Y],
        "test_vector[Dim::Y]",
        "%f");

    PrintSubsectionHeader("Checking assignment to non-const operator[] references");

    FloatVector2 another_test_vector(test_vector);
    another_test_vector[Dim::X] += 1.0f;
    TestInequality(
        another_test_vector.m[Dim::X],
        "changed value of component Dim::X",
        test_vector.m[Dim::X],
        "component Dim::X source value",
        "%f");
}

void VectorUnitTest::AtomInplaceAddition ()
{
    FloatVector2 a_plus_b(
        m_a.m[Dim::X] + m_b.m[Dim::X],
        m_a.m[Dim::Y] + m_b.m[Dim::Y]);
    FloatVector2 b_plus_a(
        m_b.m[Dim::X] + m_a.m[Dim::X],
        m_b.m[Dim::Y] + m_a.m[Dim::Y]);

    {
        PrintSubsectionHeader("Checking in-place addition");
        FloatVector2 test_vector(m_a);
        test_vector += m_b;
        TestVectorEquality(
            test_vector,
            "calculated sum",
            a_plus_b,
            "precomputed sum",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader("Checking commuted sum");
        FloatVector2 test_vector(m_b);
        test_vector += m_a;
        TestVectorEquality(
            test_vector,
            "calculated sum",
            b_plus_a,
            "precomputed sum",
            "%f",
            0.0f);
    }
}

void VectorUnitTest::AtomInplaceSubtraction ()
{
    FloatVector2 a_minus_b(
        m_a.m[Dim::X] - m_b.m[Dim::X],
        m_a.m[Dim::Y] - m_b.m[Dim::Y]);
    FloatVector2 b_minus_a(
        m_b.m[Dim::X] - m_a.m[Dim::X],
        m_b.m[Dim::Y] - m_a.m[Dim::Y]);

    {
        PrintSubsectionHeader("Checking in-place subtraction");
        FloatVector2 test_vector(m_a);
        test_vector -= m_b;
        TestVectorEquality(
            test_vector,
            "calculated difference",
            a_minus_b,
            "precomputed difference",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader("Checking commuted difference");
        FloatVector2 test_vector(m_b);
        test_vector -= m_a;
        TestVectorEquality(
            test_vector,
            "calculated difference",
            b_minus_a,
            "precomputed difference",
            "%f",
            0.0f);
    }
}

void VectorUnitTest::AtomInplaceMultiplication ()
{
    FloatVector2 a_times_b(
        m_a.m[Dim::X] * m_b.m[Dim::X],
        m_a.m[Dim::Y] * m_b.m[Dim::Y]);
    FloatVector2 b_times_a(
        m_b.m[Dim::X] * m_a.m[Dim::X],
        m_b.m[Dim::Y] * m_a.m[Dim::Y]);
    FloatVector2 a_times_s(
        m_a.m[Dim::X] * m_s,
        m_a.m[Dim::Y] * m_s);
    FloatVector2 b_times_s(
        m_b.m[Dim::X] * m_s,
        m_b.m[Dim::Y] * m_s);

    {
        PrintSubsectionHeader(
            "Checking in-place component-wise "
            "multiplication by a vector");
        FloatVector2 test_vector(m_a);
        test_vector *= m_b;
        TestVectorEquality(
            test_vector,
            "calculated product",
            a_times_b,
            "precomputed product",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader("Checking commuted product");
        FloatVector2 test_vector(m_b);
        test_vector *= m_a;
        TestVectorEquality(
            test_vector,
            "calculated product",
            b_times_a,
            "precomputed product",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader("Checking in-place multiplication by a scalar");
        FloatVector2 test_vector(m_a);
        test_vector *= m_s;
        TestVectorEquality(
            test_vector,
            "calculated product",
            a_times_s,
            "precomputed product",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader("Checking another in-place multiplication by a scalar");
        FloatVector2 test_vector(m_b);
        test_vector *= m_s;
        TestVectorEquality(
            test_vector,
            "calculated product",
            b_times_s,
            "precomputed product",
            "%f",
            0.0f);
    }
}

void VectorUnitTest::AtomInplaceDivision ()
{
    FloatVector2 a_divided_by_b(
        m_a.m[Dim::X] / m_b.m[Dim::X],
        m_a.m[Dim::Y] / m_b.m[Dim::Y]);
    FloatVector2 b_divided_by_a(
        m_b.m[Dim::X] / m_a.m[Dim::X],
        m_b.m[Dim::Y] / m_a.m[Dim::Y]);
    FloatVector2 a_divided_by_s(
        m_a.m[Dim::X] / m_s,
        m_a.m[Dim::Y] / m_s);
    FloatVector2 b_divided_by_s(
        m_b.m[Dim::X] / m_s,
        m_b.m[Dim::Y] / m_s);

    {
        PrintSubsectionHeader(
            "Checking in-place component-wise "
            "division by a vector");
        FloatVector2 test_vector(m_a);
        test_vector /= m_b;
        TestVectorEquality(
            test_vector,
            "calculated quotient",
            a_divided_by_b,
            "precomputed quotient",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader("Checking commuted quotient");
        FloatVector2 test_vector(m_b);
        test_vector /= m_a;
        TestVectorEquality(
            test_vector,
            "calculated quotient",
            b_divided_by_a,
            "precomputed quotient",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader("Checking in-place division by a scalar");
        FloatVector2 test_vector(m_a);
        test_vector /= m_s;
        TestVectorEquality(
            test_vector,
            "calculated quotient",
            a_divided_by_s,
            "precomputed quotient",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader("Checking another in-place division by a scalar");
        FloatVector2 test_vector(m_b);
        test_vector /= m_s;
        TestVectorEquality(
            test_vector,
            "calculated quotient",
            b_divided_by_s,
            "precomputed quotient",
            "%f",
            0.0f);
    }
}

void VectorUnitTest::AtomNegation ()
{
    FloatVector2 negative_a(-m_a.m[Dim::X], -m_a.m[Dim::Y]);
    FloatVector2 negative_b(-m_b.m[Dim::X], -m_b.m[Dim::Y]);

    {
        PrintSubsectionHeader("Checking unary negation operator");
        FloatVector2 test_vector(-m_a);
        TestVectorEquality(
            test_vector,
            "calculated negative vector",
            negative_a,
            "precomputed negative vector",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader("Checking additional value for unary negation");
        FloatVector2 test_vector(-m_b);
        TestVectorEquality(
            test_vector,
            "calculated negative vector",
            negative_b,
            "precomputed negative vector",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader("Checking Negate()");
        FloatVector2 test_vector(m_a);
        test_vector.Negate();
        TestVectorEquality(
            test_vector,
            "calculated negative vector",
            negative_a,
            "precomputed negative vector",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader("Checking additional value for Negate()");
        FloatVector2 test_vector(m_b);
        test_vector.Negate();
        TestVectorEquality(
            test_vector,
            "calculated negative vector",
            negative_b,
            "precomputed negative vector",
            "%f",
            0.0f);
    }
}

void VectorUnitTest::AtomLength ()
{
    Float length_squared_of_a =
        m_a.m[Dim::X]*m_a.m[Dim::X] +
        m_a.m[Dim::Y]*m_a.m[Dim::Y];
    Float length_of_a =
        FLOAT_SQRT(length_squared_of_a);
    Float length_squared_of_b =
        m_b.m[Dim::X]*m_b.m[Dim::X] +
        m_b.m[Dim::Y]*m_b.m[Dim::Y];
    Float length_of_b =
        FLOAT_SQRT(length_squared_of_b);

    {
        PrintSubsectionHeader("Checking GetLength()");
        TestEquality(
            m_a.GetLength(),
            "calculated length",
            length_of_a,
            "precomputed length",
            "%f");

        PrintSubsectionHeader("Checking additional value for GetLength()");
        TestEquality(
            m_b.GetLength(),
            "calculated length",
            length_of_b,
            "precomputed length",
            "%f");
    }

    {
        PrintSubsectionHeader("Checking GetLengthSquared()");
        TestEquality(
            m_a.GetLengthSquared(),
            "calculated length squared",
            length_squared_of_a,
            "precomputed length squared",
            "%f");

        PrintSubsectionHeader("Checking additional value for GetLengthSquared()");
        TestEquality(
            m_b.GetLengthSquared(),
            "calculated length squared",
            length_squared_of_b,
            "precomputed length squared",
            "%f");
    }
}

void VectorUnitTest::AtomNormalization ()
{
    Float length_squared_of_a =
        m_a.m[Dim::X]*m_a.m[Dim::X] +
        m_a.m[Dim::Y]*m_a.m[Dim::Y];
    Float length_of_a =
        FLOAT_SQRT(length_squared_of_a);
    Float length_squared_of_b =
        m_b.m[Dim::X]*m_b.m[Dim::X] +
        m_b.m[Dim::Y]*m_b.m[Dim::Y];
    Float length_of_b =
        FLOAT_SQRT(length_squared_of_b);

    {
        PrintSubsectionHeader("Checking GetNormalization()");
        TestVectorEquality(
            m_a.GetNormalization(),
            "calculated normalized vector",
            m_a / length_of_a,
            "precomputed normalized vector",
            "%f",
            0.0f);

        PrintSubsectionHeader("Checking additional value for GetNormalization()");
        TestVectorEquality(
            m_b.GetNormalization(),
            "calculated normalized vector",
            m_b / length_of_b,
            "precomputed normalized vector",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader("Checking Normalize()");
        FloatVector2 normalized_a(m_a);
        normalized_a.Normalize();
        TestVectorEquality(
            normalized_a,
            "calculated normalization",
            m_a / length_of_a,
            "precomputed normalization",
            "%f",
            0.0f);

        PrintSubsectionHeader("Checking additional value for Normalize()");
        FloatVector2 normalized_b(m_b);
        normalized_b.Normalize();
        TestVectorEquality(
            normalized_b,
            "calculated normalization",
            m_b / length_of_b,
            "precomputed normalization",
            "%f",
            0.0f);
    }
}

void VectorUnitTest::AtomZeroComparison ()
{
    FloatVector2 non_zero_vector(1.0f, 2.0f);

    PrintSubsectionHeader("Checking GetIsZero()");
    Test(FloatVector2::ms_zero.GetIsZero(),
         "ms_zero.GetIsZero()");
    Test(!(non_zero_vector.GetIsZero()),
         "!(non_zero_vector.GetIsZero())");

    PrintSubsectionHeader("Checking GetIsNonZero()");
    Test(!(FloatVector2::ms_zero.GetIsNonZero()),
         "!(ms_zero.GetIsNonZero())");
    Test(non_zero_vector.GetIsNonZero(),
         "non_zero_vector.GetIsNonZero()");
}

void VectorUnitTest::AtomFillWith ()
{
    PrintSubsectionHeader("Checking FillWith()");
    FloatVector2 filled_with_zeros;
    filled_with_zeros.FillWith(0.0f);
    FloatVector2 precomputed_filled_with_zeros;
    precomputed_filled_with_zeros.m[Dim::X] = 0.0f;
    precomputed_filled_with_zeros.m[Dim::Y] = 0.0f;
    TestVectorEquality(
        filled_with_zeros,
        "vector filled with 0",
        precomputed_filled_with_zeros,
        "precomputed vector filled 0",
        "%f",
        0.0f);

    FloatVector2 filled_with_three_point_fives;
    FloatVector2 precomputed_filled_with_three_point_fives;
    filled_with_three_point_fives.FillWith(3.5f);
    precomputed_filled_with_three_point_fives.m[Dim::X] = 3.5f;
    precomputed_filled_with_three_point_fives.m[Dim::Y] = 3.5f;
    TestVectorEquality(
        filled_with_three_point_fives,
        "vector filled with 3.5",
        precomputed_filled_with_three_point_fives,
        "precomputed vector filled with 3.5",
        "%f",
        0.0f);
}

void VectorUnitTest::AtomSetComponents ()
{
    FloatVector2 setup_a;
    FloatVector2 setup_b;

    PrintSubsectionHeader("Checking SetComponents() using individual components");
    setup_a.SetComponents(m_a.m[Dim::X], m_a.m[Dim::Y]);
    TestVectorEquality(
        setup_a,
        "calculated vector",
        m_a,
        "precomputed vector",
        "%f",
        0.0f);

    PrintSubsectionHeader(
        "Checking SetComponents() using array components");
    setup_b.SetComponents(m_b.m);
    TestVectorEquality(
        setup_b,
        "calculated vector",
        m_b,
        "precomputed vector",
        "%f",
        0.0f);
}

void VectorUnitTest::AtomAddition ()
{
    {
        PrintSubsectionHeader("Checking global operator + (vector addition)");
        FloatVector2 a_plus_b(
            m_a.m[Dim::X] + m_b.m[Dim::X],
            m_a.m[Dim::Y] + m_b.m[Dim::Y]);
        TestVectorEquality(
            m_a + m_b,
            "calculated vector sum",
            a_plus_b,
            "precomputed vector sum",
            "%f",
            0.0f);

        PrintSubsectionHeader("Checking commuted sum");
        FloatVector2 b_plus_a(
            m_b.m[Dim::X] + m_a.m[Dim::X],
            m_b.m[Dim::Y] + m_a.m[Dim::Y]);
        TestVectorEquality(
            m_b + m_a,
            "calculated vector sum",
            b_plus_a,
            "precomputed vector sum",
            "%f",
            0.0f);
    }
}

void VectorUnitTest::AtomSubtraction ()
{
    {
        PrintSubsectionHeader("Checking global operator - (vector subtraction)");
        FloatVector2 a_minus_b(
            m_a.m[Dim::X] - m_b.m[Dim::X],
            m_a.m[Dim::Y] - m_b.m[Dim::Y]);
        TestVectorEquality(
            m_a - m_b,
            "calculated vector difference",
            a_minus_b,
            "precomputed vector difference",
            "%f",
            0.0f);

        PrintSubsectionHeader("Checking commuted difference");
        FloatVector2 b_minus_a(
            m_b.m[Dim::X] - m_a.m[Dim::X],
            m_b.m[Dim::Y] - m_a.m[Dim::Y]);
        TestVectorEquality(
            m_b - m_a,
            "calculated vector difference",
            b_minus_a,
            "precomputed vector difference",
            "%f",
            0.0f);
    }
}

void VectorUnitTest::AtomProduct ()
{
    {
        PrintSubsectionHeader(
            "Checking global operator * "
            "(component-wise vector multiplication)");
        FloatVector2 a_times_b(
            m_a.m[Dim::X] * m_b.m[Dim::X],
            m_a.m[Dim::Y] * m_b.m[Dim::Y]);
        TestVectorEquality(
            m_a * m_b,
            "calculated vector product",
            a_times_b,
            "precomputed vector product",
            "%f",
            0.0f);

        PrintSubsectionHeader("Checking commuted product");
        FloatVector2 b_times_a(
            m_b.m[Dim::X] * m_a.m[Dim::X],
            m_b.m[Dim::Y] * m_a.m[Dim::Y]);
        TestVectorEquality(
            m_b * m_a,
            "calculated vector product",
            b_times_a,
            "precomputed vector product",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader(
            "Checking global operator * (scalar multiplication");
        FloatVector2 a_times_s(
            m_a.m[Dim::X] * m_s,
            m_a.m[Dim::Y] * m_s);
        TestVectorEquality(
            m_a * m_s,
            "calculated vector product",
            a_times_s,
            "precomputed vector product",
            "%f",
            0.0f);

        PrintSubsectionHeader("Checking commuted product");
        FloatVector2 s_times_a(
            m_s * m_a.m[Dim::X],
            m_s * m_a.m[Dim::Y]);
        TestVectorEquality(
            m_s * m_a,
            "calculated vector product",
            s_times_a,
            "precomputed vector product",
            "%f",
            0.0f);
    }
}

void VectorUnitTest::AtomDivision ()
{
    {
        PrintSubsectionHeader(
            "Checking global operator / "
            "(component-wise vector division)");
        FloatVector2 a_divided_by_b(
            m_a.m[Dim::X] / m_b.m[Dim::X],
            m_a.m[Dim::Y] / m_b.m[Dim::Y]);
        TestVectorEquality(
            m_a / m_b,
            "calculated vector quotient",
            a_divided_by_b,
            "precomputed vector quotient",
            "%f",
            0.0f);

        PrintSubsectionHeader("Checking commuted quotient");
        FloatVector2 b_divided_by_a(
            m_b.m[Dim::X] / m_a.m[Dim::X],
            m_b.m[Dim::Y] / m_a.m[Dim::Y]);
        TestVectorEquality(
            m_b / m_a,
            "calculated vector quotient",
            b_divided_by_a,
            "precomputed vector quotient",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader(
            "Checking global operator / (scalar division");
        FloatVector2 a_divided_by_s(
            m_a.m[Dim::X] / m_s,
            m_a.m[Dim::Y] / m_s);
        TestVectorEquality(
            m_a / m_s,
            "calculated vector quotient",
            a_divided_by_s,
            "precomputed vector quotient",
            "%f",
            0.0f);

        PrintSubsectionHeader("Checking commuted quotient");
        FloatVector2 s_divided_by_a(
            m_s / m_a.m[Dim::X],
            m_s / m_a.m[Dim::Y]);
        TestVectorEquality(
            m_s / m_a,
            "calculated vector quotient",
            s_divided_by_a,
            "precomputed vector quotient",
            "%f",
            0.0f);
    }
}

void VectorUnitTest::AtomDotProduct ()
{
    {
        PrintSubsectionHeader(
            "Checking global operator | (Euclidean inner product)");
        Float a_dot_b =
            m_a.m[Dim::X] * m_b.m[Dim::X] +
            m_a.m[Dim::Y] * m_b.m[Dim::Y];
        TestEquality(
            m_a | m_b,
            "calculated dot product",
            a_dot_b,
            "precomputed dot product",
            "%f");

        PrintSubsectionHeader(
            "Checking commuted product");
        Float b_dot_a =
            m_b.m[Dim::X] * m_a.m[Dim::X] +
            m_b.m[Dim::Y] * m_a.m[Dim::Y];
        TestEquality(
            m_b | m_a,
            "calculated dot product",
            b_dot_a,
            "precomputed dot product",
            "%f");
    }
}

void VectorUnitTest::AtomCrossProduct ()
{
    {
        PrintSubsectionHeader(
            "Checking global operator & for 2D Vector (scalar cross product)");
        Float a_cross_b =
            m_a.m[Dim::X] * m_b.m[Dim::Y] -
            m_a.m[Dim::Y] * m_b.m[Dim::X];
        TestEquality(
            m_a & m_b,
            "calculated cross product",
            a_cross_b,
            "precomputed cross product",
            "%f");

        PrintSubsectionHeader(
            "Checking commuted product");
        Float b_cross_a =
            m_b.m[Dim::X] * m_a.m[Dim::Y] -
            m_b.m[Dim::Y] * m_a.m[Dim::X];
        TestEquality(
            m_b & m_a,
            "calculated cross product",
            b_cross_a,
            "precomputed cross product",
            "%f");
    }
}

void VectorUnitTest::AtomOperatorProperties ()
{
    PrintSubsectionHeader("Checking various vector operator properties");

    TestEquality(
        m_a | m_a,
        "a dot a",
        m_a.GetLengthSquared(),
        "(length of a)^2",
        "%f");

    TestEquality(
        GetPerpendicularVector2(m_a) | m_a,
        "GetPerpendicularVector2(a) dot a",
        0.0f,
        "zero",
        "%f",
        1e-6f);

    TestEquality(
        m_a & GetPerpendicularVector2(m_a),
        "a cross GetPerpendicularVector2(a)",
        m_a.GetLengthSquared(),
        "(length of a)^2",
        "%f",
        1e-6f);

    TestEquality(
        m_a & m_b,
        "a cross b",
        -(m_b & m_a),
        "-(b cross a)",
        "%f",
        1e-6f);
}

