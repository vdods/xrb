// ///////////////////////////////////////////////////////////////////////////
// xrb_input_events.cpp by Victor Dods, created 2005/11/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_input_events.hpp"

#include "xrb_keymap.hpp"
#include "xrb_screen.hpp"
#include "xrb_util.hpp"

namespace Xrb
{

EventInput::~EventInput () { }

EventKey::EventKey (
    Key::Code code,
    Key::Modifier modifier,
    Float time,
    EventType event_type)
    :
    EventInput(time, event_type)
{
    // store the code, performing key mapping using the KeyMap singleton
    m_code = Singleton::KeyMap().MappedKey(code);
    // store the modifier
    m_modifier = modifier;

    // set the raw ascii code
    if (code >= Key::SPACE && code < Key::DELETE)
        m_modified_ascii = char(code);
    else
        m_modified_ascii = '\0';

    // caps lock modification
    if ((m_modifier&Key::MOD_CAPS) != 0)
        m_modified_ascii = toupper(m_modified_ascii);

    // shift key modification
    if ((m_modifier&(Key::MOD_LSHIFT|Key::MOD_RSHIFT)) != 0)
        m_modified_ascii = Util::ShiftedAscii(m_modified_ascii);

    // num lock modification
    if ((m_modifier&Key::MOD_NUM) != 0)
    {
        switch (code)
        {
            case Key::KP0:       m_modified_ascii = '0'; break;
            case Key::KP1:       m_modified_ascii = '1'; break;
            case Key::KP2:       m_modified_ascii = '2'; break;
            case Key::KP3:       m_modified_ascii = '3'; break;
            case Key::KP4:       m_modified_ascii = '4'; break;
            case Key::KP5:       m_modified_ascii = '5'; break;
            case Key::KP6:       m_modified_ascii = '6'; break;
            case Key::KP7:       m_modified_ascii = '7'; break;
            case Key::KP8:       m_modified_ascii = '8'; break;
            case Key::KP9:       m_modified_ascii = '9'; break;
            case Key::KP_PERIOD: m_modified_ascii = '.'; break;
            default:                                     break;
        }
    }

    // keypad arithmetic keys
    switch (code)
    {
        case Key::KP_DIVIDE:   m_modified_ascii = '/'; break;
        case Key::KP_MULTIPLY: m_modified_ascii = '*'; break;
        case Key::KP_MINUS:    m_modified_ascii = '-'; break;
        case Key::KP_PLUS:     m_modified_ascii = '+'; break;
        case Key::KP_EQUALS:   m_modified_ascii = '='; break;
        default:                                       break;
    }
}

EventKey::~EventKey () { }

EventMouse::EventMouse (
    Uint16 event_x,
    Uint16 event_y,
    Key::Modifier modifier,
    Screen const *screen,
    Float time,
    EventType event_type)
    :
    EventInput(time, event_type)
{
    ASSERT1(screen != NULL);
    m_position = screen->ScreenCoordsFromSDLCoords(event_x, event_y);
    m_modifier = modifier;
}

EventMouse::~EventMouse () { }

EventMouseButton::EventMouseButton (
    Key::Code button_code,
    Uint16 event_x,
    Uint16 event_y,
    Key::Modifier modifier,
    Screen const *screen,
    Float time,
    EventType event_type)
    :
    EventMouse(event_x, event_y, modifier, screen, time, event_type),
    m_button_code(button_code)
{ }

EventMouseButton::~EventMouseButton () { }

EventMouseMotion::EventMouseMotion (
    bool is_left_mouse_button_pressed,
    bool is_middle_mouse_button_pressed,
    bool is_right_mouse_button_pressed,
    Uint16 event_x,
    Uint16 event_y,
    Sint16 event_dx,
    Sint16 event_dy,
    Key::Modifier modifier,
    Screen const *screen,
    Float time)
    :
    EventMouse(event_x, event_y, modifier, screen, time, MOUSEMOTION),
    m_is_left_mouse_button_pressed(is_left_mouse_button_pressed),
    m_is_middle_mouse_button_pressed(is_middle_mouse_button_pressed),
    m_is_right_mouse_button_pressed(is_right_mouse_button_pressed),
    m_delta(event_dx, event_dy)
{ }

} // end of namespace Xrb
