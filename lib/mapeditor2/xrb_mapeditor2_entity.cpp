// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_entity.cpp by Victor Dods, created 2005/07/03
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_mapeditor2_entity.hpp"

#include "xrb_render.hpp"

namespace Xrb
{

Color MapEditor2::Entity::ms_unselected_metrics_color(0.9f, 0.0f, 0.0f, 1.0f);
Color MapEditor2::Entity::ms_selected_metrics_color(1.0f, 0.5f, 0.5f, 1.0f);

void MapEditor2::Entity::DrawMetrics (
    DrawData const &draw_data,
    Float const alpha_mask,
    MetricMode const metric_mode)
{
    Color color(IsSelected() ?
                SelectedMetricsColor() :
                UnselectedMetricsColor());
    color[Dim::A] *= alpha_mask;

    // call Object's metric drawing function for the common stuff
    Object::DrawMetrics(draw_data, alpha_mask, metric_mode);

    switch (metric_mode)
    {
        case MM_TRANSFORMATION:
            // Object handles all of this stuff
            break;

        case MM_LINEAR_VELOCITY:
            // draw the linear velocity vector as an arrow
            Render::DrawArrow(
                draw_data.GetRenderContext(),
                Translation(),
                Translation() + Velocity(),
                color);
            // draw the little vertical bar across the origin
            Render::DrawLine(
                draw_data.GetRenderContext(),
                Translation() + FloatVector2(0.0f, 0.125f*VisibleRadius()),
                Translation() - FloatVector2(0.0f, 0.125f*VisibleRadius()),
                color);
            // draw the little horizontal bar across the origin
            Render::DrawLine(
                draw_data.GetRenderContext(),
                Translation() + FloatVector2(0.125f*VisibleRadius(), 0.0f),
                Translation() - FloatVector2(0.125f*VisibleRadius(), 0.0f),
                color);
            break;

        case MM_ANGULAR_VELOCITY:
            // draw the two angle limit lines
            Render::DrawLine(
                draw_data.GetRenderContext(),
                Translation(),
                Translation() + 1.5f*VisibleRadius()*FloatVector2(1.0f, 0.0f),
                color);
            Render::DrawLine(
                draw_data.GetRenderContext(),
                Translation(),
                Translation() +
                1.5f*VisibleRadius()*FloatVector2(
                    Math::Cos(AngularVelocity()),
                    Math::Sin(AngularVelocity())),
                color);
            // draw the arc connecting the two
            Render::DrawCircularArc(
                draw_data.GetRenderContext(),
                draw_data.Transformation(),
                Translation(),
                1.25f*VisibleRadius(),
                0.0f,
                AngularVelocity(),
                color);
            // draw the little vertical bar across the origin
            Render::DrawLine(
                draw_data.GetRenderContext(),
                Translation() + FloatVector2(0.0f, 0.125f*VisibleRadius()),
                Translation() - FloatVector2(0.0f, 0.125f*VisibleRadius()),
                color);
            break;

        case MM_POLYGONS:
        case MM_VERTICES:
            // Compound handles this
            break;
            
        default:
            ASSERT0(false && "Invalid metric mode");
            break;
    }
}

Color const &MapEditor2::Entity::UnselectedMetricsColor () const
{
    return Entity::ms_unselected_metrics_color;
}

Color const &MapEditor2::Entity::SelectedMetricsColor () const
{
    return Entity::ms_selected_metrics_color;
}

void MapEditor2::Entity::ObjectSelectionSetScaleVelocity (
    Float const scale_factor,
    FloatVector2 transformation_origin,
    TransformationMode const transformation_mode)
{
    if (transformation_mode == TM_EACH_SELECTED_OBJECT_ORIGIN)
        transformation_origin = Translation();

    // get the vector based on the given origin
    m_velocity -= transformation_origin - Translation();
    // scale it
    m_velocity *= scale_factor;
    // translate it back by the origin
    m_velocity += transformation_origin - Translation();
}

void MapEditor2::Entity::ObjectSelectionSetRotateVelocity (
    Float const angle_delta,
    FloatVector2 transformation_origin,
    TransformationMode const transformation_mode,
    FloatMatrix2 const &rotation_transformation)
{
    if (transformation_mode == TM_EACH_SELECTED_OBJECT_ORIGIN)
        transformation_origin = Translation();

    // get the vector based on the given origin
    m_velocity -= transformation_origin - Translation();
    // rotate it
    m_velocity *= rotation_transformation;
    // translate it back by the origin
    m_velocity += transformation_origin - Translation();
}

} // end of namespace Xrb
