// ///////////////////////////////////////////////////////////////////////////
// xrb_frameratecalculator.hpp by Victor Dods, created 2004/05/31
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_FRAMERATECALCULATOR_HPP_)
#define _XRB_FRAMERATECALCULATOR_HPP_

#include "xrb.hpp"

#include "xrb_circularqueue.hpp"
#include "xrb_time.hpp"

namespace Xrb {
    
/// @brief A circular queue storing frame durations in order to calculate framerates.
/// @details The default queue size is 32, providing a more or less instantaneous framerate
/// (unless frames are being rendered very slowly).
class FramerateCalculator
{
public:

    /// @brief Constructs a framerate calculator with the given time unit conversion ratio.
    /// @details The time unit conversion ratio is simply multiplied into the calculated framerate before returning it, in Framerate.
    FramerateCalculator (Float time_unit_conversion_ratio = 1.0);
    ~FramerateCalculator () { }

    /// Records the duration of a new frame.
    void AddFrameTime (Time frame_time);

    /// Calculates and returns the current framerate, multiplied by the time unit conversion ratio.
    Float Framerate () const;

private:

    enum { FRAME_COUNT = 32 };

    // the circular queue which stores the frame time deltas
    CircularQueue<Time::Delta, FRAME_COUNT> m_frame_dt_queue;
    // the last frame time (to compute frame time deltas)
    Time m_last_frame_time;
    // time conversion ratio (from input units to output units)
    Float m_time_unit_conversion_ratio;
}; // end of class FramerateCalculator

} // end of namespace Xrb

#endif // !defined(_XRB_FRAMERATECALCULATOR_HPP_)
