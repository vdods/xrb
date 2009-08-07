// ///////////////////////////////////////////////////////////////////////////
// xrb_eventqueue.hpp by Victor Dods, created 2005/02/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_EVENTQUEUE_HPP_)
#define _XRB_EVENTQUEUE_HPP_

#include "xrb.hpp"

#include <set>

#include "xrb_event.hpp"
#include "xrb_framehandler.hpp"

namespace Xrb
{

class EventHandler;

/** Many EventHandler instances are "owned" by a single EventQueue.  The
  * EventQueue receives orders to enqueue events for a particular
  * EventHandler.  All events from all EventHandlers are enqueued
  * chronologically in a single queue.  The events are then dequeued (also
  * in chronologicaly order) when the current time (as passed to the
  * EventQueue) is greater or equal to the event's scheduled processing time.
  *
  * When an EventHandler is deleted, all events that are enqueued for it
  * are taken out of the queue and deleted.
  *
  * @brief Controls chronological queueing and processing of asynchronous
  *        events.
  */
class EventQueue : public FrameHandler
{
public:

    /** @brief Constructor.
      */
    EventQueue ();
    /** Deletes all remaining enqueued events.
      * @brief Destructor.
      */
    virtual ~EventQueue ();

    /** The time at which it will be processed is given the time
      * specified during the Event's construction.
      *
      * @note You shouldn't have to call this directly.  Use
      * @ref Xrb::EventHandler::EnqueueEvent instead.
      *
      * @brief Enqueues an event for the given EventHandler.
      */
    void EnqueueEvent (
        EventHandler *event_handler,
        Event const *event);
    /** @note You shouldn't have to call this directly.  It will be called
      *       automatically by EventHandler's destructor.
      * @brief Deletes events which belong to the given EventHandler
      */
    void DeleteEventsBelongingToHandler (
        EventHandler *event_handler);
    /** This is used to remove situation-specific sets of events.
      * @brief Deletes all queued events that return true when passed to the
      *        given event matching function.
      * @param EventMatchingFunction A pointer to the matching function to use.
      */
    void ScheduleMatchingEventsForDeletion (bool (*EventMatchingFunction)(Event const *));
    /** This is used to remove situation-specific sets of events.
      *
      * The template parameter specifies the type of the single extra
      * parameter of the event matching function (besides the Event itself).
      *
      * @brief Deletes all queued events that return true when passed to the
      *        given event matching function.
      * @param EventMatchingFunction A pointer to the matching function to use.
      * @param parameter The parameter to pass to the matching function.
      */
    template <typename ParameterType>
    void ScheduleMatchingEventsForDeletion (
        bool (*EventMatchingFunction)(Event const *, ParameterType),
        ParameterType parameter);
    /** This is used to remove situation-specific sets of events.
      *
      * The template parameter specifies the type of the two extra parameters
      * of the event matching function (besides the Event itself).
      *
      * @brief Deletes all queued events that return true when passed to the
      *        given event matching function.
      * @param EventMatchingFunction A pointer to the matching function to use.
      * @param parameter1 The first parameter (of type Parameter1Type) to pass
      *                   to the matching function.
      * @param parameter2 The first parameter (of type Parameter2Type) to pass
      *                   to the matching function.
      */
    template <typename Parameter1Type, typename Parameter2Type>
    void ScheduleMatchingEventsForDeletion (
        bool (*EventMatchingFunction)(Event const *, Parameter1Type, Parameter2Type),
        Parameter1Type parameter1,
        Parameter2Type parameter2);
        
protected:

    /** @brief Processes all events in the event queue with time less than or
      *        equal to the current frame time.
      */
    virtual void HandleFrame ();

private:

    // the maximum ID number
    static inline Uint32 MaxEventID ()
    {
        return UINT32_UPPER_BOUND;
    }

    // stick the buffered event queue into the main event queue
    void EnqueueBufferedEvents ();
    // when m_current_id is
    void CompactEventIDs ();

    // the EventBinding class is used to pair events with event handlers
    class EventBinding
    {
    private:

        EventHandler *m_event_handler;
        Event const *m_event;

    public:

        EventBinding (
            EventHandler *const event_handler,
            Event const *const event)
        {
            m_event_handler = event_handler;
            m_event = event;
        }

        inline EventHandler *GetEventHandler () const
        {
            return m_event_handler;
        }
        inline Event const *GetEvent () const
        {
            return m_event;
        }
    }; // end of class EventBinding

    // the OrderEventBindingsByEventTime functor is used to provide a strict
    // weak ordering for event queues which should be ordered by event time
    struct OrderEventBindingsByEventTime
    {
        bool operator () (
            EventBinding const &left_operand,
            EventBinding const &right_operand) const;
    };

    typedef std::multiset<EventBinding, OrderEventBindingsByEventTime> TimeOrderedEventBindingSet;

    // the time-ordered queue of events (ordered by time and then ID)
    TimeOrderedEventBindingSet m_time_ordered_event_queue;
    // secondary time-ordered event queue, which event bindings are inserted
    // into before being added to the main event queue.  they will be inserted
    // into the main event queue before ProcessQueue actually does any
    // processing.
    TimeOrderedEventBindingSet m_buffered_time_ordered_event_queue;
    
    // the next available Event ID.  the event IDs are used to give a unique
    // ordering to the event queue.  the event queue is ordered primarily by
    // event time, but then secondarily by the ID (so that events with the same
    // time will be sorted in the order they were issued).
    Uint32 m_current_event_id;

}; // end of class EventQueue

template <typename ParameterType>
void EventQueue::ScheduleMatchingEventsForDeletion (
    bool (*EventMatchingFunction)(Event const *, ParameterType),
    ParameterType parameter)
{
    // check all enqueued events against the given event-matching function.
    for (TimeOrderedEventBindingSet::iterator it = m_time_ordered_event_queue.begin(),
                                            it_end = m_time_ordered_event_queue.end();
         it != it_end;
         ++it)
    {
        Event const *event = it->GetEvent();
        ASSERT1(event != NULL);
        // only check events that aren't already scheduled for deletion
        if (!event->IsScheduledForDeletion())
            // if the function indicates a match, schedule the event for deletion
            if (EventMatchingFunction(event, parameter))
                event->ScheduleForDeletion();
    }
}

template <typename Parameter1Type, typename Parameter2Type>
void EventQueue::ScheduleMatchingEventsForDeletion (
    bool (*EventMatchingFunction)(Event const *, Parameter1Type, Parameter2Type),
    Parameter1Type parameter1,
    Parameter2Type parameter2)
{
    // check all enqueued events against the given event-matching function.
    for (TimeOrderedEventBindingSet::iterator it = m_time_ordered_event_queue.begin(),
                                            it_end = m_time_ordered_event_queue.end();
         it != it_end;
         ++it)
    {
        Event const *event = it->GetEvent();
        ASSERT1(event != NULL);
        // only check events that aren't already scheduled for deletion
        if (!event->IsScheduledForDeletion())
            // if the function indicates a match, schedule the event for deletion
            if (EventMatchingFunction(event, parameter1, parameter2))
                event->ScheduleForDeletion();
    }
}

} // end of namespace Xrb

#endif // !defined(_XRB_EVENTQUEUE_HPP_)

