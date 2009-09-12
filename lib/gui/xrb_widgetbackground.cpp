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
    ScreenCoordVector2 const &frame_margins) const
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

    m_texture =
        Singleton::ResourceLibrary().LoadPath<GlTexture>(
            GlTexture::Create,
            texture_path);
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
    ScreenCoordVector2 const &frame_margins) const
{
    ASSERT1(m_texture.IsValid());
    Render::DrawScreenRectTexture(
        render_context,
        *m_texture,
        widget_screen_rect);
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
    m_corner_texture =
        Singleton::ResourceLibrary().LoadPath<GlTexture>(
            GlTexture::Create,
            corner_texture_name);
    ASSERT1(m_corner_texture.IsValid());

    m_top_texture =
        Singleton::ResourceLibrary().LoadPath<GlTexture>(
            GlTexture::Create,
            top_texture_name);
    ASSERT1(m_top_texture.IsValid());

    m_left_texture =
        Singleton::ResourceLibrary().LoadPath<GlTexture>(
            GlTexture::Create,
            left_texture_name);
    ASSERT1(m_left_texture.IsValid());

    m_center_texture =
        Singleton::ResourceLibrary().LoadPath<GlTexture>(
            GlTexture::Create,
            center_texture_name);
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
    ScreenCoordVector2 const &frame_margins) const
{
    ASSERT1(m_corner_texture.IsValid());
    ASSERT1(m_top_texture.IsValid());
    ASSERT1(m_left_texture.IsValid());
    ASSERT1(m_center_texture.IsValid());
    ASSERT1(frame_margins[Dim::X] >= 0);
    ASSERT1(frame_margins[Dim::Y] >= 0);

    // set up the measurements for the rectangles
    ScreenCoord horizontal[4] =
    {
        widget_screen_rect.Left(),
        widget_screen_rect.Left() + frame_margins[Dim::X],
        widget_screen_rect.Right() - frame_margins[Dim::X],
        widget_screen_rect.Right()
    };
    ScreenCoord vertical[4] =
    {
        widget_screen_rect.Bottom(),
        widget_screen_rect.Bottom() + frame_margins[Dim::Y],
        widget_screen_rect.Top() - frame_margins[Dim::Y],
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
        *m_corner_texture,
        top_left,
        FloatSimpleTransform2( 1.0f,  1.0f, 0.0f, 0.0f));
    Render::DrawScreenRectTexture(
        render_context,
        *m_top_texture,
        top_center,
        FloatSimpleTransform2( 1.0f,  1.0f, 0.0f, 0.0f));
    Render::DrawScreenRectTexture(
        render_context,
        *m_corner_texture,
        top_right,
        FloatSimpleTransform2(-1.0f,  1.0f, 1.0f, 0.0f));
    // draw the rectangles - center row
    Render::DrawScreenRectTexture(
        render_context,
        *m_left_texture,
        center_left,
        FloatSimpleTransform2( 1.0f,  1.0f, 0.0f, 0.0f));
    Render::DrawScreenRectTexture(
        render_context,
        *m_center_texture,
        center_center,
        FloatSimpleTransform2( 1.0f,  1.0f, 0.0f, 0.0f));
    Render::DrawScreenRectTexture(
        render_context,
        *m_left_texture,
        center_right,
        FloatSimpleTransform2(-1.0f,  1.0f, 1.0f, 0.0f));
    // draw the rectangles - bottom row
    Render::DrawScreenRectTexture(
        render_context,
        *m_corner_texture,
        bottom_left,
        FloatSimpleTransform2( 1.0f, -1.0f, 0.0f, 1.0f));
    Render::DrawScreenRectTexture(
        render_context,
        *m_top_texture,
        bottom_center,
        FloatSimpleTransform2( 1.0f, -1.0f, 0.0f, 1.0f));
    Render::DrawScreenRectTexture(
        render_context,
        *m_corner_texture,
        bottom_right,
        FloatSimpleTransform2(-1.0f, -1.0f, 1.0f, 1.0f));
}

} // end of namespace Xrb
