// ///////////////////////////////////////////////////////////////////////////
// xrb_keyrepeater.hpp by Victor Dods, created 2005/02/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_KEYREPEATER_HPP_)
#define _XRB_KEYREPEATER_HPP_

#include "xrb.hpp"

#include "xrb_circularqueue.hpp"
#include "xrb_eventhandler.hpp"
#include "xrb_framehandler.hpp"
#include "xrb_input_events.hpp"
#include "xrb_time.hpp"

namespace Xrb {

/// @brief Processes keyboard events and produces key repeat events.
/// @details Key repeat events are generated to get the effect of holding a key down
/// while typing, simulating many keystrokes of the same key very quickly.
///
/// Keyboard events are processed by this class, and a series of
/// EventKeyRepeat events are queued up when HandleFrame is called
/// to indicate each iteration of the game loop.
class KeyRepeater : public EventHandler, public FrameHandler
{
public:

    /// @brief Constructs a KeyRepeater with the given properties.
    /// @param repeat_delay The time that must elapse after a key is pressed
    ///                     before key repeat events are generated.
    /// @param repeat_period The time between successive key repeat events.
    /// @details The default values for @c repeat_delay and @c repeat_period seem to
    /// be the most common values (that I've seen at least).  25 millisecond
    /// delay, and then 30 events per second.
    KeyRepeater (Time::Delta repeat_delay = 0.25f, Time::Delta repeat_period = 0.033f);
    ~KeyRepeater ();

    /// @brief Returns true iff the key repeat event queue is full.
    /// @details HandleFrame generates the key repeat events and queues them
    /// so they can be later removed and put in the main EventQueue.  This
    /// accessor is used when generating the key repeat events, indicating
    /// one of the conditions to stop generating key repeat events during
    /// this HandleFrame.
    bool IsEventQueueFull () const { return m_key_event_queue.IsFull(); }
    /// @brief Returns true iff the key repeat event queue is empty.
    /// @details HandleFrame generates the key repeat events and queues them
    /// so they can be later removed and put in the main EventQueue.  This
    /// accessor is used when emptying the queue of key repeat events and
    /// putting them into the main EventQueue.
    bool IsEventQueueEmpty () const { return m_key_event_queue.IsEmpty(); }

    /// @brief Dequeues a single key repeat event.
    /// @details The dequeued events should be enqueued into the main EventQueue.
    EventKeyRepeat *DequeueEvent ();

protected:

    /// @brief Does time-based generation of key repeat events, based on the current state of the most recently pressed key.
    virtual void HandleFrame ();
    /// @brief Processes key events.
    /// @details Performs the necessary tracking for most recently pressed key and timings and such.
    virtual bool HandleEvent (Event const &e);

private:

    void GenerateKeyEvents (Time time);

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
    Time::Delta m_repeat_delay;
    // period of repeat cycle (time between each additional key event
    // to be generated once key repeating has engaged)
    Time::Delta m_repeat_period;
    // the earliest time that a repeat event can be generated
    Time m_next_repeat_time;
    // circular queue of buffered key events
    CircularQueue<EventKeyRepeat *, DEFAULT_KEY_EVENT_BUFFER_SIZE> m_key_event_queue;
}; // end of class KeyRepeater

} // end of namespace Xrb

#endif // !defined(_XRB_KEYREPEATER_HPP_)

