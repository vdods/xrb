// ///////////////////////////////////////////////////////////////////////////
// xrb_widgetbackground.cpp by Victor Dods, created 2005/02/09
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_widgetbackground.hpp"

#include "xrb_gltexture.hpp"
#include "xrb_render.hpp"
#include "xrb_simpletransform2.hpp"

namespace Xrb
{

// ///////////////////////////////////////////////////////////////////////////
// WidgetBackgroundColored
// ///////////////////////////////////////////////////////////////////////////

WidgetBackground *WidgetBackgroundColored::CreateClone () const
{
    return new WidgetBackgroundColored(m_color);
}

void WidgetBackgroundColored::Draw (
    RenderContext const &render_context,
    ScreenCoordRect const &widget_screen_rect,
    ScreenCoordMargins const &frame_margins) const
{
    Render::DrawScreenRect(
        render_context,
        m_color,
        widget_screen_rect);
}

// ///////////////////////////////////////////////////////////////////////////
// WidgetBackgroundTextured
// ///////////////////////////////////////////////////////////////////////////

WidgetBackgroundTextured::WidgetBackgroundTextured (
    std::string const &texture_path)
    :
    WidgetBackground()
{
    ASSERT1(texture_path.length() > 0);

    m_texture = GlTexture::Load(texture_path);
    ASSERT1(m_texture.IsValid());
}

WidgetBackgroundTextured::WidgetBackgroundTextured (
    Resource<GlTexture> const &texture)
    :
    WidgetBackground()
{
    m_texture = texture;
}

WidgetBackgroundTextured::~WidgetBackgroundTextured ()
{
}

WidgetBackground *WidgetBackgroundTextured::CreateClone () const
{
    return new WidgetBackgroundTextured(m_texture);
}

void WidgetBackgroundTextured::Draw (
    RenderContext const &render_context,
    ScreenCoordRect const &widget_screen_rect,
    ScreenCoordMargins const &frame_margins) const
{
    ASSERT1(m_texture.IsValid());
    Render::DrawScreenRectTexture(
        render_context,
        **m_texture,
        widget_screen_rect);
}

// ///////////////////////////////////////////////////////////////////////////
// WidgetBackgroundTextured3Way
// ///////////////////////////////////////////////////////////////////////////

WidgetBackgroundTextured3Way::WidgetBackgroundTextured3Way (
    std::string const &left_texture_name,
    std::string const &center_texture_name,
    std::string const &right_texture_name)
    :
    WidgetBackground()
{
    m_left_texture = GlTexture::Load(left_texture_name);
    ASSERT1(m_left_texture.IsValid());

    m_center_texture = GlTexture::Load(center_texture_name);
    ASSERT1(m_center_texture.IsValid());

    if (!right_texture_name.empty())
    {
        m_right_texture = GlTexture::Load(right_texture_name);
        ASSERT1(m_right_texture.IsValid());
    }
}

WidgetBackgroundTextured3Way::WidgetBackgroundTextured3Way (
    Resource<GlTexture> const &left_texture,
    Resource<GlTexture> const &center_texture,
    Resource<GlTexture> const &right_texture)
    :
    WidgetBackground()
{
    ASSERT1(left_texture.IsValid());
    ASSERT1(center_texture.IsValid());
    // don't require right_texture to be be valid

    m_left_texture = left_texture;
    m_center_texture = center_texture;
    m_right_texture = right_texture;
}

WidgetBackgroundTextured3Way::~WidgetBackgroundTextured3Way ()
{
}

WidgetBackground *WidgetBackgroundTextured3Way::CreateClone () const
{
    return new WidgetBackgroundTextured3Way(m_left_texture, m_center_texture, m_right_texture);
}

void WidgetBackgroundTextured3Way::Draw (
    RenderContext const &render_context,
    ScreenCoordRect const &widget_screen_rect,
    ScreenCoordMargins const &frame_margins) const
{
    ASSERT1(m_left_texture.IsValid());
    ASSERT1(m_center_texture.IsValid());
    // don't require m_right_texture to be valid
    // set up the measurements for the rectangles
    ScreenCoord left_width = (widget_screen_rect.Height() * m_left_texture->Width()) / m_left_texture->Height();
    ScreenCoord right_width = left_width;
    if (m_right_texture.IsValid())
        right_width = (widget_screen_rect.Height() * m_right_texture->Width()) / m_right_texture->Height();
    ScreenCoord horizontal[4] =
    {
        widget_screen_rect.Left(),
        widget_screen_rect.Left() + left_width,
        widget_screen_rect.Right() - right_width,
        widget_screen_rect.Right()
    };
    ScreenCoord vertical[2] =
    {
        widget_screen_rect.Bottom(),
        widget_screen_rect.Top()
    };

    // set up the rectangles
    ScreenCoordRect left(
        horizontal[0], vertical[0],
        horizontal[1], vertical[1]);
    ScreenCoordRect center(
        horizontal[1], vertical[0],
        horizontal[2], vertical[1]);
    ScreenCoordRect right(
        horizontal[2], vertical[0],
        horizontal[3], vertical[1]);

    // draw the rectangles
    Render::DrawScreenRectTexture(
        render_context,
        **m_left_texture,
        left,
        FloatSimpleTransform2( 1.0f,  1.0f, 0.0f, 0.0f));
    Render::DrawScreenRectTexture(
        render_context,
        **m_center_texture,
        center,
        FloatSimpleTransform2( 1.0f,  1.0f, 0.0f, 0.0f));
    if (m_right_texture.IsValid())
    {
        Render::DrawScreenRectTexture(
            render_context,
            **m_right_texture,
            right,
            FloatSimpleTransform2( 1.0f,  1.0f, 0.0f, 0.0f));
    }
    else
    {
        Render::DrawScreenRectTexture(
            render_context,
            **m_left_texture,
            right,
            FloatSimpleTransform2(-1.0f,  1.0f, 1.0f, 0.0f));
    }
}

// ///////////////////////////////////////////////////////////////////////////
// WidgetBackgroundStylized
// ///////////////////////////////////////////////////////////////////////////

WidgetBackgroundStylized::WidgetBackgroundStylized (
    std::string const &corner_texture_name,
    std::string const &top_texture_name,
    std::string const &left_texture_name,
    std::string const &center_texture_name)
    :
    WidgetBackground()
{
    m_corner_texture = GlTexture::Load(corner_texture_name);
    ASSERT1(m_corner_texture.IsValid());

    m_top_texture = GlTexture::Load(top_texture_name);
    ASSERT1(m_top_texture.IsValid());

    m_left_texture = GlTexture::Load(left_texture_name);
    ASSERT1(m_left_texture.IsValid());

    m_center_texture = GlTexture::Load(center_texture_name);
    ASSERT1(m_center_texture.IsValid());
}

WidgetBackgroundStylized::WidgetBackgroundStylized (
    Resource<GlTexture> const &corner_texture,
    Resource<GlTexture> const &top_texture,
    Resource<GlTexture> const &left_texture,
    Resource<GlTexture> const &center_texture)
    :
    WidgetBackground()
{
    ASSERT1(corner_texture.IsValid());
    ASSERT1(top_texture.IsValid());
    ASSERT1(left_texture.IsValid());
    ASSERT1(center_texture.IsValid());

    m_corner_texture = corner_texture;
    m_top_texture = top_texture;
    m_left_texture = left_texture;
    m_center_texture = center_texture;
}

WidgetBackgroundStylized::~WidgetBackgroundStylized ()
{
}

WidgetBackground *WidgetBackgroundStylized::CreateClone () const
{
    return new WidgetBackgroundStylized(
        m_corner_texture,
        m_top_texture,
        m_left_texture,
        m_center_texture);
}

void WidgetBackgroundStylized::Draw (
    RenderContext const &render_context,
    ScreenCoordRect const &widget_screen_rect,
    ScreenCoordMargins const &frame_margins) const
{
    ASSERT1(m_corner_texture.IsValid());
    ASSERT1(m_top_texture.IsValid());
    ASSERT1(m_left_texture.IsValid());
    ASSERT1(m_center_texture.IsValid());
    ASSERT1(frame_margins.m_bottom_left[Dim::X] >= 0);
    ASSERT1(frame_margins.m_bottom_left[Dim::Y] >= 0);
    ASSERT1(frame_margins.m_top_right[Dim::X] >= 0);
    ASSERT1(frame_margins.m_top_right[Dim::Y] >= 0);

    // set up the measurements for the rectangles
    ScreenCoord horizontal[4] =
    {
        widget_screen_rect.Left(),
        widget_screen_rect.Left() + frame_margins.m_bottom_left[Dim::X],
        widget_screen_rect.Right() - frame_margins.m_top_right[Dim::X],
        widget_screen_rect.Right()
    };
    ScreenCoord vertical[4] =
    {
        widget_screen_rect.Bottom(),
        widget_screen_rect.Bottom() + frame_margins.m_bottom_left[Dim::Y],
        widget_screen_rect.Top() - frame_margins.m_top_right[Dim::Y],
        widget_screen_rect.Top()
    };

    // set up the rectangles
    ScreenCoordRect bottom_left(
        horizontal[0], vertical[0],
        horizontal[1], vertical[1]);
    ScreenCoordRect bottom_center(
        horizontal[1], vertical[0],
        horizontal[2], vertical[1]);
    ScreenCoordRect bottom_right(
        horizontal[2], vertical[0],
        horizontal[3], vertical[1]);
    ScreenCoordRect center_left(
        horizontal[0], vertical[1],
        horizontal[1], vertical[2]);
    ScreenCoordRect center_center(
        horizontal[1], vertical[1],
        horizontal[2], vertical[2]);
    ScreenCoordRect center_right(
        horizontal[2], vertical[1],
        horizontal[3], vertical[2]);
    ScreenCoordRect top_left(
        horizontal[0], vertical[2],
        horizontal[1], vertical[3]);
    ScreenCoordRect top_center(
        horizontal[1], vertical[2],
        horizontal[2], vertical[3]);
    ScreenCoordRect top_right(
        horizontal[2], vertical[2],
        horizontal[3], vertical[3]);

    // draw the rectangles - top row
    Render::DrawScreenRectTexture(
        render_context,
        **m_corner_texture,
        top_left,
        FloatSimpleTransform2( 1.0f,  1.0f, 0.0f, 0.0f));
    Render::DrawScreenRectTexture(
        render_context,
        **m_top_texture,
        top_center,
        FloatSimpleTransform2( 1.0f,  1.0f, 0.0f, 0.0f));
    Render::DrawScreenRectTexture(
        render_context,
        **m_corner_texture,
        top_right,
        FloatSimpleTransform2(-1.0f,  1.0f, 1.0f, 0.0f));
    // draw the rectangles - center row
    Render::DrawScreenRectTexture(
        render_context,
        **m_left_texture,
        center_left,
        FloatSimpleTransform2( 1.0f,  1.0f, 0.0f, 0.0f));
    Render::DrawScreenRectTexture(
        render_context,
        **m_center_texture,
        center_center,
        FloatSimpleTransform2( 1.0f,  1.0f, 0.0f, 0.0f));
    Render::DrawScreenRectTexture(
        render_context,
        **m_left_texture,
        center_right,
        FloatSimpleTransform2(-1.0f,  1.0f, 1.0f, 0.0f));
    // draw the rectangles - bottom row
    Render::DrawScreenRectTexture(
        render_context,
        **m_corner_texture,
        bottom_left,
        FloatSimpleTransform2( 1.0f, -1.0f, 0.0f, 1.0f));
    Render::DrawScreenRectTexture(
        render_context,
        **m_top_texture,
        bottom_center,
        FloatSimpleTransform2( 1.0f, -1.0f, 0.0f, 1.0f));
    Render::DrawScreenRectTexture(
        render_context,
        **m_corner_texture,
        bottom_right,
        FloatSimpleTransform2(-1.0f, -1.0f, 1.0f, 1.0f));
}

// ///////////////////////////////////////////////////////////////////////////
// WidgetBackgroundTextured9Way
// ///////////////////////////////////////////////////////////////////////////

WidgetBackgroundTextured9Way::WidgetBackgroundTextured9Way (
    Resource<GlTexture> const &top_left_texture,
    Resource<GlTexture> const &top_center_texture,
    Resource<GlTexture> const &top_right_texture,
    Resource<GlTexture> const &center_left_texture,
    Resource<GlTexture> const &center_center_texture,
    Resource<GlTexture> const &center_right_texture,
    Resource<GlTexture> const &bottom_left_texture,
    Resource<GlTexture> const &bottom_center_texture,
    Resource<GlTexture> const &bottom_right_texture)
    :
    WidgetBackground()
{
    ASSERT1(top_left_texture.IsValid());
    ASSERT1(top_center_texture.IsValid());
    ASSERT1(top_right_texture.IsValid());
    ASSERT1(center_left_texture.IsValid());
    ASSERT1(center_center_texture.IsValid());
    ASSERT1(center_right_texture.IsValid());
    ASSERT1(bottom_left_texture.IsValid());
    ASSERT1(bottom_center_texture.IsValid());
    ASSERT1(bottom_right_texture.IsValid());

    m_top_left_texture = top_left_texture;
    m_top_center_texture = top_center_texture;
    m_top_right_texture = top_right_texture;
    m_center_left_texture = center_left_texture;
    m_center_center_texture = center_center_texture;
    m_center_right_texture = center_right_texture;
    m_bottom_left_texture = bottom_left_texture;
    m_bottom_center_texture = bottom_center_texture;
    m_bottom_right_texture = bottom_right_texture;
}

WidgetBackgroundTextured9Way::~WidgetBackgroundTextured9Way ()
{
}

WidgetBackground *WidgetBackgroundTextured9Way::CreateClone () const
{
    return new WidgetBackgroundTextured9Way(
        m_top_left_texture,
        m_top_center_texture,
        m_top_right_texture,
        m_center_left_texture,
        m_center_center_texture,
        m_center_right_texture,
        m_bottom_left_texture,
        m_bottom_center_texture,
        m_bottom_right_texture);
}

void WidgetBackgroundTextured9Way::Draw (
    RenderContext const &render_context,
    ScreenCoordRect const &widget_screen_rect,
    ScreenCoordMargins const &frame_margins) const
{
    ASSERT1(frame_margins.m_bottom_left[Dim::X] >= 0);
    ASSERT1(frame_margins.m_bottom_left[Dim::Y] >= 0);
    ASSERT1(frame_margins.m_top_right[Dim::X] >= 0);
    ASSERT1(frame_margins.m_top_right[Dim::Y] >= 0);

    // set up the measurements for the rectangles
    ScreenCoord horizontal[4] =
    {
        widget_screen_rect.Left(),
        widget_screen_rect.Left() + frame_margins.m_bottom_left[Dim::X],
        widget_screen_rect.Right() - frame_margins.m_top_right[Dim::X],
        widget_screen_rect.Right()
    };
    ScreenCoord vertical[4] =
    {
        widget_screen_rect.Bottom(),
        widget_screen_rect.Bottom() + frame_margins.m_bottom_left[Dim::Y],
        widget_screen_rect.Top() - frame_margins.m_top_right[Dim::Y],
        widget_screen_rect.Top()
    };

    // set up the rectangles
    ScreenCoordRect bottom_left(
        horizontal[0], vertical[0],
        horizontal[1], vertical[1]);
    ScreenCoordRect bottom_center(
        horizontal[1], vertical[0],
        horizontal[2], vertical[1]);
    ScreenCoordRect bottom_right(
        horizontal[2], vertical[0],
        horizontal[3], vertical[1]);
    ScreenCoordRect center_left(
        horizontal[0], vertical[1],
        horizontal[1], vertical[2]);
    ScreenCoordRect center_center(
        horizontal[1], vertical[1],
        horizontal[2], vertical[2]);
    ScreenCoordRect center_right(
        horizontal[2], vertical[1],
        horizontal[3], vertical[2]);
    ScreenCoordRect top_left(
        horizontal[0], vertical[2],
        horizontal[1], vertical[3]);
    ScreenCoordRect top_center(
        horizontal[1], vertical[2],
        horizontal[2], vertical[3]);
    ScreenCoordRect top_right(
        horizontal[2], vertical[2],
        horizontal[3], vertical[3]);

    // draw the rectangles - top row
    Render::DrawScreenRectTexture(
        render_context,
        **m_top_left_texture,
        top_left);//, FloatSimpleTransform2(1.0f, 1.0f, 0.0f, 0.0f));
    Render::DrawScreenRectTexture(
        render_context,
        **m_top_center_texture,
        top_center);//, FloatSimpleTransform2(1.0f, 1.0f, 0.0f, 0.0f));
    Render::DrawScreenRectTexture(
        render_context,
        **m_top_right_texture,
        top_right);//, FloatSimpleTransform2(1.0f, 1.0f, 1.0f, 0.0f));
    // draw the rectangles - center row
    Render::DrawScreenRectTexture(
        render_context,
        **m_center_left_texture,
        center_left);//, FloatSimpleTransform2(1.0f, 1.0f, 0.0f, 0.0f));
    Render::DrawScreenRectTexture(
        render_context,
        **m_center_center_texture,
        center_center);//, FloatSimpleTransform2(1.0f, 1.0f, 0.0f, 0.0f));
    Render::DrawScreenRectTexture(
        render_context,
        **m_center_right_texture,
        center_right);//, FloatSimpleTransform2(1.0f, 1.0f, 1.0f, 0.0f));
    // draw the rectangles - bottom row
    Render::DrawScreenRectTexture(
        render_context,
        **m_bottom_left_texture,
        bottom_left);//, FloatSimpleTransform2(1.0f, 1.0f, 0.0f, 1.0f));
    Render::DrawScreenRectTexture(
        render_context,
        **m_bottom_center_texture,
        bottom_center);//, FloatSimpleTransform2(1.0f, 1.0f, 0.0f, 1.0f));
    Render::DrawScreenRectTexture(
        render_context,
        **m_bottom_right_texture,
        bottom_right);//, FloatSimpleTransform2(1.0f, 1.0f, 1.0f, 1.0f));
}

} // end of namespace Xrb
