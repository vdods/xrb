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
#include "dis_entity.h"
#include "dis_healthtrigger.h"
#include "dis_interloper.h"
#include "dis_powerup.h"
#include "dis_revulsion.h"
#include "dis_shade.h"
#include "dis_solitary.h"
#include "dis_util.h"
#include "xrb_engine2_objectlayer.h"
#include "xrb_engine2_sprite.h"
#include "xrb_engine2_world.h"

using namespace Xrb;

namespace Dis
{

Engine2::Sprite *SpawnDynamicSprite (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    std::string const &sprite_texture_filename,
    Entity *const entity,
    FloatVector2 const &translation,
    Float const scale_factor,
    Float const angle,
    Float const first_moment,
    FloatVector2 const &velocity,
    Float const angular_velocity,
    Float const elasticity)
{
    ASSERT1(world != NULL)
    ASSERT1(object_layer != NULL)
    ASSERT1(!sprite_texture_filename.empty())
    ASSERT1(entity != NULL)
    ASSERT1(scale_factor >= 0.0f)
    ASSERT1(first_moment > 0.0f)
    ASSERT1(elasticity >= 0.0f)

    entity->SetElasticity(elasticity);
    entity->SetFirstMoment(first_moment);
    entity->SetVelocity(velocity);
    entity->SetAngularVelocity(angular_velocity);

    Engine2::Sprite *sprite = Engine2::Sprite::Create(sprite_texture_filename);
    sprite->SetTranslation(translation);
    sprite->SetScaleFactor(scale_factor);
    sprite->SetAngle(angle);
    sprite->SetEntity(entity);

    world->AddDynamicObject(sprite, object_layer);

    return sprite;
}

Asteroid *SpawnAsteroid (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    Float const scale_factor,
    Float const first_moment,
    FloatVector2 const &velocity,
    Float const mineral_content,
    bool const is_a_secondary_asteroid)
{
    Asteroid *asteroid = 
        new Asteroid(
            first_moment,
            mineral_content,
            is_a_secondary_asteroid);
    SpawnDynamicSprite(
        world,
        object_layer,
        "resources/asteroid.png",
        asteroid,
        translation,
        scale_factor,
        Math::RandomAngle(),
        first_moment,
        velocity,
        Math::RandomFloat(-90.0f, 90.0f),
        0.2f);
    return asteroid;
}

Ballistic *SpawnSmartBallistic (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    Float const scale_factor,
    Float const first_moment,
    FloatVector2 const &velocity,
    Float const impact_damage,
    Float const time_to_live,
    Float const time_at_birth,
    EntityReference<Entity> const &owner)
{
    Ballistic *ballistic =
        new Ballistic(
            impact_damage,
            time_to_live,
            time_at_birth,
            owner,
            true);
    SpawnDynamicSprite(
        world,
        object_layer,
        "resources/plasma_bullet.png",
        ballistic,
        translation,
        scale_factor,
        Math::Atan(velocity),
        first_moment,
        velocity,
        0.0f,
        0.0f);
    return ballistic;
}

Ballistic *SpawnDumbBallistic (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    Float const scale_factor,
    Float const first_moment,
    FloatVector2 const &velocity,
    Float const impact_damage,
    Float const time_to_live,
    Float const time_at_birth,
    EntityReference<Entity> const &owner)
{
    Ballistic *ballistic =
        new Ballistic(
            impact_damage,
            time_to_live,
            time_at_birth,
            owner,
            false);
    SpawnDynamicSprite(
        world,
        object_layer,
        "resources/sadface_small.png",
        ballistic,
        translation,
        scale_factor,
        Math::Atan(velocity),
        first_moment,
        velocity,
        0.0f,
        0.0f);
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
    EntityReference<Entity> const &owner,
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
    SpawnDynamicSprite(
        world,
        object_layer,
        "resources/grenade_small.png",
        grenade,
        translation,
        scale_factor,
        Math::Atan(velocity),
        4.0f,
        velocity,
        Math::RandomFloat(-30.0f, 30.0f),
        0.1f);
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
    EntityReference<Entity> const &owner,
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
    SpawnDynamicSprite(
        world,
        object_layer,
        "resources/mine_small.png",
        mine,
        translation,
        scale_factor,
        Math::Atan(velocity),
        20.0f,
        velocity,
        Math::RandomFloat(-30.0f, 30.0f),
        0.1f);
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
    EntityReference<Entity> const &owner,
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
    SpawnDynamicSprite(
        world,
        object_layer,
        "resources/missile_small.png",
        missile,
        translation,
        scale_factor,
        angle,
        5.0f,
        velocity,
        0.0f,
        0.1f);
    return missile;
}
/*
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
    EntityReference<Entity> const &owner,
    Float const health)
{
    EMPBomb *emp_bomb = new EMPBomb(owner_emp_bomb_launcher, disable_time_factor, blast_radius, weapon_level, owner, health);
    SpawnDynamicSprite(
        world,
        object_layer,
        "resources/grenade_small.png",
        emp_bomb,
        translation,
        scale_factor,
        Math::Atan(velocity),
        1.0f,
        velocity,
        Math::RandomFloat(-30.0f, 30.0f),
        0.1f);
    return emp_bomb;
}
*/
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
    SpawnDynamicSprite(
        world,
        object_layer,
        Item::GetMineralSpriteFilename(mineral_type),
        powerup,
        translation,
        scale_factor,
        Math::RandomAngle(),
        first_moment,
        velocity,
        Math::RandomFloat(-90.0f, 90.0f),
        0.1f);
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
    EntityReference<Entity> const &owner)
{
    DamageExplosion *damage_explosion =
        new DamageExplosion(
            damage_amount,
            damage_radius,
            explosion_radius,
            time_to_live,
            time_at_birth,
            owner);
    SpawnDynamicSprite(
        world,
        object_layer,
        "resources/explosion1a_small.png",
        damage_explosion,
        translation,
        0.1f,
        Math::RandomAngle(),
        1.0f,
        velocity,
        0.0f,
        0.0f);
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
    SpawnDynamicSprite(
        world,
        object_layer,
        "resources/explosion1a_small.png",
        no_damage_explosion,
        translation,
        0.1f,
        Math::RandomAngle(),
        1.0f,
        velocity,
        0.0f,
        0.0f);
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
    EntityReference<Entity> const &owner)
{
    EMPExplosion *emp_explosion = new EMPExplosion(disable_time_factor, final_size, time_to_live, time_at_birth, owner);
    SpawnDynamicSprite(
        world,
        object_layer,
        "resources/emp_explosion_small.png",
        emp_explosion,
        translation,
        0.1f,
        Math::RandomAngle(),
        1.0f,
        velocity,
        0.0f,
        0.0f);
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
    EntityReference<Entity> const &owner)
{
    Fireball *fireball = new Fireball(power, final_size, time_to_live, time_at_birth, owner);
    SpawnDynamicSprite(
        world,
        object_layer,
        "resources/fireball.png",
        fireball,
        translation,
        0.1f,
        Math::RandomAngle(),
        1.0f,
        velocity,
        0.0f,
        0.0f);
    return fireball;
}

