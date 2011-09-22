// ///////////////////////////////////////////////////////////////////////////
// xrb_containerwidget.hpp by Victor Dods, created 2006/08/16
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_CONTAINERWIDGET_HPP_)
#define _XRB_CONTAINERWIDGET_HPP_

#include "xrb.hpp"

#include <list>
#include <vector>

#include "xrb_widget.hpp"

namespace Xrb {

// NOTE: subclasses should not set m_accepts_mouseover to true
class ContainerWidget : public Widget
{
public:

    // used to prevent resizing child widgets e.g. while creating
    // the contents of a complicated Layout.
    class ChildResizeBlocker
    {
    public:

        ChildResizeBlocker (ContainerWidget *container_widget)
        {
            ASSERT1(container_widget != NULL);
            m_container_widget = container_widget;
            m_container_widget->IncrementResizeBlockerCount();
        }
        ~ChildResizeBlocker ()
        {
            ASSERT1(m_container_widget != NULL);
            m_container_widget->DecrementResizeBlockerCount();
            m_container_widget = NULL;
        }

    private:

        void *operator new (size_t) throw() { ASSERT0(false && "this class should always be instantiated on the stack"); return NULL; }
        void *operator new[] (size_t) throw() { ASSERT0(false && "this class should always be instantiated on the stack"); return NULL; }
        void operator delete (void *) throw() { ASSERT0(false && "this class should always be instantiated on the stack"); }
        void operator delete[] (void *) throw() { ASSERT0(false && "this class should always be instantiated on the stack"); }

        ContainerWidget *m_container_widget;
    }; // end of class ContainerWidget::ChildResizeBlocker

    // ///////////////////////////////////////////////////////////////////////
    // constructor and destructor
    // ///////////////////////////////////////////////////////////////////////

    /** @brief Constructs a ContainerWidget.  The widget will be attached to the given
      *        parent widget during construction.
      * @param name A textual label applied to the specific widget being
      *             constructed.  As for now, this is only used in a debug-
      *             related capacity.
      */
    ContainerWidget (WidgetContext &context, std::string const &name = "ContainerWidget");
    /** Ensures the widget is set to not modal, ensures mouseover is off,
      * deletes all children, and clears the modal widget stack.
      * @brief Destructor.  Deletes all children.
      */
    virtual ~ContainerWidget ();

    // ///////////////////////////////////////////////////////////////////////
    // accessors
    // ///////////////////////////////////////////////////////////////////////

    /// Returns a pointer to the focused child widget, or NULL if no child widget is currently focused.
    Widget *Focus () const { return m_focus; }
    /// Returns the currently mouseover-focused child widget, or NULL if none.
    Widget *MouseoverFocus () const { return m_mouseover_focus; }
    /// Returns true iff the focused child widget has mouse grab on.
    bool FocusHasMouseGrab () const { return m_focus_has_mouse_grab; }
    /// Returns the "main widget" child, or NULL if there is none currently.
    Widget *GetMainWidget () const { return m_main_widget; }
    /// Returns the boolean vector containing the is-minimum-size-enabled values for width and height.
    virtual Bool2 const &MinSizeEnabled () const;
    /// Returns the screen-coordinate vector containing the minimum width and height in the X and Y components respectively.
    virtual ScreenCoordVector2 const &MinSize () const;
    /// Returns the boolean vector containing the is-maximum-size-enabled values for width and height.
    virtual Bool2 const &MaxSizeEnabled () const;
    /// Returns the screen-coordinate vector containing the maximum width and height.
    virtual ScreenCoordVector2 const &MaxSize () const;
    virtual ScreenCoordVector2 AdjustedSize (ScreenCoordVector2 const &size) const;

    // ///////////////////////////////////////////////////////////////////////
    // modifiers
    // ///////////////////////////////////////////////////////////////////////

