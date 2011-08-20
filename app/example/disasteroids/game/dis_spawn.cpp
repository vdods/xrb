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
#include "xrb_engine2_objectlayer.hpp"
#include "xrb_engine2_sprite.hpp"
#include "xrb_engine2_world.hpp"

using namespace Xrb;

namespace Dis {

Engine2::Sprite *SpawnDynamicSprite (
    Engine2::ObjectLayer *object_layer,
    std::string const &asset_path,
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
    ASSERT1(!asset_path.empty());
    ASSERT1(entity != NULL);
    ASSERT1(scale_factor >= 0.0f);
    ASSERT1(mass > 0.0f);
    ASSERT1(elasticity >= 0.0f);

    entity->SetElasticity(elasticity);
    entity->SetMass(mass);
    entity->SetVelocity(velocity);
    entity->SetAngularVelocity(angular_velocity);

    Engine2::Sprite *sprite = Engine2::Sprite::CreateAsset(asset_path, current_time);
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
    Engine2::ObjectLayer *object_layer,
    Float current_time,
    FloatVector2 const &translation,
    Float scale_factor,
    Float mass,
    FloatVector2 const &velocity,
    Float mineral_content,
    bool is_a_secondary_asteroid)
{
    Asteroid *asteroid = new Asteroid(mass, mineral_content, is_a_secondary_asteroid);
    SpawnDynamicSprite(
        object_layer,
        "resources/asteroid_small.png",
        current_time,
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
    Engine2::ObjectLayer *object_layer,
    Float current_time,
    FloatVector2 const &translation,
    Float scale_factor,
    Float mass,
    FloatVector2 const &velocity,
    Float impact_damage,
    Float time_to_live,
    Float time_at_birth,
    Uint8 weapon_level,
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
    ballistic->SetImpactEffect(Ballistic::IE_PLASMA_BALL);
    Engine2::Sprite *sprite =
        SpawnDynamicSprite(
            object_layer,
            "resources/plasma_ball_yellow.png",
            current_time,
            Z_DEPTH_BALLISTIC,
            true, // is transparent
            ballistic,
            translation,
            scale_factor,
            Math::Arg(velocity),
            mass,
            velocity,
            0.0f,
            0.0f);
    sprite->SetScaleFactors(FloatVector2(scale_factor, 22.0f / 128.0f * scale_factor));
    // set the physical size ratio to reflect the largest circle that
    // can be inscribed inside the shape of the sprite's bitmap.
    sprite->SetPhysicalSizeRatios(FloatVector2(22.0f / 128.0f, 1.0f));
    return ballistic;
}

Ballistic *SpawnDumbBallistic (
    Engine2::ObjectLayer *object_layer,
    Float current_time,
    FloatVector2 const &translation,
    Float scale_factor,
    Float mass,
    FloatVector2 const &velocity,
    Float impact_damage,
    Float time_to_live,
    Float time_at_birth,
    Uint8 weapon_level,
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
    Engine2::Sprite *sprite =
        SpawnDynamicSprite(
            object_layer,
            "resources/energy_ball.anim",
            current_time,
            Z_DEPTH_BALLISTIC,
            false, // is transparent
            ballistic,
            translation,
            scale_factor,
            Math::Arg(velocity),
            mass,
            velocity,
            0.0f,
            0.0f);
    // so that the physical size of the ballistic isn't larger than the dark
    // red part of the energy ball sprite (see energy_ball_1.png)
    sprite->SetPhysicalSizeRatio(66.0f / 80.0f);
    return ballistic;
}

Grenade *SpawnGrenade (
    Engine2::ObjectLayer *object_layer,
    Float current_time,
    FloatVector2 const &translation,
    Float scale_factor,
    FloatVector2 const &velocity,
    GrenadeLauncher *owner_grenade_launcher,
    Float damage_to_inflict,
    Float damage_radius,
    Float explosion_radius,
    Uint32 weapon_level,
    EntityReference<Entity> const &owner,
    Float health)
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
        "resources/grenade.png",
        current_time,
        Z_DEPTH_SOLID,
        false, // is transparent
        grenade,
        translation,
        scale_factor,
        Math::Arg(velocity),
        Grenade::ms_default_mass,
        velocity,
        Math::RandomFloat(-30.0f, 30.0f),
        0.1f);
    return grenade;
}

