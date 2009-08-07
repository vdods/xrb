// ///////////////////////////////////////////////////////////////////////////
// xrb_keymap.cpp by Victor Dods, created 2006/07/29
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_keymap.hpp"

#include "xrb_util.hpp"

namespace Xrb
{

KeyMap const *KeyMap::Create (std::string name)
{
    Util::MakeLowercase(&name);
    if (name == "dvorak")
        return new KeyMapDvorak();
    else
        return new KeyMapIdentity();
}

KeyMapDvorak::KeyMapDvorak ()
    :
    KeyMap()
{
    // only need to map the codes that are different from qwerty

    // top row of keys (the backquote, digits and backspace do not differ)
    m_map[Key::MINUS] = Key::LEFTBRACKET;
    m_map[Key::EQUALS] = Key::RIGHTBRACKET;

    // second row of keys (the backslash does not differ)
    m_map[Key::Q] = Key::QUOTE;
    m_map[Key::W] = Key::COMMA;
    m_map[Key::E] = Key::PERIOD;
    m_map[Key::R] = Key::P;
    m_map[Key::T] = Key::Y;
    m_map[Key::Y] = Key::F;
    m_map[Key::U] = Key::G;
    m_map[Key::I] = Key::C;
    m_map[Key::O] = Key::R;
    m_map[Key::P] = Key::L;
    m_map[Key::LEFTBRACKET] = Key::SLASH;
    m_map[Key::RIGHTBRACKET] = Key::EQUALS;

    // second row of keys
    m_map[Key::A] = Key::A; // technically unnecessary because it maps to itself
    m_map[Key::S] = Key::O;
    m_map[Key::D] = Key::E;
    m_map[Key::F] = Key::U;
    m_map[Key::G] = Key::I;
    m_map[Key::H] = Key::D;
    m_map[Key::J] = Key::H;
    m_map[Key::K] = Key::T;
    m_map[Key::L] = Key::N;
    m_map[Key::SEMICOLON] = Key::S;
    m_map[Key::QUOTE] = Key::MINUS;

    // bottom row of keys
    m_map[Key::Z] = Key::SEMICOLON;
    m_map[Key::X] = Key::Q;
    m_map[Key::C] = Key::J;
    m_map[Key::V] = Key::K;
    m_map[Key::B] = Key::X;
    m_map[Key::N] = Key::B;
    m_map[Key::M] = Key::M; // technically unnecessary because it maps to itself
    m_map[Key::COMMA] = Key::W;
    m_map[Key::PERIOD] = Key::V;
    m_map[Key::SLASH] = Key::Z;
}

Key::Code KeyMapDvorak::MappedKey (Key::Code const code) const
{
    QwertyToDvorakMap::const_iterator it = m_map.find(code);
    if (it == m_map.end())
        return code;
    else
        return it->second;
}

} // end of namespace Xrb