LaserBeam *SpawnLaserBeam (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer)
{
    ASSERT1(world != NULL)
    ASSERT1(object_layer != NULL)

    Engine2::Sprite *sprite = Engine2::Sprite::Create("resources/beam_gradient_small.png");
    // setting the scale factor this large helps with speed in adding it to
    // the quad tree, as the first time is temporary.  Laser will place
    // it for real later.
    sprite->SetScaleFactor(0.5f * object_layer->GetSideLength());

    LaserBeam *laser_beam = new LaserBeam();

    sprite->SetEntity(laser_beam);
    world->AddDynamicObject(sprite, object_layer);
    
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

    Engine2::Sprite *sprite = Engine2::Sprite::Create("resources/beam_gradient_small.png");
    sprite->SetTranslation(trail_center);
    sprite->SetScaleFactors(FloatVector2(0.5f * trail_vector.GetLength(), 0.5f * trail_width));
    sprite->SetAngle(Math::Atan(trail_vector));
    
    GaussGunTrail *gauss_gun_trail = new GaussGunTrail(time_to_live, time_at_birth);
    gauss_gun_trail->SetVelocity(trail_velocity);

    sprite->SetEntity(gauss_gun_trail);
    world->AddDynamicObject(sprite, object_layer);

    return gauss_gun_trail;
}

