// ///////////////////////////////////////////////////////////////////////////
// xrb_modalwidget.cpp by Victor Dods, created 2005/02/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_modalwidget.hpp"

#include "xrb_gui_events.hpp"
#include "xrb_widgetcontext.hpp"

namespace Xrb {

ModalWidget::ModalWidget (WidgetContext &context, std::string const &name)
    :
    ContainerWidget(context, name)
{
    SetBackgroundStyle(StyleSheet::BackgroundType::MODAL_WIDGET);
}

void ModalWidget::Shutdown ()
{
    ASSERT1(Parent() != NULL);
    // tell the Screen to detach and delete this.
    Parent()->EnqueueEvent(new EventDetachAndDeleteChildWidget(this, Parent()->MostRecentFrameTime()));
    // block future events
    SetIsBlockingEvents(true);
}

void ModalWidget::HandleActivate ()
{
    Focus();
}

} // end of namespace Xrb
