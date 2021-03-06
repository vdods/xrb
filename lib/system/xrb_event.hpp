// ///////////////////////////////////////////////////////////////////////////
// xrb_event.hpp by Victor Dods, created 2004/09/11
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_EVENT_HPP_)
#define _XRB_EVENT_HPP_

#include "xrb.hpp"

#include "xrb_key.hpp"
#include "xrb_screencoord.hpp"

namespace Xrb {

/// @brief Baseclass for all event types.
/// @details Events are generated to indicate when certain events have happened,
/// such as a key stroke, mouse motion, etc.  The Event object itself
/// carries all the information about the event as well.  Events are processed
/// by subclasses of EventHandler, in their overridden
/// @ref Xrb::EventHandler::HandleEvent methods.
///
/// Events are generally created in the game loop, though they are also
/// produced elsewhere ( e.g. in @ref Xrb::Widget::InternalProcessMouseEvent ).
/// The most important concept is that the event handler should never delete
/// the event itself.  This restriction is made so that the event pointer
/// points to valid memory for the entire scope of all HandleEvent
/// calls (specifically referring to the recursive event handling of the
/// widget hierarchy), only being deleted when execution gets back to the
/// scope at which the event was created (e.g. the game loop).
///
/// Events contain a timestamp, which is usually the time at which they were
/// generated.  This is necessary for purposes of proper ordering for events
/// queued in EventQueue.  For events being queued in EventQueue, the event
/// may be constructed with a time in the future.  This has the intuitive
/// function of scheduling the event to be processed at (no earlier than) that
/// future time.
///
/// Events processed by EventHandler must be processed in chronological order.
/// Care must be used, when constructing events, to supply the correct
/// timestamp.  ---- It is planned that a GameLoop class will be made, which
/// will handle all tracking of the "current time", and all necessary queries
/// for the current time will be made to the GameLoop class, so that it doesn't
/// need to be derived from some other object (like a FrameHandler).
class Event
{
public:

    /// @brief Enumeration for all Event subclasses.
    /// These enum values are used for real-time type information, used mainly for determining
    /// what specific event handler function should be used to process the event.
    enum EventType
    {
        DUMMY = 0,
        KEYDOWN,
        KEYUP,
        KEYREPEAT,
        MOUSEBUTTONDOWN,
        MOUSEBUTTONUP,
        MOUSEWHEEL,
        MOUSEMOTION,
        PINCHBEGIN,
        PINCHEND,
        PINCHMOTION,
        ROTATEBEGIN,
        ROTATEEND,
        ROTATEMOTION,
        JOYAXIS,
        JOYBALL,
        JOYBUTTONDOWN,
        JOYBUTTONUP,
        JOYHAT,
        FOCUS,
        MOUSEOVER,
        DETACH_AND_DELETE_CHILD_WIDGET,
        QUIT,
        STATE_MACHINE_INPUT,
        ENGINE2_DELETE_ENTITY,
        ENGINE2_REMOVE_ENTITY_FROM_WORLD,
        CUSTOM
    }; // end of enum Event::EventType

    /// @brief Construct an event with the given timestamp and event_type.
    /// @details It should be noted that EventHandler checks that the events it
    /// processes have non-descending timestamps.  Processing an event
    /// with a time less than the most recently processed will cause an assert.
    Event (Time time, EventType event_type)
        :
        m_time(time)
    {
        ASSERT1(m_time >= Time::ms_beginning_of);
        m_event_type = event_type;
        m_id = 0;
        m_is_scheduled_for_deletion = false;
    }
    /// @brief Destructor.
    /// Pure virtual.  No other pure virtual functions exist in Event, so the destructor must be
    /// pure virtual in order to make Event pure virtual.  A non-subclassed Event cannot be created.
    virtual ~Event () = 0;

