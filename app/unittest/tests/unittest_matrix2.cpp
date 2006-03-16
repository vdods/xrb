// ///////////////////////////////////////////////////////////////////////////
// matrix2unittest.cpp by Victor Dods, created 2005/05/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "matrix2unittest.h"

#include "../util.h"

Matrix2UnitTest::Matrix2UnitTest ()
    :
    UnitTest("Matrix2"),
    m_matrix0(
         4.0f, -2.5f, 6.0f,
        3.75f, 0.75f, 1.0f),
    m_matrix0_determinant(12.375f),
    m_matrix0_inverse(
         0.75f/12.375f, 2.5f/12.375f, -7.0f/12.375f,
        -3.75f/12.375f, 4.0f/12.375f, 18.5f/12.375f),
    m_matrix1(
        1.25f, 0.0f, 5.0f,
        8.25f, 6.5f, 6.0f),
    m_matrix1_determinant(8.125),
    m_matrix1_inverse(
          6.5f/8.125f,  0.0f/8.125f, -32.5f/8.125f,
        -8.25f/8.125f, 1.25f/8.125f, 33.75f/8.125f),
    m_product_of_m0_and_m1(
        -15.625f, -16.25f,  11.0f,
         10.875f,  4.875f, 24.25f),
    m_product_of_m1_and_m0(
           5.0f, -3.125f, 12.5f,
        57.375f, -15.75f, 62.0f),
    m_simpletransform0(10.5f, -8.0f, -4.0f, 1.0f),
    m_product_of_m0_and_st0(
          42.0f, 20.0f,  -12.5f,
        39.375f, -6.0f, -13.25f),
    m_product_of_st0_and_m0(
         42.0f, -26.25f, 59.0f,
        -30.0f,   -6.0f, -7.0f),
    m_angle(60.0f),
    m_matrix_rotated(
        FLOAT_COS(RADIANS(m_angle)), -FLOAT_SIN(RADIANS(m_angle)), 0.0f,
        FLOAT_SIN(RADIANS(m_angle)),  FLOAT_COS(RADIANS(m_angle)), 0.0f),
    m_vector_rotated(
        FLOAT_COS(RADIANS(m_angle)),
        FLOAT_SIN(RADIANS(m_angle))),
    m_matrix_scaled_nonuniformly(
        2.5f,   0.0f, 0.0f,
        0.0f, -3.25f, 0.0f),
    m_scale_factors(2.5f, -3.25f),
    m_matrix_scaled_uniformly(
        5.5f, 0.0f, 0.0f,
        0.0f, 5.5f, 0.0f),
    m_scale_factor(5.5f),
    m_matrix_translated(
        1.0f, 0.0f, 82.25f,
        0.0f, 1.0f, -9.75f),
    m_translation(82.25f, -9.75f)
{
    RegisterAtom(&Matrix2UnitTest::AtomStaticIdentity);
    RegisterAtom(&Matrix2UnitTest::AtomConstructByComponents);
    RegisterAtom(&Matrix2UnitTest::AtomConstructByComponentArray);
    RegisterAtom(&Matrix2UnitTest::AtomConstructByMatrix2);
    RegisterAtom(&Matrix2UnitTest::AtomConstructBySimpleTransform2);
    RegisterAtom(&Matrix2UnitTest::AtomClassOperatorBracket);
    RegisterAtom(&Matrix2UnitTest::AtomClassOperatorInplaceMultiplyMatrix2);
    RegisterAtom(&Matrix2UnitTest::AtomClassOperatorInplaceMultiplySimpleTransform2);
    RegisterAtom(&Matrix2UnitTest::AtomClassOperatorAssignmentMatrix2);
    RegisterAtom(&Matrix2UnitTest::AtomClassOperatorAssignmentSimpleTransform2);
    RegisterAtom(&Matrix2UnitTest::AtomDeterminant);
    RegisterAtom(&Matrix2UnitTest::AtomInverse);
    RegisterAtom(&Matrix2UnitTest::AtomRotate);
    RegisterAtom(&Matrix2UnitTest::AtomScale);
    RegisterAtom(&Matrix2UnitTest::AtomTranslate);
    RegisterAtom(&Matrix2UnitTest::AtomGlobalOperators);
    RegisterAtom(&Matrix2UnitTest::AtomLinearity);
}

