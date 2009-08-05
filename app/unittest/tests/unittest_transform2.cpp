// ///////////////////////////////////////////////////////////////////////////
// transform2unittest.cpp by Victor Dods, created 2005/05/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "transform2unittest.hpp"

Transform2UnitTest::Transform2UnitTest ()
    :
    UnitTest("Transform2")
{
    RegisterAtom(&Transform2UnitTest::AtomConstructors);
    RegisterAtom(&Transform2UnitTest::AtomAssignmentOperator);
    RegisterAtom(&Transform2UnitTest::AtomModifiers);
}

Transform2UnitTest::~Transform2UnitTest ()
{
}

void Transform2UnitTest::Initialize ()
{
}

void Transform2UnitTest::AtomConstructors ()
{
    FloatMatrix2 scaling_matrix(FloatMatrix2::ms_identity);
    FloatMatrix2 rotation_matrix(FloatMatrix2::ms_identity);
    FloatMatrix2 translation_matrix(FloatMatrix2::ms_identity);

    FloatVector2 scale_factors(FloatVector2(3.75f, 1.5f));
    Float angle = 69.0f;
    FloatVector2 translation(FloatVector2(7.0f, -8.0f));

    scaling_matrix.Scale(scale_factors);
    rotation_matrix.Rotate(angle);
    translation_matrix.Translate(translation);

    FloatMatrix2 post_translate_matrix;
    post_translate_matrix = scaling_matrix;
    post_translate_matrix *= rotation_matrix;
    post_translate_matrix *= translation_matrix;

    FloatMatrix2 pre_translate_matrix;
    pre_translate_matrix = translation_matrix;
    pre_translate_matrix *= scaling_matrix;
    pre_translate_matrix *= rotation_matrix;

    {
        PrintSubsectionHeader("Checking that defaultest constructor uses identity");
        FloatTransform2 constructed(true);
        TestVectorEquality(
            constructed.GetTranslation(),
            "constructed transform translation",
            FloatTransform2::ms_identity.GetTranslation(),
            "identity translation",
            "%f",
            0.0f);
        TestVectorEquality(
            constructed.GetScaleFactors(),
            "constructed transform scale factors",
            FloatTransform2::ms_identity.GetScaleFactors(),
            "identity scale factors",
            "%f",
            0.0f);
        TestEquality(
            constructed.Angle(),
            "constructed transform angle",
            FloatTransform2::ms_identity.Angle(),
            "identity angle",
            "%f",
            0.0f);

        PrintSubsectionHeader("Checking resulting transformation matrix");
        TestMatrix2Equality(
            constructed.GetTransform(),
            "constructed.GetTransform()",
            FloatMatrix2::ms_identity,
            "identity matrix",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader(
            "Checking per-property constructor "
            "for post-translate transform");
        FloatTransform2 constructed(translation, scale_factors, angle, true);
        TestVectorEquality(
            constructed.GetTranslation(),
            "constructed transform translation",
            translation,
            "precomputed translation",
            "%f",
            0.0f);
        TestVectorEquality(
            constructed.GetScaleFactors(),
            "constructed transform scale factors",
            scale_factors,
            "precomputed scale factors",
            "%f",
            0.0f);
        TestEquality(
            constructed.Angle(),
            "constructed transform angle",
            angle,
            "precomputed angle",
            "%f",
            0.0f);
        Test(constructed.GetPostTranslate() == true,
             "constructed.GetPostTranslate() is %s and should be %s",
             BOOL_TO_STRING(constructed.GetPostTranslate()),
             BOOL_TO_STRING(true));

        PrintSubsectionHeader("Checking resulting transformation matrix");
        TestMatrix2Equality(
            constructed.GetTransform(),
            "constructed.GetTransform()",
            post_translate_matrix,
            "precomputed matrix",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader(
            "Checking per-property constructor "
            "for pre-translate transform");
        FloatTransform2 constructed(translation, scale_factors, angle, false);
        TestVectorEquality(
            constructed.GetTranslation(),
            "constructed transform translation",
            translation,
            "precomputed translation",
            "%f",
            0.0f);
        TestVectorEquality(
            constructed.GetScaleFactors(),
            "constructed transform scale factors",
            scale_factors,
            "precomputed scale factors",
            "%f",
            0.0f);
        TestEquality(
            constructed.Angle(),
            "constructed transform angle",
            angle,
            "precomputed angle",
            "%f",
            0.0f);
        Test(constructed.GetPostTranslate() == false,
             "constructed.GetPostTranslate() is %s and should be %s",
             BOOL_TO_STRING(constructed.GetPostTranslate()),
             BOOL_TO_STRING(false));

        PrintSubsectionHeader("Checking resulting transformation matrix");
        TestMatrix2Equality(
            constructed.GetTransform(),
            "constructed.GetTransform()",
            pre_translate_matrix,
            "precomputed matrix",
            "%f",
            0.0f);
    }
}

void Transform2UnitTest::AtomAssignmentOperator ()
{
    FloatTransform2 transform(true);
    transform = FloatTransform2::ms_identity;
    TestTransform2Equality(
        transform,
        "assigned transform",
        FloatTransform2::ms_identity,
        "source transform",
        "%f",
        0.0f);
}

void Transform2UnitTest::AtomModifiers ()
{
    FloatMatrix2 scaling_matrix(FloatMatrix2::ms_identity);
    FloatMatrix2 rotation_matrix(FloatMatrix2::ms_identity);
    FloatMatrix2 translation_matrix(FloatMatrix2::ms_identity);

    FloatVector2 scale_factors(FloatVector2(3.75f, 1.5f));
    Float angle = 69.0f;
    FloatVector2 translation(FloatVector2(7.0f, -8.0f));

    scaling_matrix.Scale(scale_factors);
    rotation_matrix.Rotate(angle);
    translation_matrix.Translate(translation);

    FloatMatrix2 post_translate_matrix;
    post_translate_matrix = scaling_matrix;
    post_translate_matrix *= rotation_matrix;
    post_translate_matrix *= translation_matrix;

    FloatMatrix2 pre_translate_matrix;
    pre_translate_matrix = translation_matrix;
    pre_translate_matrix *= scaling_matrix;
    pre_translate_matrix *= rotation_matrix;

    {
        PrintSubsectionHeader("Checking Translate() for post-translate transform");

        FloatVector2 offset(6.82f, 102.1f);
        FloatTransform2 transform(
            translation,
            scale_factors,
            angle,
            true);
        transform.Translate(offset);
        FloatMatrix2 precomputed(FloatMatrix2::ms_identity);
        precomputed *= scaling_matrix;
        precomputed *= rotation_matrix;
        precomputed *= translation_matrix;
        precomputed.Translate(offset);
        TestMatrix2Equality(
            transform.GetTransform(),
            "calculated matrix",
            precomputed,
            "precomputed matrix",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader(
            "Checking (non-uniform) Scale() for post-translate transform");

        FloatVector2 scale_factors2(6.82f, 1.1f);
        FloatTransform2 transform(
            translation,
            scale_factors,
            angle,
            true);
        transform.Scale(scale_factors2);
        FloatMatrix2 precomputed(FloatMatrix2::ms_identity);
        precomputed.Scale(scale_factors2);
        precomputed *= scaling_matrix;
        precomputed *= rotation_matrix;
        precomputed *= translation_matrix;
        TestMatrix2Equality(
            transform.GetTransform(),
            "calculated matrix",
            precomputed,
            "precomputed matrix",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader(
            "Checking (uniform) Scale() for post-translate transform");

        Float scale_factor = 1.2345f;
        FloatTransform2 transform(
            translation,
            scale_factors,
            angle,
            true);
        transform.Scale(scale_factor);
        FloatMatrix2 precomputed(FloatMatrix2::ms_identity);
        precomputed.Scale(scale_factor);
        precomputed *= scaling_matrix;
        precomputed *= rotation_matrix;
        precomputed *= translation_matrix;
        TestMatrix2Equality(
            transform.GetTransform(),
            "calculated matrix",
            precomputed,
            "precomputed matrix",
            "%f",
            1e-6f);
    }

    {
        PrintSubsectionHeader(
            "Checking Rotate() for post-translate transform");

        Float angle2 = 35.23f;
        FloatTransform2 transform(
            translation,
            scale_factors,
            angle,
            true);
        transform.Rotate(angle2);
        FloatMatrix2 precomputed(FloatMatrix2::ms_identity);
        precomputed *= scaling_matrix;
        precomputed.Rotate(angle2);
        precomputed *= rotation_matrix;
        precomputed *= translation_matrix;
        TestMatrix2Equality(
            transform.GetTransform(),
            "calculated matrix",
            precomputed,
            "precomputed matrix",
            "%f",
            1e-6f);
    }

    {
        PrintSubsectionHeader("Checking Translate() for pre-translate transform");

        FloatVector2 offset(6.82f, 102.1f);
        FloatTransform2 transform(
            translation,
            scale_factors,
            angle,
            true);
        transform.Translate(offset);
        FloatMatrix2 precomputed(FloatMatrix2::ms_identity);
        precomputed.Translate(offset);
        precomputed *= scaling_matrix;
        precomputed *= rotation_matrix;
        precomputed *= translation_matrix;
        TestMatrix2Equality(
            transform.GetTransform(),
            "calculated matrix",
            precomputed,
            "precomputed matrix",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader(
            "Checking (non-uniform) Scale() for pre-translate transform");

        FloatVector2 scale_factors2(6.82f, 1.1f);
        FloatTransform2 transform(
            translation,
            scale_factors,
            angle,
            true);
        transform.Scale(scale_factors2);
        FloatMatrix2 precomputed(FloatMatrix2::ms_identity);
        precomputed.Scale(scale_factors2);
        precomputed *= scaling_matrix;
        precomputed *= rotation_matrix;
        precomputed *= translation_matrix;
        TestMatrix2Equality(
            transform.GetTransform(),
            "calculated matrix",
            precomputed,
            "precomputed matrix",
            "%f",
            0.0f);
    }

    {
        PrintSubsectionHeader(
            "Checking (uniform) Scale() for pre-translate transform");

        Float scale_factor = 1.2345f;
        FloatTransform2 transform(
            translation,
            scale_factors,
            angle,
            true);
        transform.Scale(scale_factor);
        FloatMatrix2 precomputed(FloatMatrix2::ms_identity);
        precomputed.Scale(scale_factor);
        precomputed *= scaling_matrix;
        precomputed *= rotation_matrix;
        precomputed *= translation_matrix;
        TestMatrix2Equality(
            transform.GetTransform(),
            "calculated matrix",
            precomputed,
            "precomputed matrix",
            "%f",
            1e-6f);
    }

    {
        PrintSubsectionHeader(
            "Checking Rotate() for pre-translate transform");

        Float angle2 = 35.23f;
        FloatTransform2 transform(
            translation,
            scale_factors,
            angle,
            true);
        transform.Rotate(angle2);
        FloatMatrix2 precomputed(FloatMatrix2::ms_identity);
        precomputed *= scaling_matrix;
        precomputed.Rotate(angle2);
        precomputed *= rotation_matrix;
        precomputed *= translation_matrix;
        TestMatrix2Equality(
            transform.GetTransform(),
            "calculated matrix",
            precomputed,
            "precomputed matrix",
            "%f",
            1e-6f);
    }
}

