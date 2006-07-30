// ///////////////////////////////////////////////////////////////////////////
// xrb_widget.h by Victor Dods, created 2004/09/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_WIDGET_H_)
#define _XRB_WIDGET_H_

#include "xrb.h"

#include <vector>
#include <string>

#include "xrb_color.h"
#include "xrb_eventhandler.h"
#include "xrb_framehandler.h"
#include "xrb_rect.h"
#include "xrb_rendercontext.h"
#include "xrb_resourcelibrary.h"
#include "xrb_screencoord.h"
#include "xrb_signalhandler.h"
#include "xrb_sizeproperties.h"
#include "xrb_vector.h"
#include "xrb_widgetskin.h"
#include "xrb_widgetskinhandler.h"

/** @file xrb_widget.h
  * @brief Contains the class declaration of the foundation of the GUI
  *        system of XuqRijBuh -- Widget.
  */

namespace Xrb
{

class EventCustom;
class EventDeleteChildWidget;
class EventFocus;
class EventJoy;
class EventKey;
class EventMouse;
class EventMouseButton;
class EventMouseMotion;
class EventMouseWheel;
class EventMouseover;
class EventStateMachineInput;
class Screen;
class WidgetBackground;

/** The base class for all GUI widgets and provides 90% of the framework
  * necessary for the GUI's operation.
  *
  * The GUI system of XuqRijBuh is modeled closely upon the commonly used
  * "widget" paradigm of GUI toolkits such as QT (http://www.trolltech.com).
  * Some examples of specialized widgets are @c Label, @c Button, and
  * @c CheckBox.
  *
  * A hierarchy of widgets (a widget being essentially a rectangular area
  * on the screen) is created to suit the task at hand.Widgets
  * subordinate to a particular widget in the hierarchy are referred to as
  * its "child widgets" or "children".  The hierarchy dictates specific
  * rules as to how widgets are drawn and how they receive events.
  *
  * The top of the widget hierarchy must be an instance of @c Screen.
  * it contains specialized functionality for initiating drawing and
  * event-sending.
  *
  * Widgets are drawn in a top-down manner -- drawing begins at the @c Screen,
  * which performs some one-time setup work (one-time per frame), and sets
  * up the OpenGL viewport/clip rect and so forth, and then calls the drawing
  * function on all its child widgets in the order in which they appear in
  * the widget stack (see @c m_child_vector).  During the draw function, a
  * widget will draw itself (e.g. backgrounds, text, pictures, nothing, etc)
  * and then call the draw function on its own children in the order specified
  * by @c m_child_vector.  There are a few caveats to this, including hidden
  * widgets, but that will be explained later.
  *
  * Events (see @c Event) are processed in a similar fashion as drawing --
  * starting at the top of the hierarchy, propagating down to children.  The
  * difference here is that the event will not be "used" by all the widgets
  * in the hierarchy.  An event is created at a particular level in the
  * hierarchy (though usually from outside and passed to the top), and then
  * each child widget is asked if it wishes to use the event.  A used
  * event is not destroyed or in any way changed, allowing for parent widgets
  * to perform further processing after a child accepts/denies usage of an
  * event.
  *
  * See @ref widget_baseclass "Widget Baseclass" for extensive system details.
  *
  * @brief The class which forms the foundation of the GUI system.
  * @note All GUI widgets must derive from this class to function properly.
  */
class Widget : public WidgetSkinHandler, public FrameHandler, public EventHandler, public SignalHandler
{
public:

    // ///////////////////////////////////////////////////////////////////////
    // constructor and destructor
    // ///////////////////////////////////////////////////////////////////////

    /** @brief Constructs a Widget.  The widget will be attached to the given
      *        parent widget during construction.
      * @param parent The parent widget to attach to during construction.
      *               This must not be NULL, except in the case of a
      *               @c Screen (the top-level widget).
      * @param name A textual label applied to the specific widget being
      *             constructed.  As for now, this is only used in a debug-
      *             related capacity.
      */
    Widget (Widget *parent, std::string const &name = "Widget");
    /** Ensures the widget is set to not modal, ensures mouseover is off,
      * deletes all children, and clears the modal widget stack.
      * @brief Destructor.  Deletes all children.
      */
    virtual ~Widget ();

    // ///////////////////////////////////////////////////////////////////////
    // SignalReceiver accessors
    // ///////////////////////////////////////////////////////////////////////

