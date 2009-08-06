// ///////////////////////////////////////////////////////////////////////////
// xrb_input_events.hpp by Victor Dods, created 2005/11/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_INPUT_EVENTS_HPP_)
#define _XRB_INPUT_EVENTS_HPP_

#include "xrb.hpp"

#include "xrb_event.hpp"

namespace Xrb
{

/// Base class for all input events (key, mouse and joy events).
class EventInput : public Event
{
public:

    /// Boring old constructor.
    EventInput (Float time, EventType event_type)
        :
        Event(time, event_type)
    { }
    /// Pure virtual destructor so you may not instantiate this class.
    virtual ~EventInput () = 0;

    /** @brief Override of Event::IsInputEvent to indicate that this is,
      *        indeed, an input event.
      */
    virtual bool IsInputEvent () const { return true; }
};

/** Provides accessors for checking if any of the key modifiers (shift,
  * control, alt) were active during this event.  Provides accessors
  * for retrieving the key code via enumerated @ref Xrb::Key::Code as
  * well as text-oriented ASCII codes.
  * @brief Baseclass for all keyboard events.
  */
class EventKey : public EventInput
{
public:

    /** @brief Constructs an EventKey from the given SDL_KeyboardEvent.
      */
    EventKey (SDL_KeyboardEvent const *e, Float time, EventType event_type);
    /** EventKey is an abstract baseclass.
      * @brief Pure virtual destructor.
      */
    virtual ~EventKey () = 0;

    /** @brief Override of Event::IsKeyEvent to indicate that this is,
      *        indeed, a key event.
      */
    virtual bool IsKeyEvent () const { return true; }
    /** @brief Returns the SDL_KeyboardEvent used to create this event.
      */
    SDL_KeyboardEvent const &GetSDLEvent () const { return m_event; }
    /** @brief Returns the @ref Xrb::Key::Code enum for this key event.
      */
    Key::Code KeyCode () const { return (Key::Code)m_event.keysym.sym; }
    /** @brief Returns true iff either left or right alt keys were pressed
      *        when this key event was generated.
      */
    inline bool IsEitherAltKeyPressed () const
    {
        return (m_event.keysym.mod & KMOD_ALT) != 0;
    }
    /** @brief Returns true iff either left or right control keys were pressed
      *        when this key event was generated.
      */
    inline bool IsEitherControlKeyPressed () const
    {
        return (m_event.keysym.mod & KMOD_CTRL) != 0;
    }
    /** @brief Returns true iff either left or right shift keys were pressed
      *        when this key event was generated.
      */
    inline bool IsEitherShiftKeyPressed () const
    {
        return (m_event.keysym.mod & KMOD_SHIFT) != 0;
    }
    /** @brief Returns the ascii code of the key event, modified by capslock,
      *        numlock, and shift key modifiers.
      */
    char ModifiedAscii () const { return m_modified_ascii; }
    /** Virtual method for subclasses to override to indicate if they
      * are a key down event.
      * @brief Returns true iff this is a key down event.
      */
    virtual bool IsKeyDownEvent () const { return false; }
    /** Virtual method for subclasses to override to indicate if they
      * are a key up event.
      * @brief Returns true iff this is a key up event.
      */
    virtual bool IsKeyUpEvent () const { return false; }
    /** Virtual method for subclasses to override to indicate if they
      * are a key repeat event.
      * @brief Returns true iff this is a key repeat event.
      */
    virtual bool IsKeyRepeatEvent () const { return false; }

private:

    // the raw SDL keyboard event
    SDL_KeyboardEvent m_event;
    // the ascii code of the keypress modified by caps and num lock
    char m_modified_ascii;
}; // end of class EventKey

/** This event indicates a keyboard key was depressed.  This event indicates
  * only the depression event, not the release event.  For the release event,
  * refer to @ref Xrb::EventKeyUp.
  * @brief Key-down subclass of EventKey.
  */
class EventKeyDown : public EventKey
{
public:

