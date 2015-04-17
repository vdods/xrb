// ///////////////////////////////////////////////////////////////////////////
// dis_config.cpp by Victor Dods, created 2006/08/04
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_config.hpp"

#include <fstream>

#include "xrb_filesystem.hpp"
#include "xrb_math.hpp"
#include "xrb_parse_datafile.hpp"
#include "xrb_parse_datafile_value.hpp"
#include "xrb_inputstate.hpp"
#include "xrb_singleton.hpp"
#include "xrb_util.hpp"

using namespace std;
using namespace Xrb;
using namespace Parse;

namespace Dis {

std::string const Config::ms_input_action_label[KEY_INPUT_ACTION_COUNT] =
{
    "Open Inventory",                   // INPUT__OPEN_INVENTORY = 0,
    "Move Forward",                     // INPUT__MOVE_FORWARD,
    "Move Left",                        // INPUT__MOVE_LEFT,
    "Move Back",                        // INPUT__MOVE_BACK,
    "Move Right",                       // INPUT__MOVE_RIGHT,
    "Primary Fire",                     // INPUT__PRIMARY_FIRE,
    "Secondary Fire",                   // INPUT__SECONDARY_FIRE,
    "Engine Brake",                     // INPUT__ENGINE_BRAKE,
    "Use Tractor",                      // INPUT__USE_TRACTOR,
    "Equip Pea Shooter",                // INPUT__EQUIP_PEA_SHOOTER,
    "Equip Laser",                      // INPUT__EQUIP_LASER,
    "Equip Flame Thrower",              // INPUT__EQUIP_FLAME_THROWER,
    "Equip Gauss Gun",                  // INPUT__EQUIP_GAUSS_GUN,
    "Equip Grenade Launcher",           // INPUT__EQUIP_GRENADE_LAUNCHER,
    "Equip Missile Launcher",           // INPUT__EQUIP_MISSILE_LAUNCHER,
    "Activate Option EMP",              // INPUT__ACTIVATE_OPTION__EMP,
    "Activate Option Attack Boost",     // INPUT__ACTIVATE_OPTION__ATTACK_BOOST,
    "Activate Option Defense Boost",    // INPUT__ACTIVATE_OPTION__DEFENSE_BOOST,
    "Activate Option Time Stretch",     // INPUT__ACTIVATE_OPTION__TIME_STRETCH,
    "Activate Selected Option",         // INPUT__ACTIVATE_OPTION__SELECTED,
    "Select Next Option",               // INPUT__SELECT_OPTION__NEXT,
    "Select Previous Option",           // INPUT__SELECT_OPTION__PREVIOUS,
};

Config::KeySpecification<bool> const Config::ms_boolean_key[KEY_BOOLEAN_COUNT] =
{
    {"|system|debug_mode",                      false},             // SYSTEM__DEBUG_MODE = 0,
    {"|video|fullscreen",                       true},              // VIDEO__FULLSCREEN,
};
Config::KeySpecification<Uint32> const Config::ms_uint32_key[KEY_UINT32_COUNT] =
{
    {"|game|difficulty_level",                  DL_LOWEST},         // GAME__DIFFICULTY_LEVEL = 0,
    {"|video|resolution_x",                     1024},              // VIDEO__RESOLUTION_X,
    {"|video|resolution_y",                     768},               // VIDEO__RESOLUTION_Y,
    {"|video|gltexture_atlas_size_x",           1024},              // VIDEO__GLTEXTURE_ATLAS_SIZE_X,
    {"|video|gltexture_atlas_size_y",           1024},              // VIDEO__GLTEXTURE_ATLAS_SIZE_Y,
};
Config::KeySpecification<char const *> const Config::ms_string_key[KEY_STRING_COUNT] =
{
    {"|system|key_map_name",                    "none"},            // SYSTEM__KEY_MAP_NAME = 0,
};
Config::KeySpecification<Key::Code> const Config::ms_input_action_key[KEY_INPUT_ACTION_COUNT] =
{
    {"|input|open_inventory",                   Key::ESCAPE},       // INPUT__OPEN_INVENTORY = 0,
    {"|input|move_forward",                     Key::W},            // INPUT__MOVE_FORWARD,
    {"|input|move_left",                        Key::A},            // INPUT__MOVE_LEFT,
    {"|input|move_back",                        Key::S},            // INPUT__MOVE_BACK,
    {"|input|move_right",                       Key::D},            // INPUT__MOVE_RIGHT,
    {"|input|primary_fire",                     Key::LEFTMOUSE},    // INPUT__PRIMARY_FIRE,
    {"|input|secondary_fire",                   Key::RIGHTMOUSE},   // INPUT__SECONDARY_FIRE,
    {"|input|engine_brake",                     Key::C},            // INPUT__ENGINE_BRAKE,
    {"|input|use_tractor",                      Key::SPACE},        // INPUT__USE_TRACTOR,
    {"|input|equip_weapon1_pea_shooter",        Key::ONE},          // INPUT__EQUIP_PEA_SHOOTER,
    {"|input|equip_weapon2_laser",              Key::TWO},          // INPUT__EQUIP_LASER,
    {"|input|equip_weapon3_flame_thrower",      Key::THREE},        // INPUT__EQUIP_FLAME_THROWER,
    {"|input|equip_weapon4_gauss_gun",          Key::FOUR},         // INPUT__EQUIP_GAUSS_GUN,
    {"|input|equip_weapon5_grenade_launcher",   Key::FIVE},         // INPUT__EQUIP_GRENADE_LAUNCHER,
    {"|input|equip_weapon6_missile_launcher",   Key::SIX},          // INPUT__EQUIP_MISSILE_LAUNCHER,
    {"|input|activate_option_emp",              Key::SEVEN},        // INPUT__ACTIVATE_OPTION__EMP,
    {"|input|activate_option_attack_boost",     Key::EIGHT},        // INPUT__ACTIVATE_OPTION__ATTACK_BOOST,
    {"|input|activate_option_defense_boost",    Key::NINE},         // INPUT__ACTIVATE_OPTION__DEFENSE_BOOST,
    {"|input|activate_option_time_stretch",     Key::ZERO},         // INPUT__ACTIVATE_OPTION__TIME_STRETCH,
    {"|input|activate_option_selected",         Key::E},            // INPUT__ACTIVATE_OPTION__SELECTED,
    {"|input|select_option_next",               Key::R},            // INPUT__SELECT_OPTION__NEXT,
    {"|input|select_option_previous",           Key::F},            // INPUT__SELECT_OPTION__PREVIOUS,
};

Config::Config ()
{
    ResetToDefaults();
}

Key::Code Config::InputAction (KeyInputAction key) const
{
    ASSERT1(key >= 0 && key < KEY_INPUT_ACTION_COUNT);

    if (!m_input_action_name[key].empty())
    {
        m_input_action_value[key] = Singleton::InputState().KeyCode(m_input_action_name[key]);
        m_input_action_name[key].clear();
    }

    if (m_input_action_value[key] == Key::INVALID)
        m_input_action_value[key] = ms_input_action_key[key].m_default_value;

    return m_input_action_value[key];
}

void Config::SetInputAction (KeyInputAction const key, Key::Code const value)
{
    ASSERT1(key >= 0 && key < KEY_INPUT_ACTION_COUNT);
    ASSERT1(Singleton::InputState().IsValidKeyCode(ms_input_action_key[key].m_default_value));
    m_input_action_name[key].clear();
    m_input_action_value[key] =
        Singleton::InputState().IsValidKeyCode(value) ?
        value :
        ms_input_action_key[key].m_default_value;
}

void Config::SetInputAction (KeyInputAction const key, std::string const &value)
{
    ASSERT1(key >= 0 && key < KEY_INPUT_ACTION_COUNT);
    ASSERT1(!value.empty());
    m_input_action_name[key] = value;
    Util::MakeUppercase(m_input_action_name[key]);
}

void Config::ResetToDefaults ()
{
    for (Uint32 i = 0; i < KEY_BOOLEAN_COUNT; ++i)
        m_boolean_value[i] = ms_boolean_key[i].m_default_value;
    for (Uint32 i = 0; i < KEY_UINT32_COUNT; ++i)
        m_uint32_value[i] = ms_uint32_key[i].m_default_value;
    for (Uint32 i = 0; i < KEY_STRING_COUNT; ++i)
        m_string_value[i] = ms_string_key[i].m_default_value;
    for (Uint32 i = 0; i < KEY_INPUT_ACTION_COUNT; ++i)
    {
        m_input_action_name[i].clear();
        m_input_action_value[i] = ms_input_action_key[i].m_default_value;
    }
}

void Config::Read (string const &config_file_path, bool reset_to_defaults_before_reading)
{
    std::cerr << "Config::Read(" << config_file_path << ");" << std::endl;
    
    if (reset_to_defaults_before_reading)
        ResetToDefaults();

    DataFile::Structure const *root = DataFile::ParseDataFileIntoStructure(config_file_path);
    // if the parse didn't work for whatever reason, don't change the values from the defaults
    // (this is how we get default values when the config file is nonexistent for example).
    if (root != NULL)
    {
        // read in and set the enumerated values
        for (Uint32 i = 0; i < KEY_BOOLEAN_COUNT; ++i)
            try { SetBoolean(static_cast<KeyBoolean>(i), root->PathElementBoolean(ms_boolean_key[i].m_data_file_path)); } catch (...) { }
        for (Uint32 i = 0; i < KEY_UINT32_COUNT; ++i)
            try { SetUint32(static_cast<KeyUint32>(i), root->PathElementUnsignedInteger(ms_uint32_key[i].m_data_file_path)); } catch (...) { }
        for (Uint32 i = 0; i < KEY_STRING_COUNT; ++i)
            try { SetString(static_cast<KeyString>(i), root->PathElementString(ms_string_key[i].m_data_file_path)); } catch (...) { }
        for (Uint32 i = 0; i < KEY_INPUT_ACTION_COUNT; ++i)
            try { SetInputAction(static_cast<KeyInputAction>(i), root->PathElementString(ms_input_action_key[i].m_data_file_path)); } catch (...) { }

        // validate config values
        if (GetUint32(GAME__DIFFICULTY_LEVEL) <= DL_LOWEST)
            SetUint32(GAME__DIFFICULTY_LEVEL, DL_LOWEST);
        if (GetUint32(GAME__DIFFICULTY_LEVEL) >= DL_HIGHEST)
            SetUint32(GAME__DIFFICULTY_LEVEL, DL_HIGHEST);
        if (GetUint32(VIDEO__RESOLUTION_X) == 0)
            SetUint32(VIDEO__RESOLUTION_X, ms_uint32_key[VIDEO__RESOLUTION_X].m_default_value);
        if (GetUint32(VIDEO__RESOLUTION_Y) == 0)
            SetUint32(VIDEO__RESOLUTION_Y, ms_uint32_key[VIDEO__RESOLUTION_Y].m_default_value);
        if (GetUint32(VIDEO__GLTEXTURE_ATLAS_SIZE_X) != 0 && !Math::IsAPowerOf2(GetUint32(VIDEO__GLTEXTURE_ATLAS_SIZE_X)))
            SetUint32(VIDEO__GLTEXTURE_ATLAS_SIZE_X, ms_uint32_key[VIDEO__GLTEXTURE_ATLAS_SIZE_X].m_default_value);
        if (GetUint32(VIDEO__GLTEXTURE_ATLAS_SIZE_Y) != 0 && !Math::IsAPowerOf2(GetUint32(VIDEO__GLTEXTURE_ATLAS_SIZE_Y)))
            SetUint32(VIDEO__GLTEXTURE_ATLAS_SIZE_Y, ms_uint32_key[VIDEO__GLTEXTURE_ATLAS_SIZE_Y].m_default_value);
    }
}

void Config::Write (string const &config_file_path) const
{
    std::cerr << "Config::Write(" << config_file_path << ");" << std::endl;
    
    std::string config_file_os_path;
    try {
        config_file_os_path = Singleton::FileSystem().OsPath(config_file_path, FileSystem::WRITABLE);
    } catch (Exception const &e) {
        std::cerr << "Config::Write(); " << e.what() << std::endl;
        return;
    }
    
    std::ofstream stream;
    stream.open(config_file_os_path.c_str());
    if (!stream.is_open())
    {
        std::cerr << "Config::Write(); could not open file \"" << config_file_path << "\" for writing" << std::endl;
        return;
    }

    DataFile::Structure *root = new DataFile::Structure();

    // write out the enumerated values
    for (Uint32 i = 0; i < KEY_BOOLEAN_COUNT; ++i)
        try { root->SetPathElementBoolean(ms_boolean_key[i].m_data_file_path, Boolean(static_cast<KeyBoolean>(i))); } catch (...) { ASSERT1(false && "this should never happen"); }
    for (Uint32 i = 0; i < KEY_UINT32_COUNT; ++i)
        try { root->SetPathElementUnsignedInteger(ms_uint32_key[i].m_data_file_path, GetUint32(static_cast<KeyUint32>(i))); } catch (...) { ASSERT1(false && "this should never happen"); }
    for (Uint32 i = 0; i < KEY_STRING_COUNT; ++i)
        try { root->SetPathElementString(ms_string_key[i].m_data_file_path, GetString(static_cast<KeyString>(i))); } catch (...) { ASSERT1(false && "this should never happen"); }
    for (Uint32 i = 0; i < KEY_INPUT_ACTION_COUNT; ++i)
        try { root->SetPathElementString(ms_input_action_key[i].m_data_file_path, Singleton::InputState().KeyName(InputAction(static_cast<KeyInputAction>(i)))); } catch (...) { ASSERT1(false && "this should never happen"); }

    IndentFormatter formatter(stream, "    ");
    root->Print(formatter);
    Delete(root);
    stream.close();
}

} // end of namespace Dis

