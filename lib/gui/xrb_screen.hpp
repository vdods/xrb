// ///////////////////////////////////////////////////////////////////////////
// xrb_screen.hpp by Victor Dods, created 2004/06/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_SCREEN_HPP_)
#define _XRB_SCREEN_HPP_

#include "xrb.hpp"

#include "xrb_containerwidget.hpp"

namespace Xrb {

class EventQueue;

// The Screen class implements screen-specific drawing functionality
// (i.e. setting resolution, coping with conversion from right-handed world
// coordinates to left-handed screen coordinates, etc).  It inherits from
// Transform2, which serves as the view-to-screen transformation.
// The Screen is post-translated.
class Screen : public ContainerWidget
{
public:

    virtual ~Screen ();

    // the width and height should indicate the dimensions of the physical
    // device this Screen will be rendered to.  the value of angle determines
    // the dimensions of the Screen with respect to the Widget system.
    // angle indicates counterclockwise rotation of the rendered Screen, and
    // must be a multiple of 90 (and it can be negative)
    static Screen *Create (ScreenCoord width, ScreenCoord height, Uint32 bit_depth, bool fullscreen, Sint32 angle = 0);

    SignalSender0 const *SenderQuitRequested () { return &m_sender_quit_requested; }

    SignalReceiver0 const *ReceiverRequestQuit () { return &m_receiver_request_quit; }

    bool IsQuitRequested () const { return m_is_quit_requested; }
    // returns the dimensions of the physical video device
    ScreenCoordVector2 const &DeviceSize () const { return m_device_size; }
    /// Used for font pixel height calculations.  Gives the "useful" size of this Widget.
    ScreenCoord SizeRatioBasis () const { return Min(Width(), Height()); }

    // if the is-quit-requested flag is false, sets it to true and signals.
    void RequestQuit ();
    // will cause a screenshot of the state of the screen at the end of the
    // next call to Draw to be written to the given path.  calling this twice
    // does not cause two files to be written, the later call overrides the
    // earlier call.  specify an empty path to cancel.
    void RequestScreenshot (std::string const &screenshot_path);
    // sets up the GL projection matrix based on the given clip_rect.  this should
    // only really be used internally by XRB.
    static void SetProjectionMatrix (ScreenCoordRect const &clip_rect);
    // sets the viewport for drawing into a widget
    void SetViewport (ScreenCoordRect const &clip_rect) const;
    // draws the whole fucking thing.  this is only non-const to allow PreDraw and PostDraw to be called.
    void Draw (Float real_time);

    /// Returns true iff the specified widget is currently attached as a modal widget.
    bool IsAttachedAsModalChildWidget (Widget const &widget) const;
    /// @brief Analogous to @c ContainerWidget::AttachChild, except this method attaches
    /// the child widget as a modal widget,  (a modal widget is one which is drawn
    /// on top of everything else, and usurps all input until it is dismissed.
    /// @details Modal widget behavior requires the Screen to divert events
    /// directly to the modal widget immediately, bypassing the widget hierarchy,
    /// which is why this is a separate "add" method.
    virtual void AttachAsModalChildWidget (Widget &child);
    /// Removes a modal child widget.  @see Screen::AttachChildAsModalWidget
    virtual void DetachAsModalChildWidget (Widget &child);
    
protected:

    // protected constructor so that you must use Create
    Screen (Sint32 angle);

    // this calculates one frame, called by the game loop
    virtual void HandleFrame ();
    // processes an event, returning true if the event was captured,
    virtual bool HandleEvent (Event const &e);
    // override from ContainerWidget in order to handle modal child widgets' focus
    virtual bool InternalProcessFocusEvent (EventFocus const &e);

private:

    ScreenCoordVector2 RotatedScreenSize (ScreenCoordVector2 const &v) const;
    ScreenCoordVector2 RotatedScreenPosition (ScreenCoordVector2 const &v) const;
    ScreenCoordVector2 RotatedScreenVector (ScreenCoordVector2 const &v) const;
    ScreenCoordRect RotatedScreenRect (ScreenCoordRect const &r) const;

    // this creates the context, sets m_context and returns it.  used in construction.
    WidgetContext *CreateAndInitializeWidgetContext ();
    
    // this is the pointer which "owns" the context.
    WidgetContext *m_context;
    
    // quit condition (maybe temporary)
    bool m_is_quit_requested;
    // screenshot path
    mutable std::string m_screenshot_path;
    // the physical video dimensions
    ScreenCoordVector2 m_device_size;
    // the original dimensions of the Screen
    ScreenCoordVector2 m_original_screen_size;
    // stores the angle the screen is rotated by
    Sint32 const m_angle;

    /// @brief Contains the stack of modal widgets (used only if this is a top-level widget).
    /// @details The beginning of the list is the bottom of the stack, while the
    /// end is the top.  The modal widgets are drawn from bottom to top.
    WidgetList m_modal_child_widget_stack;
    
    SignalSender0 m_sender_quit_requested;

    SignalReceiver0 m_receiver_request_quit;
}; // end of class Screen

} // end of namespace Xrb

#endif // !defined(_XRB_SCREEN_HPP_)

