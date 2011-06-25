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

    bool Boolean (KeyBoolean key) const { ASSERT1(key >= 0 && key < KEY_BOOLEAN_COUNT); return m_boolean_value[key]; }
    Uint32 GetUint32 (KeyUint32 key) const { ASSERT1(key >= 0 && key < KEY_UINT32_COUNT); return m_uint32_value[key]; }
    std::string const &GetString (KeyString key) const { ASSERT1(key >= 0 && key < KEY_STRING_COUNT); return m_string_value[key]; }
    Key::Code InputAction (KeyInputAction key) const;

    // ///////////////////////////////////////////////////////////////////////
    // general modifiers
    // ///////////////////////////////////////////////////////////////////////

    void SetBoolean (KeyBoolean key, bool value) { ASSERT1(key >= 0 && key < KEY_BOOLEAN_COUNT); m_boolean_value[key] = value; }
    void SetUint32 (KeyUint32 key, Uint32 value) { ASSERT1(key >= 0 && key < KEY_UINT32_COUNT); m_uint32_value[key] = value; }
    void SetString (KeyString key, std::string const &value) { ASSERT1(key >= 0 && key < KEY_STRING_COUNT); m_string_value[key] = value; }
    void SetInputAction (KeyInputAction key, Key::Code value);
    void SetInputAction (KeyInputAction key, std::string const &value);

    // ///////////////////////////////////////////////////////////////////////
    // specific accessors (used to avoid lots of casting in the app code)
    // ///////////////////////////////////////////////////////////////////////

    ScreenCoordVector2 Resolution () const { return ScreenCoordVector2(ResolutionX(), ResolutionY()); }
    ScreenCoord ResolutionX () const { return static_cast<ScreenCoord>(GetUint32(VIDEO__RESOLUTION_X)); }
    ScreenCoord ResolutionY () const { return static_cast<ScreenCoord>(GetUint32(VIDEO__RESOLUTION_Y)); }
    DifficultyLevel GetDifficultyLevel () const { return static_cast<DifficultyLevel>(GetUint32(GAME__DIFFICULTY_LEVEL)); }
    ScreenCoordVector2 GlTextureAtlasSize () const { return ScreenCoordVector2(GlTextureAtlasSizeX(), GlTextureAtlasSizeY()); }
    ScreenCoord GlTextureAtlasSizeX () const { return static_cast<ScreenCoord>(GetUint32(VIDEO__GLTEXTURE_ATLAS_SIZE_X)); }
    ScreenCoord GlTextureAtlasSizeY () const { return static_cast<ScreenCoord>(GetUint32(VIDEO__GLTEXTURE_ATLAS_SIZE_Y)); }

    // ///////////////////////////////////////////////////////////////////////
    // specific modifiers (used to avoid lots of casting in the app code)
    // ///////////////////////////////////////////////////////////////////////

    void SetResolution (ScreenCoordVector2 const &resolution) { SetResolutionX(resolution[Dim::X]); SetResolutionY(resolution[Dim::Y]); }
    void SetResolutionX (ScreenCoord resolution_x) { SetUint32(VIDEO__RESOLUTION_X, static_cast<Uint32>(resolution_x)); }
    void SetResolutionY (ScreenCoord resolution_y) { SetUint32(VIDEO__RESOLUTION_Y, static_cast<Uint32>(resolution_y)); }
    void SetDifficultyLevel (DifficultyLevel difficulty_level) { SetUint32(GAME__DIFFICULTY_LEVEL, static_cast<Uint32>(difficulty_level)); }
    void SetGlTextureAtlasSize (ScreenCoordVector2 const &gltexture_atlas_size)
    {
        SetGlTextureAtlasSizeX(gltexture_atlas_size[Dim::X]);
        SetGlTextureAtlasSizeY(gltexture_atlas_size[Dim::Y]);
    }
    void SetGlTextureAtlasSizeX (ScreenCoord gltexture_atlas_size_x) { SetUint32(VIDEO__GLTEXTURE_ATLAS_SIZE_X, static_cast<Uint32>(gltexture_atlas_size_x)); }
    void SetGlTextureAtlasSizeY (ScreenCoord gltexture_atlas_size_y) { SetUint32(VIDEO__GLTEXTURE_ATLAS_SIZE_Y, static_cast<Uint32>(gltexture_atlas_size_y)); }

    // ///////////////////////////////////////////////////////////////////////
    // procedures
    // ///////////////////////////////////////////////////////////////////////

    void ResetToDefaults ();

    void Read (std::string const &config_file_path, bool reset_to_defaults_before_reading = true);
    void Write (std::string const &config_file_path) const;

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
