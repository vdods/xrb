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

namespace Xrb
{

class Event;
class EventQueue;

/** Classes which require event handling should derive from this class.
  * @brief Provides event handling, blocking, and queueing facilities
  *        for subclasses.
  */
class EventHandler
{
public:

    /** If EventQueue is @c NULL, then calls to EnqueueEvent will cause
      * a failed assertion.
      * @brief Constructs an EventHandler with the given EventQueue.
      * @param owner_event_queue The EventQueue which processes asynchronously
      *                          queued events.
      */
    EventHandler (EventQueue *owner_event_queue);
    /** If m_owner_event_queue is not NULL, then all of this EventHandler's
      * events will be deleted from it.
      * @brief Destructor.
      */
    virtual ~EventHandler ();

    /** Returns true iff the event was used by this eventhandler.  If events
      * are being blocked, it will not call HandleEvent.
      * @brief Called to process an event immediately.
      */
    bool ProcessEvent (Event const *e);
    /** The event will be processed asynchronously in chronological order,
      * interleaved with all other events queued in m_owner_event_queue.
      *
      * If m_owner_event_queue is @c NULL, this function will assert.
      *
      * @brief Enqueues an event for this EventHandler in the EventQueue
      *        pointed to by m_owner_event_queue.
      */
    void EnqueueEvent (Event const *e);

    /** If @c NULL, then there is no associated EventQueue, and events cannot
      * be queued for this EventHandler.
      * @brief Returns m_owner_event_queue -- the EventQueue which controls
      *        asynchronous event handling for this EventHandler.
      */
    inline EventQueue *OwnerEventQueue () const
    {
        return m_owner_event_queue;
    }
    /** When an EventHandler is blocking events, ProcessEvent returns false
      * immediately upon being called.
      * @brief Returns true iff this EventHandler is blocking all events.
      */
    inline bool IsBlockingEvents () const
    {
        return m_is_blocking_events;
    }
    /** Returns 0 if no event has been processed.
      * @brief Returns the time of the most-recently processed Event.
      */
    inline Float MostRecentEventTime () const
    {
        return Max(m_most_recent_event_time, 0.0f);
    }
    /** This function may only be called from within HandleEvent,
      * since it would be meaningless anywhere else.  Calling it elsewhere
      * will cause an assert.
      * @brief Returns time of the event currently being processed.
      */
    inline Float EventTime () const
    {
        ASSERT1(m_event_dt >= 0.0);
        return m_current_event_time;
    }
    /** This function may only be called from within HandleEvent,
      * since it would be meaningless anywhere else.  Calling it elsewhere
      * will cause an assert.
      * @brief Returns the time delta since the most recently processed event.
      */
    inline Float EventDT () const
    {
        ASSERT1(m_event_dt >= 0.0);
        return m_event_dt;
    }

    /** If true, ProcessEvent will immediately return false, otherwise
      * the normal event handling will be done.
      * @brief Sets the event blocking property of this EventHandler.
      */
    inline void SetIsBlockingEvents (bool const is_blocking_events)
    {
        m_is_blocking_events = is_blocking_events;
    }

protected:

    /** Widget, being an EventHandler itself, is constructed with no owner
      * EventQueue.  This function is used to assign the owner EventQueue of
      * its parent when attached.  Screen uses this function as well, though
      * its owner EventQueue is passed in through the constructor.
      * @brief Sets the owner EventQueue for this EventHandler.
      */
    inline void SetOwnerEventQueue (EventQueue *const owner_event_queue)
    {
        m_owner_event_queue = owner_event_queue;
    }

    /** ProcessEvent calls this function to do the subclass-specific event
      * handling.  Overrides of this function should return true if the
      * event was used, otherwise false.
      * @brief Subclasses must override this to provide the actual event
      *        handling necessary.
      */
    virtual bool HandleEvent (Event const *e) = 0;

private:

    // the EventQueue which administers this EventHandler
    EventQueue *m_owner_event_queue;
    // indicates if this event handler is blocking events or not
    bool m_is_blocking_events;
    // the event time for the most recently processed event
    Float m_most_recent_event_time;
    // the current event time
    Float m_current_event_time;
    // the delta between the last event time and the current frame time
    Float m_event_dt;
}; // end of class EventHandler

} // end of namespace Xrb

#endif // !defined(_XRB_EVENTHANDLER_HPP_)

