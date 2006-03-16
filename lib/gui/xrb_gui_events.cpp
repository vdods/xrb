// ///////////////////////////////////////////////////////////////////////////
// xrb_gui_events.cpp by Victor Dods, created 2005/11/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_gui_events.h"

#include "xrb_widget.h"

namespace Xrb
{

EventActive::EventActive (
    SDL_ActiveEvent const *const e,
    Float const time)
    :
    Event(time, ACTIVE)
{
    ASSERT1(e != NULL)
    m_event = *e;
}

void EventDeleteChildWidget::DeleteChildWidget () const
{
    ASSERT1(m_child_to_delete != NULL)
    DeleteAndNullify(m_child_to_delete);
}

EventResize::EventResize (
    SDL_ResizeEvent const *const e,
    Float const time)
    :
    Event(time, RESIZE)
{
    ASSERT1(e != NULL)
    m_event = *e;
}

EventExpose::EventExpose (
    SDL_ExposeEvent const *const e,
    Float const time)
    :
    Event(time, EXPOSE)
{
    ASSERT1(e != NULL)
    m_event = *e;
}

EventSysWM::EventSysWM (
    SDL_SysWMEvent const *const e,
    Float const time)
    :
    Event(time, SYSWM)
{
    ASSERT1(e != NULL)
    m_event = *e;
}

} // end of namespace Xrb
