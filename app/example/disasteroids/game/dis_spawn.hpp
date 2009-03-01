// ///////////////////////////////////////////////////////////////////////////
// dis_spawn.hpp by Victor Dods, created 2005/11/10
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_SPAWN_HPP_)
#define _DIS_SPAWN_HPP_

#include <string>

#include "dis_mortal.hpp"
#include "xrb_color.hpp"
#include "xrb_vector.hpp"

using namespace Xrb;

namespace Xrb
{
namespace Engine2
{
    class ObjectLayer;
    class Sprite;
    class World;
} // end of namespace Engine2
} // end of namespace Xrb

namespace Dis
{

class Asteroid;
class Ballistic;
class DamageExplosion;
class Demi;
class Devourment;
// class EMPBomb;
// class EMPBombLayer;
class EnemyShip;
class Entity;
class EMPExplosion;
class Fireball;
class GaussGunTrail;
class Grenade;
class GrenadeLauncher;
class GuidedMissile;
class HealthTrigger;
class Interloper;
class Item;
class LaserBeam;
class Missile;
class NoDamageExplosion;
class Powerup;
class ReticleEffect;
class Revulsion;
class Shade;
class ShieldEffect;
class Solitary;
class TractorBeam;

Asteroid *SpawnAsteroid (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    Float scale_factor,
    Float first_moment,
    FloatVector2 const &velocity,
    Float mineral_content,
    bool is_a_secondary_asteroid);

Ballistic *SpawnSmartBallistic (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    Float scale_factor,
    Float first_moment,
    FloatVector2 const &velocity,
    Float impact_damage,
    Float time_to_live,
    Float time_at_birth,
    Uint8 weapon_level,
    EntityReference<Entity> const &owner);

Ballistic *SpawnDumbBallistic (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    Float scale_factor,
    Float first_moment,
    FloatVector2 const &velocity,
    Float impact_damage,
    Float time_to_live,
    Float time_at_birth,
    Uint8 weapon_level,
    EntityReference<Entity> const &owner);

Grenade *SpawnGrenade (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    Float scale_factor,
    FloatVector2 const &velocity,
    GrenadeLauncher *owner_grenade_launcher,
    Float damage_to_inflict,
    Float damage_radius,
    Float explosion_radius,
    Uint32 weapon_level,
    EntityReference<Entity> const &owner,
    Float health);

Missile *SpawnMissile (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    Float scale_factor,
    Float angle,
    FloatVector2 const &velocity,
    Float time_to_live,
    Float time_at_birth,
    Float damage_to_inflict,
    Float damage_radius,
    Float explosion_radius,
    Uint32 weapon_level,
    EntityReference<Entity> const &owner,
    Float health,
    bool is_enemy_missile);

GuidedMissile *SpawnGuidedMissile (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    Float scale_factor,
    Float angle,
    FloatVector2 const &velocity,
    Float time_to_live,
    Float time_at_birth,
    Float damage_to_inflict,
    Float damage_radius,
    Float explosion_radius,
    Uint32 weapon_level,
    EntityReference<Entity> const &owner,
    Float health,
    bool is_enemy_missile);

/*
EMPBomb *SpawnEMPBomb (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    Float scale_factor,
    FloatVector2 const &velocity,
    EMPBombLayer *owner_emp_bomb_layer,
    Float disable_time_factor,
    Float blast_radius,
    Uint32 weapon_level,
    EntityReference<Entity> const &owner,
    Float health);
*/
Powerup *SpawnPowerup (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    Float scale_factor,
    Float first_moment,
    FloatVector2 const &velocity,
    std::string const &sprite_texture_filename,
    ItemType item_type);

Powerup *SpawnPowerup (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    Float scale_factor,
    Float first_moment,
    FloatVector2 const &velocity,
    std::string const &sprite_texture_filename,
    Item *item);

DamageExplosion *SpawnDamageExplosion (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Float damage_amount,
    Float damage_radius,
    Float explosion_radius,
    Float time_to_live,
    Float time_at_birth,
    EntityReference<Entity> const &owner);

NoDamageExplosion *SpawnNoDamageExplosion (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Float final_size,
    Float time_to_live,
    Float time_at_birth);

EMPExplosion *SpawnEMPExplosion (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Float disable_time_factor,
    Float final_size,
    Float time_to_live,
    Float time_at_birth,
    EntityReference<Entity> const &owner);

Fireball *SpawnFireball (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Float starting_damage,
    Float potential_damage,
    Float final_size,
    Float time_to_live,
    Float time_at_birth,
    EntityReference<Entity> const &owner);

LaserBeam *SpawnLaserBeam (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer);

GaussGunTrail *SpawnGaussGunTrail (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &trail_start,
    FloatVector2 const &trail_vector,
    FloatVector2 const &trail_velocity,
    Float trail_width,
    Float time_to_live,
    Float time_at_birth);

TractorBeam *SpawnTractorBeam (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer);

ShieldEffect *SpawnShieldEffect (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer);

ReticleEffect *SpawnReticleEffect (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    Color const &color_mask);

HealthTrigger *SpawnHealthTrigger (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    Float scale_factor,
    FloatVector2 const &velocity,
    Float health_delta_rate,
    Mortal::DamageType damage_type,
    EntityReference<Mortal> const &ignore_this_mortal,
    EntityReference<Entity> const &owner);

Solitary *SpawnSolitary (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity);

Interloper *SpawnInterloper (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 enemy_level);

Shade *SpawnShade (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 enemy_level);

Revulsion *SpawnRevulsion (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 enemy_level);

Devourment *SpawnDevourment (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 enemy_level);

Demi *SpawnDemi (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 enemy_level);

EnemyShip *SpawnEnemyShip (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    EntityType enemy_type,
    Uint8 enemy_level);

HealthTrigger *SpawnDevourmentMouthHealthTrigger (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    Float scale_factor,
    FloatVector2 const &velocity,
    Float health_delta_rate,
    Mortal::DamageType damage_type,
    EntityReference<Mortal> const &ignore_this_mortal,
    EntityReference<Entity> const &owner,
    Uint8 enemy_level);

} // end of namespace Dis

#endif // !defined(_DIS_SPAWN_HPP_)

