// ///////////////////////////////////////////////////////////////////////////
// xrb_event.h by Victor Dods, created 2004/09/11
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_EVENT_H_)
#define _XRB_EVENT_H_

#include "xrb.h"

#include "xrb_key.h"
#include "xrb_screencoord.h"

namespace Xrb
{

class Screen;

/** Events are generated to indicate when certain events have happened,
  * such as a key stroke, mouse motion, etc.  The Event object itself
  * carries all the information about the event as well.  Events are processed
  * by subclasses of EventHandler, in their overridden
  * @ref Xrb::EventHandler::HandleEvent methods.
  *
  * Events are generally created in the game loop, though they are also
  * produced elsewhere ( e.g. in @ref Xrb::Widget::InternalProcessMouseEvent ).
  * The most important concept is that the event handler should never delete
  * the event itself.  This restriction is made so that the event pointer
  * points to valid memory for the entire scope of all HandleEvent
  * calls (specifically referring to the recursive event handling of the
  * widget hierarchy), only being deleted when execution gets back to the
  * scope at which the event was created (e.g. the game loop).
  *
  * Events contain a timestamp, which is usually the time at which they were
  * generated.  This is necessary for purposes of proper ordering for events
  * queued in EventQueue.  For events being queued in EventQueue, the event
  * may be constructed with a time in the future.  This has the intuitive
  * function of scheduling the event to be processed at (no earlier than) that
  * future time.
  *
  * Events processed by EventHandler must be processed in chronological order.
  * Care must be used, when constructing events, to supply the correct
  * timestamp.  ---- It is planned that a GameLoop class will be made, which
  * will handle all tracking of the "current time", and all necessary queries
  * for the current time will be made to the GameLoop class, so that it doesn't
  * need to be derived from some other object (like a FrameHandler).
  *
  * @brief Baseclass for events.
  */
class Event
{
public:

    /** These enum values are used for real-time type information, used mainly
      * for determining what specific event handler function should be used
      * to process the event.
      * @brief Enumeration for all Event subclasses.
      */
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
        JOYAXIS,
        JOYBALL,
        JOYBUTTONDOWN,
        JOYBUTTONUP,
        JOYHAT,
        FOCUS,
        MOUSEOVER,
        DELETE_CHILD_WIDGET,
        ACTIVE,
        RESIZE,
        EXPOSE,
        QUIT,
        SYSWM,
        STATE_MACHINE_INPUT,
        ENGINE2_DELETE_ENTITY,
        ENGINE2_REMOVE_ENTITY_FROM_WORLD,
        CUSTOM
    }; // end of enum Event::EventType

    /** It should be noted that EventHandler checks that the events it
      * processes have non-descending timestamps.  Processing an event
      * with a time less than the most recently processed will cause an
      * assert.
      * @brief Construct an event with the given timestamp and event_type.
      */
    inline Event (Float time, EventType event_type)
    {
        ASSERT1(time >= 0.0);
        m_time = time;
        m_event_type = event_type;
        m_id = 0;
        m_is_scheduled_for_deletion = false;
    }
    /** Pure virtual.  No other pure virtual functions exist in Event,
      * so the destructor must be pure virtual in order to make Event
      * pure virtual.  A non-subclassed Event cannot be created.
      * @brief Destructor.
      */
    virtual ~Event () = 0;

