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
#include "xrb_stylesheet.hpp"

/** @file xrb_widget.h
  * @brief Contains the class declaration of the foundation of the GUI
  *        system of XuqRijBuh -- Widget.
  */

namespace Xrb {

class ContainerWidget;
class EventCustom;
class EventDetachAndDeleteChildWidget;
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
class WidgetContext;

/// @brief The class which forms the foundation of the GUI system.
/// @note All GUI widgets must derive from this class to function properly.
/// @details The base class for all GUI widgets and provides 90% of the
/// framework necessary for the GUI's operation.
///
/// The GUI system of XuqRijBuh is modeled closely upon the commonly used
/// "widget" paradigm of GUI toolkits such as QT (http://www.trolltech.com).
/// Some examples of specialized widgets are @c Label, @c Button, and
/// @c CheckBox.
///
/// A hierarchy of widgets (a widget being essentially a rectangular area
/// on the screen) is created to suit the task at hand.  Widgets
/// subordinate to a particular widget in the hierarchy are referred to as
/// its "child widgets" or "children".  The hierarchy dictates specific
/// rules as to how widgets are drawn and how they receive events.
///
/// The top of the widget hierarchy must be an instance of @c Screen.
/// it contains specialized functionality for initiating drawing and
/// event-sending.
///
/// Widgets are drawn in a top-down manner -- drawing begins at the @c Screen,
/// which performs some one-time setup work (one-time per frame), and sets
/// up the OpenGL viewport/clip rect and so forth, and then calls the drawing
/// function on all its child widgets in the order in which they appear in
/// the widget stack (see @c m_child_vector).  During the draw function, a
/// widget will draw itself (e.g. backgrounds, text, pictures, nothing, etc)
/// and then call the draw function on its own children in the order specified
/// by @c m_child_vector.  There are a few caveats to this, including hidden
/// widgets, but that will be explained later.
///
/// Events (see @c Event) are processed in a similar fashion as drawing --
/// starting at the top of the hierarchy, propagating down to children.  The
/// difference here is that the event will not be "used" by all the widgets
/// in the hierarchy.  An event is created at a particular level in the
/// hierarchy (though usually from outside and passed to the top), and then
/// each child widget is asked if it wishes to use the event.  A used
/// event is not destroyed or in any way changed, allowing for parent widgets
/// to perform further processing after a child accepts/denies usage of an
/// event.
///
/// See @ref widget_baseclass "Widget Baseclass" for extensive system details.
class Widget : public FrameHandler, public EventHandler, public SignalHandler
{
public:

    // ///////////////////////////////////////////////////////////////////////
    // constructor and destructor
    // ///////////////////////////////////////////////////////////////////////

    /// @brief The widget will be associated to the given WidgetContext widget during construction.
    /// @param context The WidgetContext which this Widget will be associated with.  This comes from a/the Screen ultimately.
    /// @param name A textual label applied to the specific widget being constructed.  This is only used for debugging.
    Widget (WidgetContext &context, std::string const &name = "Widget");
    /// The widget removes itself from WidgetContext upon destruction.
    virtual ~Widget ();

    // ///////////////////////////////////////////////////////////////////////
    // SignalReceiver accessors
    // ///////////////////////////////////////////////////////////////////////

    SignalReceiver1<bool> const *ReceiverSetIsEnabled () { return &m_receiver_set_is_enabled; } ///< This receiver calls SetIsEnabled.
    SignalReceiver0 const *ReceiverEnable () { return &m_receiver_enable; }                     ///< This receiver calls Enable.
    SignalReceiver0 const *ReceiverDisable () { return &m_receiver_disable; }                   ///< This receiver calls Disable.

    SignalReceiver1<bool> const *ReceiverSetIsHidden () { return &m_receiver_set_is_hidden; }   ///< This receiver calls SetIsHidden.
    SignalReceiver0 const *ReceiverHide () { return &m_receiver_hide; }                         ///< This receiver calls Hide.
    SignalReceiver0 const *ReceiverShow () { return &m_receiver_show; }                         ///< This receiver calls Show.

    // ///////////////////////////////////////////////////////////////////////
    // accessors
    // ///////////////////////////////////////////////////////////////////////

