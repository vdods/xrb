// ///////////////////////////////////////////////////////////////////////////
// dis_enums.h by Victor Dods, created 2006/01/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_ENUMS_H_)
#define _DIS_ENUMS_H_

#include "xrb.h"

using namespace Xrb;

namespace Dis
{

enum
{
    MINERAL_COUNT = 4,
    UPGRADE_LEVEL_COUNT = 4
};

enum ItemType
{
    IT_WEAPON_PEA_SHOOTER = 0,
    IT_WEAPON_LASER,
    IT_WEAPON_FLAME_THROWER,
    IT_WEAPON_GAUSS_GUN,
    IT_WEAPON_GRENADE_LAUNCHER,
    IT_WEAPON_MINE_LAYER,
    IT_WEAPON_MISSILE_LAUNCHER,
    IT_WEAPON_EMP_CORE,
//     IT_WEAPON_EMP_BOMB_LAYER,
    IT_WEAPON_AUTO_DESTRUCT,
    IT_WEAPON_TRACTOR,
    IT_ENGINE,
    IT_ARMOR,
    IT_SHIELD,
    IT_POWER_GENERATOR,

    // this is above the minerals because you shouldn't actually make
    // an item subclass for each/any minerals
    IT_COUNT,

    IT_ENEMY_WEAPON_SLOW_BULLET_GUN,

    // these aren't items, but they need to be in here for Powerup to use
    IT_MINERAL_LOWEST,
    IT_MINERAL_HIGHEST = IT_MINERAL_LOWEST + MINERAL_COUNT - 1,
    
    IT_WEAPON_LOWEST = IT_WEAPON_PEA_SHOOTER,
    IT_WEAPON_HIGHEST = IT_WEAPON_TRACTOR,
    IT_WEAPON_COUNT = IT_WEAPON_HIGHEST - IT_WEAPON_LOWEST + 1,

    IT_ENEMY_WEAPON_LOWEST = IT_ENEMY_WEAPON_SLOW_BULLET_GUN,
    IT_ENEMY_WEAPON_HIGHEST = IT_ENEMY_WEAPON_SLOW_BULLET_GUN,
    IT_ENEMY_WEAPON_COUNT = IT_ENEMY_WEAPON_HIGHEST - IT_ENEMY_WEAPON_LOWEST + 1,
}; // end of enum ItemType

} // end of namespace Dis

#endif // !defined(_DIS_ENUMS_H_)

