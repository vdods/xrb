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

namespace Xrb {
namespace Engine2 {

class ObjectLayer;
class Sprite;
class World;

} // end of namespace Engine2
} // end of namespace Xrb

namespace Dis {

class Asteroid;
class Ballistic;
class DamageExplosion;
class Demi;
class Devourment;
class EnemyShip;
class Entity;
class EMPExplosion;
class Explosion;
class Fireball;
class GaussGunTrail;
class Grenade;
class GrenadeLauncher;
class GuidedMissile;
class HealthTrigger;
class Interloper;
class Item;
class LaserBeam;
class LaserImpactEffect;
class LightningEffect;
class Missile;
class MissileLauncher;
class NoDamageExplosion;
class Powerup;
class ReticleEffect;
class Revulsion;
class Shade;
class ShieldEffect;
class Solitary;
class TractorBeam;

Asteroid *SpawnAsteroid (
    Engine2::ObjectLayer *object_layer,
    Time current_time,
    FloatVector2 const &translation,
    Float scale_factor,
    Float mass,
    FloatVector2 const &velocity,
    Float mineral_content,
    bool is_a_secondary_asteroid);

Ballistic *SpawnSmartBallistic (
    Engine2::ObjectLayer *object_layer,
    Time current_time,
    FloatVector2 const &translation,
    Float scale_factor,
    Float mass,
    FloatVector2 const &velocity,
    Float impact_damage,
    Time::Delta time_to_live,
    Time time_at_birth,
    Uint8 weapon_level,
    EntityReference<Entity> const &owner);

Ballistic *SpawnDumbBallistic (
    Engine2::ObjectLayer *object_layer,
    Time current_time,
    FloatVector2 const &translation,
    Float scale_factor,
    Float mass,
    FloatVector2 const &velocity,
    Float impact_damage,
    Time::Delta time_to_live,
    Time time_at_birth,
    Uint8 weapon_level,
    EntityReference<Entity> const &owner);

Grenade *SpawnGrenade (
    Engine2::ObjectLayer *object_layer,
    Time current_time,
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
    Engine2::ObjectLayer *object_layer,
    Time current_time,
    FloatVector2 const &translation,
    Float scale_factor,
    Float angle,
    FloatVector2 const &velocity,
    MissileLauncher *owner_missile_launcher,
    Time::Delta time_to_live,
    Time time_at_birth,
    Float damage_to_inflict,
    Float damage_radius,
    Float explosion_radius,
    Uint32 weapon_level,
    EntityReference<Entity> const &owner,
    Float health,
    bool is_enemy_missile);

GuidedMissile *SpawnGuidedMissile (
    Engine2::ObjectLayer *object_layer,
    Time current_time,
    FloatVector2 const &translation,
    Float scale_factor,
    Float angle,
    FloatVector2 const &velocity,
    Time::Delta time_to_live,
    Time time_at_birth,
    Float damage_to_inflict,
    Float damage_radius,
    Float explosion_radius,
    Uint32 weapon_level,
    EntityReference<Entity> const &owner,
    Float health,
    bool is_enemy_missile);

Powerup *SpawnPowerup (
    Engine2::ObjectLayer *object_layer,
    std::string const &asset_path,
    Time current_time,
    FloatVector2 const &translation,
    Float scale_factor,
    Float mass,
    FloatVector2 const &velocity,
    ItemType item_type);

Powerup *SpawnPowerup (
    Engine2::ObjectLayer *object_layer,
    std::string const &asset_path,
    Time current_time,
    FloatVector2 const &translation,
    Float scale_factor,
    Float mass,
    FloatVector2 const &velocity,
    Item *item);

// Explosion is pretty generic and is used for non-explosion type things
// such as the Shade teleportation sequence
Explosion *SpawnExplosion (
    Engine2::ObjectLayer *object_layer,
    std::string const &asset_path,
    Time current_time,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Float angle,
    Float initial_size,
    Float final_size,
    Time::Delta time_to_live,
    Time time_at_birth,
    EntityType entity_type,
    Engine2::Circle::CollisionType collision_type);

DamageExplosion *SpawnDamageExplosion (
    Engine2::ObjectLayer *object_layer,
    std::string const &asset_path,
    Time current_time,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Float damage_amount,
    Float damage_radius,
    Float initial_size,
    Float explosion_radius,
    Time::Delta time_to_live,
    Time time_at_birth,
    EntityReference<Entity> const &owner);

// NoDamageExplosion is used for exactly that, but also the ship death sequence
// and part of the Shade teleportation sequence.
NoDamageExplosion *SpawnNoDamageExplosion (
    Engine2::ObjectLayer *object_layer,
    std::string const &asset_path,
    Time current_time,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Float initial_size,
    Float final_size,
    Time::Delta time_to_live,
    Time time_at_birth);

EMPExplosion *SpawnEMPExplosion (
    Engine2::ObjectLayer *object_layer,
    Time current_time,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Float disable_time_factor,
    Float initial_size,
    Float final_size,
    Time::Delta time_to_live,
    Time time_at_birth,
    EntityReference<Entity> const &owner);

Fireball *SpawnFireball (
    Engine2::ObjectLayer *object_layer,
    Time current_time,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Float starting_damage,
    Float potential_damage,
    Float initial_size,
    Float final_size,
    Time::Delta time_to_live,
    Time time_at_birth,
    EntityReference<Entity> const &owner);

LaserBeam *SpawnLaserBeam (Engine2::ObjectLayer *object_layer);

LaserImpactEffect *SpawnLaserImpactEffect (Engine2::ObjectLayer *object_layer, Time current_time);

void SpawnSplashImpactEffect (
    Engine2::ObjectLayer *object_layer,
    std::string const &asset_path,
    Time current_time,
    FloatVector2 const &location,
    FloatVector2 const &direction,
    FloatVector2 const &base_velocity,
    Float seed_angle,
    Float seed_radius,
    Uint32 particle_count,
    Float particle_spread_angle,
    Time::Delta particle_time_to_live,
    Float particle_speed_proportion);

void SpawnGaussGunTrail (
    Engine2::ObjectLayer *object_layer,
    std::string const &asset_path,
    Time current_time,
    FloatVector2 const &trail_start,
    FloatVector2 trail_direction,
    FloatVector2 const &trail_velocity,
    Float segment_width,
    Float segment_length,
    Uint32 segment_count,
    Time::Delta time_to_live,
    Time time_at_birth);

TractorBeam *SpawnTractorBeam (Engine2::ObjectLayer *object_layer);

ShieldEffect *SpawnShieldEffect (Engine2::ObjectLayer *object_layer, Time current_time);

LightningEffect *SpawnLightningEffect (Engine2::ObjectLayer *object_layer, Time current_time);

ReticleEffect *SpawnReticleEffect (Engine2::ObjectLayer *object_layer, Color const &color_mask);

HealthTrigger *SpawnHealthTrigger (
    Engine2::ObjectLayer *object_layer,
    Time current_time,
    FloatVector2 const &translation,
    Float scale_factor,
    FloatVector2 const &velocity,
    Float health_delta_rate,
    Mortal::DamageType damage_type,
    EntityReference<Mortal> const &ignore_this_mortal,
    EntityReference<Entity> const &owner);

Solitary *SpawnSolitary (
    Engine2::ObjectLayer *object_layer,
    Time current_time,
    FloatVector2 const &translation,
    FloatVector2 const &velocity);

Interloper *SpawnInterloper (
    Engine2::ObjectLayer *object_layer,
    Time current_time,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 enemy_level);

Shade *SpawnShade (
    Engine2::ObjectLayer *object_layer,
    Time current_time,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 enemy_level);

Revulsion *SpawnRevulsion (
    Engine2::ObjectLayer *object_layer,
    Time current_time,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 enemy_level);

Devourment *SpawnDevourment (
    Engine2::ObjectLayer *object_layer,
    Time current_time,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 enemy_level);

Demi *SpawnDemi (
    Engine2::ObjectLayer *object_layer,
    Time current_time,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 enemy_level);

EnemyShip *SpawnEnemyShip (
    Engine2::ObjectLayer *object_layer,
    Time current_time,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    EntityType enemy_type,
    Uint8 enemy_level);

HealthTrigger *SpawnDevourmentMouthHealthTrigger (
    Engine2::ObjectLayer *object_layer,
    Time current_time,
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

