// ///////////////////////////////////////////////////////////////////////////
// simpletransform2unittest.cpp by Victor Dods, created 2005/05/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "simpletransform2unittest.hpp"

SimpleTransform2UnitTest::SimpleTransform2UnitTest ()
    :
    UnitTest("SimpleTransform2"),
    m_transform0(5.75f, -3.5f, 10.0f, -1.5f),
    m_transform1(2.0f, 3.75f, 0.5f, 6.0f),
    m_product_of_t0_and_t1(11.5f, -13.125f, 12.875f, -22.5f),
    m_product_of_t1_and_t0(11.5f, -13.125f, 20.5f, 0.375f),
    m_transform0_inverse(
        -3.5f/-20.125f, 5.75f/-20.125f, 35.0f/-20.125f, 8.625f/-20.125f),
    m_transform1_inverse(
        3.75f/7.5f, 2.0f/7.5f, -1.875f/7.5f, -12.0f/7.5f),
    m_one_one_multiplied_by_t0(15.75f, -5.0f),
    m_one_one_multiplied_by_t1(2.5f, 9.75f)
{
    RegisterAtom(&SimpleTransform2UnitTest::AtomConstructByComponents);
    RegisterAtom(&SimpleTransform2UnitTest::AtomCopyConstructor);
    RegisterAtom(&SimpleTransform2UnitTest::AtomOperatorBracket);
    RegisterAtom(&SimpleTransform2UnitTest::AtomInplaceMultiplySimpleTransform2);
    RegisterAtom(&SimpleTransform2UnitTest::AtomMultiplySimpleTransform2);
    RegisterAtom(&SimpleTransform2UnitTest::AtomInplaceMultiplyVector2);
    RegisterAtom(&SimpleTransform2UnitTest::AtomMultiplyVector2);
    RegisterAtom(&SimpleTransform2UnitTest::AtomInverse);
}

SimpleTransform2UnitTest::~SimpleTransform2UnitTest ()
{
}

void SimpleTransform2UnitTest::Initialize ()
{
}

void SimpleTransform2UnitTest::AtomConstructByComponents ()
{
    PrintSubsectionHeader("Checking construction by components");

    FloatSimpleTransform2 constructed(1.0f, 2.0f, 3.0f, 4.0f);
    FloatSimpleTransform2 precomputed;
    precomputed.m_scale_factors.m[Dim::X] = 1.0f;
    precomputed.m_scale_factors.m[Dim::Y] = 2.0f;
    precomputed.m_translation.m[Dim::X] = 3.0f;
    precomputed.m_translation.m[Dim::Y] = 4.0f;

    TestSimpleTransform2Equality(
        constructed,
        "constructed transform",
        precomputed,
        "precomputed transform",
        "%f",
        0.0f);
}

void SimpleTransform2UnitTest::AtomCopyConstructor ()
{
    PrintSubsectionHeader("Checking copy constructor");

    FloatSimpleTransform2 precomputed;
    precomputed.m_scale_factors.m[Dim::X] = 10.0f;
    precomputed.m_scale_factors.m[Dim::Y] = 25.0f;
    precomputed.m_translation.m[Dim::X] = 31.0f;
    precomputed.m_translation.m[Dim::Y] = 42.0f;
    FloatSimpleTransform2 constructed(precomputed);

    TestSimpleTransform2Equality(
        constructed,
        "constructed transform",
        precomputed,
        "precomputed transform",
        "%f",
        0.0f);
}

void SimpleTransform2UnitTest::AtomOperatorBracket ()
{
    PrintSubsectionHeader("Checking operator[]");

    FloatSimpleTransform2 test_matrix(82.3f, 125.365f, 1985.01f, 359.2f);

    TestEquality(
        test_matrix.m_scale_factors[Dim::X],
        "test_matrix.m_scale_factors[Dim::X]",
        test_matrix[FloatSimpleTransform2::R],
        "test_matrix[FloatSimpleTransform2::R]",
        "%f");
    TestEquality(
        test_matrix.m_scale_factors[Dim::Y],
        "test_matrix.m_scale_factors[Dim::Y]",
        test_matrix[FloatSimpleTransform2::S],
        "test_matrix[FloatSimpleTransform2::S]",
        "%f");
    TestEquality(
        test_matrix.m_translation[Dim::X],
        "test_matrix.m_translation[Dim::X]",
        test_matrix[FloatSimpleTransform2::X],
        "test_matrix[FloatSimpleTransform2::X]",
        "%f");
    TestEquality(
        test_matrix.m_translation[Dim::Y],
        "test_matrix.m_translation[Dim::Y]",
        test_matrix[FloatSimpleTransform2::Y],
        "test_matrix[FloatSimpleTransform2::Y]",
        "%f");

    PrintSubsectionHeader("Testing assignment to non-const operator[] references");

    FloatSimpleTransform2 another_test_matrix(test_matrix);
    another_test_matrix[FloatSimpleTransform2::R] += 1.0f;
    TestInequality(
        another_test_matrix[FloatSimpleTransform2::R],
        "changed value of component R",
        test_matrix[FloatSimpleTransform2::R],
        "component R source value",
        "%f");
}

