// ///////////////////////////////////////////////////////////////////////////
// dis_spawn.cpp by Victor Dods, created 2005/11/10
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_spawn.hpp"

#include "dis_asteroid.hpp"
#include "dis_ballistic.hpp"
#include "dis_demi.hpp"
#include "dis_devourment.hpp"
#include "dis_effect.hpp"
#include "dis_explosive.hpp"
#include "dis_entity.hpp"
#include "dis_healthtrigger.hpp"
#include "dis_interloper.hpp"
#include "dis_powerup.hpp"
#include "dis_revulsion.hpp"
#include "dis_shade.hpp"
#include "dis_solitary.hpp"
#include "dis_util.hpp"
#include "dis_world.hpp"
#include "xrb_engine2_animatedsprite.hpp"
#include "xrb_engine2_objectlayer.hpp"
#include "xrb_engine2_sprite.hpp"
#include "xrb_engine2_world.hpp"

using namespace Xrb;

// lower z-depth values indicate closer to the screen

#define Z_DEPTH_EMP_EXPLOSION      -0.8f
#define Z_DEPTH_RETICLE_EFFECT     -0.2f
#define Z_DEPTH_LIGHTNING_EFFECT   -0.15f
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
    Engine2::ObjectLayer *object_layer,
    std::string const &sprite_texture_path,
    Float z_depth,
    bool is_transparent,
    Entity *entity,
    FloatVector2 const &translation,
    Float scale_factor,
    Float angle,
    Float mass,
    FloatVector2 const &velocity,
    Float angular_velocity,
    Float elasticity)
{
    ASSERT1(object_layer != NULL);
    ASSERT1(object_layer->OwnerWorld() != NULL);
    ASSERT1(!sprite_texture_path.empty());
    ASSERT1(entity != NULL);
    ASSERT1(scale_factor >= 0.0f);
    ASSERT1(mass > 0.0f);
    ASSERT1(elasticity >= 0.0f);

    entity->SetElasticity(elasticity);
    entity->SetMass(mass);
    entity->SetVelocity(velocity);
    entity->SetAngularVelocity(angular_velocity);

    Engine2::Sprite *sprite = Engine2::Sprite::Create(sprite_texture_path);
    sprite->SetZDepth(z_depth);
    sprite->SetIsTransparent(is_transparent);
    sprite->SetTranslation(translation);
    sprite->SetScaleFactor(scale_factor);
    sprite->SetAngle(angle);
    sprite->SetEntity(entity);

    object_layer->OwnerWorld()->AddDynamicObject(sprite, object_layer);

    return sprite;
}

Engine2::AnimatedSprite *SpawnDynamicAnimatedSprite (
    Engine2::ObjectLayer *object_layer,
    std::string const &sprite_texture_path,
    Float current_time,
    Float z_depth,
    bool is_transparent,
    Entity *entity,
    FloatVector2 const &translation,
    Float scale_factor,
    Float angle,
    Float mass,
    FloatVector2 const &velocity,
    Float angular_velocity,
    Float elasticity)
{
    ASSERT1(object_layer != NULL);
    ASSERT1(object_layer->OwnerWorld() != NULL);
    ASSERT1(!sprite_texture_path.empty());
    ASSERT1(entity != NULL);
    ASSERT1(scale_factor >= 0.0f);
    ASSERT1(mass > 0.0f);
    ASSERT1(elasticity >= 0.0f);

    entity->SetElasticity(elasticity);
    entity->SetMass(mass);
    entity->SetVelocity(velocity);
    entity->SetAngularVelocity(angular_velocity);

    Engine2::AnimatedSprite *sprite = Engine2::AnimatedSprite::Create(sprite_texture_path, current_time);
    sprite->SetZDepth(z_depth);
    sprite->SetIsTransparent(is_transparent);
    sprite->SetTranslation(translation);
    sprite->SetScaleFactor(scale_factor);
    sprite->SetAngle(angle);
    sprite->SetEntity(entity);

    object_layer->OwnerWorld()->AddDynamicObject(sprite, object_layer);

    return sprite;
}