Matrix2UnitTest::~Matrix2UnitTest ()
{
}

void Matrix2UnitTest::Initialize ()
{
}

void Matrix2UnitTest::AtomStaticIdentity ()
{
    PrintSubsectionHeader("Checking identity matrix");

    FloatMatrix2 precomputed_identity;
    precomputed_identity.m[FloatMatrix2::A] = 1.0f;
    precomputed_identity.m[FloatMatrix2::B] = 0.0f;
    precomputed_identity.m[FloatMatrix2::X] = 0.0f;
    precomputed_identity.m[FloatMatrix2::C] = 0.0f;
    precomputed_identity.m[FloatMatrix2::D] = 1.0f;
    precomputed_identity.m[FloatMatrix2::Y] = 0.0f;
    TestMatrix2Equality(
        FloatMatrix2::ms_identity,
        "static identity",
        precomputed_identity,
        "precomputed identity",
        "%f",
        0.0f);
}

void Matrix2UnitTest::AtomConstructByComponents ()
{
    PrintSubsectionHeader("Checking matrix construction by components");

    FloatMatrix2 constructed_matrix(
        1.0f, 2.0f, 3.0f,
        4.0f, 5.0f, 6.0f);
    FloatMatrix2 precomputed_matrix;
    precomputed_matrix.m[FloatMatrix2::A] = 1.0f;
    precomputed_matrix.m[FloatMatrix2::B] = 2.0f;
    precomputed_matrix.m[FloatMatrix2::X] = 3.0f;
    precomputed_matrix.m[FloatMatrix2::C] = 4.0f;
    precomputed_matrix.m[FloatMatrix2::D] = 5.0f;
    precomputed_matrix.m[FloatMatrix2::Y] = 6.0f;

    TestMatrix2Equality(
        constructed_matrix,
        "component-constructed matrix",
        precomputed_matrix,
        "precomputed matrix",
        "%f",
        0.0f);
}

void Matrix2UnitTest::AtomConstructByComponentArray ()
{
    PrintSubsectionHeader("Checking matrix construction by array of components");

    Float elements[FloatMatrix2::COMPONENT_COUNT];
    elements[FloatMatrix2::A] = 1.0f;
    elements[FloatMatrix2::B] = 2.0f;
    elements[FloatMatrix2::X] = 3.0f;
    elements[FloatMatrix2::C] = 4.0f;
    elements[FloatMatrix2::D] = 5.0f;
    elements[FloatMatrix2::Y] = 6.0f;

    FloatMatrix2 constructed_matrix(elements);
    FloatMatrix2 precomputed_matrix;
    precomputed_matrix.m[FloatMatrix2::A] = 1.0f;
    precomputed_matrix.m[FloatMatrix2::B] = 2.0f;
    precomputed_matrix.m[FloatMatrix2::X] = 3.0f;
    precomputed_matrix.m[FloatMatrix2::C] = 4.0f;
    precomputed_matrix.m[FloatMatrix2::D] = 5.0f;
    precomputed_matrix.m[FloatMatrix2::Y] = 6.0f;

    TestMatrix2Equality(
        constructed_matrix,
        "array-constructed matrix",
        precomputed_matrix,
        "precomputed matrix",
        "%f",
        0.0f);
}

void Matrix2UnitTest::AtomConstructByMatrix2 ()
{
    PrintSubsectionHeader("Checking copy constructor");

    FloatMatrix2 constructed_matrix(FloatMatrix2::ms_identity);
    TestMatrix2Equality(
        constructed_matrix,
        "copy-constructed matrix",
        FloatMatrix2::ms_identity,
        "static identity",
        "%f",
        0.0f);
}