    /// Returns a const reference to the WidgetContext to which this Widget belongs.
    WidgetContext const &Context () const { return m_context; }
    /// Returns a reference to the WidgetContext to which this Widget belongs.
    WidgetContext &Context () { return m_context; }
    /// Returns the name of this widget
    std::string const &Name () const { return m_name; }
    /// Returns a pointer to the parent (const) of this widget.
    ContainerWidget const *Parent () const { return m_parent; }
    /// Returns a pointer to the parent (non-const) of this widget.
    ContainerWidget *Parent () { return m_parent; }
    /// @brief Returns true iff this widget has been added to the "active" set of
    /// widgets that are being rendered and controlled by the Screen.
    /// @details A widget is considered "active" if it has a direct line of 
    /// ancestry to the Screen in the WidgetContext.  This is useful in determining 
    /// focus, mouseover, etc.
    bool IsActive () const;
    /// Returns true iff this widget is focused.  If this is a top-level widget, then it has focus by default.
    bool IsFocused () const;
    /// @brief Returns true iff this widget even accepts focus.
    /// @details The member variable @c m_accepts_focus should be set in Widget derivative classes to indicate
    /// if they will accept focus.  The default value set by the constructor of Widget is false.
    bool AcceptsFocus () const { return m_accepts_focus; }
    /// Returns true iff this widget currently has mouseover focus (i.e. its parent has this widget as its m_mouseover_focus value).
    bool IsMouseover () const;
    /// @brief Returns true iff this widget accepts mouseover focus.
    /// @details The member variable @c m_accepts_mouseover should be set in Widget derivative classes
    /// to indicate if they will accept mouseover focus. The default value set by the constructor of Widget is true.
    bool AcceptsMouseover () const { return m_accepts_mouseover; }
    /// Returns true iff this widget has mouse grab focus (meaning that all mouse
    /// events will go to/through it even if the mouse is not directly over it).
    bool IsMouseGrabbed () const;
    /// Returns true iff this widget is enabled.
    bool IsEnabled () const { return m_is_enabled; }
    /// Returns true iff this widget is hidden.
    bool IsHidden () const { return m_is_hidden; }
    /// Returns true iff this widget is currently modal.
    bool IsModal () const;
    /// Returns the widget stack priority of this widget.  @see StackPriority.
    StackPriority GetStackPriority () const { return m_stack_priority; }

    /// Returns true iff there is no background style, meaning that the background is specified explicitly.
    bool BackgroundStyleIsOverridden () const { return m_background_style.empty(); }
    /// If nonempty, gives the StyleSheet type of background to use.  @see StyleSheet::Background.
    /// If empty, the backgound is accessible via Background.
    std::string const &BackgroundStyle () const { return m_background_style; }
    /// If the background style is overridden, returns the overridden background.  Otherwise returns the stylized background.
    WidgetBackground const *Background () const;
    
    /// Returns true iff there is no frame margins style, meaning that the frame margins are specified explicitly.
    bool FrameMarginsStyleIsOverridden () const { return m_frame_margins_style.empty(); }
    /// If nonempty, gives the StyleSheet type of margins ratios to use for the frame margins.
    /// @see StyleSheet::MarginsRatios.  If empty, the frame margins ratios are accessible via FrameMarginsRatios.
    std::string const &FrameMarginsStyle () const { return m_frame_margins_style; }
    /// @brief If the frame margins style is overridden, returns the overridden frame margins.  Otherwise returns the stylized frame margins.
    /// @details The return value is based on the value of FrameMarginsRatios via WidgetContext::MarginsFromRatios.
    ScreenCoordMargins FrameMargins () const;
    /// If the frame margins style is overridden, returns the overridden frame margins ratios.
    /// Otherwise returns the stylized frame margins ratios.
    FloatMargins const &FrameMarginsRatios () const;
    
