// ///////////////////////////////////////////////////////////////////////////
// xrb_eventqueue.cpp by Victor Dods, created 2005/02/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_eventqueue.hpp"

#include "xrb_event.hpp"
#include "xrb_eventhandler.hpp"

namespace Xrb
{

// ///////////////////////////////////////////////////////////////////////////
// EventQueue
// ///////////////////////////////////////////////////////////////////////////

EventQueue::EventQueue ()
    :
    FrameHandler()
{
    m_current_event_id = 0;
}

EventQueue::~EventQueue ()
{
    // make sure the buffered events are added into the main queue
    EnqueueBufferedEvents();

    // delete all unprocessed events
    for (TimeOrderedEventBindingSetIterator it = m_time_ordered_event_queue.begin(),
                                            it_end = m_time_ordered_event_queue.end();
         it != it_end;
         ++it)
    {
        Event const *event = it->GetEvent();
        ASSERT1(event != NULL);
        Delete(event);
    }

    // clear the time-ordered event queue
    m_time_ordered_event_queue.clear();
}

void EventQueue::EnqueueEvent (
    EventHandler *const event_handler,
    Event const *const event)
{
    ASSERT1(event_handler != NULL);
    ASSERT1(event != NULL);
    ASSERT1(event_handler->MostRecentEventTime() <= event->Time());

    // this call makes sure that we don't overflow the IDs, which is essential
    // for proper ordering of events inside the queue.
    CompactEventIDs();

    // set the given event's ID to the current ID and increment the current ID
    event->SetID(m_current_event_id++);

    // insert the bound event and eventhandler into the secondary queue, which
    // will be inserted into the main queues before processing events
    EventBinding event_binding(event_handler, event);
    m_buffered_time_ordered_event_queue.insert(event_binding);
}

void EventQueue::DeleteEventsBelongingToHandler (
    EventHandler *const event_handler)
{
    ASSERT1(event_handler != NULL);

    for (TimeOrderedEventBindingSetIterator it = m_time_ordered_event_queue.begin(),
                                            it_end = m_time_ordered_event_queue.end();
         it != it_end;
         ++it)
    {
        if (!it->GetEvent()->IsScheduledForDeletion())
            if (it->GetEventHandler() == event_handler)
                it->GetEvent()->ScheduleForDeletion();
    }
}

void EventQueue::ScheduleMatchingEventsForDeletion (
    bool (*EventMatchingFunction)(Event const *))
{
    // check all enqueued events against the given event-matching function.
    for (TimeOrderedEventBindingSetIterator it = m_time_ordered_event_queue.begin(),
                                            it_end = m_time_ordered_event_queue.end();
         it != it_end;
         ++it)
    {
        Event const *event = it->GetEvent();
        ASSERT1(event != NULL);
        // only check events that aren't already scheduled for deletion
        if (!event->IsScheduledForDeletion())
            // if the function indicates a match, schedule the event for deletion
            if (EventMatchingFunction(event))
                event->ScheduleForDeletion();
    }
}

void EventQueue::HandleFrame ()
{
    // make sure the buffered events are added into the main queue
    EnqueueBufferedEvents();

    // this event is used to get the last event which should be processed
    EventDummy event_limit(FrameTime());
    event_limit.SetID(MaxEventID());
    EventBinding binding_limit(NULL, &event_limit);

    // get the range of events to be processed, and loop through,
    // processing them and scheduling each for deletion
    {
        TimeOrderedEventBindingSetIterator it_begin =
            m_time_ordered_event_queue.begin();
        TimeOrderedEventBindingSetIterator it_end =
            m_time_ordered_event_queue.upper_bound(binding_limit);

        // process each event and schedule it for deletion.
        for (TimeOrderedEventBindingSetIterator it = it_begin;
             it != it_end;
             ++it)
        {
            ASSERT1(it->GetEventHandler() != NULL);
            ASSERT1(it->GetEvent() != NULL);
            // don't process the event if it's already scheduled for deletion
            if (!it->GetEvent()->IsScheduledForDeletion())
            {
                // schedule the event for deletion first, so if it ever checks
                // for scheduled-to-be-deleted events during its processing,
                // it won't end up in a recursive hell.
                it->GetEventHandler()->ProcessEvent(it->GetEvent());
                it->GetEvent()->ScheduleForDeletion();
            }
        }

        // delete each event.  this must be done separately from the above
        // for-loop, otherwise calls to ScheduleMatchingEventsForDeletion
        // during processing of each event will potentially make invalid reads
        // to the deleted events.
        for (TimeOrderedEventBindingSetIterator it = it_begin;
             it != it_end;
             ++it)
        {
            ASSERT1(it->GetEvent()->IsScheduledForDeletion());
            delete it->GetEvent();            
        }
        
        // clear the range of processed events from the time-ordered event queue
        m_time_ordered_event_queue.erase(it_begin, it_end);
    }
}

void EventQueue::EnqueueBufferedEvents ()
{
    // insert the secondary queue into the main queue, so we only
    // have to worry about the main queue
    m_time_ordered_event_queue.insert(
        m_buffered_time_ordered_event_queue.begin(),
        m_buffered_time_ordered_event_queue.end());
    // clear the buffered event queue
    m_buffered_time_ordered_event_queue.clear();
}

void EventQueue::CompactEventIDs ()
{
    // only perform compaction (which is infrequent but slow) if
    // the current ID is at the maximum number
    if (m_current_event_id == MaxEventID())
    {
        // make sure the buffered events are added to the main queue
        EnqueueBufferedEvents();
        // at this point, the events in the main event queue are perfectly
        // ordered, which means that we can assign incremental IDs to the
        // events in order and the queue's ordering won't change.  set
        // the current ID to zero and start assigning
        m_current_event_id = 0;
        for (TimeOrderedEventBindingSetIterator
             it = m_time_ordered_event_queue.begin(),
             it_end = m_time_ordered_event_queue.end();
             it != it_end;
             ++it)
        {
            // set the event's ID
            it->GetEvent()->SetID(m_current_event_id++);
            // just make sure there are enough IDs to identify all events
            ASSERT1(m_current_event_id != MaxEventID());
        }
    }
}

// ///////////////////////////////////////////////////////////////////////////
// EventQueue::OrderEventBindingsByEventTime
// ///////////////////////////////////////////////////////////////////////////

bool EventQueue::OrderEventBindingsByEventTime::operator () (
    EventBinding const &left_operand,
    EventBinding const &right_operand) const
{
    if (left_operand.GetEvent()->Time() < right_operand.GetEvent()->Time())
        return true;
    else if (left_operand.GetEvent()->Time() == right_operand.GetEvent()->Time())
        return left_operand.GetEvent()->ID() < right_operand.GetEvent()->ID();
    else
        return false;
}

} // end of namespace Xrb
