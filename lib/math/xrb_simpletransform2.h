// ///////////////////////////////////////////////////////////////////////////
// xrb_simpletransform2.h by Victor Dods, created 2005/05/27
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_SIMPLETRANSFORM2_H_)
#define _XRB_SIMPLETRANSFORM2_H_

#include "xrb.h"

#include <stdio.h>

#include "xrb_vector.h"

namespace Xrb
{

template <typename T>
class SimpleTransform2
{
public:

    static SimpleTransform2<T> const ms_identity;

    enum ScaleFactorComponent
    {
        R = Dim::X,
        S = Dim::Y
    };

    enum TranslationComponent
    {
        X = Dim::X,
        Y = Dim::Y
    };

    // NOTE: when multiplying a vector by a SimpleTransform2, scaling
    // happens before translation

    Vector<T, 2> m_scale_factors;
    Vector<T, 2> m_translation;

    // ///////////////////////////////////////////////////////////////////////
    // constructors and destructor
    // ///////////////////////////////////////////////////////////////////////

    inline SimpleTransform2 () { }
    inline SimpleTransform2 (T r, T s, T x, T y)
    {
        m_scale_factors.SetComponents(r, s);
        m_translation.SetComponents(x, y);
    }
    inline SimpleTransform2 (SimpleTransform2<T> const &source)
    {
        m_scale_factors = source.m_scale_factors;
        m_translation = source.m_translation;
    }
    inline ~SimpleTransform2 () { }

    // ///////////////////////////////////////////////////////////////////////
    // overloaded operators
    // ///////////////////////////////////////////////////////////////////////

    inline T const &operator [] (ScaleFactorComponent scale_factor_component) const
    {
        return m_scale_factors.m[static_cast<Dim::Component>(scale_factor_component)];
    }
    inline T &operator [] (ScaleFactorComponent scale_factor_component)
    {
        return m_scale_factors.m[static_cast<Dim::Component>(scale_factor_component)];
    }
    inline T const &operator [] (TranslationComponent translation_component) const
    {
        return m_translation.m[static_cast<Dim::Component>(translation_component)];
    }
    inline T &operator [] (TranslationComponent translation_component)
    {
        return m_translation.m[static_cast<Dim::Component>(translation_component)];
    }
    inline void operator *= (SimpleTransform2<T> const &operand)
    {
        m_scale_factors *= operand.m_scale_factors;
        m_translation *= operand.m_scale_factors;
        m_translation += operand.m_translation;
    }
    inline void operator = (SimpleTransform2<T> const &operand)
    {
        m_scale_factors = operand.m_scale_factors;
        m_translation = operand.m_translation;
    }

    // ///////////////////////////////////////////////////////////////////////
    // accessors
    // ///////////////////////////////////////////////////////////////////////

    Vector<T, 2> const &GetScaleFactors () const
    {
        return m_scale_factors;
    }
    Vector<T, 2> const &GetTranslation () const
    {
        return m_translation;
    }

    SimpleTransform2<T> const &GetTransformation () const
    {
        return *this;
    }

    SimpleTransform2<T> GetInverse () const
    {
        SimpleTransform2<T> retval;
        T determinant = m_scale_factors[Dim::X] * m_scale_factors[Dim::Y];
        T negative_determinant = -determinant;
        if (determinant != negative_determinant) // equivalent to determinant != 0
        {
            retval.m_translation = -m_translation / m_scale_factors;
            retval.m_scale_factors = 1.0f / m_scale_factors;
        }
        return retval;
    }

    // ///////////////////////////////////////////////////////////////////////
    // modifiers
    // ///////////////////////////////////////////////////////////////////////

    inline void Invert ()
    {
        T determinant = m_scale_factors[Dim::X] * m_scale_factors[Dim::Y];
        T negative_determinant = -determinant;
        if (determinant != negative_determinant) // equivalent to determinant != 0
        {
            m_translation /= -m_scale_factors;
            m_scale_factors = 1.0f / m_scale_factors;
        }
    }

    inline void Scale (Vector<T, 2> const &scale_factors)
    {
        m_scale_factors *= scale_factors;
    }
    inline void Scale (T r, T s)
    {
        m_scale_factors[Dim::X] *= r;
        m_scale_factors[Dim::Y] *= s;
    }
    inline void Scale (T scale_factor)
    {
        m_scale_factors *= scale_factor;
    }
    inline void Translate (Vector<T, 2> const &translation)
    {
        m_translation += translation;
    }
    inline void Translate (T x, T y)
    {
        m_translation[Dim::X] += x;
        m_translation[Dim::Y] += y;
    }

