// ///////////////////////////////////////////////////////////////////////////
// dis_item.cpp by Victor Dods, created 2005/12/30
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_item.hpp"

#include "dis_armor.hpp"
#include "dis_engine.hpp"
#include "dis_enums.hpp"
#include "dis_powergenerator.hpp"
#include "dis_shield.hpp"
#include "dis_weapon.hpp"

using namespace Xrb;

namespace Dis
{

Item *Item::Create (ItemType const item_type, Uint8 const upgrade_level)
{
    ASSERT1(item_type < IT_COUNT);
    ASSERT1(upgrade_level < UPGRADE_LEVEL_COUNT);

    switch (item_type)
    {
        case IT_WEAPON_PEA_SHOOTER:           return new PeaShooter(upgrade_level);
        case IT_WEAPON_LASER:                 return new Laser(upgrade_level);
        case IT_WEAPON_FLAME_THROWER:         return new FlameThrower(upgrade_level);
        case IT_WEAPON_GAUSS_GUN:             return new GaussGun(upgrade_level);
        case IT_WEAPON_GRENADE_LAUNCHER:      return new GrenadeLauncher(upgrade_level);
        case IT_WEAPON_MISSILE_LAUNCHER:      return new MissileLauncher(upgrade_level);
        case IT_WEAPON_EMP_CORE:              return new EMPCore(upgrade_level);
//         case IT_WEAPON_EMP_BOMB_LAYER:        return new EMPBombLayer(upgrade_level);
        case IT_WEAPON_TRACTOR:               return new Tractor(upgrade_level);
        case IT_ENEMY_WEAPON_SLOW_BULLET_GUN: return new SlowBulletGun(upgrade_level);
        case IT_ENGINE:                       return new Engine(upgrade_level);
        case IT_ARMOR:                        return new Armor(upgrade_level);
        case IT_SHIELD:                       return new Shield(upgrade_level);
        case IT_POWER_GENERATOR:              return new PowerGenerator(upgrade_level);

        default:
            ASSERT1(false && "Invalid ItemType");
            return NULL;
    }
}

std::string const &Item::MineralSpriteFilename (Uint8 const mineral_index)
{
    static std::string const s_mineral_sprite_filename[MINERAL_COUNT] =
    {
        "resources/mineral_0.png",
        "resources/mineral_1.png",
        "resources/mineral_2.png",
        "resources/mineral_3.png",
    };

    ASSERT1(mineral_index < MINERAL_COUNT);
    return s_mineral_sprite_filename[mineral_index];
}

Uint32 Item::ItemPrice (
    ItemType const item_type,
    Uint8 const upgrade_level,
    Uint8 const mineral_index)
{
    static Uint32 const s_item_price[IT_COUNT][UPGRADE_LEVEL_COUNT][MINERAL_COUNT] =
    {
        { // IT_WEAPON_PEA_SHOOTER
            {   0,   0,   0,   0}, // player always has this
            {   7,   0,   0,   0},
            {  75,  83, 134,   0},
            { 118, 145, 152, 199},
        },
        { // IT_WEAPON_LASER
            {  22,   0,   0,   0},
            { 133,  50,   0,   0},
            { 192,  95,  59,  80},
            { 209, 175, 124, 143},
        },
        { // IT_WEAPON_FLAME_THROWER
            {  12,   0,   0,   0},
            {  52,  34,   0,   0},
            { 116,  85,  68,   0},
            { 110, 136, 141,  68},
        },
        { // IT_WEAPON_GAUSS_GUN
            {  29,   0,   0,   0},
            {  61,  73,   0,   0},
            { 138, 140, 124,   0},
            { 145, 193, 223, 168},
        },
        { // IT_WEAPON_GRENADE_LAUNCHER
            {  37,   0,   0,   0},
            {  58,  55,   0,   0},
            {  89, 106,  58,   0},
            { 148, 189, 161,  60},
        },
        { // IT_WEAPON_MISSILE_LAUNCHER
            { 217,   0,   0,   0},
            { 184,  89,   0,   0},
            { 163,  91,  68,   0},
            { 143, 136, 120,  81},
        },
        { // IT_WEAPON_EMP_CORE // IT_WEAPON_EMP_BOMB_LAYER
            {  31,   0,   0,   0},
            {  67,  59,   0,   0},
            {  83,  85,  71,   0},
            { 129,  97,  88, 101},
        },
        { // IT_WEAPON_TRACTOR
            {  10,   0,   0,   0},
            {  22,  18,   0,   0},
            {  47,  59,  47,   0},
            {  93,  77,  52,  48},
        },
        { // IT_ENGINE
            {   0,   0,   0,   0}, // player always has this
            {  18,  27,   0,   0},
            {  48,  79,  67,   0},
            { 142, 123,  98,  43},
        },
        { // IT_ARMOR
            {   0,   0,   0,   0}, // player always has this
            {  45,  42,   0,   0},
            { 101,  90,  87,   0},
            { 137, 188, 132,  49},
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
            { 122, 141, 234,  67},
        }
    };

    ASSERT1(item_type < IT_COUNT);
    ASSERT1(upgrade_level < UPGRADE_LEVEL_COUNT);
    ASSERT1(mineral_index < MINERAL_COUNT);

    return s_item_price[item_type][upgrade_level][mineral_index];
}

} // end of namespace Dis