    /// Returns true iff there is no content margins style, meaning that the content margins are specified explicitly.
    bool ContentMarginsStyleIsOverridden () const { return m_content_margins_style.empty(); }
    /// If nonempty, gives the StyleSheet type of margins ratios to use for the content margins.
    /// @see StyleSheet::MarginsRatios.  If empty, the content margins ratios are accessible via ContentMarginsRatios.
    std::string const &ContentMarginsStyle () const { return m_content_margins_style; }
    /// @brief If the content margins style is overridden, returns the overridden content margins.  Otherwise returns the stylized content margins.
    /// @details The return value is based on the value of ContentMarginsRatios via WidgetContext::MarginsFromRatios.
    ScreenCoordMargins ContentMargins () const;
    /// If the content margins style is overridden, returns the overridden content margins ratios.
    /// Otherwise returns the stylized content margins ratios.
    FloatMargins const &ContentMarginsRatios () const;
    
    /// Returns the last known mouse position (derived from the most recent mouse motion event received by this widget).
    ScreenCoordVector2 const &LastMousePosition () const { return m_last_mouse_position; }
    /// Returns the current position of the lower-left corner of this widget.
    ScreenCoordVector2 Position () const { return m_screen_rect.BottomLeft(); }
    /// @brief Returns the current position of the center of this widget.
    /// @details Integer division is involved, so rounding may occur.
    ScreenCoordVector2 Center () const { return m_screen_rect.Center(); }
    /// Returns the current size of this widget.
    ScreenCoordVector2 Size () const { return m_screen_rect.Size(); }
    /// Returns the widget of this widget.
    ScreenCoord Width () const { return m_screen_rect.Width(); }
    /// Returns the height of this widget.
    ScreenCoord Height () const { return m_screen_rect.Height(); }
    /// Returns the boolean vector containing the is-minimum-size-enabled values for width and height.
    virtual Bool2 const &MinSizeEnabled () const { return m_size_properties.m_min_size_enabled; }
    /// Returns the screen-coordinate vector containing the minimum width and height.
    virtual ScreenCoordVector2 const &MinSize () const { return m_size_properties.m_min_size; }
    /// Returns the boolean vector containing the is-maximum-size-enabled values for width and height.
    virtual Bool2 const &MaxSizeEnabled () const { return m_size_properties.m_max_size_enabled; }
    /// Returns the screen-coordinate vector containing the maximum width and height.
    virtual ScreenCoordVector2 const &MaxSize () const { return m_size_properties.m_max_size; }
    /// @brief Returns the adjusted value of the given size vector.
    /// @param size The size vector to adjust.
    /// @details Adjusts the size vector to satisfy the minimum size properties of this widget.  Then adjusts
    /// the resulting value to satisfy the maximum size properties of this widget.  If there is a main widget,
    /// then its min/max size properties will be used instead.
    virtual ScreenCoordVector2 AdjustedSize (ScreenCoordVector2 const &size) const;
    /// Returns this widget's screen coordinate rectangle.
    ScreenCoordRect const &ScreenRect () const { return m_screen_rect; }
    /// @brief Returns the rectangle representing the content area.
    /// @details The content area is the area within the sum of the frame margins and the content margins.
    /// Note that the content margins can be negative.
    ScreenCoordRect ContentsRect () const;
    /// Returns this widget's color bias (the color bias is applied to everything drawn by the widget, AFTER the color mask).
    Color const &ColorBias () const { return m_color_bias; }
    /// Returns this widget's color mask (the color mask is applied to everything drawn by the widget).
    Color const &ColorMask () const { return m_color_mask; }
    /// Returns this widget's color bias as a non-const reference to be modified directly.
    Color &ColorBias () { return m_color_bias; }
    /// Returns this widget's color mask as a non-const reference to be modified directly.
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

    /// @brief Sets the widget's stack priority.
    /// @details Will cause a reshuffling of child widgets in this widget's parent's
    /// child vector, in order to maintain proper widget stack priority ordering.
    void SetStackPriority (StackPriority stack_priority);

    /// @brief Sets the background style (see StyleSheet), overriding any manually specified background.
    /// @details The style string can't be empty -- that indicates that the background is explicitly
    /// specified -- Use SetBackground for this purpose instead.
    void SetBackgroundStyle (std::string const &style);
    /// Overrides the background style and sets the basic background for this widget.  NULL indicates no background (transparent).
    void SetBackground (WidgetBackground const *background);
    
