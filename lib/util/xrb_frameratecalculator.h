// ///////////////////////////////////////////////////////////////////////////
// xrb_frameratecalculator.h by Victor Dods, created 2004/05/31
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_FRAMERATECALCULATOR_H_)
#define _XRB_FRAMERATECALCULATOR_H_

#include "xrb.h"

#include "xrb_circularqueue.h"

namespace Xrb
{

/** The default queue size is 100, providing a more or less instantaneous
  * framerate (unless frames are being rendered very slowly).
  * @brief A circular queue storing frame durations in order to calculate
  *        framerates.
  */
class FramerateCalculator
{
public:

    /** The time unit conversion ratio is simply multiplied into the
      * calculated framerate before returning it, in GetFramerate.
      * @brief Constructs a framerate calculator with the given time
      *        unit conversion ratio.
      */
    FramerateCalculator (Float time_unit_conversion_ratio = 1.0);
    /** @brief Destructor.  Boring.
      */
    ~FramerateCalculator () { }

    /** @brief Records the duration of a new frame.
      */
    void AddFrameTime (Float frame_time);

    /** @brief Calculates and returns the current framerate, multiplied by
      *        the time unit conversion ratio.
      */
    Float GetFramerate () const;

private:

    enum
    {
        FRAME_COUNT = 32
    };

    // the circular queue which stores the frame times
    CircularQueue<Float, FRAME_COUNT> m_frame_queue;
    // the last frame time (to compute frame deltas)
    Float m_last_frame_time;
    // time conversion ratio (from input units to output units)
    Float m_time_unit_conversion_ratio;
}; // end of class FramerateCalculator

} // end of namespace Xrb

#endif // !defined(_XRB_FRAMERATECALCULATOR_H_)
