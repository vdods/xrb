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

namespace Xrb {

EventHandler::EventHandler (EventQueue *owner_event_queue)
{
    m_owner_event_queue = owner_event_queue;
    m_is_blocking_events = false;
    m_most_recent_event_time = Time::ms_negative_infinity;
    m_current_event_time = Time::ms_negative_infinity;
    m_allow_event_time_access = false;
}

EventHandler::~EventHandler ()
{
    if (m_owner_event_queue != NULL)
    {
        m_owner_event_queue->DeleteEventsBelongingToHandler(*this);
        m_owner_event_queue = NULL;
    }

    m_is_blocking_events = true;
}

bool EventHandler::ProcessEvent (Event const &e)
{
    ASSERT1(!e.IsScheduledForDeletion());

    // make sure that events show up "in order", even if their times
    // aren't actually completely correct.
    Time event_time = e.GetTime();
    if (event_time < m_most_recent_event_time)
        event_time = m_most_recent_event_time;

    // if this event handler is blocking events, return that the event was unused
    if (IsBlockingEvents())
        return false;

    // set the current event time
    m_current_event_time = event_time;
    // allow the EventTime accessor to be used
    m_allow_event_time_access = true;

    if (HandleEvent(e))
    {
        // update the most recent event time only if this event was used
        m_most_recent_event_time = m_current_event_time;
        // return that the event was used
        return true;
    }

    // disallow the EventTime accessor to be used (no use outside of HandleEvent())
    m_allow_event_time_access = false;

    // return that the event was unused
    return false;
}

void EventHandler::EnqueueEvent (Event const *e)
{
    ASSERT1(e != NULL);
    ASSERT0(m_owner_event_queue != NULL);
    m_owner_event_queue->EnqueueEvent(*this, e);
}

} // end of namespace Xrb
