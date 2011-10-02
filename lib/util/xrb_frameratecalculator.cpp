// ///////////////////////////////////////////////////////////////////////////
// xrb_frameratecalculator by Victor Dods, created 2004/05/31
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_frameratecalculator.hpp"

namespace Xrb {

FramerateCalculator::FramerateCalculator (Float time_unit_conversion_ratio)
{
    m_last_frame_time = Time::ms_beginning_of;
    m_time_unit_conversion_ratio = (time_unit_conversion_ratio > 0.0) ? time_unit_conversion_ratio : 1.0f;
}

void FramerateCalculator::AddFrameTime (Time frame_time)
{
    // make sure not to overflow the queue
    if (m_frame_dt_queue.IsFull())
        m_frame_dt_queue.Dequeue();

    // enqueue the frame time delta
    m_frame_dt_queue.Enqueue(frame_time - m_last_frame_time);
    // save off the given frame time for the next AddFrameTime
    m_last_frame_time = frame_time;
}

Float FramerateCalculator::Framerate () const
{
    Time::Delta total = 0.0f;

    // total up the queued frame times
    for (Uint32 i = 0; i < m_frame_dt_queue.EntryCount(); ++i)
        total += m_frame_dt_queue.Entry(i);

    // avoid a divide by zero by giving a bogus value
    if (total == 0.0f)
        total = 1.0f;

    return m_time_unit_conversion_ratio * m_frame_dt_queue.EntryCount() / total;
}

} // end of namespace Xrb
