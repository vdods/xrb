// ///////////////////////////////////////////////////////////////////////////
// dis_config.h by Victor Dods, created 2006/08/04
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_CONFIG_H_)
#define _DIS_CONFIG_H_

#include "xrb.h"

#include <string>

#include "xrb_key.h"
#include "xrb_screencoord.h"

using namespace Xrb;

namespace Dis
{

// this is designed to be able to read multiple config files -- perhaps a
// "default" config file first to initialize default values, and then the
// user's custom config file.
class Config
{
public:

    Config ();

    inline bool GetFullscreen () const { return m_fullscreen; }
    inline ScreenCoordVector2 const &GetResolution () const { return m_resolution; }
    inline std::string const &GetKeyMapName () const { return m_key_map_name; }
    inline Key::Code GetKeyMoveRight () const { return m_key_move_right; }
    inline Key::Code GetKeyMoveLeft () const { return m_key_move_left; }
    inline Key::Code GetKeyMoveForward () const { return m_key_move_forward; }
    inline Key::Code GetKeyMoveBack () const { return m_key_move_back; }
    inline Key::Code GetKeyEngineBrake () const { return m_key_engine_brake; }
    inline Key::Code GetKeyUseTractor () const { return m_key_use_tractor; }

    void ResetToDefaults ();

    void Read (std::string const &config_filename);
    void Write (std::string const &config_filename) const;

private:

    bool m_fullscreen;
    ScreenCoordVector2 m_resolution;
    std::string m_key_map_name;
    Key::Code m_key_move_right;
    Key::Code m_key_move_left;
    Key::Code m_key_move_forward;
    Key::Code m_key_move_back;
    Key::Code m_key_engine_brake;
    Key::Code m_key_use_tractor;
}; // end of class Config

} // end of namespace Dis

#endif // !defined(_DIS_CONFIG_H_)