    /** @brief Constructs an EventKeyDown from the given SDL_KeyboardEvent.
      */
    EventKeyDown (
        SDL_KeyboardEvent const *const e,
        Float const time)
        :
        EventKey(e, time, KEYDOWN)
    { }
    /** @brief Boring, empty virtual destructor.
      */
    virtual ~EventKeyDown () { }

    /** @brief Override of @ref Xrb::EventKey::IsKeyDownEvent to indicate
      *        that this is, indeed, a key down event.
      */
    virtual bool IsKeyDownEvent () const { return true; }
}; // end of class EventKeyDown

/** This event indicates a keyboard key was released.  This event indicates
  * only the release event, not the depression event.  For the depression
  * event, refer to @ref Xrb::EventKeyDown.
  * @brief Key-up subclass of EventKey.
  */
class EventKeyUp : public EventKey
{
public:

    /** @brief Constructs an EventKeyUp from the given SDL_KeyboardEvent.
      */
    EventKeyUp (
        SDL_KeyboardEvent const *const e,
        Float const time)
        :
        EventKey(e, time, KEYUP)
    { }
    /** @brief Boring, empty virtual destructor.
      */
    virtual ~EventKeyUp () { }

    /** @brief Override of @ref Xrb::EventKey::IsKeyUpEvent to indicate
      *        that this is, indeed, a key up event.
      */
    virtual bool IsKeyUpEvent () const { return true; }
}; // end of class EventKeyUp

/** This event indicates a keyboard key has been depressed and is now
  * repeating.  There will be one of these events generated for each
  * repetition.  This event can be interpreted as a key down event (e.g.
  * in LineEdit and such).  There is no equivalent repeat event for key up
  * events.
  * @brief Key-repeat subclass of EventKey.
  */
class EventKeyRepeat : public EventKey
{
public:

    /** @brief Constructs an EventKeyRepeat from the given SDL_KeyboardEvent.
      */
    EventKeyRepeat (
        SDL_KeyboardEvent const *const e,
        Float const time)
        :
        EventKey(e, time, KEYREPEAT)
    { }
    /** @brief Boring, empty virtual destructor.
      */
    virtual ~EventKeyRepeat () { }

    /** @brief Override of @ref Xrb::EventKey::IsKeyRepeatEvent to indicate
      *        that this is, indeed, a key repeat event.
      */
    virtual bool IsKeyRepeatEvent () const { return true; }
}; // end of class EventKeyRepeat

/** EventMouse is an abstract baseclass for the specific mouse events.
  *
  * Mouse events all pertain to a position on a Screen.  The position given
  * from the SDL_Mouse*Event is transformed into the equivalent screen
  * coordinate on the given Screen object.
  *
  * Provides accessors for checking if any of the key modifiers (shift,
  * control, alt, capslock, numlock) were active during this event.
  * @brief Baseclass for all mouse events.
  */
class EventMouse : public EventInput
{
public:

    /** @brief Constructs a mouse event from the given key modifiers, Screen,
      *        and mouse event position.
      */
    EventMouse (
        SDLMod modifiers,
        Screen const *screen,
        Uint16 sdl_event_x,
        Uint16 sdl_event_y,
        Float time,
        EventType event_type);
    /** @brief Pure virtual destructor.
      */
    virtual ~EventMouse () = 0;

    /** @brief Override of Event::IsMouseEvent to indicate that this is,
      *        indeed, a mouse event.
      */
    virtual bool IsMouseEvent () const { return true; }
    /** @brief Returns the screen coordinate position of this mouse event.
      */
    ScreenCoordVector2 const &GetPosition () const { return m_position; }
    /** @brief Returns true iff either left or right alt keys were pressed
      *        when this mouse event was generated.
      */
    inline bool IsEitherAltKeyPressed () const
    {
        return (m_modifiers & KMOD_ALT) != 0;
    }
    /** @brief Returns true iff either left or right control keys were pressed
      *        when this mouse event was generated.
      */
    inline bool IsEitherControlKeyPressed () const
    {
        return (m_modifiers & KMOD_CTRL) != 0;
    }
    /** @brief Returns true iff either left or shift alt keys were pressed
      *        when this mouse event was generated.
      */
    inline bool IsEitherShiftKeyPressed () const
    {
        return (m_modifiers & KMOD_SHIFT) != 0;
    }
    /** @brief Returns true iff capslock was active when this event
      *        was generated.
      */
    inline bool IsCapsLockOn () const
    {
        return (m_modifiers & KMOD_CAPS) != 0;
    }
    /** @brief Returns true iff numlock was active when this event
      *        was generated.
      */
    inline bool IsNumLockOn () const
    {
        return (m_modifiers & KMOD_NUM) != 0;
    }

private:

