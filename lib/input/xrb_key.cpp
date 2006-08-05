// ///////////////////////////////////////////////////////////////////////////
// xrb_key.cpp by Victor Dods, created 2004/06/09
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_key.h"

#include "xrb_input_events.h"

namespace Xrb
{

Key *Key::Create (Key::Code const code, std::string const &name)
{
    Key *retval = new Key();

    retval->m_code = code;
    retval->m_name = name;

    return retval;
}

bool Key::GetIsKeyRepeatable (Key::Code const code)
{
    // repeatable keys:
    if (code >= Key::SPACE && code <= Key::DELETE       // ASCII char keys
        ||
        code == Key::BACKSPACE                          // backspace
        ||
        code >= Key::KP0 && code <= Key::KP_PLUS        // keypad keys
        ||
        code >= Key::UP && code <= Key::LEFT            // arrow keys
        ||
        code == Key::PAGEUP                             // page up
        ||
        code == Key::PAGEDOWN                           // page down
        ||
        code >= Key::WORLD_0 && code <= Key::WORLD_95)  // world keys
        return true;

    // anything else is not repeatable
    return false;
}

bool Key::GetIsKeyAscii (Key::Code const code)
{
    if (code >= ZERO && code <= NINE || code >= A && code <= Z)
        return true;

    switch (code)
    {
        case TAB:
        case RETURN:
        case ESCAPE:
        case SPACE:
        case QUOTE:
        case COMMA:
        case MINUS:
        case PERIOD:
        case SLASH:
        case SEMICOLON:
        case EQUALS:
        case LEFTBRACKET:
        case BACKSLASH:
        case RIGHTBRACKET:
        case BACKQUOTE:
        case DELETE:
            return true;

        default:
            return false;
    }
}

void Key::ResetPressed ()
{
    m_pressed = false;
}

Key::Key ()
    :
    EventHandler(NULL)
{
    m_code = Key::INVALID;
    m_pressed = false;
}

bool Key::ProcessEventOverride (Event const *const e)
{
    ASSERT1(e != NULL)

    switch (e->GetEventType())
    {
        case Event::KEYDOWN:
            // assert so that we don't pass the event to the wrong key
            ASSERT1(static_cast<EventKeyDown const *const>(e)->GetKeyCode() == m_code)
            m_pressed = true;
            break;

        case Event::KEYUP:
            // assert so that we don't pass the event to the wrong key
            ASSERT1(static_cast<EventKeyUp const *const>(e)->GetKeyCode() == m_code)
            m_pressed = false;
            break;

        case Event::MOUSEBUTTONDOWN:
            // assert so that we don't pass the event to the wrong key
            ASSERT1(static_cast<EventMouseButtonDown const *const>(e)->GetButtonCode() == m_code)
            m_pressed = true;
            break;

        case Event::MOUSEBUTTONUP:
            // assert so that we don't pass the event to the wrong key
            ASSERT1(static_cast<EventMouseButtonUp const *const>(e)->GetButtonCode() == m_code)
            m_pressed = false;
            break;

        default:
            ASSERT1(0 && "Events which are not of type KEYDOWN, KEYUP, MOUSEBUTTONDOWN, or MOUSEBUTTONUP should not be passed to Key::ProcessEventOverride()")
            break;
    }

    return true;
}

} // end of namespace Xrb
