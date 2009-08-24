// ///////////////////////////////////////////////////////////////////////////
// bm_enums.hpp by Victor Dods, created 2009/08/23
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_BM_ENUMS_HPP_)
#define _BM_ENUMS_HPP_

#include "xrb.hpp"

#include <string>

using namespace Xrb;

namespace Bm
{

// ///////////////////////////////////////////////////////////////////////////
// Config key enums
// ///////////////////////////////////////////////////////////////////////////

enum KeyBoolean
{
    VIDEO__FULLSCREEN = 0,

    KEY_BOOLEAN_COUNT
}; // end of enum KeyBoolean

enum KeyUint32
{
    VIDEO__RESOLUTION_X = 0,
    VIDEO__RESOLUTION_Y,

    KEY_UINT32_COUNT
}; // end of enum KeyUint32

enum KeyString
{
    SYSTEM__KEY_MAP_NAME = 0,

    KEY_STRING_COUNT
}; // end of enum KeyString

enum KeyInputAction
{
/*
    INPUT__IN_GAME_INVENTORY_PANEL = 0,
    INPUT__MOVE_FORWARD,
    INPUT__MOVE_LEFT,
    INPUT__MOVE_BACK,
    INPUT__MOVE_RIGHT,
    INPUT__PRIMARY_FIRE,
    INPUT__SECONDARY_FIRE,
    INPUT__ENGINE_BRAKE,
    INPUT__USE_TRACTOR,
    INPUT__EQUIP_PEA_SHOOTER,
    INPUT__EQUIP_LASER,
    INPUT__EQUIP_FLAME_THROWER,
    INPUT__EQUIP_GAUSS_GUN,
    INPUT__EQUIP_GRENADE_LAUNCHER,
    INPUT__EQUIP_MISSILE_LAUNCHER,
    INPUT__EQUIP_EMP_CORE,
*/
    KEY_INPUT_ACTION_COUNT = 0
}; // end of enum KeyInputAction

} // end of namespace Bm

#endif // !defined(_BM_ENUMS_HPP_)

