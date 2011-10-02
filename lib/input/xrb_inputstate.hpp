// ///////////////////////////////////////////////////////////////////////////
// xrb_inputstate.hpp by Victor Dods, created 2004/06/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_INPUTSTATE_HPP_)
#define _XRB_INPUTSTATE_HPP_

#include "xrb.hpp"

#include <list>
#include <map>
#include <string>

#include "xrb_key.hpp"
#include "xrb_eventhandler.hpp"

namespace Xrb {

/// @brief Stores the current state of the keyboard and mouse buttons.
/// @details The InputState class will process events, and store the state of the keyboard and mouse buttons
/// in a map (which indicates if any key is pressed or not).  Key modifiers are also tracked and stored.
class InputState : public EventHandler
{
public:

    InputState ();
    ~InputState ();

    Key const *GetKey (Key::Code code) const;
    Key const *GetKey (std::string const &name) const;
    bool IsValidKeyCode (Key::Code code) const;
    bool IsValidKeyName (std::string const &name) const;
    Key::Code KeyCode (std::string const &name) const;
    std::string const &KeyName (Key::Code code) const;
    bool IsKeyPressed (Key::Code const code) const;
    bool IsKeyPressed (std::string const &name) const;
    bool IsEitherAltKeyPressed () const;
    bool IsEitherControlKeyPressed () const;
    bool IsEitherShiftKeyPressed () const;
    bool IsCapsLockOn () const { return m_is_caps_lock_on; }
    bool IsNumLockOn () const { return m_is_num_lock_on; }
    bool IsScrollLockOn () const { return m_is_scroll_lock_on; }
    Key::Modifier Modifier () const;

    // calls ResetPressed on all the keys
    void ResetPressed ();

protected:

    // processes an event
    virtual bool HandleEvent (Event const &e);
    void InitKeyMaps ();

private:

    // mapping Key::Code to Key objects, so key codes can access
    typedef std::map<Key::Code, Key *> KeyCodeMap;
    // mapping std::string names to Key objects, so there is human-readable access
    typedef std::map<std::string, Key *> KeyNameMap;

    /// @brief The keycode-to-key map.
    /// @details Stores the command-bind for each, each entry mapped to an Key::Code.
    /*mutable */KeyCodeMap m_keycode_map;
    /// @brief the name-to-key map
    /// @details Maps everything from m_keycode_map to a human-readable std::string
    /// which is the name of each key (i.e. "a" or "LALT" or "F11").
    /*mutable */KeyNameMap m_keyname_map;
    /// Indicates if the caps lock is engaged.
    bool m_is_caps_lock_on;
    /// Indicates if the num lock is engaged.
    bool m_is_num_lock_on;
    /// Indicates if the scroll lock is engaged.
    bool m_is_scroll_lock_on;
}; // end of class InputState

} // end of namespace Xrb

#endif // !defined(_XRB_INPUTSTATE_HPP_)