    inline void SetScaleFactors (Vector<T, 2> const &scale_factors)
    {
        m_scale_factors = scale_factors;
    }
    inline void SetScaleFactors (T r, T s)
    {
        m_scale_factors[Dim::X] = r;
        m_scale_factors[Dim::Y] = s;
    }
    inline void SetScaleFactor (ScaleFactorComponent component, T scale_factor)
    {
        ASSERT1(component == R || component == S);
        m_scale_factors[component] = scale_factor;
    }
    inline void SetScaleFactor (T scale_factor)
    {
        m_scale_factors[Dim::X] == scale_factor;
        m_scale_factors[Dim::Y] == scale_factor;
    }
    inline void SetTranslation (Vector<T, 2> const &translation)
    {
        m_translation = translation;
    }
    inline void SetTranslation (T x, T y)
    {
        m_translation[Dim::X] = x;
        m_translation[Dim::Y] = y;
    }
    inline void SetTranslation (TranslationComponent component, T translation)
    {
        ASSERT1(component == X || component == Y);
        m_translation[component] = translation;
    }

    inline void SetComponents (T r, T s, T x, T y)
    {
        m_scale_factors.SetComponents(r, s);
        m_translation.SetComponents(x, y);
    }

    // ///////////////////////////////////////////////////////////////////////
    // procedures
    // ///////////////////////////////////////////////////////////////////////

    void Fprint (FILE *fptr, char const *component_printf_format) const
    {
        ASSERT1(fptr != NULL);
        ASSERT1(component_printf_format != NULL);

        // clever way to get zero
        T zero = m_scale_factors[Dim::X] - m_scale_factors[Dim::X];

        fprintf(fptr, "SimpleTransform2:\n\t[");
        fprintf(fptr, component_printf_format, m_scale_factors[Dim::X]);
        fprintf(fptr, ", ");
        fprintf(fptr, component_printf_format, zero);
        fprintf(fptr, ", ");
        fprintf(fptr, component_printf_format, m_translation[Dim::X]);
        fprintf(fptr, "]\n\t[");
        fprintf(fptr, component_printf_format, m_scale_factors[Dim::Y]);
        fprintf(fptr, ", ");
        fprintf(fptr, component_printf_format, zero);
        fprintf(fptr, ", ");
        fprintf(fptr, component_printf_format, m_translation[Dim::Y]);
        fprintf(fptr, "]\n");
    }
}; // end of class SimpleTransform2

/** This templatized definition will take care of defining the static
  * @c ms_identity member transform, assuming that @c static_cast<T>(0) and
  * @c static_cast<T>(1) work.
  * @brief Templatized static definition of the @c ms_identity transform.
  */
template <typename T>
SimpleTransform2<T> const SimpleTransform2<T>::ms_identity(
    static_cast<T>(1), static_cast<T>(1),
    static_cast<T>(0), static_cast<T>(0));

// ///////////////////////////////////////////////////////////////////////////
// Operator overloads
// ///////////////////////////////////////////////////////////////////////////

// simpletransform * simpletransform
template <typename T>
inline SimpleTransform2<T> operator * (SimpleTransform2<T> const left_operand,
                                       SimpleTransform2<T> const right_operand)
{
    return SimpleTransform2<T>(
        left_operand[SimpleTransform2<T>::R] * right_operand[SimpleTransform2<T>::R],
        left_operand[SimpleTransform2<T>::S] * right_operand[SimpleTransform2<T>::S],
        left_operand[SimpleTransform2<T>::R] * right_operand[SimpleTransform2<T>::X] +
        left_operand[SimpleTransform2<T>::X],
        left_operand[SimpleTransform2<T>::S] * right_operand[SimpleTransform2<T>::Y] +
        left_operand[SimpleTransform2<T>::Y]);
}

// vector *= simpletransform
template <typename T>
inline void operator *= (Vector<T, 2> &assignee,
                         SimpleTransform2<T> const &operand)
{
    assignee *= operand.m_scale_factors;
    assignee += operand.m_translation;
}

// simpletransform * vector
template <typename T>
inline Vector<T, 2> operator * (SimpleTransform2<T> const &left_operand,
                                Vector<T, 2> const &right_operand)
{
    return Vector<T, 2>(
        left_operand[SimpleTransform2<T>::R] * right_operand[Dim::X] +
        left_operand[SimpleTransform2<T>::X],
        left_operand[SimpleTransform2<T>::S] * right_operand[Dim::Y] +
        left_operand[SimpleTransform2<T>::Y]);
}

// ///////////////////////////////////////////////////////////////////////////
// convenience typedefs for SimpleTransform2 of different types,
// pre-made representations of the identity for each typedef,
// and format-specific Fprint functions for each typedef.
// ///////////////////////////////////////////////////////////////////////////

/** FloatSimpleTransform2
  * @brief Convenience typedef for a SimpleTransform2<Float>.
  */
typedef SimpleTransform2<Float> FloatSimpleTransform2;
/** Sint32SimpleTransform2
  * @brief Convenience typedef for a SimpleTransform2<Sint32>.
  */
typedef SimpleTransform2<Sint32> Sint32SimpleTransform2;

/** This is a convenience function to provide a default
  * printf format to FloatSimpleTransform2::Fprint.
  * @brief Prints the given FloatSimpleTransform2 to the given file stream.
  * @param fptr The file stream to print to.
  * @param matrix The FloatSimpleTransform2 to print.
  */
void Fprint (FILE *fptr, FloatSimpleTransform2 const &simple_transform);

} // end of namespace Xrb
    
#endif // !defined(_XRB_SIMPLETRANSFORM2_H_)

