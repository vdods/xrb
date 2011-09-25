// ///////////////////////////////////////////////////////////////////////////
// xrb_rect.hpp by Victor Dods, created 2005/04/22
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_RECT_HPP_)
/// Ensures this header is not included twice in an implementation.
#define _XRB_RECT_HPP_

#include "xrb.hpp"

#include "xrb_simpletransform2.hpp"
#include "xrb_vector.hpp"

namespace Xrb {

/** Intended for use to specify regions in screen coordinates.  Templatized
  * for the purpose of allowing integer and floating-point based
  * rectangles.  The coordinate system used is right-handed: <pre>
      A
      |  top-left corner          top-right corner
    Y |       +-----------------------------+
      |       |                             |
    a |       |                             |
    x |       |                             |
    i |       |                             |
    s |       +-----------------------------+
      |  bottom-left corner       bottom-right corner
  ----+----------------------------------------------------->
      |                                          X axis
      |
  * </pre> The region specified by a Rect object is the axially-aligned
  * box contained within the coordinates given by m_bottom_left and
  * m_top_right.
  * @brief A rectangle primitive.
  */
template <typename T>
class Rect
{
public:

    /// The bottom-left coordinate of the rectangle.
    Vector<T, 2> m_bottom_left;
    /// The top-right coordinate of the rectangle.
    Vector<T, 2> m_top_right;

    /** Does no initialization.
      * @brief Default constructor
      */
    Rect () { }
    Rect (Vector<T, 2> const &size)
    {
        m_bottom_left = Vector<T, 2>::ms_zero;
        m_top_right = size;
    }
    Rect (T left, T bottom, T right, T top)
    {
        m_bottom_left[Dim::X] = left;
        m_bottom_left[Dim::Y] = bottom;
        m_top_right[Dim::X] = right;
        m_top_right[Dim::Y] = top;
    }
    Rect (Vector<T, 2> const &bottom_left,
                 Vector<T, 2> const &top_right)
    {
        m_bottom_left = bottom_left;
        m_top_right = top_right;
    }
    Rect (Rect<T> const &source)
    {
        m_bottom_left = source.m_bottom_left;
        m_top_right = source.m_top_right;
    }
    ~Rect ()
    {
    }

    // ///////////////////////////////////////////////////////////////////////
    // overloaded operators
    // ///////////////////////////////////////////////////////////////////////

    void operator = (Vector<T, 2> const &size)
    {
        m_bottom_left = Vector<T, 2>::ms_zero;
        m_top_right = size;
    }
    void operator = (Rect<T> const &source)
    {
        m_bottom_left = source.m_bottom_left;
        m_top_right = source.m_top_right;
    }

    bool operator == (Rect<T> const &operand)
    {
        return m_bottom_left == operand.m_bottom_left &&
               m_top_right == operand.m_top_right;
    }
    bool operator != (Rect<T> const &operand)
    {
        return m_bottom_left != operand.m_bottom_left ||
               m_top_right != operand.m_top_right;
    }

    void operator |= (Rect<T> const &operand)
    {
        EncompassPoint(operand.TopLeft());
        EncompassPoint(operand.BottomRight());
    }
    void operator &= (Rect<T> const &operand)
    {
        m_bottom_left[Dim::X] =
            Max(m_bottom_left[Dim::X], operand.m_bottom_left[Dim::X]);
        m_bottom_left[Dim::Y] =
            Max(m_bottom_left[Dim::Y], operand.m_bottom_left[Dim::Y]);
        m_top_right[Dim::X] = Min(m_top_right[Dim::X], operand.m_top_right[Dim::X]);
        m_top_right[Dim::Y] = Min(m_top_right[Dim::Y], operand.m_top_right[Dim::Y]);
        if (m_bottom_left[Dim::X] > m_top_right[Dim::X])
            m_bottom_left[Dim::X] = m_top_right[Dim::X];
        if (m_bottom_left[Dim::Y] > m_top_right[Dim::Y])
            m_bottom_left[Dim::Y] = m_top_right[Dim::Y];
    }

