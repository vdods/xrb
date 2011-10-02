// ///////////////////////////////////////////////////////////////////////////
// xrb_rendercontext.hpp by Victor Dods, created 2005/02/07
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_RENDERCONTEXT_HPP_)
#define _XRB_RENDERCONTEXT_HPP_

#include "xrb.hpp"

#include "xrb_color.hpp"
#include "xrb_screencoord.hpp"
#include "xrb_time.hpp"

namespace Xrb {

/// @brief Provides lowest common denominator of information necessary to render to screen.
/// @details Contains the clipping rectangle, color mask and color bias for rendering. The clipping
/// rectangle indicates the area of the screen which is valid to render to.  The color mask is a
/// color which is component-wise multiplied into each pixel of the rendered pixels.  The color bias
/// specifies the color that the drawn pixels will be tinted (the alpha channel is used to specify
/// the strength of the tinting).
///
/// RenderContext is just a convenient container for the properties, and provides a few extra convenience methods.
///
/// This form of container lends itself mainly toward recursive widget rendering, where each child
/// widget has a progressively smaller rectangle inside its parent widget, and the color mask and
/// color bias are cumulative down the widget hierarchy.  Widget creates a new RenderContext for
/// each recursive level, applying the clipping rectangle, color mask and bias color of each child
/// it renders.  See @ref Xrb::Widget::Draw.
class RenderContext
{
public:

    /// Constructs a RenderContext using the given @c clip_rect and @c color_mask.
    RenderContext (
        ScreenCoordRect const &clip_rect,
        Color const &color_bias,
        Color const &color_mask,
        Time render_time)
        :
        m_clip_rect(clip_rect),
        m_color_bias(color_bias),
        m_color_mask(color_mask),
        m_render_time(render_time)
    { }
    RenderContext (RenderContext const &source)
        :
        m_clip_rect(source.m_clip_rect),
        m_color_bias(source.m_color_bias),
        m_color_mask(source.m_color_mask),
        m_render_time(source.m_render_time)
    { }
    ~RenderContext () { }

    /// Assignment operator
    void operator = (RenderContext const &source)
    {
        m_clip_rect = source.m_clip_rect;
        m_color_bias = source.m_color_bias;
        m_color_mask = source.m_color_mask;
        m_render_time = source.m_render_time;
    }

    /// Returns the clipping rectangle.
    ScreenCoordRect const &ClipRect () const { return m_clip_rect; }
    /// Returns the color bias.
    Color const &ColorBias () const { return m_color_bias; }
    /// Returns the color mask.
    Color const &ColorMask () const { return m_color_mask; }
    /// Returns a non-const reference to the color bias.  Use this reference to change the color bias.
    Color &ColorBias () { return m_color_bias; }
    /// Returns a non-const reference to the color mask.  Use this reference to change the color mask.
    Color &ColorMask () { return m_color_mask; }
    /// Returns the time as it pertains to rendering.
    Time RenderTime () const { return m_render_time; }
    /// @brief Returns the intersection of the clipping rect and the given.
    /// @details This method is used when rectangular regions need to be clipped against the clipping rectangle.
    ScreenCoordRect ClippedRect (ScreenCoordRect const &rect) const { return m_clip_rect & rect; }
    /// Returns the given color bias blended (on the right/inside) with this render context's
    /// color bias (i.e. blending function composition).
    Color BlendedColorBias (Color const &color_bias) const
    {
        Color blended_color_bias(m_color_bias);
        blended_color_bias.Blend(color_bias);
        return blended_color_bias;
    }
    /// @brief Returns the masked version of the given color.
    /// @details Performs component-wise multiplication of the color and color mask,
    Color MaskedColor (Color const &color) const { return m_color_mask * color; }
    /// Returns true iff the color mask and color bias would force any rendering operation to be a
    /// no op (i.e. completely transparent).
    bool MaskAndBiasWouldResultInNoOp () const;
    /// @brief Returns true iff the color mask, color bias and drawing color alpha channel value would force any rendering
    /// operation to be a no op (i.e. completely transparent).
    /// @param color_alpha_channel_value The alpha channel value of the drawing color, analogous to a texture fragment alpha value.
    bool MaskAndBiasWouldResultInNoOp (ColorCoord color_alpha_channel_value) const;

    /// Sets the value of the clipping rectangle using the given rect.
    void SetClipRect (ScreenCoordRect const &clip_rect) { m_clip_rect = clip_rect; }

    /// Clips the clipping rectangle using the given rectangle.
    void ApplyClipRect (ScreenCoordRect const &clip_rect) { m_clip_rect &= clip_rect; }
    /// Blends the current color bias with the given color (right-multiplication).
    void ApplyColorBias (Color const &color_bias) { m_color_bias.Blend(color_bias); }
    /// Masks the color mask using the given color.
    void ApplyColorMask (Color const &color_mask) { m_color_mask *= color_mask; }
    /// Masks only the alpha component of the color mask.
    void ApplyAlphaMaskToColorMask (Float const alpha_mask) { m_color_mask[Dim::A] *= alpha_mask; }

private:

    ScreenCoordRect m_clip_rect;
    Color m_color_bias;
    Color m_color_mask;
    Time m_render_time;
}; // end of class RenderContext

} // end of namespace Xrb

#endif // !defined(_XRB_RENDERCONTEXT_HPP_)