    /// @brief Sets the frame margins style (see StyleSheet), overriding any manually specified frame margins.
    /// @details The style string can't be empty -- that indicates that the frame margins are explicitly specified.
    /// Use SetFrameMargins or SetFrameMarginsRatios for this purpose.
    void SetFrameMarginsStyle (std::string const &style);
    /// Overrides the frame margins style and sets the frame margins for this widget (in direct screen coordinates).
    void SetFrameMargins (ScreenCoordMargins const &frame_margins);
    /// Overrides the frame margins style and sets the frame margins ratios for this widget (ratio is relative to WidgetContext::SizeRatioBasis()).
    void SetFrameMarginsRatios (FloatMargins const &frame_margins_ratios);
    
    /// @brief Sets the content margins style (see StyleSheet), overriding any manually specified content margins.
    /// @details The style string can't be empty -- that indicates that the content margins are explicitly specified.
    /// Use SetContentMargins or SetContentMarginsRatios for this purpose.
    void SetContentMarginsStyle (std::string const &style);
    /// Overrides the content margins style and sets the content margins for this widget (in direct screen coordinates).
    void SetContentMargins (ScreenCoordMargins const &content_margins);
    /// Overrides the content margins style and sets the content margins ratios for this widget (ratio is relative to WidgetContext::SizeRatioBasis()).
    void SetContentMarginsRatios (FloatMargins const &content_margins_ratios);

    // ///////////////////////////////////////////////////////////////////////
    // procedures
    // ///////////////////////////////////////////////////////////////////////

    /// Sets the min and max sizes to the given size, and then enables
    /// min/max size enabled properties for both X and Y components.
    void FixSize (ScreenCoordVector2 const &size);
    /// Sets the min and max sizes to the given size, and then enables min/max size enabled
    /// properties for both X and Y components -- the size is given in a screen-basis ratio vector.
    void FixSizeRatios (FloatVector2 const &size_ratios);
    /// Disables min and max size enabled properties for both X and Y components.
    void UnfixSize ();
    /// Sets the min and max width to the given width, and then enables min/max size enabled properties for the X component only.
    void FixWidth (ScreenCoord width);
    /// Sets the min and max widths to the given size, and then enables min/max size enabled
    /// properties for the X component only -- the width is given as a screen-basis ratio.
    void FixWidthRatio (Float width_ratio);
    /// Disables min and max size enabled properties for the X component only.
    void UnfixWidth ();
    /// Sets the min and max height to the given height, and then enables min/max size enabled properties for the Y component only.
    void FixHeight (ScreenCoord height);
    /// Sets the min and max heights to the given size, and then enables min/max size enabled
    /// properties for the Y component only -- the height is given as a screen-basis ratio.
    void FixHeightRatio (Float height_ratio);
    /// Disables min and max size enabled properties for the Y component only.
    void UnfixHeight ();

    /// An overridable, non-const method which is called immediately before any widgets are Draw'n.
    virtual void PreDraw ();
    /// @brief Draws the widget using the provided RenderContext.
    /// @note You can NOT count on Draw being called every single video frame, since it will not
    /// be called if the clip_rect is not valid (0 area).
    /// @details This function should be overridden in subclasses to provide the
    /// means to draw whatever is necessary.  Generally the subclass's Draw
    /// function should call the Draw function of its immediate superclass
    /// before doing anything else, in order to have the background and
    /// other necessary visual elements drawn.
    ///
    /// Draw should only be called on top-level widgets -- the Widget
    /// baseclass takes care of setting up the RenderContext and calling
    /// Draw on its child widgets appropriately.
    virtual void Draw (RenderContext const &render_context) const;
    /// An overridable, non-const method which is called immediately after all widgets are Draw'n.
    virtual void PostDraw ();
    
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

    /// Returns true iff this widget is the Screen in the associated WidgetContext.
    bool IsScreen () const;

    /// This method is called when the WidgetContext's style sheet changes.
    /// Classes which override this method should call the parent's method.
    virtual void HandleChangedStyleSheet ();

