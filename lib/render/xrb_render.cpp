// ///////////////////////////////////////////////////////////////////////////
// xrb_render.cpp by Victor Dods, created 2004/07/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_render.hpp"

#include "xrb_gl.hpp"
#include "xrb_gltexture.hpp"
#include "xrb_math.hpp"

namespace Xrb
{

void Render::DrawLine (
    RenderContext const &render_context,
    FloatVector2 const &from,
    FloatVector2 const &to,
    Color const &color)
{
    if (render_context.MaskAndBiasWouldResultInNoOp(color[Dim::A]))
        return;
        
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    SetupTextureUnits(
        GL::GLTexture_OpaqueWhite().Handle(), 
        render_context.MaskedColor(color), 
        render_context.BiasColor());

    glBegin(GL_LINES);
        glVertex2fv(from.m);
        glVertex2fv(to.m);
    glEnd();
}

void Render::DrawArrow (
    RenderContext const &render_context,
    FloatVector2 const &from,
    FloatVector2 const &to,
    Color const &color)
{
    if (render_context.MaskAndBiasWouldResultInNoOp(color[Dim::A]))
        return;
        
    // don't draw anything if the length is 0
    if (to == from)
        return;

    FloatVector2 basis_x(to - from);
    FloatVector2 basis_y(PerpendicularVector2(basis_x));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    SetupTextureUnits(
        GL::GLTexture_OpaqueWhite().Handle(), 
        render_context.MaskedColor(color), 
        render_context.BiasColor());

    glBegin(GL_LINE_STRIP);
        glVertex2fv(from.m);
        glVertex2fv(to.m);
        glVertex2fv((to+0.25f*(basis_y-basis_x)).m);
    glEnd();

    glBegin(GL_LINE_STRIP);
        glVertex2fv((to-0.25f*(basis_y+basis_x)).m);
        glVertex2fv(to.m);
    glEnd();
}

void Render::DrawPolygon (
    RenderContext const &render_context,
    FloatVector2 const &center,
    Float const radius,
    Float angle,
    Color const &color,
    Uint32 const vertex_count)
{
    // a polygon with less than 3 vertices is degenerate
    ASSERT1(vertex_count >= 3);

    if (render_context.MaskAndBiasWouldResultInNoOp(color[Dim::A]))
        return;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    SetupTextureUnits(
        GL::GLTexture_OpaqueWhite().Handle(), 
        render_context.MaskedColor(color), 
        render_context.BiasColor());

    // convert the angle which is in degrees, into radians for
    // computation in cos/sin's native units.
    angle = Math::Radians(angle);

    // draw each line
    FloatVector2 vertex = center + radius * FloatVector2(cos(angle), sin(angle));
    Float const angle_delta = 2.0f * static_cast<Float>(M_PI) / static_cast<Float>(vertex_count);

    glBegin(GL_LINE_LOOP);
    glVertex2fv(vertex.m);

    for (Uint32 i = 0; i < vertex_count; ++i)
    {
        angle += angle_delta;
        vertex = center + radius * FloatVector2(cos(angle), sin(angle));
        glVertex2fv(vertex.m);
    }

    glEnd();
}

void Render::DrawCircle (
    RenderContext const &render_context,
    FloatMatrix2 const &transformation,
    FloatVector2 const &center,
    Float const radius,
    Color const &color)
{
    if (render_context.MaskAndBiasWouldResultInNoOp(color[Dim::A]))
        return;

    // find out how large the radius is in pixels
    Float pixel_radius =
        Max((transformation * FloatVector2(radius, 0.0f) -
             transformation * FloatVector2::ms_zero).Length(),
            (transformation * FloatVector2(0.0f, radius) -
             transformation * FloatVector2::ms_zero).Length());
    // figure out how many lines there should be
    Float const radius_limit_upper = 100.0f;
    Float const radius_limit_lower = 2.0f;
    Float const tesselation_limit_upper = 30.0f;
    Float const tesselation_limit_lower = 6.0f;

    Uint32 facet_count;
    if (pixel_radius <= radius_limit_lower)
        facet_count = static_cast<Uint32>(tesselation_limit_lower);
    else if (pixel_radius >= radius_limit_upper)
        facet_count = static_cast<Uint32>(tesselation_limit_upper);
    else
    {
        Float x =
            (pixel_radius - radius_limit_lower) /
            (radius_limit_upper - radius_limit_lower);
        facet_count =
            static_cast<Uint32>(
                tesselation_limit_upper * x +
                tesselation_limit_lower * (1.0f - x));
    }

    ASSERT1(facet_count >= 6);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    SetupTextureUnits(
        GL::GLTexture_OpaqueWhite().Handle(), 
        render_context.MaskedColor(color), 
        render_context.BiasColor());

    // draw each line
    FloatVector2 vertex;
    vertex = center + FloatVector2(radius, 0.0);
    Float const angle_delta = 2.0f * static_cast<Float>(M_PI) / facet_count;
    Float angle = angle_delta;

    glBegin(GL_LINE_LOOP);
    glVertex2fv(vertex.m);

    for (Uint32 i = 0; i < facet_count; ++i)
    {
        vertex = center + radius * FloatVector2(cos(angle), sin(angle));
        glVertex2fv(vertex.m);
        angle += angle_delta;
    }

    glEnd();
}

void Render::DrawCircularArc (
    RenderContext const &render_context,
    FloatMatrix2 const &transformation,
    FloatVector2 const &center,
    Float const radius,
    Float start_angle,
    Float end_angle,
    Color const &color)
{
    if (render_context.MaskAndBiasWouldResultInNoOp(color[Dim::A]))
        return;

    // don't draw anything if the arc is 0 radians
    if (start_angle == end_angle)
        return;

    // make sure the start_angle is lower than the end_angle
    if (start_angle > end_angle)
    {
        Float temp = start_angle;
        start_angle = end_angle;
        end_angle = temp;
    }
    ASSERT1(end_angle > start_angle);

    // find out how large the radius is in pixels
    Float pixel_radius =
        (transformation * FloatVector2(radius, 0.0f) -
         transformation * FloatVector2::ms_zero).Length();
    // figure out the portion of a full circle this angle is
    Float arc_portion = (end_angle - start_angle) / (2.0f * static_cast<Float>(M_PI));
    // figure out how many lines there should be
    Float const radius_limit_upper = 100.0f;
    Float const radius_limit_lower = 2.0f;
    Float const tesselation_limit_upper = 30.0f;
    Float const tesselation_limit_lower = 6.0f;

    Uint32 facet_count;
    if (pixel_radius <= radius_limit_lower)
        facet_count =
            static_cast<Uint32>(Math::Ceiling(tesselation_limit_lower * arc_portion));
    else if (pixel_radius >= radius_limit_upper)
        facet_count =
            static_cast<Uint32>(Math::Ceiling(tesselation_limit_upper * arc_portion));
    else
    {
        Float x = (pixel_radius - radius_limit_lower) /
                  (radius_limit_upper - radius_limit_lower);
        facet_count =
            static_cast<Uint32>(
                Math::Ceiling(arc_portion *
                              (tesselation_limit_upper * x +
                               tesselation_limit_lower * (1.0f - x))));
    }

    if (facet_count < 1)
        facet_count = 1;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    SetupTextureUnits(
        GL::GLTexture_OpaqueWhite().Handle(), 
        render_context.MaskedColor(color), 
        render_context.BiasColor());

    // draw each line
    FloatVector2 vertex;
    vertex = center + radius * FloatVector2(Math::Cos(start_angle), Math::Sin(start_angle));
    Float const angle_delta = (end_angle - start_angle) / facet_count;
    Float angle = start_angle + angle_delta;

    glBegin(GL_LINE_STRIP);
    glVertex2fv(vertex.m);

    for (Uint32 i = 0; i < facet_count; ++i)
    {
        vertex = center + radius * FloatVector2(Math::Cos(angle), Math::Sin(angle));
        glVertex2fv(vertex.m);
        angle += angle_delta;
    }

    glEnd();
}

void Render::DrawScreenRect (
    RenderContext const &render_context,
    Color const &color,
    ScreenCoordRect const &screen_rect)
{
    if (render_context.MaskAndBiasWouldResultInNoOp(color[Dim::A]))
        return;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    SetupTextureUnits(
        GL::GLTexture_OpaqueWhite().Handle(), 
        render_context.MaskedColor(color), 
        render_context.BiasColor());

    glBegin(GL_QUADS);
        glVertex2iv(screen_rect.GetTopLeft().m);
        glVertex2iv(screen_rect.BottomLeft().m);
        glVertex2iv(screen_rect.BottomRight().m);
        glVertex2iv(screen_rect.GetTopRight().m);
    glEnd();
}

void Render::DrawScreenRectTexture (
    RenderContext const &render_context,
    GLTexture const *const gl_texture,
    ScreenCoordRect const &screen_rect,
    FloatSimpleTransform2 const &transformation)
{
    if (render_context.MaskAndBiasWouldResultInNoOp())
        return;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    SetupTextureUnits(
        gl_texture->Handle(), 
        render_context.ColorMask(), 
        render_context.BiasColor());

    glBegin(GL_QUADS);
        glTexCoord2fv((transformation * FloatVector2(0.0f, 0.0f)).m);
        glVertex2iv(screen_rect.GetTopLeft().m);

        glTexCoord2fv((transformation * FloatVector2(0.0f, 1.0f)).m);
        glVertex2iv(screen_rect.BottomLeft().m);

        glTexCoord2fv((transformation * FloatVector2(1.0f, 1.0f)).m);
        glVertex2iv(screen_rect.BottomRight().m);

        glTexCoord2fv((transformation * FloatVector2(1.0f, 0.0f)).m);
        glVertex2iv(screen_rect.GetTopRight().m);
    glEnd();
}

void Render::SetupTextureUnits (
    GLuint gltexture_handle, 
    Color const &mask_color,
    Color const &bias_color)
{
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gltexture_handle);
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, mask_color.m);

    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
    // TODO -- assert that the all-white texture is bound
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, bias_color.m);
}

} // end of namespace Xrb