void Matrix2UnitTest::AtomConstructBySimpleTransform2 ()
{
    PrintSubsectionHeader("Checking construction by SimpleTransform2");

    FloatMatrix2 constructed_matrix(m_simpletransform0);
    FloatMatrix2 precomputed_matrix;
    precomputed_matrix.m[FloatMatrix2::A] =
        m_simpletransform0.m_scale_factors[Dim::X];
    precomputed_matrix.m[FloatMatrix2::B] = 0.0f;
    precomputed_matrix.m[FloatMatrix2::X] =
        m_simpletransform0.m_translation[Dim::X];
    precomputed_matrix.m[FloatMatrix2::C] = 0.0f;
    precomputed_matrix.m[FloatMatrix2::D] =
        m_simpletransform0.m_scale_factors[Dim::Y];
    precomputed_matrix.m[FloatMatrix2::Y] =
        m_simpletransform0.m_translation[Dim::Y];

    TestMatrix2Equality(
        constructed_matrix,
        "simpletransform-constructed matrix",
        precomputed_matrix,
        "precomputed matrix",
        "%f",
        0.0f);
}

void Matrix2UnitTest::AtomClassOperatorBracket ()
{
    PrintSubsectionHeader("Checking operator[]");

    FloatMatrix2 test_matrix(
         82.3f, 125.365f, 1985.01f,
        359.2f,   195.2f, 295.915f);

    Test(test_matrix.m[FloatMatrix2::A] == test_matrix[FloatMatrix2::A],
         "checked that test_matrix.m[A] (%f) == test_matrix[A] (%f).",
         test_matrix.m[FloatMatrix2::A],
         test_matrix[FloatMatrix2::A]);
    Test(test_matrix.m[FloatMatrix2::B] == test_matrix[FloatMatrix2::B],
         "checked that test_matrix.m[B] (%f) == test_matrix[B] (%f).",
         test_matrix.m[FloatMatrix2::B],
         test_matrix[FloatMatrix2::B]);
    Test(test_matrix.m[FloatMatrix2::X] == test_matrix[FloatMatrix2::X],
         "checked that test_matrix.m[X] (%f) == test_matrix[X] (%f).",
         test_matrix.m[FloatMatrix2::X],
         test_matrix[FloatMatrix2::X]);
    Test(test_matrix.m[FloatMatrix2::C] == test_matrix[FloatMatrix2::C],
         "checked that test_matrix.m[C] (%f) == test_matrix[C] (%f).",
         test_matrix.m[FloatMatrix2::C],
         test_matrix[FloatMatrix2::C]);
    Test(test_matrix.m[FloatMatrix2::D] == test_matrix[FloatMatrix2::D],
         "checked that test_matrix.m[D] (%f) == test_matrix[D] (%f).",
         test_matrix.m[FloatMatrix2::D],
         test_matrix[FloatMatrix2::D]);
    Test(test_matrix.m[FloatMatrix2::Y] == test_matrix[FloatMatrix2::Y],
         "checked that test_matrix.m[Y] (%f) == test_matrix[Y] (%f).",
         test_matrix.m[FloatMatrix2::Y],
         test_matrix[FloatMatrix2::Y]);

    PrintSubsectionHeader("Testing assignment to non-const operator[] references");

    FloatMatrix2 another_test_matrix(test_matrix);
    another_test_matrix[FloatMatrix2::A] += 1.0f;
    TestInequality(
        another_test_matrix.m[FloatMatrix2::A],
        "changed value of component A",
        test_matrix.m[FloatMatrix2::A],
        "component A source value",
        "%f");
}

void Matrix2UnitTest::AtomClassOperatorInplaceMultiplyMatrix2 ()
{
    PrintSubsectionHeader("Checking in-place multiply by Matrix2");

    FloatMatrix2 product_of_m0_and_m1(m_matrix1);
    product_of_m0_and_m1 *= m_matrix0;
    TestMatrix2Equality(
        product_of_m0_and_m1,
        "m0*m1",
        m_product_of_m0_and_m1,
        "precomputed product",
        "%f",
        0.0f);

    PrintSubsectionHeader("Checking commuted product");

    FloatMatrix2 product_of_m1_and_m0(m_matrix0);
    product_of_m1_and_m0 *= m_matrix1;
    TestMatrix2Equality(
        product_of_m1_and_m0,
        "m1*m0",
        m_product_of_m1_and_m0,
        "precomputed product",
        "%f",
        0.0f);
}

