// ///////////////////////////////////////////////////////////////////////////
// xrb_rendercontext.h by Victor Dods, created 2005/02/07
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_RENDERCONTEXT_H_)
#define _XRB_RENDERCONTEXT_H_

#include "xrb.h"

#include "xrb_color.h"
#include "xrb_screencoord.h"

namespace Xrb
{

/** Contains the clipping rectangle and the color mask for rendering.
  * The clipping rectangle indicates the area of the screen which is valid
  * to render to.  The color mask is a color which is component-wise
  * multiplied into each pixel of the rendered pixels (by setting OpenGL's
  * glColor property).
  *
  * RenderContext is just a convenient container for the two properties,
  * and provides a few extra convenience methods.
  *
  * This form of container lends itself mainly toward recursive widget
  * rendering, where each child widget has a progressively smaller rectangle
  * inside its parent widget, and the color mask is cumulative down the
  * widget hierarchy.  Widget creates a new RenderContext for each recursive
  * level, applying the clipping rectangle and color mask of each child
  * it renders.  See @ref Xrb::Widget::Draw.
  *
  * @brief Provides lowest common denominator of information necessary to
  *        render to screen.
  */
class RenderContext
{
public:

    /** @brief Constructs a RenderContext using the given @c clip_rect
      *        and @c color_mask.
      */
    inline RenderContext (
        ScreenCoordRect const &clip_rect,
        Color const &color_mask)
    {
        m_clip_rect = clip_rect;
        m_color_mask = color_mask;
    }
    /** Simply copies the values of the source RenderContext.
      * @brief Copy constructor.
      */
    inline RenderContext (
        RenderContext const &source)
    {
        m_clip_rect = source.m_clip_rect;
        m_color_mask = source.m_color_mask;
    }
    /** @brief Destructor.  Does nothing.
      */
    inline ~RenderContext () { }

    /** @brief Returns the clipping rectangle.
      */
    inline ScreenCoordRect const &GetClipRect () const
    {
        return m_clip_rect;
    }
    /** @brief Returns the color mask.
      */
    inline Color const &GetColorMask () const
    {
        return m_color_mask;
    }
    /** This method is used when rectangular regions need to be clipped
      * against the clipping rectangle.
      * @brief Returns the intersection of the clipping rect and the given.
      */
    inline ScreenCoordRect GetClippedRect (ScreenCoordRect const &rect) const
    {
        return m_clip_rect & rect;
    }
    /** Performs component-wise multiplication of the color and color mask,
      * @brief Returns the masked version of the given color.
      */
    inline Color GetMaskedColor (Color const &color) const
    {
        return m_color_mask * color;
    }

    /** @brief Sets the value of the clipping rectangle using the given rect.
      */
    inline void SetClipRect (ScreenCoordRect const &clip_rect)
    {
        m_clip_rect = clip_rect;
    }
    /** @brief Sets the value of the color mask using the given color.
      */
    inline void SetColorMask (Color const &color_mask)
    {
        m_color_mask = color_mask;
    }

    /** @brief Clips the clipping rectangle using the given rectangle.
      */
    inline void ApplyClipRect (ScreenCoordRect const &clip_rect)
    {
        m_clip_rect &= clip_rect;
    }
    /** @brief Masks the color mask using the given color.
      */
    inline void ApplyColorMask (Color const &color_mask)
    {
        m_color_mask *= color_mask;
    }
    /** @brief Masks only the alpha component of the color mask.
      */
    inline void ApplyAlphaMaskToColorMask (Float const alpha_mask)
    {
        m_color_mask[Dim::A] *= alpha_mask;
    }
    /** @brief Causes OpenGL to use the current clipping rectangle.
      */
    void SetupGLClipRect ();

    /** Simply copies the properties of the source.
      * @brief Assignment operator.
      */
    inline void operator = (RenderContext const &source)
    {
        m_clip_rect = source.m_clip_rect;
        m_color_mask = source.m_color_mask;
    }

private:

    ScreenCoordRect m_clip_rect;
    Color m_color_mask;
}; // end of class RenderContext

} // end of namespace Xrb

#endif // !defined(_XRB_RENDERCONTEXT_H_)
