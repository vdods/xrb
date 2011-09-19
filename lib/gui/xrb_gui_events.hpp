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

namespace Xrb {

class Widget;

/** Generated by Widget in response to mouse button down events.  These
  * events are used to focus the widgets that are under the mouse when
  * a mouse button down event occurs.
  * @brief Widget focus event.
  */
class EventFocus : public Event
{
public:

    /// Constructs a focus event from the given screen coordinate vector
    /// (the position of the mouse at the time of a mouse button down event).
    EventFocus (ScreenCoordVector2 const &position, Float time)
        :
        Event(time, FOCUS)
    {
        m_position = position;
    }
    virtual ~EventFocus () { }

    /// Returns the position used to construct this event.
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

    /// Constructs a mouseover event from the given screen coordinate vector
    /// (the position of the mouse at the time of a mouse motion event).
    EventMouseover (ScreenCoordVector2 const &position, Float time)
        :
        Event(time, MOUSEOVER)
    {
        m_position = position;
    }
    virtual ~EventMouseover () { }

    /// Returns the position used to construct this event.
    ScreenCoordVector2 const &Position () const { return m_position; }

private:

    ScreenCoordVector2 m_position;
}; // end of class EventMouseOver

/** Currently only generated by @ref Xrb::ModalWidget::Shutdown.
  *
  * Automatically processed by Widget.
  *
  * @brief Used to asynchronously schedule widget detachment and deletion.
  */
class EventDetachAndDeleteChildWidget : public Event
{
public:

    /// Constructs the event, specifying a pointer to the child widget to delete.
    EventDetachAndDeleteChildWidget (Widget *child_to_detach_and_delete, Float time)
        :
        Event(time, DETACH_AND_DELETE_CHILD_WIDGET)
    {
        ASSERT1(child_to_detach_and_delete != NULL);
        m_child_to_detach_and_delete = child_to_detach_and_delete;
    }
    virtual ~EventDetachAndDeleteChildWidget ()
    {
        m_child_to_detach_and_delete = NULL;
    }

private:

    /// Returns a pointer to the child widget to delete.
    Widget *ChildToDetachAndDelete () const { return m_child_to_detach_and_delete; }

    /// Detaches the child from its parent (calling DetachAsModalChildWidget if the
    /// child is modal), then deletes and nullifies the child to delete widget pointer.
    void DetachAndDeleteChildWidget () const;

    mutable Widget *m_child_to_detach_and_delete;

    friend class ContainerWidget;
}; // end of class EventDetachAndDeleteChildWidget

} // end of namespace Xrb

#endif // !defined(_XRB_GUI_EVENTS_HPP_)

