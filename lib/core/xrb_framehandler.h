// ///////////////////////////////////////////////////////////////////////////
// xrb_framehandler.h by Victor Dods, created 2004/09/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_FRAMEHANDLER_H_)
#define _XRB_FRAMEHANDLER_H_

#include "xrb.h"

namespace Xrb
{

/** Time-based computation can be done using a FrameHandler.  The processing
  * function is guaranteed to be called exactly once per game loop (as opposed
  * to the non-guaranteed calls to @ref Xrb::Widget::Draw for example).
  * Various time metrics are provided for use inside the frame processing
  * function.
  * @brief Facilitates once-per-game-loop-frame computation.
  */
class FrameHandler
{
public:

    /** The frame count starts at 0.
      * @brief Constructor.
      */
    FrameHandler ();
    /** Does nothing.
      * @brief Destructor.
      */
    virtual ~FrameHandler ();

    /** Returns 0 if no frame has been processed yet.
      * @brief Returns the time of the most recent frame.
      */
    inline Float GetMostRecentFrameTime () const { return Max(m_most_recent_time, 0.0f); }
    /** The frame count is incremented once per call to ProcessFrame.
      * @brief Returns the accumulated frame count.
      */
    inline Uint32 GetFrameCount () const { return m_frame_count; }

    // this function will skip time so that the next call to ProcessFrame
    // incurs a time delta of 0.
    /** @todo Deprecate this fucktion.
      * @brief This function will be deprecated soon.
      */
    inline void SetSkipTime (bool const skip_time) { m_skip_time = skip_time; }

    /** Sets up the frame variables and then calls the overridden
      * HandleFrame which is what actuall does the processing.
      * @brief Initiates once-per-game-loop-frame computation.
      */
    void ProcessFrame (Float const time);

protected:

    /** Only valid inside HandleFrame -- use GetMostRecentFrameTime
      * otherwise.  If this method is called from outside of
      * HandleFrame, it will assert.
      * @brief Returns the current frame time.
      */
    inline Float GetFrameTime () const
    {
        ASSERT1(m_lock > 0)
        return m_most_recent_time;
    }
    /** Only valid inside HandleFrame.  If this method is called
      * from outside of HandleFrame, it will assert.
      * @brief Returns the time delta from the most recently processed frame.
      */
    inline Float GetFrameDT () const
    {
        ASSERT1(m_lock > 0)
        return m_frame_dt;
    }

    /** This is called by ProcessFrame after a bit of preparation.
      * GetFrameTime and GetFrameDT can be used inside this method.
      * @brief Overridable method for the actual computation required
      *        by subclasses.
      */
    virtual void HandleFrame () = 0;

private:

    // handles setting up all the member variables for starting a frame
    void StartFrame (Float const time);
    // handles ending the frame and updating m_most_recent_time
    void EndFrame ();

    // mutual exclusion so that calls to superclass Frames work
    Uint32 m_lock;
    // the most recently processed frame's time (stores the current frame
    // time during ProcessFrame()
    Float m_most_recent_time;
    // the delta time value
    Float m_frame_dt;
    // the frame counter
    Uint32 m_frame_count;
    // indicates if the next frame should skip time and make
    // the frame's time delta 0.
    bool m_skip_time;
}; // end of class FrameHandler

} // end of namespace Xrb

#endif // !defined(_XRB_FRAMEHANDLER_H_)

