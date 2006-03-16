// ///////////////////////////////////////////////////////////////////////////
// dis_spawn.h by Victor Dods, created 2005/11/10
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_SPAWN_H_)
#define _DIS_SPAWN_H_

#include <string>

#include "dis_gameobject.h"
#include "dis_mortal.h"
#include "xrb_engine2_enums.h"
#include "xrb_vector.h"

using namespace Xrb;

namespace Xrb
{
namespace Engine2
{
    class EntityGuts;
    class ObjectLayer;
    class SpriteEntity;
    class World;
} // end of namespace Engine2
} // end of namespace Xrb

namespace Dis
{

class Asteroid;
class Ballistic;
class DamageExplosion;
class Devourment;
class EMPBomb;
class EMPBombLayer;
class EMPExplosion;
class Fireball;
class GaussGunTrail;
class Grenade;
class GrenadeLauncher;
class HealthTrigger;
class Interloper;
class LaserBeam;
class Mine;
class MineLayer;
class Missile;
class NoDamageExplosion;
class Powerup;
class ReticleEffect;
class Revulsion;
class Shade;
class ShieldEffect;
class Solitary;
class TractorBeam;

Engine2::SpriteEntity *SpawnSpriteEntity (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    std::string const &sprite_texture_filename,
    Engine2::EntityGuts *entity_guts,
    FloatVector2 const &translation,
    Float scale_factor,
    Float angle,
    Float first_moment,
    FloatVector2 const &velocity,
    Float second_moment,
    Float angular_velocity,
    Float elasticity,
    Engine2::CollisionType collision_type);

Asteroid *SpawnAsteroid (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    Float scale_factor,
    Float first_moment,
    FloatVector2 const &velocity,
    Float mineral_content);
    
Ballistic *SpawnBallistic (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    Float scale_factor,
    FloatVector2 const &velocity,
    Float impact_damage,
    Float time_to_live,
    GameObjectReference<GameObject> const &owner);
    
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
    GameObjectReference<GameObject> const &owner,
    Float health);
    
Mine *SpawnMine (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    Float scale_factor,
    FloatVector2 const &velocity,
    MineLayer *owner_mine_layer,
    Float damage_to_inflict,
    Float damage_radius,
    Float explosion_radius,
    Uint32 weapon_level,
    GameObjectReference<GameObject> const &owner,
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
    Float power,
    Uint32 weapon_level,
    GameObjectReference<GameObject> const &owner,
    Float health);

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
    GameObjectReference<GameObject> const &owner,
    Float health);
    
Powerup *SpawnMineral (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    Float scale_factor,
    Float first_moment,
    FloatVector2 const &velocity,
    Uint8 mineral_type);
    
DamageExplosion *SpawnDamageExplosion (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Float power,
    Float final_size,
    Float time_to_live,
    Float time_at_birth,
    GameObjectReference<GameObject> const &owner);
    
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
    GameObjectReference<GameObject> const &owner);
    
Fireball *SpawnFireball (
    Engine2::World *world,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Float power,
    Float final_size,
    Float time_to_live,
    Float time_at_birth,
    GameObjectReference<GameObject> const &owner);

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
    GameObjectReference<Mortal> const &ignore_this_mortal,
    GameObjectReference<GameObject> const &owner);
    
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
    
} // end of namespace Dis

#endif // !defined(_DIS_SPAWN_H_)

