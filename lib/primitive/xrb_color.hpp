// ///////////////////////////////////////////////////////////////////////////
// xrb_color.hpp by Victor Dods, created 2004/07/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_COLOR_HPP_)
#define _XRB_COLOR_HPP_

#include "xrb.hpp"

#include <stdio.h>

#include "xrb_ntuple.hpp"
#include "xrb_serializer.hpp"

namespace Xrb
{

typedef Float ColorCoord;
class Color : public NTuple<ColorCoord, 4>
{
public:

    static Color const ms_opaque_white;
    static Color const ms_opaque_black;
    static Color const ms_transparent_black;

    static Color const &ms_identity_color_mask;
    static Color const &ms_identity_color_bias;

    // efficient no-init constructor
    Color () { }
    // constructs an opaque RGB color (fully opaque alpha channel)
    Color (ColorCoord r, ColorCoord g, ColorCoord b)
        :
        NTuple<ColorCoord, 4>(r, g, b, 1.0f)
    { }
    // constructs an RGBA color
    Color (ColorCoord r, ColorCoord g, ColorCoord b, ColorCoord a)
        :
        NTuple<ColorCoord, 4>(r, g, b, a)
    { }
    // copy constructor
    Color (NTuple<ColorCoord, 4> const &c)
        :
        NTuple<ColorCoord, 4>(c)
    { }

    // let this Color object be x.  this method is essentially
    // x = blend(x,y).
    //
    // let blend(x,y) be the composition of blending functions x and y
    // for a single color channel (i.e. red/green/blue; not alpha), where
    // Ax and Ay are the alpha channels of the color bias and Cx and Cy
    // are the single color channels (i.e. red/green/blue; not alpha)
    // of the color bias.
    //
    // blend(x,y) = blend( (Ax,Cx), (Ay,Cy) )
    //                if Ax+Ay-Ax*Ay == 0,
    //            = ( 0 , 0 )
    //                otherwise
    //            = ( Ax+Ay-Ax*Ay , Cx*Ax+Cy*Ay-Cy*Ax*Ay / Ax+Ay-Ax*Ay )
    void Blend (Color const &y)
    {
        Color &x = *this;
        ColorCoord old_a = x[Dim::A];
        x[Dim::A] += y[Dim::A] - x[Dim::A]*y[Dim::A];
        if (x[Dim::A] == 0.0f)
            x[Dim::R] = x[Dim::G] = x[Dim::B] = 0.0f;
        else
        {
            x[Dim::R] = (x[Dim::R]*old_a + y[Dim::R]*y[Dim::A] - y[Dim::R]*old_a*y[Dim::A]) / x[Dim::A];
            x[Dim::G] = (x[Dim::G]*old_a + y[Dim::G]*y[Dim::A] - y[Dim::G]*old_a*y[Dim::A]) / x[Dim::A];
            x[Dim::B] = (x[Dim::B]*old_a + y[Dim::B]*y[Dim::A] - y[Dim::B]*old_a*y[Dim::A]) / x[Dim::A];
        }
    }
    static Color Blend (Color x, Color const &y)
    {
        x.Blend(y);
        return x;
    }
}; // end of class Color
// typedef NTuple<ColorCoord, 4> Color;

// ///////////////////////////////////////////////////////////////////////////
// partial template specialization to allow Serializer::ReadAggregate and
// Serializer::WriteAggregate on Color
// ///////////////////////////////////////////////////////////////////////////

template <>
struct Aggregate<Color>
{
    static void Read (Serializer &serializer, Color &dest) throw(Exception)
    {
        serializer.ReadBuffer<ColorCoord>(dest.m, LENGTHOF(dest.m));
    }
    static void Write (Serializer &serializer, Color const &source) throw(Exception)
    {
        serializer.WriteBuffer<ColorCoord>(source.m, LENGTHOF(source.m));
    }
};

} // end of namespace Xrb

#endif // !defined(_XRB_COLOR_HPP_)

