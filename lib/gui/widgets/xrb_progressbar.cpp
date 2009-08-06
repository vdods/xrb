// ///////////////////////////////////////////////////////////////////////////
// xrb_progressbar.cpp by Victor Dods, created 2005/12/16
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_progressbar.hpp"

#include "xrb_render.hpp"

using namespace Xrb;

namespace Xrb
{

ProgressBar::ProgressBar (
    GrowOrientation const grow_orientation,
    ContainerWidget *const parent,
    std::string const &name)
    :
    Widget(parent, name),
    m_receiver_set_progress(&ProgressBar::SetProgress, this)
{
    ASSERT1(grow_orientation < GO_COUNT);
    m_grow_orientation = grow_orientation;
    m_progress = 0.0f;
    m_color = Color(1.0f, 1.0f, 1.0f, 1.0f);
}

ProgressBar::~ProgressBar ()
{
}

void ProgressBar::Draw (RenderContext const &render_context) const
{
    ASSERT1(m_progress >= 0.0f && m_progress <= 1.0f);
    
    // this handles drawing of the background
    Widget::Draw(render_context);

    ScreenCoordRect progress_rect(GetScreenRect());
    Float const remaining_progress = 1.0f - m_progress;
        
    if (m_grow_orientation == GO_RIGHT)
        progress_rect.m_top_right[Dim::X] -=
            static_cast<ScreenCoord>(
                remaining_progress * 
                static_cast<Float>(progress_rect.GetWidth()));
    else if (m_grow_orientation == GO_LEFT)
        progress_rect.m_bottom_left[Dim::X] +=
            static_cast<ScreenCoord>(
                remaining_progress *
                static_cast<Float>(progress_rect.GetWidth()));
    else if (m_grow_orientation == GO_UP)
        progress_rect.m_top_right[Dim::Y] -=
            static_cast<ScreenCoord>(
                remaining_progress * 
                static_cast<Float>(progress_rect.Height()));
    else if (m_grow_orientation == GO_DOWN)
        progress_rect.m_bottom_left[Dim::Y] +=
            static_cast<ScreenCoord>(
                remaining_progress *
                static_cast<Float>(progress_rect.Height()));
    else
        ASSERT1(false && "Invalid ProgressBar::GrowOrientation");

    ASSERT1(progress_rect.GetWidth() >= 0);
    ASSERT1(progress_rect.Height() >= 0);
    Render::DrawScreenRect(render_context, m_color, progress_rect);
}

} // end of namespace Xrb