    /// Returns the a pointer to the background object which is rendered in @a Widget::Draw.
    WidgetBackground const *RenderBackground () const { ASSERT1(!RenderBackgroundNeedsUpdate()); return m_render_background; }
    /// Returns the a pointer to the background object which is rendered in @a Widget::Draw,
    /// but calls UpdateRenderBackground if RenderBackgroundNeedsUpdate() returns true.
    WidgetBackground const *RenderBackground ()
    {
        if (RenderBackgroundNeedsUpdate())
            UpdateRenderBackground();
        ASSERT1(!RenderBackgroundNeedsUpdate());
        return m_render_background;
    }

    /// Sets the background to use in @a Widget::Draw.
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
    /// Subclasses may override this to process key events.
    virtual bool ProcessKeyEvent (EventKey const *e) { return false; }
    /// Subclasses may override this to process mouse button events.
    virtual bool ProcessMouseButtonEvent (EventMouseButton const *e) { return false; }
    /// Subclasses may override this to process mouse wheel events.
    virtual bool ProcessMouseWheelEvent (EventMouseWheel const *e) { return false; }
    /// Subclasses may override this to process motion events.
    virtual bool ProcessMouseMotionEvent (EventMouseMotion const *e) { return false; }
    /// Subclasses may override this to process pinch begin events.
    virtual bool ProcessPinchBeginEvent (EventPinchBegin const *e) { return false; }
    /// Subclasses may override this to process pinch end events.
    virtual bool ProcessPinchEndEvent (EventPinchEnd const *e) { return false; }
    /// Subclasses may override this to process pinch motion events.
    virtual bool ProcessPinchMotionEvent (EventPinchMotion const *e) { return false; }
    /// Subclasses may override this to process rotate motion events.
    virtual bool ProcessRotateMotionEvent (EventRotateMotion const *e) { return false; }
    /// Subclasses may override this to process rotate begin events.
    virtual bool ProcessRotateBeginEvent (EventRotateBegin const *e) { return false; }
    /// Subclasses may override this to process rotate end events.
    virtual bool ProcessRotateEndEvent (EventRotateEnd const *e) { return false; }
    /// Subclasses may override this to process joystick events.
    virtual bool ProcessJoyEvent (EventJoy const *e) { return false; }
    /// Subclasses may override this to process StateMachineInput events.
    virtual bool ProcessStateMachineInputEvent (EventStateMachineInput const *e) { return false; }
    /// Subclasses may override this to process custom events -- events not explicitly handled by the above overridable handlers.
    virtual bool ProcessCustomEvent (EventCustom const *e) { return false; }
    /// Processes a delete child widget event.  This is used mainly to delete modal widgets.
    virtual bool ProcessDetachAndDeleteChildWidgetEvent (EventDetachAndDeleteChildWidget const *e);

    /// @brief Handler that is called when this widget is attached to a parent.
    /// @details Subclasses should override this when they need to do something when
    /// @c ContainerWidget::AttachChild is called on this.  This method will
    /// be called after the child is attached (so in particular, Parent() is not NULL).
    virtual void HandleAttachedToParent ();
    /// @brief Handler that is called when this widget is detached from a parent.
    /// @details Subclasses should override this when they need to do something when
    /// @c ContainerWidget::DetachChild is called on this.  This method will
    /// be called before the child is detached (so in particular, Parent() is not NULL).
    virtual void HandleAboutToDetachFromParent ();
    /// Overridable method that is called when this widget becomes active (see @c Widget::IsActive).
    virtual void HandleActivate () { }
    /// Overridable method that is called when this widget goes from active to not active (see @c Widget::IsActive).
    virtual void HandleDeactivate () { }
    /// Overridable method that is called when this widget becomes focused.
    virtual void HandleFocus () { }
    /// Overridable method that is called when this widget becomes unfocused.
    virtual void HandleUnfocus () { }
    /// Overridable method that is called when the mouseover flag is set ON (from off).
    virtual void HandleMouseoverOn () { }
    /// Overridable method that is called when the mouseover flag is set OFF (from on).
    virtual void HandleMouseoverOff () { }
    /// Overridable method that is called when this widget grabs the mouse input.
    virtual void HandleMouseGrabOn () { }
    /// Overridable method that is called when this widget lets go of the mouse input.
    virtual void HandleMouseGrabOff () { }
    /// Overridable method that is called when the frame margins have been changed.  @see Layout.
    virtual void HandleChangedFrameMargins () { }
    /// Overridable method that is called when the content margins have been changed.  @see Layout.
    virtual void HandleChangedContentMargins () { }

