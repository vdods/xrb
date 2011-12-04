// ///////////////////////////////////////////////////////////////////////////
// dis_demi.cpp by Victor Dods, created 2006/05/07
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_demi.hpp"

#include "dis_ballistic.hpp"
#include "dis_effect.hpp"
#include "dis_explosive.hpp"
#include "dis_powerup.hpp"
#include "dis_spawn.hpp"
#include "dis_weapon.hpp"
#include "dis_world.hpp"
#include "xrb_engine2_circle_physicshandler.hpp"
#include "xrb_engine2_objectlayer.hpp"

using namespace Xrb;

#define THINK_STATE(x) &Demi::x

namespace Dis {

Float const Demi::ms_max_health[ENEMY_LEVEL_COUNT] = { 1500.0f, 3000.0f, 6000.0f, 12000.0f };
Float const Demi::ms_engine_thrust[ENEMY_LEVEL_COUNT] = { 500000.0f, 700000.0f, 925000.0f, 1200000.0f };
Float const Demi::ms_max_angular_velocity[ENEMY_LEVEL_COUNT] = { 90.0f, 90.0f, 90.0f, 90.0f };
Float const Demi::ms_ship_radius[ENEMY_LEVEL_COUNT] = { 55.0f, 65.0f, 75.0f, 85.0f };
Float const Demi::ms_baseline_mass[ENEMY_LEVEL_COUNT] = { 10000.0f, 14000.0f, 18500.0f, 24000.0f };
Float const Demi::ms_damage_dissipation_rate[ENEMY_LEVEL_COUNT] = { 4.0f, 8.0f, 12.0f, 16.0f };
Float const Demi::ms_wander_speed[ENEMY_LEVEL_COUNT] = { 30.0f, 40.0f, 50.0f, 60.0f };
Float const Demi::ms_weapon_fov[ENEMY_LEVEL_COUNT] = { 60.0f, 60.0f, 60.0f, 60.0f };
Time::Delta const Demi::ms_spinning_attack_acceleration_duration[ENEMY_LEVEL_COUNT] = { 0.75f, 0.75f, 0.75f, 0.75f };
Time::Delta const Demi::ms_spinning_attack_duration[ENEMY_LEVEL_COUNT] = { 2.0f, 2.0f, 2.0f, 2.0f };
Time::Delta const Demi::ms_slow_bullet_gun_spam_duration[ENEMY_LEVEL_COUNT] = { 6.0f, 6.0f, 6.0f, 6.0f };
Float const Demi::ms_slow_bullet_gun_range[ENEMY_LEVEL_COUNT] = { 1000.0f, 1000.0f, 1000.0f, 1000.0f };
Float const Demi::ms_slow_bullet_gun_fire_rate[ENEMY_LEVEL_COUNT] = { 12.0f, 16.0f, 20.0f, 24.0f };
Float const Demi::ms_flame_thrower_max_damage_per_fireball[ENEMY_LEVEL_COUNT] = { 10.0f, 20.0f, 40.0f, 80.0f };
Float const Demi::ms_flame_thrower_final_fireball_size[ENEMY_LEVEL_COUNT] = { 70.0f, 80.0f, 90.0f, 100.0f };
Float const Demi::ms_gauss_gun_impact_damage[ENEMY_LEVEL_COUNT] = { 20.0f, 40.0f, 80.0f, 160.0f };
Float const Demi::ms_gauss_gun_aim_error_radius[ENEMY_LEVEL_COUNT] = { 25.0f, 20.0f, 15.0f, 10.0f };
Float const Demi::ms_gauss_gun_aim_max_speed[ENEMY_LEVEL_COUNT] = { 150.0f, 150.0f, 150.0f, 150.0f };
Float const Demi::ms_gauss_gun_reticle_scale_factor[ENEMY_LEVEL_COUNT] = { 20.0f, 20.0f, 20.0f, 20.0f };
Time::Delta const Demi::ms_gauss_gun_max_duration[ENEMY_LEVEL_COUNT] = { 7.0f, 7.0f, 7.0f, 7.0f };
Time::Delta const Demi::ms_flame_throw_sweep_duration[ENEMY_LEVEL_COUNT] = { 2.0f, 2.0f, 2.0f, 2.0f };
Time::Delta const Demi::ms_flame_throw_blast_duration[ENEMY_LEVEL_COUNT] = { 0.15f, 0.15f, 0.15f, 0.15f };
Time::Delta const Demi::ms_missile_launch_duration[ENEMY_LEVEL_COUNT] = { 1.0f, 1.0f, 1.0f, 1.0f };
Time::Delta const Demi::ms_enemy_spawn_blast_duration[ENEMY_LEVEL_COUNT] = { 2.0f, 2.0f, 2.0f, 2.0f };
Time::Delta const Demi::ms_tractor_target_closer_duration[ENEMY_LEVEL_COUNT] = { 1.0f, 1.0f, 1.0f, 1.0f };
Float const Demi::ms_tractor_range[ENEMY_LEVEL_COUNT] = { 20000.0f, 20000.0f, 20000.0f, 20000.0f };
Float const Demi::ms_tractor_strength[ENEMY_LEVEL_COUNT] = { 2000.0f, 3000.0f, 6000.0f, 12000.0f };
Float const Demi::ms_tractor_max_force[ENEMY_LEVEL_COUNT] = { 2000000.0f, 2000000.0f, 2000000.0f, 2000000.0f };
Float const Demi::ms_tractor_beam_radius[ENEMY_LEVEL_COUNT] = { 80.0f, 100.0f, 120.0f, 140.0f };
Float const Demi::ms_target_near_range_distance[ENEMY_LEVEL_COUNT] = { 200.0f, 210.0f, 220.0f, 230.0f };
Float const Demi::ms_target_mid_range_distance[ENEMY_LEVEL_COUNT] = { 350.0f, 360.0f, 370.0f, 380.0f };
Time::Delta const Demi::ms_pause_duration[ENEMY_LEVEL_COUNT] = { 2.0f, 1.5f, 1.25f, 1.0f };
Float const Demi::ms_health_powerup_amount_to_spawn[ENEMY_LEVEL_COUNT] = { 20.0f, 20.0f, 20.0f, 20.0f };
Float const Demi::ms_option_powerup_spawn_density_min[ENEMY_LEVEL_COUNT] = { 1.0f, 1.2f, 1.4f, 1.8f };
Float const Demi::ms_option_powerup_spawn_density_max[ENEMY_LEVEL_COUNT] = { 1.6f, 1.8f, 2.2f, 2.7f };

Float const Demi::ms_side_port_angle = 64.7f;

Demi::Demi (Uint8 const enemy_level)
    :
    EnemyShip(enemy_level, ms_max_health[enemy_level], ET_DEMI),
    m_port_reticle_coordinates(FloatVector2::ms_zero),
    m_starboard_reticle_coordinates(FloatVector2::ms_zero)
{
    // PlayerShip needs to be more sensitive to damage with respect
    // to damage/healing flashes.  the default is 0.2, but we need
    // something much lower.
    SetFullFlashIntensityHealthRatio(0.01f);

    m_think_state = THINK_STATE(PickWanderDirection);
    m_port_tractor_think_state = THINK_STATE(PortTractorDeflectStuff);
    m_starboard_tractor_think_state = THINK_STATE(StarboardTractorDeflectStuff);

    m_spinning_attack_uses_secondary_fire = false;
    m_spinning_attack_finish_state = NULL;

    m_main_weapon = NULL;
    m_port_weapon = NULL;
    m_starboard_weapon = NULL;
    m_aft_weapon = NULL;

    // main weapon setup
    {
        // the gauss gun weapon level is only used to indicate
        // weapon range and fire rate
        m_gauss_gun = new GaussGun(3);
        m_gauss_gun->SetImpactDamageOverride(ms_gauss_gun_impact_damage[EnemyLevel()]);
        m_gauss_gun->Equip(this);

        m_flame_thrower = new FlameThrower(3);
        m_flame_thrower->SetMaxDamagePerFireballOverride(ms_flame_thrower_max_damage_per_fireball[EnemyLevel()]);
        m_flame_thrower->SetFinalFireballSizeOverride(ms_flame_thrower_final_fireball_size[EnemyLevel()]);
        m_flame_thrower->Equip(this);

        // TODO: overrides for missile launcher damage/etc
        m_missile_launcher = new MissileLauncher(0);
        m_missile_launcher->SetSpawnEnemyMissiles(true);
        m_missile_launcher->Equip(this);

        m_slow_bullet_gun = new SlowBulletGun(EnemyLevel());
        m_slow_bullet_gun->SetRangeOverride(ms_slow_bullet_gun_range[EnemyLevel()]);
        m_slow_bullet_gun->SetFireRateOverride(ms_slow_bullet_gun_fire_rate[EnemyLevel()]);
        m_slow_bullet_gun->Equip(this);
    }

    // port-side weapon setup
    {
        m_port_tractor = new AdvancedTractor(3);
        m_port_tractor->SetRangeOverride(ms_tractor_range[EnemyLevel()]);
        m_port_tractor->SetStrengthOverride(ms_tractor_strength[EnemyLevel()]);
        m_port_tractor->SetMaxForceOverride(ms_tractor_max_force[EnemyLevel()]);
        m_port_tractor->SetBeamRadiusOverride(ms_tractor_beam_radius[EnemyLevel()]);
        m_port_tractor->Equip(this);

        m_port_flame_thrower = new FlameThrower(3);
        m_port_flame_thrower->SetMaxDamagePerFireballOverride(ms_flame_thrower_max_damage_per_fireball[EnemyLevel()]);
        m_port_flame_thrower->SetFinalFireballSizeOverride(ms_flame_thrower_final_fireball_size[EnemyLevel()]);
        m_port_flame_thrower->Equip(this);

        m_port_missile_launcher = new MissileLauncher(0);
        m_port_missile_launcher->SetSpawnEnemyMissiles(true);
        m_port_missile_launcher->Equip(this);

        m_port_slow_bullet_gun = new SlowBulletGun(EnemyLevel());
        m_port_slow_bullet_gun->SetRangeOverride(ms_slow_bullet_gun_range[EnemyLevel()]);
        m_port_slow_bullet_gun->SetFireRateOverride(ms_slow_bullet_gun_fire_rate[EnemyLevel()]);
        m_port_slow_bullet_gun->Equip(this);
    }

    // starboard-side weapon setup
    {
        m_starboard_tractor = new AdvancedTractor(3);
        m_starboard_tractor->SetRangeOverride(ms_tractor_range[EnemyLevel()]);
        m_starboard_tractor->SetStrengthOverride(ms_tractor_strength[EnemyLevel()]);
        m_starboard_tractor->SetMaxForceOverride(ms_tractor_max_force[EnemyLevel()]);
        m_starboard_tractor->SetBeamRadiusOverride(ms_tractor_beam_radius[EnemyLevel()]);
        m_starboard_tractor->Equip(this);

        m_starboard_flame_thrower = new FlameThrower(3);
        m_starboard_flame_thrower->SetMaxDamagePerFireballOverride(ms_flame_thrower_max_damage_per_fireball[EnemyLevel()]);
        m_starboard_flame_thrower->SetFinalFireballSizeOverride(ms_flame_thrower_final_fireball_size[EnemyLevel()]);
        m_starboard_flame_thrower->Equip(this);

        m_starboard_missile_launcher = new MissileLauncher(0);
        m_starboard_missile_launcher->SetSpawnEnemyMissiles(true);
        m_starboard_missile_launcher->Equip(this);

        m_starboard_slow_bullet_gun = new SlowBulletGun(EnemyLevel());
        m_starboard_slow_bullet_gun->SetRangeOverride(ms_slow_bullet_gun_range[EnemyLevel()]);
        m_starboard_slow_bullet_gun->SetFireRateOverride(ms_slow_bullet_gun_fire_rate[EnemyLevel()]);
        m_starboard_slow_bullet_gun->Equip(this);
    }

    // aft-port weapon setup
    {
        m_aft_enemy_spawner = new EnemySpawner(EnemyLevel());
        m_aft_enemy_spawner->Equip(this);

        m_aft_flame_thrower = new FlameThrower(3);
        m_aft_flame_thrower->SetMaxDamagePerFireballOverride(ms_flame_thrower_max_damage_per_fireball[EnemyLevel()]);
        m_aft_flame_thrower->SetFinalFireballSizeOverride(ms_flame_thrower_final_fireball_size[EnemyLevel()]);
        m_aft_flame_thrower->Equip(this);

        m_aft_missile_launcher = new MissileLauncher(0);
        m_aft_missile_launcher->SetSpawnEnemyMissiles(true);
        m_aft_missile_launcher->Equip(this);

        m_aft_slow_bullet_gun = new SlowBulletGun(EnemyLevel());
        m_aft_slow_bullet_gun->SetRangeOverride(ms_slow_bullet_gun_range[EnemyLevel()]);
        m_aft_slow_bullet_gun->SetFireRateOverride(ms_slow_bullet_gun_fire_rate[EnemyLevel()]);
        m_aft_slow_bullet_gun->Equip(this);
    }

    SetStrength(D_MINING_LASER);
    SetImmunity(D_COLLISION);
    SetDamageDissipationRate(ms_damage_dissipation_rate[EnemyLevel()]);

    Demi::ResetInputs();
}

Demi::~Demi ()
{
    // delete the main weapons
    ASSERT1(m_gauss_gun != NULL);
    DeleteAndNullify(m_gauss_gun);
    ASSERT1(m_flame_thrower != NULL);
    DeleteAndNullify(m_flame_thrower);
    ASSERT1(m_missile_launcher != NULL);
    DeleteAndNullify(m_missile_launcher);
    ASSERT1(m_slow_bullet_gun != NULL);
    DeleteAndNullify(m_slow_bullet_gun);
    if (m_reticle_effect.IsValid())
    {
        if (m_reticle_effect->IsInWorld())
            m_reticle_effect->RemoveFromWorld();
        delete m_reticle_effect->OwnerObject();
    }

    // delete the port weapons
    ASSERT1(m_port_tractor != NULL);
    DeleteAndNullify(m_port_tractor);
    ASSERT1(m_port_flame_thrower != NULL);
    DeleteAndNullify(m_port_flame_thrower);
    ASSERT1(m_port_missile_launcher != NULL);
    DeleteAndNullify(m_port_missile_launcher);
    ASSERT1(m_port_slow_bullet_gun != NULL);
    DeleteAndNullify(m_port_slow_bullet_gun);
    if (m_port_tractor_beam.IsValid())
    {
        if (m_port_tractor_beam->IsInWorld())
            m_port_tractor_beam->RemoveFromWorld();
        delete m_port_tractor_beam->OwnerObject();
    }

    // delete the starboard weapons
    ASSERT1(m_starboard_tractor != NULL);
    DeleteAndNullify(m_starboard_tractor);
    ASSERT1(m_starboard_flame_thrower != NULL);
    DeleteAndNullify(m_starboard_flame_thrower);
    ASSERT1(m_starboard_missile_launcher != NULL);
    DeleteAndNullify(m_starboard_missile_launcher);
    ASSERT1(m_starboard_slow_bullet_gun != NULL);
    DeleteAndNullify(m_starboard_slow_bullet_gun);
    if (m_starboard_tractor_beam.IsValid())
    {
        if (m_starboard_tractor_beam->IsInWorld())
            m_starboard_tractor_beam->RemoveFromWorld();
        delete m_starboard_tractor_beam->OwnerObject();
    }

    // delete the aft weapons
    ASSERT1(m_aft_enemy_spawner != NULL);
    DeleteAndNullify(m_aft_enemy_spawner);
    ASSERT1(m_aft_flame_thrower != NULL);
    DeleteAndNullify(m_aft_flame_thrower);
    ASSERT1(m_aft_missile_launcher != NULL);
    DeleteAndNullify(m_aft_missile_launcher);
    ASSERT1(m_aft_slow_bullet_gun != NULL);
    DeleteAndNullify(m_aft_slow_bullet_gun);
}

void Demi::Think (Time time, Time::Delta frame_dt)
{
    // can't think if we're dead.
    if (IsDead())
        return;

    // Demi can't be disabled.  this call must happen before EnemyShip::Think
    CancelDisableTime();

    EnemyShip::Think(time, frame_dt);

    // ensure the port tractor beam is allocated (lazy allocation)
    if (!m_port_tractor_beam.IsValid())
        m_port_tractor_beam = SpawnTractorBeam(GetObjectLayer())->GetReference();
    // if the port tractor beam is already allocated but not in the world, re-add it.
    else if (!m_port_tractor_beam->IsInWorld())
        m_port_tractor_beam->AddBackIntoWorld();
    // set the port tractor beam effect in the Tractor weapon
    m_port_tractor->SetTractorBeam(*m_port_tractor_beam);

    // ensure the starboard tractor beam is allocated (lazy allocation)
    if (!m_starboard_tractor_beam.IsValid())
        m_starboard_tractor_beam = SpawnTractorBeam(GetObjectLayer())->GetReference();
    // if the starboard tractor beam is already allocated but not in the world, re-add it.
    else if (!m_starboard_tractor_beam->IsInWorld())
        m_starboard_tractor_beam->AddBackIntoWorld();
    // set the starboard tractor beam effect in the Tractor weapon
    m_starboard_tractor->SetTractorBeam(*m_starboard_tractor_beam);

    // call the main think state function (which will set the inputs)
    (this->*m_think_state)(time, frame_dt);
    // call the port/starboard think state functions (if they're not NULL)
    if (m_port_tractor_think_state != NULL)
        (this->*m_port_tractor_think_state)(time, frame_dt);
    if (m_starboard_tractor_think_state != NULL)
        (this->*m_starboard_tractor_think_state)(time, frame_dt);

    // since enemy ships do not use the PlayerShip device code, engines
    // weapons, etc must be activated/simulated manually here.

    if (AngularVelocity() == 0.0f)
    {
        if (m_target.IsValid())
            AimShipAtCoordinates(m_target->Translation(), frame_dt);
        else if (Velocity().LengthSquared() > 0.001f)
            AimShipAtCoordinates(Translation() + Velocity(), frame_dt);
    }

    // set the main weapon inputs and activate
    if (m_main_weapon != NULL)
    {
        m_main_weapon->SetInputs(
            NormalizedWeaponPrimaryInput(),
            NormalizedWeaponSecondaryInput(),
            MuzzleLocation(m_main_weapon),
            MuzzleDirection(m_main_weapon),
            ReticleCoordinates());
        m_main_weapon->Activate(
            m_main_weapon->PowerToBeUsedBasedOnInputs(false, false, time, frame_dt),
            false, // no attack boost
            false, // no defense boost
            time,
            frame_dt);
    }

    // set the port weapon inputs and activate
    if (m_port_weapon != NULL)
    {
        m_port_weapon->SetInputs(
            NormalizedPortWeaponPrimaryInput(),
            NormalizedPortWeaponSecondaryInput(),
            MuzzleLocation(m_port_weapon),
            MuzzleDirection(m_port_weapon),
            m_port_reticle_coordinates);
        m_port_weapon->Activate(
            m_port_weapon->PowerToBeUsedBasedOnInputs(false, false, time, frame_dt),
            false, // no attack boost
            false, // no defense boost
            time,
            frame_dt);
    }
    // remove the port tractor beam from the world if necessary
    if (m_port_weapon != m_port_tractor &&
        m_port_tractor_beam.IsValid() &&
        m_port_tractor_beam->IsInWorld())
    {
        m_port_tractor_beam->ScheduleForRemovalFromWorld(0.0f);
    }

    // set the starboard weapon inputs and activate
    if (m_starboard_weapon != NULL)
    {
        m_starboard_weapon->SetInputs(
            NormalizedStarboardWeaponPrimaryInput(),
            NormalizedStarboardWeaponSecondaryInput(),
            MuzzleLocation(m_starboard_weapon),
            MuzzleDirection(m_starboard_weapon),
            m_starboard_reticle_coordinates);
        m_starboard_weapon->Activate(
            m_starboard_weapon->PowerToBeUsedBasedOnInputs(false, false, time, frame_dt),
            false, // no attack boost
            false, // no defense boost
            time,
            frame_dt);
    }
    // remove the starboard tractor beam from the world if necessary
    if (m_starboard_weapon != m_starboard_tractor &&
        m_starboard_tractor_beam.IsValid() &&
        m_starboard_tractor_beam->IsInWorld())
    {
        m_starboard_tractor_beam->ScheduleForRemovalFromWorld(0.0f);
    }

    // set the aft weapon inputs and activate
    if (m_aft_weapon != NULL)
    {
        m_aft_weapon->SetInputs(
            NormalizedAftWeaponPrimaryInput(),
            NormalizedAftWeaponSecondaryInput(),
            MuzzleLocation(m_aft_weapon),
            MuzzleDirection(m_aft_weapon),
            FloatVector2::ms_zero); // reticle coords don't matter here
        m_aft_weapon->Activate(
            m_aft_weapon->PowerToBeUsedBasedOnInputs(false, false, time, frame_dt),
            false, // no attack boost
            false, // no defense boost
            time,
            frame_dt);
    }

    ResetInputs();
}

void Demi::Die (
    Entity *killer,
    Entity *kill_medium,
    FloatVector2 const &kill_location,
    FloatVector2 const &kill_normal,
    Float kill_force,
    DamageType kill_type,
    Time time,
    Time::Delta frame_dt)
{
    EnemyShip::Die(
        killer,
        kill_medium,
        kill_location,
        kill_normal,
        kill_force,
        kill_type,
        time,
        frame_dt);

    // spawn health powerups
    static Float const s_min_powerup_amount = 1.0f;
    static Float const s_max_powerup_amount = 3.0f;
    static Float const s_powerup_ejection_speed = 100.0f;

    static Float const s_health_powerup_coefficient = 15.0f;

    Float health_powerup_amount_left_to_spawn = ms_health_powerup_amount_to_spawn[EnemyLevel()];
    while (health_powerup_amount_left_to_spawn > s_min_powerup_amount)
    {
        Float health_powerup_amount =
            Math::RandomFloat(
                s_min_powerup_amount,
                Min(s_max_powerup_amount, health_powerup_amount_left_to_spawn));
        Float mass = health_powerup_amount * s_health_powerup_coefficient;
        Float scale_factor = Math::Sqrt(mass);
        Float velocity_angle = Math::RandomAngle();
        Float velocity_ratio = Math::RandomFloat(scale_factor, 0.5f * VisibleRadius()) / (0.5f * VisibleRadius());
        FloatVector2 velocity = Velocity() + s_powerup_ejection_speed * velocity_ratio * Math::UnitVector(velocity_angle);

        Powerup *health_powerup =
            SpawnPowerup(
                GetObjectLayer(),
                "fs://health_powerup.anim",
                time,
                Translation() + 0.5f * VisibleRadius() * velocity_ratio * Math::UnitVector(velocity_angle),
                scale_factor,
                mass,
                velocity,
                IT_POWERUP_HEALTH);
        health_powerup->SetEffectiveValue(health_powerup_amount);

        health_powerup_amount_left_to_spawn -= health_powerup_amount;
    }

    Uint32 option_powerup_spawn_count = Uint32(Math::Round(Math::RandomFloat(ms_option_powerup_spawn_density_min[EnemyLevel()], ms_option_powerup_spawn_density_max[EnemyLevel()])));
    while (option_powerup_spawn_count > 0)
    {
        Float velocity_angle = Math::RandomAngle();
        Float velocity_ratio = Math::RandomFloat(Powerup::ms_radius, 0.5f * PhysicalRadius()) / (0.5f * PhysicalRadius());
        FloatVector2 velocity = Velocity() + s_powerup_ejection_speed * velocity_ratio * Math::UnitVector(velocity_angle);

        SpawnPowerup(
            GetObjectLayer(),
            "fs://option_powerup.anim",
            time,
            Translation() + 0.5f * VisibleRadius() * velocity_ratio * Math::UnitVector(velocity_angle),
            Powerup::ms_radius,
            Sqr(Powerup::ms_radius),
            velocity,
            IT_POWERUP_OPTION);

        --option_powerup_spawn_count;
    }

    static Uint32 const s_death_enemies_to_spawn_count = 10;
    for (Uint32 i = 0; i < s_death_enemies_to_spawn_count; ++i)
    {
        Float velocity_angle = Math::RandomAngle();
        Float velocity_ratio = Math::RandomFloat(0.0f, 1.0f);
        FloatVector2 velocity = Velocity() + s_powerup_ejection_speed * velocity_ratio * Math::UnitVector(velocity_angle);

        SpawnEnemyShip(
            GetObjectLayer(),
            time,
            Translation() + 0.5f * VisibleRadius() * velocity_ratio * Math::UnitVector(velocity_angle),
            velocity,
            EntityType(Math::RandomUint16(ET_INTERLOPER, ET_REVULSION)), // relies on ET_INTERLOPER, ET_SHADE and
            Math::RandomUint16(0, EnemyLevel()));                        // ET_REVULSION being sequential enums
    }

    // remove the port tractor beam, if it exists
    if (m_port_tractor_beam.IsValid() && m_port_tractor_beam->IsInWorld())
        m_port_tractor_beam->ScheduleForRemovalFromWorld(0.0f);

    // remove the starboard tractor beam, if it exists
    if (m_starboard_tractor_beam.IsValid() && m_starboard_tractor_beam->IsInWorld())
        m_starboard_tractor_beam->ScheduleForRemovalFromWorld(0.0f);
}

FloatVector2 Demi::MuzzleLocation (Weapon const *weapon) const
{
    ASSERT1(weapon != NULL);
    if (weapon == m_gauss_gun || weapon == m_flame_thrower || weapon == m_missile_launcher || weapon == m_slow_bullet_gun)
    {
        return EnemyShip::MuzzleLocation(weapon);
    }
    else if (weapon == m_port_tractor || weapon == m_port_flame_thrower || weapon == m_port_missile_launcher || weapon == m_port_slow_bullet_gun)
    {
        return Translation() + VisibleRadius() * Math::UnitVector(Angle() + ms_side_port_angle);
    }
    else if (weapon == m_starboard_tractor || weapon == m_starboard_flame_thrower || weapon == m_starboard_missile_launcher || weapon == m_starboard_slow_bullet_gun)
    {
        return Translation() + VisibleRadius() * Math::UnitVector(Angle() - ms_side_port_angle);
    }
    else if (weapon == m_aft_enemy_spawner || weapon == m_aft_flame_thrower || weapon == m_aft_missile_launcher || weapon == m_aft_slow_bullet_gun)
    {
        return Translation() - VisibleRadius() * Math::UnitVector(Angle());
    }
    else
    {
        ASSERT1(false && "Unknown weapon");
        return FloatVector2::ms_zero;
    }
}

FloatVector2 Demi::MuzzleDirection (Weapon const *weapon) const
{
    if (weapon == m_gauss_gun || weapon == m_flame_thrower || weapon == m_missile_launcher || weapon == m_slow_bullet_gun)
    {
        FloatVector2 reticle_offset(GetObjectLayer()->AdjustedDifference(ReticleCoordinates(), MuzzleLocation(weapon)));
        if (reticle_offset.Length() < 0.01f)
            return Math::UnitVector(Angle());

        Float reticle_angle = Math::CanonicalAngle(Math::Arg(reticle_offset) - Angle());
        if (reticle_angle < -ms_weapon_fov[EnemyLevel()])
            reticle_angle = -ms_weapon_fov[EnemyLevel()];
        else if (reticle_angle > ms_weapon_fov[EnemyLevel()])
            reticle_angle = ms_weapon_fov[EnemyLevel()];
        reticle_angle += Angle();

        return Math::UnitVector(reticle_angle);
    }
    else if (weapon == m_port_tractor || weapon == m_port_flame_thrower || weapon == m_port_missile_launcher || weapon == m_port_slow_bullet_gun)
    {
        return Math::UnitVector(Angle() + ms_side_port_angle);
    }
    else if (weapon == m_starboard_tractor || weapon == m_starboard_flame_thrower || weapon == m_starboard_missile_launcher || weapon == m_starboard_slow_bullet_gun)
    {
        return Math::UnitVector(Angle() - ms_side_port_angle);
    }
    else if (weapon == m_aft_enemy_spawner || weapon == m_aft_flame_thrower || weapon == m_aft_missile_launcher || weapon == m_aft_slow_bullet_gun)
    {
        return Math::UnitVector(Angle() + 180.0f);
    }
    else
    {
        ASSERT1(false && "Unknown weapon");
        return FloatVector2::ms_zero;
    }
}

bool Demi::TakePowerup (Powerup *powerup, Time time, Time::Delta frame_dt)
{
    ASSERT1(powerup != NULL);

    // just suck up all powerups, to piss off the player

    // health powerups heal
    if (powerup->GetItemType() == IT_POWERUP_HEALTH)
    {
        ASSERT1(powerup->GetItem() == NULL);
        Heal(
            powerup,
            powerup,
            powerup->EffectiveValue(),
            (Mass()*powerup->Translation() + powerup->Mass()*Translation()) /
                (Mass() + powerup->Mass()),
            (Translation() - powerup->Translation()).Normalization(),
            0.0f,
            time,
            frame_dt);
    }

    delete powerup->GetItem();
    powerup->ClearItem();
    return true;
}

void Demi::SetTarget (Mortal *target)
{
    if (target == NULL)
        m_target.Release();
    else
    {
        m_target = target->GetReference();
        m_think_state = THINK_STATE(Stalk);
    }
}

void Demi::ResetInputs ()
{
    EnemyShip::ResetInputs();
    m_port_weapon_primary_input = 0;
    m_port_weapon_secondary_input = 0;
    m_starboard_weapon_primary_input = 0;
    m_starboard_weapon_secondary_input = 0;
    m_aft_weapon_primary_input = 0;
    m_aft_weapon_secondary_input = 0;
}

void Demi::PickWanderDirection (Time time, Time::Delta frame_dt)
{
    ASSERT1(!m_target.IsValid());

    m_main_weapon = NULL;
    m_port_weapon = NULL;
    m_starboard_weapon = NULL;
    m_aft_weapon = NULL;

    PickTractorThinkStates();

    // update the next time to pick a wander direction
    m_next_wander_time = time + 6.0f;
    // pick a direction/speed to wander in
    m_wander_angle = Math::RandomAngle();
    m_think_state = THINK_STATE(Wander);
}

void Demi::Wander (Time time, Time::Delta frame_dt)
{
    ASSERT1(!m_target.IsValid());

    static Float const s_scan_radius = 400.0f;

    // scan area for targets
    Engine2::Circle::AreaTraceList area_trace_list;
    ASSERT1(GetObjectLayer() != NULL);
    GetPhysicsHandler()->AreaTrace(
        *GetObjectLayer(),
        Translation(),
        s_scan_radius,
        false,
        area_trace_list);
    // check the area trace list for targets
    for (Engine2::Circle::AreaTraceList::iterator it = area_trace_list.begin(),
                                                  it_end = area_trace_list.end();
         it != it_end;
         ++it)
    {
        Entity *entity = DStaticCast<Entity *>(*it);
        ASSERT1(entity != NULL);

        // if this entity is a solitary, set m_target and transition
        // to GaussGunStartAim
        if (entity->GetEntityType() == ET_SOLITARY)
        {
            m_target = entity->GetReference();
            m_think_state = THINK_STATE(Stalk);
            return;
        }
    }

    // incrementally accelerate up to the wander direction/speed
    FloatVector2 wander_velocity(ms_wander_speed[EnemyLevel()] * Math::UnitVector(m_wander_angle));
    MatchVelocity(wander_velocity, frame_dt);

    if (time >= m_next_wander_time)
        m_think_state = THINK_STATE(PickWanderDirection);
}

void Demi::Stalk (Time time, Time::Delta frame_dt)
{
    if (!m_target.IsValid() || m_target->IsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }

    Float target_distance = GetObjectLayer()->AdjustedDistance(m_target->Translation(), Translation());

    static WeightedThinkState const s_transition_near[] =
    {
        {    THINK_STATE(FlameThrowSweepStart), 8 },
        {  THINK_STATE(SpinningSlowBulletSpam), 6 },
        { THINK_STATE(SpinningInterloperSpawn), 5 }, // level 0
        {      THINK_STATE(SpinningShadeSpawn), 3 }, // level 1
        {      THINK_STATE(MissileLaunchStart), 3 }, // level 2
        {  THINK_STATE(SpinningRevulsionSpawn), 2 }  // level 3
    };
    static Uint32 const s_transition_near_count[ENEMY_LEVEL_COUNT] = { 3, 4, 5, 6 };
    static Uint32 s_transition_near_total_weight[ENEMY_LEVEL_COUNT] = { 0, 0, 0, 0 };

    static WeightedThinkState const s_transition_mid[] =
    {
        {    THINK_STATE(TractorTargetCloserStart),  3 },
        {            THINK_STATE(GaussGunStartAim), 10 },
        {        THINK_STATE(FlameThrowBlastStart), 20 },
        {      THINK_STATE(SpinningSlowBulletSpam), 15 },
        {     THINK_STATE(SpinningInterloperSpawn),  2 },
        {   THINK_STATE(InterloperSpawnBlastStart),  2 },
        {        THINK_STATE(ShadeSpawnBlastStart),  1 },
        {          THINK_STATE(SpinningFlameThrow), 10 }, // level 0
        {       THINK_STATE(SpinningMissileLaunch), 10 }, // level 1
        {        THINK_STATE(ShadeSpawnBlastStart),  3 },
        {          THINK_STATE(SpinningShadeSpawn),  3 }, // level 2
        {    THINK_STATE(RevulsionSpawnBlastStart),  4 },
        {      THINK_STATE(SpinningRevulsionSpawn),  3 },
        { THINK_STATE(SpinningGuidedMissileLaunch),  2 }  // level 3
    };
    static Uint32 const s_transition_mid_count[ENEMY_LEVEL_COUNT] = { 8, 9, 11, 14 };
    static Uint32 s_transition_mid_total_weight[ENEMY_LEVEL_COUNT] = { 0, 0, 0, 0 };

    static WeightedThinkState const s_transition_far[] =
    {
        {               THINK_STATE(ChargeStart), 20 },
        {  THINK_STATE(TractorTargetCloserStart), 10 },
        {    THINK_STATE(SpinningSlowBulletSpam),  5 }, // level 0
        { THINK_STATE(InterloperSpawnBlastStart),  5 }, // level 1
        {      THINK_STATE(ShadeSpawnBlastStart),  4 }, // level 2
        {  THINK_STATE(RevulsionSpawnBlastStart),  3 }  // level 3
    };
    static Uint32 const s_transition_far_count[ENEMY_LEVEL_COUNT] = { 3, 4, 5, 6 };
    static Uint32 s_transition_far_total_weight[ENEMY_LEVEL_COUNT] = { 0, 0, 0, 0 };

    // initialize the weights if need be.
    if (s_transition_near_total_weight[0] == 0)
    {
        Uint32 weight;
        for (Uint32 enemy_level = 0; enemy_level < ENEMY_LEVEL_COUNT; ++enemy_level)
        {
            weight = 0;
            ASSERT1(s_transition_near_count[enemy_level] <= LENGTHOF(s_transition_near));
            for (Uint32 j = 0; j < s_transition_near_count[enemy_level]; ++j)
                weight += s_transition_near[j].m_weight;
            s_transition_near_total_weight[enemy_level] = weight;

            weight = 0;
            ASSERT1(s_transition_mid_count[enemy_level] <= LENGTHOF(s_transition_mid));
            for (Uint32 j = 0; j < s_transition_mid_count[enemy_level]; ++j)
                weight += s_transition_mid[j].m_weight;
            s_transition_mid_total_weight[enemy_level] = weight;

            weight = 0;
            ASSERT1(s_transition_far_count[enemy_level] <= LENGTHOF(s_transition_far));
            for (Uint32 j = 0; j < s_transition_far_count[enemy_level]; ++j)
                weight += s_transition_far[j].m_weight;
            s_transition_far_total_weight[enemy_level] = weight;
        }
        ASSERT1(s_transition_near_total_weight[0] != 0);

//         std::cerr << "s_transition_near_total_weight = { ";
//         for (Uint32 enemy_level = 0; enemy_level < ENEMY_LEVEL_COUNT; ++enemy_level)
//             std::cerr << s_transition_near_total_weight[enemy_level] << ", ";
//         std::cerr << "};" << std::endl;
//         
//         std::cerr << "s_transition_mid_total_weight = { ";
//         for (Uint32 enemy_level = 0; enemy_level < ENEMY_LEVEL_COUNT; ++enemy_level)
//             std::cerr << s_transition_mid_total_weight[enemy_level] << ", ";
//         std::cerr << "};" << std::endl;
//         
//         std::cerr << "s_transition_far_total_weight = { ";
//         for (Uint32 enemy_level = 0; enemy_level < ENEMY_LEVEL_COUNT; ++enemy_level)
//             std::cerr << s_transition_far_total_weight[enemy_level] << ", ";
//         std::cerr << "};" << std::endl;
    }
    
    WeightedThinkState const *transition = NULL;
    Uint32 transition_count = 0;
    Uint32 transition_total_weight = 0;

    if (target_distance < ms_target_near_range_distance[EnemyLevel()])
    {
        transition = s_transition_near;
        transition_count = s_transition_near_count[EnemyLevel()];
        transition_total_weight = s_transition_near_total_weight[EnemyLevel()];
    }
    else if (target_distance < ms_target_mid_range_distance[EnemyLevel()])
    {
        transition = s_transition_mid;
        transition_count = s_transition_mid_count[EnemyLevel()];
        transition_total_weight = s_transition_mid_total_weight[EnemyLevel()];
    }
    else
    {
        transition = s_transition_far;
        transition_count = s_transition_far_count[EnemyLevel()];
        transition_total_weight = s_transition_far_total_weight[EnemyLevel()];
    }

    ASSERT1(transition != NULL);
    ASSERT1(transition_count > 0);
    ASSERT1(transition_total_weight > 0);

    Uint32 seed = Math::RandomUint16() % transition_total_weight;
    Uint32 i;
    for (i = 0; i < transition_count; seed -= transition[i].m_weight, ++i)
    {
        ASSERT1(seed < transition_total_weight);
        if (seed < transition[i].m_weight)
        {
            m_think_state = transition[i].m_think_state;
            break;
        }
    }
    ASSERT1(i < transition_count);
}

void Demi::PauseStart (Time time, Time::Delta frame_dt)
{
    m_start_time = time;
    m_think_state = THINK_STATE(PauseContinue);

    PickTractorThinkStates();
}

void Demi::PauseContinue (Time time, Time::Delta frame_dt)
{
    if (time >= m_start_time + ms_pause_duration[EnemyLevel()])
    {
        m_think_state = THINK_STATE(Stalk);
        return;
    }
}

void Demi::ChargeStart (Time time, Time::Delta frame_dt)
{
    if (!m_target.IsValid() || m_target->IsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }

    // record the time we picked where to aim
    m_start_time = time;
    // record the direction to charge in
    m_charge_velocity = GetObjectLayer()->AdjustedDifference(m_target->Translation(), Translation());
    ASSERT1(!m_charge_velocity.IsZero());
    m_charge_velocity.Normalize();
    m_charge_velocity *= 300.0f;

    // transition to and call ChargeAccelerate
    m_think_state = THINK_STATE(ChargeAccelerate);
    ChargeAccelerate(time, frame_dt);
}

void Demi::ChargeAccelerate (Time time, Time::Delta frame_dt)
{
    if (time >= m_start_time + 1.0f)
    {
        m_start_time = time;
        m_think_state = THINK_STATE(ChargeCoast);
        return;
    }

    MatchVelocity(m_charge_velocity, frame_dt, 10.0f * ms_engine_thrust[EnemyLevel()]);
}

void Demi::ChargeCoast (Time time, Time::Delta frame_dt)
{
    if (time >= m_start_time + 2.0f)
    {
        m_start_time = time;
        m_think_state = THINK_STATE(ChargeDecelerate);
    }
    else if (m_target.IsValid() && !m_target->IsDead())
    {
        Float target_distance = GetObjectLayer()->AdjustedDistance(m_target->Translation(), Translation());

        static Float const s_threshold_distance = 200.0f;
        if (target_distance <= s_threshold_distance)
        {
            m_start_time = time;
            m_think_state = THINK_STATE(ChargeDecelerate);
        }
    }
}

void Demi::ChargeDecelerate (Time time, Time::Delta frame_dt)
{
    if (Speed() < 10.0f)
    {
        m_start_time = time;
        m_think_state = THINK_STATE(PauseStart);
        return;
    }

    MatchVelocity(FloatVector2::ms_zero, frame_dt, 10.0f * ms_engine_thrust[EnemyLevel()]);
}

void Demi::GaussGunStartAim (Time time, Time::Delta frame_dt)
{
    if (!m_target.IsValid() || m_target->IsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }

    m_main_weapon = m_gauss_gun;

    // record the time we picked where to aim
    m_start_time = time;

    ASSERT1(!m_reticle_effect.IsValid() || !m_reticle_effect->IsInWorld());
    // ensure the reticle effect is allocated (lazy allocation)
    if (!m_reticle_effect.IsValid())
        m_reticle_effect =
            SpawnReticleEffect(
                GetObjectLayer(),
                Color(1.0f, 0.0f, 0.0f, 0.65f))->GetReference();
    // if the reticle effect is already allocated but not in the world, re-add it.
    else if (!m_reticle_effect->IsInWorld())
        m_reticle_effect->AddBackIntoWorld();
    ASSERT1(m_reticle_effect.IsValid() && m_reticle_effect->IsInWorld());

    // initialize the reticle coordinates
    m_reticle_effect->SnapToLocationAndSetScaleFactor(
        MuzzleLocation(m_gauss_gun),
        ms_gauss_gun_reticle_scale_factor[EnemyLevel()]);
    SetReticleCoordinates(m_reticle_effect->Translation());

    // transition to and call GaussGunContinueAim
    m_think_state = THINK_STATE(GaussGunContinueAim);
    GaussGunContinueAim(time, frame_dt);
}

void Demi::GaussGunContinueAim (Time time, Time::Delta frame_dt)
{
    ASSERT1(m_reticle_effect.IsValid() && m_reticle_effect->IsInWorld());
    ASSERT1(m_main_weapon == m_gauss_gun);

    if (!m_target.IsValid() || m_target->IsDead())
    {
        ASSERT1(m_reticle_effect.IsValid() && m_reticle_effect->IsInWorld());
        m_reticle_effect->ScheduleForRemovalFromWorld(0.0f);
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }

    FloatVector2 aim_direction(GetObjectLayer()->AdjustedDifference(m_target->Translation(), m_reticle_effect->Translation()));
    Float target_distance = GetObjectLayer()->AdjustedDistance(m_target->Translation(), MuzzleLocation(m_gauss_gun));
    Float reticle_target_distance = aim_direction.Length();
    Float distance_parameter =
        reticle_target_distance /
        GaussGun::ms_range[m_gauss_gun->UpgradeLevel()];

    // if the target went out of range, or if we're wasting time
    // transition back to sum'm
    if (target_distance > GaussGun::ms_range[m_gauss_gun->UpgradeLevel()] ||
        time > m_start_time + ms_gauss_gun_max_duration[EnemyLevel()])
    {
        ASSERT1(m_reticle_effect.IsValid() && m_reticle_effect->IsInWorld());
        m_reticle_effect->ScheduleForRemovalFromWorld(0.0f);
        m_think_state = THINK_STATE(PauseStart);
        return;
    }

    SetReticleCoordinates(m_reticle_effect->Translation());
    m_reticle_effect->SetScaleFactor(
        ms_gauss_gun_reticle_scale_factor[EnemyLevel()] *
        (1.0f - 0.5f * distance_parameter));

    ASSERT1(ms_gauss_gun_aim_error_radius[EnemyLevel()] > 0.0f);
    if (reticle_target_distance <= ms_gauss_gun_aim_error_radius[EnemyLevel()])
    {
        m_think_state = THINK_STATE(GaussGunFire);
        return;
    }
    else
    {
        aim_direction.Normalize();
        m_reticle_effect->SetVelocity(
            ms_gauss_gun_aim_max_speed[EnemyLevel()] * aim_direction +
            Velocity());
    }
}

void Demi::GaussGunFire (Time time, Time::Delta frame_dt)
{
    ASSERT1(m_reticle_effect.IsValid() && m_reticle_effect->IsInWorld());
    m_reticle_effect->ScheduleForRemovalFromWorld(0.0f);

    if (!m_target.IsValid() || m_target->IsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }

    SetReticleCoordinates(m_reticle_effect->Translation());
    SetWeaponPrimaryInput(UINT8_UPPER_BOUND);

    m_think_state = THINK_STATE(PauseStart);
    SetNextTimeToThink(time + 0.3f);
}

void Demi::FlameThrowSweepStart (Time time, Time::Delta frame_dt)
{
    m_main_weapon = m_flame_thrower;

    // record the time we started throwing flames
    m_start_time = time;

    // disable the tractor think functions
    m_port_tractor_think_state = NULL;
    m_starboard_tractor_think_state = NULL;
    // transition to and call FlameThrowSweepContinue
    m_think_state = THINK_STATE(FlameThrowSweepContinue);
    FlameThrowSweepContinue(time, frame_dt);
}

void Demi::FlameThrowSweepContinue (Time time, Time::Delta frame_dt)
{
    if (time >= m_start_time + ms_flame_throw_sweep_duration[EnemyLevel()])
    {
        m_think_state = THINK_STATE(PauseStart);
        return;
    }

    // aim (the constant coefficient is the left/right sweeping speed)
    Float aim_parameter = 180.0f * (time - m_start_time);
    Float aim_angle = ms_weapon_fov[EnemyLevel()] * Math::Cos(aim_parameter) + Angle();
    // and fire
    SetReticleCoordinates(MuzzleLocation(m_main_weapon) + Math::UnitVector(aim_angle));
    SetWeaponPrimaryInput(UINT8_UPPER_BOUND);
}

void Demi::FlameThrowBlastStart (Time time, Time::Delta frame_dt)
{
    m_main_weapon = m_flame_thrower;
    m_port_weapon = m_port_flame_thrower;
    m_starboard_weapon = m_starboard_flame_thrower;
    m_aft_weapon = m_aft_flame_thrower;

    // record the time we started throwing flames
    m_start_time = time;

    // disable the tractor think functions
    m_port_tractor_think_state = NULL;
    m_starboard_tractor_think_state = NULL;
    // transition to and call FlameThrowBlastContinue
    m_think_state = THINK_STATE(FlameThrowBlastContinue);
    FlameThrowBlastContinue(time, frame_dt);
}

void Demi::FlameThrowBlastContinue (Time time, Time::Delta frame_dt)
{
    if (time >= m_start_time + ms_flame_throw_blast_duration[EnemyLevel()])
    {
        m_think_state = THINK_STATE(PauseStart);
        return;
    }

    SetReticleCoordinates(MuzzleLocation(m_main_weapon) + Math::UnitVector(Angle()));
    SetWeaponPrimaryInput(UINT8_UPPER_BOUND);
    SetPortWeaponPrimaryInput(UINT8_UPPER_BOUND);
    SetStarboardWeaponPrimaryInput(UINT8_UPPER_BOUND);
    SetAftWeaponPrimaryInput(UINT8_UPPER_BOUND);
}

void Demi::MissileLaunchStart (Time time, Time::Delta frame_dt)
{
    if (!m_target.IsValid() || m_target->IsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }

    m_main_weapon = m_missile_launcher;

    // record the time we started launching missiles
    m_start_time = time;

    // disable the tractor think functions
    m_port_tractor_think_state = NULL;
    m_starboard_tractor_think_state = NULL;
    // transition to and call MissileLaunchContinue
    m_think_state = THINK_STATE(MissileLaunchContinue);
    MissileLaunchContinue(time, frame_dt);
}

void Demi::MissileLaunchContinue (Time time, Time::Delta frame_dt)
{
    if (time >= m_start_time + ms_missile_launch_duration[EnemyLevel()])
    {
        m_think_state = THINK_STATE(PauseStart);
        return;
    }

    // TODO: more accurate aiming for higher enemy levels

    SetReticleCoordinates(m_target->Translation());
    SetWeaponPrimaryInput(UINT8_UPPER_BOUND);
}

void Demi::InterloperSpawnBlastStart (Time time, Time::Delta frame_dt)
{
    m_aft_enemy_spawner->SetFireRateOverride(5.0f);
    m_aft_enemy_spawner->SetEnemySpawnType(ET_INTERLOPER);
    m_aft_weapon = m_aft_enemy_spawner;
    // record the time we started spawning enemies
    m_start_time = time;
    // transition to and call InterloperSpawnBlastContinue
    m_think_state = THINK_STATE(InterloperSpawnBlastContinue);
    InterloperSpawnBlastContinue(time, frame_dt);
}

void Demi::InterloperSpawnBlastContinue (Time time, Time::Delta frame_dt)
{
    if (time >= m_start_time + ms_enemy_spawn_blast_duration[EnemyLevel()])
    {
        m_aft_enemy_spawner->ClearFireRateOverride();
        m_think_state = THINK_STATE(PauseStart);
        return;
    }

    SetAftWeaponPrimaryInput(UINT8_UPPER_BOUND);
}

void Demi::ShadeSpawnBlastStart (Time time, Time::Delta frame_dt)
{
    m_aft_enemy_spawner->SetFireRateOverride(5.0f);
    m_aft_enemy_spawner->SetEnemySpawnType(ET_SHADE);
    m_aft_weapon = m_aft_enemy_spawner;
    // record the time we started spawning enemies
    m_start_time = time;
    // transition to and call ShadeSpawnBlastContinue
    m_think_state = THINK_STATE(ShadeSpawnBlastContinue);
    ShadeSpawnBlastContinue(time, frame_dt);
}

void Demi::ShadeSpawnBlastContinue (Time time, Time::Delta frame_dt)
{
    if (time >= m_start_time + ms_enemy_spawn_blast_duration[EnemyLevel()])
    {
        m_aft_enemy_spawner->ClearFireRateOverride();
        m_think_state = THINK_STATE(PauseStart);
        return;
    }

    SetAftWeaponPrimaryInput(UINT8_UPPER_BOUND);
}

void Demi::RevulsionSpawnBlastStart (Time time, Time::Delta frame_dt)
{
    m_aft_enemy_spawner->SetFireRateOverride(5.0f);
    m_aft_enemy_spawner->SetEnemySpawnType(ET_REVULSION);
    m_aft_weapon = m_aft_enemy_spawner;
    // record the time we started spawning enemies
    m_start_time = time;
    // transition to and call RevulsionSpawnBlastContinue
    m_think_state = THINK_STATE(RevulsionSpawnBlastContinue);
    RevulsionSpawnBlastContinue(time, frame_dt);
}

void Demi::RevulsionSpawnBlastContinue (Time time, Time::Delta frame_dt)
{
    if (time >= m_start_time + ms_enemy_spawn_blast_duration[EnemyLevel()])
    {
        m_aft_enemy_spawner->ClearFireRateOverride();
        m_think_state = THINK_STATE(PauseStart);
        return;
    }

    SetAftWeaponPrimaryInput(UINT8_UPPER_BOUND);
}

void Demi::SpinningFlameThrow (Time time, Time::Delta frame_dt)
{
    // set the appropriate weapons
    m_main_weapon = m_flame_thrower;
    m_port_weapon = m_port_flame_thrower;
    m_starboard_weapon = m_starboard_flame_thrower;
    m_aft_weapon = m_aft_flame_thrower;
    m_spin_accelerate_through_angle = 360.0f;
    m_spin_acceleration_duration = ms_spinning_attack_acceleration_duration[EnemyLevel()];
    m_spin_duration = ms_spinning_attack_duration[EnemyLevel()];
    m_spinning_attack_uses_secondary_fire = false;
    // transition to and call SpinningAttackStart
    m_think_state = THINK_STATE(SpinningAttackStart);
    m_spinning_attack_finish_state = THINK_STATE(PauseStart);
    SpinningAttackStart(time, frame_dt);
}

void Demi::SpinningMissileLaunch (Time time, Time::Delta frame_dt)
{
    // set the appropriate weapons
    m_main_weapon = m_missile_launcher;
    m_port_weapon = m_port_missile_launcher;
    m_starboard_weapon = m_starboard_missile_launcher;
    m_aft_weapon = m_aft_missile_launcher;
    m_spin_accelerate_through_angle = 360.0f;
    m_spin_acceleration_duration = ms_spinning_attack_acceleration_duration[EnemyLevel()];
    m_spin_duration = ms_spinning_attack_duration[EnemyLevel()];
    m_spinning_attack_uses_secondary_fire = false;
    // transition to and call SpinningAttackStart
    m_think_state = THINK_STATE(SpinningAttackStart);
    m_spinning_attack_finish_state = THINK_STATE(PauseStart);
    SpinningAttackStart(time, frame_dt);
}

void Demi::SpinningGuidedMissileLaunch (Time time, Time::Delta frame_dt)
{
    // set the appropriate weapons
    m_main_weapon = m_missile_launcher;
    m_port_weapon = m_port_missile_launcher;
    m_starboard_weapon = m_starboard_missile_launcher;
    m_aft_weapon = m_aft_missile_launcher;
    m_spin_accelerate_through_angle = 360.0f;
    m_spin_acceleration_duration = ms_spinning_attack_acceleration_duration[EnemyLevel()];
    m_spin_duration = ms_spinning_attack_duration[EnemyLevel()];
    m_spinning_attack_uses_secondary_fire = true;
    // transition to and call SpinningAttackStart
    m_think_state = THINK_STATE(SpinningAttackStart);
    m_spinning_attack_finish_state = THINK_STATE(PauseStart);
    SpinningAttackStart(time, frame_dt);
}

void Demi::SpinningSlowBulletSpam (Time time, Time::Delta frame_dt)
{
    // set the appropriate weapons
    m_main_weapon = m_slow_bullet_gun;
    m_port_weapon = m_port_slow_bullet_gun;
    m_starboard_weapon = m_starboard_slow_bullet_gun;
    m_aft_weapon = m_aft_slow_bullet_gun;
    m_spin_accelerate_through_angle = 360.0f;
    m_spin_acceleration_duration = ms_spinning_attack_acceleration_duration[EnemyLevel()];
    m_spin_duration = ms_slow_bullet_gun_spam_duration[EnemyLevel()];
    m_spinning_attack_uses_secondary_fire = false;
    // transition to and call SpinningAttackStart
    m_think_state = THINK_STATE(SpinningAttackStart);
    m_spinning_attack_finish_state = THINK_STATE(SpinningSlowBulletSpamFinish);
    SpinningAttackStart(time, frame_dt);
}

void Demi::SpinningSlowBulletSpamFinish (Time time, Time::Delta frame_dt)
{
    // this does exactly the same thing as PauseStart, except that it specifies TA_SUCK_UP_POWERUPS
    
    m_start_time = time;
    m_think_state = THINK_STATE(PauseContinue);

    PickTractorThinkStates(TA_SUCK_UP_POWERUPS);
}

void Demi::SpinningInterloperSpawn (Time time, Time::Delta frame_dt)
{
    // set the appropriate weapons
    m_main_weapon = NULL;
    m_port_weapon = NULL;
    m_starboard_weapon = NULL;
    m_aft_enemy_spawner->SetEnemySpawnType(ET_INTERLOPER);
    m_aft_weapon = m_aft_enemy_spawner;
    m_spin_accelerate_through_angle = 90.0f;
    m_spin_acceleration_duration = ms_spinning_attack_acceleration_duration[EnemyLevel()];
    ASSERT1(m_spin_acceleration_duration > 0.0f);
    Float v = 2.0f * m_spin_accelerate_through_angle / m_spin_acceleration_duration;
    ASSERT1(v > 0.0f);
    m_spin_duration = (360.0f - 2.0f * m_spin_accelerate_through_angle) / v;
    m_spinning_attack_uses_secondary_fire = false;
    // transition to and call SpinningAttackStart
    m_think_state = THINK_STATE(SpinningAttackStart);
    m_spinning_attack_finish_state = THINK_STATE(PauseStart);
    SpinningAttackStart(time, frame_dt);
}

void Demi::SpinningShadeSpawn (Time time, Time::Delta frame_dt)
{
    // set the appropriate weapons
    m_main_weapon = NULL;
    m_port_weapon = NULL;
    m_starboard_weapon = NULL;
    m_aft_enemy_spawner->SetEnemySpawnType(ET_SHADE);
    m_aft_weapon = m_aft_enemy_spawner;
    m_spin_accelerate_through_angle = 90.0f;
    m_spin_acceleration_duration = ms_spinning_attack_acceleration_duration[EnemyLevel()];
    ASSERT1(m_spin_acceleration_duration > 0.0f);
    Float v = 2.0f * m_spin_accelerate_through_angle / m_spin_acceleration_duration;
    ASSERT1(v > 0.0f);
    m_spin_duration = (360.0f - 2.0f * m_spin_accelerate_through_angle) / v;
    m_spinning_attack_uses_secondary_fire = false;
    // transition to and call SpinningAttackStart
    m_think_state = THINK_STATE(SpinningAttackStart);
    m_spinning_attack_finish_state = THINK_STATE(PauseStart);
    SpinningAttackStart(time, frame_dt);
}

void Demi::SpinningRevulsionSpawn (Time time, Time::Delta frame_dt)
{
    // set the appropriate weapons
    m_main_weapon = NULL;
    m_port_weapon = NULL;
    m_starboard_weapon = NULL;
    m_aft_enemy_spawner->SetEnemySpawnType(ET_REVULSION);
    m_aft_weapon = m_aft_enemy_spawner;
    m_spin_accelerate_through_angle = 90.0f;
    m_spin_acceleration_duration = ms_spinning_attack_acceleration_duration[EnemyLevel()];
    ASSERT1(m_spin_acceleration_duration > 0.0f);
    Float v = 2.0f * m_spin_accelerate_through_angle / m_spin_acceleration_duration;
    ASSERT1(v > 0.0f);
    m_spin_duration = (360.0f - 2.0f * m_spin_accelerate_through_angle) / v;
    m_spinning_attack_uses_secondary_fire = false;
    // transition to and call SpinningAttackStart
    m_think_state = THINK_STATE(SpinningAttackStart);
    m_spinning_attack_finish_state = THINK_STATE(PauseStart);
    SpinningAttackStart(time, frame_dt);
}

void Demi::SpinningAttackStart (Time time, Time::Delta frame_dt)
{
    // disable the tractor think functions
    m_port_tractor_think_state = NULL;
    m_starboard_tractor_think_state = NULL;

    // record the time we started launching missiles
    m_start_time = time;
    // randomly decide to go counter/clockwise
    m_spin_direction = (Math::RandomUint16()&1) == 0 ? 1.0f : -1.0f;
    // transition to and call SpinningAttackAccelerate
    m_think_state = THINK_STATE(SpinningAttackAccelerate);
    SpinningAttackAccelerate(time, frame_dt);
}

void Demi::SpinningAttackAccelerate (Time time, Time::Delta frame_dt)
{
    // if we're done accelerating the spin, transition to SpinningAttackFire
    ASSERT1(m_spin_acceleration_duration >= 0.0f);
    if (time >= m_start_time + m_spin_acceleration_duration)
    {
        m_start_time = time;
        m_think_state = THINK_STATE(SpinningAttackFire);
        return;
    }

    // calculate the angular acceleration (we want to accelerate
    // through one whole revolution up to the spinning speed).
    // accel = 2*angle/T^2
    ASSERT1(m_spin_accelerate_through_angle > 0.0f);
    ASSERT1(m_spin_acceleration_duration > 0.0f);
    Float const angular_acceleration =
        m_spin_direction * 2.0f * m_spin_accelerate_through_angle /
        (m_spin_acceleration_duration * m_spin_acceleration_duration);
    // accelerate the spin
    SetAngularVelocity(AngularVelocity() + angular_acceleration * frame_dt);
}

void Demi::SpinningAttackFire (Time time, Time::Delta frame_dt)
{
    // if we're done firing, start decelerating
    ASSERT1(m_spin_duration >= 0.0f);
    if (time >= m_start_time + m_spin_duration)
    {
        m_start_time = time;
        m_think_state = THINK_STATE(SpinningAttackDecelerate);
        return;
    }

    SetReticleCoordinates((1.0f + VisibleRadius()) * Math::UnitVector(Angle()));
    if (m_spinning_attack_uses_secondary_fire)
    {
        SetWeaponSecondaryInput(UINT8_UPPER_BOUND);
        SetPortWeaponSecondaryInput(UINT8_UPPER_BOUND);
        SetStarboardWeaponSecondaryInput(UINT8_UPPER_BOUND);
        SetAftWeaponSecondaryInput(UINT8_UPPER_BOUND);
    }
    else
    {
        SetWeaponPrimaryInput(UINT8_UPPER_BOUND);
        SetPortWeaponPrimaryInput(UINT8_UPPER_BOUND);
        SetStarboardWeaponPrimaryInput(UINT8_UPPER_BOUND);
        SetAftWeaponPrimaryInput(UINT8_UPPER_BOUND);
    }
}

void Demi::SpinningAttackDecelerate (Time time, Time::Delta frame_dt)
{
    // if we're done decelerating the spin, we're done with this attack
    ASSERT1(m_spin_acceleration_duration >= 0.0f);
    if (time >= m_start_time + m_spin_acceleration_duration)
    {
        SetAngularVelocity(0.0f);
        ASSERT1(m_spinning_attack_finish_state != NULL);
        m_think_state = m_spinning_attack_finish_state;
        m_spinning_attack_finish_state = NULL;
        return;
    }

    // calculate the angular deceleration (we want to decelerate
    // through one whole revolution down to 0 angular speed).
    ASSERT1(m_spin_accelerate_through_angle > 0.0f);
    ASSERT1(m_spin_acceleration_duration > 0.0f);
    Float const angular_acceleration =
        -m_spin_direction * 2.0f * m_spin_accelerate_through_angle /
        (m_spin_acceleration_duration * m_spin_acceleration_duration);
    // accelerate the spin
    SetAngularVelocity(AngularVelocity() + angular_acceleration * frame_dt);
}

void Demi::TractorTargetCloserStart (Time time, Time::Delta frame_dt)
{
    // set the tractors' think states
    m_port_tractor_think_state = THINK_STATE(PortTractorPullTargetCloser);
    m_starboard_tractor_think_state = THINK_STATE(StarboardTractorPullTargetCloser);
    // record the time we started launching missiles
    m_start_time = time;
    // transition to and call TractorTargetCloserContinue
    m_think_state = THINK_STATE(TractorTargetCloserContinue);
    TractorTargetCloserContinue(time, frame_dt);
}

void Demi::TractorTargetCloserContinue (Time time, Time::Delta frame_dt)
{
    if (!m_target.IsValid() || m_target->IsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }

    if (time >= m_start_time + ms_tractor_target_closer_duration[EnemyLevel()])
    {
        m_think_state = THINK_STATE(FlameThrowSweepStart);
        m_port_tractor_think_state = NULL;
        m_starboard_tractor_think_state = NULL;
        return;
    }
}

void Demi::PortTractorDeflectStuff (Time time, Time::Delta frame_dt)
{
    m_port_weapon = m_port_tractor;

    Entity *best_target =
        FindTractorTarget(
            TA_DEFLECT,
            MuzzleLocation(m_port_tractor),
            MuzzleDirection(m_port_tractor),
            time,
            frame_dt);

    if (best_target != NULL)
    {
        // aim the tractor and set it to push
        SetPortReticleCoordinates(best_target->Translation());
        SetPortWeaponPrimaryInput(0);
        SetPortWeaponSecondaryInput(UINT8_UPPER_BOUND);
    }
}

void Demi::StarboardTractorDeflectStuff (Time time, Time::Delta frame_dt)
{
    m_starboard_weapon = m_starboard_tractor;

    Entity *best_target =
        FindTractorTarget(
            TA_DEFLECT,
            MuzzleLocation(m_starboard_tractor),
            MuzzleDirection(m_starboard_tractor),
            time,
            frame_dt);

    if (best_target != NULL)
    {
        // aim the tractor and set it to push
        SetStarboardReticleCoordinates(best_target->Translation());
        SetStarboardWeaponPrimaryInput(0);
        SetStarboardWeaponSecondaryInput(UINT8_UPPER_BOUND);
    }
}

void Demi::PortTractorPullTargetCloser (Time time, Time::Delta frame_dt)
{
    if (!m_target.IsValid() || m_target->IsDead())
        return;

    m_port_weapon = m_port_tractor;

    // aim the tractor and set it to pull
    SetPortReticleCoordinates(m_target->Translation());
    SetPortWeaponPrimaryInput(UINT8_UPPER_BOUND);
    SetPortWeaponSecondaryInput(0);
}

void Demi::StarboardTractorPullTargetCloser (Time time, Time::Delta frame_dt)
{
    if (!m_target.IsValid() || m_target->IsDead())
        return;

    m_starboard_weapon = m_starboard_tractor;

    // aim the tractor and set it to pull
    SetStarboardReticleCoordinates(m_target->Translation());
    SetStarboardWeaponPrimaryInput(UINT8_UPPER_BOUND);
    SetStarboardWeaponSecondaryInput(0);
}

void Demi::PortTractorSuckUpPowerups (Time time, Time::Delta frame_dt)
{
    m_port_weapon = m_port_tractor;

    Entity *best_target =
        FindTractorTarget(
            TA_SUCK_UP_POWERUPS,
            MuzzleLocation(m_port_tractor),
            MuzzleDirection(m_port_tractor),
            time,
            frame_dt);

    if (best_target != NULL)
    {
        // aim the tractor and set it to grab
        SetPortReticleCoordinates(best_target->Translation());
        SetPortWeaponPrimaryInput(UINT8_UPPER_BOUND);
        SetPortWeaponSecondaryInput(0);
    }
}

void Demi::StarboardTractorSuckUpPowerups (Time time, Time::Delta frame_dt)
{
    m_starboard_weapon = m_starboard_tractor;

    Entity *best_target =
        FindTractorTarget(
            TA_SUCK_UP_POWERUPS,
            MuzzleLocation(m_starboard_tractor),
            MuzzleDirection(m_starboard_tractor),
            time,
            frame_dt);

    if (best_target != NULL)
    {
        // aim the tractor and set it to grab
        SetStarboardReticleCoordinates(best_target->Translation());
        SetStarboardWeaponPrimaryInput(UINT8_UPPER_BOUND);
        SetStarboardWeaponSecondaryInput(0);
    }
}

void Demi::PortTractorFlingStuffAtTarget (Time time, Time::Delta frame_dt)
{
    if (!m_target.IsValid() || m_target->IsDead())
        return;

    m_port_weapon = m_port_tractor;

    Entity *best_target =
        FindTractorTarget(
            TA_FLING,
            MuzzleLocation(m_port_tractor),
            MuzzleDirection(m_port_tractor),
            time,
            frame_dt);

    if (best_target != NULL)
    {
        // aim the tractor and set it to grab
        SetPortReticleCoordinates(best_target->Translation());
        SetPortWeaponPrimaryInput(UINT8_UPPER_BOUND);
        SetPortWeaponSecondaryInput(UINT8_UPPER_BOUND);
        // set the target to fling at
        m_port_tractor->Target(*m_target);
    }
}

void Demi::StarboardTractorFlingStuffAtTarget (Time time, Time::Delta frame_dt)
{
    if (!m_target.IsValid() || m_target->IsDead())
        return;

    m_starboard_weapon = m_starboard_tractor;

    Entity *best_target =
        FindTractorTarget(
            TA_FLING,
            MuzzleLocation(m_starboard_tractor),
            MuzzleDirection(m_starboard_tractor),
            time,
            frame_dt);

    if (best_target != NULL)
    {
        // aim the tractor and set it to grab
        SetStarboardReticleCoordinates(best_target->Translation());
        SetStarboardWeaponPrimaryInput(UINT8_UPPER_BOUND);
        SetStarboardWeaponSecondaryInput(UINT8_UPPER_BOUND);
        // set the target to fling at
        m_starboard_tractor->Target(*m_target);
    }
}

void Demi::PickTractorThinkStates (TractorAction tractor_action)
{
    ASSERT1(tractor_action < TA_COUNT || tractor_action == TA_RANDOM);
    
    // reset these -- technically unnecessary, but why not.
    m_port_tractor->Target(NULL);
    m_starboard_tractor->Target(NULL);
    // if the tractor action is TA_RANDOM, pick a random action.
    if (tractor_action == TA_RANDOM)
    {
        // choose which tractor action to do.  if there is a target,
        // then TA_DEFLECT, TA_SUCK_UP_POWERUPS and TA_FLING are all possibilities.
        Uint16 behavior = 0;
        if (m_target.IsValid())
            behavior = Math::RandomUint16(0, 2);
        // otherwise only TA_DEFLECT and TA_SUCK_UP_POWERUPS work.
        else
            behavior = Math::RandomUint16(0, 1);
        tractor_action = static_cast<TractorAction>(behavior);
    }

    switch (tractor_action)
    {
        case TA_DEFLECT:
            m_port_tractor_think_state = THINK_STATE(PortTractorDeflectStuff);
            m_starboard_tractor_think_state = THINK_STATE(StarboardTractorDeflectStuff);
            break;

        case TA_SUCK_UP_POWERUPS:
            m_port_tractor_think_state = THINK_STATE(PortTractorSuckUpPowerups);
            m_starboard_tractor_think_state = THINK_STATE(StarboardTractorSuckUpPowerups);
            break;

        case TA_FLING:
            m_port_tractor_think_state = THINK_STATE(PortTractorFlingStuffAtTarget);
            m_starboard_tractor_think_state = THINK_STATE(StarboardTractorFlingStuffAtTarget);
            break;

        default:
            ASSERT1(false && "invalid TractorAction");
            break;
    }
}

void Demi::MatchVelocity (FloatVector2 const &velocity, Time::Delta frame_dt, Float max_thrust)
{
    // calculate what thrust is required to match the desired velocity
    FloatVector2 velocity_differential =
        velocity - (Velocity() + frame_dt * Force() / Mass());
    FloatVector2 thrust_vector = Mass() * velocity_differential / frame_dt;
    if (!thrust_vector.IsZero())
    {
        if (max_thrust < 0.0f)
            max_thrust = ms_engine_thrust[EnemyLevel()];

        Float thrust_force = thrust_vector.Length();
        if (thrust_force > max_thrust)
            thrust_vector = max_thrust * thrust_vector.Normalization();

        AccumulateForce(thrust_vector);
    }
}

Entity *Demi::FindTractorTarget (
    TractorAction tractor_action,
    FloatVector2 const &muzzle_location,
    FloatVector2 const &muzzle_direction,
    Time time,
    Time::Delta frame_dt)
{
    ASSERT1(tractor_action < TA_COUNT);

    Float scan_radius = 0.5f * (ms_tractor_beam_radius[EnemyLevel()] + PhysicalRadius());
    Engine2::Circle::AreaTraceList area_trace_list;
    ASSERT1(GetObjectLayer() != NULL);
    GetPhysicsHandler()->AreaTrace(
        *GetObjectLayer(),
        muzzle_location + scan_radius * muzzle_direction.Normalization(),
        scan_radius,
        false,
        area_trace_list);

    Entity *best_target = NULL;
    Float best_target_priority = 0.0f;
    for (Engine2::Circle::AreaTraceList::iterator it = area_trace_list.begin(),
                                                  it_end = area_trace_list.end();
         it != it_end;
         ++it)
    {
        Entity *entity = DStaticCast<Entity *>(*it);
        ASSERT1(entity != NULL);

        // we don't want to deflect ourselves (it would cancel out anyway)
        if (entity == this)
            continue;

        Float potential_target_priority = -1.0f;

        switch (tractor_action)
        {
            case TA_DEFLECT:
                {
                    // only target stuff that's about to collide with us (TODO: verify this works)
                    Time::Delta collision_dt = CollisionTime(entity, 2.0f);
                    if (collision_dt >= 0.0f)
                    {
                        if (entity->IsExplosive() &&
                            entity->GetEntityType() != ET_ENEMY_MISSILE &&
                            entity->GetEntityType() != ET_GUIDED_ENEMY_MISSILE)
                        {
                            potential_target_priority = 30.0f + DStaticCast<Explosive *>(entity)->WeaponLevel();
                        }
                        else if (entity->IsBallistic())
                        {
                            potential_target_priority = 20.0f + DStaticCast<Ballistic *>(entity)->WeaponLevel();
                        }
                        else if (entity->GetEntityType() == ET_ASTEROID)
                        {
                            Float target_weight = Min(entity->Mass(), 2000.0f) - 1000.0f;
                            target_weight *= target_weight;
                            target_weight /= -1000000.0f;
                            target_weight += 1.0;
                            // target weight should now be in [0.0f, 1.0f], and entities with
                            // masses closer to 1000 will be weighted closer to 1.0f.
                            potential_target_priority = 10.0f + 9.0f * target_weight;
                        }
                    }
                }
                break;


            case TA_SUCK_UP_POWERUPS:
                // don't want to suck up the target (that's a different mode)
                if (entity == *m_target)
                    continue;

                // only want to suck up powerups
                if (entity->IsPowerup())
                    potential_target_priority = DStaticCast<Powerup *>(entity)->EffectiveValue();
                break;

            case TA_FLING:
                // don't want to fling the target (it would cancel out anyway)
                if (entity == *m_target)
                    continue;

                // don't want to fling non-solids or powerups at the player.
                if (entity->GetCollisionType() == Engine2::Circle::CT_SOLID_COLLISION && !entity->IsPowerup())
                {
                    // just go by mass
                    potential_target_priority = entity->Mass();
                }
                break;

            default:
                ASSERT1(false && "this shouldn't happen");
                break;
        }

        if (potential_target_priority > best_target_priority)
        {
            best_target = entity;
            best_target_priority = potential_target_priority;
        }
    }

    return best_target;
}

} // end of namespace Dis
