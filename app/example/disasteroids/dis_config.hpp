// ///////////////////////////////////////////////////////////////////////////
// dis_config.hpp by Victor Dods, created 2006/08/04
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_CONFIG_HPP_)
#define _DIS_CONFIG_HPP_

#include "xrb.hpp"

#include <string>

#include "dis_enums.hpp"
#include "xrb_key.hpp"
#include "xrb_screencoord.hpp"

using namespace Xrb;

namespace Dis
{

// this is designed to be able to read multiple config files -- perhaps a
// "default" config file first to initialize default values, and then the
// user's custom config file.
class Config
{
public:

    static std::string const ms_input_action_label[KEY_INPUT_ACTION_COUNT];

    Config ();

    // ///////////////////////////////////////////////////////////////////////
    // general accessors
    // ///////////////////////////////////////////////////////////////////////

    inline bool GetBoolean (KeyBoolean key) const { ASSERT1(key >= 0 && key < KEY_BOOLEAN_COUNT); return m_boolean_value[key]; }
    inline Uint32 GetUint32 (KeyUint32 key) const { ASSERT1(key >= 0 && key < KEY_UINT32_COUNT); return m_uint32_value[key]; }
    inline std::string const &GetString (KeyString key) const { ASSERT1(key >= 0 && key < KEY_STRING_COUNT); return m_string_value[key]; }
    Key::Code GetInputAction (KeyInputAction key) const;

    // ///////////////////////////////////////////////////////////////////////
    // general modifiers
    // ///////////////////////////////////////////////////////////////////////

    inline void SetBoolean (KeyBoolean key, bool value) { ASSERT1(key >= 0 && key < KEY_BOOLEAN_COUNT); m_boolean_value[key] = value; }
    inline void SetUint32 (KeyUint32 key, Uint32 value) { ASSERT1(key >= 0 && key < KEY_UINT32_COUNT); m_uint32_value[key] = value; }
    inline void SetString (KeyString key, std::string const &value) { ASSERT1(key >= 0 && key < KEY_STRING_COUNT); m_string_value[key] = value; }
    void SetInputAction (KeyInputAction key, Key::Code value);
    void SetInputAction (KeyInputAction key, std::string const &value);

    // ///////////////////////////////////////////////////////////////////////
    // specific accessors (used to avoid lots of casting in the app code)
    // ///////////////////////////////////////////////////////////////////////

    inline ScreenCoordVector2 GetResolution () const { return ScreenCoordVector2(GetResolutionX(), GetResolutionY()); }
    inline ScreenCoord GetResolutionX () const { return static_cast<ScreenCoord>(GetUint32(VIDEO__RESOLUTION_X)); }
    inline ScreenCoord GetResolutionY () const { return static_cast<ScreenCoord>(GetUint32(VIDEO__RESOLUTION_Y)); }
    inline DifficultyLevel GetDifficultyLevel () const { return static_cast<DifficultyLevel>(GetUint32(GAME__DIFFICULTY_LEVEL)); }

    // ///////////////////////////////////////////////////////////////////////
    // specific modifiers (used to avoid lots of casting in the app code)
    // ///////////////////////////////////////////////////////////////////////

    inline void SetResolution (ScreenCoordVector2 const &resolution) { SetResolutionX(resolution[Dim::X]); SetResolutionY(resolution[Dim::Y]); }
    inline void SetResolutionX (ScreenCoord resolution_x) { SetUint32(VIDEO__RESOLUTION_X, static_cast<Uint32>(resolution_x)); }
    inline void SetResolutionY (ScreenCoord resolution_y) { SetUint32(VIDEO__RESOLUTION_Y, static_cast<Uint32>(resolution_y)); }
    inline void SetDifficultyLevel (DifficultyLevel difficulty_level) { SetUint32(GAME__DIFFICULTY_LEVEL, static_cast<Uint32>(difficulty_level)); }

    // ///////////////////////////////////////////////////////////////////////
    // procedures
    // ///////////////////////////////////////////////////////////////////////

    void ResetToDefaults ();

    void Read (std::string const &config_filename, bool reset_to_defaults_before_reading = true);
    void Write (std::string const &config_filename) const;

private:

    template <typename ValueType>
    struct KeySpecification
    {
        char const *m_data_file_path;
        ValueType m_default_value;
    }; // end of struct Config::KeySpecification<ValueType>

    static KeySpecification<bool> const ms_boolean_key[KEY_BOOLEAN_COUNT];
    static KeySpecification<Uint32> const ms_uint32_key[KEY_UINT32_COUNT];
    static KeySpecification<char const *> const ms_string_key[KEY_STRING_COUNT];
    static KeySpecification<Key::Code> const ms_input_action_key[KEY_INPUT_ACTION_COUNT];

    bool m_boolean_value[KEY_BOOLEAN_COUNT];
    Uint32 m_uint32_value[KEY_UINT32_COUNT];
    std::string m_string_value[KEY_STRING_COUNT];
    mutable std::string m_input_action_name[KEY_INPUT_ACTION_COUNT];
    mutable Key::Code m_input_action_value[KEY_INPUT_ACTION_COUNT];
}; // end of class Config

} // end of namespace Dis

#endif // !defined(_DIS_CONFIG_HPP_)