    /// @brief Assigns the basic widget background to the render background.
    /// @details Subclasses may override this function to implement custom background
    /// behavior such as mouseover highlighting and button-depressing.
    virtual void UpdateRenderBackground ();
    /// Returns true iff the "render background needs update" flag is set.  @see SetRenderBackgroundNeedsUpdate.
    bool RenderBackgroundNeedsUpdate () const { return m_render_background_needs_update; }
    /// Indicates that @c UpdateRenderBackground should be called before @c Draw.
    void SetRenderBackgroundNeedsUpdate () { m_render_background_needs_update = true; }
    /// @brief Calls ChildSizePropertiesChanged on this widget's parent, with the "this" pointer as the parameter.
    /// @param defer_parent_update If this is false, nothing is done.  This is used primarily when there is going
    /// to be lots of updates in a row, and all but the last update don't matter.
    void ParentChildSizePropertiesUpdate (bool defer_parent_update);
    /// Adjusts *size by the minimum size.  Returns true iff *size was changed.
    bool AdjustFromMinSize (ScreenCoordRect *screen_rect) const;
    /// Adjusts *size by the maximum size.  Returns true iff *size was changed.
    bool AdjustFromMaxSize (ScreenCoordRect *screen_rect) const;
    /// Performs range checking to ensure the min and max sizes are valid after the minimum size was changed.
    void MinSizeUpdated ();
    /// Performs range checking to ensure the min and max sizes are valid after the maximum size was changed.
    void MaxSizeUpdated ();
    /// Ensures the size of the given rectangle is non-negative in both dimensions.
    void SizeRangeAdjustment (ScreenCoordRect *rect) const;

    /// Contains the min/max and min/max-enabled size properties of this widget.
    SizeProperties m_size_properties;
    /// @brief Indicates if this widget accepts focus on its own.
    /// @details  A value of false <strong>will</strong> prevent container widgets from blocking child widgets from gaining focus.
    bool m_accepts_focus;
    /// @brief Indicates if this widget accepts the mouseover flag on its own.
    /// @details A value of false <strong>will not</strong> prevent container
    /// widgets from blocking child widgets from accepting mouseover.
    bool m_accepts_mouseover;
    /// @brief The color bias of the widget used in rendering.
    /// @details The color bias represents a blending function which is applied
    /// to each drawing operation at or below this widget in the widget hierarchy.
    Color m_color_bias;
    /// @brief The color mask of the widget used in rendering.
    /// @details The color mask represents a modulation function (simple multiplication)
    /// which is applied to each drawing operation at or below this widget in the widget hierarchy.
    Color m_color_mask;
    /// Used in the interplay of the destructors of Widget, Screen and WidgetContext.
    /// <strong>Do not change</strong> -- this should be set by Widget and Screen only.
    bool m_remove_from_widget_context_upon_destruction;

private:

    /// Focuses all widgets in this widget's line of parency, from top down.
    void FocusWidgetLine ();
    /// Unfocuses all widgets from this one down, starting at the lowest child .
    void UnfocusWidgetLine ();
    /// Changes the mouseover to this widget.  Returns true iff a widget accepted mouseover.
    bool MouseoverOn ();
    /// Removes mouseover from this widget.
    void MouseoverOff ();
    /// Mouseover-ons all widgets in this widget's line of parency, top down.
    void MouseoverOnWidgetLine ();
    /// Mouseover-offs all widgets from this one down, starting at lowest child, going up.
    void MouseoverOffWidgetLine ();
    /// Performs some necessary event processing on key events before the key event handler gets them.
    virtual bool InternalProcessKeyEvent (EventKey const *e);
    /// Performs some necessary event processing on mouse events before the mouse event handler gets them.
    virtual bool InternalProcessMouseEvent (EventMouse const *e);
    /// Performs some necessary event processing on pinch events before the pinch event handler gets them.
    virtual bool InternalProcessPinchEvent (EventPinch const *e);
    /// Performs some necessary event processing on rotate events before the rotate event handler gets them.
    virtual bool InternalProcessRotateEvent (EventRotate const *e);
    /// Performs some necessary event processing on joy events before the joy event handler gets them.
    virtual bool InternalProcessJoyEvent (EventJoy const *e);
    /// Performs some necessary event processing on focus events.
    virtual bool InternalProcessFocusEvent (EventFocus const *e);
    /// Performs some necessary event processing on mouseover events.
    virtual bool InternalProcessMouseoverEvent (EventMouseover const *e);

