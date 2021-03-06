// ///////////////////////////////////////////////////////////////////////////
// xrb_keymap.hpp by Victor Dods, created 2006/07/29
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_KEYMAP_HPP_)
#define _XRB_KEYMAP_HPP_

#include "xrb.hpp"

#include <map>
#include <string>

#include "xrb_key.hpp"

namespace Xrb
{

/** This is really only necessary in windows, because the windows version of
  * SDL does not handle alternate keyboard layouts correctly (it always
  * uses standard QWERTY).
  * @brief Interface for alternate key maps (e.g. Dvorak).
  */
class KeyMap
{
public:

    virtual ~KeyMap () { }

    /** Valid values:
      *
      * <ul>
      * <li>@c "dvorak" -- @ref KeyMapDvorak </li>
      * </ul>
      *
      * Anything else will create @ref KeyMapIdentity .
      *
      * @brief Creates a KeyMap implementation via name.
      */
    static KeyMap const *Create (std::string name);

    virtual std::string Name () const = 0;
    virtual Key::Code MappedKey (Key::Code code) const = 0;
}; // end of class KeyMap

class KeyMapIdentity : public KeyMap
{
public:

    virtual std::string Name () const { return "none"; }
    virtual Key::Code MappedKey (Key::Code code) const { return code; }
}; // end of KeyMapIdentity

class KeyMapDvorak : public KeyMap
{
public:

    KeyMapDvorak ();

    virtual std::string Name () const { return "dvorak"; }
    virtual Key::Code MappedKey (Key::Code code) const;

private:

    typedef std::map<Key::Code, Key::Code> QwertyToDvorakMap;

    QwertyToDvorakMap m_map;
}; // end of class KeyMapDvorak

} // end of namespace Xrb

#endif // !defined(_XRB_KEYMAP_HPP_)

