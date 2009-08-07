// ///////////////////////////////////////////////////////////////////////////
// xrb_gui_events.hpp by Victor Dods, created 2005/11/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_GUI_EVENTS_HPP_)
#define _XRB_GUI_EVENTS_HPP_

#include "xrb.hpp"

#include "xrb_event.hpp"

namespace Xrb
{

class Widget;

/** Generated by Widget in response to mouse button down events.  These
  * events are used to focus the widgets that are under the mouse when
  * a mouse button down event occurs.
  * @brief Widget focus event.
  */
class EventFocus : public Event
{
public:

    /** @brief Constructs a focus event from the given screen coordinate
      *        vector (the position of the mouse at the time of a mouse button
      *        down event).
      */
    EventFocus (
        ScreenCoordVector2 const &position,
        Float const time)
        :
        Event(time, FOCUS)
    {
        m_position = position;
    }
    /** @brief Boring destructor.
      */
    virtual ~EventFocus () { }

    /** @brief Returns the position used to construct this event.
      */
    ScreenCoordVector2 const &Position () const { return m_position; }

private:

    ScreenCoordVector2 m_position;
}; // end of class EventFocus

/** Generated by Widget in response to mouse motion events.  These
  * events are used to mouseover-focus the widgets that are under the
  * mouse.
  * @brief Mouseover event.
  */
class EventMouseover : public Event
{
public:

    /** @brief Constructs a mouseover event from the given screen coordinate
      *        vector (the position of the mouse at the time of a mouse
      *        motion event).
      */
    EventMouseover (
        ScreenCoordVector2 const &position,
        Float const time)
        :
        Event(time, MOUSEOVER)
    {
        m_position = position;
    }
    /** @brief Destructor.  Dull, dull, dull, oh GOD it's so desperately dull.
      */
    virtual ~EventMouseover () { }

    /** @brief Returns the position used to construct this event.
      */
    ScreenCoordVector2 const &Position () const { return m_position; }

private:

    ScreenCoordVector2 m_position;
}; // end of class EventMouseOver

/** Currently only generated by @ref Xrb::ModalWidget::Shutdown.
  *
  * Automatically processed by Widget.
  *
  * @brief Used to asynchronously schedule widget deletion.
  */
class EventDeleteChildWidget : public Event
{
public:

    /** @brief Constructs the event, specifying a pointer to the child
      *        widget to delete.
      */
    EventDeleteChildWidget (
        Widget *const child_to_delete,
        Float const time)
        :
        Event(time, DELETE_CHILD_WIDGET)
    {
        ASSERT1(child_to_delete != NULL);
        m_child_to_delete = child_to_delete;
    }
    /** @brief Destructor.
      */
    virtual ~EventDeleteChildWidget ()
    {
        m_child_to_delete = NULL;
    }

private:

    /** @brief Returns a pointer to the child widget to delete.
      */
    inline Widget *ChildToDelete () const { return m_child_to_delete; }

    /** @brief Deletes and nullifies the child to delete widget pointer.
      */
    void DeleteChildWidget () const;

    mutable Widget *m_child_to_delete;

    friend class ContainerWidget;
}; // end of class EventDeleteChildWidget

/** Analogous to SDL_ActiveEvent.
  * @brief SDL-oriented event, not currently supported/used by XuqRijBuh
  */
class EventActive : public Event
{
public:

    // constructs this type of event from the given SDL_Event
    /** @brief Constructs an EventActive from the given SDL_ActiveEvent.
      */
    EventActive (SDL_ActiveEvent const *e, Float time);
    /** @brief Boring old destructor.
      */
    virtual ~EventActive () { }

    /** @brief Returns the SDL_ActiveEvent used to create this event.
      */
    SDL_ActiveEvent const &SDLEvent () const { return m_event; }

private:

    SDL_ActiveEvent m_event;
}; // end of class EventActive

/** Analogous to SDL_ResizeEvent.
  * @brief SDL-oriented event, not currently supported/used by XuqRijBuh
  */
class EventResize : public Event
{
public:

    // constructs this type of event from the given SDL_Event
    EventResize (SDL_ResizeEvent const *const e, Float const time);
    virtual ~EventResize () { }

    SDL_ResizeEvent const &SDLEvent () const { return m_event; }

private:

    SDL_ResizeEvent m_event;
}; // end of class EventResize

/** Analogous to SDL_ExposeEvent.
  * @brief SDL-oriented event, not currently supported/used by XuqRijBuh
  */
class EventExpose : public Event
{
public:

    // constructs this type of event from the given SDL_Event
    EventExpose (SDL_ExposeEvent const *const e, Float const time);
    virtual ~EventExpose () { }

    SDL_ExposeEvent const &SDLEvent () const { return m_event; }

private:

    SDL_ExposeEvent m_event;
}; // end of class EventExpose

/** Analogous to SDL_SysWMEvent.
  * @brief SDL-oriented event, not currently supported/used by XuqRijBuh
  */
class EventSysWM : public Event
{
public:

    // constructs this type of event from the given SDL_Event
    EventSysWM (SDL_SysWMEvent const *const e, Float const time);
    virtual ~EventSysWM () { }

    SDL_SysWMEvent const &SDLEvent () const { return m_event; }

private:

    SDL_SysWMEvent m_event;
}; // end of class EventSysWM

} // end of namespace Xrb

#endif // !defined(_XRB_GUI_EVENTS_HPP_)
