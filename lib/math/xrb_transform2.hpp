// ///////////////////////////////////////////////////////////////////////////
// xrb_transform2.hpp by Victor Dods, created 2005/05/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_TRANSFORM2_HPP_)
#define _XRB_TRANSFORM2_HPP_

#include "xrb.hpp"

#include "xrb_math.hpp"
#include "xrb_matrix2.hpp"
#include "xrb_serializer.hpp"
#include "xrb_simpletransform2.hpp"
#include "xrb_vector.hpp"

namespace Xrb
{

template <typename T>
class Transform2
{
public:

    static Transform2<T> const ms_identity;

    // ///////////////////////////////////////////////////////////////////////
    // constructors and destructor
    // ///////////////////////////////////////////////////////////////////////

    Transform2 (bool post_translate)
    {
        m_post_translate = post_translate;
        Dirtify();
    }
    Transform2 (
        Vector<T, 2> const &translation,
        Vector<T, 2> const &scale_factors,
        T angle,
        bool post_translate)
    {
        m_translation = translation;
        m_scale_factors = scale_factors;
        m_angle = angle;
        m_post_translate = post_translate;
        Dirtify();
    }
    Transform2 (
        SimpleTransform2<T> const &source,
        bool post_translate)
    {
        m_translation = source.m_translation;
        m_scale_factors = source.m_scale_factors;
        m_angle = static_cast<T>(0);
        m_post_translate = post_translate;
        Dirtify();
    }
    Transform2 (Transform2<T> const &source)
    {
        m_translation = source.m_translation;
        m_scale_factors = source.m_scale_factors;
        m_angle = source.m_angle;
        m_post_translate = source.m_post_translate;
        Dirtify();
    }
    Transform2 (
        Transform2<T> const &source,
        bool post_translate)
    {
        m_translation = source.m_translation;
        m_scale_factors = source.m_scale_factors;
        m_angle = source.m_angle;
        m_post_translate = post_translate;
        Dirtify();
    }
    ~Transform2 () { }

    // ///////////////////////////////////////////////////////////////////////
    // overloaded operators
    // ///////////////////////////////////////////////////////////////////////

    void operator = (Transform2<T> const &source)
    {
        m_translation = source.m_translation;
        m_scale_factors = source.m_scale_factors;
        m_angle = source.m_angle;
        // NOTE: does not set m_post_translate!
        Dirtify();
    }

    // ///////////////////////////////////////////////////////////////////////
    // accessors
    // ///////////////////////////////////////////////////////////////////////

    Vector<T, 2> const &Translation () const
    {
        return m_translation;
    }
    Vector<T, 2> const &ScaleFactors () const
    {
        return m_scale_factors;
    }
    T ScaleFactor () const
    {
        ASSERT1(m_scale_factors[Dim::X] == m_scale_factors[Dim::Y]);
        return m_scale_factors[Dim::X];
    }
    T Angle () const
    {
        return m_angle;
    }
    bool PostTranslate () const
    {
        return m_post_translate;
    }

    Matrix2<T> const &Transformation () const
    {
        RecalculateTransformIfNecessary();
        ASSERT1(!m_cached_transform_is_dirty);
        return m_cached_transform;
    }
    Matrix2<T> TransformationInverse () const
    {
        RecalculateTransformIfNecessary();
        ASSERT1(!m_cached_transform_is_dirty);
        return m_cached_transform.Inverse();
    }

    T Determinant () const
    {
        RecalculateScalingAndRotationIfNecessary();
        return m_cached_scaling_and_rotation.Determinant();
    }

    bool IsDirty () const
    {
        return m_cached_transform_is_dirty;
    }
    bool ScalingAndRotationIsDirty () const
    {
        return m_cached_scaling_and_rotation_is_dirty;
    }

    // ///////////////////////////////////////////////////////////////////////
    // modifiers
    // ///////////////////////////////////////////////////////////////////////

