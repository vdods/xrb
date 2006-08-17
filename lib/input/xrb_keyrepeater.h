// ///////////////////////////////////////////////////////////////////////////
// xrb_keyrepeater.h by Victor Dods, created 2005/02/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_KEYREPEATER_H_)
#define _XRB_KEYREPEATER_H_

#include "xrb.h"

#include "xrb_circularqueue.h"
#include "xrb_eventhandler.h"
#include "xrb_framehandler.h"
#include "xrb_input_events.h"

namespace Xrb
{

/** Key repeat events are generated to get the effect of holding a key down
  * while typing, simulating many keystrokes of the same key very quickly.
  *
  * Keyboard events are processed by this class, and a series of
  * EventKeyRepeat events are queued up when HandleFrame is called
  * to indicate each iteration of the game loop.
  *
  * @brief Processes keyboard events and produces key repeat events.
  */
class KeyRepeater : public EventHandler, public FrameHandler
{
public:

    /** The default values for @c repeat_delay and @c repeat_period seem to
      * be the most common values (that I've seen at least).  25 millisecond
      * delay, and then 30 events per second.
      * @brief Constructs a KeyRepeater with the given properties.
      * @param repeat_delay The time that must elapse after a key is pressed
      *                     before key repeat events are generated.
      * @param repeat_period The time between successive key repeat events.
      */
    KeyRepeater (
        Float repeat_delay = 0.25,
        Float repeat_period = 0.033);
    /** @brief Destructor.
      */
    ~KeyRepeater ();

    /** HandleFrame generates the key repeat events and queues them
      * so they can be later removed and put in the main EventQueue.  This
      * accessor is used when generating the key repeat events, indicating
      * one of the conditions to stop generating key repeat events during
      * this HandleFrame.
      * @brief Returns true iff the key repeat event queue is full.
      */
    inline bool GetIsEventQueueFull () const
    {
        return m_key_event_queue.GetIsFull();
    }
    /** HandleFrame generates the key repeat events and queues them
      * so they can be later removed and put in the main EventQueue.  This
      * accessor is used when emptying the queue of key repeat events and
      * putting them into the main EventQueue.
      * @brief Returns true iff the key repeat event queue is empty.
      */
    inline bool GetIsEventQueueEmpty () const
    {
        return m_key_event_queue.GetIsEmpty();
    }

    /** The dequeued events should be enqueued into the main EventQueue.
      * @brief Dequeues a single key repeat event.
      */
    EventKeyRepeat *DequeueEvent ();

protected:

    /** @brief Does time-based generation of key repeat events, based on
      *        the current state of the most recently pressed key.
      */
    virtual void HandleFrame ();
    /** Performs the necessary tracking for most recently pressed key
      * and timings and such.
      * @brief Processes key events.
      */
    virtual bool HandleEvent (Event const *e);

private:

    void GenerateKeyEvents (Float time);

    enum
    {
        DEFAULT_KEY_EVENT_BUFFER_SIZE = 32
    };

    // indicates if there is a key currently being kept track
    // of by m_current_key_event
    bool m_is_current_key_event_active;
    // the keyboard event which is of the last key to be held down
    // which is still currently held down
    EventKeyDown m_current_key_event;
    // delay before key repeating starts
    Float m_repeat_delay;
    // period of repeat cycle (time between each additional key event
    // to be generated once key repeating has engaged)
    Float m_repeat_period;
    // the earliest time that a repeat event can be generated
    Float m_next_repeat_time;
    // circular queue of buffered key events
    CircularQueue<EventKeyRepeat *, DEFAULT_KEY_EVENT_BUFFER_SIZE> m_key_event_queue;
}; // end of class KeyRepeater

} // end of namespace Xrb

#endif // !defined(_XRB_KEYREPEATER_H_)

