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

namespace Xrb
{

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
      * @param parent The parent widget to attach to during construction.
      *               This must not be NULL, except in the case of a
      *               @c Screen (the top-level widget).
      * @param name A textual label applied to the specific widget being
      *             constructed.  As for now, this is only used in a debug-
      *             related capacity.
      */
    ContainerWidget (ContainerWidget *parent, std::string const &name = "ContainerWidget");
    /** Ensures the widget is set to not modal, ensures mouseover is off,
      * deletes all children, and clears the modal widget stack.
      * @brief Destructor.  Deletes all children.
      */
    virtual ~ContainerWidget ();

    // ///////////////////////////////////////////////////////////////////////
    // accessors
    // ///////////////////////////////////////////////////////////////////////

    /** @brief Returns a pointer to the focused child widget, or NULL if
      *        no child widget is currently focused.
      */
    inline Widget *GetFocus () const { return m_focus; }
    /** @brief Returns the currently mouseover-focused child widget,
      *        or NULL if none.
      */
    inline Widget *GetMouseoverFocus () const { return m_mouseover_focus; }
    /** @brief Returns true iff the focused child widget has mouse grab on.
      */
    inline bool GetFocusHasMouseGrab () const { return m_focus_has_mouse_grab; }
    /** @brief Returns the "main widget" child, or NULL if there is
      *        none currently.
      */
    inline Widget *GetMainWidget () const { return m_main_widget; }
    /** @brief Returns the boolean vector containing the
      *        is-minimum-size-enabled values for width and height in the
      *        x and y components respectively.
      */
    virtual Bool2 const &GetMinSizeEnabled () const
    {
        return m_main_widget ?
               m_main_widget->GetMinSizeEnabled() :
               m_size_properties.m_min_size_enabled;
    }
    /** @brief Returns the screen-coordinate vector containing the minimum
      *        width and height in the X and Y components respectively.
      */
    virtual ScreenCoordVector2 const &GetMinSize () const
    {
        return m_main_widget ?
               m_main_widget->GetMinSize() :
               m_size_properties.m_min_size;
    }
    /** @brief Returns the boolean vector containing the
      *        is-maximum-size-enabled values for width and height in the
      *        X and Y components respectively.
      */
    virtual Bool2 const &GetMaxSizeEnabled () const
    {
        return m_main_widget ?
               m_main_widget->GetMaxSizeEnabled() :
               m_size_properties.m_max_size_enabled;
    }
    /** @brief Returns the screen-coordinate vector containing the maximum
      *        width and height in the X and Y components respectively.
      */
    virtual ScreenCoordVector2 const &GetMaxSize () const
    {
        return m_main_widget ?
               m_main_widget->GetMaxSize() :
               m_size_properties.m_max_size;
    }
    virtual ScreenCoordVector2 GetAdjustedSize (ScreenCoordVector2 const &size) const;

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

    /** This function should be overridden in subclasses to provide the
      * means to draw whatever is necessary.  Generally the subclass's Draw
      * function should call the Draw function of its immediate superclass
      * before doing anything else, in order to have the background and
      * other necessary visual elements drawn.
      *
      * Draw should only be called on top-level widgets -- the Widget
      * baseclass takes care of setting up the RenderContext and calling
      * Draw on its child widgets appropriately.
      *
      * @brief Draws the widget using the provided RenderContext.
      * @note You can NOT count on Draw being called every single video frame,
      *       since it will not be called if the clip_rect is not valid (0
      *       area).
      */
    virtual void Draw (RenderContext const &render_context) const;
    /** @brief Move this widget by a specific delta vector.
      * @param delta The vector to add to this widget's position.
      */
    virtual void MoveBy (ScreenCoordVector2 const &delta);
    /** This function should be overridden when it is necessary to know when
      * a widget has been resized.
      *
      * The Widget baseclass will resize the widget based on its minimum
      * and maximum sizes, if applicable.  If there is a main widget,
      * @c Resize will be called on the main widget with the min/max adjusted
      * size.
      *
      * If the size of the widget was actually changed, then its parent will
      * be notified of the change via @c ParentChildSizePropertiesUpdate.
      *
      * For examples of overriding these and related functions, see @a Layout
      * and @a CellPaddingWidget.
      *
      * @brief Attempt to resize this widget to the given size.
      * @param size The requested size.
      * @return The actual size of the widget after the resize operation.
      */
    virtual ScreenCoordVector2 Resize (ScreenCoordVector2 const &size);
    /** The given @c child widget must not currently have a parent widget.
      * @brief Attaches the given widget as a child of this widget.
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

    /** @brief This implementation was required by @c WidgetSkinHandler.
      * @see WidgetSkinHandler::GetWidgetSkinHandlerChildCount
      */
    virtual Uint32 GetWidgetSkinHandlerChildCount () const;
    /** @brief This implementation was required by @c WidgetSkinHandler.
      * @see WidgetSkinHandler::GetWidgetSkinHandlerChild
      */
    virtual WidgetSkinHandler *GetWidgetSkinHandlerChild (Uint32 index);