Asteroid *SpawnAsteroid (
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    Float const scale_factor,
    Float const mass,
    FloatVector2 const &velocity,
    Float const mineral_content,
    bool const is_a_secondary_asteroid)
{
    Asteroid *asteroid =
        new Asteroid(
            mass,
            mineral_content,
            is_a_secondary_asteroid);
    SpawnDynamicSprite(
        object_layer,
        "resources/asteroid_small.png",
        Z_DEPTH_SOLID,
        false, // is transparent
        asteroid,
        translation,
        scale_factor,
        Math::RandomAngle(),
        mass,
        velocity,
        Math::RandomFloat(-90.0f, 90.0f),
        0.2f);
    return asteroid;
}

Ballistic *SpawnSmartBallistic (
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    Float const scale_factor,
    Float const mass,
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
            object_layer,
            "resources/plasma_bullet.png",
            Z_DEPTH_BALLISTIC,
            true, // is transparent
            ballistic,
            translation,
            scale_factor,
            Math::Atan(velocity),
            mass,
            velocity,
            0.0f,
            0.0f);
    // set the physical size ratio to reflect the largest circle that
    // can be inscribed inside the shape of the sprite's bitmap.
    sprite->SetPhysicalSizeRatio(22.0f / 128.0f);
    return ballistic;
}

Ballistic *SpawnDumbBallistic (
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    Float const scale_factor,
    Float const mass,
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
        object_layer,
        "resources/tractor_beam.png",
        Z_DEPTH_BALLISTIC,
        false, // is transparent
        ballistic,
        translation,
        scale_factor,
        Math::Atan(velocity),
        mass,
        velocity,
        0.0f,
        0.0f);
    return ballistic;
}

Grenade *SpawnGrenade (
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
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    Float const scale_factor,
    Float const angle,
    FloatVector2 const &velocity,
    MissileLauncher *owner_missile_launcher,
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
            owner_missile_launcher,
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
            object_layer,
            "resources/missile_small.png",
            Z_DEPTH_SOLID,
            false, // is transparent
            missile,
            translation,
            scale_factor,
            angle,
            5.0f, // mass
            velocity,
            0.0f,
            0.1f);
    // set the physical size ratio to reflect the largest circle that
    // can be inscribed inside the shape of the sprite's bitmap.
    sprite->SetPhysicalSizeRatio(26.0f / 128.0f);
    return missile;
}

