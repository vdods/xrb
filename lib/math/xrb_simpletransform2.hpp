// ///////////////////////////////////////////////////////////////////////////
// xrb_simpletransform2.hpp by Victor Dods, created 2005/05/27
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_SIMPLETRANSFORM2_HPP_)
#define _XRB_SIMPLETRANSFORM2_HPP_

#include "xrb.hpp"

#include "xrb_vector.hpp"

namespace Xrb {

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

    SimpleTransform2 () { }
    SimpleTransform2 (T r, T s, T x, T y)
    {
        m_scale_factors.SetComponents(r, s);
        m_translation.SetComponents(x, y);
    }
    SimpleTransform2 (SimpleTransform2<T> const &source)
    {
        m_scale_factors = source.m_scale_factors;
        m_translation = source.m_translation;
    }
    ~SimpleTransform2 () { }

    // ///////////////////////////////////////////////////////////////////////
    // overloaded operators
    // ///////////////////////////////////////////////////////////////////////

    T const &operator [] (ScaleFactorComponent scale_factor_component) const
    {
        return m_scale_factors.m[static_cast<Dim::Component>(scale_factor_component)];
    }
    T &operator [] (ScaleFactorComponent scale_factor_component)
    {
        return m_scale_factors.m[static_cast<Dim::Component>(scale_factor_component)];
    }
    T const &operator [] (TranslationComponent translation_component) const
    {
        return m_translation.m[static_cast<Dim::Component>(translation_component)];
    }
    T &operator [] (TranslationComponent translation_component)
    {
        return m_translation.m[static_cast<Dim::Component>(translation_component)];
    }
    void operator *= (SimpleTransform2<T> const &operand)
    {
        m_scale_factors *= operand.m_scale_factors;
        m_translation *= operand.m_scale_factors;
        m_translation += operand.m_translation;
    }
    void operator = (SimpleTransform2<T> const &operand)
    {
        m_scale_factors = operand.m_scale_factors;
        m_translation = operand.m_translation;
    }

    // ///////////////////////////////////////////////////////////////////////
    // accessors
    // ///////////////////////////////////////////////////////////////////////

    Vector<T, 2> const &ScaleFactors () const { return m_scale_factors; }
    Vector<T, 2> const &Translation () const { return m_translation; }

    SimpleTransform2<T> const &Transformation () const { return *this; }

    SimpleTransform2<T> Inverse () const
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

    void Invert ()
    {
        T determinant = m_scale_factors[Dim::X] * m_scale_factors[Dim::Y];
        T negative_determinant = -determinant;
        if (determinant != negative_determinant) // equivalent to determinant != 0
        {
            m_translation /= -m_scale_factors;
            m_scale_factors = 1.0f / m_scale_factors;
        }
    }

    void Scale (Vector<T, 2> const &scale_factors)
    {
        m_scale_factors *= scale_factors;
    }
    void Scale (T r, T s)
    {
        m_scale_factors[Dim::X] *= r;
        m_scale_factors[Dim::Y] *= s;
    }
    void Scale (T scale_factor)
    {
        m_scale_factors *= scale_factor;
    }
    void Translate (Vector<T, 2> const &translation)
    {
        m_translation += translation;
    }
    void Translate (T x, T y)
    {
        m_translation[Dim::X] += x;
        m_translation[Dim::Y] += y;
    }

    void SetScaleFactors (Vector<T, 2> const &scale_factors)
    {
        m_scale_factors = scale_factors;
    }
    void SetScaleFactors (T r, T s)
    {
        m_scale_factors[Dim::X] = r;
        m_scale_factors[Dim::Y] = s;
    }
    void SetScaleFactor (ScaleFactorComponent component, T scale_factor)
    {
        ASSERT1(component == R || component == S);
        m_scale_factors[component] = scale_factor;
    }
    void SetScaleFactor (T scale_factor)
    {
        m_scale_factors[Dim::X] == scale_factor;
        m_scale_factors[Dim::Y] == scale_factor;
    }
    void SetTranslation (Vector<T, 2> const &translation)
    {
        m_translation = translation;
    }
    void SetTranslation (T x, T y)
    {
        m_translation[Dim::X] = x;
        m_translation[Dim::Y] = y;
    }
    void SetTranslation (TranslationComponent component, T translation)
    {
        ASSERT1(component == X || component == Y);
        m_translation[component] = translation;
    }

    void SetComponents (T r, T s, T x, T y)
    {
        m_scale_factors.SetComponents(r, s);
        m_translation.SetComponents(x, y);
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
SimpleTransform2<T> operator * (SimpleTransform2<T> const &l, SimpleTransform2<T> const &r)
{
    return SimpleTransform2<T>(
        l[SimpleTransform2<T>::R] * r[SimpleTransform2<T>::R],
        l[SimpleTransform2<T>::S] * r[SimpleTransform2<T>::S],
        l[SimpleTransform2<T>::R] * r[SimpleTransform2<T>::X] +
        l[SimpleTransform2<T>::X],
        l[SimpleTransform2<T>::S] * r[SimpleTransform2<T>::Y] +
        l[SimpleTransform2<T>::Y]);
}

// vector *= simpletransform
template <typename T>
void operator *= (Vector<T, 2> &assignee, SimpleTransform2<T> const &operand)
{
    assignee *= operand.m_scale_factors;
    assignee += operand.m_translation;
}

// simpletransform * vector
template <typename T>
Vector<T, 2> operator * (SimpleTransform2<T> const &l,
                                Vector<T, 2> const &r)
{
    return Vector<T, 2>(
        l[SimpleTransform2<T>::R] * r[Dim::X] +
        l[SimpleTransform2<T>::X],
        l[SimpleTransform2<T>::S] * r[Dim::Y] +
        l[SimpleTransform2<T>::Y]);
}

template <typename T>
std::ostream &operator << (std::ostream &stream, SimpleTransform2<T> const &t)
{
    return stream << "SimpleTransform2:\n"
                     "\t[" << t.m_scale_factors[Dim::X] << ", " << T(0) << ", " << t.m_translation[Dim::X] << "]\n"
                     "\t[" << T(0) << ", " << t.m_scale_factors[Dim::Y] << ", " << t.m_translation[Dim::Y] << "]\n";
}

/// Convenience typedef for a SimpleTransform2<Float>.
typedef SimpleTransform2<Float> FloatSimpleTransform2;

} // end of namespace Xrb
    
#endif // !defined(_XRB_SIMPLETRANSFORM2_HPP_)