    /** @brief Accessor for the SignalReceiver which calls @a SetIsEnabled
      *        with the received value.
      */
    inline SignalReceiver1<bool> const *ReceiverSetIsEnabled ()
    {
        return &m_receiver_set_is_enabled;
    }
    /** @brief Accessor for the SignalReceiver which enables the widget.
      *        Calls @a Enable.
      */
    inline SignalReceiver0 const *ReceiverEnable ()
    {
        return &m_receiver_enable;
    }
    /** @brief Accessor for the SignalReceiver which disables the widget.
      *        Calls @a Disable.
      */
    inline SignalReceiver0 const *ReceiverDisable ()
    {
        return &m_receiver_disable;
    }

    /** @brief Accessor for the SignalReceiver which calls @a SetIsHidden
      *        with the received value.
      */
    inline SignalReceiver1<bool> const *ReceiverSetIsHidden ()
    {
        return &m_receiver_set_is_hidden;
    }
    /** @brief Accessor for the SignalReceiver which hides the widget.
      *        Calls @a Hide.
      */
    inline SignalReceiver0 const *ReceiverHide ()
    {
        return &m_receiver_hide;
    }
    /** @brief Accessor for the SignalReceiver which shows the widget.
      *        Calls @a Show.
      */
    inline SignalReceiver0 const *ReceiverShow ()
    {
        return &m_receiver_show;
    }

    // ///////////////////////////////////////////////////////////////////////
    // accessors
    // ///////////////////////////////////////////////////////////////////////

