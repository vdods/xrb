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
#include "xrb_util.h"

using namespace std;
using namespace Xrb;

namespace Dis
{

Config::Config ()
{
    ResetToDefaults();
}

void Config::ResetToDefaults ()
{
    m_fullscreen = true;
    m_resolution = ScreenCoordVector2(1024, 768);
    m_key_map_name = "none";
    m_key_move_right = Key::D;
    m_key_move_left = Key::A;
    m_key_move_forward = Key::W;
    m_key_move_back = Key::S;
    m_key_engine_brake = Key::C;
    m_key_use_tractor = Key::SPACE;
}

void Config::Read (string const &config_filename)
{
    DataFileParser parser;
    // if the parse didn't work for whatever reason, don't change the values.
    if (parser.Parse(config_filename) == DataFileParser::RC_SUCCESS)
    {
        DataFileStructure const *root = parser.GetAcceptedStructure();

        // convenience macro.  ignore malformed or undefined config values.
        #define GET_CONFIG_VALUE(member, Type, path) \
            try { member = root->GetPathElement##Type(path); } catch (...) { }
        GET_CONFIG_VALUE(m_fullscreen, Boolean, "|fullscreen");
        GET_CONFIG_VALUE(m_resolution[Dim::X], Uint32, "|resolution_x");
        GET_CONFIG_VALUE(m_resolution[Dim::Y], Uint32, "|resolution_y");
        GET_CONFIG_VALUE(m_key_map_name, String, "|key_map_name");
        #undef GET_CONFIG_VALUE

        // convenience macro.  ignore malformed or undefined config and ascii values.
        #define GET_KEYBIND_VALUE(member, path) \
            try \
            { \
                char c = Util::Lowercase(root->GetPathElementCharacter(path)); \
                if (Key::GetIsKeyAscii(static_cast<Key::Code>(c))) \
                    member = static_cast<Key::Code>(c); \
            } \
            catch (...) { }
        GET_KEYBIND_VALUE(m_key_move_right, "|controls|move_right");
        GET_KEYBIND_VALUE(m_key_move_left, "|controls|move_left");
        GET_KEYBIND_VALUE(m_key_move_forward, "|controls|move_forward");
        GET_KEYBIND_VALUE(m_key_move_back, "|controls|move_back");
        GET_KEYBIND_VALUE(m_key_engine_brake, "|controls|engine_brake");
        GET_KEYBIND_VALUE(m_key_use_tractor, "|controls|use_tractor");
        #undef GET_KEYBIND_VALUE
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

    root->SetPathElementCharacter("|controls|move_right", m_key_move_right);
    root->SetPathElementCharacter("|controls|move_left", m_key_move_left);
    root->SetPathElementCharacter("|controls|move_forward", m_key_move_forward);
    root->SetPathElementCharacter("|controls|move_back", m_key_move_back);
    root->SetPathElementCharacter("|controls|engine_brake", m_key_engine_brake);
    root->SetPathElementCharacter("|controls|use_tractor", m_key_use_tractor);

    IndentFormatter formatter(fptr, "    ");
    root->Print(formatter);
    Delete(root);
    fclose(fptr);
}

} // end of namespace Dis