    // the mouse button position in virtual screen coordinates
    ScreenCoordVector2 m_position;
    // the metakey modifiers for this event
    SDLMod m_modifiers;
}; // end of class EventMouse

/** Ties together the common functionality of the button down and button
  * up events.
  *
  * The code of the relevant mousebutton is returned by ButtonCode.
  *
  * @brief Baseclass for EventMouseButtonDown and EventMouseButtonUp.
  */
class EventMouseButton : public EventMouse
{
public:

    /** @brief Constructs an EventMouseButton from the given
      * SDL_MouseButtonEvent, keyboard modifiers, and Screen.
      */
    EventMouseButton (
        SDL_MouseButtonEvent const *e,
        SDLMod modifiers,
        Screen const *screen,
        Float time,
        EventType event_type);
    /** @brief Pure virtual destructor.
      */
    virtual ~EventMouseButton () = 0;

    /** @brief Returns the SDL_MouseButtonEvent that was used to create this
      *        event.
      */
    SDL_MouseButtonEvent const &GetSDLEvent () const { return m_event; }
    /** @brief Returns the event's button code (see @ref Xrb::Key::Code).
      */
    Key::Code ButtonCode () const { return (Key::Code)m_event.button; }
    /** @brief Override of Event::IsMouseButtonEvent to indicate that
      *        this is, indeed, a mouse button event.
      */
    virtual bool IsMouseButtonEvent () const { return true; }
    /** Virtual method for subclasses to override to indicate if they
      * are a mouse button down event.
      * @brief Returns true iff this is a mouse button down event.
      */
    virtual bool IsMouseButtonDownEvent () const { return false; }
    /** Virtual method for subclasses to override to indicate if they
      * are a mouse button up event.
      * @brief Returns true iff this is a mouse button up event.
      */
    virtual bool IsMouseButtonUpEvent () const { return false; }

private:

    // the SDL_MouseButtonEvent used to create this event.
    SDL_MouseButtonEvent m_event;
}; // end of class EventMouseButton

/** This event indicates a mouse button was depressed.  This event indicates
  * only the depression event, not the release event.  For the release event,
  * refer to @ref Xrb::EventMouseButtonUp.
  * @brief Mouse button down subclass of EventMouseButton.
  */
class EventMouseButtonDown : public EventMouseButton
{
public:

    /** @brief Constructs an EventMouseButton from the given
      * SDL_MouseButtonEvent, keyboard modifiers, and Screen.
      */
    EventMouseButtonDown (
        SDL_MouseButtonEvent const *e,
        SDLMod modifiers,
        Screen const *screen,
        Float time)
        :
        EventMouseButton(e, modifiers, screen, time, MOUSEBUTTONDOWN)
    { }
    /** @brief Choose.  Choose the form of the destructor!
      */
    virtual ~EventMouseButtonDown () { }

    /** @brief Override of @ref Xrb::EventMouseButton::IsMouseButtonDownEvent to
      *        indicate that this is, indeed, a mouse button down event.
      */
    virtual bool IsMouseButtonDownEvent () const { return true; }
}; // end of class EventMouseButtonDown

/** This event indicates a mouse button was released.  This event indicates
  * only the release event, not the depression event.  For the depression
  * event, refer to @ref Xrb::EventMouseButtonDown.
  * @brief Mouse button up subclass of EventMouseButton.
  */
class EventMouseButtonUp : public EventMouseButton
{
public:

