// ///////////////////////////////////////////////////////////////////////////
// xrb_transform2.h by Victor Dods, created 2005/05/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_TRANSFORM2_H_)
#define _XRB_TRANSFORM2_H_

#include "xrb.h"

#include "xrb_matrix2.h"
#include "xrb_simpletransform2.h"
#include "xrb_vector.h"

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

    inline Transform2 (bool post_translate)
    {
        m_post_translate = post_translate;
        Dirtify();
    }
    inline Transform2 (
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
    inline Transform2 (
        SimpleTransform2<T> const &source,
        bool post_translate)
    {
        m_translation = source.m_translation;
        m_scale_factors = source.m_scale_factors;
        m_angle = static_cast<T>(0);
        m_post_translate = post_translate;
        Dirtify();
    }
    inline Transform2 (Transform2<T> const &source)
    {
        m_translation = source.m_translation;
        m_scale_factors = source.m_scale_factors;
        m_angle = source.m_angle;
        m_post_translate = source.m_post_translate;
        Dirtify();
    }
    inline Transform2 (
        Transform2<T> const &source,
        bool post_translate)
    {
        m_translation = source.m_translation;
        m_scale_factors = source.m_scale_factors;
        m_angle = source.m_angle;
        m_post_translate = post_translate;
        Dirtify();
    }
    inline ~Transform2 () { }

    // ///////////////////////////////////////////////////////////////////////
    // overloaded operators
    // ///////////////////////////////////////////////////////////////////////

    inline void operator = (Transform2<T> const &source)
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

    inline Vector<T, 2> const &GetTranslation () const
    {
        return m_translation;
    }
    inline Vector<T, 2> const &GetScaleFactors () const
    {
        return m_scale_factors;
    }
    inline T GetScaleFactor () const
    {
        ASSERT1(m_scale_factors[Dim::X] == m_scale_factors[Dim::Y])
        return m_scale_factors[Dim::X];
    }
    inline T GetAngle () const
    {
        return m_angle;
    }
    inline bool GetPostTranslate () const
    {
        return m_post_translate;
    }

    inline Matrix2<T> const &GetTransformation () const
    {
        RecalculateTransformIfNecessary();
        ASSERT1(!m_cached_transform_is_dirty)
        return m_cached_transform;
    }
    inline Matrix2<T> GetTransformationInverse () const
    {
        RecalculateTransformIfNecessary();
        ASSERT1(!m_cached_transform_is_dirty)
        return m_cached_transform.GetInverse();
    }

    inline T GetDeterminant () const
    {
        RecalculateScalingAndRotationIfNecessary();
        return m_cached_scaling_and_rotation.GetDeterminant();
    }

    inline bool GetIsDirty () const
    {
        return m_cached_transform_is_dirty;
    }
    inline bool GetScalingAndRotationIsDirty () const
    {
        return m_cached_scaling_and_rotation_is_dirty;
    }

    // ///////////////////////////////////////////////////////////////////////
    // modifiers
    // ///////////////////////////////////////////////////////////////////////

    inline void SetTranslation (Vector<T, 2> const &translation)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(translation[Dim::X]))
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(translation[Dim::Y]))
        m_translation = translation;
        m_cached_transform_is_dirty = true;
    }
    inline void SetScaleFactors (Vector<T, 2> const &scale_factors)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factors[Dim::X]))
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factors[Dim::Y]))
        m_scale_factors = scale_factors;
        Dirtify();
    }
    inline void SetScaleFactor (T scale_factor)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factor))
        m_scale_factors.FillWith(scale_factor);
        Dirtify();
    }
    inline void SetAngle (T angle)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(angle))
        m_angle = angle;
        Dirtify();
    }
    inline void SetPostTranslate (bool post_translate)
    {
        m_post_translate = post_translate;
        Dirtify();
    }

    inline void Translate (Vector<T, 2> const &translation)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(translation[Dim::X]))
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(translation[Dim::Y]))
        m_translation += translation;
        m_cached_transform_is_dirty = true;
    }
    inline void Scale (Vector<T, 2> const &scale_factors)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factors[Dim::X]))
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factors[Dim::Y]))
        m_scale_factors *= scale_factors;
        Dirtify();
    }
    inline void Scale (T scale_factor)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factor))
        m_scale_factors *= scale_factor;
        Dirtify();
    }
    inline void Rotate (T angle)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(angle))
        m_angle += angle;
        Dirtify();
    }

    inline void ResetTranslation ()
    {
        m_translation = Vector<T, 2>::ms_zero;
        m_cached_transform_is_dirty = true;
    }
    inline void ResetScale ()
    {
        m_scale_factors.FillWith(static_cast<T>(1));
        Dirtify();
    }
    inline void ResetAngle ()
    {
        m_angle = static_cast<T>(0);
        Dirtify();
    }

    // ///////////////////////////////////////////////////////////////////////
    // procedures
    // ///////////////////////////////////////////////////////////////////////

    inline void Dirtify () const
    {
        m_cached_scaling_and_rotation_is_dirty = true;
        m_cached_transform_is_dirty = true;
    }
        
    void Fprint (
        FILE *fptr,
        char const *component_printf_format,
        bool add_newline = true) const
    {
        ASSERT1(fptr != NULL)
        ASSERT1(component_printf_format != NULL)

        if (m_post_translate)
        {
            fprintf(fptr, "Transform2: scale factors (");
            fprintf(fptr, component_printf_format, m_scale_factors[Dim::X]);
            fprintf(fptr, ", ");
            fprintf(fptr, component_printf_format, m_scale_factors[Dim::Y]);
            fprintf(fptr, "), angle (");
            fprintf(fptr, component_printf_format, m_angle);
            fprintf(fptr, "), translation (");
            fprintf(fptr, component_printf_format, m_translation[Dim::X]);
            fprintf(fptr, ", ");
            fprintf(fptr, component_printf_format, m_translation[Dim::Y]);
            fprintf(fptr, "), %c", add_newline ? '\n' : '\0');
        }
        else
        {
            fprintf(fptr, "Transform2: translation (");
            fprintf(fptr, component_printf_format, m_translation[Dim::X]);
            fprintf(fptr, ", ");
            fprintf(fptr, component_printf_format, m_translation[Dim::Y]);
            fprintf(fptr, "), scale factors (");
            fprintf(fptr, component_printf_format, m_scale_factors[Dim::X]);
            fprintf(fptr, ", ");
            fprintf(fptr, component_printf_format, m_scale_factors[Dim::Y]);
            fprintf(fptr, "), angle (");
            fprintf(fptr, component_printf_format, m_angle);
            fprintf(fptr, ")%c", add_newline ? '\n' : '\0');
        }
    }

    inline void RecalculateTransformIfNecessary () const
    {
        if (m_cached_transform_is_dirty)
        {
            if (m_cached_scaling_and_rotation_is_dirty)
                RecalculateScalingAndRotation();
            ASSERT1(!m_cached_scaling_and_rotation_is_dirty)

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
    }
    inline void RecalculateTransform () const
    {
        ASSERT1(m_cached_transform_is_dirty)
        if (m_cached_scaling_and_rotation_is_dirty)
            RecalculateScalingAndRotation();
        ASSERT1(!m_cached_scaling_and_rotation_is_dirty)

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
    inline void RecalculateTransformWithoutScalingAndRotation () const
    {
        ASSERT1(m_cached_transform_is_dirty)
        ASSERT1(!m_cached_scaling_and_rotation_is_dirty)
    
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
        {
            // scaling happens first
            m_cached_scaling_and_rotation = Matrix2<T>::ms_identity;
            m_cached_scaling_and_rotation.Scale(m_scale_factors);
            m_cached_scaling_and_rotation.Rotate(m_angle);

            m_cached_scaling_and_rotation_is_dirty = false;
        }
    }
    void RecalculateScalingAndRotation () const
    {
        ASSERT1(m_cached_scaling_and_rotation_is_dirty)
        
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

// ///////////////////////////////////////////////////////////////////////////
// convenience typedefs for Transform2 of different types,
// pre-made representations of the identity, zero and one for each typedef,
// and format-specific Fprint functions for each typedef.
// ///////////////////////////////////////////////////////////////////////////

/** FloatTransform2
  * @brief Convenience typedef for a Transform2<Float>.
  */
typedef Transform2<Float> FloatTransform2;

/** This is a convenience function to provide a default
  * printf format to FloatTransform2::Fprint.
  * @brief Prints the given FloatTransform2 to the given file stream.
  * @param fptr The file stream to print to.
  * @param transform The FloatTransform2 to print.
  */
void Fprint (FILE *fptr, FloatTransform2 const &transform, bool add_newline = true);

} // end of namespace Xrb
                       
#endif // !defined(_XRB_TRANSFORM2_H_)