TractorBeam *SpawnTractorBeam (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer)
{
    ASSERT1(world != NULL)
    ASSERT1(object_layer != NULL)

    Engine2::Sprite *sprite = Engine2::Sprite::Create("resources/tractor_beam.png");
    // setting the scale factor this large helps with speed in adding it to
    // the quad tree, as the first time is temporary.  Tractor will place
    // it for real later.
    sprite->SetScaleFactor(0.5f * object_layer->GetSideLength());

    TractorBeam *tractor_beam = new TractorBeam();

    sprite->SetEntity(tractor_beam);
    world->AddDynamicObject(sprite, object_layer);
    
    return tractor_beam;
}

ShieldEffect *SpawnShieldEffect (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer)
{
    ASSERT1(world != NULL)
    ASSERT1(object_layer != NULL)

    Engine2::Sprite *sprite =
        Engine2::Sprite::Create("resources/shield_effect_small.png");
    // setting the scale factor this large helps with speed in adding it to
    // the quad tree, as the first time is temporary.  it will be placed
    // later for real.
    sprite->SetScaleFactor(0.5f * object_layer->GetSideLength());
    // default the shield effect to transparent
    sprite->SetColorMask(Color(1.0f, 1.0f, 1.0f, 0.0f));

    ShieldEffect *shield_effect = new ShieldEffect();

    sprite->SetEntity(shield_effect);
    world->AddDynamicObject(sprite, object_layer);
    
    return shield_effect;
}

ReticleEffect *SpawnReticleEffect (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    Color const &color_mask)
{
    ASSERT1(world != NULL)
    ASSERT1(object_layer != NULL)

    Engine2::Sprite *sprite =
        Engine2::Sprite::Create("resources/reticle1.png");
    // setting the scale factor this large helps with speed in adding it to
    // the quad tree, as the first time is temporary.  it will be placed
    // later for real.
    sprite->SetScaleFactor(0.5f * object_layer->GetSideLength());
    sprite->SetColorMask(color_mask);

    ReticleEffect *reticle_effect = new ReticleEffect();

    sprite->SetEntity(reticle_effect);
    world->AddDynamicObject(sprite, object_layer);
    
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
    EntityReference<Mortal> const &ignore_this_mortal,
    EntityReference<Entity> const &owner)
{
    HealthTrigger *health_trigger =
        new HealthTrigger(
            health_delta_rate,
            damage_type,
            ignore_this_mortal,
            owner);
    Engine2::Sprite *sprite = SpawnDynamicSprite(
        world,
        object_layer,
        "resources/sadface_small.png",
        health_trigger,
        translation,
        scale_factor,
        0.0f,
        1.0f,
        velocity,
        0.0f,
        0.3f);
    // make the sprite invisible
    sprite->SetColorMask(Color(1.0f, 1.0f, 1.0f, 0.0f));
    return health_trigger;
}

Solitary *SpawnSolitary (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity)
{
    Solitary *solitary = new Solitary();
    SpawnDynamicSprite(
        world,
        object_layer,
        "resources/sadface_small.png",
        solitary,
        translation,
        1.0f, // arbitrary, the ship will set its own scale factor
        0.0f,
        1.0f, // arbitrary, the ship will set its own first moment
        velocity,
        0.0f,
        0.3f);
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
    SpawnDynamicSprite(
        world,
        object_layer,
        "resources/interloper.png",
        interloper,
        translation,
        1.0f, // arbitrary, the ship will set its own scale factor
        Math::RandomAngle(),
        1.0f, // arbitrary, the ship will set its own first moment
        velocity,
        0.0f,
        0.3f);
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
    SpawnDynamicSprite(
        world,
        object_layer,
        "resources/shade.png",
        shade,
        translation,
        1.0f, // arbitrary, the ship will set its own scale factor
        Math::RandomAngle(),
        1.0f, // arbitrary, the ship will set its own first moment
        velocity,
        0.0f,
        0.3f);
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
    SpawnDynamicSprite(
        world,
        object_layer,
        "resources/revulsion.png",
        revulsion,
        translation,
        1.0f, // arbitrary, the ship will set its own scale factor
        Math::RandomAngle(),
        1.0f, // arbitrary, the ship will set its own first moment
        velocity,
        0.0f,
        0.3f);
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
    SpawnDynamicSprite(
        world,
        object_layer,
        "resources/devourment.png",
        devourment,
        translation,
        1.0f, // arbitrary, the ship will set its own scale factor
        Math::RandomAngle(),
        1.0f, // arbitrary, the ship will set its own first moment
        velocity,
        0.0f,
        0.3f);
    return devourment;
}

} // end of namespace Dis