    void operator += (Vector<T, 2> const &operand)
    {
        m_bottom_left += operand;
        m_top_right += operand;
    }
    void operator -= (Vector<T, 2> const &operand)
    {
        m_bottom_left -= operand;
        m_top_right -= operand;
    }
    void operator *= (SimpleTransform2<T> const &transform)
    {
        m_bottom_left *= transform;
        m_top_right *= transform;
    }

    // ///////////////////////////////////////////////////////////////////////
    // accessors
    // ///////////////////////////////////////////////////////////////////////

    Vector<T, 2> TopLeft () const { return Vector<T, 2>(m_bottom_left[Dim::X], m_top_right[Dim::Y]); }
    Vector<T, 2> const &TopRight () const { return m_top_right; }
    Vector<T, 2> const &BottomLeft () const { return m_bottom_left; }
    Vector<T, 2> BottomRight () const { return Vector<T, 2>(m_top_right[Dim::X], m_bottom_left[Dim::Y]); }

    T Top () const { return m_top_right[Dim::Y]; }
    T Left () const { return m_bottom_left[Dim::X]; }
    T Bottom () const { return m_bottom_left[Dim::Y]; }
    T Right () const { return m_top_right[Dim::X]; }

    Vector<T, 2> Center () const { return (m_bottom_left + m_top_right) / static_cast<T>(2); }

    Vector<T, 2> Size () const { return m_top_right - m_bottom_left; }
    T Width () const { return Right() - Left(); }
    T Height () const { return Top() - Bottom(); }

    Rect<T> Grown (Vector<T, 2> const &grow_by) const
    {
        Rect<T> retval;
        retval.m_bottom_left = m_bottom_left - grow_by;
        retval.m_top_right = m_top_right + grow_by;
        return retval;
    }
    Rect<T> Grown (Vector<T, 2> const &grow_bottom_left_by, Vector<T, 2> const &grow_top_right_by) const
    {
        Rect<T> retval;
        retval.m_bottom_left = m_bottom_left - grow_bottom_left_by;
        retval.m_top_right = m_top_right + grow_top_right_by;
        return retval;
    }

    bool IsValid () const
    {
        return m_bottom_left[Dim::X] <= m_top_right[Dim::X] &&
               m_bottom_left[Dim::Y] <= m_top_right[Dim::Y];
    }
    bool HasPositiveArea () const
    {
        return m_bottom_left[Dim::X] < m_top_right[Dim::X] &&
               m_bottom_left[Dim::Y] < m_top_right[Dim::Y];
    }
    bool IsPointInside (Vector<T, 2> const &point) const
    {
        return m_bottom_left[Dim::X] <= point[Dim::X] &&
               point[Dim::X] < m_top_right[Dim::X] &&
               m_bottom_left[Dim::Y] <= point[Dim::Y] &&
               point[Dim::Y] < m_top_right[Dim::Y];
    }

    template <typename U>
    Rect<U> StaticCast () const
    {
        Rect<U> retval;
        retval.SetBottomLeft(m_bottom_left.StaticCast<U>());
        retval.SetTopRight(m_top_right.StaticCast<U>());
        return retval;
    }

    // ///////////////////////////////////////////////////////////////////////
    // modifiers
    // ///////////////////////////////////////////////////////////////////////

    void SetTopLeft (Vector<T, 2> const &top_left) { m_bottom_left[Dim::X] = top_left[Dim::X]; m_top_right[Dim::Y] = top_left[Dim::Y]; }
    void SetTopRight (Vector<T, 2> const &top_right) { m_top_right = top_right; }
    void SetBottomLeft (Vector<T, 2> const &bottom_left) { m_bottom_left = bottom_left; }
    void SetBottomRight (Vector<T, 2> const &bottom_right) { m_top_right[Dim::X] = bottom_right[Dim::X]; m_bottom_left[Dim::Y] = bottom_right[Dim::Y]; }

    void SetTop (T top) { m_top_right[Dim::Y] = top; }
    void SetLeft (T left) { m_bottom_left[Dim::X] = left; }
    void SetBottom (T bottom) { m_bottom_left[Dim::Y] = bottom; }
    void SetRight (T right) { m_top_right[Dim::X] = right; }

