// ///////////////////////////////////////////////////////////////////////////
// dis_item.cpp by Victor Dods, created 2005/12/30
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_item.h"

#include "dis_armor.h"
#include "dis_engine.h"
#include "dis_enums.h"
#include "dis_powergenerator.h"
#include "dis_shield.h"
#include "dis_weapon.h"

using namespace Xrb;

namespace Dis
{

Item *Item::Create (ItemType const type, Uint8 const upgrade_level)
{
    ASSERT1(type < IT_COUNT)
    ASSERT1(upgrade_level < UPGRADE_LEVEL_COUNT)

    switch (type)
    {
        case IT_WEAPON_PEA_SHOOTER:           return new PeaShooter(upgrade_level);
        case IT_WEAPON_LASER:                 return new Laser(upgrade_level);
        case IT_WEAPON_FLAME_THROWER:         return new FlameThrower(upgrade_level);
        case IT_WEAPON_GAUSS_GUN:             return new GaussGun(upgrade_level);
        case IT_WEAPON_GRENADE_LAUNCHER:      return new GrenadeLauncher(upgrade_level);
        case IT_WEAPON_MINE_LAYER:            return new MineLayer(upgrade_level);
        case IT_WEAPON_MISSILE_LAUNCHER:      return new MissileLauncher(upgrade_level);
        case IT_WEAPON_EMP_CORE:              return new EMPCore(upgrade_level);
//         case IT_WEAPON_EMP_BOMB_LAYER:        return new EMPBombLayer(upgrade_level);
        case IT_WEAPON_AUTO_DESTRUCT:         return new AutoDestruct(upgrade_level);
        case IT_WEAPON_TRACTOR:               return new Tractor(upgrade_level);
        case IT_ENEMY_WEAPON_SLOW_BULLET_GUN: return new SlowBulletGun(upgrade_level);
        case IT_ENGINE:                       return new Engine(upgrade_level);
        case IT_ARMOR:                        return new Armor(upgrade_level);
        case IT_SHIELD:                       return new Shield(upgrade_level);
        case IT_POWER_GENERATOR:              return new PowerGenerator(upgrade_level);
            
        default:
            ASSERT1(false && "Invalid ItemType")
            return NULL;
    }
}

std::string const &Item::GetMineralSpriteFilename (Uint8 const mineral_index)
{
    static std::string const s_mineral_sprite_filename[MINERAL_COUNT] =
    {
        "resources/mineral_0.png",
        "resources/mineral_1.png",
        "resources/mineral_2.png",
        "resources/mineral_3.png",
    };

    ASSERT1(mineral_index < MINERAL_COUNT)
    return s_mineral_sprite_filename[mineral_index];
}

Uint32 Item::GetItemPrice (
    ItemType const item_type,
    Uint8 const upgrade_level,
    Uint8 const mineral_index)
{
    static Uint32 const s_item_price[IT_COUNT][UPGRADE_LEVEL_COUNT][MINERAL_COUNT] =
    {
        { // IT_WEAPON_PEA_SHOOTER
            {   0,   0,   0,   0}, // player always has this
            {   7,   0,   0,   0},
            {  95,   0,   0,   0},
            {  78,  40,   0,   0},
        },
        { // IT_WEAPON_LASER
            {  22,   0,   0,   0},
            {  73,   0,   0,   0},
            {  92,  75,   0,   0},
            { 167, 125, 104,  99},
        },
        { // IT_WEAPON_FLAME_THROWER
            {  12,   0,   0,   0},
            {  32,  34,   0,   0},
            {  46,  55,  48,   0},
            { 100, 106, 110,  18},
        },
        { // IT_WEAPON_GAUSS_GUN
            {  29,  23,   0,   0},
            {  61,  63,   0,   0},
            { 118, 120,  84,   0},
            { 155, 143, 145,  60},
        },
        { // IT_WEAPON_GRENADE_LAUNCHER
            {  37,   0,   0,   0},
            {  38,  35,   0,   0},
            {  59,  56,  48,   0},
            {  58,  59,  61,  60},
        },
        { // IT_WEAPON_MINE_LAYER
            {  32,  36,   0,   0},
            {  41,  45,  44,   0},
            {  66,  70,  62,  65},
            {  82,  75,  79,  81},
        },
        { // IT_WEAPON_MISSILE_LAUNCHER
            {  58,   0,   0,   0},
            {  57,  59,   0,   0},
            {  64,  67,  68,   0},
            {  83,  76,  80,  81},
        },
        { // IT_WEAPON_EMP_CORE // IT_WEAPON_EMP_BOMB_LAYER
            {  41,   0,   0,   0},
            {  47,  49,   0,   0},
            {  63,  65,  71,   0},
            {  89,  97,  98, 101},
        },
        { // IT_WEAPON_AUTO_DESTRUCT
            { 103,   0,   0,   0},
            {   0, 195,   0,   0},
            {   0,   0, 310,   0},
            {   0,   0,   0, 451},
        },
        { // IT_WEAPON_TRACTOR
            {  10,   0,   0,   0},
            {  22,  18,   0,   0},
            {  47,  39,  37,   0},
            {  83,  67,  42,  28},
        },
        { // IT_ENGINE
            {   0,   0,   0,   0}, // player always has this
            {  18,  27,   0,   0},
            {  48,  79,  67,   0},
            { 142, 123,  98,  43},
        },
        { // IT_ARMOR
            {  55,   0,   0,   0},
            {  79,  87,   0,   0},
            { 121,  90,  97,   0},
            { 167, 188, 132,  49},
        },
        { // IT_SHIELD
            {  29,   0,   0,   0},
            {  45,  43,   0,   0},
            {  61,  58,  65,   0},
            {  90, 111,  89,  78},
        },
        { // IT_POWER_GENERATOR
            {   0,   0,   0,   0}, // player always has this
            {  20,  22,   0,   0},
            {  44,  51,  65,   0},
            {  32, 121, 234,  67},
        }
    };

    ASSERT1(item_type < IT_COUNT)
    ASSERT1(upgrade_level < UPGRADE_LEVEL_COUNT)
    ASSERT1(mineral_index < MINERAL_COUNT)

    return s_item_price[item_type][upgrade_level][mineral_index];
}

} // end of namespace Dis