    /** @brief Returns the name of this widget
      */
    inline std::string const &GetName () const
    {
        return m_name;
    }
    /** @brief Returns a pointer to the parent (const) of this widget.
      */
    inline Widget const *GetParent () const
    {
        return m_parent;
    }
    /** @brief Returns a pointer to the parent (non-const) of this widget.
      */
    inline Widget *GetParent ()
    {
        return m_parent;
    }
    /** Modal widgets behave slightly differently than normal widgets.  Since
      * they must appear above all other widgets, effectively their parent
      * is the top-level parent (see @c Screen), which does some special
      * handling of drawing and events for their proper operation.
      * @brief Returns a pointer to the effective parent (const) of this
      *        widget -- if this is a modal widget, the top-level parent is
      *        returned, otherwise its direct parent.
      */
    Widget const *GetEffectiveParent () const;
    /** Modal widgets behave slightly differently than normal widgets.  Since
      * they must appear above all other widgets, effectively their parent
      * is the top-level parent (see @c Screen), which does some special
      * handling of drawing and events for their proper operation.
      * @brief Returns a pointer to the effective parent (non-const) of this
      *        widget -- if this is a modal widget, the top-level parent is
      *        returned, otherwise its direct parent.
      */
    Widget *GetEffectiveParent ();
    /** @brief Returns a pointer to the top-level parent (const) of this
      *        widget hierarchy (a @c Screen).
      */
    Screen const *GetTopLevelParent () const;
    /** @brief Returns a pointer to the top-level parent (non-const) of this
      *        widget hierarchy (a @c Screen).
      */
    Screen *GetTopLevelParent ();
    /** @brief Returns true iff this is a top-level widget (i.e. it has
      *        no parent).
      */
    inline bool GetIsTopLevelParent () const
    {
        return m_parent == NULL;
    }
    /** @brief Returns a pointer to the focused child widget, or NULL if
      *        no child widget is currently focused.
      */
    inline Widget *GetFocus () const
    {
        return m_focus;
    }
    /** @brief Returns true iff this widget is focused.  If this is a
      *        top-level widget, then it has focus by default.
      */
    inline bool GetIsFocused () const
    {
        return (m_parent != NULL) ?
               m_parent->m_focus == this :
               true;
    }
    /** The member variable @c m_accepts_focus should be set in Widget
      * derivative classes to indicate if they will accept focus.  The
      * default value set by the constructor of Widget is false.
      * @brief Returns true iff this widget even accepts focus.
      */
    inline bool GetAcceptsFocus () const
    {
        return m_accepts_focus;
    }
    /** @brief Returns the currently mouseover-focused child widget,
      *        or NULL if none.
      */
    inline Widget *GetMouseoverFocus () const
    {
        return m_mouseover_focus;
    }
    /** @brief Returns true iff this widget currently has mouseover focus
      *        (i.e. its parent has this widget as its m_mouseover_focus
      *        value).
      */
    bool GetIsMouseover () const;
    /** The member variable @c m_accepts_mouseover should be set in Widget
      * derivative classes to indicate if they will accept mouseover focus.
      * The default value set by the constructor of Widget is true.
      * @brief Returns true iff this widget accepts mouseover focus.
      */
    inline bool GetAcceptsMouseover () const
    {
        return m_accepts_mouseover;
    }
    /** @brief Returns true iff this widget has mouse grab focus (meaning that
      *        all mouse events will go to/through it even if the mouse is not
      *        directly over it).
      */
    inline bool GetIsMouseGrabbed () const
    {
        return m_parent ?
               (GetIsFocused() && m_parent->m_focus_has_mouse_grab) :
               true;
    }
    /** @brief Returns true iff the focused child widget has mouse grab on.
      */
    inline bool GetFocusHasMouseGrab () const
    {
        return m_focus_has_mouse_grab;
    }
    /** @brief Returns true iff this widget is enabled.
      */
    inline bool GetIsEnabled () const
    {
        return m_is_enabled;
    }
    /** @brief Returns true iff this widget is hidden.
      */
    inline bool GetIsHidden () const
    {
        return m_is_hidden;
    }
    /** @brief Returns true iff this widget is currently modal.
      */
    inline bool GetIsModal () const
    {
        return m_is_modal;
    }
    /** @brief Returns the "main widget" child, or NULL if there is
      *        none currently.
      */
    inline Widget *GetMainWidget () const
    {
        return m_main_widget;
    }
    /** @brief Returns the widget stack priority of this widget.
      * @see StackPriority
      */
    inline StackPriority GetStackPriority () const
    {
        return m_stack_priority;
    }
    /** @brief Returns the basic background for this widget.
      * @see WidgetBackground
      */
    inline WidgetBackground *GetBackground () const
    {
        return m_background;
    }
    /** @brief Returns the frame margins for this widget.
      */
    inline ScreenCoordVector2 const &GetFrameMargins () const
    {
        return m_frame_margins;
    }
    /** The content margins are added to the frame margins to indicate the
      * content area of the label.  The content margins can be negative (up to
      * the point that they totally cancel the frame margins).
      * @brief Returns the content margins for this widget.
      */
    inline ScreenCoordVector2 const &GetContentMargins () const
    {
        return m_content_margins;
    }
    /** @brief Returns the last known mouse position (derived from the most
      *        recent mouse motion event received by this widget).
      */
    inline ScreenCoordVector2 const &GetLastMousePosition () const
    {
        return m_last_mouse_position;
    }
    /** @brief Returns the current position of the lower-left corner of
      *        this widget.
      */
    inline ScreenCoordVector2 GetPosition () const
    {
        return m_screen_rect.GetBottomLeft();
    }
    /** @brief Returns the current size of this widget.
      */
    inline ScreenCoordVector2 GetSize () const
    {
        return m_screen_rect.GetSize();
    }
    /** @brief Returns the widget of this widget.
      */
    inline ScreenCoord GetWidth () const
    {
        return m_screen_rect.GetWidth();
    }
    /** @brief Returns the height of this widget.
      */
    inline ScreenCoord GetHeight () const
    {
        return m_screen_rect.GetHeight();
    }
    /** @brief Returns the boolean vector containing the
      *        is-minimum-size-enabled values for width and height in the
      *        x and y components respectively.
      */
    inline Bool2 const &GetMinSizeEnabled () const
    {
        return m_main_widget ?
               m_main_widget->GetMinSizeEnabled() :
               m_size_properties.m_min_size_enabled;
    }
    /** @brief Returns the screen-coordinate vector containing the minimum
      *        width and height in the X and Y components respectively.
      */
    inline ScreenCoordVector2 const &GetMinSize () const
    {
        return m_main_widget ?
               m_main_widget->GetMinSize() :
               m_size_properties.m_min_size;
    }
    /** @brief Returns the boolean vector containing the
      *        is-maximum-size-enabled values for width and height in the
      *        X and Y components respectively.
      */
    inline Bool2 const &GetMaxSizeEnabled () const
    {
        return m_main_widget ?
               m_main_widget->GetMaxSizeEnabled() :
               m_size_properties.m_max_size_enabled;
    }
    /** @brief Returns the screen-coordinate vector containing the maximum
      *        width and height in the X and Y components respectively.
      */
    inline ScreenCoordVector2 const &GetMaxSize () const
    {
        return m_main_widget ?
               m_main_widget->GetMaxSize() :
               m_size_properties.m_max_size;
    }
    /** Adjusts the size vector to satisfy the minimum size properties
      * of this widget.  Then adjusts the resulting value to satisfy
      * the maximum size properties of this widget.  If there is a
      * main widget, then its min/max size properties will be used instead.
      * @brief Returns the adjusted value of the given size vector.
      * @param size The size vector to adjust.
      */
    ScreenCoordVector2 GetAdjustedSize (ScreenCoordVector2 const &size) const;
    /** @brief Returns this widget's screen coordinate rectangle.
      */
    inline ScreenCoordRect const &GetScreenRect () const
    {
        return m_screen_rect;
    }
    /** The content area is the area within the sum of the frame margins
      * and the content margins.  Note that the content margins can be
      * negative, up to the negative of the frame margins.
      * @brief Returns the rectangle representing the content area.
      */
    inline ScreenCoordRect GetContentsRect () const
    {
        return GetScreenRect().GetGrown(-(GetFrameMargins() + GetContentMargins()));
    }
    /** @brief Returns this widget's color mask (the color mask is applied to
      * everything drawn by the widget).
      */
    inline Color const &GetColorMask () const
    {
        return m_color_mask;
    }

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
    /** This function is used to set the minimum-size-to-screen-basis ratio
      * and maximum-size-to-screen-basis for both the X/Y components,
      * separately.
      *
      * This function should be overridden in a subclass when it is necessary
      * to know when the min/max-size-to-screen-basis ratio of a widget is
      * changed.
      * @see Layout::SetSizePropertyRatio
      *
      * @brief Sets a single component of the min/max-size-to-screen-basis
      *        ratio of this widget.
      * @param property Indicates which property is to be set (MIN or MAX).
      * @param component Can only be either Dim::X or Dim::Y (0 or 1,
      *                  respectively).
      * @param ratio The fraction of the screen basis indicating the
      *              new value of the specified property.
      * @param defer_parent_update This parameter is likely to be moved out
      *                            of the public API.
      */
    virtual void SetSizePropertyRatio (
        SizeProperties::Property property,
        Uint32 component,
        Float ratio,
        bool defer_parent_update = false);
    /** This function is used to set the minimum-size-to-screen-basis ratio
      * and maximum-size-to-screen-basis for both the X/Y components at once.
      *
      * This function should be overridden in a subclass when it is necessary
      * to know when the min/max-size-to-screen-basis ratio of a widget is
      * changed.
      * @see Layout::SetSizePropertyRatios
      *
      * @brief Sets both components of the min/max-size-to-screen-basis
      *        ratio vector of this widget.
      * @param property Indicates which property is to be set (MIN or MAX).
      * @param component Can only be either Dim::X or Dim::Y (0 or 1,
      *                  respectively).
      * @param ratio The vector of component-wise fractions of the screen
      *              basis indicating the new value of the specified ratio
      *              vector.
      * @param defer_parent_update This parameter is likely to be moved out
      *                            of the public API.
      */
    virtual void SetSizePropertyRatios (
        SizeProperties::Property property,
        FloatVector2 const &ratios,
        bool defer_parent_update = false);

