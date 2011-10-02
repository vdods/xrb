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

namespace Xrb {

/// Base class for all input events (key, mouse and joy events).
class EventInput : public Event
{
public:

    /// Boring old constructor.
    EventInput (Time time, EventType event_type)
        :
        Event(time, event_type)
    { }
    /// Pure virtual destructor so you may not instantiate this class.
    virtual ~EventInput () = 0;

    /// Override of Event::IsInputEvent to indicate that this is, indeed, an input event.
    virtual bool IsInputEvent () const { return true; }
};

/// @brief Baseclass for all keyboard events.
/// @details Provides accessors for checking if any of the key modifiers (shift, control, alt) were
/// active during this event.  Provides accessors for retrieving the key code via enumerated
/// @ref Xrb::Key::Code as well as text-oriented ASCII codes.
class EventKey : public EventInput
{
public:

    /// Constructs an EventKey with the given parameters.
    EventKey (Key::Code code, Key::Modifier modifier, Time time, EventType event_type);
    /// Pure virtual destructor.  EventKey is an abstract baseclass.
    virtual ~EventKey () = 0;

    /// Override of Event::IsKeyEvent to indicate that this is, indeed, a key event.
    virtual bool IsKeyEvent () const { return true; }
    /// Returns the @ref Xrb::Key::Code enum for this key event.
    Key::Code KeyCode () const { return m_code; }
    /// Returns the @ref Xrb::Key::Modifier flags for this key event.
    Key::Modifier KeyModifier () const { return m_modifier; }
    /// Returns true iff either left or right alt keys were pressed when this key event was generated.
    bool IsEitherAltKeyPressed () const { return (m_modifier & Key::MOD_ALT) != 0; }
    /// Returns true iff either left or right control keys were pressed when this key event was generated.
    bool IsEitherControlKeyPressed () const { return (m_modifier & Key::MOD_CTRL) != 0; }
    /// Returns true iff either left or right shift keys were pressed when this key event was generated.
    bool IsEitherShiftKeyPressed () const { return (m_modifier & Key::MOD_SHIFT) != 0; }
    /// Returns the ascii code of the key event, modified by capslock, numlock, and shift key modifiers.
    char ModifiedAscii () const { return m_modified_ascii; }
    /// @brief Returns true iff this is a key down event.
    /// @details Virtual method for subclasses to override to indicate if they are a key down event.
    virtual bool IsKeyDownEvent () const { return false; }
    /// @brief Returns true iff this is a key up event.
    /// @details Virtual method for subclasses to override to indicate if they are a key up event.
    virtual bool IsKeyUpEvent () const { return false; }
    /// @brief Returns true iff this is a key repeat event.
    /// @details Virtual method for subclasses to override to indicate if they are a key repeat event.
    virtual bool IsKeyRepeatEvent () const { return false; }

private:

    // the keycode (e.g. A, NUMLOCK, CTRL)
    Key::Code m_code;
    // the key modifier (e.g. left-alt)
    Key::Modifier m_modifier;
    // the ascii code of the keypress modified by caps and num lock
    char m_modified_ascii;
}; // end of class EventKey

/// @brief Key-down subclass of EventKey.
/// @details This event indicates a keyboard key was depressed.  This event indicates only the depression
/// event, not the release event.  For the release event, refer to @ref Xrb::EventKeyUp.
class EventKeyDown : public EventKey
{
public:

    /// Constructs an EventKeyDown with the given parameters.
    EventKeyDown (
        Key::Code code,
        Key::Modifier modifier,
        Time time)
        :
        EventKey(code, modifier, time, KEYDOWN)
    { }
    virtual ~EventKeyDown () { }

    /// Override of @ref Xrb::EventKey::IsKeyDownEvent to indicate that this is, indeed, a key down event.
    virtual bool IsKeyDownEvent () const { return true; }
}; // end of class EventKeyDown

/// @brief Key-up subclass of EventKey.
/// @details This event indicates a keyboard key was released.  This event indicates only the release event,
/// not the depression event.  For the depression event, refer to @ref Xrb::EventKeyDown.
////
class EventKeyUp : public EventKey
{
public:

    /// Constructs an EventKeyUp with the given parameters.
    EventKeyUp (
        Key::Code code,
        Key::Modifier modifier,
        Time time)
        :
        EventKey(code, modifier, time, KEYUP)
    { }
    virtual ~EventKeyUp () { }

    /// Override of @ref Xrb::EventKey::IsKeyUpEvent to indicate that this is, indeed, a key up event.
    virtual bool IsKeyUpEvent () const { return true; }
}; // end of class EventKeyUp

/// @brief Key-repeat subclass of EventKey.
/// @details This event indicates a keyboard key has been depressed and is now repeating.  There will be one of
/// these events generated for each repetition.  This event can be interpreted as a key down event (e.g. in
/// LineEdit and such).  There is no equivalent repeat event for key up events.
class EventKeyRepeat : public EventKey
{
public:

