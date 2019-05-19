// ///////////////////////////////////////////////////////////////////////////
// xrb_framehandler.cpp by Victor Dods, created 2004/09/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_framehandler.hpp"

namespace Xrb {

FrameHandler::FrameHandler ()
{
    m_most_recent_frame_time = Time::ms_beginning_of;
    m_frame_dt = 0.0f;
    m_lock = 0;
    m_frame_count = 0;
}

FrameHandler::~FrameHandler ()
{
    ASSERT1(m_lock == 0 && "Do not delete a FrameHandler while processing a frame");
}

void FrameHandler::ProcessFrame (Time frame_time)
{
    // start the frame
    StartFrame(frame_time);
    // call the Float guts of the frame handler, supplied by the derived class
    HandleFrame();
    // end the frame
    EndFrame();
}

void FrameHandler::ResetMostRecentFrameTime (Time frame_time)
{
    m_most_recent_frame_time = frame_time;
}

void FrameHandler::StartFrame (Time frame_time)
{
    ASSERT1(frame_time >= Time::ms_beginning_of);

    // only start the frame if the lock is unused
    if (m_lock == 0)
    {
        // if this is the first frame, or if time moved backwards, init the previous frame time.
        if (m_most_recent_frame_time == Time::ms_beginning_of || frame_time < m_most_recent_frame_time)
            m_most_recent_frame_time = frame_time;

        // calculate dt -- dt can be zero.
        m_frame_dt = frame_time - m_most_recent_frame_time;
        // store the most recent time
        m_most_recent_frame_time = frame_time;
    }

    // assert if there will be overflow (this is practically impossible)
    ASSERT1(m_lock < UINT32_UPPER_BOUND);
    // increment the lock's usage
    ++m_lock;
}

void FrameHandler::EndFrame ()
{
    ASSERT1(m_lock > 0);
    // decrement the lock's usage
    --m_lock;

    if (m_lock == 0)
    {
        // zero out the delta time
        m_frame_dt = 0.0f;
        // increment the frame count
        ++m_frame_count;
    }
}

} // end of namespace Xrb