GuidedMissile *SpawnGuidedMissile (
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

Powerup *SpawnPowerup (
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    Float const scale_factor,
    Float const mass,
    FloatVector2 const &velocity,
    std::string const &sprite_texture_path,
    ItemType const item_type)
{
    ASSERT1(item_type < IT_POWERUP_LIMIT);
    Powerup *powerup = new Powerup(item_type);
    SpawnDynamicSprite(
        object_layer,
        sprite_texture_path,
        Z_DEPTH_SOLID,
        false, // is transparent
        powerup,
        translation,
        scale_factor,
        Math::RandomAngle(),
        mass,
        velocity,
        Math::RandomFloat(-90.0f, 90.0f),
        0.1f);
    return powerup;
}

Powerup *SpawnPowerup (
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    Float const scale_factor,
    Float const mass,
    FloatVector2 const &velocity,
    std::string const &sprite_texture_path,
    Item *const item)
{
    ASSERT1(item != NULL);
    Powerup *powerup = new Powerup(item);
    SpawnDynamicSprite(
        object_layer,
        sprite_texture_path,
        Z_DEPTH_SOLID,
        false, // is transparent
        powerup,
        translation,
        scale_factor,
        Math::RandomAngle(),
        mass,
        velocity,
        Math::RandomFloat(-90.0f, 90.0f),
        0.1f);
    return powerup;
}

DamageExplosion *SpawnDamageExplosion (
    Engine2::ObjectLayer *object_layer,
    std::string const &sprite_texture_path,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Float damage_amount,
    Float damage_radius,
    Float initial_size,
    Float explosion_radius,
    Float time_to_live,
    Float time_at_birth,
    EntityReference<Entity> const &owner)
{
    DamageExplosion *damage_explosion =
        new DamageExplosion(
            damage_amount,
            damage_radius,
            initial_size,
            explosion_radius,
            time_to_live,
            time_at_birth,
            owner);
    SpawnDynamicSprite(
        object_layer,
        sprite_texture_path,
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
    Engine2::ObjectLayer *object_layer,
    std::string const &sprite_texture_path,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Float initial_size,
    Float final_size,
    Float time_to_live,
    Float time_at_birth)
{
    NoDamageExplosion *no_damage_explosion = new NoDamageExplosion(initial_size, final_size, time_to_live, time_at_birth);
    SpawnDynamicSprite(
        object_layer,
        sprite_texture_path,
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
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Float disable_time_factor,
    Float initial_size,
    Float final_size,
    Float time_to_live,
    Float time_at_birth,
    EntityReference<Entity> const &owner)
{
    EMPExplosion *emp_explosion = new EMPExplosion(disable_time_factor, initial_size, final_size, time_to_live, time_at_birth, owner);
    SpawnDynamicSprite(
        object_layer,
        "resources/shield_effect_small.png",
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
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Float starting_damage,
    Float potential_damage,
    Float initial_size,
    Float final_size,
    Float time_to_live,
    Float time_at_birth,
    EntityReference<Entity> const &owner)
{
    Fireball *fireball =
        new Fireball(
            starting_damage,
            potential_damage,
            initial_size,
            final_size,
            time_to_live,
            time_at_birth,
            owner);
    SpawnDynamicSprite(
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
    Engine2::ObjectLayer *const object_layer)
{
    ASSERT1(object_layer != NULL);
    ASSERT1(object_layer->OwnerWorld() != NULL);

    Engine2::Sprite *sprite = Engine2::Sprite::Create("resources/beam_gradient_small.png");
    // setting the scale factor this large helps with speed in adding it to
    // the quad tree, as the first time is temporary.  Laser will place
    // it for real later.
    sprite->SetZDepth(Z_DEPTH_LASER_BEAM);
    sprite->SetIsTransparent(true);
    sprite->SetScaleFactor(0.5f * object_layer->SideLength());

    LaserBeam *laser_beam = new LaserBeam();

    sprite->SetEntity(laser_beam);
    object_layer->OwnerWorld()->AddDynamicObject(sprite, object_layer);

    return laser_beam;
}

GaussGunTrail *SpawnGaussGunTrail (
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &trail_start,
    FloatVector2 const &trail_vector,
    FloatVector2 const &trail_velocity,
    Float const trail_width,
    Float const time_to_live,
    Float const time_at_birth)
{
    ASSERT1(object_layer != NULL);
    ASSERT1(object_layer->OwnerWorld() != NULL);
    ASSERT1(trail_width > 0.0f);
    ASSERT1(time_to_live > 0.0f);

    FloatVector2 trail_center = trail_start + 0.5f * trail_vector;

    Engine2::Sprite *sprite = Engine2::Sprite::Create("resources/beam_gradient_small.png");
    sprite->SetZDepth(Z_DEPTH_GAUSS_GUN_TRAIL);
    sprite->SetIsTransparent(true);
    sprite->SetTranslation(trail_center);
    sprite->SetScaleFactors(FloatVector2(0.5f * trail_vector.Length(), 0.5f * trail_width));
    sprite->SetAngle(Math::Atan(trail_vector));

    GaussGunTrail *gauss_gun_trail = new GaussGunTrail(time_to_live, time_at_birth);
    gauss_gun_trail->SetVelocity(trail_velocity);

    sprite->SetEntity(gauss_gun_trail);
    object_layer->OwnerWorld()->AddDynamicObject(sprite, object_layer);

    return gauss_gun_trail;
}

TractorBeam *SpawnTractorBeam (
    Engine2::ObjectLayer *object_layer)
{
    ASSERT1(object_layer != NULL);
    ASSERT1(object_layer->OwnerWorld() != NULL);

    Engine2::Sprite *sprite = Engine2::Sprite::Create("resources/tractor_beam.png");
    sprite->SetZDepth(Z_DEPTH_TRACTOR_BEAM);
    sprite->SetIsTransparent(true);
    // setting the scale factor this large helps with speed in adding it to
    // the quad tree, as the first time is temporary.  Tractor will place
    // it for real later.
    sprite->SetScaleFactor(0.5f * object_layer->SideLength());

    TractorBeam *tractor_beam = new TractorBeam();

    sprite->SetEntity(tractor_beam);
    object_layer->OwnerWorld()->AddDynamicObject(sprite, object_layer);

    return tractor_beam;
}

ShieldEffect *SpawnShieldEffect (
    Engine2::ObjectLayer *object_layer)
{
    ASSERT1(object_layer != NULL);
    ASSERT1(object_layer->OwnerWorld() != NULL);

    Engine2::Sprite *sprite =
        Engine2::Sprite::Create("resources/shield_effect_small.png");
    sprite->SetZDepth(Z_DEPTH_SHIELD_EFFECT);
    sprite->SetIsTransparent(true);
    // setting the scale factor this large helps with speed in adding it to
    // the quad tree, as the first time is temporary.  it will be placed
    // later for real.
    sprite->SetScaleFactor(0.5f * object_layer->SideLength());
    // default the shield effect to transparent
    sprite->ColorMask() = Color::ms_transparent_black;

    ShieldEffect *shield_effect = new ShieldEffect();

    sprite->SetEntity(shield_effect);
    object_layer->OwnerWorld()->AddDynamicObject(sprite, object_layer);

    return shield_effect;
}

LightningEffect *SpawnLightningEffect (
    Engine2::ObjectLayer *object_layer,
    Float current_time)
{
    ASSERT1(object_layer != NULL);
    ASSERT1(object_layer->OwnerWorld() != NULL);

    Engine2::AnimatedSprite *sprite =
        Engine2::AnimatedSprite::Create("resources/lightning.anim", current_time);
    sprite->SetZDepth(Z_DEPTH_LIGHTNING_EFFECT);
    sprite->SetIsTransparent(true);
    // setting the scale factor this large helps with speed in adding it to
    // the quad tree, as the first time is temporary.  it will be placed
    // later for real.
    sprite->SetScaleFactor(0.5f * object_layer->SideLength());
//     // default the lightning effect to transparent
//     sprite->ColorMask() = Color::ms_transparent_black;

    LightningEffect *lightning_effect = new LightningEffect();

    sprite->SetEntity(lightning_effect);
    object_layer->OwnerWorld()->AddDynamicObject(sprite, object_layer);

    return lightning_effect;
}

ReticleEffect *SpawnReticleEffect (
    Engine2::ObjectLayer *object_layer,
    Color const &color_mask)
{
    ASSERT1(object_layer != NULL);
    ASSERT1(object_layer->OwnerWorld() != NULL);

    Engine2::Sprite *sprite =
        Engine2::Sprite::Create("resources/reticle1.png");
    sprite->SetZDepth(Z_DEPTH_RETICLE_EFFECT);
    // setting the scale factor this large helps with speed in adding it to
    // the quad tree, as the first time is temporary.  it will be placed
    // later for real.
    sprite->SetScaleFactor(0.5f * object_layer->SideLength());
    sprite->ColorMask() = color_mask;

    ReticleEffect *reticle_effect = new ReticleEffect();

    sprite->SetEntity(reticle_effect);
    object_layer->OwnerWorld()->AddDynamicObject(sprite, object_layer);

    return reticle_effect;
}

HealthTrigger *SpawnHealthTrigger (
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
            object_layer,
            "resources/tractor_beam.png",
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
    sprite->ColorMask() = Color::ms_transparent_black;
    return health_trigger;
}

Solitary *SpawnSolitary (
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity)
{
    Solitary *solitary = new Solitary();
    SpawnDynamicSprite(
        object_layer,
        Ship::ShipSpritePath(ET_SOLITARY, 0),
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
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 const enemy_level)
{
    Interloper *interloper = new Interloper(enemy_level);
    SpawnDynamicSprite(
        object_layer,
        Ship::ShipSpritePath(ET_INTERLOPER, enemy_level),
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
    DStaticCast<Dis::World *>(object_layer->OwnerWorld())->RecordCreatedEnemyShip(interloper);
    return interloper;
}

Shade *SpawnShade (
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 const enemy_level)
{
    Shade *shade = new Shade(enemy_level);
    SpawnDynamicSprite(
        object_layer,
        Ship::ShipSpritePath(ET_SHADE, enemy_level),
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
    DStaticCast<Dis::World *>(object_layer->OwnerWorld())->RecordCreatedEnemyShip(shade);
    return shade;
}

Revulsion *SpawnRevulsion (
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 const enemy_level)
{
    Revulsion *revulsion = new Revulsion(enemy_level);
    SpawnDynamicSprite(
        object_layer,
        Ship::ShipSpritePath(ET_REVULSION, enemy_level),
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
    DStaticCast<Dis::World *>(object_layer->OwnerWorld())->RecordCreatedEnemyShip(revulsion);
    return revulsion;
}

Devourment *SpawnDevourment (
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 const enemy_level)
{
    Devourment *devourment = new Devourment(enemy_level);
    SpawnDynamicSprite(
        object_layer,
        Ship::ShipSpritePath(ET_DEVOURMENT, enemy_level),
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
    DStaticCast<Dis::World *>(object_layer->OwnerWorld())->RecordCreatedEnemyShip(devourment);
    return devourment;
}

Demi *SpawnDemi (
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 const enemy_level)
{
    Demi *demi = new Demi(enemy_level);
    SpawnDynamicSprite(
        object_layer,
        Ship::ShipSpritePath(ET_DEMI, enemy_level),
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
    DStaticCast<Dis::World *>(object_layer->OwnerWorld())->RecordCreatedEnemyShip(demi);
    return demi;
}

EnemyShip *SpawnEnemyShip (
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    EntityType enemy_type,
    Uint8 enemy_level)
{
    EnemyShip *enemy_ship = NULL;
    switch (enemy_type)
    {
        case ET_INTERLOPER:
            enemy_ship = SpawnInterloper(
                object_layer,
                translation,
                velocity,
                enemy_level);
            break;

        case ET_SHADE:
            enemy_ship = SpawnShade(
                object_layer,
                translation,
                velocity,
                enemy_level);
            break;

        case ET_REVULSION:
            enemy_ship = SpawnRevulsion(
                object_layer,
                translation,
                velocity,
                enemy_level);
            break;

        default:
            ASSERT0(false && "You shouldn't be spawning this type");
            break;
    }
    ASSERT1(enemy_ship != NULL);

    Float angle;
    if (velocity.IsZero())
        angle = Math::RandomAngle();
    else
        angle = Math::Atan(velocity);
    enemy_ship->SetAngle(angle);

    return enemy_ship;
}

HealthTrigger *SpawnDevourmentMouthHealthTrigger (
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
    static std::string const s_grinder_sprite_path[EnemyShip::ENEMY_LEVEL_COUNT] =
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
        object_layer,
        s_grinder_sprite_path[enemy_level],
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