Missile *SpawnMissile (
    Engine2::ObjectLayer *object_layer,
    Float current_time,
    FloatVector2 const &translation,
    Float scale_factor,
    Float angle,
    FloatVector2 const &velocity,
    MissileLauncher *owner_missile_launcher,
    Float time_to_live,
    Float time_at_birth,
    Float damage_to_inflict,
    Float damage_radius,
    Float explosion_radius,
    Uint32 weapon_level,
    EntityReference<Entity> const &owner,
    Float health,
    bool is_enemy_missile)
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
            "resources/missile.png",
            current_time,
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
    Engine2::ObjectLayer *object_layer,
    Float current_time,
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
    bool is_enemy_missile)
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
            "resources/missile.png",
            current_time,
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
    Engine2::ObjectLayer *object_layer,
    std::string const &asset_path,
    Float current_time,
    FloatVector2 const &translation,
    Float scale_factor,
    Float mass,
    FloatVector2 const &velocity,
    ItemType item_type)
{
    // mineral is considered a powerup
    ASSERT1((item_type >= IT_POWERUP_LOWEST && item_type <= IT_POWERUP_HIGHEST)
            ||
            (item_type >= IT_MINERAL_LOWEST && item_type <= IT_MINERAL_HIGHEST));
    Powerup *powerup = new Powerup(item_type);
    SpawnDynamicSprite(
        object_layer,
        asset_path,
        current_time,
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
    Engine2::ObjectLayer *object_layer,
    Float current_time,
    FloatVector2 const &translation,
    Float scale_factor,
    Float mass,
    FloatVector2 const &velocity,
    std::string const &asset_path,
    Item *item)
{
    ASSERT1(item != NULL);
    Powerup *powerup = new Powerup(item);
    SpawnDynamicSprite(
        object_layer,
        asset_path,
        current_time,
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

Explosion *SpawnExplosion (
    Engine2::ObjectLayer *object_layer,
    std::string const &asset_path,
    Float current_time,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Float angle,
    Float initial_size,
    Float final_size,
    Float time_to_live,
    Float time_at_birth,
    EntityType entity_type,
    Engine2::Circle::CollisionType collision_type)
{
    Explosion *explosion =
        new Explosion(
            initial_size,
            final_size,
            time_to_live,
            time_at_birth,
            entity_type,
            collision_type);
    SpawnDynamicSprite(
        object_layer,
        asset_path,
        current_time,
        Z_DEPTH_SOLID,
        false, // is transparent
        explosion,
        translation,
        initial_size,
        angle,
        1.0f,
        velocity,
        0.0f,
        0.0f);
    return explosion;
}

DamageExplosion *SpawnDamageExplosion (
    Engine2::ObjectLayer *object_layer,
    std::string const &asset_path,
    Float current_time,
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
        asset_path,
        current_time,
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
    std::string const &asset_path,
    Float current_time,
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
        asset_path,
        current_time,
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
    Float current_time,
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
        "resources/shockwave.png",
        current_time,
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
    Float current_time,
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
        current_time,
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

LaserBeam *SpawnLaserBeam (Engine2::ObjectLayer *object_layer)
{
    ASSERT1(object_layer != NULL);
    ASSERT1(object_layer->OwnerWorld() != NULL);

    Engine2::Sprite *sprite = Engine2::Sprite::Create("resources/laser_beam.png");
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

LaserImpactEffect *SpawnLaserImpactEffect (Engine2::ObjectLayer *object_layer, Float current_time)
{
    ASSERT1(object_layer != NULL);
    ASSERT1(object_layer->OwnerWorld() != NULL);

    Engine2::Sprite *sprite = Engine2::Sprite::CreateAsset("resources/laser_impact.anim", current_time);
    // setting the scale factor this large helps with speed in adding it to
    // the quad tree, as the first time is temporary.  Laser will place
    // it for real later.
    sprite->SetZDepth(Z_DEPTH_LASER_IMPACT_EFFECT);
    sprite->SetIsTransparent(true);
    sprite->SetScaleFactor(0.5f * object_layer->SideLength());

    LaserImpactEffect *laser_impact_effect = new LaserImpactEffect();

    sprite->SetEntity(laser_impact_effect);
    object_layer->OwnerWorld()->AddDynamicObject(sprite, object_layer);

    return laser_impact_effect;
}

void SpawnSplashImpactEffect (
    Engine2::ObjectLayer *object_layer,
    std::string const &asset_path,
    Float current_time,
    FloatVector2 const &location,
    FloatVector2 const &direction,
    FloatVector2 const &base_velocity,
    Float seed_angle,
    Float seed_radius,
    Uint32 particle_count,
    Float particle_spread_angle,
    Float particle_time_to_live,
    Float particle_speed_proportion)
{
    ASSERT1(object_layer != NULL);
    ASSERT1(object_layer->OwnerWorld() != NULL);
    ASSERT1(direction.LengthSquared() > 0.0f);
    ASSERT1(particle_spread_angle >= 0.0f);
    ASSERT1(particle_time_to_live > 0.0f);
    ASSERT1(particle_speed_proportion >= 0.0f);

    // spawn particles emanating from location the angle range [-particle_spread_angle, particle_spread_angle]
    // proportion of the physical radius of the ballistic itself that the particles will be sized to.
    static Float const s_particle_radius_min_factor = 0.2f;//0.75f;
    static Float const s_particle_radius_max_factor = 1.5f;//1.5f;
    // particle_speed_proportion is particle speed in diameters per lifetime

    for (Uint32 i = 0; i < particle_count; ++i)
    {
        Float angle = seed_angle +
                      Math::Arg(direction) +
                      Math::LinearlyInterpolate(-particle_spread_angle, particle_spread_angle, 0, particle_count-1, i);
        Float radius = Math::RandomFloat(s_particle_radius_min_factor, s_particle_radius_max_factor) * seed_radius;
        Float speed = 2.0f * radius * particle_speed_proportion / particle_time_to_live;
        NoDamageExplosion *explosion =
            SpawnNoDamageExplosion(
                object_layer,
                asset_path,
                current_time,
                location,
                base_velocity + speed * Math::UnitVector(angle),
                radius,
                radius,
                particle_time_to_live,
                current_time);
        explosion->SetScaleInterpolationPower(0.5f); // so it grows quickly at first and then slowly
        explosion->SetColorMaskInterpolationPower(2.0f);
        explosion->OwnerObject()->SetZDepth(Z_DEPTH_SPLASH_IMPACT);
    }
}

void SpawnGaussGunTrail (
    Engine2::ObjectLayer *object_layer,
    std::string const &asset_path,
    Float current_time,
    FloatVector2 const &trail_start,
    FloatVector2 trail_direction,
    FloatVector2 const &trail_velocity,
    Float segment_width,
    Float segment_length,
    Uint32 segment_count,
    Float time_to_live,
    Float time_at_birth)
{
    ASSERT1(object_layer != NULL);
    ASSERT1(object_layer->OwnerWorld() != NULL);
    ASSERT1(segment_width > 0.0f);
    ASSERT1(segment_length >= 0.0f);
    ASSERT1(time_to_live > 0.0f);

    if (segment_length == 0.0f)
        return; // nothing to do

    trail_direction.Normalize();

    for (Uint32 i = 0; i < segment_count; ++i)
    {
        FloatVector2 trail_center = trail_start + (0.5f + i) * segment_length * trail_direction;

        Engine2::Sprite *sprite = Engine2::Sprite::CreateAsset(asset_path, current_time);
        sprite->SetZDepth(Z_DEPTH_GAUSS_GUN_TRAIL);
        sprite->SetIsTransparent(true);
        sprite->SetTranslation(trail_center);
        sprite->SetScaleFactors(FloatVector2(0.5f * segment_length, 0.5f * segment_width));
        sprite->SetAngle(Math::Arg(trail_direction));

        GaussGunTrail *gauss_gun_trail = new GaussGunTrail(time_to_live, time_at_birth);
        gauss_gun_trail->SetVelocity(trail_velocity);

        sprite->SetEntity(gauss_gun_trail);
        object_layer->OwnerWorld()->AddDynamicObject(sprite, object_layer);
    }
}

TractorBeam *SpawnTractorBeam (Engine2::ObjectLayer *object_layer)
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

ShieldEffect *SpawnShieldEffect (Engine2::ObjectLayer *object_layer, Float current_time)
{
    ASSERT1(object_layer != NULL);
    ASSERT1(object_layer->OwnerWorld() != NULL);

    Engine2::Sprite *sprite = Engine2::Sprite::CreateAsset("resources/shield.anim", current_time);
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

LightningEffect *SpawnLightningEffect (Engine2::ObjectLayer *object_layer, Float current_time)
{
    ASSERT1(object_layer != NULL);
    ASSERT1(object_layer->OwnerWorld() != NULL);

    Engine2::Sprite *sprite = Engine2::Sprite::CreateAsset("resources/lightning.anim", current_time);
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

ReticleEffect *SpawnReticleEffect (Engine2::ObjectLayer *object_layer, Color const &color_mask)
{
    ASSERT1(object_layer != NULL);
    ASSERT1(object_layer->OwnerWorld() != NULL);

    Engine2::Sprite *sprite = Engine2::Sprite::Create("resources/reticle1.png");
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
    Engine2::ObjectLayer *object_layer,
    Float current_time,
    FloatVector2 const &translation,
    Float scale_factor,
    FloatVector2 const &velocity,
    Float health_delta_rate,
    Mortal::DamageType damage_type,
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
            current_time,
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
    Engine2::ObjectLayer *object_layer,
    Float current_time,
    FloatVector2 const &translation,
    FloatVector2 const &velocity)
{
    Solitary *solitary = new Solitary();
    SpawnDynamicSprite(
        object_layer,
        Ship::ShipSpritePath(ET_SOLITARY, 0),
        current_time,
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
    Engine2::ObjectLayer *object_layer,
    Float current_time,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 enemy_level)
{
    Interloper *interloper = new Interloper(enemy_level);
    SpawnDynamicSprite(
        object_layer,
        Ship::ShipSpritePath(ET_INTERLOPER, enemy_level),
        current_time,
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
    Engine2::ObjectLayer *object_layer,
    Float current_time,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 enemy_level)
{
    Shade *shade = new Shade(enemy_level);
    SpawnDynamicSprite(
        object_layer,
        Ship::ShipSpritePath(ET_SHADE, enemy_level),
        current_time,
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
    Engine2::ObjectLayer *object_layer,
    Float current_time,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 enemy_level)
{
    Revulsion *revulsion = new Revulsion(enemy_level);
    SpawnDynamicSprite(
        object_layer,
        Ship::ShipSpritePath(ET_REVULSION, enemy_level),
        current_time,
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
    Engine2::ObjectLayer *object_layer,
    Float current_time,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 enemy_level)
{
    Devourment *devourment = new Devourment(enemy_level);
    SpawnDynamicSprite(
        object_layer,
        Ship::ShipSpritePath(ET_DEVOURMENT, enemy_level),
        current_time,
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
    Engine2::ObjectLayer *object_layer,
    Float current_time,
    FloatVector2 const &translation,
    FloatVector2 const &velocity,
    Uint8 enemy_level)
{
    Demi *demi = new Demi(enemy_level);
    SpawnDynamicSprite(
        object_layer,
        Ship::ShipSpritePath(ET_DEMI, enemy_level),
        current_time,
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
    Float current_time,
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
                current_time,
                translation,
                velocity,
                enemy_level);
            break;

        case ET_SHADE:
            enemy_ship = SpawnShade(
                object_layer,
                current_time,
                translation,
                velocity,
                enemy_level);
            break;

        case ET_REVULSION:
            enemy_ship = SpawnRevulsion(
                object_layer,
                current_time,
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
        angle = Math::Arg(velocity);
    enemy_ship->SetAngle(angle);

    return enemy_ship;
}

HealthTrigger *SpawnDevourmentMouthHealthTrigger (
    Engine2::ObjectLayer *object_layer,
    Float current_time,
    FloatVector2 const &translation,
    Float scale_factor,
    FloatVector2 const &velocity,
    Float health_delta_rate,
    Mortal::DamageType damage_type,
    EntityReference<Mortal> const &ignore_this_mortal,
    EntityReference<Entity> const &owner,
    Uint8 enemy_level)
{
    static std::string const s_grinder_asset_path[EnemyShip::ENEMY_LEVEL_COUNT] =
    {
        "resources/grinder_0.anim",
        "resources/grinder_1.anim",
        "resources/grinder_2.anim",
        "resources/grinder_3.anim"
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
        s_grinder_asset_path[enemy_level],
        current_time,
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