    /** @brief Sets the color mask of this widget.
      */
    void SetColorMask (Color const &color_mask)
    {
        m_color_mask = color_mask;
    }

    /** Makes the call to @a AddModalWidget or @a RemoveModalWidget as
      * appropriate).  If this causes the widget to become not modal, then
      * SetIsHidden(true) is called.
      * @brief Sets the modal state of this widget.
      */
    void SetIsModal (bool is_modal);
    /** Attempts to resize the given main widget to the size of this widget.
      * If NULL is given, then the main widget is cleared.
      * @brief Sets the main [child] widget for this widget.
      */
    virtual void SetMainWidget (Widget *main_widget);
    /** Will cause a reshuffling of child widgets in this widget's parent's
      * child vector, in order to maintain proper widget stack priority
      * ordering.
      * @brief Sets the widget's stack priority.
      */
    void SetStackPriority (StackPriority stack_priority);
    /** @brief Sets the basic background for this widget, NULL for no background
      *        (completely transparent).
      */
    void SetBackground (WidgetBackground *background);
    /** @brief Sets the frame margins for this widget (in direct screen
      *        coordinates).
      */
    void SetFrameMargins (ScreenCoordVector2 const &frame_margins);
    /** Actually just calls @a SetFrameMargins with the product of the given
      * ratios and the screen basis.
      * @brief Sets the frame margins for this widget via screen-basis ratios.
      * @see Screen::GetSizeRatioBasis
      */
    void SetFrameMarginRatios (FloatVector2 const &frame_margin_ratios);
    /** @brief Sets the content margins for this widget (in direct screen coordinates).
      */
    void SetContentMargins (ScreenCoordVector2 const &content_margins);
    /** Actually just calls @a SetContentMargins with the product of the given
      * ratios and the screen basis.
      * @brief Sets the content margins for this widget via screen-basis ratios.
      * @see Screen::GetSizeRatioBasis
      */
    void SetContentMarginRatios (FloatVector2 const &content_margin_ratios);

