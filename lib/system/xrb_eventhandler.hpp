// ///////////////////////////////////////////////////////////////////////////
// xrb_eventhandler.hpp by Victor Dods, created 2004/09/11
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_EVENTHANDLER_HPP_)
#define _XRB_EVENTHANDLER_HPP_

#include "xrb.hpp"

#include "xrb_time.hpp"

namespace Xrb {

class Event;
class EventQueue;

/// @brief Provides event handling, blocking, and queueing facilities for subclasses.
/// @details Classes which require event handling should derive from this class.
class EventHandler
{
public:

    /// @brief Constructs an EventHandler with the given EventQueue.
    /// @param owner_event_queue The EventQueue which processes asynchronously queued events.
    /// If EventQueue is @c NULL, then calls to EnqueueEvent will cause a failed assertion.
    EventHandler (EventQueue *owner_event_queue);
    /// If m_owner_event_queue is not NULL, then all of this EventHandler's events will be deleted from it.
    virtual ~EventHandler ();

    /// @brief Called to process an event immediately.
    /// @details Returns true iff the event was used by this eventhandler.  If events are being blocked, it will not call HandleEvent.
    bool ProcessEvent (Event const &e);
    /// @brief Enqueues an event for this EventHandler in the EventQueue pointed to by m_owner_event_queue.
    /// @details The event will be processed asynchronously in chronological order, interleaved
    /// with all other events queued in m_owner_event_queue.  If m_owner_event_queue is @c NULL,
    /// this function will assert.
    void EnqueueEvent (Event const *e);

    /// @brief Returns m_owner_event_queue -- the EventQueue which controls asynchronous event handling for this EventHandler.
    /// @details If @c NULL, then there is no associated EventQueue, and events cannot be queued for this EventHandler.
    EventQueue *OwnerEventQueue () const { return m_owner_event_queue; }
    /// @brief Returns true iff this EventHandler is blocking all events.
    /// @details When an EventHandler is blocking events, ProcessEvent returns false immediately upon being called.
    bool IsBlockingEvents () const { return m_is_blocking_events; }
    /// @brief Returns the time of the most-recently processed Event.
    /// @details Returns 0 if no event has been processed.
    Time MostRecentEventTime () const { return m_most_recent_event_time; }
    /// @brief Returns time of the event currently being processed.
    /// @details This function may only be called from within HandleEvent, since it would
    /// be meaningless anywhere else.  Calling it elsewhere will cause an assert.
    Time EventTime () const { ASSERT1(m_allow_event_time_access); return m_current_event_time; }

    /// @brief Sets the event blocking property of this EventHandler.
    /// @details If true, ProcessEvent will immediately return false, otherwise the normal event handling will be done.
    void SetIsBlockingEvents (bool is_blocking_events) { m_is_blocking_events = is_blocking_events; }

protected:

    /// @brief Sets the owner EventQueue for this EventHandler.
    /// @details Widget, being an EventHandler itself, is constructed with no owner EventQueue.  This function
    /// is used to assign the owner EventQueue of its parent when attached.  Screen uses this function as well,
    /// though its owner EventQueue is passed in through the constructor.
    void SetOwnerEventQueue (EventQueue *owner_event_queue) { m_owner_event_queue = owner_event_queue; }

    /// @brief Subclasses must override this to provide the actual event handling necessary.
    /// @details ProcessEvent calls this function to do the subclass-specific event handling.  Overrides of
    /// this function should return true if the event was used, otherwise false.
    virtual bool HandleEvent (Event const &e) = 0;

private:

    // the EventQueue which administers this EventHandler
    EventQueue *m_owner_event_queue;
    // indicates if this event handler is blocking events or not
    bool m_is_blocking_events;
    // the event time for the most recently processed event
    Time m_most_recent_event_time;
    // the current event time
    Time m_current_event_time;
    // indicates if EventTime() can be called or not.
    bool m_allow_event_time_access;
}; // end of class EventHandler

} // end of namespace Xrb

#endif // !defined(_XRB_EVENTHANDLER_HPP_)