    void SetTranslation (Vector<T, 2> const &translation)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(translation[Dim::X]));
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(translation[Dim::Y]));
        m_translation = translation;
        m_cached_transform_is_dirty = true;
    }
    void SetScaleFactors (Vector<T, 2> const &scale_factors)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factors[Dim::X]));
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factors[Dim::Y]));
        m_scale_factors = scale_factors;
        Dirtify();
    }
    void SetScaleFactor (T scale_factor)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factor));
        m_scale_factors.FillWith(scale_factor);
        Dirtify();
    }
    void SetAngle (T angle)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(angle));
        m_angle = angle;
        Dirtify();
    }
    void SetPostTranslate (bool post_translate)
    {
        m_post_translate = post_translate;
        Dirtify();
    }

    void Translate (Vector<T, 2> const &translation)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(translation[Dim::X]));
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(translation[Dim::Y]));
        m_translation += translation;
        m_cached_transform_is_dirty = true;
    }
    void Scale (Vector<T, 2> const &scale_factors)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factors[Dim::X]));
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factors[Dim::Y]));
        m_scale_factors *= scale_factors;
        Dirtify();
    }
    void Scale (T scale_factor)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factor));
        m_scale_factors *= scale_factor;
        Dirtify();
    }
    void Rotate (T angle)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(angle));
        m_angle += angle;
        Dirtify();
    }

    void ResetTranslation ()
    {
        m_translation = Vector<T, 2>::ms_zero;
        m_cached_transform_is_dirty = true;
    }
    void ResetScale ()
    {
        m_scale_factors.FillWith(static_cast<T>(1));
        Dirtify();
    }
    void ResetAngle ()
    {
        m_angle = static_cast<T>(0);
        Dirtify();
    }

    // attempts to fit a general Matrix2 (which has 6 degrees of freedom)
    // to a Transform2 (which has 5 degrees of freedom);
    // [a  b  x]   [r cos U  -s sin V  x]
    // [c  d  y] = [r sin U   s cos V  y]
    // [0  0  1]   [   0         0     1]
    // in order to exactly match a Transform2, U must equal V.  the angle of
    // the Transform2 is given by the average of U and V, and the return value
    // is the absolute value of the difference U-V.  the Transform2 is set
    // to post-translate.
    Float FitMatrix2 (Matrix2<T> const &matrix)
    {
        Float r = Math::Sqrt(matrix[Matrix2<T>::A]*matrix[Matrix2<T>::A] + matrix[Matrix2<T>::C]*matrix[Matrix2<T>::C]);
        Float s = Math::Sqrt(matrix[Matrix2<T>::B]*matrix[Matrix2<T>::B] + matrix[Matrix2<T>::D]*matrix[Matrix2<T>::D]);
        Float U = static_cast<T>(0);
        Float V = static_cast<T>(0);
        if (r != static_cast<T>(0))
            U = Math::Arctan2(matrix[Matrix2<T>::C], matrix[Matrix2<T>::A]);
        if (s != static_cast<T>(0))
            V = Math::Arctan2(-matrix[Matrix2<T>::B], matrix[Matrix2<T>::D]);

        SetPostTranslate(true);
        SetTranslation(Vector<T,2>(matrix[Matrix2<T>::X], matrix[Matrix2<T>::Y]));
        SetAngle((U + V) / static_cast<T>(2));
        SetScaleFactors(Vector<T,2>(r, s));

        return Abs(U - V);
    }

    // ///////////////////////////////////////////////////////////////////////
    // procedures
    // ///////////////////////////////////////////////////////////////////////

    void Dirtify () const
    {
        m_cached_scaling_and_rotation_is_dirty = true;
        m_cached_transform_is_dirty = true;
    }

    void RecalculateTransformIfNecessary () const
    {
        if (m_cached_transform_is_dirty)
            RecalculateTransform();
    }
    void RecalculateTransform () const
    {
        ASSERT1(m_cached_transform_is_dirty);
        if (m_cached_scaling_and_rotation_is_dirty)
            RecalculateScalingAndRotation();
        RecalculateTransformWithoutScalingAndRotation();
    }
    void RecalculateTransformWithoutScalingAndRotation () const
    {
        ASSERT1(m_cached_transform_is_dirty);
        ASSERT1(!m_cached_scaling_and_rotation_is_dirty);

        if (m_post_translate)
        {
            m_cached_transform = m_cached_scaling_and_rotation;
            m_cached_transform.Translate(m_translation);
        }
        else
        {
            m_cached_transform = Matrix2<T>::ms_identity;
            m_cached_transform.Translate(m_translation);
            m_cached_transform *= m_cached_scaling_and_rotation;
        }

        m_cached_transform_is_dirty = false;
    }

