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
    ET_MINE,
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