void Matrix2UnitTest::AtomClassOperatorInplaceMultiplySimpleTransform2 ()
{
    PrintSubsectionHeader("Checking in-place multiply by SimpleTransform2");

    FloatMatrix2 product_of_st0_and_m0(m_matrix0);
    product_of_st0_and_m0 *= m_simpletransform0;
    TestMatrix2Equality(
        product_of_st0_and_m0,
        "st0*m0",
        m_product_of_st0_and_m0,
        "precomputed product",
        "%f",
        0.0f);

    PrintSubsectionHeader("Checking commuted product via global operator *");

    FloatMatrix2 product_of_m0_and_st0(m_matrix0 * m_simpletransform0);
    TestMatrix2Equality(
        product_of_m0_and_st0,
        "m0*st0",
        m_product_of_m0_and_st0,
        "precomputed product",
        "%f",
        0.0f);
}

void Matrix2UnitTest::AtomClassOperatorAssignmentMatrix2 ()
{
    PrintSubsectionHeader("Checking assignment operator");

    FloatMatrix2 assigned_matrix;
    assigned_matrix = m_matrix0;
    TestMatrix2Equality(
        assigned_matrix,
        "assigned matrix",
        m_matrix0,
        "source matrix",
        "%f",
        0.0f);

    PrintSubsectionHeader("Checking additional assignment");

    assigned_matrix = m_matrix1;
    TestMatrix2Equality(
        assigned_matrix,
        "assigned matrix",
        m_matrix1,
        "source matrix",
        "%f",
        0.0f);
}

void Matrix2UnitTest::AtomClassOperatorAssignmentSimpleTransform2 ()
{
    PrintSubsectionHeader(
        "Checking assignment operator "
        "(assignment from SimpleTransform2");

    FloatMatrix2 assigned_matrix;
    assigned_matrix = m_simpletransform0;
    FloatMatrix2 precomputed_matrix;
    precomputed_matrix.m[FloatMatrix2::A] =
        m_simpletransform0.m_scale_factors[Dim::X];
    precomputed_matrix.m[FloatMatrix2::B] = 0.0f;
    precomputed_matrix.m[FloatMatrix2::X] =
        m_simpletransform0.m_translation[Dim::X];
    precomputed_matrix.m[FloatMatrix2::C] = 0.0f;
    precomputed_matrix.m[FloatMatrix2::D] =
        m_simpletransform0.m_scale_factors[Dim::Y];
    precomputed_matrix.m[FloatMatrix2::Y] =
        m_simpletransform0.m_translation[Dim::Y];
    TestMatrix2Equality(
        assigned_matrix,
        "assigned matrix",
        precomputed_matrix,
        "source simpletransform",
        "%f",
        0.0f);
}

void Matrix2UnitTest::AtomDeterminant ()
{
    PrintSubsectionHeader("Checking GetDeterminant()");

    TestEquality(
        FloatMatrix2::ms_identity.GetDeterminant(),
        "determinant of the identity",
        1.0f,
        "one",
        "%f");
    TestEquality(
        m_matrix0.GetDeterminant(),
        "calculated determinant of m0",
        m_matrix0_determinant,
        "precomputed determinant",
        "%f");
    TestEquality(
        m_matrix1.GetDeterminant(),
        "calculated determinant of m1",
        m_matrix1_determinant,
        "precomputed determinant",
        "%f");
}