    void SetCenter (Vector<T, 2> const &center) { operator-=(Center()); operator+=(center); }

    void SetSize (Vector<T, 2> const &size) { m_top_right = m_bottom_left + size; }
    void SetSize (Uint32 component, T size) { ASSERT1(component < 2); m_top_right[component] = m_bottom_left[component] + size; }
    void SetWidth (T const width) { m_top_right[Dim::X] = m_bottom_left[Dim::X] + width; }
    void SetHeight (T const height) { m_top_right[Dim::Y] = m_bottom_left[Dim::Y] + height; }

    template <typename U>
    void StaticCastAssign (Rect<U> const &source)
    {
        m_bottom_left = source.BottomLeft().StaticCast<T>();
        m_top_right = source.TopRight().StaticCast<T>();
    }

    // ///////////////////////////////////////////////////////////////////////
    // procedures
    // ///////////////////////////////////////////////////////////////////////

    void EncompassPoint (Vector<T, 2> const &point)
    {
        if (point[Dim::X] < m_bottom_left[Dim::X])
            m_bottom_left[Dim::X] = point[Dim::X];
        else if (point[Dim::X] > m_top_right[Dim::X])
            m_top_right[Dim::X] = point[Dim::X];

        if (point[Dim::Y] < m_bottom_left[Dim::Y])
            m_bottom_left[Dim::Y] = point[Dim::Y];
        else if (point[Dim::Y] > m_top_right[Dim::Y])
            m_top_right[Dim::Y] = point[Dim::Y];
    }

    void Flip ()
    {
        T temp;

        temp = m_bottom_left[Dim::X];
        m_bottom_left[Dim::X] = m_top_right[Dim::X];
        m_top_right[Dim::X] = temp;

        temp = m_bottom_left[Dim::Y];
        m_bottom_left[Dim::Y] = m_top_right[Dim::Y];
        m_top_right[Dim::Y] = temp;
    }
    void Flip (Dim::Component const component)
    {
        ASSERT1(component <= Dim::Y);

        T temp = m_bottom_left[component];
        m_bottom_left[component] = m_top_right[component];
        m_top_right[component] = temp;
    }

    void Grow (Vector<T, 2> const &vector_to_grow_by)
    {
        m_bottom_left -= vector_to_grow_by;
        m_top_right += vector_to_grow_by;
    }
}; // end of class Rect

template <typename T>
Rect<T> operator | (Rect<T> const &left_operand, Rect<T> const &right_operand)
{
    Rect<T> retval(left_operand);
    retval |= right_operand;
    return retval;
}

template <typename T>
Rect<T> operator & (Rect<T> const &left_operand, Rect<T> const &right_operand)
{
    Rect<T> retval(left_operand);
    retval &= right_operand;
    return retval;
}

template <typename T>
Rect<T> operator + (Rect<T> const &left_operand, Vector<T, 2> const &right_operand)
{
    Rect<T> retval(left_operand);
    retval += right_operand;
    return retval;
}

template <typename T>
Rect<T> operator - (Rect<T> const &left_operand, Vector<T, 2> const &right_operand)
{
    Rect<T> retval(left_operand);
    retval -= right_operand;
    return retval;
}

template <typename T>
Rect<T> operator * (SimpleTransform2<T> const &left_operand, Rect<T> const &right_operand)
{
    Rect<T> retval(right_operand);
    retval *= left_operand;
    return retval;
}

template <typename T>
std::ostream &operator << (std::ostream &stream, Rect<T> const &r)
{
    return stream << "Rect: left/bottom = " << r.BottomLeft() << ", top/right = " << r.TopRight() << ", size = " << r.Size();
}

/// Convenience typedef for a Rect<Float>.
typedef Rect<Float> FloatRect;
/// Convenience typedef for a Rect<Sint32>.
typedef Rect<Sint32> Sint32Rect;

} // end of namespace Xrb

#endif // !defined(_XRB_RECT_HPP_)
