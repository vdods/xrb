// ///////////////////////////////////////////////////////////////////////////
// dis_config.cpp by Victor Dods, created 2006/08/04
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_config.h"

#include "xrb_datafileparser.h"
#include "xrb_datafilevalue.h"
#include "xrb_input.h"
#include "xrb_util.h"

using namespace std;
using namespace Xrb;

namespace Dis
{

std::string const Config::ms_input_action_name[IA_COUNT] =
{
    "In-Game Inventory Panel",  // IA_IN_GAME_INVENTORY_PANEL = 0,
    "Move Forward",             // IA_MOVE_FORWARD,
    "Move Left",                // IA_MOVE_LEFT,
    "Move Back",                // IA_MOVE_BACK,
    "Move Right",               // IA_MOVE_RIGHT,
    "Primary Fire",             // IA_PRIMARY_FIRE,
    "Secondary Fire",           // IA_SECONDARY_FIRE,
    "Engine Brake",             // IA_ENGINE_BRAKE,
    "Use Tractor",              // IA_USE_TRACTOR,
    "Equip Pea Shooter",        // IA_EQUIP_PEA_SHOOTER,
    "Equip Laser",              // IA_EQUIP_LASER,
    "Equip Flame Thrower",      // IA_EQUIP_FLAME_THROWER,
    "Equip Gauss Gun",          // IA_EQUIP_GAUSS_GUN,
    "Equip Grenade Launcher",   // IA_EQUIP_GRENADE_LAUNCHER,
    "Equip Missile Launcher",   // IA_EQUIP_MISSILE_LAUNCHER,
    "Equip EMP Core"            // IA_EQUIP_EMP_CORE,
};

Key::Code const Config::ms_input_action_default_key_code[IA_COUNT] =
{
    Key::ESCAPE,        // IA_IN_GAME_INVENTORY_PANEL = 0,
    Key::W,             // IA_MOVE_FORWARD,
    Key::A,             // IA_MOVE_LEFT,
    Key::S,             // IA_MOVE_BACK,
    Key::D,             // IA_MOVE_RIGHT,
    Key::LEFTMOUSE,     // IA_PRIMARY_FIRE,
    Key::RIGHTMOUSE,    // IA_SECONDARY_FIRE,
    Key::C,             // IA_ENGINE_BRAKE,
    Key::SPACE,         // IA_USE_TRACTOR,
    Key::ONE,           // IA_EQUIP_PEA_SHOOTER,
    Key::TWO,           // IA_EQUIP_LASER,
    Key::THREE,         // IA_EQUIP_FLAME_THROWER,
    Key::FOUR,          // IA_EQUIP_GAUSS_GUN,
    Key::FIVE,          // IA_EQUIP_GRENADE_LAUNCHER,
    Key::SIX,           // IA_EQUIP_MISSILE_LAUNCHER,
    Key::SEVEN          // IA_EQUIP_EMP_CORE,
};

std::string const Config::ms_input_action_path[IA_COUNT] =
{
    "|input|in_game_inventory_panel",        // IA_IN_GAME_INVENTORY_PANEL = 0,
    "|input|move_forward",                   // IA_MOVE_FORWARD,
    "|input|move_left",                      // IA_MOVE_LEFT,
    "|input|move_back",                      // IA_MOVE_BACK,
    "|input|move_right",                     // IA_MOVE_RIGHT,
    "|input|primary_fire",                   // IA_PRIMARY_FIRE,
    "|input|secondary_fire",                 // IA_SECONDARY_FIRE,
    "|input|engine_brake",                   // IA_ENGINE_BRAKE,
    "|input|use_tractor",                    // IA_USE_TRACTOR,
    "|input|equip_weapon1_pea_shooter",      // IA_EQUIP_PEA_SHOOTER,
    "|input|equip_weapon2_laser",            // IA_EQUIP_LASER,
    "|input|equip_weapon3_flame_thrower",    // IA_EQUIP_FLAME_THROWER,
    "|input|equip_weapon4_gauss_gun",        // IA_EQUIP_GAUSS_GUN,
    "|input|equip_weapon5_grenade_launcher", // IA_EQUIP_GRENADE_LAUNCHER,
    "|input|equip_weapon6_missile_launcher", // IA_EQUIP_MISSILE_LAUNCHER,
    "|input|equip_weapon7_emp_core"          // IA_EQUIP_EMP_CORE,
};

Config::Config ()
{
    ResetToDefaults();
}

Key::Code Config::GetInputActionKeyCode (InputAction const input_action) const
{
    ASSERT1(input_action < IA_COUNT)

    if (!m_input_action_key_name[input_action].empty())
    {
        m_input_action_key_code[input_action] = Singletons::Input().GetKeyCode(m_input_action_key_name[input_action]);
        m_input_action_key_name[input_action].clear();
    }

    if (m_input_action_key_code[input_action] == Key::INVALID)
        m_input_action_key_code[input_action] = ms_input_action_default_key_code[input_action];

    return m_input_action_key_code[input_action];
}

void Config::SetInputActionKeyCode (InputAction const input_action, Key::Code const key_code)
{
    ASSERT1(input_action < IA_COUNT)
    m_input_action_key_code[input_action] =
        Singletons::Input().GetIsValidKeyCode(key_code) ?
        key_code :
        Key::INVALID;
}

void Config::ResetToDefaults ()
{
    m_fullscreen = true;
    m_resolution = ScreenCoordVector2(1024, 768);
    m_key_map_name = "none";
    for (Uint32 i = 0; i < IA_COUNT; ++i)
    {
        m_input_action_key_name[i].clear();
        m_input_action_key_code[i] = Key::INVALID;
    }
}

void Config::Read (string const &config_filename, bool const reset_to_defaults_before_reading)
{
    if (reset_to_defaults_before_reading)
        ResetToDefaults();

    DataFileParser parser;
    // if the parse didn't work for whatever reason, don't change the values.
    if (parser.Parse(config_filename) == DataFileParser::RC_SUCCESS)
    {
        DataFileStructure const *root = parser.GetAcceptedStructure();

        // convenience macro.  ignore malformed or undefined config values.
        #define GET_CONFIG_VALUE(member, Type, path) \
            try { member = root->GetPathElement##Type(path); } \
            catch (...) { }
        GET_CONFIG_VALUE(m_fullscreen, Boolean, "|video|fullscreen");
        GET_CONFIG_VALUE(m_resolution[Dim::X], Uint32, "|video|resolution_x");
        GET_CONFIG_VALUE(m_resolution[Dim::Y], Uint32, "|video|resolution_y");
        GET_CONFIG_VALUE(m_key_map_name, String, "|key_map_name");
        #undef GET_CONFIG_VALUE

        for (Uint32 i = 0; i < IA_COUNT; ++i)
        {
            // ignore malformed or undefined config values
            try {
                m_input_action_key_name[i] = root->GetPathElementString(ms_input_action_path[i]);
                Util::MakeUppercase(&m_input_action_key_name[i]);
            } catch (...) { }
        }
    }
}

void Config::Write (string const &config_filename) const
{
    FILE *fptr = fopen(config_filename.c_str(), "wt");
    if (fptr == NULL)
        return;

    DataFileStructure *root = new DataFileStructure();

    root->SetPathElementBoolean("|video|fullscreen", m_fullscreen);
    root->SetPathElementUint32("|video|resolution_x", m_resolution[Dim::X]);
    root->SetPathElementUint32("|video|resolution_y", m_resolution[Dim::Y]);
    root->SetPathElementString("|key_map_name", m_key_map_name);

    for (Uint32 i = 0; i < IA_COUNT; ++i)
        root->SetPathElementString(
            ms_input_action_path[i],
            Singletons::Input().GetKeyName(GetInputActionKeyCode(static_cast<InputAction>(i))));

    IndentFormatter formatter(fptr, "    ");
    root->Print(formatter);
    Delete(root);
    fclose(fptr);
}

} // end of namespace Dis