    /** This function is used to set the minimum-size-enabled and
      * maximum-size-enabled for both the X/Y components, separately.
      *
      * This function should be overridden in a subclass when it is necessary
      * to know when the min/max-size-enabled property of a widget is changed.
      * @see Layout::SetSizePropertyEnabled
      *
      * @brief Sets a single component of the min/max-size-enabled property
      *        of this widget.
      * @param property Indicates which property is to be set (MIN or MAX).
      * @param component Can only be either Dim::X or Dim::Y (0 or 1,
      *                  respectively).
      * @param value The boolean indicating the new value of the specified
      *              property.
      * @param defer_parent_update This parameter is likely to be moved out
      *                            of the public API.
      */
    virtual void SetSizePropertyEnabled (
        SizeProperties::Property property,
        Uint32 component,
        bool value,
        bool defer_parent_update = false);
    /** This function is used to set either the minimum-size-enabled and
      * maximum-size-enabled for both the X/Y components at once.
      *
      * This function should be overridden in a subclass when it is necessary
      * to know when the min/max-size-enabled property of a widget is changed.
      * @see Layout::SetSizePropertyEnabled
      *
      * @brief Sets both components of the min/max-size-enabled property
      *        vector of this widget.
      * @param property Indicates which property is to be set (MIN or MAX).
      * @param value The boolean vector indicating the new components of the
      *              specified property.
      * @param defer_parent_update This parameter is likely to be moved out
      *                            of the public API.
      */
    virtual void SetSizePropertyEnabled (
        SizeProperties::Property property,
        Bool2 const &value,
        bool defer_parent_update = false);
    /** This function is used to set the minimum-size and
      * maximum-size for both the X/Y components, separately.
      *
      * This function should be overridden in a subclass when it is necessary
      * to know when the min/max-size property of a widget is changed.
      * @see Layout::SetSizeProperty
      *
      * @brief Sets a single component of the min/max-size property
      *        of this widget.
      * @param property Indicates which property is to be set (MIN or MAX).
      * @param component Can only be either Dim::X or Dim::Y (0 or 1,
      *                  respectively).
      * @param value The screen coordinate indicating the new value of the
      *              specified property.
      * @param defer_parent_update This parameter is likely to be moved out
      *                            of the public API.
      */
    virtual void SetSizeProperty (
        SizeProperties::Property property,
        Uint32 component,
        ScreenCoord value,
        bool defer_parent_update = false);
    /** This function is used to set either the minimum-size and
      * maximum-size for both the X/Y components at once.
      *
      * This function should be overridden in a subclass when it is necessary
      * to know when the min/max-size property of a widget is changed.
      * @see Layout::SetSizeProperty
      *
      * @brief Sets both components of the min/max-size property vector
      *        of this widget.
      * @param property Indicates which property is to be set (MIN or MAX).
      * @param value The screen coordinate vector indicating the new
      *              components of the specified property.
      * @param defer_parent_update This parameter is likely to be moved out
      *                            of the public API.
      */
    virtual void SetSizeProperty (
        SizeProperties::Property property,
        ScreenCoordVector2 const &value,
        bool defer_parent_update = false);

    /** Attempts to resize the given main widget to the size of this widget.
      * If NULL is given, then the main widget is cleared.
      * @brief Sets the main [child] widget for this widget.
      */
    virtual void SetMainWidget (Widget *main_widget);

    // ///////////////////////////////////////////////////////////////////////
    // procedures
    // ///////////////////////////////////////////////////////////////////////

    virtual void PreDraw ();
    virtual void Draw (RenderContext const &render_context) const;
    virtual void PostDraw ();
    virtual void MoveBy (ScreenCoordVector2 const &delta);
    virtual ScreenCoordVector2 Resize (ScreenCoordVector2 const &size);
    
    /** The given @c child widget must not currently have a parent widget.
      * @brief Attaches the given widget as a child of this widget.
      * @param child The Widget to attach using this ContainerWidget as a parent.
      */
    virtual void AttachChild (Widget *child);
    /** The given @c child widget must be a child of this widget.
      * @brief Detaches the given widget from this widget.
      */
    virtual void DetachChild (Widget *child);
    /** The position of the given child widget will be swapped with the
      * one whose index is one less in @c m_child_vector, unless the given
      * child widget's position index is 0, in which case, nothing happens.
      * @brief Moves the given child down the visibility stack
      *        (m_child_vector) by one.
      * @param child A pointer to the child widget to move.  Must actually be
      *              a child of this widget.
      */
    virtual void MoveChildDown (Widget *child);
    /** The position of the given child widget will be swapped with the
      * one whose index is one higher in @c m_child_vector, unless the given
      * child widget's position index is the highest possible, in which case,
      * nothing happens.
      * @brief Moves the given child up the visibility stack
      *        (m_child_vector) by one.
      * @param child A pointer to the child widget to move.  Must actually be
      *              a child of this widget.
      */
    virtual void MoveChildUp (Widget *child);
    /** The given child widget will be moved to index 0 in @c m_child_vector,
      * and the widgets occupying the indices below its original position will
      * all be shifted up one index.  If the given child widget is already at
      * index 0, nothing happens.
      * @brief Moves the given child to the bottom of the visibility stack
      *        (m_child_vector).
      * @param child A pointer to the child widget to move.  Must actually be
      *              a child of this widget.
      */
    virtual void MoveChildToBottom (Widget *child);
    /** The given child widget will be moved to the highest index in
      * @c m_child_vector, and the widgets occupying the indices above its
      * original position will all be shifted down one index.  If the given
      * child widget is already at the highest index, nothing happens.
      * @brief Moves the given child to the top of the visibility stack
      *        (m_child_vector).
      * @param child A pointer to the child widget to move.  Must actually be
      *              a child of this widget.
      */
    virtual void MoveChildToTop (Widget *child);
    /** This happens automatically during the destructor, so there is no
      * reason to call this unless you want to delete all child widgets
      * without deleting this widget.
      * @brief Deletes all child widgets (they are automatically detached).
      */
    void DeleteAllChildren ();

protected:

    // TODO document
    // these are interfaces for container widgets
    virtual Bool2 ContentsMinSizeEnabled () const;
    virtual ScreenCoordVector2 ContentsMinSize () const;
    virtual Bool2 ContentsMaxSizeEnabled () const;
    virtual ScreenCoordVector2 ContentsMaxSize () const;

