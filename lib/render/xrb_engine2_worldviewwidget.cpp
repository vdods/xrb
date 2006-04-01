// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_worldviewwidget.cpp by Victor Dods, created 2004/06/12
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_worldviewwidget.h"

#include "xrb_engine2_world.h"
#include "xrb_engine2_worldview.h"

namespace Xrb
{

Engine2::WorldViewWidget::WorldViewWidget (
    Engine2::WorldView *const world_view,
    Widget *const parent,
    std::string const &name)
    :
    Widget(parent, name)
{
    ASSERT1(parent != NULL)

    m_transform = FloatSimpleTransform2::ms_identity;

    m_draw_focus_frame = true;
    m_accepts_focus = true;

    // attach the view
    m_world_view = world_view;
    // make sure the transformation is adjusted according to the new position
    // and size
    ComputeTransform();

    SetBackground(NULL);
}

Engine2::WorldViewWidget::~WorldViewWidget ()
{
    if (m_world_view != NULL)
    {
        m_world_view->DetachFromWorld();
        ASSERT1(m_world_view->GetWorld() == NULL)
    }
    Delete(m_world_view);
}

void Engine2::WorldViewWidget::SetWorldView (Engine2::WorldView *const world_view)
{
    if (m_world_view != NULL)
    {
        m_world_view->DetachFromWorld();
        ASSERT1(m_world_view->GetWorld() == NULL)
    }
    Delete(m_world_view);
    m_world_view = world_view;
}

void Engine2::WorldViewWidget::Draw (RenderContext const &render_context) const
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

void Engine2::WorldViewWidget::ProcessFrameOverride ()
{
    // the WorldView's frame handler
    if (m_world_view != NULL)
        m_world_view->ProcessFrame(GetFrameTime());
    // call the base class ProcessFrameOverride
    Widget::ProcessFrameOverride();
}

void Engine2::WorldViewWidget::MoveBy (ScreenCoordVector2 const &delta)
{
    Widget::MoveBy(delta);
    ComputeTransform();
}

ScreenCoordVector2 Engine2::WorldViewWidget::Resize (ScreenCoordVector2 const &size)
{
    Widget::Resize(size);
    ComputeTransform();
    return GetSize();
}

bool Engine2::WorldViewWidget::ProcessKeyEvent (EventKey const *const e)
{
    if (m_world_view != NULL)
        return m_world_view->ProcessKeyEvent(e);
    else
        return false;
}

bool Engine2::WorldViewWidget::ProcessMouseButtonEvent (EventMouseButton const *const e)
{
    if (m_world_view != NULL)
        return m_world_view->ProcessMouseButtonEvent(e);
    else
        return false;
}

bool Engine2::WorldViewWidget::ProcessMouseWheelEvent (EventMouseWheel const *const e)
{
    if (m_world_view != NULL)
        return m_world_view->ProcessMouseWheelEvent(e);
    else
        return false;
}

bool Engine2::WorldViewWidget::ProcessMouseMotionEvent (EventMouseMotion const *const e)
{
    if (m_world_view != NULL)
        return m_world_view->ProcessMouseMotionEvent(e);
    else
        return false;
}

void Engine2::WorldViewWidget::HandleFocus ()
{
    if (m_world_view != NULL)
        m_world_view->HandleFocus();
}

void Engine2::WorldViewWidget::HandleUnfocus ()
{
    if (m_world_view != NULL)
        m_world_view->HandleUnfocus();
}

void Engine2::WorldViewWidget::DrawFocusFrame (RenderContext const &render_context) const
{
    // if we don't want to draw the focus frame, early out
    if (!m_draw_focus_frame)
        return;

    // somehow draw a frame around the inside of the widget
}

void Engine2::WorldViewWidget::ComputeTransform ()
{
    // reset the transform
    m_transform = FloatSimpleTransform2::ms_identity;
    // scale 1 unit up to the smaller of width or height.
    // the 0.5f factor is because normally the viewport maps to the rect
    // (-1, -1) to (1, 1)
    m_transform.Scale(0.5f * static_cast<Float>(Min(GetWidth(), GetHeight())));
    // translate so the origin of the view is at the center of the
    // WorldViewWidget widget
    m_transform.Translate(GetScreenRect().GetCenter().StaticCast<Float>());
}

} // end of namespace Xrb