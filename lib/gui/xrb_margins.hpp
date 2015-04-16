// ///////////////////////////////////////////////////////////////////////////
// xrb_margins.hpp by Victor Dods, created 2011/06/29
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_MARGINS_HPP_)
#define _XRB_MARGINS_HPP_

#include "xrb.hpp"

#include "xrb_screencoord.hpp"

namespace Xrb {

/** Gives the left, right, bottom, top margin sizes for a widget or in general
  * an object which has margins.  Basically just a dumb container.
  */
template <typename T>
struct Margins
{
    // zeroed margins
    static Margins<T> const ms_zero;

    typedef Vector<T, 2> VectorType;

    VectorType m_bottom_left;
    VectorType m_top_right;

    Margins (VectorType const &identical_margins = VectorType::ms_zero)
        :
        m_bottom_left(identical_margins),
        m_top_right(identical_margins)
    { }
    Margins (VectorType const &bottom_left, VectorType const &top_right)
        :
        m_bottom_left(bottom_left),
        m_top_right(top_right)
    { }

    void operator = (Margins const &x) { m_bottom_left = x.m_bottom_left; m_top_right = x.m_top_right; }
    bool operator == (Margins const &x) { return m_bottom_left == x.m_bottom_left && m_top_right == x.m_top_right; }
    bool operator != (Margins const &x) { return m_bottom_left != x.m_bottom_left || m_top_right != x.m_top_right; }

    Margins<T> operator - () const { return Margins<T>(-m_bottom_left, -m_top_right); }

    VectorType TotalMarginSize () const { return m_bottom_left + m_top_right; }

    template <typename U>
    inline Margins<U> StaticCast () const
    {
        Margins<U> retval;
        retval.m_bottom_left = m_bottom_left.template StaticCast<U>();
        retval.m_top_right = m_top_right.template StaticCast<U>();
        return retval;
    }
}; // end of template <typename T> class Margins<T>

template <typename T>
Margins<T> const Margins<T>::ms_zero;

template <typename T>
inline Margins<T> operator + (Margins<T> const &x, Margins<T> const &y)
{
    return Margins<T>(x.m_bottom_left + y.m_bottom_left, x.m_top_right + y.m_top_right);
}

template <typename T>
inline Margins<T> operator - (Margins<T> const &x, Margins<T> const &y)
{
    return Margins<T>(x.m_bottom_left - y.m_bottom_left, x.m_top_right - y.m_top_right);
}

template <typename T>
inline Margins<T> operator * (Margins<T> const &x, T y)
{
    return Margins<T>(x.m_bottom_left * y, x.m_top_right * y);
}

template <typename T>
inline Margins<T> operator / (Margins<T> const &x, T y)
{
    return Margins<T>(x.m_bottom_left / y, x.m_top_right / y);
}

// ///////////////////////////////////////////////////////////////////////////
// convenience typedefs for Margins of different types,
// ///////////////////////////////////////////////////////////////////////////

/** @brief Convenience typedef for a Margins<Float>.
  */
typedef Margins<Float> FloatMargins;
/** @brief Convenience typedef for a Margins<ScreenCoord>.
  */
typedef Margins<ScreenCoord> ScreenCoordMargins;

} // end of namespace Xrb

#endif // !defined(_XRB_MARGINS_HPP_)
