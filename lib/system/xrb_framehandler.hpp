// ///////////////////////////////////////////////////////////////////////////
// xrb_framehandler.hpp by Victor Dods, created 2004/09/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_FRAMEHANDLER_HPP_)
#define _XRB_FRAMEHANDLER_HPP_

#include "xrb.hpp"

#include "xrb_time.hpp"

namespace Xrb {

/// @brief Facilitates once-per-game-loop-frame computation.
/// @details Time-based computation can be done using a FrameHandler.  The processing
/// function is guaranteed to be called exactly once per game loop (as opposed
/// to the non-guaranteed calls to @ref Xrb::Widget::Draw for example).
/// Various time metrics are provided for use inside the frame processing
/// function.
class FrameHandler
{
public:

    /// The frame count starts at 0.
    FrameHandler ();
    virtual ~FrameHandler ();

    /// Returns the time of the most recent frame.  Returns 0 if no frame has been processed yet.
    Time MostRecentFrameTime () const { return Max(Time::ms_beginning_of, m_most_recent_frame_time); }
    /// Returns the accumulated frame count.  The frame count is incremented once per call to ProcessFrame.
    Uint32 FrameCount () const { return m_frame_count; }

    /// @brief Initiates once-per-game-loop-frame computation.
    /// @details Sets up the frame variables and then calls the overridden HandleFrame which is what actually does the processing.
    void ProcessFrame (Time frame_time);

protected:

    /// @brief Returns the current frame time.
    /// @details Only valid inside HandleFrame -- use MostRecentFrameTime otherwise.  If this
    /// method is called from outside of HandleFrame, it will assert.
    Time FrameTime () const
    {
        ASSERT1(m_lock > 0);
        return m_most_recent_frame_time;
    }
    /// @brief Returns the time delta from the most recently processed frame.  THIS CAN BE ZERO.
    /// @details Only valid inside HandleFrame.  If this method is called from outside of HandleFrame, it will assert.
    Time::Delta FrameDT () const
    {
        ASSERT1(m_lock > 0);
        return m_frame_dt;
    }

    /// @brief Overridable method for the actual computation required by subclasses.
    /// This is called by ProcessFrame after a bit of preparation.  FrameTime and FrameDT can be used inside this method.
    virtual void HandleFrame () = 0;

private:

    // handles setting up all the member variables for starting a frame
    void StartFrame (Time frame_time);
    // handles ending the frame and updating m_most_recent_frame_time
    void EndFrame ();

    // mutual exclusion so that calls to superclass Frames work
    Uint32 m_lock;
    // the most recently processed frame's time (stores the current frame time during ProcessFrame()
    Time m_most_recent_frame_time;
    // the delta time value
    Time::Delta m_frame_dt;
    // the frame counter
    Uint32 m_frame_count;
}; // end of class FrameHandler

} // end of namespace Xrb

#endif // !defined(_XRB_FRAMEHANDLER_HPP_)