private:

    void RecalculateScalingAndRotationIfNecessary () const
    {
        if (m_cached_scaling_and_rotation_is_dirty)
            RecalculateScalingAndRotation();
    }
    void RecalculateScalingAndRotation () const
    {
        ASSERT1(m_cached_scaling_and_rotation_is_dirty);

        // scaling happens first
        m_cached_scaling_and_rotation = Matrix2<T>::ms_identity;
        m_cached_scaling_and_rotation.Scale(m_scale_factors);
        m_cached_scaling_and_rotation.Rotate(m_angle);

        m_cached_scaling_and_rotation_is_dirty = false;
    }

    Vector<T, 2> m_translation;
    Vector<T, 2> m_scale_factors;
    // angle in degrees
    T m_angle;
    bool m_post_translate;

    mutable Matrix2<T> m_cached_transform;
    mutable bool m_cached_transform_is_dirty;

    mutable Matrix2<T> m_cached_scaling_and_rotation;
    mutable bool m_cached_scaling_and_rotation_is_dirty;
}; // end of class Transform2

/** This templatized definition will take care of defining the static
  * @c ms_identity member transform, assuming that @c static_cast<T>(0)
  * and @c static_cast<T>(1) work.
  * @brief Templatized static definition of the @c ms_identity transform.
  */
template <typename T>
Transform2<T> const Transform2<T>::ms_identity(
    Vector<T, 2>(static_cast<T>(0), static_cast<T>(0)),
    Vector<T, 2>(static_cast<T>(1), static_cast<T>(1)),
    static_cast<T>(0),
    true); // the post-translate value for ms_identity is arbitrary

template <typename T>
std::ostream &operator << (std::ostream &stream, Transform2<T> const &t)
{
    if (t.PostTranslate())
        return stream << "Transform2: scale factors " << t.ScaleFactors() << ", angle " << t.Angle() << ", translation " << t.Translation();
    else
        return stream << "Transform2: translation " << t.Translation() << ", angle " << t.Angle() << ", scale factors " << t.ScaleFactors();
}

// ///////////////////////////////////////////////////////////////////////////
// partial template specialization to allow Serializer::ReadAggregate and
// Serializer::WriteAggregate on Vector<T,dimension>
// ///////////////////////////////////////////////////////////////////////////

template <typename T>
struct Aggregate<Transform2<T> >
{
    static void Read (Serializer &serializer, Transform2<T> &dest)
    {
        {
            FloatVector2 translation;
            serializer.ReadAggregate<FloatVector2>(translation);
            dest.SetTranslation(translation);
        }
        {
            FloatVector2 scale_factors;
            serializer.ReadAggregate<FloatVector2>(scale_factors);
            dest.SetScaleFactors(scale_factors);
        }
        {
            Float angle;
            serializer.Read<Float>(angle);
            dest.SetAngle(angle);
        }
        {
            bool post_translate;
            serializer.Read<bool>(post_translate);
            dest.SetPostTranslate(post_translate);
        }
    }
    static void Write (Serializer &serializer, Transform2<T> const &source)
    {
        serializer.WriteAggregate<FloatVector2>(source.Translation());
        serializer.WriteAggregate<FloatVector2>(source.ScaleFactors());
        serializer.Write<Float>(source.Angle());
        serializer.Write<bool>(source.PostTranslate());
    }
};

/// Convenience typedef for a Transform2<Float>.
typedef Transform2<Float> FloatTransform2;

} // end of namespace Xrb

#endif // !defined(_XRB_TRANSFORM2_HPP_)