    /// Returns the textual representation of the event type.
    static std::string const &Name (EventType event_type);
    /// Returns the event's timestamp.
    Time GetTime () const { return m_time; }
    /// Returns the event's type.
    EventType GetEventType () const { return m_event_type; }
    /// @brief Returns the ID of the event.
    /// @details This is used by EventQueue.  You should not need to use it.
    Uint32 ID () const { return m_id; }
    /// @brief Returns true iff the EventQueue has scheduled this event to be deleted.
    /// @details Events that have been scheduled to be deleted should never be processed.
    bool IsScheduledForDeletion () const { return m_is_scheduled_for_deletion; }
    /// @brief Returns true iff this is an input event (key/mouse/joy).
    /// @details Input event subclasses (key/mouse/joy) will override this function to return true.
    virtual bool IsInputEvent () const { return false; }
    /// @brief Returns true iff this is a keyboard event.
    /// @details Keyboard event subclasses will override this function to return true.
    virtual bool IsKeyEvent () const { return false; }
    /// @brief Returns true iff this is a mouse event.
    /// @details Mouse event subclasses will override this function to return true.
    virtual bool IsMouseEvent () const { return false; }
    /// @brief Returns true iff this is a mouse button event.
    /// @details Mouse button event subclasses will override this function to return true.
    virtual bool IsMouseButtonEvent () const { return false; }
    /// @brief Returns true iff this is a mouse motion event.
    /// @details Mouse motion event subclasses will override this function to return true.
    virtual bool IsMouseMotionEvent () const { return false; }
    /// @brief Returns true iff this is a pinch event.
    /// @details Pinch event subclasses will override this function to return true.
    virtual bool IsPinchEvent () const { return false; }
    /// @brief Returns true iff this is a pinch motion event.
    /// @details Pinch motion event subclasses will override this function to return true.
    virtual bool IsPinchMotionEvent () const { return false; }
    /// @brief Returns true iff this is a rotate event.
    /// @details Rotate event subclasses will override this function to return true.
    virtual bool IsRotateEvent () const { return false; }
    /// @brief Returns true iff this is a rotate motion event.
    /// @details Rotate motion event subclasses will override this function to return true.
    virtual bool IsRotateMotionEvent () const { return false; }
    /// @brief Returns true iff this is a joy event.
    /// @details Joy event subclasses will override this function to return true.
    virtual bool IsJoyEvent () const { return false; }

private:

    void SetID (Uint32 id) const { m_id = id; }

    void ScheduleForDeletion () const { m_is_scheduled_for_deletion = true; }

    Time m_time;
    EventType m_event_type;
    mutable Uint32 m_id;
    mutable bool m_is_scheduled_for_deletion;

    friend class EventQueue;
}; // end of class Event

/// @brief Event for asynchronously sending input to state machines.
/// @details There is no built-in facility for sending this event to StateMachine -- a StateMachine isn't
/// an EventHandler, so you must inherit EventHandler manually, and provide the necessary code in HandleEvent().
class EventStateMachineInput : public Event
{
public:

    EventStateMachineInput (StateMachineInput input, Time time)
        :
        Event(time, STATE_MACHINE_INPUT),
        m_input(input)
    { }

    StateMachineInput GetInput () const { return m_input; }

private:

    StateMachineInput const m_input;
}; // end of class EventStateMachineInput

/// @brief Abstract baseclass for custom events.
/// @details For events not forseen by XuqRijBuh, EventCustom provides a baseclass from which custom events can
/// be derived.  Widget provides a special overridable function for handling custom events.
class EventCustom : public Event
{
public:

    typedef Uint16 CustomType;

    EventCustom (CustomType custom_type, Time time)
        :
        Event(time, CUSTOM),
        m_custom_type(custom_type)
    { }
    virtual ~EventCustom () = 0;

    CustomType GetCustomType () const { return m_custom_type; }

private:

    CustomType const m_custom_type;
}; // end of class EventCustom

/// @cond IGNORE_THIS
// skip documenting this class
/// @brief An event subclass that is only used by EventQueue.  Ignore this (you should not see this in the docs).
class EventDummy : public Event
{
private:

    EventDummy (Time time) : Event(time, DUMMY) { }
    ~EventDummy () { }

    friend class EventQueue;
}; // end of class EventDummy
/// @endcond

// ///////////////////////////////////////////////////////////////////////////
// event-matching functions for use in EventQueue
// ///////////////////////////////////////////////////////////////////////////

bool MatchEventType (Event const &event, Event::EventType event_type);
bool MatchCustomType (Event const &event, EventCustom::CustomType custom_type);

} // end of namespace Xrb

#endif // !defined(_XRB_EVENT_HPP_)

