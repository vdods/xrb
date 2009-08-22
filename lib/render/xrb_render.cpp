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

    // TODO: use glEnable(GL_LINE_SMOOTH).  also look at glLineWidth

    SetupTextureUnits(
        GL::GLTexture_OpaqueWhite().Handle(),
        render_context.MaskedColor(color),
        render_context.ColorBias());

    {
        FloatVector2 vertex_array[2] = { from, to };

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, vertex_array);
        glDrawArrays(GL_LINES, 0, 2);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
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
        render_context.ColorBias());

    glEnableClientState(GL_VERTEX_ARRAY);

    {
        FloatVector2 vertex_array[3] = { from, to, to+0.25f*(basis_y-basis_x) };

        glVertexPointer(2, GL_FLOAT, 0, vertex_array);
        glDrawArrays(GL_LINE_STRIP, 0, 3);
    }

    {
        FloatVector2 vertex_array[2] = { to-0.25f*(basis_y+basis_x), to };

        glVertexPointer(2, GL_FLOAT, 0, vertex_array);
        glDrawArrays(GL_LINE_STRIP, 0, 2);
    }

    glDisableClientState(GL_VERTEX_ARRAY);
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
        render_context.ColorBias());

    // convert the angle, which is in degrees, into radians for
    // computation in cos/sin's native units.
    angle = Math::Radians(angle);

    // draw each line
    FloatVector2 vertex = center + radius * FloatVector2(cos(angle), sin(angle));
    Float const angle_delta = 2.0f * static_cast<Float>(M_PI) / static_cast<Float>(vertex_count);

    // TODO: limit vertex_count to 32 (or so) and use a static array instead of new'ing.
    // or we could use a small static array and just call glDrawArrays multiple times.
    {
        FloatVector2 *vertex_array = new FloatVector2[vertex_count];
        for (Uint32 i = 0; i < vertex_count; ++i, angle += angle_delta)
            vertex_array[i] = center + radius * FloatVector2(cos(angle), sin(angle));

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, vertex_array);
        glDrawArrays(GL_LINE_LOOP, 0, vertex_count);
        glDisableClientState(GL_VERTEX_ARRAY);

        delete[] vertex_array;
    }
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
    ASSERT2(facet_count <= 30);

    DrawPolygon(render_context, center, radius, 0.0f, color, facet_count);
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

    // convert the angles, which are in degrees, into radians for
    // computation in cos/sin's native units.
    start_angle = Math::Radians(start_angle);
    end_angle = Math::Radians(end_angle);

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
        render_context.ColorBias());

    // TODO: is it possible to bound facet_count and use a static array?  (maybe not,
    // since the arc can be through a really high angle).  or we could use a small static
    // array and just call glDrawArrays multiple times.
    {
        Float const angle_delta = (end_angle - start_angle) / facet_count;
        Float angle = start_angle + angle_delta;

        FloatVector2 *vertex_array = new FloatVector2[facet_count+1];
        for (Uint32 i = 0; i < facet_count; ++i, angle += angle_delta)
            vertex_array[i] = center + radius * FloatVector2(cos(angle), sin(angle));
        // one more (there is 1 more vertex than facet)
        vertex_array[facet_count] = center + radius * FloatVector2(cos(angle), sin(angle));

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, vertex_array);
        glDrawArrays(GL_LINE_STRIP, 0, facet_count+1);
        glDisableClientState(GL_VERTEX_ARRAY);

        delete[] vertex_array;
    }
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
        render_context.ColorBias());

    {
        Sint16 vertex_coordinate_array[8] =
        {
            screen_rect.BottomLeft()[Dim::X], screen_rect.BottomLeft()[Dim::Y], 
            screen_rect.BottomRight()[Dim::X], screen_rect.BottomRight()[Dim::Y],
            screen_rect.TopLeft()[Dim::X], screen_rect.TopLeft()[Dim::Y],
            screen_rect.TopRight()[Dim::X], screen_rect.TopRight()[Dim::Y]
        };
/*
        ScreenCoordVector2 vertex_coordinate_array[4] =
        {
            ScreenCoordVector2(screen_rect.BottomLeft().m),
            ScreenCoordVector2(screen_rect.BottomRight().m),
            ScreenCoordVector2(screen_rect.TopLeft().m),
            ScreenCoordVector2(screen_rect.TopRight().m)
        };
*/

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_SHORT, 0, vertex_coordinate_array);
//         glVertexPointer(2, GL_INT, 0, vertex_coordinate_array);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
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
        render_context.ColorBias());

    {
        FloatVector2 texture_coordinate_array[4] =
        {
            FloatVector2((transformation * FloatVector2(0.0f, 1.0f)).m),
            FloatVector2((transformation * FloatVector2(1.0f, 1.0f)).m),
            FloatVector2((transformation * FloatVector2(0.0f, 0.0f)).m),
            FloatVector2((transformation * FloatVector2(1.0f, 0.0f)).m)
        };
        Sint16 vertex_coordinate_array[8] =
        {
            screen_rect.BottomLeft()[Dim::X], screen_rect.BottomLeft()[Dim::Y], 
            screen_rect.BottomRight()[Dim::X], screen_rect.BottomRight()[Dim::Y],
            screen_rect.TopLeft()[Dim::X], screen_rect.TopLeft()[Dim::Y],
            screen_rect.TopRight()[Dim::X], screen_rect.TopRight()[Dim::Y]
        };
/*
        ScreenCoordVector2 vertex_coordinate_array[4] =
        {
            ScreenCoordVector2(screen_rect.BottomLeft().m),
            ScreenCoordVector2(screen_rect.BottomRight().m),
            ScreenCoordVector2(screen_rect.TopLeft().m),
            ScreenCoordVector2(screen_rect.TopRight().m)
        };
*/

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(2, GL_SHORT, 0, vertex_coordinate_array);
//         glVertexPointer(2, GL_INT, 0, vertex_coordinate_array);
        glClientActiveTexture(GL_TEXTURE0);
        glTexCoordPointer(2, GL_FLOAT, 0, texture_coordinate_array);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
}

void Render::SetupTextureUnits (
    GLuint gltexture_handle, 
    Color const &color_mask,
    Color const &color_bias)
{
    // set up texture unit 0
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gltexture_handle);
    // due to limitations in the PowerVR MBX platform, (see
    // http://developer.apple.com/iphone/library/documentation/3DDrawing/Conceptual/OpenGLES_ProgrammingGuide/OpenGLESPlatforms/OpenGLESPlatforms.html ),
    // the value of GL_TEXTURE_ENV_COLOR must be the same for both texture
    // units.  but in texture unit 0, we don't actually use the value of
    // GL_TEXTURE_ENV_COLOR, we use the glColor value instead.
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, color_bias.m);
//     glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, color_mask.m); // old way
    glColor4fv(color_mask.m);

//     // the specific use of GL_COMBINE (see GL::Initialize) is not
//     // available on openGL ES implementations for all
//     // iphones/ipod-touches, so it's disabled for iphone builds.
// #if !defined(__IPHONEOS__)
    // set up texture unit 1
    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    // TODO -- assert that the opaque white texture is bound
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, color_bias.m);
// #endif // !defined(__IPHONEOS__)
}

} // end of namespace Xrb
