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

std::string const Config::ms_input_action_default_key_name[IA_COUNT] =
{
    "W",        // IA_MOVE_FORWARD = 0,
    "A",        // IA_MOVE_LEFT,
    "S",        // IA_MOVE_BACK,
    "D",        // IA_MOVE_RIGHT,
    "LMOUSE",   // IA_PRIMARY_FIRE,
    "RMOUSE",   // IA_SECONDARY_FIRE,
    "C",        // IA_ENGINE_BRAKE,
    "SPACE"     // IA_USE_TRACTOR,
};

std::string const Config::ms_input_action_path[IA_COUNT] =
{
    "|controls|move_forward",   // IA_MOVE_FORWARD = 0,
    "|controls|move_left",      // IA_MOVE_LEFT,
    "|controls|move_back",      // IA_MOVE_BACK,
    "|controls|move_right",     // IA_MOVE_RIGHT,
    "|controls|primary_fire",   // IA_PRIMARY_FIRE,
    "|controls|secondary_fire", // IA_SECONDARY_FIRE,
    "|controls|engine_brake",   // IA_ENGINE_BRAKE,
    "|controls|use_tractor"     // IA_USE_TRACTOR,
};

Config::Config ()
{
    ResetToDefaults();
}

Key::Code Config::GetKeyCode (InputAction const input_action) const
{
    ASSERT1(input_action < IA_COUNT)

    if (m_input_action_key_code[input_action] == Key::INVALID)
    {
        m_input_action_key_code[input_action] = Singletons::Input().GetKeyCode(m_input_action_key_name[input_action]);
        if (m_input_action_key_code[input_action] == Key::INVALID)
        {
            m_input_action_key_name[input_action] = ms_input_action_default_key_name[input_action];
            m_input_action_key_code[input_action] = Singletons::Input().GetKeyCode(m_input_action_key_name[input_action]);
        }
    }
    return m_input_action_key_code[input_action];
}

void Config::SetInputActionKeyName (InputAction const input_action, std::string const &key_name)
{
    ASSERT1(input_action < IA_COUNT)
    m_input_action_key_name[input_action] = key_name;
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
        GET_CONFIG_VALUE(m_fullscreen, Boolean, "|fullscreen");
        GET_CONFIG_VALUE(m_resolution[Dim::X], Uint32, "|resolution_x");
        GET_CONFIG_VALUE(m_resolution[Dim::Y], Uint32, "|resolution_y");
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

    root->SetPathElementBoolean("|fullscreen", m_fullscreen);
    root->SetPathElementUint32("|resolution_x", m_resolution[Dim::X]);
    root->SetPathElementUint32("|resolution_y", m_resolution[Dim::Y]);
    root->SetPathElementString("|key_map_name", m_key_map_name);

    for (Uint32 i = 0; i < IA_COUNT; ++i)
    {
        // this call is to ensure m_input_action_key_name[i] is updated
        GetKeyCode(static_cast<InputAction>(i));
        root->SetPathElementString(ms_input_action_path[i], m_input_action_key_name[i]);
    }

    IndentFormatter formatter(fptr, "    ");
    root->Print(formatter);
    Delete(root);
    fclose(fptr);
}

} // end of namespace Dis

