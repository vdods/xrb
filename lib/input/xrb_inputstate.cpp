// ///////////////////////////////////////////////////////////////////////////
// xrb_inputstate.cpp by Victor Dods, created 2004/06/09
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_inputstate.hpp"

#include "xrb_emptystring.hpp"
#include "xrb_input_events.hpp"

namespace Xrb
{

InputState::InputState ()
    :
    EventHandler(NULL)
{
    m_is_caps_lock_on = false;
    m_is_num_lock_on = false;
    m_is_scroll_lock_on = false;
    // init the maps
    InitKeyMaps();
}

InputState::~InputState ()
{
    // shut down the key maps and delete the Key objects
    for (KeyCodeMap::iterator it = m_keycode_map.begin(),
                            it_end = m_keycode_map.end();
         it != it_end;
         ++it)
    {
        Delete(it->second);
    }
    m_keycode_map.clear();
    m_keyname_map.clear();
}

Key const *InputState::GetKey (Key::Code const code) const
{
    KeyCodeMap::const_iterator it = m_keycode_map.find(code);
    if (it == m_keycode_map.end())
        return NULL;
    else
    {
        ASSERT1(it->second != NULL);
        return it->second;
    }
}

Key const *InputState::GetKey (std::string const &name) const
{
    KeyNameMap::const_iterator it = m_keyname_map.find(name);
    if (it == m_keyname_map.end())
        return NULL;
    else
    {
        ASSERT1(it->second != NULL);
        return it->second;
    }
}

bool InputState::IsValidKeyCode (Key::Code const code) const
{
    return m_keycode_map.find(code) != m_keycode_map.end();
}

bool InputState::IsValidKeyName (std::string const &name) const
{
    return m_keyname_map.find(name) != m_keyname_map.end();
}

Key::Code InputState::KeyCode (std::string const &name) const
{
    KeyNameMap::const_iterator it = m_keyname_map.find(name);
    if (it == m_keyname_map.end())
        return Key::INVALID;
    else
    {
        ASSERT1(it->second != NULL);
        return it->second->GetCode();
    }
}

std::string const &InputState::KeyName (Key::Code const code) const
{
    KeyCodeMap::const_iterator it = m_keycode_map.find(code);
    if (it == m_keycode_map.end())
        return g_empty_string;
    else
    {
        ASSERT1(it->second != NULL);
        return it->second->Name();
    }
}

bool InputState::IsKeyPressed (Key::Code const code) const
{
    Key const *key = GetKey(code);
    if (key != NULL)
        return key->IsPressed();
    else
        return false;
}

bool InputState::IsKeyPressed (std::string const &name) const
{
    Key const *key = GetKey(name);
    if (key != NULL)
        return key->IsPressed();
    else
        return false;
}

bool InputState::IsEitherAltKeyPressed () const
{
    return GetKey(Key::LALT)->IsPressed() ||
           GetKey(Key::RALT)->IsPressed();
}

bool InputState::IsEitherControlKeyPressed () const
{
    return GetKey(Key::LCTRL)->IsPressed() ||
           GetKey(Key::RCTRL)->IsPressed();
}

bool InputState::IsEitherShiftKeyPressed () const
{
    return GetKey(Key::LSHIFT)->IsPressed() ||
           GetKey(Key::RSHIFT)->IsPressed();
}

Key::Modifier InputState::Modifier () const
{
    return Key::Modifier(
        (IsKeyPressed(Key::LALT)     ? Key::MOD_LALT   : 0) |
        (IsKeyPressed(Key::RALT)     ? Key::MOD_RALT   : 0) |
        (IsKeyPressed(Key::LCTRL)    ? Key::MOD_LCTRL  : 0) |
        (IsKeyPressed(Key::RCTRL)    ? Key::MOD_RCTRL  : 0) |
        (IsKeyPressed(Key::LSHIFT)   ? Key::MOD_LSHIFT : 0) |
        (IsKeyPressed(Key::RSHIFT)   ? Key::MOD_RSHIFT : 0) |
        (IsKeyPressed(Key::LMETA)    ? Key::MOD_LMETA  : 0) |
        (IsKeyPressed(Key::RMETA)    ? Key::MOD_RMETA  : 0) |
        (IsKeyPressed(Key::NUMLOCK)  ? Key::MOD_NUM    : 0) |
        (IsKeyPressed(Key::CAPSLOCK) ? Key::MOD_CAPS   : 0));
}

void InputState::ResetPressed ()
{
    for (KeyCodeMap::const_iterator it = m_keycode_map.begin(),
                                    it_end = m_keycode_map.end();
         it != it_end;
         ++it)
    {
        ASSERT1(it->second != NULL);
        it->second->ResetPressed();
    }
}

bool InputState::HandleEvent (Event const *const e)
{
    ASSERT1(e != NULL);

    Key::Code code;
    switch (e->GetEventType())
    {
        case Event::KEYDOWN:
            code = static_cast<EventKeyDown const *const>(e)->KeyCode();

            // do caps/num/scroll lock updating
            if (code == Key::CAPSLOCK)
                m_is_caps_lock_on = !m_is_caps_lock_on;
            else if (code == Key::NUMLOCK)
                m_is_num_lock_on = !m_is_num_lock_on;
            else if (code == Key::SCROLLLOCK)
                m_is_scroll_lock_on = !m_is_scroll_lock_on;
            break;

        case Event::KEYUP:
            code = static_cast<EventKeyUp const *const>(e)->KeyCode();
            break;

        case Event::MOUSEBUTTONDOWN:
            code = static_cast<EventMouseButtonDown const *const>(e)->ButtonCode();
            break;

        case Event::MOUSEBUTTONUP:
            code = static_cast<EventMouseButtonUp const *const>(e)->ButtonCode();
            break;

        default:
            return false;
    }

    KeyCodeMap::const_iterator it = m_keycode_map.find(code);
    if (it != m_keycode_map.end())
    {
        ASSERT1(it->second != NULL);
        return it->second->ProcessEvent(e);
    }
    // unknown key/mousebutton (this should never happen but it does)
    else
        return false;
}

void InputState::InitKeyMaps ()
{
    // NOTE: Key::INVALID is not and should not be mapped.
    m_keycode_map[Key::NONE] = Key::Create(Key::NONE, "NONE");
    m_keycode_map[Key::UNKNOWN] = Key::Create(Key::UNKNOWN, "UNKNOWN");
    m_keycode_map[Key::LEFTMOUSE] = Key::Create(Key::LEFTMOUSE, "LEFTMOUSE");
    m_keycode_map[Key::MIDDLEMOUSE] = Key::Create(Key::MIDDLEMOUSE, "MIDDLEMOUSE");
    m_keycode_map[Key::RIGHTMOUSE] = Key::Create(Key::RIGHTMOUSE, "RIGHTMOUSE");
    m_keycode_map[Key::MOUSEWHEELUP] = Key::Create(Key::MOUSEWHEELUP, "MOUSEWHEELUP");
    m_keycode_map[Key::MOUSEWHEELDOWN] = Key::Create(Key::MOUSEWHEELDOWN, "MOUSEWHEELDOWN");
    m_keycode_map[Key::BACKSPACE] = Key::Create(Key::BACKSPACE, "BACKSPACE");
    m_keycode_map[Key::TAB] = Key::Create(Key::TAB, "TAB");
    m_keycode_map[Key::CLEAR] = Key::Create(Key::CLEAR, "CLEAR");
    m_keycode_map[Key::RETURN] = Key::Create(Key::RETURN, "RETURN");
    m_keycode_map[Key::PAUSE] = Key::Create(Key::PAUSE, "PAUSE");
    m_keycode_map[Key::ESCAPE] = Key::Create(Key::ESCAPE, "ESCAPE");
    m_keycode_map[Key::SPACE] = Key::Create(Key::SPACE, "SPACE");
    m_keycode_map[Key::QUOTE] = Key::Create(Key::QUOTE, "'");
    m_keycode_map[Key::COMMA] = Key::Create(Key::COMMA, ",");
    m_keycode_map[Key::MINUS] = Key::Create(Key::MINUS, "-");
    m_keycode_map[Key::PERIOD] = Key::Create(Key::PERIOD, ".");
    m_keycode_map[Key::SLASH] = Key::Create(Key::SLASH, "/");
    m_keycode_map[Key::ZERO] = Key::Create(Key::ZERO, "0");
    m_keycode_map[Key::ONE] = Key::Create(Key::ONE, "1");
    m_keycode_map[Key::TWO] = Key::Create(Key::TWO, "2");
    m_keycode_map[Key::THREE] = Key::Create(Key::THREE, "3");
    m_keycode_map[Key::FOUR] = Key::Create(Key::FOUR, "4");
    m_keycode_map[Key::FIVE] = Key::Create(Key::FIVE, "5");
    m_keycode_map[Key::SIX] = Key::Create(Key::SIX, "6");
    m_keycode_map[Key::SEVEN] = Key::Create(Key::SEVEN, "7");
    m_keycode_map[Key::EIGHT] = Key::Create(Key::EIGHT, "8");
    m_keycode_map[Key::NINE] = Key::Create(Key::NINE, "9");
    m_keycode_map[Key::SEMICOLON] = Key::Create(Key::SEMICOLON, ";");
    m_keycode_map[Key::EQUALS] = Key::Create(Key::EQUALS, "=");
    m_keycode_map[Key::LEFTBRACKET] = Key::Create(Key::LEFTBRACKET, "[");
    m_keycode_map[Key::BACKSLASH] = Key::Create(Key::BACKSLASH, "\\");
    m_keycode_map[Key::RIGHTBRACKET] = Key::Create(Key::RIGHTBRACKET, "]");
    m_keycode_map[Key::BACKQUOTE] = Key::Create(Key::BACKQUOTE, "`");
    m_keycode_map[Key::A] = Key::Create(Key::A, "A");
    m_keycode_map[Key::B] = Key::Create(Key::B, "B");
    m_keycode_map[Key::C] = Key::Create(Key::C, "C");
    m_keycode_map[Key::D] = Key::Create(Key::D, "D");
    m_keycode_map[Key::E] = Key::Create(Key::E, "E");
    m_keycode_map[Key::F] = Key::Create(Key::F, "F");
    m_keycode_map[Key::G] = Key::Create(Key::G, "G");
    m_keycode_map[Key::H] = Key::Create(Key::H, "H");
    m_keycode_map[Key::I] = Key::Create(Key::I, "I");
    m_keycode_map[Key::J] = Key::Create(Key::J, "J");
    m_keycode_map[Key::K] = Key::Create(Key::K, "K");
    m_keycode_map[Key::L] = Key::Create(Key::L, "L");
    m_keycode_map[Key::M] = Key::Create(Key::M, "M");
    m_keycode_map[Key::N] = Key::Create(Key::N, "N");
    m_keycode_map[Key::O] = Key::Create(Key::O, "O");
    m_keycode_map[Key::P] = Key::Create(Key::P, "P");
    m_keycode_map[Key::Q] = Key::Create(Key::Q, "Q");
    m_keycode_map[Key::R] = Key::Create(Key::R, "R");
    m_keycode_map[Key::S] = Key::Create(Key::S, "S");
    m_keycode_map[Key::T] = Key::Create(Key::T, "T");
    m_keycode_map[Key::U] = Key::Create(Key::U, "U");
    m_keycode_map[Key::V] = Key::Create(Key::V, "V");
    m_keycode_map[Key::W] = Key::Create(Key::W, "W");
    m_keycode_map[Key::X] = Key::Create(Key::X, "X");
    m_keycode_map[Key::Y] = Key::Create(Key::Y, "Y");
    m_keycode_map[Key::Z] = Key::Create(Key::Z, "Z");
    m_keycode_map[Key::DELETE] = Key::Create(Key::DELETE, "DELETE");
    m_keycode_map[Key::WORLD_0] = Key::Create(Key::WORLD_0, "WORLD_0");
    m_keycode_map[Key::WORLD_1] = Key::Create(Key::WORLD_1, "WORLD_1");
    m_keycode_map[Key::WORLD_2] = Key::Create(Key::WORLD_2, "WORLD_2");
    m_keycode_map[Key::WORLD_3] = Key::Create(Key::WORLD_3, "WORLD_3");
    m_keycode_map[Key::WORLD_4] = Key::Create(Key::WORLD_4, "WORLD_4");
    m_keycode_map[Key::WORLD_5] = Key::Create(Key::WORLD_5, "WORLD_5");
    m_keycode_map[Key::WORLD_6] = Key::Create(Key::WORLD_6, "WORLD_6");
    m_keycode_map[Key::WORLD_7] = Key::Create(Key::WORLD_7, "WORLD_7");
    m_keycode_map[Key::WORLD_8] = Key::Create(Key::WORLD_8, "WORLD_8");
    m_keycode_map[Key::WORLD_9] = Key::Create(Key::WORLD_9, "WORLD_9");
    m_keycode_map[Key::WORLD_10] = Key::Create(Key::WORLD_10, "WORLD_10");
    m_keycode_map[Key::WORLD_11] = Key::Create(Key::WORLD_11, "WORLD_11");
    m_keycode_map[Key::WORLD_12] = Key::Create(Key::WORLD_12, "WORLD_12");
    m_keycode_map[Key::WORLD_13] = Key::Create(Key::WORLD_13, "WORLD_13");
    m_keycode_map[Key::WORLD_14] = Key::Create(Key::WORLD_14, "WORLD_14");
    m_keycode_map[Key::WORLD_15] = Key::Create(Key::WORLD_15, "WORLD_15");
    m_keycode_map[Key::WORLD_16] = Key::Create(Key::WORLD_16, "WORLD_16");
    m_keycode_map[Key::WORLD_17] = Key::Create(Key::WORLD_17, "WORLD_17");
    m_keycode_map[Key::WORLD_18] = Key::Create(Key::WORLD_18, "WORLD_18");
    m_keycode_map[Key::WORLD_19] = Key::Create(Key::WORLD_19, "WORLD_19");
    m_keycode_map[Key::WORLD_20] = Key::Create(Key::WORLD_20, "WORLD_20");
    m_keycode_map[Key::WORLD_21] = Key::Create(Key::WORLD_21, "WORLD_21");
    m_keycode_map[Key::WORLD_22] = Key::Create(Key::WORLD_22, "WORLD_22");
    m_keycode_map[Key::WORLD_23] = Key::Create(Key::WORLD_23, "WORLD_23");
    m_keycode_map[Key::WORLD_24] = Key::Create(Key::WORLD_24, "WORLD_24");
    m_keycode_map[Key::WORLD_25] = Key::Create(Key::WORLD_25, "WORLD_25");
    m_keycode_map[Key::WORLD_26] = Key::Create(Key::WORLD_26, "WORLD_26");
    m_keycode_map[Key::WORLD_27] = Key::Create(Key::WORLD_27, "WORLD_27");
    m_keycode_map[Key::WORLD_28] = Key::Create(Key::WORLD_28, "WORLD_28");
    m_keycode_map[Key::WORLD_29] = Key::Create(Key::WORLD_29, "WORLD_29");
    m_keycode_map[Key::WORLD_30] = Key::Create(Key::WORLD_30, "WORLD_30");
    m_keycode_map[Key::WORLD_31] = Key::Create(Key::WORLD_31, "WORLD_31");
    m_keycode_map[Key::WORLD_32] = Key::Create(Key::WORLD_32, "WORLD_32");
    m_keycode_map[Key::WORLD_33] = Key::Create(Key::WORLD_33, "WORLD_33");
    m_keycode_map[Key::WORLD_34] = Key::Create(Key::WORLD_34, "WORLD_34");
    m_keycode_map[Key::WORLD_35] = Key::Create(Key::WORLD_35, "WORLD_35");
    m_keycode_map[Key::WORLD_36] = Key::Create(Key::WORLD_36, "WORLD_36");
    m_keycode_map[Key::WORLD_37] = Key::Create(Key::WORLD_37, "WORLD_37");
    m_keycode_map[Key::WORLD_38] = Key::Create(Key::WORLD_38, "WORLD_38");
    m_keycode_map[Key::WORLD_39] = Key::Create(Key::WORLD_39, "WORLD_39");
    m_keycode_map[Key::WORLD_40] = Key::Create(Key::WORLD_40, "WORLD_40");
    m_keycode_map[Key::WORLD_41] = Key::Create(Key::WORLD_41, "WORLD_41");
    m_keycode_map[Key::WORLD_42] = Key::Create(Key::WORLD_42, "WORLD_42");
    m_keycode_map[Key::WORLD_43] = Key::Create(Key::WORLD_43, "WORLD_43");
    m_keycode_map[Key::WORLD_44] = Key::Create(Key::WORLD_44, "WORLD_44");
    m_keycode_map[Key::WORLD_45] = Key::Create(Key::WORLD_45, "WORLD_45");
    m_keycode_map[Key::WORLD_46] = Key::Create(Key::WORLD_46, "WORLD_46");
    m_keycode_map[Key::WORLD_47] = Key::Create(Key::WORLD_47, "WORLD_47");
    m_keycode_map[Key::WORLD_48] = Key::Create(Key::WORLD_48, "WORLD_48");
    m_keycode_map[Key::WORLD_49] = Key::Create(Key::WORLD_49, "WORLD_49");
    m_keycode_map[Key::WORLD_50] = Key::Create(Key::WORLD_50, "WORLD_50");
    m_keycode_map[Key::WORLD_51] = Key::Create(Key::WORLD_51, "WORLD_51");
    m_keycode_map[Key::WORLD_52] = Key::Create(Key::WORLD_52, "WORLD_52");
    m_keycode_map[Key::WORLD_53] = Key::Create(Key::WORLD_53, "WORLD_53");
    m_keycode_map[Key::WORLD_54] = Key::Create(Key::WORLD_54, "WORLD_54");
    m_keycode_map[Key::WORLD_55] = Key::Create(Key::WORLD_55, "WORLD_55");
    m_keycode_map[Key::WORLD_56] = Key::Create(Key::WORLD_56, "WORLD_56");
    m_keycode_map[Key::WORLD_57] = Key::Create(Key::WORLD_57, "WORLD_57");
    m_keycode_map[Key::WORLD_58] = Key::Create(Key::WORLD_58, "WORLD_58");
    m_keycode_map[Key::WORLD_59] = Key::Create(Key::WORLD_59, "WORLD_59");
    m_keycode_map[Key::WORLD_60] = Key::Create(Key::WORLD_60, "WORLD_60");
    m_keycode_map[Key::WORLD_61] = Key::Create(Key::WORLD_61, "WORLD_61");
    m_keycode_map[Key::WORLD_62] = Key::Create(Key::WORLD_62, "WORLD_62");
    m_keycode_map[Key::WORLD_63] = Key::Create(Key::WORLD_63, "WORLD_63");
    m_keycode_map[Key::WORLD_64] = Key::Create(Key::WORLD_64, "WORLD_64");
    m_keycode_map[Key::WORLD_65] = Key::Create(Key::WORLD_65, "WORLD_65");
    m_keycode_map[Key::WORLD_66] = Key::Create(Key::WORLD_66, "WORLD_66");
    m_keycode_map[Key::WORLD_67] = Key::Create(Key::WORLD_67, "WORLD_67");
    m_keycode_map[Key::WORLD_68] = Key::Create(Key::WORLD_68, "WORLD_68");
    m_keycode_map[Key::WORLD_69] = Key::Create(Key::WORLD_69, "WORLD_69");
    m_keycode_map[Key::WORLD_70] = Key::Create(Key::WORLD_70, "WORLD_70");
    m_keycode_map[Key::WORLD_71] = Key::Create(Key::WORLD_71, "WORLD_71");
    m_keycode_map[Key::WORLD_72] = Key::Create(Key::WORLD_72, "WORLD_72");
    m_keycode_map[Key::WORLD_73] = Key::Create(Key::WORLD_73, "WORLD_73");
    m_keycode_map[Key::WORLD_74] = Key::Create(Key::WORLD_74, "WORLD_74");
    m_keycode_map[Key::WORLD_75] = Key::Create(Key::WORLD_75, "WORLD_75");
    m_keycode_map[Key::WORLD_76] = Key::Create(Key::WORLD_76, "WORLD_76");
    m_keycode_map[Key::WORLD_77] = Key::Create(Key::WORLD_77, "WORLD_77");
    m_keycode_map[Key::WORLD_78] = Key::Create(Key::WORLD_78, "WORLD_78");
    m_keycode_map[Key::WORLD_79] = Key::Create(Key::WORLD_79, "WORLD_79");
    m_keycode_map[Key::WORLD_80] = Key::Create(Key::WORLD_80, "WORLD_80");
    m_keycode_map[Key::WORLD_81] = Key::Create(Key::WORLD_81, "WORLD_81");
    m_keycode_map[Key::WORLD_82] = Key::Create(Key::WORLD_82, "WORLD_82");
    m_keycode_map[Key::WORLD_83] = Key::Create(Key::WORLD_83, "WORLD_83");
    m_keycode_map[Key::WORLD_84] = Key::Create(Key::WORLD_84, "WORLD_84");
    m_keycode_map[Key::WORLD_85] = Key::Create(Key::WORLD_85, "WORLD_85");
    m_keycode_map[Key::WORLD_86] = Key::Create(Key::WORLD_86, "WORLD_86");
    m_keycode_map[Key::WORLD_87] = Key::Create(Key::WORLD_87, "WORLD_87");
    m_keycode_map[Key::WORLD_88] = Key::Create(Key::WORLD_88, "WORLD_88");
    m_keycode_map[Key::WORLD_89] = Key::Create(Key::WORLD_89, "WORLD_89");
    m_keycode_map[Key::WORLD_90] = Key::Create(Key::WORLD_90, "WORLD_90");
    m_keycode_map[Key::WORLD_91] = Key::Create(Key::WORLD_91, "WORLD_91");
    m_keycode_map[Key::WORLD_92] = Key::Create(Key::WORLD_92, "WORLD_92");
    m_keycode_map[Key::WORLD_93] = Key::Create(Key::WORLD_93, "WORLD_93");
    m_keycode_map[Key::WORLD_94] = Key::Create(Key::WORLD_94, "WORLD_94");
    m_keycode_map[Key::WORLD_95] = Key::Create(Key::WORLD_95, "WORLD_95");
    m_keycode_map[Key::KP0] = Key::Create(Key::KP0, "KP0");
    m_keycode_map[Key::KP1] = Key::Create(Key::KP1, "KP1");
    m_keycode_map[Key::KP2] = Key::Create(Key::KP2, "KP2");
    m_keycode_map[Key::KP3] = Key::Create(Key::KP3, "KP3");
    m_keycode_map[Key::KP4] = Key::Create(Key::KP4, "KP4");
    m_keycode_map[Key::KP5] = Key::Create(Key::KP5, "KP5");
    m_keycode_map[Key::KP6] = Key::Create(Key::KP6, "KP6");
    m_keycode_map[Key::KP7] = Key::Create(Key::KP7, "KP7");
    m_keycode_map[Key::KP8] = Key::Create(Key::KP8, "KP8");
    m_keycode_map[Key::KP9] = Key::Create(Key::KP9, "KP9");
    m_keycode_map[Key::KP_PERIOD] = Key::Create(Key::KP_PERIOD, "KP_PERIOD");
    m_keycode_map[Key::KP_DIVIDE] = Key::Create(Key::KP_DIVIDE, "KP_DIVIDE");
    m_keycode_map[Key::KP_MULTIPLY] = Key::Create(Key::KP_MULTIPLY, "KP_MULTIPLY");
    m_keycode_map[Key::KP_MINUS] = Key::Create(Key::KP_MINUS, "KP_MINUS");
    m_keycode_map[Key::KP_PLUS] = Key::Create(Key::KP_PLUS, "KP_PLUS");
    m_keycode_map[Key::KP_ENTER] = Key::Create(Key::KP_ENTER, "KP_ENTER");
    m_keycode_map[Key::KP_EQUALS] = Key::Create(Key::KP_EQUALS, "KP_EQUALS");
    m_keycode_map[Key::UP] = Key::Create(Key::UP, "UP");
    m_keycode_map[Key::DOWN] = Key::Create(Key::DOWN, "DOWN");
    m_keycode_map[Key::RIGHT] = Key::Create(Key::RIGHT, "RIGHT");
    m_keycode_map[Key::LEFT] = Key::Create(Key::LEFT, "LEFT");
    m_keycode_map[Key::INSERT] = Key::Create(Key::INSERT, "INSERT");
    m_keycode_map[Key::HOME] = Key::Create(Key::HOME, "HOME");
    m_keycode_map[Key::END] = Key::Create(Key::END, "END");
    m_keycode_map[Key::PAGEUP] = Key::Create(Key::PAGEUP, "PAGEUP");
    m_keycode_map[Key::PAGEDOWN] = Key::Create(Key::PAGEDOWN, "PAGEDOWN");
    m_keycode_map[Key::F1] = Key::Create(Key::F1, "F1");
    m_keycode_map[Key::F2] = Key::Create(Key::F2, "F2");
    m_keycode_map[Key::F3] = Key::Create(Key::F3, "F3");
    m_keycode_map[Key::F4] = Key::Create(Key::F4, "F4");
    m_keycode_map[Key::F5] = Key::Create(Key::F5, "F5");
    m_keycode_map[Key::F6] = Key::Create(Key::F6, "F6");
    m_keycode_map[Key::F7] = Key::Create(Key::F7, "F7");
    m_keycode_map[Key::F8] = Key::Create(Key::F8, "F8");
    m_keycode_map[Key::F9] = Key::Create(Key::F9, "F9");
    m_keycode_map[Key::F10] = Key::Create(Key::F10, "F10");
    m_keycode_map[Key::F11] = Key::Create(Key::F11, "F11");
    m_keycode_map[Key::F12] = Key::Create(Key::F12, "F12");
    m_keycode_map[Key::F13] = Key::Create(Key::F13, "F13");
    m_keycode_map[Key::F14] = Key::Create(Key::F14, "F14");
    m_keycode_map[Key::F15] = Key::Create(Key::F15, "F15");
    m_keycode_map[Key::NUMLOCK] = Key::Create(Key::NUMLOCK, "NUMLOCK");
    m_keycode_map[Key::CAPSLOCK] = Key::Create(Key::CAPSLOCK, "CAPSLOCK");
    m_keycode_map[Key::SCROLLLOCK] = Key::Create(Key::SCROLLLOCK, "SCROLLOCK");
    m_keycode_map[Key::RSHIFT] = Key::Create(Key::RSHIFT, "RIGHTSHIFT");
    m_keycode_map[Key::LSHIFT] = Key::Create(Key::LSHIFT, "LEFTSHIFT");
    m_keycode_map[Key::RCTRL] = Key::Create(Key::RCTRL, "RIGHTCONTROL");
    m_keycode_map[Key::LCTRL] = Key::Create(Key::LCTRL, "LEFTCONTROL");
    m_keycode_map[Key::RALT] = Key::Create(Key::RALT, "RIGHTALT");
    m_keycode_map[Key::LALT] = Key::Create(Key::LALT, "LEFTALT");
    m_keycode_map[Key::RMETA] = Key::Create(Key::RMETA, "RIGHTMETA");
    m_keycode_map[Key::LMETA] = Key::Create(Key::LMETA, "LEFTMETA");
    m_keycode_map[Key::LWINBLOWS] = Key::Create(Key::LWINBLOWS, "LEFTWINDOWS");
    m_keycode_map[Key::RWINBLOWS] = Key::Create(Key::RWINBLOWS, "RIGHTWINDEOWS");
    m_keycode_map[Key::MODE] = Key::Create(Key::MODE, "MODE");
    m_keycode_map[Key::COMPOSE] = Key::Create(Key::COMPOSE, "COMPOSE");
    m_keycode_map[Key::HELP] = Key::Create(Key::HELP, "HELP");
    m_keycode_map[Key::PRINT] = Key::Create(Key::PRINT, "PRINT");
    m_keycode_map[Key::SYSREQ] = Key::Create(Key::SYSREQ, "SYSREQ");
    m_keycode_map[Key::BREAK] = Key::Create(Key::BREAK, "BREAK");
    m_keycode_map[Key::MENU] = Key::Create(Key::MENU, "MENU");
    m_keycode_map[Key::POWER] = Key::Create(Key::POWER, "POWER");
    m_keycode_map[Key::EURO] = Key::Create(Key::EURO, "EURO");
    m_keycode_map[Key::UNDO] = Key::Create(Key::UNDO, "UNDO");

    // now create the name-key binding
    for (KeyCodeMap::const_iterator it = m_keycode_map.begin(),
                            it_end = m_keycode_map.end();
         it != it_end;
         ++it)
    {
        ASSERT1(it->second != NULL);
        m_keyname_map[it->second->Name()] = it->second;
    }
}

} // end of namespace Xrb