    // TODO document
    // these are interfaces for container widgets
    virtual Bool2 GetContentsMinSizeEnabled () const;
    virtual ScreenCoordVector2 GetContentsMinSize () const;
    virtual Bool2 GetContentsMaxSizeEnabled () const;
    virtual ScreenCoordVector2 GetContentsMaxSize () const;

    inline Uint32 GetChildResizeBlockerCount () const { return m_child_resize_blocker_count; }
    inline bool GetChildResizeWasBlocked () const { return m_child_resize_was_blocked; }

    inline void IndicateChildResizeWasBlocked () { m_child_resize_was_blocked = true; }

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
    /** @brief Processes a delete child widget event.  This is used mainly to
      *        delete modal widgets.
      */
    virtual bool ProcessDeleteChildWidgetEvent (EventDeleteChildWidget const *e);

    /** Modal widget behavior requires the top level widget to divert events
      * directly to the modal widget immediately, bypassing the widget
      * hierarchy.
      * @brief Adds a modal widget to the Screen (the top level widget).
      * @note You shouldn't use this function directly.  It is called
      *       automatically when making a non-modal widget modal.
      */
    virtual void AddModalWidget (Widget *modal_widget);
    /** @brief Removes a modal widget from the Screen (the top level widget).
      * @note You shouldn't use this function directly.  It is called
      *       automatically when making a modal widget non-modal.
      */
    virtual void RemoveModalWidget (Widget *modal_widget);
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
    virtual void ChildStackPriorityChanged (
        Widget *child,
        StackPriority previous_stack_priority);

    typedef std::vector<Widget *> WidgetVector;
    typedef WidgetVector::iterator WidgetVectorIterator;
    typedef WidgetVector::const_iterator WidgetVectorConstIterator;
    typedef WidgetVector::reverse_iterator WidgetVectorReverseIterator;

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

private:

    typedef std::list<Widget *> WidgetList;
    typedef WidgetList::iterator WidgetListIterator;
    typedef WidgetList::const_iterator WidgetListConstIterator;
    typedef WidgetList::reverse_iterator WidgetListReverseIterator;

    /** @brief Returns a WidgetVectorIterator which matches the given
      *        child in m_child_vector.
      */
    WidgetVectorIterator FindChildWidget (Widget const *child);
    /** @brief Performs some necessary event processing on key
      *        events before the key event handler gets them.
      */
    virtual bool InternalProcessKeyEvent (EventKey const *e);
    /** @brief Performs some necessary event processing on mouse
      *        events before the mouse event handler gets them.
      */
    virtual bool InternalProcessMouseEvent (EventMouse const *e);
    /** @brief Performs some necessary event processing on joy
      *        events before the joy event handler gets them.
      */
    virtual bool InternalProcessJoyEvent (EventJoy const *e);
    /** @brief Performs some necessary event processing on focus events.
      */
    virtual bool InternalProcessFocusEvent (EventFocus const *e);
    /** @brief Performs some necessary event processing on mouseover events.
      */
    virtual bool InternalProcessMouseoverEvent (EventMouseover const *e);
    /** @brief A convenience function for sending a mouse event to the
      *        child widget highest in m_child_vector which lies underneath
      *        the mouse event position.
      * @return True iff the mouse event was accepted by any of the children.
      */
    bool SendMouseEventToChild (EventMouse const *e);

    void IncrementResizeBlockerCount ();
    void DecrementResizeBlockerCount ();

    /** NULL indicates that there is currently no focused widget.
      * @brief Child widget which currently has focus.
      */
    Widget *m_focus;
    /** NULL indicates that there is currently no mouseover focus widget.
      * @brief Child widget which currently has mouseover focus.
      */
    Widget *m_mouseover_focus;
    /** @brief Iff true, the focused widget has mouse grab.
      */
    bool m_focus_has_mouse_grab;
    /** The beginning of the list is the bottom of the stack, while the
      * end is the top.  The modal widgets are drawn from bottom to top.
      * @brief Contains the stack of modal widgets (used only if this is a
      *        top-level widget).
      */
    WidgetList m_modal_widget_stack;
    /** The main widget gets resized/repositioned whenever this widget does,
      * and when the main widget is resized, this widget is resized to match
      * it.  Also the size properties of each are matched in a similar manner.
      * @brief Pointer to the 'main' widget.
      */
    Widget *m_main_widget;
    /** Once this value goes from 1 to 0, this widget will attempt to resize
      * the children to bring its layout up to date.
      * @brief Number of ResizeBlockers currently blocking this widget from
      *        resizing its child widgets.
      */
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