    /// Gives access to "global" context with respect to the "active" widgets associated with a Screen object.
    WidgetContext &m_context;
    /// Textual name of this instance of the widget.  Incredibly useful when debugging GUI code.
    std::string m_name;
    /// @brief Pointer to the parent widget.
    /// @details NULL indicates that this is a top-level widget or a parentless one hanging out in limbo.
    ContainerWidget *m_parent;
    /// Indicates if this widget is enabled.
    bool m_is_enabled;
    /// Stores the SignalSender-blocking state while the widget is disabled.
    bool m_enabled_sender_blocking_state;
    /// Iff true, this widget is hidden.
    bool m_is_hidden;
    /// The screen-coordinate rectangle which defines the size and position of this widget.
    ScreenCoordRect m_screen_rect;
    /// Last known mouse position (from processed mouse motion events).
    ScreenCoordVector2 m_last_mouse_position;
    /// Widget stack priority (indicates which block this widget will remain in inside m_child_vector).
    StackPriority m_stack_priority;
    /// If nonempty, gives the StyleSheet type of background to use for the basic background.
    /// @see StyleSheet::Background.  If empty, the background is specified manually in m_background.
    std::string m_background_style;
    /// When m_background_style is empty, this specifies the basic background for the widget.
    WidgetBackground const *m_background;
    /// @brief The background which will be rendered in @c Draw .
    /// @details Subclasses can use this to specify different backgrounds for different behaviors.  NULL
    /// indicates no background will be rendered (transparent).  Also @see m_render_background_needs_update.
    WidgetBackground const *m_render_background;
    /// Indicates if @c UpdateRenderBackground should be called before @c Draw.
    bool m_render_background_needs_update;
    /// If nonempty, gives the StyleSheet type of margins ratios to use for the frame margins.
    /// @see StyleSheet::MarginsRatios.  If empty, the frame margins ratios are specified manually
    /// in m_frame_margins_ratios
    std::string m_frame_margins_style;
    /// When m_frame_margins_style is empty, this specifies the frame margins ratios, upon which
    /// the frame margins (in pixels) is based via WidgetContext::MarginsFromRatios.
    FloatMargins m_frame_margins_ratios;
    /// If nonempty, gives the StyleSheet type of margins ratios to use for the frame margins.
    /// @see StyleSheet::MarginsRatios.  If empty, the frame margins ratios can be specified manually.
    std::string m_content_margins_style;
    /// When m_contents_margins_style is empty, this specifies the content margins ratios, upon which
    /// the content margins (in pixels) is based via WidgetContext::MarginsFromRatios.
    FloatMargins m_content_margins_ratios;

    // ///////////////////////////////////////////////////////////////////////
    // SignalReceivers
    // ///////////////////////////////////////////////////////////////////////

    SignalReceiver1<bool> m_receiver_set_is_enabled;    ///< Calls SetIsEnabled.
    SignalReceiver0 m_receiver_enable;                  ///< Calls Enable.
    SignalReceiver0 m_receiver_disable;                 ///< Calls Disable.

    SignalReceiver1<bool> m_receiver_set_is_hidden;     ///< Calls SetIsHidden.
    SignalReceiver0 m_receiver_hide;                    ///< Calls Hide.
    SignalReceiver0 m_receiver_show;                    ///< Calls Show.

    // kludgey (as are all friend statements), but this is the simplest way
    friend class ContainerWidget;
    friend class Screen;
    friend class WidgetContext;
}; // end of class Widget

} // end of namespace Xrb

#endif // !defined(_XRB_WIDGET_HPP_)

