// ///////////////////////////////////////////////////////////////////////////
// xrb_keyrepeater.cpp by Victor Dods, created 2005/02/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_keyrepeater.h"

#include "xrb_eventqueue.h"

namespace Xrb
{

KeyRepeater::KeyRepeater (
    Float const repeat_delay,
    Float const repeat_period)
    :
    EventHandler(NULL),
    FrameHandler(),
    m_current_key_event(0, 0.0)
{
    ASSERT1(repeat_delay > 0.0)
    ASSERT1(repeat_period > 0.0)

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
    ASSERT1(!m_key_event_queue.GetIsEmpty())
    return m_key_event_queue.Dequeue();
}

void KeyRepeater::HandleFrame ()
{
    // generate key events from the current state
    GenerateKeyEvents(GetFrameTime());
}

bool KeyRepeater::HandleEvent (Event const *const e)
{
    if (e->GetIsKeyEvent())
    {
        // generate key events from the current state
        GenerateKeyEvents(e->GetTime());

        EventKey const *key_event = static_cast<EventKey const *>(e);
        if (key_event->GetIsKeyDownEvent() &&
            Key::GetIsKeyRepeatable(key_event->GetKeyCode()))
        {

            EventKeyDown const *key_down_event = static_cast<EventKeyDown const *>(e);
            // use this key as the current key event
            m_current_key_event = *key_down_event;
            m_is_current_key_event_active = true;
            m_next_repeat_time = key_down_event->GetTime() + m_repeat_delay;

            // the event was used
            return true;
        }
        else if (key_event->GetIsKeyUpEvent() &&
                 Key::GetIsKeyRepeatable(key_event->GetKeyCode()))
        {
            // if the key up event matches the current key event, then
            // deactivate the current key event.
            if (m_current_key_event.GetSDLEvent().keysym.sym ==
                key_event->GetSDLEvent().keysym.sym)
                m_is_current_key_event_active = false;

            // the event was used
            return true;
        }
    }

    // the event was not used
    return false;
}

void KeyRepeater::GenerateKeyEvents (Float const time)
{
    // early out if there's no key being held down
    if (!m_is_current_key_event_active)
        return;

    // loop to generate events until they are no more to be made
    while (m_next_repeat_time <= time && !m_key_event_queue.GetIsFull())
    {
        // generate the event
        EventKeyRepeat *key_repeat_event =
            new EventKeyRepeat(&m_current_key_event.GetSDLEvent(), time);
        // enqueue the event
        m_key_event_queue.Enqueue(key_repeat_event);
        // schedule the next repeat time
        m_next_repeat_time += m_repeat_period;
    }

    // if the buffer filled up, skip the next repeat time to the current time
    if (m_key_event_queue.GetIsFull())
        m_next_repeat_time = time;
}

} // end of namespace Xrb
