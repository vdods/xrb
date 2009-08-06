// ///////////////////////////////////////////////////////////////////////////
// xrb_eventhandler.cpp by Victor Dods, created 2004/09/11
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_eventhandler.hpp"

#include "xrb_event.hpp"
#include "xrb_eventqueue.hpp"

namespace Xrb
{

EventHandler::EventHandler (EventQueue *const owner_event_queue)
{
    m_owner_event_queue = owner_event_queue;
    m_is_blocking_events = false;
    m_most_recent_event_time = -1.0;
    m_current_event_time = -1.0;
    m_event_dt = -1.0;
}

EventHandler::~EventHandler ()
{
    if (m_owner_event_queue != NULL)
    {
        m_owner_event_queue->DeleteEventsBelongingToHandler(this);
        m_owner_event_queue = NULL;
    }

    m_is_blocking_events = true;
}

bool EventHandler::ProcessEvent (Event const *const e)
{
    ASSERT1(e != NULL);
    ASSERT1(!e->IsScheduledForDeletion());

    // make sure that events show up "in order", even if their times
    // aren't actually completely correct.
    Float event_time = e->Time();
    if (event_time < m_most_recent_event_time)
        event_time = m_most_recent_event_time;

    // if this event handler is blocking events, return that the event was unused
    if (IsBlockingEvents())
        return false;

    // set the current event time
    m_current_event_time = event_time;
    // if this is the first event, initialize the most recent event time
    if (m_most_recent_event_time < 0.0)
        m_most_recent_event_time = m_current_event_time;
    // set the event time delta
    m_event_dt = m_current_event_time - m_most_recent_event_time;

    if (HandleEvent(e))
    {
        // update the most recent event time only if this event was used
        m_most_recent_event_time = m_current_event_time;
        // return that the event was used
        return true;
    }

    // reset the event time delta so you can't reference the
    // event DT outside of ProcessEvent
    m_event_dt = -1.0;

    // return that the event was unused
    return false;
}

void EventHandler::EnqueueEvent (Event const *const e)
{
    ASSERT1(e != NULL);
    ASSERT0(m_owner_event_queue != NULL);
    m_owner_event_queue->EnqueueEvent(this, e);
}

} // end of namespace Xrb