void SimpleTransform2UnitTest::AtomInplaceMultiplySimpleTransform2 ()
{
    PrintSubsectionHeader("Checking in-place multiplication by SimpleTransform2");

    FloatSimpleTransform2 product_of_t0_and_t1(m_transform1);
    product_of_t0_and_t1 *= m_transform0;
    TestSimpleTransform2Equality(
        product_of_t0_and_t1,
        "calculated product",
        m_product_of_t0_and_t1,
        "precomputed product",
        "%f",
        0.0f);

    PrintSubsectionHeader("Checking commuted product");
    FloatSimpleTransform2 product_of_t1_and_t0(m_transform0);
    product_of_t1_and_t0 *= m_transform1;
    TestSimpleTransform2Equality(
        product_of_t1_and_t0,
        "calculated product",
        m_product_of_t1_and_t0,
        "precomputed product",
        "%f",
        0.0f);
}

void SimpleTransform2UnitTest::AtomMultiplySimpleTransform2 ()
{
    PrintSubsectionHeader("Checking multiplication by SimpleTransform2");

    FloatSimpleTransform2 product_of_t0_and_t1(m_transform0 * m_transform1);
    TestSimpleTransform2Equality(
        product_of_t0_and_t1,
        "calculated product",
        m_product_of_t0_and_t1,
        "precomputed product",
        "%f",
        0.0f);

    PrintSubsectionHeader("Checking commuted product");
    FloatSimpleTransform2 product_of_t1_and_t0(m_transform1 * m_transform0);
    TestSimpleTransform2Equality(
        product_of_t1_and_t0,
        "calculated product",
        m_product_of_t1_and_t0,
        "precomputed product",
        "%f",
        0.0f);
}

void SimpleTransform2UnitTest::AtomInplaceMultiplyVector2 ()
{
    PrintSubsectionHeader(
        "Checking in-place multiplication of a vector "
        "by a SimpleTransform2");

    FloatVector2 one_one(1.0f, 1.0f);

    FloatVector2 product_with_t0(one_one);
    product_with_t0 *= m_transform0;
    TestVectorEquality(
        product_with_t0,
        "calculated product",
        m_one_one_multiplied_by_t0,
        "precomputed product",
        "%f",
        0.0f);

    PrintSubsectionHeader("Checking commuted product");
    FloatVector2 product_with_t1(one_one);
    product_with_t1 *= m_transform1;
    TestVectorEquality(
        product_with_t1,
        "calculated product",
        m_one_one_multiplied_by_t1,
        "precomputed product",
        "%f",
        0.0f);
}

void SimpleTransform2UnitTest::AtomMultiplyVector2 ()
{
    PrintSubsectionHeader(
        "Checking multiplication of a vector "
        "by a SimpleTransform2");

    FloatVector2 one_one(1.0f, 1.0f);

    FloatVector2 product_with_t0(m_transform0 * one_one);
    TestVectorEquality(
        product_with_t0,
        "calculated product",
        m_one_one_multiplied_by_t0,
        "precomputed product",
        "%f",
        0.0f);

    PrintSubsectionHeader("Checking commuted product");
    FloatVector2 product_with_t1(m_transform1 * one_one);
    TestVectorEquality(
        product_with_t1,
        "calculated product",
        m_one_one_multiplied_by_t1,
        "precomputed product",
        "%f",
        0.0f);
}

void SimpleTransform2UnitTest::AtomInverse ()
{
    {
        PrintSubsectionHeader("Checking Inverse()");

        FloatSimpleTransform2 transform0_inverse(m_transform0.Inverse());
        TestSimpleTransform2Equality(
            transform0_inverse,
            "calculated t0 inverse",
            m_transform0_inverse,
            "precomputed t0 inverse",
            "%f",
            0.0f);

        PrintSubsectionHeader("Checking additional inverse");
        FloatSimpleTransform2 transform1_inverse(m_transform1.Inverse());
        TestSimpleTransform2Equality(
            transform1_inverse,
            "calculated t0 inverse",
            m_transform1_inverse,
            "precomputed t0 inverse",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader("Checking Invert()");

        FloatSimpleTransform2 transform0_inverse(m_transform0);
        transform0_inverse.Invert();
        TestSimpleTransform2Equality(
            transform0_inverse,
            "calculated t0 inverse",
            m_transform0_inverse,
            "precomputed t0 inverse",
            "%f",
            0.0f);

        PrintSubsectionHeader("Checking additional inverse");
        FloatSimpleTransform2 transform1_inverse(m_transform1);
        transform1_inverse.Invert();
        TestSimpleTransform2Equality(
            transform1_inverse,
            "calculated t0 inverse",
            m_transform1_inverse,
            "precomputed t0 inverse",
            "%f",
            0.0f);
    }
}

