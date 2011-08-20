// ///////////////////////////////////////////////////////////////////////////
// dis_enums.hpp by Victor Dods, created 2006/01/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_ENUMS_HPP_)
#define _DIS_ENUMS_HPP_

#include "xrb.hpp"

#include <string>

using namespace Xrb;

namespace Dis {

// z-depth values for classes of sprites.
// lower z-depth values indicate closer to the screen

#define Z_DEPTH_EMP_EXPLOSION      -0.8f
#define Z_DEPTH_RETICLE_EFFECT     -0.2f
#define Z_DEPTH_LIGHTNING_EFFECT   -0.15f
#define Z_DEPTH_SPLASH_IMPACT      -0.1f
#define Z_DEPTH_SOLID               0.0f
#define Z_DEPTH_DEVOURMENT_GRINDER  0.05f
#define Z_DEPTH_BALLISTIC           0.075f
#define Z_DEPTH_SHIELD_EFFECT       0.1f
#define Z_DEPTH_FIREBALL            0.3f
#define Z_DEPTH_LASER_IMPACT_EFFECT 0.35f
#define Z_DEPTH_LASER_BEAM          0.4f
#define Z_DEPTH_GAUSS_GUN_TRAIL     0.45f
#define Z_DEPTH_DAMAGE_EXPLOSION    0.5f
#define Z_DEPTH_NO_DAMAGE_EXPLOSION 0.55f
#define Z_DEPTH_TRACTOR_BEAM        0.8f

enum
{
    MINERAL_COUNT = 4,
    UPGRADE_LEVEL_COUNT = 4
};

enum DifficultyLevel
{
    DL_EASY = 0,
    DL_MEDIUM,
    DL_HARD,
    DL_CRUEL,

    DL_COUNT,

    DL_LOWEST = DL_EASY,
    DL_HIGHEST = DL_CRUEL
};

std::string const &DifficultyLevelString (DifficultyLevel difficulty_level);

enum EntityType
{
    ET_ASTEROID = 0,
    ET_POWERUP,

    // all ships must be grouped contiguously
    ET_SOLITARY,
    ET_INTERLOPER,
    ET_SHADE,
    ET_REVULSION,
    ET_DEVOURMENT,
    ET_DEMI,

    // projectiles
    ET_GRENADE,
    ET_MISSILE,
    ET_GUIDED_MISSILE,
    ET_ENEMY_MISSILE,
    ET_GUIDED_ENEMY_MISSILE,
    ET_BALLISTIC,
    ET_FIREBALL,

    // explosions
    ET_EXPLOSION,
    ET_DAMAGE_EXPLOSION,
    ET_NO_DAMAGE_EXPLOSION,
    ET_EMP_EXPLOSION,

    // effects
    ET_LASER_BEAM,
    ET_LASER_IMPACT_EFFECT,
    ET_GAUSS_GUN_TRAIL,
    ET_TRACTOR_BEAM,
    ET_SHIELD_EFFECT,
    ET_LIGHTNING_EFFECT,
    ET_RETICLE_EFFECT,

    // invisible
    ET_HEALTH_TRIGGER,

    ET_COUNT,

    ET_SHIP_LOWEST = ET_SOLITARY,
    ET_SHIP_HIGHEST = ET_DEMI,
    ET_SHIP_COUNT = ET_SHIP_HIGHEST - ET_SHIP_LOWEST + 1,

    ET_ENEMY_SHIP_LOWEST = ET_INTERLOPER,
    ET_ENEMY_SHIP_HIGHEST = ET_DEMI,
    ET_ENEMY_SHIP_COUNT = ET_ENEMY_SHIP_HIGHEST - ET_ENEMY_SHIP_LOWEST + 1,

    ET_PROJECTILE_LOWEST = ET_GRENADE,
    ET_PROJECTILE_HIGHEST = ET_FIREBALL,
    ET_PROJECTILE_COUNT = ET_PROJECTILE_HIGHEST - ET_PROJECTILE_LOWEST + 1
}; // end of enum EntityType

enum ItemType
{
    IT_WEAPON_PEA_SHOOTER = 0,
    IT_WEAPON_LASER,
    IT_WEAPON_FLAME_THROWER,
    IT_WEAPON_GAUSS_GUN,
    IT_WEAPON_GRENADE_LAUNCHER,
    IT_WEAPON_MISSILE_LAUNCHER,
    IT_WEAPON_TRACTOR,
    IT_ENGINE,
    IT_ARMOR,
    IT_SHIELD,
    IT_POWER_GENERATOR,