    // ///////////////////////////////////////////////////////////////////////
    // procedures
    // ///////////////////////////////////////////////////////////////////////

    /** @brief Sets the min and max sizes to the given size, and then enables
      *        min/max size enabled properties for both X and Y components.
      */
    void FixSize (ScreenCoordVector2 const &size);
    /** @brief Sets the min and max sizes to the given size, and then enables
      *        min/max size enabled properties for both X and Y components --
      *        the size is given in a screen-basis ratio vector.
      */
    void FixSizeRatios (FloatVector2 const &size_ratios);
    /** @brief Disables min and max size enabled properties for both X and Y
      *        components.
      */
    void UnfixSize ();

    /** @brief Sets the min and max width to the given width, and then enables
      *        min/max size enabled properties for the X component only.
      */
    void FixWidth (ScreenCoord width);
    /** @brief Sets the min and max widths to the given size, and then enables
      *        min/max size enabled properties for the X component only --
      *        the width is given as a screen-basis ratio.
      */
    void FixWidthRatio (Float width_ratio);
    /** @brief Disables min and max size enabled properties for the X
      *        component only.
      */
    void UnfixWidth ();

    /** @brief Sets the min and max height to the given height, and then
      *        enables min/max size enabled properties for the Y component
      *        only.
      */
    void FixHeight (ScreenCoord height);
    /** @brief Sets the min and max heights to the given size, and then
      *        enables min/max size enabled properties for the Y component
      *        only -- the height is given as a screen-basis ratio.
      */
    void FixHeightRatio (Float height_ratio);
    /** @brief Disables min and max size enabled properties for the Y
      *        component only.
      */
    void UnfixHeight ();

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
    /** @brief Move this widget to the given position.
      * @param position The intended position of the lower-left corner of
      *                 this widget.
      */
    void MoveTo (ScreenCoordVector2 const &position);
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
    /** @brief A screen-basis-ratio based frontend to @a Resize.
      * @return The resulting actual size of this widget.
      */
    ScreenCoordVector2 ResizeByRatios (FloatVector2 const &ratios);
    /** @brief Attempts to move and resize this widget so that its
      *        screen rectangle matches the one given.
      * @return The resulting actual size of this widget.
      */
    ScreenCoordVector2 MoveToAndResize (ScreenCoordRect const &screen_rect);
    /** @brief Centers this widget on the center of the given widget.
      */
    void CenterOnWidget (Widget const *widget);
    /** Widgets that are not direct ancestors of this widget will be defocused
      * before any focusing happens.
      * @brief Focuses this widget with respect to its parent widget.
      * @return True iff it (or any of its children) accepted focus.
      */
    virtual bool Focus ();
    /** The parent widget remains focused.
      * @brief Unfocuses this widget with respect to its parent widget.
      */
    virtual void Unfocus ();
    /** All mouse events will be exclusively sent to this widget via its
      * ancestors.
      * @brief Grabs the mouse input.
      */
    virtual void GrabMouse ();
    /** Other widgets may now receive mouse events.
      * @brief Lets go of the mouse input.
      */
    virtual void UnGrabMouse ();
    /** The given @c child widget must not currently have a parent widget.
      * @brief Attaches the given widget as a child of this widget.
      */
    virtual void AttachChild (Widget *child);
    /** The given @c child widget must be a child of this widget.
      * @brief Detaches the given widget from this widget.
      */
    virtual void DetachChild (Widget *child);
    /** This widget must have a parent widget.
      * @brief Detaches this widget from its parent.
      */
    virtual void DetachFromParent ();
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

    /** Disabled widgets will not accept events.
      * @brief Sets the enabled state of this widget.
      */
    virtual void SetIsEnabled (bool is_enabled);
    /** @brief Enables this widget.
      */
    inline void Enable () { SetIsEnabled(true); }
    /** Disabled widgets will not accept events.
      * @brief Disables this widget.
      */
    inline void Disable () { SetIsEnabled(false); }

    /** Must not currently be a modal widget.
      * @brief Toggles the hidden state.
      */
    void ToggleIsHidden ();
    /** If this widget is a child in a layout widget, then this will cause
      * the widget to be ignored by the layout (and may cause relative
      * repositioning of the other Layout child widgets).
      *
      * This widget must not currently be a modal widget.
      *
      * @brief Sets the hidden state of this widget.
      * @param is_hidden The new value of the hidden property.
      */
    void SetIsHidden (bool is_hidden);
    /** Must not currently be a modal widget.
      * @brief Hides this widget.
      */
    inline void Hide () { SetIsHidden(true); }
    /** Must not currently be a modal widget.
      * @brief Shows this widget.
      */
    inline void Show () { SetIsHidden(false); }

protected:

