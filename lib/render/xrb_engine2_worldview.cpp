// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_worldview.cpp by Victor Dods, created 2004/06/04
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_worldview.hpp"

#include "xrb_engine2_entity.hpp"
#include "xrb_engine2_objectlayer.hpp"
#include "xrb_engine2_sprite.hpp"
#include "xrb_engine2_world.hpp"
#include "xrb_engine2_worldviewwidget.hpp"
#include "xrb_gl.hpp"
#include "xrb_math.hpp"
#include "xrb_render.hpp"
#include "xrb_screen.hpp"

namespace Xrb
{

Engine2::WorldView::WorldView (Engine2::WorldViewWidget *const parent_world_view_widget)
    :
    FloatTransform2(FloatTransform2::ms_identity, false),
    FrameHandler()
{
    ASSERT1(parent_world_view_widget != NULL);

    m_grid_line_type = GR_NO_DRAW;
    m_grid_number_base = 4;
    m_current_grid_scale = 1;

    m_zoom_factor = 0.0625f; // arbitrary
    m_min_zoom_factor = 0.0f; // (sort of) arbitrary
    m_max_zoom_factor = 1000000.0f; // arbitrary
    m_fade_distance_upper_limit = 100.0f; // arbitrary
    m_fade_distance_lower_limit = 5.0f; // arbitrary
    m_is_view_locked = false;
    m_is_gl_projection_matrix_in_use = false;
    m_draw_border_grid_lines = false;
    m_is_transform_scaling_based_upon_widget_radius = false;

    m_is_parallaxed_world_to_view_dirty = true;
    m_is_parallaxed_view_to_world_dirty = true;
    m_is_parallaxed_world_to_screen_dirty = true;
    m_is_parallaxed_screen_to_world_dirty = true;

    m_world = NULL;
    m_parent_world_view_widget = parent_world_view_widget;
    m_parent_world_view_widget->SetWorldView(this);
}

Engine2::WorldView::~WorldView ()
{
    // this is to ensure that Engine2::World::DetachWorldView was called
    ASSERT1(m_world == NULL);
}

FloatMatrix2 Engine2::WorldView::CompoundTransformation () const
{
    ASSERT1(m_parent_world_view_widget != NULL);
    return m_parent_world_view_widget->Transformation() * Transformation();
}

Float Engine2::WorldView::ViewDepth (Engine2::ObjectLayer const *object_layer) const
{
    ASSERT1(object_layer != NULL);
    return object_layer->ZDepth() - 1.0f / ZoomFactor();
}

Float Engine2::WorldView::ParallaxedViewRadius (Engine2::ObjectLayer const *object_layer) const
{
    ASSERT1(object_layer != NULL);
    ASSERT1(ViewDepth(object_layer) != object_layer->ZDepth());

    FloatMatrix2 world_to_screen(
        ParentWorldViewWidget()->Transformation() *
        Transformation());
    return
        CalculateViewRadius(
            world_to_screen.Inverse(),
            ParentWorldViewWidget()->ScreenRect(),
            Center())
        *
        ParallaxFactor(
            ViewDepth(MainObjectLayer()),
            object_layer->ZDepth());
}

FloatMatrix2 Engine2::WorldView::ParallaxedTransformation (
    FloatMatrix2 const &world_to_view,
    FloatMatrix2 const &view_to_whatever,
    Engine2::ObjectLayer const *object_layer) const
{
    if (object_layer == NULL)
        object_layer = GetWorld()->MainObjectLayer();

    ASSERT1(ViewDepth(object_layer) != object_layer->ZDepth());

    // compute the view transformation adjusted by the parallax factor
    // and then get the world-to-whatever transformation
    FloatMatrix2 world_to_whatever(world_to_view);
    world_to_whatever.Scale(
        1.0f / ParallaxFactor(ViewDepth(MainObjectLayer()), object_layer->ZDepth()));
    world_to_whatever *= view_to_whatever;

    return world_to_whatever;
}

Float Engine2::WorldView::MinorAxisRadius () const
{
    FloatVector2 minor_axis;
    if (m_parent_world_view_widget->Width() < m_parent_world_view_widget->Height())
        minor_axis =
            0.5f * FloatVector2(
                static_cast<Float>(m_parent_world_view_widget->Width()),
                0.0f);
    else
        minor_axis =
            0.5f * FloatVector2(
                0.0f,
                static_cast<Float>(m_parent_world_view_widget->Height()));
    return
        (ParallaxedScreenToWorld() * minor_axis -
         ParallaxedScreenToWorld() * FloatVector2::ms_zero).Length();
}

Float Engine2::WorldView::MajorAxisRadius () const
{
    FloatVector2 major_axis;
    if (m_parent_world_view_widget->Width() > m_parent_world_view_widget->Height())
        major_axis =
            0.5f * FloatVector2(
                static_cast<Float>(m_parent_world_view_widget->Width()),
                0.0f);
    else
        major_axis =
            0.5f * FloatVector2(
                0.0f,
                static_cast<Float>(m_parent_world_view_widget->Height()));
    return
        (ParallaxedScreenToWorld() * major_axis -
         ParallaxedScreenToWorld() * FloatVector2::ms_zero).Length();
}

Float Engine2::WorldView::CornerRadius () const
{
    FloatVector2 corner_vector(
        0.5f * static_cast<Float>(m_parent_world_view_widget->Width()),
        0.5f * static_cast<Float>(m_parent_world_view_widget->Height()));
    return
        (ParallaxedScreenToWorld() * corner_vector -
         ParallaxedScreenToWorld() * FloatVector2::ms_zero).Length();
}

void Engine2::WorldView::SetCenter (FloatVector2 const &position)
{
    if (!IsViewLocked())
    {
        SetTranslation(-position);
        // set the appropriate dirty bits
        DirtyAllParallaxedTransformations();
    }
}

void Engine2::WorldView::SetZoomFactor (Float zoom_factor)
{
    ASSERT1(zoom_factor > 0.0);

    if (zoom_factor < m_min_zoom_factor)
        zoom_factor = m_min_zoom_factor;

    if (zoom_factor > m_max_zoom_factor)
        zoom_factor = m_max_zoom_factor;

    if (!IsViewLocked())
    {
        m_zoom_factor = zoom_factor;
        // set the appropriate dirty bits
        DirtyAllParallaxedTransformations();
    }
}

void Engine2::WorldView::SetAngle (Float const angle)
{
    if (!IsViewLocked())
    {
        FloatTransform2::SetAngle(-angle);
        // set the appropriate dirty bits
        DirtyAllParallaxedTransformations();
    }
}

void Engine2::WorldView::SetMinZoomFactor (Float const min_zoom_factor)
{
    ASSERT1(min_zoom_factor >= 0.0f);
    // adjust the max zoom factor from the new min zoom factor
    if (m_max_zoom_factor < min_zoom_factor)
        m_max_zoom_factor = min_zoom_factor;
    // set the min zoom factor
    m_min_zoom_factor = min_zoom_factor;
    // attempt to use the current zoom factor
    SetZoomFactor(m_zoom_factor);
}

void Engine2::WorldView::SetMaxZoomFactor (Float const max_zoom_factor)
{
    ASSERT1(max_zoom_factor > 0.0f);
    // adjust the min zoom factor from the new max zoom factor
    if (m_min_zoom_factor > max_zoom_factor)
        m_min_zoom_factor = max_zoom_factor;
    // set the max zoom factor
    m_max_zoom_factor = max_zoom_factor;
    // attempt to use the current zoom factor
    SetZoomFactor(m_zoom_factor);
}

void Engine2::WorldView::SetIsTransformScalingBasedUponWidgetRadius (bool const is_transform_scaling_based_upon_widget_radius)
{
    if (m_is_transform_scaling_based_upon_widget_radius != is_transform_scaling_based_upon_widget_radius)
    {
        m_is_transform_scaling_based_upon_widget_radius = is_transform_scaling_based_upon_widget_radius;
        ParentWorldViewWidget()->SetIsTransformScalingBasedUponWidgetRadius(m_is_transform_scaling_based_upon_widget_radius);
    }
}

void Engine2::WorldView::MoveView (FloatVector2 const &delta_position)
{
    if (!IsViewLocked())
    {
        Translate(-delta_position);
        // set the appropriate dirty bits
        DirtyAllParallaxedTransformations();
    }
}

void Engine2::WorldView::ZoomView (Float const delta_zoom_factor)
{
    ASSERT1(delta_zoom_factor > 0.0);
    SetZoomFactor(m_zoom_factor * delta_zoom_factor);
}

void Engine2::WorldView::RotateView (Float const delta_angle)
{
    if (!IsViewLocked())
    {
        Rotate(-delta_angle);
        // set the appropriate dirty bits
        DirtyAllParallaxedTransformations();
    }
}

void Engine2::WorldView::DetachFromWorld ()
{
    ASSERT1(m_world != NULL);
    m_world->DetachWorldView(this);
    ASSERT1(m_world == NULL);
}

void Engine2::WorldView::Draw (RenderContext const &render_context)
{
    ASSERT1(m_parent_world_view_widget != NULL);
    ASSERT1(m_world != NULL && "You must call Engine2::World::AttachWorldView before anything happens");

    // create a RenderContext for the view to apply color masks to
    RenderContext view_render_context(render_context);

    // resets the draw info so that it can be accumulated
    // during this execution of Draw()
    m_draw_info.Reset();

    Float pixels_in_view_radius =
        0.5f * render_context.ClipRect().Size().StaticCast<Float>().Length();

    // vars which are used in the while loop which should only be
    // initialized once, before the loop.
    bool main_object_layer_has_been_drawn = false;
    FloatMatrix2 parallaxed_world_to_screen;
    // iterate through and draw all object layers from back to front
    for (World::ObjectLayerList::iterator
             it = GetWorld()->GetObjectLayerList().begin(),
             it_end = GetWorld()->GetObjectLayerList().end();
         it != it_end;
         ++it)
    {
        ObjectLayer *object_layer;
        Float layer_offset;
        Float distance_fade;
        Float parallaxed_view_radius;

        object_layer = *it;
        ASSERT1(object_layer != NULL);

        layer_offset = object_layer->ZDepth() - ViewDepth(MainObjectLayer());
        // only fade out layers in front of the main layer
        if (main_object_layer_has_been_drawn)
        {
            // calculate the transparency of close-up object layers
            // which are in front of the main object layer
            distance_fade =
                (layer_offset - m_fade_distance_lower_limit) /
                (m_fade_distance_upper_limit - m_fade_distance_lower_limit);
            // ensure distance_fade is within the range [0,1]
            if (distance_fade < 0.0f)
                distance_fade = 0.0f;
            else if (distance_fade > 1.0f)
                distance_fade = 1.0f;
        }
        else
        {
            distance_fade = 1.0f;
        }

        // only draw non-transparent layers which are in front of the view
        if (layer_offset > 0.0f && distance_fade > 0.0f)
        {
            // set up the GL projection matrix for drawing this object layer
            PushParallaxedGLProjectionMatrix(render_context, object_layer);

            // apply the distance fade transparency to the color mask
            view_render_context.ColorMask() = render_context.ColorMask();
            view_render_context.ApplyAlphaMaskToColorMask(distance_fade);

            // if this is the main layer
            if (object_layer == GetWorld()->MainObjectLayer())
            {
                // set the drawn flag
                main_object_layer_has_been_drawn = true;
                // use the function which caches the result to calculate
                // the world-to-screen transformation
                parallaxed_world_to_screen = ParallaxedWorldToScreen();
                // if indicated, draw the grid lines before the main layer
                if (m_grid_line_type == GR_BELOW_MAIN_LAYER)
                    DrawGridLines(view_render_context);
            }
            else
            {
                // calculate the world-to-screen transformation normally
                parallaxed_world_to_screen =
                    ParallaxedTransformation(
                        Transformation(),
                        ParentWorldViewWidget()->Transformation(),
                        object_layer);
            }

            // calculate the parallaxed view radius for this layer
            parallaxed_view_radius = ParallaxedViewRadius(object_layer);

            // draw the contents of the object layer (this does opaque and then
            // back-to-front transparent rendering for correct z-depth order).
            m_draw_info.m_drawn_opaque_object_count =
                object_layer->Draw(
                    view_render_context,
                    parallaxed_world_to_screen,
                    pixels_in_view_radius,
                    Center(),
                    parallaxed_view_radius,
                    &m_transparent_object_vector);

            // if indicated, draw the grid lines after the main layer
            if (object_layer == GetWorld()->MainObjectLayer() &&
                m_grid_line_type == GR_ABOVE_MAIN_LAYER)
            {
                DrawGridLines(view_render_context);
            }

            // we're done with the GL projection matrix for this object layer
            PopGLProjectionMatrix();
        }
    }

    // draw the main object layer's border above everything
    if (m_draw_border_grid_lines)
    {
        PushParallaxedGLProjectionMatrix(
            render_context,
            MainObjectLayer());

        // get the parallaxed view radius for the main object layer
        Float parallaxed_view_radius = ParallaxedViewRadius(MainObjectLayer());

        DrawGridLineSet(
            render_context,
            0,
            true,
            MainObjectLayer()->IsWrapped(),
            Center(),
            parallaxed_view_radius,
            Color(1.0f, 1.0f, 0.0f, 1.0f));

        PopGLProjectionMatrix();
    }
}

bool Engine2::WorldView::ProcessKeyEvent (EventKey const *const e)
{
    return false;
}

bool Engine2::WorldView::ProcessMouseButtonEvent (EventMouseButton const *const e)
{
    return false;
}

bool Engine2::WorldView::ProcessMouseWheelEvent (EventMouseWheel const *const e)
{
    return false;
}

bool Engine2::WorldView::ProcessMouseMotionEvent (EventMouseMotion const *const e)
{
    return false;
}

Engine2::ObjectLayer *Engine2::WorldView::MainObjectLayer () const
{
    return GetWorld()->MainObjectLayer();
}

Float Engine2::WorldView::GridScaleUnit (Uint32 const grid_scale) const
{
    return 0.5f * MainObjectLayer()->SideLength() /
           Math::Pow(static_cast<Float>(m_grid_number_base), static_cast<Float>(grid_scale));
}

FloatMatrix2 const &Engine2::WorldView::ParallaxedWorldToWorldView () const
{
    if (m_is_parallaxed_world_to_view_dirty)
    {
        m_parallaxed_world_to_view =
            ParallaxedTransformation(
                Transformation(),
                FloatMatrix2::ms_identity,
                NULL);
        m_is_parallaxed_world_to_view_dirty = false;
    }

    return m_parallaxed_world_to_view;
}

FloatMatrix2 const &Engine2::WorldView::ParallaxedWorldViewToWorld () const
{
    if (m_is_parallaxed_view_to_world_dirty)
    {
        m_parallaxed_view_to_world = ParallaxedWorldToWorldView().Inverse();
        m_is_parallaxed_view_to_world_dirty = false;
    }

    return m_parallaxed_view_to_world;
}

FloatMatrix2 const &Engine2::WorldView::ParallaxedWorldToScreen () const
{
    if (m_is_parallaxed_world_to_screen_dirty)
    {
        m_parallaxed_world_to_screen =
            ParallaxedTransformation(
                Transformation(),
                ParentWorldViewWidget()->Transformation(),
                NULL);
        m_is_parallaxed_world_to_screen_dirty = false;
    }

    return m_parallaxed_world_to_screen;
}

FloatMatrix2 const &Engine2::WorldView::ParallaxedScreenToWorld () const
{
    if (m_is_parallaxed_screen_to_world_dirty)
    {
        m_parallaxed_screen_to_world =
            ParallaxedWorldToScreen().Inverse();
        m_is_parallaxed_screen_to_world_dirty = false;
    }

    return m_parallaxed_screen_to_world;
}

void Engine2::WorldView::DrawGridLines (RenderContext const &render_context)
{
    // early out if we don't even want the lines
    if (m_grid_line_type == GR_NO_DRAW)
        return;

    bool is_wrapped = MainObjectLayer()->IsWrapped();
    Float view_radius = ParallaxedViewRadius(MainObjectLayer());

    // draw the minor grid
    DrawGridLineSet(
        render_context,
        m_current_grid_scale+1,
        false,
        is_wrapped,
        Center(),
        view_radius,
        Color(0.4f, 0.4f, 0.4f, 1.0f));
    // draw the major grid
    DrawGridLineSet(
        render_context,
        m_current_grid_scale,
        false,
        is_wrapped,
        Center(),
        view_radius,
        Color(0.7f, 0.7f, 0.7f, 1.0f));
}

void Engine2::WorldView::DrawGridLineSet (
    RenderContext const &render_context,
    Uint32 const grid_scale,
    bool const is_border_grid,
    bool const is_wrapped,
    FloatVector2 const &view_center,
    Float const view_radius,
    Color color)
{
    // constants which control the grid line distance fading
    Float const fade_scale_upper_limit = 20.0;
    Float const fade_scale_lower_limit = 3.0;

    Float grid_scale_unit = GridScaleUnit(grid_scale);

    // calculate the fading as the grid becomes too small to be useful
    Float transformed_grid_scale_unit =
        ((ParallaxedWorldToScreen() * FloatVector2(grid_scale_unit, 0.0)) -
         (ParallaxedWorldToScreen() * FloatVector2::ms_zero)).Length();
    Float distance_fade =
        (transformed_grid_scale_unit - fade_scale_lower_limit) /
        (fade_scale_upper_limit - fade_scale_lower_limit);
    color[Dim::A] *= Min(Max(distance_fade, 0.0f), 1.0f);

    // only bother drawing the grid lines if the alpha isn't 0
    if (color[Dim::A] == 0.0f)
        return;

    Sint32 x_start;
    Sint32 x_stop;
    Sint32 y_start;
    Sint32 y_stop;
    Float view_limit;
    Float half_side_length = 0.5f * MainObjectLayer()->SideLength();

    view_limit = view_center[Dim::X] - view_radius;
    if (!is_wrapped && view_limit < -half_side_length)
        view_limit = -half_side_length;
    x_start = static_cast<Sint32>(Math::Floor(view_limit / grid_scale_unit));

    view_limit = view_center[Dim::X] + view_radius;
    if (!is_wrapped && view_limit > half_side_length)
        view_limit = half_side_length;
    x_stop = static_cast<Sint32>(Math::Ceiling(view_limit / grid_scale_unit));

    view_limit = view_center[Dim::Y] - view_radius;
    if (!is_wrapped && view_limit < -half_side_length)
        view_limit = -half_side_length;
    y_start = static_cast<Sint32>(Math::Floor(view_limit / grid_scale_unit));

    view_limit = view_center[Dim::Y] + view_radius;
    if (!is_wrapped && view_limit > half_side_length)
        view_limit = half_side_length;
    y_stop = static_cast<Sint32>(Math::Ceiling(view_limit / grid_scale_unit));

    // draw the vertical grid lines, skipping the even indexed ones if
    // is_border_grid is true
    for (Sint32 x = x_start; x <= x_stop; ++x)
        if (!is_border_grid || (x & 1))
            Render::DrawLine(
                render_context,
                FloatVector2(grid_scale_unit*x, grid_scale_unit*y_start),
                FloatVector2(grid_scale_unit*x, grid_scale_unit*y_stop),
                color);

    // draw the horizontal grid lines, skipping the even indexed ones if
    // is_border_grid is true
    for (Sint32 y = y_start; y <= y_stop; ++y)
        if (!is_border_grid || (y & 1))
            Render::DrawLine(
                render_context,
                FloatVector2(grid_scale_unit*x_start, grid_scale_unit*y),
                FloatVector2(grid_scale_unit*x_stop, grid_scale_unit*y),
                color);
}

Float Engine2::WorldView::CalculateViewRadius (
    FloatMatrix2 const &screen_to_world,
    ScreenCoordRect const &view_rect,
    FloatVector2 const &view_center) const
{
    Float retval;
    Float distance_squared;
    FloatRect cast_view_rect(view_rect.StaticCast<Float>());

    // calculate the radius of the view rect
    distance_squared =
        (screen_to_world * cast_view_rect.TopLeft() -
        view_center).LengthSquared();
    retval = distance_squared;

    distance_squared =
        (screen_to_world * cast_view_rect.TopRight() -
        view_center).LengthSquared();
    if (distance_squared > retval)
        retval = distance_squared;

    distance_squared =
        (screen_to_world * cast_view_rect.BottomLeft() -
        view_center).LengthSquared();
    if (distance_squared > retval)
        retval = distance_squared;

    distance_squared =
        (screen_to_world * cast_view_rect.BottomRight() -
        view_center).LengthSquared();
    if (distance_squared > retval)
        retval = distance_squared;

    return Math::Sqrt(retval);
}

void Engine2::WorldView::PushParallaxedGLProjectionMatrix (
    RenderContext const &render_context,
    Engine2::ObjectLayer const *const object_layer)
{
    ASSERT1(!m_is_gl_projection_matrix_in_use);
    m_is_gl_projection_matrix_in_use = true;

    glMatrixMode(GL_PROJECTION);
    // save off the Widget-created projection matrix
    glPushMatrix();
    // start our matrix from scratch
    glLoadIdentity();

    // viewport perspective correction - this effectively takes
    // the place of the view-to-screen transform.
    if (m_is_transform_scaling_based_upon_widget_radius)
    {
        Float viewport_radius = render_context.ClipRect().Size().StaticCast<Float>().Length();
        glScalef(
            viewport_radius / render_context.ClipRect().Size()[Dim::X],
            viewport_radius / render_context.ClipRect().Size()[Dim::Y],
            1.0f);
    }
    else
    {
        Float min_viewport_size =
            static_cast<Float>(
                Min(render_context.ClipRect().Size()[Dim::X],
                    render_context.ClipRect().Size()[Dim::Y]));
        glScalef(
            min_viewport_size / render_context.ClipRect().Size()[Dim::X],
            min_viewport_size / render_context.ClipRect().Size()[Dim::Y],
            1.0f);
    }

    // perform the view parallaxing transformation
    Float parallax_factor =
        ParallaxFactor(
            ViewDepth(MainObjectLayer()),
            object_layer->ZDepth());
    ASSERT1(parallax_factor != 0.0f);
    glScalef(
        1.0f / parallax_factor,
        1.0f / parallax_factor,
        1.0f);

    // perform the world-to-view transformation
    ASSERT1(ScaleFactors()[Dim::X] == 1.0f);
    ASSERT1(ScaleFactors()[Dim::Y] == 1.0f);
    glRotatef(FloatTransform2::Angle(), 0.0f, 0.0f, 1.0f);
    glTranslatef(
        Translation()[Dim::X],
        Translation()[Dim::Y],
        0.0f);
}

void Engine2::WorldView::PopGLProjectionMatrix ()
{
    ASSERT1(m_is_gl_projection_matrix_in_use);
    // restore the projection matrix
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    m_is_gl_projection_matrix_in_use = false;
}

void Engine2::WorldView::DirtyAllParallaxedTransformations ()
{
    m_is_parallaxed_world_to_view_dirty = true;
    m_is_parallaxed_view_to_world_dirty = true;
    m_is_parallaxed_world_to_screen_dirty = true;
    m_is_parallaxed_screen_to_world_dirty = true;
}

} // end of namespace Xrb
