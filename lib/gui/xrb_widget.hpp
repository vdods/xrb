// ///////////////////////////////////////////////////////////////////////////
// xrb_widget.hpp by Victor Dods, created 2004/09/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_WIDGET_HPP_)
#define _XRB_WIDGET_HPP_

#include "xrb.hpp"

#include <string>

#include "xrb_color.hpp"
#include "xrb_eventhandler.hpp"
#include "xrb_framehandler.hpp"
#include "xrb_margins.hpp"
#include "xrb_rect.hpp"
#include "xrb_rendercontext.hpp"
#include "xrb_resourcelibrary.hpp"
#include "xrb_screencoord.hpp"
#include "xrb_signalhandler.hpp"
#include "xrb_sizeproperties.hpp"
#include "xrb_vector.hpp"
#include "xrb_widgetskin.hpp"
#include "xrb_widgetskinhandler.hpp"

/** @file xrb_widget.h
  * @brief Contains the class declaration of the foundation of the GUI
  *        system of XuqRijBuh -- Widget.
  */

namespace Xrb {

class ContainerWidget;
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
class EventPinch;
class EventPinchBegin;
class EventPinchEnd;
class EventPinchMotion;
class EventRotate;
class EventRotateBegin;
class EventRotateEnd;
class EventRotateMotion;
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
  * on the screen) is created to suit the task at hand.  Widgets
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
      * @param name A textual label applied to the specific widget being
      *             constructed.  As for now, this is only used in a debug-
      *             related capacity.
      */
    Widget (std::string const &name = "Widget");
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
    SignalReceiver1<bool> const *ReceiverSetIsEnabled () { return &m_receiver_set_is_enabled; }
    /** @brief Accessor for the SignalReceiver which enables the widget.
      *        Calls @a Enable.
      */
    SignalReceiver0 const *ReceiverEnable () { return &m_receiver_enable; }
    /** @brief Accessor for the SignalReceiver which disables the widget.
      *        Calls @a Disable.
      */
    SignalReceiver0 const *ReceiverDisable () { return &m_receiver_disable; }

    /** @brief Accessor for the SignalReceiver which calls @a SetIsHidden
      *        with the received value.
      */
    SignalReceiver1<bool> const *ReceiverSetIsHidden () { return &m_receiver_set_is_hidden; }
    /** @brief Accessor for the SignalReceiver which hides the widget.
      *        Calls @a Hide.
      */
    SignalReceiver0 const *ReceiverHide () { return &m_receiver_hide; }
    /** @brief Accessor for the SignalReceiver which shows the widget.
      *        Calls @a Show.
      */
    SignalReceiver0 const *ReceiverShow () { return &m_receiver_show; }

    // ///////////////////////////////////////////////////////////////////////
    // accessors
    // ///////////////////////////////////////////////////////////////////////

    /** @brief Returns the name of this widget
      */
    std::string const &Name () const { return m_name; }
    /** @brief Returns a pointer to the parent (const) of this widget.
      */
    ContainerWidget const *Parent () const { return m_parent; }
    /** @brief Returns a pointer to the parent (non-const) of this widget.
      */
    ContainerWidget *Parent () { return m_parent; }
    /** Modal widgets behave slightly differently than normal widgets.  Since
      * they must appear above all other widgets, effectively their parent
      * is the root widget (see @c Screen), which does some special
      * handling of drawing and events for their proper operation.
      * @brief Returns a pointer to the effective parent (const) of this
      *        widget -- if this is a modal widget, the root widget is
      *        returned, otherwise its direct parent.
      */
    ContainerWidget const *EffectiveParent () const;
    /** Modal widgets behave slightly differently than normal widgets.  Since
      * they must appear above all other widgets, effectively their parent
      * is the root widget (see @c Screen), which does some special
      * handling of drawing and events for their proper operation.
      * @brief Returns a pointer to the effective parent (non-const) of this
      *        widget -- if this is a modal widget, the root widget is
      *        returned, otherwise its direct parent.
      */
    ContainerWidget *EffectiveParent ();
    
    /// Returns true iff this Widget has no parent.
    bool IsRootWidget () const { return m_parent == NULL; }
    /// Returns a pointer to the highest ancestor Widget, which is 'this' if this Widget has no parent.
    Widget const &RootWidget () const;
    /// Returns a pointer to the highest ancestor Widget, which is 'this' if this Widget has no parent.
    Widget &RootWidget ();

    /// Returns true iff this Widget's root widget is a Screen.
    bool RootWidgetIsScreen () const;
    /// Does a dynamic_cast on the result of RootWidget -- so in particular, if the root widget
    /// is not actually a Screen, it will throw a bad_cast exception.
    Screen const &RootWidgetAsScreen () const;
    /// Does a dynamic_cast on the result of RootWidget -- so in particular, if the root widget
    /// is not actually a Screen, it will throw a bad_cast exception.
    Screen &RootWidgetAsScreen ();
    
    /** @brief Returns true iff this widget is focused.  If this is a
      *        top-level widget, then it has focus by default.
      */
    bool IsFocused () const;
    /** The member variable @c m_accepts_focus should be set in Widget
      * derivative classes to indicate if they will accept focus.  The
      * default value set by the constructor of Widget is false.
      * @brief Returns true iff this widget even accepts focus.
      */
    bool AcceptsFocus () const { return m_accepts_focus; }
    /** @brief Returns true iff this widget currently has mouseover focus
      *        (i.e. its parent has this widget as its m_mouseover_focus
      *        value).
      */
    bool IsMouseover () const;
    /** The member variable @c m_accepts_mouseover should be set in Widget
      * derivative classes to indicate if they will accept mouseover focus.
      * The default value set by the constructor of Widget is true.
      * @brief Returns true iff this widget accepts mouseover focus.
      */
    bool AcceptsMouseover () const { return m_accepts_mouseover; }
    /** @brief Returns true iff this widget has mouse grab focus (meaning that
      *        all mouse events will go to/through it even if the mouse is not
      *        directly over it).
      */
    bool IsMouseGrabbed () const;
    /** @brief Returns true iff this widget is enabled.
      */
    bool IsEnabled () const { return m_is_enabled; }
    /** @brief Returns true iff this widget is hidden.
      */
    bool IsHidden () const { return m_is_hidden; }
    /** @brief Returns true iff this widget is currently modal.
      */
    bool IsModal () const { return m_is_modal; }
    /** @brief Returns the widget stack priority of this widget.
      * @see StackPriority
      */
    StackPriority GetStackPriority () const { return m_stack_priority; }
    /** @brief Returns the basic background for this widget.
      * @see WidgetBackground
      */
    WidgetBackground *Background () const { return m_background; }
    /** @brief Returns the frame margins for this widget.
      */
    ScreenCoordMargins const &FrameMargins () const { return m_frame_margins; }
    /** The content margins are added to the frame margins to indicate the
      * content area of the label.  The content margins can be negative (up to
      * the point that they totally cancel the frame margins).
      * @brief Returns the content margins for this widget.
      */
    ScreenCoordMargins const &ContentMargins () const { return m_content_margins; }
    /** @brief Returns the last known mouse position (derived from the most
      *        recent mouse motion event received by this widget).
      */
    ScreenCoordVector2 const &LastMousePosition () const { return m_last_mouse_position; }
    /** @brief Returns the current position of the lower-left corner of
      *        this widget.
      */
    ScreenCoordVector2 Position () const { return m_screen_rect.BottomLeft(); }
    /** Integer division is involved, so rounding may occur.
      * @brief Returns the current position of the center of this widget.
      */
    ScreenCoordVector2 Center () const { return m_screen_rect.Center(); }
    /** @brief Returns the current size of this widget.
      */
    ScreenCoordVector2 Size () const { return m_screen_rect.Size(); }
    /** @brief Returns the widget of this widget.
      */
    ScreenCoord Width () const { return m_screen_rect.Width(); }
    /** @brief Returns the height of this widget.
      */
    ScreenCoord Height () const { return m_screen_rect.Height(); }
    /// Used for font pixel height calculations.  Gives the "useful" size of this Widget.
    ScreenCoord SizeRatioBasis () const { return Min(Width(), Height()); }
    /** @brief Returns the boolean vector containing the
      *        is-minimum-size-enabled values for width and height in the
      *        x and y components respectively.
      */
    virtual Bool2 const &MinSizeEnabled () const { return m_size_properties.m_min_size_enabled; }
    /** @brief Returns the screen-coordinate vector containing the minimum
      *        width and height in the X and Y components respectively.
      */
    virtual ScreenCoordVector2 const &MinSize () const { return m_size_properties.m_min_size; }
    /** @brief Returns the boolean vector containing the
      *        is-maximum-size-enabled values for width and height in the
      *        X and Y components respectively.
      */
    virtual Bool2 const &MaxSizeEnabled () const { return m_size_properties.m_max_size_enabled; }
    /** @brief Returns the screen-coordinate vector containing the maximum
      *        width and height in the X and Y components respectively.
      */
    virtual ScreenCoordVector2 const &MaxSize () const { return m_size_properties.m_max_size; }
    /** Adjusts the size vector to satisfy the minimum size properties
      * of this widget.  Then adjusts the resulting value to satisfy
      * the maximum size properties of this widget.  If there is a
      * main widget, then its min/max size properties will be used instead.
      * @brief Returns the adjusted value of the given size vector.
      * @param size The size vector to adjust.
      */
    virtual ScreenCoordVector2 AdjustedSize (ScreenCoordVector2 const &size) const;
    /** @brief Returns this widget's screen coordinate rectangle.
      */
    ScreenCoordRect const &ScreenRect () const { return m_screen_rect; }
    /** The content area is the area within the sum of the frame margins
      * and the content margins.  Note that the content margins can be
      * negative, up to the negative of the frame margins.
      * @brief Returns the rectangle representing the content area.
      */
    ScreenCoordRect ContentsRect () const;
    /** @brief Returns this widget's color bias (the color bias is applied to
      * everything drawn by the widget, AFTER the color mask).
      */
    Color const &ColorBias () const { return m_color_bias; }
    /** @brief Returns this widget's color mask (the color mask is applied to
      * everything drawn by the widget).
      */
    Color const &ColorMask () const { return m_color_mask; }
    /** Use this method to change the color bias
      * @brief Returns this widget's color bias as a non-const reference (the
      * color bias is applied to everything drawn by the widget, AFTER the
      * color mask).
      */
    Color &ColorBias () { return m_color_bias; }
    /** Use this method to change the color mask.
      * @brief Returns this widget's color mask as a non-const reference (the
      * color mask is applied to everything drawn by the widget).
      */
    Color &ColorMask () { return m_color_mask; }

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
    void SetSizePropertyRatio (
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
    void SetSizePropertyRatios (
        SizeProperties::Property property,
        FloatVector2 const &ratios,
        bool defer_parent_update = false);

    /** Makes the call to @a AddModalWidget or @a RemoveModalWidget as
      * appropriate).  If this causes the widget to become not modal, then
      * SetIsHidden(true) is called.
      * @brief Sets the modal state of this widget.
      */
    void SetIsModal (bool is_modal);
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
    void SetFrameMargins (ScreenCoordMargins const &frame_margins);
    /** Actually just calls @a SetFrameMargins with the product of the given
      * ratios and the screen basis.
      * @brief Sets the frame margins for this widget via screen-basis ratios.
      * @see Screen::SizeRatioBasis
      */
    void SetFrameMarginRatios (FloatMargins const &frame_margin_ratios);
    /** @brief Sets the content margins for this widget (in direct screen coordinates).
      */
    void SetContentMargins (ScreenCoordMargins const &content_margins);
    /** Actually just calls @a SetContentMargins with the product of the given
      * ratios and the screen basis.
      * @brief Sets the content margins for this widget via screen-basis ratios.
      * @see Screen::SizeRatioBasis
      */
    void SetContentMarginRatios (FloatMargins const &content_margin_ratios);

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
    void CenterOnWidget (Widget const &widget);
    /** Widgets that are not direct ancestors of this widget will be defocused
      * before any focusing happens.
      * @brief Focuses this widget with respect to its parent widget.
      * @return True iff it (or any of its children) accepted focus.
      */
    bool Focus ();
    /** The parent widget remains focused.
      * @brief Unfocuses this widget with respect to its parent widget.
      */
    void Unfocus ();
    /** All mouse events will be exclusively sent to this widget via its
      * ancestors.
      * @brief Grabs the mouse input.
      */
    void GrabMouse ();
    /** Other widgets may now receive mouse events.
      * @brief Lets go of the mouse input.
      */
    void UnGrabMouse ();
    /** This widget must have a parent widget.
      * @brief Detaches this widget from its parent.
      */
    void DetachFromParent ();

    /** Disabled widgets will not accept events.
      * @brief Sets the enabled state of this widget.
      */
    virtual void SetIsEnabled (bool is_enabled);
    /** @brief Enables this widget.
      */
    void Enable () { SetIsEnabled(true); }
    /** Disabled widgets will not accept events.
      * @brief Disables this widget.
      */
    void Disable () { SetIsEnabled(false); }

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
    void Hide () { SetIsHidden(true); }
    /** Must not currently be a modal widget.
      * @brief Shows this widget.
      */
    void Show () { SetIsHidden(false); }

protected:

    virtual void HandleChangedWidgetSkin ();

    virtual ScreenCoord WidgetSkinHandlerSizeRatioBasis () const { return RootWidget().SizeRatioBasis(); }
    
    /** @brief This implementation was required by @c WidgetSkinHandler.
      * @see WidgetSkinHandler::WidgetSkinHandlerChildCount
      */
    virtual Uint32 WidgetSkinHandlerChildCount () const { return 0; }
    /** @brief This implementation was required by @c WidgetSkinHandler.
      * @see WidgetSkinHandler::WidgetSkinHandlerChild
      */
    virtual WidgetSkinHandler *WidgetSkinHandlerChild (Uint32 index) { return NULL; }
    /** @brief This implementation was required by @c WidgetSkinHandler.
      * @see WidgetSkinHandler::WidgetSkinHandlerParent
      */
    virtual WidgetSkinHandler *WidgetSkinHandlerParent ();

    /** @brief Returns the a pointer to the background object which is
      *        rendered in @a Widget::Draw.
      */
    WidgetBackground const *RenderBackground () const { return m_render_background; }

    /** @brief Sets the background to use in @a Widget::Draw.
      */
    void SetRenderBackground (WidgetBackground const *render_background) { m_render_background = render_background; }

    /** Calls FrameHandler::ProcessFrame on all child widgets.
      *
      * This function is guaranteed to be called once per game loop frame,
      * unlike @a Widget::Draw.
      *
      * @brief Override of @a FrameHandler::HandleFrame which does
      *        anything that needs to be done once per iteration of the game
      *        loop.
      */
    virtual void HandleFrame () { }
    /** Subclasses of widget <strong>should not</strong> override this
      * function -- all of the user interface behavior for Widget is
      * handled/delegated here.
      * @brief Override of @a EventHandler::HandleEvent which does
      *        generic event processing.
      */
    virtual bool HandleEvent (Event const *e);
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
    /** Subclasses may override this to process pinch begin events.
      * @brief Process a pinch begin event.
      */
    virtual bool ProcessPinchBeginEvent (EventPinchBegin const *e) { return false; }
    /** Subclasses may override this to process pinch end events.
      * @brief Process a pinch end event.
      */
    virtual bool ProcessPinchEndEvent (EventPinchEnd const *e) { return false; }
    /** Subclasses may override this to process pinch motion events.
      * @brief Process a pinch motion event.
      */
    virtual bool ProcessPinchMotionEvent (EventPinchMotion const *e) { return false; }
    /** Subclasses may override this to process rotate motion events.
      * @brief Process a rotate motion event.
      */
    virtual bool ProcessRotateMotionEvent (EventRotateMotion const *e) { return false; }
    /** Subclasses may override this to process rotate begin events.
      * @brief Process a rotate begin event.
      */
    virtual bool ProcessRotateBeginEvent (EventRotateBegin const *e) { return false; }
    /** Subclasses may override this to process rotate end events.
      * @brief Process a rotate end event.
      */
    virtual bool ProcessRotateEndEvent (EventRotateEnd const *e) { return false; }
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
    virtual bool ProcessDeleteChildWidgetEvent (EventDeleteChildWidget const *e);

    /** Subclasses should override this when they need to do something when
      * @c ContainerWidget::AttachChild is called on this.  This method will
      * be called after the child is attached (so in particular, Parent() is
      * not NULL).
      * @brief Handler that is called when this widget is attached to a parent.
      */
    virtual void HandleAttachedToParent () { }
    /** Subclasses should override this when they need to do something when
      * @c ContainerWidget::DetachChild is called on this.  This method will
      * be called before the child is detached (so in particular, Parent() is
      * not NULL).
      * @brief Handler that is called when this widget is detached from a parent.
      */
    virtual void HandleAboutToDetachFromParent () { }
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

    /** @brief Contains the min/max and min/max-enabled size
      *        properties of this widget.
      */
    SizeProperties m_size_properties;
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
    /** The color bias represents a blending function which is applied to
      * each drawing operation at or below this widget in the widget hierarchy.
      * @brief The color bias of the widget.
      */
    Color m_color_bias;
    /** The color mask represents a modulation function (simple multiplication)
      * which is applied to each drawing operation at or below this widget in
      * the widget hierarchy.
      * @brief The color mask of the widget.
      */
    Color m_color_mask;

private:

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
    /** @brief Performs some necessary event processing on key
      *        events before the key event handler gets them.
      */
    virtual bool InternalProcessKeyEvent (EventKey const *e);
    /** @brief Performs some necessary event processing on mouse
      *        events before the mouse event handler gets them.
      */
    virtual bool InternalProcessMouseEvent (EventMouse const *e);
    /** @brief Performs some necessary event processing on pinch
      *        events before the pinch event handler gets them.
      */
    virtual bool InternalProcessPinchEvent (EventPinch const *e);
    /** @brief Performs some necessary event processing on rotate
      *        events before the rotate event handler gets them.
      */
    virtual bool InternalProcessRotateEvent (EventRotate const *e);
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

    /** Currently only used for debugging purposes
      * @brief Textual name of this instance of the widget.
      */
    std::string m_name;
    /** NULL indicates that this is a top-level widget (or a parentless one
      * hanging out in limbo).
      * @brief Pointer to the parent widget.
      */
    ContainerWidget *m_parent;
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
    /** Indicates which block this widget will remain in inside
      * m_child_vector.
      * @brief Widget stack priority.
      */
    StackPriority m_stack_priority;
    /** @brief the basic background for the widget
      */
    WidgetBackground *m_background;
    /** Subclasses can use this to specify different backgrounds for different
      * behaviors.  NULL indicates no background will be rendered
      * (transparent).
      * @brief The background which will be rendered in @c Draw .
      */
    WidgetBackground const *m_render_background;
    /** @brief Contains the frame margins which are used by the background
      *        and various other Widget subclasses for drawing and layout.
      */
    ScreenCoordMargins m_frame_margins;
    /** @brief Contains the content margins which are used by various widgets.
      */
    ScreenCoordMargins m_content_margins;

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

    // kludgey (as are all friend statements), but this is the simplest way
    friend class ContainerWidget;
}; // end of class Widget

} // end of namespace Xrb

#endif // !defined(_XRB_WIDGET_HPP_)