void Matrix2UnitTest::AtomInverse ()
{
    PrintSubsectionHeader("Checking GetInverse()");
    {
        FloatMatrix2 inverted_identity(FloatMatrix2::ms_identity.GetInverse());
        TestMatrix2Equality(
            inverted_identity,
            "inverted identity",
            FloatMatrix2::ms_identity,
            "static identity",
            "%f",
            0.0f);

        PrintSubsectionHeader("Checking additional GetInverse()");
        FloatMatrix2 inverted_matrix0(m_matrix0.GetInverse());
        TestMatrix2Equality(
            inverted_matrix0,
            "inverted m0",
            m_matrix0_inverse,
            "precomputed inverse",
            "%f",
            0.0f);

        PrintSubsectionHeader("Checking additional GetInverse()");
        FloatMatrix2 inverted_matrix1(m_matrix1.GetInverse());
        TestMatrix2Equality(
            inverted_matrix1,
            "inverted m1",
            m_matrix1_inverse,
            "precomputed inverse",
            "%f",
            0.0f);
    }

    PrintSubsectionHeader("Checking Invert()");
    {
        FloatMatrix2 inverted_identity(FloatMatrix2::ms_identity);
        TestMatrix2Equality(
            inverted_identity,
            "inverted identity",
            FloatMatrix2::ms_identity,
            "static identity",
            "%f",
            0.0f);

        PrintSubsectionHeader("Checking additional Invert()");
        FloatMatrix2 inverted_matrix0(m_matrix0);
        inverted_matrix0.Invert();
        TestMatrix2Equality(
            inverted_matrix0,
            "inverted m0",
            m_matrix0_inverse,
            "precomputed inverse",
            "%f",
            0.0f);

        PrintSubsectionHeader("Checking additional Invert()");
        FloatMatrix2 inverted_matrix1(m_matrix1);
        inverted_matrix1.Invert();
        TestMatrix2Equality(
            inverted_matrix1,
            "inverted m1",
            m_matrix1_inverse,
            "precomputed inverse",
            "%f",
            0.0f);
    }
}

void Matrix2UnitTest::AtomRotate ()
{
    PrintSubsectionHeader("Checking Rotate()");

    FloatMatrix2 rotated_matrix(FloatMatrix2::ms_identity);
    rotated_matrix.Rotate(60.0f);
    TestMatrix2Equality(
        rotated_matrix,
        "calculated rotated matrix",
        m_matrix_rotated,
        "precomputed rotated matrix",
        "%f",
        0.0f);
}

