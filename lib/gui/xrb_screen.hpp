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

namespace Xrb
{

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
    static Screen *Create (
        ScreenCoord width,
        ScreenCoord height,
        Uint32 bit_depth,
        bool fullscreen,
        Sint32 angle = 0);

    SignalSender0 const *SenderQuitRequested () { return &m_sender_quit_requested; }

    SignalReceiver0 const *ReceiverRequestQuit () { return &m_receiver_request_quit; }

    bool IsQuitRequested () const { return m_is_quit_requested; }
    ScreenCoord SizeRatioBasis () const { return Min(Width(), Height()); }
    // returns the dimensions of the physical video device
    ScreenCoordVector2 const &DeviceSize () const { return m_device_size; }

    // if the is-quit-requested flag is false, sets it to true and signals.
    void RequestQuit ();
    // sets the viewport for drawing into a widget
    void SetViewport (ScreenCoordRect const &clip_rect) const;
    // draws the whole fucking thing.
    void Draw () const;

protected:

    // protected constructor so that you must use Create
    Screen (Sint32 angle);

    // this calculates one frame, called by the game loop
    virtual void HandleFrame ();
    // processes an event, returning true if the event was captured,
    // otherwise it will pass the event to VirtualScreen::ProcessEvent()
    // and return that function's return value.
    virtual bool HandleEvent (Event const *e);

private:

    ScreenCoordVector2 RotatedScreenSize (ScreenCoordVector2 const &v) const;
    ScreenCoordVector2 RotatedScreenPosition (ScreenCoordVector2 const &v) const;
    ScreenCoordVector2 RotatedScreenVector (ScreenCoordVector2 const &v) const;
    ScreenCoordRect RotatedScreenRect (ScreenCoordRect const &r) const;

    // quit condition (maybe temporary)
    bool m_is_quit_requested;
    // the physical video dimensions
    ScreenCoordVector2 m_device_size;
    // the original dimensions of the Screen
    ScreenCoordVector2 m_original_screen_size;
    // stores the angle the screen is rotated by
    Sint32 const m_angle;

    SignalSender0 m_sender_quit_requested;

    SignalReceiver0 m_receiver_request_quit;
}; // end of class Screen

} // end of namespace Xrb

#endif // !defined(_XRB_SCREEN_HPP_)