    /** @brief This implementation was required by @c WidgetSkinHandler.
      * @see WidgetSkinHandler::GetWidgetSkinHandlerChildCount
      */
    virtual Uint32 GetWidgetSkinHandlerChildCount () const;
    /** @brief This implementation was required by @c WidgetSkinHandler.
      * @see WidgetSkinHandler::GetWidgetSkinHandlerChild
      */
    virtual WidgetSkinHandler *GetWidgetSkinHandlerChild (Uint32 index);
    /** @brief This implementation was required by @c WidgetSkinHandler.
      * @see WidgetSkinHandler::GetWidgetSkinHandlerParent
      */
    virtual WidgetSkinHandler *GetWidgetSkinHandlerParent ();

    // TODO document
    // these are interfaces for container widgets
    virtual Bool2 GetContentsMinSizeEnabled () const;
    virtual ScreenCoordVector2 GetContentsMinSize () const;
    virtual Bool2 GetContentsMaxSizeEnabled () const;
    virtual ScreenCoordVector2 GetContentsMaxSize () const;

    /** @brief Returns the a pointer to the background object which is
      *        rendered in @a Widget::Draw.
      */
    inline WidgetBackground const *GetRenderBackground () const
    {
        return m_render_background;
    }

    /** @brief Sets the background to use in @a Widget::Draw.
      */
    inline void SetRenderBackground (WidgetBackground const *render_background)
    {
        m_render_background = render_background;
    }

    /** @brief Called only by Widget and Screen after they get a WidgetSkin.
      */
    void InitializeFromWidgetSkinProperties ();

    /** Calls FrameHandler::ProcessFrame on all child widgets.
      *
      * This function is guaranteed to be called once per game loop frame,
      * unlike @a Widget::Draw.
      *
      * @brief Override of @a FrameHandler::ProcessFrameOverride which does
      *        anything that needs to be done once per iteration of the game
      *        loop.
      */
    virtual void ProcessFrameOverride ();
    /** Subclasses of widget <strong>should not</strong> override this
      * function -- all of the user interface behavior for Widget is
      * handled/delegated here.
      * @brief Override of @a EventHandler::ProcessEventOverride which does
      *        generic event processing.
      */
    virtual bool ProcessEventOverride (Event const *e);
    /** Subclasses may override this to process key events.
      * @brief Process a key event.
      */
    virtual bool ProcessKeyEvent (EventKey const *e) { return false; }
    /** Subclasses may override this to process mouse button events.
      * @brief Process a mouse button event.
      */
    virtual bool ProcessMouseButtonEvent (EventMouseButton const *e) { return false; }
    /** Subclasses may override this to process mouse wheel events.
      * @brief Process a mouse wheel event.
      */
    virtual bool ProcessMouseWheelEvent (EventMouseWheel const *e) { return false; }
    /** Subclasses may override this to process motion events.
      * @brief Process a mouse motion event.
      */
    virtual bool ProcessMouseMotionEvent (EventMouseMotion const *e) { return false; }
    /** Subclasses may override this to process joystick events.
      * @brief Process a joystick event.
      */
    virtual bool ProcessJoyEvent (EventJoy const *e) { return false; }
    /** Subclasses may override this to process StateMachineInput events.
      * @brief Process a StateMachineInput event.
      */
    virtual bool ProcessStateMachineInputEvent (EventStateMachineInput const *e) { return false; }
    /** Subclasses may override this to process custom events -- events not
      * explicitly handled by the above overridable handlers.
      * @brief Process a custom event.
      */
    virtual bool ProcessCustomEvent (EventCustom const *e) { return false; }
    /** @brief Processes a delete child widget event.  This is used mainly to
      *        delete modal widgets.
      */
    bool ProcessDeleteChildWidgetEvent (EventDeleteChildWidget const *e);

