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
#include "dis_demi.h"
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
#include "dis_world.h"
#include "xrb_engine2_objectlayer.h"
#include "xrb_engine2_sprite.h"
#include "xrb_engine2_world.h"

using namespace Xrb;

// lower z-depth values indicate closer to the screen

#define Z_DEPTH_EMP_EXPLOSION      -0.8f
#define Z_DEPTH_RETICLE_EFFECT     -0.2f
#define Z_DEPTH_BALLISTIC          -0.05f
#define Z_DEPTH_SOLID               0.0f
#define Z_DEPTH_DEVOURMENT_GRINDER  0.05f
#define Z_DEPTH_SHIELD_EFFECT       0.1f
#define Z_DEPTH_FIREBALL            0.3f
#define Z_DEPTH_LASER_BEAM          0.4f
#define Z_DEPTH_GAUSS_GUN_TRAIL     0.45f
#define Z_DEPTH_DAMAGE_EXPLOSION    0.5f
#define Z_DEPTH_NO_DAMAGE_EXPLOSION 0.55f
#define Z_DEPTH_TRACTOR_BEAM        0.8f

namespace Dis
{

Engine2::Sprite *SpawnDynamicSprite (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    std::string const &sprite_texture_filename,
    Float const z_depth,
    bool const is_transparent,
    Entity *const entity,
    FloatVector2 const &translation,
    Float const scale_factor,
    Float const angle,
    Float const first_moment,
    FloatVector2 const &velocity,
    Float const angular_velocity,
    Float const elasticity)
{
    ASSERT1(world != NULL);
    ASSERT1(object_layer != NULL);
    ASSERT1(!sprite_texture_filename.empty());
    ASSERT1(entity != NULL);
    ASSERT1(scale_factor >= 0.0f);
    ASSERT1(first_moment > 0.0f);
    ASSERT1(elasticity >= 0.0f);

    entity->SetElasticity(elasticity);
    entity->SetFirstMoment(first_moment);
    entity->SetVelocity(velocity);
    entity->SetAngularVelocity(angular_velocity);

    Engine2::Sprite *sprite = Engine2::Sprite::Create(sprite_texture_filename);
    sprite->SetZDepth(z_depth);
    sprite->SetIsTransparent(is_transparent);
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
        "resources/asteroid_small.png",
        Z_DEPTH_SOLID,
        false, // is transparent
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
    Uint8 const weapon_level,
    EntityReference<Entity> const &owner)
{
    Ballistic *ballistic =
        new Ballistic(
            impact_damage,
            time_to_live,
            time_at_birth,
            weapon_level,
            owner,
            true);
    Engine2::Sprite *sprite =
        SpawnDynamicSprite(
            world,
            object_layer,
            "resources/plasma_bullet.png",
            Z_DEPTH_BALLISTIC,
            true, // is transparent
            ballistic,
            translation,
            scale_factor,
            Math::Atan(velocity),
            first_moment,
            velocity,
            0.0f,
            0.0f);
    // set the physical size ratio to reflect the largest circle that
    // can be inscribed inside the shape of the sprite's bitmap.
    sprite->SetPhysicalSizeRatio(22.0f / 128.0f);
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
    Uint8 const weapon_level,
    EntityReference<Entity> const &owner)
{
    Ballistic *ballistic =
        new Ballistic(
            impact_damage,
            time_to_live,
            time_at_birth,
            weapon_level,
            owner,
            false);
    SpawnDynamicSprite(
        world,
        object_layer,
        "resources/sadface_small.png",
        Z_DEPTH_BALLISTIC,
        false, // is transparent
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
        Z_DEPTH_SOLID,
        false, // is transparent
        grenade,
        translation,
        scale_factor,
        Math::Atan(velocity),
        Grenade::ms_default_mass,
        velocity,
        Math::RandomFloat(-30.0f, 30.0f),
        0.1f);
    return grenade;
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
    Float const health,
    bool const is_enemy_missile)
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
            health,
            is_enemy_missile ? ET_ENEMY_MISSILE : ET_MISSILE);
    Engine2::Sprite *sprite =
        SpawnDynamicSprite(
            world,
            object_layer,
            "resources/missile_small.png",
            Z_DEPTH_SOLID,
            false, // is transparent
            missile,
            translation,
            scale_factor,
            angle,
            5.0f,
            velocity,
            0.0f,
            0.1f);
    // set the physical size ratio to reflect the largest circle that
    // can be inscribed inside the shape of the sprite's bitmap.
    sprite->SetPhysicalSizeRatio(26.0f / 128.0f);
    return missile;
}