    /** @brief Constructs an EventMouseButton from the given
      * SDL_MouseButtonEvent, keyboard modifiers, and Screen.
      */
    EventMouseButtonUp (
        SDL_MouseButtonEvent const *e,
        SDLMod modifiers,
        Screen const *screen,
        Float time)
        :
        EventMouseButton(e, modifiers, screen, time, MOUSEBUTTONUP)
    { }
    /** @brief Destructor.
      */
    virtual ~EventMouseButtonUp () { }

    /** @brief Override of @ref Xrb::EventMouseButton::IsMouseButtonUpEvent to
      *        indicate that this is, indeed, a mouse button up event.
      */
    virtual bool IsMouseButtonUpEvent () const { return true; }
}; // end of class EventMouseButtonUp

/** This event indicates the mouse wheel has been scrolled.  One of
  * these events will be generated for each individual click of the mouse's
  * scroll wheel.
  * @brief Mouse wheel subclass of EventMouseButton.
  */
class EventMouseWheel : public EventMouseButton
{
public:

    /** @brief Constructs an EventMouseButton from the given
      * SDL_MouseButtonEvent, keyboard modifiers, and Screen.
      */
    EventMouseWheel (
        SDL_MouseButtonEvent const *e,
        SDLMod modifiers,
        Screen const *screen,
        Float time)
        :
        EventMouseButton(e, modifiers, screen, time, MOUSEWHEEL)
    { }
    /** @brief Destructor.
      */
    virtual ~EventMouseWheel () { }
};

/** This event contains the mouse motion delta (in addition to EventMouse's
  * position).
  *
  * Accessors are also provided to check if the left, middle, or right mouse
  * buttons were depressed at the time the event was generated.
  *
  * @brief Generated when the mouse moves.
  */
class EventMouseMotion : public EventMouse
{
public:

    /** @brief Constructs an EventMouseButton from the given
      * SDL_MouseMotionEvent, keyboard modifiers, and Screen.
      */
    EventMouseMotion (
        SDL_MouseMotionEvent const *e,
        SDLMod modifiers,
        Screen const *screen,
        Float time);
    virtual ~EventMouseMotion () { }

    /** @brief Override of Event::IsMouseMotionEvent to indicate that
      *        this is, indeed, a mouse motion event.
      */
    virtual bool IsMouseMotionEvent () const { return true; }
    /** @brief Returns the SDL_MouseMotionEvent used to create this event.
      */
    inline SDL_MouseMotionEvent const &GetSDLEvent () const { return m_event; }
    /** @brief Returns the mouse's position delta generated by this event.
      */
    inline ScreenCoordVector2 const &Delta () const { return m_delta; }
    /** @brief Returns true iff the left mouse button was depressed at
      *        the time this event was generated.
      */
    inline bool IsLeftMouseButtonPressed () const
    {
        return (m_event.state & SDL_BUTTON_LMASK) != 0;
    }
    /** @brief Returns true iff the middle mouse button was depressed at
      *        the time this event was generated.
      */
    inline bool IsMiddleMouseButtonPressed () const
    {
        return (m_event.state & SDL_BUTTON_MMASK) != 0;
    }
    /** @brief Returns true iff the right mouse button was depressed at
      *        the time this event was generated.
      */
    inline bool IsRightMouseButtonPressed () const
    {
        return (m_event.state & SDL_BUTTON_RMASK) != 0;
    }

private:

    SDL_MouseMotionEvent m_event;
    // the mouse motion delta in virtual screen coordinates
    ScreenCoordVector2 m_delta;
}; // end of class EventMouseMotion

/** No special properties of the joystick events are articulated; the events
  * simply supply the SDL events via accessors.
  * @brief Abstract baseclass for all joystick events.
  */
class EventJoy : public EventInput
{
public:

    /** @brief Constructs an EventJoy.
      */
    EventJoy (Float time, EventType event_type) : EventInput(time, event_type) { }
    /** @brief Pure virtual destructor.
      */
    virtual ~EventJoy () = 0;