    /** @brief Returns the textual representation of the event type.
      */
    static std::string const &GetName (EventType event_type);
    /** @brief Returns the event's timestamp.
      */
    inline Float GetTime () const { return m_time; }
    /** @brief Returns the event's type.
      */
    inline EventType GetEventType () const { return m_event_type; }
    /** This is used by EventQueue.  You should not need to use it.
      * @brief Returns the ID of the event.
      */
    inline Uint32 GetID () const { return m_id; }
    /** Events that have been scheduled to be deleted should never be
      * processed.
      * @brief Returns true iff the EventQueue has scheduled this event to
      * be deleted.
      */
    inline bool GetIsScheduledForDeletion () const { return m_is_scheduled_for_deletion; }
    /** Input event subclasses (key/mouse/joy) will override this function to
      * return true.
      * @brief Returns true iff this is an input event (key/mouse/joy).
      */
    virtual bool GetIsInputEvent () const { return false; }
    /** Keyboard event subclasses will override this function to return true.
      * @brief Returns true iff this is a keyboard event.
      */
    virtual bool GetIsKeyEvent () const { return false; }
    /** Mouse event subclasses will override this function to return true.
      * @brief Returns true iff this is a mouse event.
      */
    virtual bool GetIsMouseEvent () const { return false; }
    /** Mouse button event subclasses will override this function to return
      * true.
      * @brief Returns true iff this is a mouse button event.
      */
    virtual bool GetIsMouseButtonEvent () const { return false; }
    /** Mouse motion event subclasses will override this function to return
      * true.
      * @brief Returns true iff this is a mouse motion event.
      */
    virtual bool GetIsMouseMotionEvent () const { return false; }
    /** Joy event subclasses will override this function to return true.
      * @brief Returns true iff this is a joy event.
      */
    virtual bool GetIsJoyEvent () const { return false; }

    /** The SDL_Event itself will be saved in the Event, and the created
      * event will be populated with the necessary class-specific information.
      *
      * You are responsible for deleting the returned event.
      *
      * @brief Creates a new event of the appropriate type from the given
      *        SDL_Event.
      */
    static Event *CreateEventFromSDLEvent (
        SDL_Event const *e,
        Screen const *screen,
        Float time);

private:

    inline void SetID (Uint32 id) const { m_id = id; }

    inline void ScheduleForDeletion () const { m_is_scheduled_for_deletion = true; }

    Float m_time;
    EventType m_event_type;
    mutable Uint32 m_id;
    mutable bool m_is_scheduled_for_deletion;

    friend class EventQueue;
}; // end of class Event

/** There is no built-in facility for sending this event to StateMachine --
  * a StateMachine isn't an EventHandler, so you must inherit EventHandler
  * manually, and provide the necessary code in HandleEvent().
  * @brief Event for asynchronously sending input to state machines.
  */
class EventStateMachineInput : public Event
{
public:

    /** @brief Constructs an EventStateMachineInput.
      */
    EventStateMachineInput (StateMachineInput input, Float time)
        :
        Event(time, STATE_MACHINE_INPUT),
        m_input(input)
    { }

    inline StateMachineInput GetInput () const { return m_input; }

private:

    StateMachineInput const m_input;
}; // end of class EventStateMachineInput

/** For events not forseen by XuqRijBuh, EventCustom provides a baseclass
  * from which custom events can be derived.  Widget provides a special
  * overridable function for handling custom events.
  * @brief Abstract baseclass for custom events.
  */
class EventCustom : public Event
{
public:

    typedef Uint16 CustomType;

    /** @brief Constructs an EventCustom.
      */
    EventCustom (CustomType custom_type, Float time)
        :
        Event(time, CUSTOM),
        m_custom_type(custom_type)
    { }
    /** @brief Pure virtual destructor.
      */
    virtual ~EventCustom () = 0;

    inline CustomType GetCustomType () const { return m_custom_type; }

private:

    CustomType const m_custom_type;
}; // end of class EventCustom

/// @cond IGNORE_THIS
// skip documenting this class
/** Ignore it.
  * @brief An event subclass that is only used by EventQueue.
  */
class EventDummy : public Event
{
private:

    EventDummy (Float time) : Event(time, DUMMY) { }
    ~EventDummy () { }

    friend class EventQueue;
}; // end of class EventDummy
/// @endcond

// ///////////////////////////////////////////////////////////////////////////
// event-matching functions for use in EventQueue
// ///////////////////////////////////////////////////////////////////////////

bool MatchEventType (Event const *event, Event::EventType event_type);
bool MatchCustomType (Event const *event, EventCustom::CustomType custom_type);

} // end of namespace Xrb

#endif // !defined(_XRB_EVENT_H_)