GuidedMissile *SpawnGuidedMissile (
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
    Float const health,
    bool const is_enemy_missile)
{
    GuidedMissile *guided_missile =
        is_enemy_missile
        ?
        new GuidedEnemyMissile(
            time_to_live,
            time_at_birth,
            damage_to_inflict,
            damage_radius,
            explosion_radius,
            weapon_level,
            owner,
            health)
        :
        new GuidedMissile(
            time_to_live,
            time_at_birth,
            damage_to_inflict,
            damage_radius,
            explosion_radius,
            weapon_level,
            owner,
            health);
    Engine2::Sprite *sprite =
        SpawnDynamicSprite(
            world,
            object_layer,
            "resources/missile_small.png",
            Z_DEPTH_SOLID,
            false, // is transparent
            guided_missile,
            translation,
            scale_factor,
            angle,
            5.0f,
            velocity,
            0.0f,
            0.1f);
    // set the physical size ratio to reflect the largest circle that
    // can be inscribed inside the shape of the sprite's bitmap.
    sprite->SetPhysicalSizeRatio(26.0f / 128.0f);
    return guided_missile;
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
        Z_DEPTH_SOLID,
        false, // is transparent
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
Powerup *SpawnPowerup (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    Float const scale_factor,
    Float const first_moment,
    FloatVector2 const &velocity,
    std::string const &sprite_texture_filename,
    ItemType const item_type)
{
    ASSERT1(item_type < IT_POWERUP_LIMIT);
    Powerup *powerup = new Powerup(item_type);
    SpawnDynamicSprite(
        world,
        object_layer,
        sprite_texture_filename,
        Z_DEPTH_SOLID,
        false, // is transparent
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

Powerup *SpawnPowerup (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    Float const scale_factor,
    Float const first_moment,
    FloatVector2 const &velocity,
    std::string const &sprite_texture_filename,
    Item *const item)
{
    ASSERT1(item != NULL);
    Powerup *powerup = new Powerup(item);
    SpawnDynamicSprite(
        world,
        object_layer,
        sprite_texture_filename,
        Z_DEPTH_SOLID,
        false, // is transparent
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
        Z_DEPTH_DAMAGE_EXPLOSION,
        true, // is transparent
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
        Z_DEPTH_NO_DAMAGE_EXPLOSION,
        true, // is transparent
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
        Z_DEPTH_EMP_EXPLOSION,
        true, // is transparent
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
    Float const starting_damage,
    Float const potential_damage,
    Float const final_size,
    Float const time_to_live,
    Float const time_at_birth,
    EntityReference<Entity> const &owner)
{
    Fireball *fireball =
        new Fireball(
            starting_damage,
            potential_damage,
            final_size,
            time_to_live,
            time_at_birth,
            owner);
    SpawnDynamicSprite(
        world,
        object_layer,
        "resources/fireball.png",
        Z_DEPTH_FIREBALL,
        true, // is transparent
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
    ASSERT1(world != NULL);
    ASSERT1(object_layer != NULL);

    Engine2::Sprite *sprite = Engine2::Sprite::Create("resources/beam_gradient_small.png");
    // setting the scale factor this large helps with speed in adding it to
    // the quad tree, as the first time is temporary.  Laser will place
    // it for real later.
    sprite->SetZDepth(Z_DEPTH_LASER_BEAM);
    sprite->SetIsTransparent(true);
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
    ASSERT1(world != NULL);
    ASSERT1(object_layer != NULL);
    ASSERT1(trail_width > 0.0f);
    ASSERT1(time_to_live > 0.0f);

    FloatVector2 trail_center = trail_start + 0.5f * trail_vector;

    Engine2::Sprite *sprite = Engine2::Sprite::Create("resources/beam_gradient_small.png");
    sprite->SetZDepth(Z_DEPTH_GAUSS_GUN_TRAIL);
    sprite->SetIsTransparent(true);
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
    ASSERT1(world != NULL);
    ASSERT1(object_layer != NULL);

    Engine2::Sprite *sprite = Engine2::Sprite::Create("resources/tractor_beam.png");
    sprite->SetZDepth(Z_DEPTH_TRACTOR_BEAM);
    sprite->SetIsTransparent(true);
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
    ASSERT1(world != NULL);
    ASSERT1(object_layer != NULL);

    Engine2::Sprite *sprite =
        Engine2::Sprite::Create("resources/shield_effect_small.png");
    sprite->SetZDepth(Z_DEPTH_SHIELD_EFFECT);
    sprite->SetIsTransparent(true);
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
    ASSERT1(world != NULL);
    ASSERT1(object_layer != NULL);

    Engine2::Sprite *sprite =
        Engine2::Sprite::Create("resources/reticle1.png");
    sprite->SetZDepth(Z_DEPTH_RETICLE_EFFECT);
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
    Engine2::Sprite *sprite =
        SpawnDynamicSprite(
            world,
            object_layer,
            "resources/sadface_small.png",
            0.01f, // z depth (arbitrary, since its invisible)
            true, // is transparent
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
        Ship::GetShipSpriteFilename(ET_SOLITARY, 0),
        Z_DEPTH_SOLID,
        false, // is transparent
        solitary,
        translation,
        1.0f, // arbitrary, the ship will set its own scale factor
        0.0f,
        1.0f, // arbitrary, the ship will set its own first moment
        velocity,
        0.0f,
        0.05f);
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
        Ship::GetShipSpriteFilename(ET_INTERLOPER, enemy_level),
        Z_DEPTH_SOLID,
        false, // is transparent
        interloper,
        translation,
        1.0f, // arbitrary, the ship will set its own scale factor
        Math::RandomAngle(),
        1.0f, // arbitrary, the ship will set its own first moment
        velocity,
        0.0f,
        0.05f);
    DStaticCast<Dis::World *>(world)->RecordCreatedEnemyShip(interloper);
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
        Ship::GetShipSpriteFilename(ET_SHADE, enemy_level),
        Z_DEPTH_SOLID,
        false, // is transparent
        shade,
        translation,
        1.0f, // arbitrary, the ship will set its own scale factor
        Math::RandomAngle(),
        1.0f, // arbitrary, the ship will set its own first moment
        velocity,
        0.0f,
        0.05f);
    DStaticCast<Dis::World *>(world)->RecordCreatedEnemyShip(shade);
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
        Ship::GetShipSpriteFilename(ET_REVULSION, enemy_level),
        Z_DEPTH_SOLID,
        false, // is transparent
        revulsion,
        translation,
        1.0f, // arbitrary, the ship will set its own scale factor
        Math::RandomAngle(),
        1.0f, // arbitrary, the ship will set its own first moment
        velocity,
        0.0f,
        0.05f);
    DStaticCast<Dis::World *>(world)->RecordCreatedEnemyShip(revulsion);
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
        Ship::GetShipSpriteFilename(ET_DEVOURMENT, enemy_level),
        Z_DEPTH_SOLID,
        false, // is transparent
        devourment,
        translation,
        1.0f, // arbitrary, the ship will set its own scale factor
        Math::RandomAngle(),
        1.0f, // arbitrary, the ship will set its own first moment
        velocity,
        0.0f,
        0.05f);
    DStaticCast<Dis::World *>(world)->RecordCreatedEnemyShip(devourment);
    return devourment;
}

Demi *SpawnDemi (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 const enemy_level)
{
    Demi *demi = new Demi(enemy_level);
    SpawnDynamicSprite(
        world,
        object_layer,
        Ship::GetShipSpriteFilename(ET_DEMI, enemy_level),
        Z_DEPTH_SOLID,
        false, // is transparent
        demi,
        translation,
        1.0f, // arbitrary, the ship will set its own scale factor
        Math::RandomAngle(),
        1.0f, // arbitrary, the ship will set its own first moment
        velocity,
        0.0f,
        0.05f);
    DStaticCast<Dis::World *>(world)->RecordCreatedEnemyShip(demi);
    return demi;
}

HealthTrigger *SpawnDevourmentMouthHealthTrigger (
    Engine2::World *const world,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    Float const scale_factor,
    FloatVector2 const &velocity,
    Float const health_delta_rate,
    Mortal::DamageType const damage_type,
    EntityReference<Mortal> const &ignore_this_mortal,
    EntityReference<Entity> const &owner,
    Uint8 const enemy_level)
{
    static std::string const s_grinder_sprite_filename[EnemyShip::ENEMY_LEVEL_COUNT] =
    {
        "resources/grinder0_small.png",
        "resources/grinder1_small.png",
        "resources/grinder2_small.png",
        "resources/grinder3_small.png"
    };

    ASSERT1(enemy_level < EnemyShip::ENEMY_LEVEL_COUNT);

    HealthTrigger *health_trigger =
        new HealthTrigger(
            health_delta_rate,
            damage_type,
            ignore_this_mortal,
            owner);
    SpawnDynamicSprite(
        world,
        object_layer,
        s_grinder_sprite_filename[enemy_level],
        Z_DEPTH_DEVOURMENT_GRINDER,
        false, // is transparent
        health_trigger,
        translation,
        scale_factor,
        0.0f,
        1.0f,
        velocity,
        0.0f,
        0.3f);
    return health_trigger;
}

} // end of namespace Dis
