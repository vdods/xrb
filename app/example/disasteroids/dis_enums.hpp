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

namespace Dis
{

enum
{
    MINERAL_COUNT = 4,
    UPGRADE_LEVEL_COUNT = 4
};

/** For example, an intangible debris particle entity would likely use
  * CT_NO_COLLISION so that it does not effect other entities and
  * waste processing time.  A normal object such as a spaceship would have
  * CT_SOLID_COLLISION, and collide and bounce off of other entities
  * with CT_SOLID_COLLISION.  A fireball entity may use the value
  * CT_NONSOLID_COLLISION so that it still records collisions (which
  * in this case could damage the player's entity), but does not physically
  * bounce off of solid objects (the fireball would pass over the spaceship).
  * @brief Used by Engine2::Entity for indicating the collision properties
  *        of an entity subclass' geometry.
  */
enum CollisionType
{
    /// Indicates that the entity does not collide with others.
    CT_NO_COLLISION = 0,
    /// Indicates the entity records collisions and imparts collision forces.
    CT_SOLID_COLLISION,
    /// Indicates the entity records collisions without imparting collision forces.
    CT_NONSOLID_COLLISION,

    /// Number of collision types
    CT_COUNT
}; // end of enum CollisionType

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

std::string const &GetDifficultyLevelString (DifficultyLevel difficulty_level);

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

    ET_GRENADE,
    ET_MISSILE,
    ET_GUIDED_MISSILE,
    ET_ENEMY_MISSILE,
    ET_GUIDED_ENEMY_MISSILE,
    ET_EMP_BOMB,
    ET_BALLISTIC,
    ET_FIREBALL,

    ET_DAMAGE_EXPLOSION,
    ET_NO_DAMAGE_EXPLOSION,
    ET_EMP_EXPLOSION,

    ET_LASER_BEAM,
    ET_GAUSS_GUN_TRAIL,
    ET_TRACTOR_BEAM,
    ET_SHIELD_EFFECT,
    ET_RETICLE_EFFECT,

    ET_HEALTH_TRIGGER,

    ET_COUNT,

    ET_SHIP_LOWEST = ET_SOLITARY,
    ET_SHIP_HIGHEST = ET_DEMI,
    ET_SHIP_COUNT = ET_SHIP_HIGHEST - ET_SHIP_LOWEST + 1,

    ET_ENEMY_SHIP_LOWEST = ET_INTERLOPER,
    ET_ENEMY_SHIP_HIGHEST = ET_DEMI,
    ET_ENEMY_SHIP_COUNT = ET_ENEMY_SHIP_HIGHEST - ET_ENEMY_SHIP_LOWEST + 1
}; // end of enum EntityType

enum ItemType
{
    IT_WEAPON_PEA_SHOOTER = 0,
    IT_WEAPON_LASER,
    IT_WEAPON_FLAME_THROWER,
    IT_WEAPON_GAUSS_GUN,
    IT_WEAPON_GRENADE_LAUNCHER,
    IT_WEAPON_MISSILE_LAUNCHER,
    IT_WEAPON_EMP_CORE,
//     IT_WEAPON_EMP_BOMB_LAYER,
    IT_WEAPON_TRACTOR,
    IT_ENGINE,
    IT_ARMOR,
    IT_SHIELD,
    IT_POWER_GENERATOR,

    // this is above the minerals because you shouldn't actually make
    // an item subclass for each/any minerals
    IT_COUNT,

    IT_ENEMY_WEAPON_SLOW_BULLET_GUN,
    IT_ENEMY_WEAPON_ENEMY_SPAWNER,

    // these aren't items, but they need to be in here for Powerup to use
    IT_MINERAL_LOWEST,
    IT_MINERAL_HIGHEST = IT_MINERAL_LOWEST + MINERAL_COUNT - 1,

    IT_POWERUP_HEALTH,

    IT_POWERUP_LIMIT,

    IT_WEAPON_LOWEST = IT_WEAPON_PEA_SHOOTER,
    IT_WEAPON_HIGHEST = IT_WEAPON_TRACTOR,
    IT_WEAPON_COUNT = IT_WEAPON_HIGHEST - IT_WEAPON_LOWEST + 1,

    IT_ENEMY_WEAPON_LOWEST = IT_ENEMY_WEAPON_SLOW_BULLET_GUN,
    IT_ENEMY_WEAPON_HIGHEST = IT_ENEMY_WEAPON_ENEMY_SPAWNER,
    IT_ENEMY_WEAPON_COUNT = IT_ENEMY_WEAPON_HIGHEST - IT_ENEMY_WEAPON_LOWEST + 1,
}; // end of enum ItemType

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
    GAME__DIFFICULTY_LEVEL = 0,
    VIDEO__RESOLUTION_X,
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

    KEY_INPUT_ACTION_COUNT
}; // end of enum KeyInputAction

} // end of namespace Dis

#endif // !defined(_DIS_ENUMS_HPP_)