    /** Subclasses should override this when they need to do something when
      * the widget gains focus.
      * @brief Handler that is called when this widget becomes focused.
      */
    virtual void HandleFocus () { }
    /** Subclasses should override this when they need to do something when
      * the widget loses focus.
      * @brief Handler that is called when this widget becomes unfocused.
      */
    virtual void HandleUnfocus () { }
    /** Subclasses should override this when they need to do something when
      * the mouse moves onto the widget.
      * @brief Handler that is called when the mouseover flag is set
      *        ON (from off).
      */
    virtual void HandleMouseoverOn () { }
    /** Subclasses should override this when they need to do something when
      * the mouse moves off the widget.
      * @brief Handler that is called when the mouseover flag is set
      *        OFF (from on).
      */
    virtual void HandleMouseoverOff () { }
    /** Subclasses should override this when they need to do something when
      * the mouse input grab is turned on.
      * @brief Handler that is called when this widget grabs the mouse input.
      */
    virtual void HandleMouseGrabOn () { }
    /** Subclasses should override this when they need to do something when
      * the mouse input grab is turned off.
      * @brief Handler that is called when this widget lets go of the
      *        mouse input.
      */
    virtual void HandleMouseGrabOff () { }

    /** Subclasses should override this when they need to do something when
      * the basic render background object has changed.
      * @brief Handler that is called when the basic background object is
      *        changed.
      * @note If a subclass overrides this function, it should call
      *       @a Widget::HandleChangedBackground -- or the
      *       HandleChangedBackground belonging to its superclass, if
      *       applicable.
      */
    virtual void HandleChangedBackground ();
    /** Subclasses should override this when they need to do something when
      * the frame margins have been changed.
      * @see Layout
      * @brief Handler that is called when the frame margins have been
      *        changed.
      */
    virtual void HandleChangedFrameMargins () { }
    /** Subclasses should override this when they need to do something when
      * the content margins have been changed.
      * @see Layout
      * @brief Handler that is called when the content margins have been
      *        changed.
      */
    virtual void HandleChangedContentMargins () { }

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
    /** Subclasses may override this function to implement custom background
      * behavior such as mouseover highlighting and button-depressing.
      * @brief Assigns the basic widget background to the render background.
      */
    virtual void UpdateRenderBackground ();

    /** @brief Calls ChildSizePropertiesChanged on this widget's parent,
      *        with the "this" pointer as the parameter.
      * @param defer_parent_update If this is false, nothing is done.
      *                            This is used primarily when there is going
      *                            to be lots of updates in a row, and all but
      *                            the last update don't matter.
      */
    void ParentChildSizePropertiesUpdate (bool defer_parent_update);
    /** @brief Adjusts *size by the minimum size.
      * @return True iff *size was changed.
      */
    bool AdjustFromMinSize (ScreenCoordRect *screen_rect) const;
    /** @brief Adjusts *size by the maximum size.
      * @return True iff *size was changed.
      */
    bool AdjustFromMaxSize (ScreenCoordRect *screen_rect) const;
    /** @brief Performs range checking to ensure the min and max sizes
      *        are valid after the minimum size was changed.
      */
    void MinSizeUpdated ();
    /** @brief Performs range checking to ensure the min and max sizes
      *        are valid after the maximum size was changed.
      */
    void MaxSizeUpdated ();
    /** @brief Ensures the size of the given rectangle is non-negative
      *        in both dimensions.
      */
    void SizeRangeAdjustment (ScreenCoordRect *rect) const;

    typedef std::vector<Widget *> WidgetVector;
    typedef WidgetVector::iterator WidgetVectorIterator;
    typedef WidgetVector::const_iterator WidgetVectorConstIterator;
    typedef WidgetVector::reverse_iterator WidgetVectorReverseIterator;

    /** Stored in back-to-front drawing order (back-most having index 0).
      * @brief Ordered container of child widgets.
      */
    WidgetVector m_child_vector;
    /** @brief Contains the min/max and min/max-enabled size
      *        properties of this widget.
      */
    SizeProperties m_size_properties;
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
    /** A value of false <strong>will</strong> prevent container widgets
      * from blocking child widgets from gaining focus.
      * @brief Indicates if this widget accepts focus on its own.
      */
    bool m_accepts_focus;
    /** A value of false <strong>will not</strong> prevent container widgets
      * from blocking child widgets from accepting mouseover.
      * @brief Indicates if this widget accepts the mouseover flag on its own.
      */
    bool m_accepts_mouseover;
    /** This mask is applied by its parent for the call to @c Draw.
      * @brief The color mask of the widget.
      */
    Color m_color_mask;

private:

    typedef std::list<Widget *> WidgetList;
    typedef WidgetList::iterator WidgetListIterator;
    typedef WidgetList::const_iterator WidgetListConstIterator;
    typedef WidgetList::reverse_iterator WidgetListReverseIterator;

