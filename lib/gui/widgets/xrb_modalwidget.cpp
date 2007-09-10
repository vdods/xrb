// ///////////////////////////////////////////////////////////////////////////
// xrb_modalwidget.cpp by Victor Dods, created 2005/02/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_modalwidget.h"

#include "xrb_gui_events.h"

namespace Xrb
{

ModalWidget::ModalWidget (
    ContainerWidget *const parent,
    std::string const &name)
    :
    ContainerWidget(parent, name)
{
    // make this widget modal
    SetIsModal(true);
}

void ModalWidget::Shutdown ()
{
    // tell the parent to delete this widget
    ASSERT1(GetParent() != NULL);
    EventDeleteChildWidget *delete_child_widget_event =
        new EventDeleteChildWidget(this, GetParent()->GetMostRecentFrameTime());
    GetParent()->EnqueueEvent(delete_child_widget_event);

    // make this widget un-modal
    SetIsModal(false);
    // block future events
    SetIsBlockingEvents(true);
}

void ModalWidget::UpdateRenderBackground ()
{
    SetRenderBackground(
        GetWidgetSkinWidgetBackground(WidgetSkin::MODAL_WIDGET_BACKGROUND));
}

void ModalWidget::HandleChangedModalWidgetBackground ()
{
    UpdateRenderBackground();
}

void ModalWidget::HandleChangedWidgetSkinWidgetBackground (
    WidgetSkin::WidgetBackgroundType const widget_background_type)
{
    if (widget_background_type == WidgetSkin::MODAL_WIDGET_BACKGROUND)
        HandleChangedModalWidgetBackground();
}

} // end of namespace Xrb