    /** @brief Override of Event::IsJoyEvent to indicate that this is,
      *        indeed, a joy event.
      */
    virtual bool IsJoyEvent () const { return true; }
}; // end of class EventJoy

/** See the man page for SDL_JoyAxisEvent for more details.
  * @brief Joystick axis event.
  */
class EventJoyAxis : public EventJoy
{
public:

    // constructs this type of event from the given SDL_Event
    EventJoyAxis (SDL_JoyAxisEvent const *const e, Float const time);
    virtual ~EventJoyAxis () { }

    SDL_JoyAxisEvent const &GetSDLEvent () const { return m_event; }

private:

    SDL_JoyAxisEvent m_event;
}; // end of class EventJoyAxis

/** See the man page for SDL_JoyBallEvent for more details.
  * @brief Joystick ball event.
  */
class EventJoyBall : public EventJoy
{
public:

    // constructs this type of event from the given SDL_Event
    EventJoyBall (SDL_JoyBallEvent const *const e, Float const time);
    virtual ~EventJoyBall () { }

    SDL_JoyBallEvent const &GetSDLEvent () const { return m_event; }

private:

    SDL_JoyBallEvent m_event;
}; // end of class EventJoyBall

/** See the man page for SDL_JoyButtonEvent for more details.
  * @brief Joystick button event.
  */
class EventJoyButton : public EventJoy
{
public:

    EventJoyButton (SDL_JoyButtonEvent const *const e,
                    Float const time,
                    EventType const event_type);
    virtual ~EventJoyButton () { }

    SDL_JoyButtonEvent const &GetSDLEvent () const { return m_event; }

private:

    SDL_JoyButtonEvent m_event;
}; // end of class EventJoyButton

/** See the man page for SDL_JoyButtonEvent for more details.
  * @brief Joystick button down event.
  */
class EventJoyButtonDown : public EventJoyButton
{
public:

    // constructs this type of event from the given SDL_Event
    EventJoyButtonDown (
        SDL_JoyButtonEvent const *const e,
        Float const time)
        :
        EventJoyButton(e, time, JOYBUTTONDOWN)
    { }
    virtual ~EventJoyButtonDown () { }
}; // end of class EventJoyButtonDown

/** See the man page for SDL_JoyButtonEvent for more details.
  * @brief Joystick button up event.
  */
class EventJoyButtonUp : public EventJoyButton
{
public:

    // constructs this type of event from the given SDL_Event
    EventJoyButtonUp (
        SDL_JoyButtonEvent const *const e,
        Float const time)
        :
        EventJoyButton(e, time, JOYBUTTONUP)
    { }
    virtual ~EventJoyButtonUp () { }
}; // end of class EventJoyButtonUp

/** See the man page for SDL_JoyHatEvent for more details.
  * @brief Joystick hat event.
  */
class EventJoyHat : public EventJoy
{
public:

    // constructs this type of event from the given SDL_Event
    EventJoyHat (SDL_JoyHatEvent const *const e, Float const time);
    virtual ~EventJoyHat () { }

    SDL_JoyHatEvent const &GetSDLEvent () const { return m_event; }

private:

    SDL_JoyHatEvent m_event;
}; // end of class EventJoyHat

/** See the manpage for SDL_QuitEvent for more details.
  * @brief Event generated by SDL when the user quits the program.
  */
class EventQuit : public Event
{
public:

    /** @brief Constructs an EventQuit from the given SDL_QuitEvent.
      */
    EventQuit (SDL_QuitEvent const *const e, Float const time);
    /** @brief Boring destructor.
      */
    virtual ~EventQuit () { }

    /** @brief Returns the SDL_QuitEvent used to create this event.
      */
    SDL_QuitEvent const &GetSDLEvent () const { return m_event; }

private:

    SDL_QuitEvent m_event;
}; // end of class EventQuit

} // end of namespace Xrb

#endif // !defined(_XRB_INPUT_EVENTS_HPP_)