    // this is above the minerals because you shouldn't actually make
    // an item subclass for each/any minerals
    IT_COUNT,

    IT_ENEMY_WEAPON_ADVANCED_TRACTOR,
    IT_ENEMY_WEAPON_SLOW_BULLET_GUN,
    IT_ENEMY_WEAPON_ENEMY_SPAWNER,

    IT_POWERUP_HEALTH,
    IT_POWERUP_OPTION,

    // these aren't items, but they need to be in here for Powerup to use
    IT_MINERAL_LOWEST,
    IT_MINERAL_HIGHEST = IT_MINERAL_LOWEST + MINERAL_COUNT - 1,

    IT_WEAPON_LOWEST = IT_WEAPON_PEA_SHOOTER,
    IT_WEAPON_HIGHEST = IT_WEAPON_TRACTOR,
    IT_WEAPON_COUNT = IT_WEAPON_HIGHEST - IT_WEAPON_LOWEST + 1,

    IT_ENEMY_WEAPON_LOWEST = IT_ENEMY_WEAPON_ADVANCED_TRACTOR,
    IT_ENEMY_WEAPON_HIGHEST = IT_ENEMY_WEAPON_ENEMY_SPAWNER,
    IT_ENEMY_WEAPON_COUNT = IT_ENEMY_WEAPON_HIGHEST - IT_ENEMY_WEAPON_LOWEST + 1,

    IT_POWERUP_LOWEST = IT_POWERUP_HEALTH,
    IT_POWERUP_HIGHEST = IT_POWERUP_OPTION,
    IT_POWERUP_COUNT = IT_POWERUP_HIGHEST - IT_POWERUP_LOWEST + 1
}; // end of enum ItemType

enum ExplosionType
{
    EXPLO_DENSE = 0,
    EXPLO_SPARSE,
    EXPLO_ROCK,
    EXPLO_EMP,
    EXPLO_FIREBALL,
    EXPLO_SHOCKWAVE,

    EXPLO_COUNT
}; // end of enum ExplosionType

std::string const &ExplosionAssetPath (ExplosionType explosion_type);

// ///////////////////////////////////////////////////////////////////////////
// Config key enums
// ///////////////////////////////////////////////////////////////////////////

enum KeyBoolean
{
    SYSTEM__DEBUG_MODE = 0,
    VIDEO__FULLSCREEN,

    KEY_BOOLEAN_COUNT
}; // end of enum KeyBoolean

enum KeyUint32
{
    GAME__DIFFICULTY_LEVEL = 0,
    VIDEO__RESOLUTION_X,
    VIDEO__RESOLUTION_Y,
    VIDEO__GLTEXTURE_ATLAS_SIZE_X,
    VIDEO__GLTEXTURE_ATLAS_SIZE_Y,

    KEY_UINT32_COUNT
}; // end of enum KeyUint32

enum KeyString
{
    SYSTEM__KEY_MAP_NAME = 0,

    KEY_STRING_COUNT
}; // end of enum KeyString

enum KeyInputAction
{
    INPUT__OPEN_INVENTORY = 0,
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
    INPUT__ACTIVATE_OPTION__EMP,
    INPUT__ACTIVATE_OPTION__ATTACK_BOOST,
    INPUT__ACTIVATE_OPTION__DEFENSE_BOOST,
    INPUT__ACTIVATE_OPTION__TIME_STRETCH,
    INPUT__ACTIVATE_OPTION__SELECTED,
    INPUT__SELECT_OPTION__NEXT,
    INPUT__SELECT_OPTION__PREVIOUS,

    KEY_INPUT_ACTION_COUNT,

    INPUT__ACTIVATE_OPTION__LOWEST = INPUT__ACTIVATE_OPTION__EMP,
    INPUT__ACTIVATE_OPTION__HIGHEST = INPUT__ACTIVATE_OPTION__SELECTED,
}; // end of enum KeyInputAction

} // end of namespace Dis

#endif // !defined(_DIS_ENUMS_HPP_)

