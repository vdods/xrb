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

namespace Xrb
{

FrameHandler::FrameHandler ()
{
    m_most_recent_time = -1.0f;
    m_frame_dt = 0.0f;
    m_lock = 0;
    m_frame_count = 0;
    m_skip_time = false;
}

FrameHandler::~FrameHandler ()
{
    ASSERT1(m_lock == 0 && "Do not delete a FrameHandler while processing a frame");
}

void FrameHandler::ProcessFrame (Float const time)
{
    // start the frame
    StartFrame(time);
    // call the Float guts of the frame handler, supplied by the derived class
    HandleFrame();
    // end the frame
    EndFrame();
}

void FrameHandler::StartFrame (Float const time)
{
    ASSERT1(time >= 0.0f);
    ASSERT1(time >= m_most_recent_time);

    // only start the frame if the lock is unused
    if (m_lock == 0)
    {
        // if this is the first frame, init the previous frame time
        if (m_most_recent_time == -1.0f)
            m_most_recent_time = time;

        // if skip time was set, cause the previous time to be
        // the current time, and the time delta will be 0.
        if (m_skip_time)
        {
            m_skip_time = false;
            m_most_recent_time = time;
        }

        // calculate dt
        ASSERT1(time >= m_most_recent_time);
        m_frame_dt = time - m_most_recent_time;

        // store the most recent time
        m_most_recent_time = time;
    }

    // assert if there will be overflow
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