    /// Constructs an EventKeyRepeat with the given parameters.
    EventKeyRepeat (
        Key::Code code,
        Key::Modifier modifier,
        Time time)
        :
        EventKey(code, modifier, time, KEYREPEAT)
    { }
    virtual ~EventKeyRepeat () { }

    /// Override of @ref Xrb::EventKey::IsKeyRepeatEvent to indicate that this is, indeed, a key repeat event.
    virtual bool IsKeyRepeatEvent () const { return true; }
}; // end of class EventKeyRepeat

/// @brief Baseclass for all mouse events.
/// @details EventMouse is an abstract baseclass for the specific mouse events.  Provides accessors for checking
/// if any of the key modifiers (shift, control, alt, capslock, numlock) were active during this event.
class EventMouse : public EventInput
{
public:

    /// Constructs a mouse event from the given parameters.
    EventMouse (
        ScreenCoordVector2 const &position,
        Key::Modifier modifier,
        Time time,
        EventType event_type);
    virtual ~EventMouse () = 0;

    /// Override of Event::IsMouseEvent to indicate that this is, indeed, a mouse event.
    virtual bool IsMouseEvent () const { return true; }
    /// Returns the screen coordinate position of this mouse event.
    ScreenCoordVector2 const &Position () const { return m_position; }
    /// Returns true iff either left or right alt keys were pressed when this mouse event was generated.
    bool IsEitherAltKeyPressed () const { return (m_modifier & Key::MOD_ALT) != 0; }
    /// Returns true iff either left or right control keys were pressed when this mouse event was generated.
    bool IsEitherControlKeyPressed () const { return (m_modifier & Key::MOD_CTRL) != 0; }
    /// Returns true iff either left or shift alt keys were pressed when this mouse event was generated.
    bool IsEitherShiftKeyPressed () const { return (m_modifier & Key::MOD_SHIFT) != 0; }
    /// Returns true iff capslock was active when this event was generated.
    bool IsCapsLockOn () const { return (m_modifier & Key::MOD_CAPS) != 0; }
    /// Returns true iff numlock was active when this event was generated.
    bool IsNumLockOn () const { return (m_modifier & Key::MOD_NUM) != 0; }

private:

    /// @brief Sets the screen coordinate position of this mouse event.
    /// @details This is needed by Screen for view rotation.
    void SetPosition (ScreenCoordVector2 const &position) const { m_position = position; }

    // the mouse button position in screen coordinates
    mutable ScreenCoordVector2 m_position;
    // the metakey modifier flags for this event
    Key::Modifier m_modifier;

    // this is necessary for screen rotation
    friend class Screen;
}; // end of class EventMouse

/// @brief Baseclass for EventMouseButtonDown and EventMouseButtonUp.
/// @details Ties together the common functionality of the button down and button up events.
///
/// The code of the relevant mousebutton is returned by ButtonCode.
class EventMouseButton : public EventMouse
{
public:

    /// Constructs an EventMouseButton with the given parameters.
    EventMouseButton (
        Key::Code button_code,
        ScreenCoordVector2 const &position,
        Key::Modifier modifier,
        Time time,
        EventType event_type);
    /// Pure virtual destructor.
    virtual ~EventMouseButton () = 0;

    /// Returns the event's button code (see @ref Xrb::Key::Code).
    Key::Code ButtonCode () const { return m_button_code; }
    /// Override of Event::IsMouseButtonEvent to indicate that this is, indeed, a mouse button event.
    virtual bool IsMouseButtonEvent () const { return true; }
    /// @brief Returns true iff this is a mouse button down event.
    /// @details Virtual method for subclasses to override to indicate if they are a mouse button down event.
    virtual bool IsMouseButtonDownEvent () const { return false; }
    /// @brief Returns true iff this is a mouse button up event.
    /// @details Virtual method for subclasses to override to indicate if they are a mouse button up event.
    virtual bool IsMouseButtonUpEvent () const { return false; }

private:

    Key::Code m_button_code;
}; // end of class EventMouseButton

/// @brief Mouse button down subclass of EventMouseButton.
/// @details This event indicates a mouse button was depressed.  This event indicates only the depression
/// event, not the release event.  For the release event, refer to @ref Xrb::EventMouseButtonUp.
class EventMouseButtonDown : public EventMouseButton
{
public:

    /// Constructs an EventMouseButton with the given parameters.
    EventMouseButtonDown (
        Key::Code button_code,
        ScreenCoordVector2 const &position,
        Key::Modifier modifier,
        Time time)
        :
        EventMouseButton(button_code, position, modifier, time, MOUSEBUTTONDOWN)
    { }
    /// Choose.  Choose the form of the destructor!
    virtual ~EventMouseButtonDown () { }

    /// Override of @ref Xrb::EventMouseButton::IsMouseButtonDownEvent to indicate that this is, indeed, a mouse button down event.
    virtual bool IsMouseButtonDownEvent () const { return true; }
}; // end of class EventMouseButtonDown

/// @brief Mouse button up subclass of EventMouseButton.
/// @details This event indicates a mouse button was released.  This event indicates only the release event, not
/// the depression event.  For the depression event, refer to @ref Xrb::EventMouseButtonDown.
class EventMouseButtonUp : public EventMouseButton
{
public:

    /// Constructs an EventMouseButton with the given parameters.
    EventMouseButtonUp (
        Key::Code button_code,
        ScreenCoordVector2 const &position,
        Key::Modifier modifier,
        Time time)
        :
        EventMouseButton(button_code, position, modifier, time, MOUSEBUTTONUP)
    { }
    virtual ~EventMouseButtonUp () { }

    /// Override of @ref Xrb::EventMouseButton::IsMouseButtonUpEvent to indicate that this is, indeed, a mouse button up event.
    virtual bool IsMouseButtonUpEvent () const { return true; }
}; // end of class EventMouseButtonUp

/// @brief Mouse wheel subclass of EventMouseButton.
/// @details This event indicates the mouse wheel has been scrolled.  One of these events will be generated
/// for each individual click of the mouse's scroll wheel.
class EventMouseWheel : public EventMouseButton
{
public:

    /// Constructs an EventMouseButton with the given parameters.
    EventMouseWheel (
        Key::Code button_code,
        ScreenCoordVector2 const &position,
        Key::Modifier modifier,
        Time time)
        :
        EventMouseButton(button_code, position, modifier, time, MOUSEWHEEL)
    { }
    virtual ~EventMouseWheel () { }
};

/// @brief Generated when the mouse moves.
/// @details This event contains the mouse motion delta (in addition to EventMouse's position).
///
/// Accessors are also provided to check if the left, middle, or right mouse buttons were depressed at the
/// time the event was generated.
class EventMouseMotion : public EventMouse
{
public:

    /// Constructs an EventMouseMotion from the given parameters.
    EventMouseMotion (
        bool is_left_mouse_button_pressed,
        bool is_middle_mouse_button_pressed,
        bool is_right_mouse_button_pressed,
        ScreenCoordVector2 const &position,
        ScreenCoordVector2 const &delta,
        Key::Modifier modifier,
        Time time);
    virtual ~EventMouseMotion () { }

    /// Override of Event::IsMouseMotionEvent to indicate that this is, indeed, a mouse motion event.
    virtual bool IsMouseMotionEvent () const { return true; }
    /// Returns the mouse's position delta generated by this event.
    ScreenCoordVector2 const &Delta () const { return m_delta; }
    /// Returns true iff the left mouse button was depressed at the time this event was generated.
    bool IsLeftMouseButtonPressed () const { return m_is_left_mouse_button_pressed; }
    /// Returns true iff the middle mouse button was depressed at the time this event was generated.
    bool IsMiddleMouseButtonPressed () const { return m_is_middle_mouse_button_pressed; }
    /// Returns true iff the right mouse button was depressed at the time this event was generated.
    bool IsRightMouseButtonPressed () const { return m_is_right_mouse_button_pressed; }

private:

    /// @brief Sets the position delta of this mouse event.
    /// @details This is needed by Screen for view rotation.
    void SetDelta (ScreenCoordVector2 const &delta) const { m_delta = delta; }

    bool m_is_left_mouse_button_pressed;
    bool m_is_middle_mouse_button_pressed;
    bool m_is_right_mouse_button_pressed;
    // the mouse motion delta in screen coordinates
    mutable ScreenCoordVector2 m_delta;

    // this is needed for screen rotation
    friend class Screen;
}; // end of class EventMouseMotion

/// @brief Baseclass for all pinch events.
/// @details EventPinch is an abstract baseclass for the specific pinch events (e.g. from an iPhone).
class EventPinch : public EventInput
{
public:

    /// Constructs a pinch event from the given parameters.
    EventPinch (Time time, EventType event_type) : EventInput(time, event_type) { }
    /// Pure virtual destructor.
    virtual ~EventPinch () = 0;

    /// Override of Event::IsPinchEvent to indicate that this is, indeed, a pinch event.
    virtual bool IsPinchEvent () const { return true; }
}; // end of class EventPinch

/// Generated by the beginning of a pinch gesture (e.g. on the iPhone).
class EventPinchBegin : public EventPinch
{
public:

    /// @brief Constructs an EventPinchBegin from the given parameters.
    /// @details The scale parameter should be the initial value for scale (can be generated by the first pinch gesture motion).
    EventPinchBegin (Float scale, Time time);
    virtual ~EventPinchBegin () { }

    /// Returns the scale of the pinch as a floating point ratio with respect to the smaller dimension of the screen.
    Float Scale () const { return m_scale; }

private:

    Float const m_scale;
}; // end of class EventPinchBegin

/// Generated by the end of a pinch gesture (e.g. on the iPhone).
class EventPinchEnd : public EventPinch
{
public:

    /// Constructs an EventPinchEnd from the given parameters.
    EventPinchEnd (Time time) : EventPinch(time, PINCHEND) { }
    virtual ~EventPinchEnd () { }
}; // end of class EventPinchEnd

/// @brief Generated by a pinch gesture (e.g. on the iPhone).
/// @details This event contains the pinch motion delta.
class EventPinchMotion : public EventPinch
{
public:

    /// Constructs an EventPinchMotion from the given parameters.
    EventPinchMotion (Float scale, Float velocity, Time time);
    virtual ~EventPinchMotion () { }

    /// Override of Event::IsPinchMotionEvent to indicate that this is, indeed, a pinch motion event.
    virtual bool IsPinchMotionEvent () const { return true; }
    /// Returns the scale of the pinch as a floating point ratio with respect to the smaller dimension of the screen.
    Float Scale () const { return m_scale; }
    /// Returns the velocity of the pinch which seems to be some per-pixel measurement of the pinch movement.
    Float Velocity () const { return m_velocity; }

private:

    Float const m_scale;
    Float const m_velocity;
}; // end of class EventPinchMotion

/// @brief Baseclass for all rotate events.
/// @details EventRotate is an abstract baseclass for the specific rotate events (e.g. from an iPhone).
class EventRotate : public EventInput
{
public:

    /// Constructs a rotate event from the given parameters.
    EventRotate (Time time, EventType event_type) : EventInput(time, event_type) { }
    /// Pure virtual destructor.
    virtual ~EventRotate () = 0;

    /// Override of Event::IsRotateEvent to indicate that this is, indeed, a rotate event.
    virtual bool IsRotateEvent () const { return true; }
}; // end of class EventRotate

/// Generated by the beginning of a rotate gesture (e.g. on the iPhone).
class EventRotateBegin : public EventRotate
{
public:

    /// Constructs an EventRotateBegin from the given parameters.
    EventRotateBegin (Time time) : EventRotate(time, ROTATEBEGIN) { }
    virtual ~EventRotateBegin () { }
}; // end of class EventRotateBegin

/// Generated by the end of a rotate gesture (e.g. on the iPhone).
class EventRotateEnd : public EventRotate
{
public:

    /// Constructs an EventRotateEnd from the given parameters.
    EventRotateEnd (Time time) : EventRotate(time, ROTATEEND) { }
    virtual ~EventRotateEnd () { }
}; // end of class EventRotateEnd

/// @brief Generated by a rotate gesture (e.g. on the iPhone).
/// @details This event contains the rotate motion delta.
class EventRotateMotion : public EventRotate
{
public:

    /// Constructs an EventRotateMotion from the given parameters.
    EventRotateMotion (Float rotation, Float velocity, Time time);
    virtual ~EventRotateMotion () { }

    /// Override of Event::IsRotateMotionEvent to indicate that this is, indeed, a rotate motion event.
    virtual bool IsRotateMotionEvent () const { return true; }
    /// The rotation variable starts at zero and gives the rotation from the initial position in radians.
    Float Rotation () const { return m_rotation; }
    /// Returns the velocity of the rotate which seems to be some per-pixel measurement of the rotate movement.
    Float Velocity () const { return m_velocity; }

private:

    Float const m_rotation;
    Float const m_velocity;
}; // end of class EventRotateMotion

/// @brief Abstract baseclass for all joystick events.
/// @details TODO: articulate joystick events
class EventJoy : public EventInput
{
public:

    /// Constructs an EventJoy.
    EventJoy (Time time, EventType event_type) : EventInput(time, event_type) { }
    /// Pure virtual destructor.
    virtual ~EventJoy () = 0;

    /// Override of Event::IsJoyEvent to indicate that this is, indeed, a joy event.
    virtual bool IsJoyEvent () const { return true; }
}; // end of class EventJoy

/// Event signaling that the program has requested to quit.
class EventQuit : public Event
{
public:

    EventQuit (Time time) : Event(time, QUIT) { }
}; // end of class EventQuit

} // end of namespace Xrb

#endif // !defined(_XRB_INPUT_EVENTS_HPP_)