void Matrix2UnitTest::AtomScale ()
{
    {
        PrintSubsectionHeader("Checking nonuniform Scale()");
        FloatMatrix2 scaled_matrix(FloatMatrix2::ms_identity);
        scaled_matrix.Scale(m_scale_factors);
        TestMatrix2Equality(
            scaled_matrix,
            "calculated scaled matrix",
            m_matrix_scaled_nonuniformly,
            "precomputed scaled matrix",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader("Checking uniform Scale()");
        FloatMatrix2 scaled_matrix(FloatMatrix2::ms_identity);
        scaled_matrix.Scale(m_scale_factor);
        TestMatrix2Equality(
            scaled_matrix,
            "calculated scaled matrix",
            m_matrix_scaled_uniformly,
            "precomputed scaled matrix",
            "%f",
            0.0f);
    }
}

void Matrix2UnitTest::AtomTranslate ()
{
    PrintSubsectionHeader("Checking Translate()");

    FloatMatrix2 translated_matrix(FloatMatrix2::ms_identity);
    translated_matrix.Translate(m_translation);
    TestMatrix2Equality(
        translated_matrix,
        "calculated translated matrix",
        m_matrix_translated,
        "precomputed translated matrix",
        "%f",
        0.0f);
}

void Matrix2UnitTest::AtomGlobalOperators ()
{
    {
        PrintSubsectionHeader(
            "Checking multiplication of a vector by a rotated matrix");
        FloatVector2 source_vector(1.0f, 0.0f);
        FloatVector2 rotated_vector(source_vector);
        rotated_vector *= m_matrix_rotated;
        TestVectorEquality(
            rotated_vector,
            "calculated rotated vector",
            m_vector_rotated,
            "precomputed rotated vector",
            "%f",
            0.0f);
        TestVectorEquality(
            m_matrix_rotated * source_vector,
            "rotation matrix * vector",
            rotated_vector,
            "vector *= rotation matrix",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader(
            "Checking multiplication of a vector "
            "by a nonuniformly scaled matrix");
        FloatVector2 source_vector(1.0f, 1.0f);
        FloatVector2 scaled_vector(source_vector);
        scaled_vector *= m_matrix_scaled_nonuniformly;
        TestVectorEquality(
            scaled_vector,
            "calculated scaled vector",
            m_scale_factors,
            "precomputed scaled vector",
            "%f",
            0.0f);
        TestVectorEquality(
            m_matrix_scaled_nonuniformly * source_vector,
            "scaling matrix * vector",
            scaled_vector,
            "vector *= scaling matrix",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader(
            "Checking multiplication of a vector "
            "by a uniformly scaled matrix");
        FloatVector2 source_vector(1.0f, 1.0f);
        FloatVector2 scaled_vector(source_vector);
        scaled_vector *= m_matrix_scaled_uniformly;
        FloatVector2 precomputed_scaled_vector(m_scale_factor, m_scale_factor);
        TestVectorEquality(
            scaled_vector,
            "calculated scaled vector",
            precomputed_scaled_vector,
            "precomputed scaled vector",
            "%f",
            0.0f);
        TestVectorEquality(
            m_matrix_scaled_uniformly * source_vector,
            "scaling matrix * vector",
            scaled_vector,
            "vector *= scaling matrix",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader(
            "Checking multiplication of a "
            "vector by a translated matrix");

        FloatVector2 source_vector(FloatVector2::ms_zero);
        FloatVector2 translated_vector(source_vector);
        translated_vector *= m_matrix_translated;
        TestVectorEquality(
            translated_vector,
            "calculated translated vector",
            m_translation,
            "precomputed translated vector",
            "%f",
            0.0f);
        TestVectorEquality(
            m_matrix_translated * source_vector,
            "translation matrix * vector",
            translated_vector,
            "vector *= translation matrix",
            "%f",
            0.0f);
    }
}

void Matrix2UnitTest::AtomLinearity ()
{
    FloatVector2 a(3.5f, 8.25f);
    FloatVector2 b(-4.75f, 0.0f);
    FloatVector2 a_plus_b(a + b);

    {
        PrintSubsectionHeader("Checking linearity for rotation");
        FloatVector2 transformed_zero(FloatVector2::ms_zero);
        transformed_zero *= m_matrix_rotated;
        TestVectorEquality(
            FloatVector2::ms_zero,
            "the zero vector",
            transformed_zero,
            "transformed zero vector",
            "%f",
            0.0f);

        FloatVector2 transformed_a(m_matrix_rotated * a);
        FloatVector2 transformed_b(m_matrix_rotated * b);
        FloatVector2 transformed_a_plus_b(m_matrix_rotated * a_plus_b);
        TestVectorEquality(
            transformed_a + transformed_b,
            "Rotate(a) + Rotate(b)",
            transformed_a_plus_b,
            "Rotate(a + b)",
            "%f",
            1e-6f);
    }

    {
        PrintSubsectionHeader("Checking linearity for nonuniform scaling");
        FloatVector2 transformed_zero(FloatVector2::ms_zero);
        transformed_zero *= m_matrix_scaled_nonuniformly;
        TestVectorEquality(
            FloatVector2::ms_zero,
            "the zero vector",
            transformed_zero,
            "transformed zero vector",
            "%f",
            0.0f);
        FloatVector2 transformed_a(m_matrix_scaled_nonuniformly * a);
        FloatVector2 transformed_b(m_matrix_scaled_nonuniformly * b);
        FloatVector2 transformed_a_plus_b(m_matrix_scaled_nonuniformly * a_plus_b);
        TestVectorEquality(
            transformed_a + transformed_b,
            "Rotate(a) + Rotate(b)",
            transformed_a_plus_b,
            "Rotate(a + b)",
            "%f",
            1e-6f);
    }

    {
        PrintSubsectionHeader("Checking linearity for uniform scaling");
        FloatVector2 transformed_zero(FloatVector2::ms_zero);
        transformed_zero *= m_matrix_scaled_uniformly;
        TestVectorEquality(
            FloatVector2::ms_zero,
            "the zero vector",
            transformed_zero,
            "transformed zero vector",
            "%f",
            0.0f);
        FloatVector2 transformed_a(m_matrix_scaled_uniformly * a);
        FloatVector2 transformed_b(m_matrix_scaled_uniformly * b);
        FloatVector2 transformed_a_plus_b(m_matrix_scaled_uniformly * a_plus_b);
        TestVectorEquality(
            transformed_a + transformed_b,
            "Rotate(a) + Rotate(b)",
            transformed_a_plus_b,
            "Rotate(a + b)",
            "%f",
            1e-6f);
    }
}

