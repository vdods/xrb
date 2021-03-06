// ///////////////////////////////////////////////////////////////////////////
// xrb_keyrepeater.cpp by Victor Dods, created 2005/02/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_keyrepeater.hpp"

#include "xrb_eventqueue.hpp"

namespace Xrb {

KeyRepeater::KeyRepeater (Time::Delta repeat_delay, Time::Delta repeat_period)
    :
    EventHandler(NULL),
    FrameHandler(),
    m_current_key_event(Key::NONE, Key::MOD_NONE, Time::ms_beginning_of)
{
    ASSERT1(repeat_delay > 0.0);
    ASSERT1(repeat_period > 0.0);

    m_repeat_delay = repeat_delay;
    m_repeat_period = repeat_period;
    m_is_current_key_event_active = false;
}

KeyRepeater::~KeyRepeater ()
{
    m_is_current_key_event_active = false;
}

EventKeyRepeat *KeyRepeater::DequeueEvent ()
{
    ASSERT1(!m_key_event_queue.IsEmpty());
    return m_key_event_queue.Dequeue();
}

void KeyRepeater::HandleFrame ()
{
    // generate key events from the current state
    GenerateKeyEvents(FrameTime());
}

bool KeyRepeater::HandleEvent (Event const &e)
{
    if (e.IsKeyEvent())
    {
        // generate key events from the current state
        GenerateKeyEvents(e.GetTime());

        EventKey const &key_event = dynamic_cast<EventKey const &>(e);
        if (key_event.IsKeyDownEvent() && Key::IsKeyRepeatable(key_event.KeyCode()))
        {
            EventKeyDown const &key_down_event = dynamic_cast<EventKeyDown const &>(e);
            // use this key as the current key event
            m_current_key_event = key_down_event;
            m_is_current_key_event_active = true;
            m_next_repeat_time = key_down_event.GetTime() + m_repeat_delay;

            // the event was used
            return true;
        }
        else if (key_event.IsKeyUpEvent() && Key::IsKeyRepeatable(key_event.KeyCode()))
        {
            // if the key up event matches the current key event, then
            // deactivate the current key event.
            if (m_current_key_event.KeyCode() == key_event.KeyCode())
                m_is_current_key_event_active = false;

            // the event was used
            return true;
        }
    }

    // the event was not used
    return false;
}

void KeyRepeater::GenerateKeyEvents (Time time)
{
    // early out if there's no key being held down
    if (!m_is_current_key_event_active)
        return;

    // loop to generate events until they are no more to be made
    while (m_next_repeat_time <= time && !m_key_event_queue.IsFull())
    {
        // generate the event
        EventKeyRepeat *key_repeat_event =
            new EventKeyRepeat(m_current_key_event.KeyCode(), m_current_key_event.KeyModifier(), time);
        // enqueue the event
        m_key_event_queue.Enqueue(key_repeat_event);
        // schedule the next repeat time
        m_next_repeat_time += m_repeat_period;
    }

    // if the buffer filled up, skip the next repeat time to the current time
    if (m_key_event_queue.IsFull())
        m_next_repeat_time = time;
}

} // end of namespace Xrb