    /** @brief Returns a WidgetVectorIterator which matches the given
      *        child in m_child_vector.
      */
    WidgetVectorIterator FindChildWidget (Widget const *child);
    /** @brief Focuses all widgets in this widget's line of parency,
      *        from top down.
      */
    void FocusWidgetLine ();
    /** @brief Unfocuses all widgets from this one down, starting at
      *        the lowest child .
      *
      */
    void UnfocusWidgetLine ();
    /** @brief Changes the mouseover to this widget.
      * @return True iff a widget accepted mouseover.
      */
    bool MouseoverOn ();
    /** @brief Removes mouseover from this widget.
      */
    void MouseoverOff ();
    /** @brief Mouseover-ons all widgets in this widget's
      *        line of parency, top down.
      */
    void MouseoverOnWidgetLine ();
    /** @brief Mouseover-offs all widgets from this one
      *        down, starting at lowest child, going up.
      */
    void MouseoverOffWidgetLine ();
    /** @brief Performs some necessary event processing on mouse
      *        events before the mouse event handler gets them.
      */
    bool PreprocessMouseEvent (EventMouse const *e);
    /** @brief Performs some necessary event processing on mouse wheel
      *        events before the mouse wheel event handler gets them.
      */
    bool PreprocessMouseWheelEvent (EventMouseWheel const *e);
    /** @brief Performs some necessary event processing on focus events.
      */
    bool PreprocessFocusEvent (EventFocus const *e);
    /** @brief Performs some necessary event processing on mouseover events.
      */
    bool PreprocessMouseoverEvent (EventMouseover const *e);
    /** @brief A convenience function for sending a mouse event to the
      *        child widget highest in m_child_vector which lies underneath
      *        the mouse event position.
      * @return True iff the mouse event was accepted by any of the children.
      */
    bool SendMouseEventToChild (EventMouse const *e);

    /** Currently only used for debugging purposes
      * @brief Textual name of this instance of the widget.
      */
    std::string m_name;
    /** NULL indicates that this is a top-level widget (or a parentless one
      * hanging out in limbo).
      * @brief Pointer to the parent widget.
      */
    Widget *m_parent;
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
    /** @brief Indicates if this widget is enabled.
      */
    bool m_is_enabled;
    /** @brief Stores the SignalSender-blocking state while the widget
      *        is disabled.
      */
    bool m_enabled_sender_blocking_state;
    /** @brief Iff true, this widget is hidden.
      */
    bool m_is_hidden;
    /** @brief The screen-coordinate rectangle which defines the size
      *        and position of this widget.
      */
    ScreenCoordRect m_screen_rect;
    /** @brief Last known mouse position (from processed mouse motion events).
      */
    ScreenCoordVector2 m_last_mouse_position;
    /** @brief Iff true, this is a modal widget.
      */
    bool m_is_modal;
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
    /** Indicates which block this widget will remain in inside
      * m_child_vector.
      * @brief Widget stack priority.
      */
    StackPriority m_stack_priority;
    /** @brief the basic background for the widget
      */
    WidgetBackground *m_background;
    //
    /** Subclasses can use this to specify different backgrounds for different
      * behaviors.  NULL indicates no background will be rendered
      * (transparent).
      * @brief The background which will be rendered in @c Draw .
      */
    WidgetBackground const *m_render_background;
    /** @brief Contains the frame margins which are used by the background
      *        and various other Widget subclasses for drawing and layout.
      */
    ScreenCoordVector2 m_frame_margins;
    /** @brief Contains the content margins which are used by various widgets.
      */
    ScreenCoordVector2 m_content_margins;

    // ///////////////////////////////////////////////////////////////////////
    // SignalReceivers
    // ///////////////////////////////////////////////////////////////////////

    /** @brief SignalReceiver which calls @a SetIsEnabled with the
      *        received value.
      */
    SignalReceiver1<bool> m_receiver_set_is_enabled;
    /** @brief SignalReceiver which calls @a Enable.
      */
    SignalReceiver0 m_receiver_enable;
    /** @brief SignalReceiver which calls @a Disable .
      */
    SignalReceiver0 m_receiver_disable;

    /** @brief SignalReceiver which calls @a SetIsHidden with the
      *        received value.
      */
    SignalReceiver1<bool> m_receiver_set_is_hidden;
    /** @brief SignalReceiver which calls @a Hide .
      */
    SignalReceiver0 m_receiver_hide;
    /** @brief SignalReceiver which calls @a Show .
      */
    SignalReceiver0 m_receiver_show;
}; // end of class Widget

} // end of namespace Xrb

#endif // !defined(_XRB_WIDGET_H_)