    Uint32 ChildResizeBlockerCount () const { return m_child_resize_blocker_count; }
    bool ChildResizeWasBlocked () const { return m_child_resize_was_blocked; }

    void IndicateChildResizeWasBlocked () { m_child_resize_was_blocked = true; }

    /** Calls FrameHandler::ProcessFrame on all child widgets.
      *
      * This function is guaranteed to be called once per game loop frame,
      * unlike @a Widget::Draw.
      *
      * @brief Override of @a FrameHandler::HandleFrame which does
      *        anything that needs to be done once per iteration of the game
      *        loop.
      */
    virtual void HandleFrame ();
    // overridden from Widget
    virtual bool ProcessDetachAndDeleteChildWidgetEvent (EventDetachAndDeleteChildWidget const &e);

    virtual void HandleActivate ();
    virtual void HandleDeactivate ();
    
    /** Only used by container widgets that control the size and position
      * if their child widgets.
      * @brief Used to make sure this widget's min/max size constraints
      *        are consistent with those of its contained child widgets.
      */
    virtual void CalculateMinAndMaxSizePropertiesFromContents ();
    /** @brief This function is called (by a child widget) when a child
      *        widget's size properties have changed and the parent may
      *        need to update its properties.
      * @see Layout
      */
    virtual void ChildSizePropertiesChanged (Widget *child);
    /** @brief This function is called (by a child widget) when a child
      *        widget's stack priority has changed, and the parent needs
      *        to reorder the child vector to compensate.
      * @param child The child whose stack priority has changed.
      * @param previous_stack_priority Its most recent stack priority.
      * @note You shouldn't need to call this function directly.  It is
      *       called automatically when changing the stack priority
      *       of a widget.
      */
    virtual void ChildStackPriorityChanged (Widget *child, StackPriority previous_stack_priority);

    typedef std::vector<Widget *> WidgetVector;

    /** Stored in back-to-front drawing order (back-most having index 0).
      * @brief Ordered container of child widgets.
      */
    WidgetVector m_child_vector;
    /** This stores the size properties given by calls to SetMinSizeEnabled,
      * SetMinSize, etc, so that when the size restrictions of the child
      * widgets of this layout permit, they can be resized to satisfy
      * these properties.
      *
      * This is only used when a main widget is set, as well as in Layout
      * and WidgetStack.
      *
      * @brief Stores the 'preferred' size properties.
      */
    SizeProperties m_preferred_size_properties;
    /** The default value is false.
      * @brief True indicates child widgets get first chance at key/mouse/joy
      *        events (with this widget getting second chance).  False
      *        indicates this widget gets first chance and children get second.
      */
    bool m_children_get_input_events_first;

    typedef std::list<Widget *> WidgetList;

private:

    /// Returns a WidgetVector::iterator which matches the given child in m_child_vector.
    WidgetVector::iterator FindChildWidget (Widget const *child);

    // some overrides from Widget
    virtual bool InternalProcessKeyEvent (EventKey const &e);
    virtual bool InternalProcessMouseEvent (EventMouse const &e);
    virtual bool InternalProcessPinchEvent (EventPinch const &e);
    virtual bool InternalProcessRotateEvent (EventRotate const &e);
    virtual bool InternalProcessJoyEvent (EventJoy const &e);
    virtual bool InternalProcessFocusEvent (EventFocus const &e);
    virtual bool InternalProcessMouseoverEvent (EventMouseover const &e);
    
    /// @brief A convenience function for sending a mouse event to the child widget
    /// highest in m_child_vector which lies underneath the mouse event position.
    /// @return True iff the mouse event was accepted by any of the children.
    bool SendMouseEventToChild (EventMouse const &e);

    void IncrementResizeBlockerCount ();
    void DecrementResizeBlockerCount ();

    /// Child widget which currently has focus.  NULL indicates that there is currently no focused widget.
    Widget *m_focus;
    /// Child widget which currently has mouseover focus.  NULL indicates that there is currently no mouseover focus widget.
    Widget *m_mouseover_focus;
    /// Iff true, the focused widget has mouse grab.
    bool m_focus_has_mouse_grab;
    /// @brief Pointer to the 'main' widget.
    /// @details The main widget gets resized/repositioned whenever this widget does, and when the main widget is
    /// resized, this widget is resized to match it.  Also the size properties of each are matched in a similar manner.
    Widget *m_main_widget;
    /// @brief Number of ResizeBlockers currently blocking this widget from resizing its child widgets.
    /// @details Once this value goes from 1 to 0, this widget will attempt to resize the children to bring its layout up to date.
    Uint32 m_child_resize_blocker_count;
    /// Indicates that a resize (this widget or a child) was blocked and Resize
    /// should be called once the last ChildResizeBlocker is released.
    bool m_child_resize_was_blocked;

    // kludgey (as are all friend statements), but this is the simplest way
    friend class ChildResizeBlocker;
    friend class Screen;
    friend class Widget;
}; // end of class ContainerWidget

} // end of namespace Xrb

#endif // !defined(_XRB_CONTAINERWIDGET_HPP_)

