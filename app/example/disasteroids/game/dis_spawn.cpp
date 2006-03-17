// ///////////////////////////////////////////////////////////////////////////
// dis_spawn.cpp by Victor Dods, created 2005/11/10
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_spawn.h"

#include "dis_asteroid.h"
#include "dis_ballistic.h"
#include "dis_devourment.h"
#include "dis_effect.h"
#include "dis_explosive.h"
#include "dis_gameobject.h"
#include "dis_healthtrigger.h"
#include "dis_interloper.h"
#include "dis_powerup.h"
#include "dis_revulsion.h"
#include "dis_shade.h"
#include "dis_solitary.h"
#include "dis_util.h"
#include "xrb_engine2_objectlayer.h"
#include "xrb_engine2_spriteentity.h"
#include "xrb_engine2_world.h"

using namespace Xrb;

namespace Dis
{

Engine2::SpriteEntity *SpawnSpriteEntity (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    std::string const &sprite_texture_filename,
    Engine2::EntityGuts  *entity_guts,
    FloatVector2 const &translation,
    Float const scale_factor,
    Float const angle,
    Float const first_moment,
    FloatVector2 const &velocity,
    Float const second_moment,
    Float const angular_velocity,
    Float const elasticity,
    Engine2::CollisionType const collision_type)
{
    ASSERT1(world != NULL)
    ASSERT1(object_layer != NULL)
    ASSERT1(!sprite_texture_filename.empty())
    ASSERT1(scale_factor >= 0.0f)
    ASSERT1(first_moment > 0.0f)
    ASSERT1(second_moment > 0.0f)
    ASSERT1(elasticity >= 0.0f)

    Engine2::SpriteEntity *sprite_entity =
        Engine2::SpriteEntity::Create(sprite_texture_filename);

    sprite_entity->SetTranslation(translation);
    sprite_entity->SetScaleFactor(scale_factor);
    sprite_entity->SetAngle(angle);
    sprite_entity->SetFirstMoment(first_moment);
    sprite_entity->SetVelocity(velocity);
    sprite_entity->SetSecondMoment(second_moment);
    sprite_entity->SetAngularVelocity(angular_velocity);
    sprite_entity->SetElasticity(elasticity);
    sprite_entity->SetCollisionType(collision_type);

    world->AddEntity(sprite_entity, object_layer);

    // this should be done after setting everything else, so that a
    // EntityGuts::HandleNewOwnerEntity override can have full control.
    sprite_entity->SetEntityGuts(entity_guts);
    
    return sprite_entity;
}

Asteroid *SpawnAsteroid (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    Float const scale_factor,
    Float const first_moment,
    FloatVector2 const &velocity,
    Float const mineral_content)
{
    Asteroid *asteroid = 
        new Dis::Asteroid(
            first_moment,
            mineral_content,
            false);
    Dis::SpawnSpriteEntity(
        world,
        object_layer,
        "resources/asteroid.png",
        asteroid,
        translation,
        scale_factor,
        Math::RandomAngle(),
        first_moment,
        velocity,
        1.0f,
        Math::RandomFloat(-90.0f, 90.0f),
        0.2f,
        Engine2::CT_SOLID_COLLISION);
    return asteroid;
}

Ballistic *SpawnBallistic (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    Float const scale_factor,
    FloatVector2 const &velocity,
    Float const impact_damage,
    Float const time_to_live,
    GameObjectReference<GameObject> const &owner)
{
    Ballistic *ballistic = new Ballistic(impact_damage, time_to_live, owner);
    SpawnSpriteEntity(
        world,
        object_layer,
        "resources/sadface_small.png",
        ballistic,
        translation,
        scale_factor,
        Math::Atan(velocity),
        0.001f,
        velocity,
        1.0f,
        0.0f,
        0.0f,
        Engine2::CT_NONSOLID_COLLISION);
    return ballistic;
}

Grenade *SpawnGrenade (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    Float const scale_factor,
    FloatVector2 const &velocity,
    GrenadeLauncher *const owner_grenade_launcher,
    Float const damage_to_inflict,
    Float const damage_radius,
    Float const explosion_radius,
    Uint32 const weapon_level,
    GameObjectReference<GameObject> const &owner,
    Float const health)
{
    Grenade *grenade =
        new Grenade(
            owner_grenade_launcher,
            damage_to_inflict,
            damage_radius,
            explosion_radius,
            weapon_level,
            owner,
            health);
    SpawnSpriteEntity(
        world,
        object_layer,
        "resources/grenade_small.png",
        grenade,
        translation,
        scale_factor,
        Math::Atan(velocity),
        2.0f,
        velocity,
        1.0f,
        Math::RandomFloat(-30.0f, 30.0f),
        0.1f,
        Engine2::CT_SOLID_COLLISION);
    return grenade;
}

Mine *SpawnMine (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    Float const scale_factor,
    FloatVector2 const &velocity,
    MineLayer *const owner_mine_layer,
    Float const damage_to_inflict,
    Float const damage_radius,
    Float const explosion_radius,
    Uint32 const weapon_level,
    GameObjectReference<GameObject> const &owner,
    Float const health)
{
    Mine *mine =
        new Mine(
            owner_mine_layer,
            damage_to_inflict,
            damage_radius,
            explosion_radius,
            weapon_level,
            owner,
            health);
    SpawnSpriteEntity(
        world,
        object_layer,
        "resources/mine_small.png",
        mine,
        translation,
        scale_factor,
        Math::Atan(velocity),
        20.0f,
        velocity,
        1.0f,
        Math::RandomFloat(-30.0f, 30.0f),
        0.1f,
        Engine2::CT_SOLID_COLLISION);
    return mine;
}

Missile *SpawnMissile (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    Float const scale_factor,
    Float const angle,
    FloatVector2 const &velocity,
    Float const time_to_live,
    Float const time_at_birth,
    Float const damage_to_inflict,
    Float const damage_radius,
    Float const explosion_radius,
    Uint32 const weapon_level,
    GameObjectReference<GameObject> const &owner,
    Float const health)
{
    Missile *missile =
        new Missile(
            time_to_live,
            time_at_birth,
            damage_to_inflict,
            damage_radius,
            explosion_radius,
            weapon_level,
            owner,
            health);
    SpawnSpriteEntity(
        world,
        object_layer,
        "resources/missile_small.png",
        missile,
        translation,
        scale_factor,
        angle,
        10.0f,
        velocity,
        1.0f,
        0.0f,
        0.1f,
        Engine2::CT_NONSOLID_COLLISION);//CT_SOLID_COLLISION);
    return missile;
}

EMPBomb *SpawnEMPBomb (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    Float const scale_factor,
    FloatVector2 const &velocity,
    EMPBombLayer *const owner_emp_bomb_launcher,
    Float const disable_time_factor,
    Float const blast_radius,
    Uint32 const weapon_level,
    GameObjectReference<GameObject> const &owner,
    Float const health)
{
    EMPBomb *emp_bomb = new EMPBomb(owner_emp_bomb_launcher, disable_time_factor, blast_radius, weapon_level, owner, health);
    SpawnSpriteEntity(
        world,
        object_layer,
        "resources/grenade_small.png",
        emp_bomb,
        translation,
        scale_factor,
        Math::Atan(velocity),
        1.0f,
        velocity,
        1.0f,
        Math::RandomFloat(-30.0f, 30.0f),
        0.1f,
        Engine2::CT_SOLID_COLLISION);
    return emp_bomb;
}

Powerup *SpawnMineral (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    Float const scale_factor,
    Float const first_moment,
    FloatVector2 const &velocity,
    Uint8 const mineral_type)
{
    ASSERT1(mineral_type < MINERAL_COUNT)
    Powerup *powerup = new Powerup(static_cast<ItemType>(IT_MINERAL_LOWEST + mineral_type), Powerup::PU_SOLITARY);
    SpawnSpriteEntity(
        world,
        object_layer,
        Item::GetMineralSpriteFilename(mineral_type),
        powerup,
        translation,
        scale_factor,
        Math::RandomAngle(),
        first_moment,
        velocity,
        1.0f,
        Math::RandomFloat(-90.0f, 90.0f),
        0.1f,
        Engine2::CT_SOLID_COLLISION);
    return powerup;
}

DamageExplosion *SpawnDamageExplosion (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Float const damage_amount,
    Float const damage_radius,
    Float const explosion_radius,
    Float const time_to_live,
    Float const time_at_birth,
    GameObjectReference<GameObject> const &owner)
{
    DamageExplosion *damage_explosion =
        new DamageExplosion(
            damage_amount,
            damage_radius,
            explosion_radius,
            time_to_live,
            time_at_birth,
            owner);
    SpawnSpriteEntity(
        world,
        object_layer,
        "resources/explosion1a_small.png",
        damage_explosion,
        translation,
        0.1f,
        Math::RandomAngle(),
        1.0f,
        velocity,
        1.0f,
        0.0f,
        0.0f,
        Engine2::CT_NONSOLID_COLLISION);
    return damage_explosion;
}

NoDamageExplosion *SpawnNoDamageExplosion (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Float const final_size,
    Float const time_to_live,
    Float const time_at_birth)
{
    NoDamageExplosion *no_damage_explosion = new NoDamageExplosion(final_size, time_to_live, time_at_birth);
    SpawnSpriteEntity(
        world,
        object_layer,
        "resources/explosion1a_small.png",
        no_damage_explosion,
        translation,
        0.1f,
        Math::RandomAngle(),
        1.0f,
        velocity,
        1.0f,
        0.0f,
        0.0f,
        Engine2::CT_NO_COLLISION);
    return no_damage_explosion;
}

EMPExplosion *SpawnEMPExplosion (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Float const disable_time_factor,
    Float const final_size,
    Float const time_to_live,
    Float const time_at_birth,
    GameObjectReference<GameObject> const &owner)
{
    EMPExplosion *emp_explosion = new EMPExplosion(disable_time_factor, final_size, time_to_live, time_at_birth, owner);
    SpawnSpriteEntity(
        world,
        object_layer,
        "resources/emp_explosion_small.png",
        emp_explosion,
        translation,
        0.1f,
        Math::RandomAngle(),
        1.0f,
        velocity,
        1.0f,
        0.0f,
        0.0f,
        Engine2::CT_NONSOLID_COLLISION);
    return emp_explosion;
}

Fireball *SpawnFireball (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Float const power,
    Float const final_size,
    Float const time_to_live,
    Float const time_at_birth,
    GameObjectReference<GameObject> const &owner)
{
    Fireball *fireball = new Fireball(power, final_size, time_to_live, time_at_birth, owner);
    SpawnSpriteEntity(
        world,
        object_layer,
        "resources/fireball.png",
        fireball,
        translation,
        0.1f,
        Math::RandomAngle(),
        1.0f,
        velocity,
        1.0f,
        0.0f,
        0.0f,
        Engine2::CT_NONSOLID_COLLISION);
    return fireball;
}

LaserBeam *SpawnLaserBeam (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer)
{
    ASSERT1(world != NULL)
    ASSERT1(object_layer != NULL)

    Engine2::SpriteEntity *sprite_entity =
        Engine2::SpriteEntity::Create("resources/beam_gradient_small.png");

    LaserBeam *laser_beam = new LaserBeam();
    sprite_entity->SetEntityGuts(laser_beam);
    // setting the scale factor this large helps with speed in adding it to
    // the quad tree, as the first time is temporary.  Laser will place
    // it for real later.
    sprite_entity->SetScaleFactor(0.5f * object_layer->GetSideLength());
    sprite_entity->SetFirstMoment(1.0f);
    sprite_entity->SetSecondMoment(1.0f);
    sprite_entity->SetAngularVelocity(0.0f);
    sprite_entity->SetElasticity(0.0f);
    sprite_entity->SetCollisionType(Engine2::CT_NO_COLLISION);

    world->AddEntity(sprite_entity, object_layer);
    
    return laser_beam;
}

GaussGunTrail *SpawnGaussGunTrail (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &trail_start,
    FloatVector2 const &trail_vector,
    FloatVector2 const &trail_velocity,
    Float const trail_width,
    Float const time_to_live,
    Float const time_at_birth)
{
    ASSERT1(world != NULL)
    ASSERT1(object_layer != NULL)
    ASSERT1(trail_width > 0.0f)
    ASSERT1(time_to_live > 0.0f)

    FloatVector2 trail_center = trail_start + 0.5f * trail_vector;

    GaussGunTrail *gauss_gun_trail = new GaussGunTrail(time_to_live, time_at_birth);
        
    Engine2::SpriteEntity *sprite_entity =
        Engine2::SpriteEntity::Create("resources/beam_gradient_small.png");

    sprite_entity->SetEntityGuts(gauss_gun_trail);
    sprite_entity->SetTranslation(trail_center);
    sprite_entity->SetScaleFactors(FloatVector2(0.5f * trail_vector.GetLength(), 0.5f * trail_width));
    sprite_entity->SetAngle(Math::Atan(trail_vector));
    sprite_entity->SetFirstMoment(1.0f);
    sprite_entity->SetVelocity(trail_velocity);
    sprite_entity->SetSecondMoment(1.0f);
    sprite_entity->SetAngularVelocity(0.0f);
    sprite_entity->SetElasticity(0.0f);
    sprite_entity->SetCollisionType(Engine2::CT_NO_COLLISION);

    world->AddEntity(sprite_entity, object_layer);

    return gauss_gun_trail;
}

TractorBeam *SpawnTractorBeam (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer)
{
    ASSERT1(world != NULL)
    ASSERT1(object_layer != NULL)

    Engine2::SpriteEntity *sprite_entity =
        Engine2::SpriteEntity::Create("resources/tractor_beam.png");

    TractorBeam *tractor_beam = new TractorBeam();
    sprite_entity->SetEntityGuts(tractor_beam);
    // setting the scale factor this large helps with speed in adding it to
    // the quad tree, as the first time is temporary.  Tractor will place
    // it for real later.
    sprite_entity->SetScaleFactor(0.5f * object_layer->GetSideLength());
    sprite_entity->SetFirstMoment(1.0f);
    sprite_entity->SetSecondMoment(1.0f);
    sprite_entity->SetAngularVelocity(0.0f);
    sprite_entity->SetElasticity(0.0f);
    sprite_entity->SetCollisionType(Engine2::CT_NO_COLLISION);

    world->AddEntity(sprite_entity, object_layer);
    
    return tractor_beam;
}

ShieldEffect *SpawnShieldEffect (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer)
{
    ASSERT1(world != NULL)
    ASSERT1(object_layer != NULL)

    Engine2::SpriteEntity *sprite_entity =
        Engine2::SpriteEntity::Create("resources/shield_effect_small.png");

    ShieldEffect *shield_effect = new ShieldEffect();
    sprite_entity->SetEntityGuts(shield_effect);
    // setting the scale factor this large helps with speed in adding it to
    // the quad tree, as the first time is temporary.  it will be placed
    // later for real.
    sprite_entity->SetScaleFactor(0.5f * object_layer->GetSideLength());
    sprite_entity->SetFirstMoment(1.0f);
    sprite_entity->SetSecondMoment(1.0f);
    sprite_entity->SetAngularVelocity(0.0f);
    sprite_entity->SetElasticity(0.0f);
    sprite_entity->SetCollisionType(Engine2::CT_NO_COLLISION);
    // default the shield effect to transparent
    sprite_entity->SetColorMask(Color(1.0f, 1.0f, 1.0f, 0.0f));

    world->AddEntity(sprite_entity, object_layer);
    
    return shield_effect;
}

ReticleEffect *SpawnReticleEffect (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    Color const &color_mask)
{
    ASSERT1(world != NULL)
    ASSERT1(object_layer != NULL)

    Engine2::SpriteEntity *sprite_entity =
        Engine2::SpriteEntity::Create("resources/reticle1.png");

    ReticleEffect *reticle_effect = new ReticleEffect();
    sprite_entity->SetEntityGuts(reticle_effect);
    // setting the scale factor this large helps with speed in adding it to
    // the quad tree, as the first time is temporary.  it will be placed
    // later for real.
    sprite_entity->SetScaleFactor(0.5f * object_layer->GetSideLength());
    sprite_entity->SetFirstMoment(1.0f);
    sprite_entity->SetSecondMoment(1.0f);
    sprite_entity->SetAngularVelocity(0.0f);
    sprite_entity->SetElasticity(0.0f);
    sprite_entity->SetCollisionType(Engine2::CT_NO_COLLISION);
    sprite_entity->SetColorMask(color_mask);

    world->AddEntity(sprite_entity, object_layer);
    
    return reticle_effect;
}

HealthTrigger *SpawnHealthTrigger (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    Float const scale_factor,
    FloatVector2 const &velocity,
    Float const health_delta_rate,
    Mortal::DamageType const damage_type,
    GameObjectReference<Mortal> const &ignore_this_mortal,
    GameObjectReference<GameObject> const &owner)
{
    HealthTrigger *health_trigger =
        new HealthTrigger(
            health_delta_rate,
            damage_type,
            ignore_this_mortal,
            owner);
    Engine2::SpriteEntity *sprite_entity = SpawnSpriteEntity(
        world,
        object_layer,
        "resources/sadface_small.png",
        health_trigger,
        translation,
        scale_factor,
        0.0f,
        1.0f,
        velocity,
        1.0f,
        0.0f,
        0.3f,
        Engine2::CT_NONSOLID_COLLISION);
    // make the sprite invisible
    sprite_entity->SetColorMask(Color(1.0f, 1.0f, 1.0f, 0.0f));
    return health_trigger;
}

Solitary *SpawnSolitary (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity)
{
    Solitary *solitary = new Solitary();
    SpawnSpriteEntity(
        world,
        object_layer,
        "resources/sadface_small.png",
        solitary,
        translation,
        1.0f, // arbitrary, the ship will set its own scale factor
        0.0f,
        1.0f, // arbitrary, the ship will set its own first moment
        velocity,
        1.0f,
        0.0f,
        0.3f,
        Engine2::CT_SOLID_COLLISION);
    return solitary;
}

Interloper *SpawnInterloper (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 const enemy_level)
{
    Interloper *interloper = new Interloper(enemy_level);
    SpawnSpriteEntity(
        world,
        object_layer,
        "resources/interloper.png",
        interloper,
        translation,
        1.0f, // arbitrary, the ship will set its own scale factor
        Math::RandomAngle(),
        1.0f, // arbitrary, the ship will set its own first moment
        velocity,
        1.0f,
        0.0f,
        0.3f,
        Engine2::CT_SOLID_COLLISION);
    return interloper;
}

Shade *SpawnShade (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 const enemy_level)
{
    Shade *shade = new Shade(enemy_level);
    SpawnSpriteEntity(
        world,
        object_layer,
        "resources/shade.png",
        shade,
        translation,
        1.0f, // arbitrary, the ship will set its own scale factor
        Math::RandomAngle(),
        1.0f, // arbitrary, the ship will set its own first moment
        velocity,
        1.0f,
        0.0f,
        0.3f,
        Engine2::CT_SOLID_COLLISION);
    return shade;
}

Revulsion *SpawnRevulsion (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 const enemy_level)
{
    Revulsion *revulsion = new Revulsion(enemy_level);
    SpawnSpriteEntity(
        world,
        object_layer,
        "resources/revulsion.png",
        revulsion,
        translation,
        1.0f, // arbitrary, the ship will set its own scale factor
        Math::RandomAngle(),
        1.0f, // arbitrary, the ship will set its own first moment
        velocity,
        1.0f,
        0.0f,
        0.3f,
        Engine2::CT_SOLID_COLLISION);
    return revulsion;
}

Devourment *SpawnDevourment (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 const enemy_level)
{
    Devourment *devourment = new Devourment(enemy_level);
    SpawnSpriteEntity(
        world,
        object_layer,
        "resources/devourment.png",
        devourment,
        translation,
        1.0f, // arbitrary, the ship will set its own scale factor
        Math::RandomAngle(),
        1.0f, // arbitrary, the ship will set its own first moment
        velocity,
        1.0f,
        0.0f,
        0.3f,
        Engine2::CT_SOLID_COLLISION);
    return devourment;
}

} // end of namespace Dis
