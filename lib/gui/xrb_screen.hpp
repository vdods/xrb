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
#include "xrb_frameratecalculator.hpp"

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

    static Screen *Create (
        ScreenCoord width,
        ScreenCoord height,
        Uint32 bit_depth,
        Uint32 flags);

    SignalSender0 const *SenderQuitRequested () { return &m_sender_quit_requested; }

    SignalReceiver0 const *ReceiverRequestQuit () { return &m_receiver_request_quit; }

    // returns true if the quit condition has been hit
    bool GetIsQuitRequested () const { return m_is_quit_requested; }
    inline ScreenCoord GetSizeRatioBasis () const
    {
        return Min(GetWidth(), GetHeight());
    }
    inline ScreenCoordVector2 GetScreenCoordsFromSDLCoords (
        Sint32 x,
        Sint32 y) const
    {
        return ScreenCoordVector2(
            static_cast<ScreenCoord>(x),
            GetScreenRect().GetHeight() - static_cast<ScreenCoord>(y));
    }
    inline Float GetFramerate () const
    {
        return m_framerate_calculator.GetFramerate();
    }

    // if the is-quit-requested flag is false, sets it to true and signals.
    void RequestQuit ();
    // draws the whole fucking thing.
    void Draw () const;

protected:

    // protected constructor so that you must use Create
    Screen ();

    // this calculates one frame, called by the game loop
    virtual void HandleFrame ();
    // processes an event, returning true if the event was captured,
    // otherwise it will pass the event to VirtualScreen::ProcessEvent()
    // and return that function's return value.
    virtual bool HandleEvent (Event const *e);

private:

    // quit condition (maybe temporary)
    bool m_is_quit_requested;
    // the current video mode resolution
    ScreenCoordVector2 m_current_video_resolution;
    // the master frame rate calculator for this screen
    mutable FramerateCalculator m_framerate_calculator;

    SignalSender0 m_sender_quit_requested;

    SignalReceiver0 m_receiver_request_quit;
}; // end of class Screen

} // end of namespace Xrb

#endif // !defined(_XRB_SCREEN_HPP_)

