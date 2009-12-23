// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_worldviewwidget.cpp by Victor Dods, created 2004/06/12
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_worldviewwidget.hpp"

#include "xrb_engine2_world.hpp"
#include "xrb_engine2_worldview.hpp"

namespace Xrb {
namespace Engine2 {

WorldViewWidget::WorldViewWidget (
    ContainerWidget *const parent,
    std::string const &name)
    :
    Widget(parent, name)
{
    ASSERT1(parent != NULL);

    m_transform = FloatSimpleTransform2::ms_identity;
    m_is_transform_scaling_based_upon_widget_radius = false;

    m_draw_focus_frame = true;
    m_accepts_focus = true;

    m_world_view = NULL;
    // make sure the transformation is adjusted according to the new position
    // and size
    ComputeTransform();

    SetBackground(NULL);
}

WorldViewWidget::~WorldViewWidget ()
{
    if (m_world_view != NULL)
    {
        m_world_view->DetachFromWorld();
        ASSERT1(m_world_view->GetWorld() == NULL);
    }
    Delete(m_world_view);
}

void WorldViewWidget::SetWorldView (WorldView *const world_view)
{
    // if the given world view is already attached, do nothing.
    if (m_world_view == world_view)
        return;

    if (m_world_view != NULL)
    {
        m_world_view->DetachFromWorld();
        ASSERT1(m_world_view->GetWorld() == NULL);
    }
    Delete(m_world_view);
    m_world_view = world_view;
    SetIsTransformScalingBasedUponWidgetRadius(m_world_view->IsTransformScalingBasedUponWidgetRadius());
}

void WorldViewWidget::SetIsTransformScalingBasedUponWidgetRadius (bool const is_transform_scaling_based_upon_widget_radius)
{
    if (m_is_transform_scaling_based_upon_widget_radius != is_transform_scaling_based_upon_widget_radius)
    {
        m_is_transform_scaling_based_upon_widget_radius = is_transform_scaling_based_upon_widget_radius;
        if (m_world_view != NULL)
            m_world_view->SetIsTransformScalingBasedUponWidgetRadius(m_is_transform_scaling_based_upon_widget_radius);
        ComputeTransform();
    }
}

void WorldViewWidget::Draw (RenderContext const &render_context) const
{
    // draw the view first
    if (m_world_view != NULL)
        m_world_view->Draw(render_context);

    // the WorldViewWidget may have child widgets (like HUD labels or something)
    // so draw them here, on top of the WorldView
    Widget::Draw(render_context);

    // draw the focus frame on top
    DrawFocusFrame(render_context);
}

void WorldViewWidget::HandleFrame ()
{
    // the WorldView's frame handler
    if (m_world_view != NULL)
        m_world_view->ProcessFrame(FrameTime());
}

void WorldViewWidget::MoveBy (ScreenCoordVector2 const &delta)
{
    Widget::MoveBy(delta);
    ComputeTransform();
}

ScreenCoordVector2 WorldViewWidget::Resize (ScreenCoordVector2 const &size)
{
    Widget::Resize(size);
    ComputeTransform();
    return Size();
}

bool WorldViewWidget::ProcessKeyEvent (EventKey const *const e)
{
    if (m_world_view != NULL)
        return m_world_view->ProcessKeyEvent(e);
    else
        return false;
}

bool WorldViewWidget::ProcessMouseButtonEvent (EventMouseButton const *const e)
{
    if (m_world_view != NULL)
        return m_world_view->ProcessMouseButtonEvent(e);
    else
        return false;
}

bool WorldViewWidget::ProcessMouseWheelEvent (EventMouseWheel const *const e)
{
    if (m_world_view != NULL)
        return m_world_view->ProcessMouseWheelEvent(e);
    else
        return false;
}

bool WorldViewWidget::ProcessMouseMotionEvent (EventMouseMotion const *const e)
{
    if (m_world_view != NULL)
        return m_world_view->ProcessMouseMotionEvent(e);
    else
        return false;
}

void WorldViewWidget::HandleFocus ()
{
    if (m_world_view != NULL)
        m_world_view->HandleFocus();
}

void WorldViewWidget::HandleUnfocus ()
{
    if (m_world_view != NULL)
        m_world_view->HandleUnfocus();
}

void WorldViewWidget::DrawFocusFrame (RenderContext const &render_context) const
{
    // if we don't want to draw the focus frame, early out
    if (!m_draw_focus_frame)
        return;

    // somehow draw a frame around the inside of the widget
}

void WorldViewWidget::ComputeTransform ()
{
    // reset the transform
    m_transform = FloatSimpleTransform2::ms_identity;
    // scale 1 unit up to the smaller of width or height.
    // the 0.5f factor is because normally the viewport maps to the rect
    // (-1, -1) to (1, 1)
    if (m_is_transform_scaling_based_upon_widget_radius)
        m_transform.Scale(0.5f * Size().StaticCast<Float>().Length());
    else
        m_transform.Scale(0.5f * static_cast<Float>(Min(Width(), Height())));
    // translate so the origin of the view is at the center of the
    // WorldViewWidget widget
    m_transform.Translate(ScreenRect().Center().StaticCast<Float>());
}

} // end of namespace Engine2
} // end of namespace Xrb
