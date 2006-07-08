// ///////////////////////////////////////////////////////////////////////////
// dis_demi.cpp by Victor Dods, created 2006/05/07
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_demi.h"

#include "dis_ballistic.h"
#include "dis_effect.h"
#include "dis_explosive.h"
#include "dis_physicshandler.h"
#include "dis_spawn.h"
#include "dis_weapon.h"
#include "dis_world.h"
#include "xrb_engine2_objectlayer.h"

using namespace Xrb;

#define THINK_STATE(x) &Demi::x

namespace Dis
{

Float const Demi::ms_max_health[ENEMY_LEVEL_COUNT] = { 1500.0f, 3000.0f, 6000.0f, 12000.0f };
Float const Demi::ms_engine_thrust[ENEMY_LEVEL_COUNT] = { 500000.0f, 700000.0f, 925000.0f, 1200000.0f };
Float const Demi::ms_max_angular_velocity[ENEMY_LEVEL_COUNT] = { 90.0f, 90.0f, 90.0f, 90.0f };
Float const Demi::ms_scale_factor[ENEMY_LEVEL_COUNT] = { 55.0f, 65.0f, 75.0f, 85.0f };
Float const Demi::ms_baseline_first_moment[ENEMY_LEVEL_COUNT] = { 10000.0f, 14000.0f, 18500.0f, 24000.0f };
Float const Demi::ms_damage_dissipation_rate[ENEMY_LEVEL_COUNT] = { 4.0f, 8.0f, 12.0f, 16.0f };
Float const Demi::ms_wander_speed[ENEMY_LEVEL_COUNT] = { 30.0f, 40.0f, 50.0f, 60.0f };
Float const Demi::ms_weapon_fov[ENEMY_LEVEL_COUNT] = { 60.0f, 60.0f, 60.0f, 60.0f };
Float const Demi::ms_spinning_attack_acceleration_duration[ENEMY_LEVEL_COUNT] = { 0.75f, 0.75f, 0.75f, 0.75f };
Float const Demi::ms_spinning_attack_duration[ENEMY_LEVEL_COUNT] = { 2.0f, 2.0f, 2.0f, 2.0f };
Float const Demi::ms_flame_thrower_max_damage_per_fireball[ENEMY_LEVEL_COUNT] = { 10.0f, 20.0f, 40.0f, 80.0f };
Float const Demi::ms_flame_thrower_final_fireball_size[ENEMY_LEVEL_COUNT] = { 70.0f, 80.0f, 90.0f, 100.0f };
Float const Demi::ms_gauss_gun_impact_damage[ENEMY_LEVEL_COUNT] = { 20.0f, 40.0f, 80.0f, 160.0f };
Float const Demi::ms_gauss_gun_aim_error_radius[ENEMY_LEVEL_COUNT] = { 25.0f, 20.0f, 15.0f, 10.0f };
Float const Demi::ms_gauss_gun_aim_max_speed[ENEMY_LEVEL_COUNT] = { 150.0f, 150.0f, 150.0f, 150.0f };
Float const Demi::ms_gauss_gun_reticle_scale_factor[ENEMY_LEVEL_COUNT] = { 20.0f, 20.0f, 20.0f, 20.0f };
Float const Demi::ms_gauss_gun_max_duration[ENEMY_LEVEL_COUNT] = { 7.0f, 7.0f, 7.0f, 7.0f };
Float const Demi::ms_flame_throw_sweep_duration[ENEMY_LEVEL_COUNT] = { 3.0f, 3.0f, 3.0f, 3.0f };
Float const Demi::ms_flame_throw_blast_duration[ENEMY_LEVEL_COUNT] = { 0.15f, 0.15f, 0.15f, 0.15f };
Float const Demi::ms_missile_launch_duration[ENEMY_LEVEL_COUNT] = { 1.0f, 1.0f, 1.0f, 1.0f };
Float const Demi::ms_enemy_spawn_blast_duration[ENEMY_LEVEL_COUNT] = { 2.0f, 2.0f, 2.0f, 2.0f };
Float const Demi::ms_tractor_target_closer_duration[ENEMY_LEVEL_COUNT] = { 1.0f, 1.0f, 1.0f, 1.0f };
Float const Demi::ms_tractor_range[ENEMY_LEVEL_COUNT] = { 20000.0f, 20000.0f, 20000.0f, 20000.0f };
Float const Demi::ms_tractor_strength[ENEMY_LEVEL_COUNT] = { 1500.0f, 3000.0f, 6000.0f, 12000.0f };
Float const Demi::ms_tractor_max_force[ENEMY_LEVEL_COUNT] = { 2000000.0f, 2000000.0f, 2000000.0f, 2000000.0f };
Float const Demi::ms_tractor_beam_radius[ENEMY_LEVEL_COUNT] = { 80.0f, 100.0f, 120.0f, 140.0f };
Float const Demi::ms_target_near_range_distance[ENEMY_LEVEL_COUNT] = { 200.0f, 210.0f, 220.0f, 230.0f };
Float const Demi::ms_target_mid_range_distance[ENEMY_LEVEL_COUNT] = { 350.0f, 360.0f, 370.0f, 380.0f };
Float const Demi::ms_pause_duration[ENEMY_LEVEL_COUNT] = { 2.0f, 1.5f, 1.25f, 1.0f };

Float const Demi::ms_side_port_angle = 64.7f;

Demi::Demi (Uint8 const enemy_level)
    :
    EnemyShip(enemy_level, ms_max_health[enemy_level], ET_DEMI),
    m_port_reticle_coordinates(FloatVector2::ms_zero),
    m_starboard_reticle_coordinates(FloatVector2::ms_zero)
{
    m_think_state = THINK_STATE(PickWanderDirection);
    m_port_tractor_think_state = THINK_STATE(PortTractorDeflectStuff);
    m_starboard_tractor_think_state = THINK_STATE(StarboardTractorDeflectStuff);

    m_spinning_attack_uses_secondary_fire = false;

    m_main_weapon = NULL;
    m_port_weapon = NULL;
    m_starboard_weapon = NULL;
    m_aft_weapon = NULL;

    // main weapon setup
    {
        // the gauss gun weapon level is only used to indicate
        // weapon range and fire rate
        m_gauss_gun = new GaussGun(3);
        m_gauss_gun->SetImpactDamageOverride(ms_gauss_gun_impact_damage[GetEnemyLevel()]);
        m_gauss_gun->Equip(this);

        m_flame_thrower = new FlameThrower(3);
        m_flame_thrower->SetMaxDamagePerFireballOverride(ms_flame_thrower_max_damage_per_fireball[GetEnemyLevel()]);
        m_flame_thrower->SetFinalFireballSizeOverride(ms_flame_thrower_final_fireball_size[GetEnemyLevel()]);
        m_flame_thrower->Equip(this);

        // TODO: overrides for missile launcher damage/etc
        m_missile_launcher = new MissileLauncher(0);
        m_missile_launcher->SetSpawnEnemyMissiles(true);
        m_missile_launcher->Equip(this);
    }

    // port-side weapon setup
    {
        m_port_tractor = new Tractor(3);
        m_port_tractor->SetRangeOverride(ms_tractor_range[GetEnemyLevel()]);
        m_port_tractor->SetStrengthOverride(ms_tractor_strength[GetEnemyLevel()]);
        m_port_tractor->SetMaxForceOverride(ms_tractor_max_force[GetEnemyLevel()]);
        m_port_tractor->SetBeamRadiusOverride(ms_tractor_beam_radius[GetEnemyLevel()]);
        m_port_tractor->Equip(this);

        m_port_flame_thrower = new FlameThrower(3);
        m_port_flame_thrower->SetMaxDamagePerFireballOverride(ms_flame_thrower_max_damage_per_fireball[GetEnemyLevel()]);
        m_port_flame_thrower->SetFinalFireballSizeOverride(ms_flame_thrower_final_fireball_size[GetEnemyLevel()]);
        m_port_flame_thrower->Equip(this);

        m_port_missile_launcher = new MissileLauncher(0);
        m_port_missile_launcher->SetSpawnEnemyMissiles(true);
        m_port_missile_launcher->Equip(this);
    }

    // starboard-side weapon setup
    {
        m_starboard_tractor = new Tractor(3);
        m_starboard_tractor->SetRangeOverride(ms_tractor_range[GetEnemyLevel()]);
        m_starboard_tractor->SetStrengthOverride(ms_tractor_strength[GetEnemyLevel()]);
        m_starboard_tractor->SetMaxForceOverride(ms_tractor_max_force[GetEnemyLevel()]);
        m_starboard_tractor->SetBeamRadiusOverride(ms_tractor_beam_radius[GetEnemyLevel()]);
        m_starboard_tractor->Equip(this);

        m_starboard_flame_thrower = new FlameThrower(3);
        m_starboard_flame_thrower->SetMaxDamagePerFireballOverride(ms_flame_thrower_max_damage_per_fireball[GetEnemyLevel()]);
        m_starboard_flame_thrower->SetFinalFireballSizeOverride(ms_flame_thrower_final_fireball_size[GetEnemyLevel()]);
        m_starboard_flame_thrower->Equip(this);

        m_starboard_missile_launcher = new MissileLauncher(0);
        m_starboard_missile_launcher->SetSpawnEnemyMissiles(true);
        m_starboard_missile_launcher->Equip(this);
    }

    // aft-port weapon setup
    {
        m_aft_enemy_spawner = new EnemySpawner(GetEnemyLevel());
        m_aft_enemy_spawner->Equip(this);

        m_aft_flame_thrower = new FlameThrower(3);
        m_aft_flame_thrower->SetMaxDamagePerFireballOverride(ms_flame_thrower_max_damage_per_fireball[GetEnemyLevel()]);
        m_aft_flame_thrower->SetFinalFireballSizeOverride(ms_flame_thrower_final_fireball_size[GetEnemyLevel()]);
        m_aft_flame_thrower->Equip(this);

        m_aft_missile_launcher = new MissileLauncher(0);
        m_aft_missile_launcher->SetSpawnEnemyMissiles(true);
        m_aft_missile_launcher->Equip(this);
    }

    SetStrength(D_MINING_LASER);
    SetImmunity(D_COLLISION);
    SetDamageDissipationRate(ms_damage_dissipation_rate[GetEnemyLevel()]);

    Demi::ResetInputs();
}

Demi::~Demi ()
{
    // delete the main weapons
    ASSERT1(m_gauss_gun != NULL)
    Delete(m_gauss_gun);
    ASSERT1(m_flame_thrower != NULL)
    Delete(m_flame_thrower);
    ASSERT1(m_missile_launcher != NULL)
    Delete(m_missile_launcher);
    if (m_reticle_effect.GetIsValid())
    {
        if (m_reticle_effect->GetIsInWorld())
            m_reticle_effect->RemoveFromWorld();
        delete m_reticle_effect->GetOwnerObject();
    }

    // delete the port weapons
    ASSERT1(m_port_tractor != NULL)
    Delete(m_port_tractor);
    ASSERT1(m_port_flame_thrower != NULL)
    Delete(m_port_flame_thrower);
    ASSERT1(m_port_missile_launcher != NULL)
    Delete(m_port_missile_launcher);
    if (m_port_tractor_beam.GetIsValid())
    {
        if (m_port_tractor_beam->GetIsInWorld())
            m_port_tractor_beam->RemoveFromWorld();
        delete m_port_tractor_beam->GetOwnerObject();
    }

    // delete the starboard weapons
    ASSERT1(m_starboard_tractor != NULL)
    Delete(m_starboard_tractor);
    ASSERT1(m_starboard_flame_thrower != NULL)
    Delete(m_starboard_flame_thrower);
    ASSERT1(m_starboard_missile_launcher != NULL)
    Delete(m_starboard_missile_launcher);
    if (m_starboard_tractor_beam.GetIsValid())
    {
        if (m_starboard_tractor_beam->GetIsInWorld())
            m_starboard_tractor_beam->RemoveFromWorld();
        delete m_starboard_tractor_beam->GetOwnerObject();
    }

    // delete the aft weapons
    ASSERT1(m_aft_enemy_spawner != NULL)
    Delete(m_aft_enemy_spawner);
    ASSERT1(m_aft_flame_thrower != NULL)
    Delete(m_aft_flame_thrower);
    ASSERT1(m_aft_missile_launcher != NULL)
    Delete(m_aft_missile_launcher);
}

void Demi::Think (Float const time, Float const frame_dt)
{
    // can't think if we're dead.
    if (GetIsDead())
        return;

    EnemyShip::Think(time, frame_dt);

    // NOTE: Devourment can't be disabled -- the
    // disabled code would go here otherwise

    // ensure the port tractor beam is allocated (lazy allocation)
    if (!m_port_tractor_beam.GetIsValid())
        m_port_tractor_beam = SpawnTractorBeam(GetWorld(), GetObjectLayer())->GetReference();
    // if the port tractor beam is already allocated but not in the world, re-add it.
    else if (!m_port_tractor_beam->GetIsInWorld())
        m_port_tractor_beam->AddBackIntoWorld();
    // set the port tractor beam effect in the Tractor weapon
    m_port_tractor->SetTractorBeam(*m_port_tractor_beam);

    // ensure the starboard tractor beam is allocated (lazy allocation)
    if (!m_starboard_tractor_beam.GetIsValid())
        m_starboard_tractor_beam = SpawnTractorBeam(GetWorld(), GetObjectLayer())->GetReference();
    // if the starboard tractor beam is already allocated but not in the world, re-add it.
    else if (!m_starboard_tractor_beam->GetIsInWorld())
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

    if (GetAngularVelocity() == 0.0f)
    {
        if (m_target.GetIsValid())
            AimShipAtCoordinates(m_target->GetTranslation(), frame_dt);
        else if (GetVelocity().GetLengthSquared() > 0.001f)
            AimShipAtCoordinates(GetTranslation() + GetVelocity(), frame_dt);
    }

    // set the main weapon inputs and activate
    if (m_main_weapon != NULL)
    {
        m_main_weapon->SetInputs(
            GetNormalizedWeaponPrimaryInput(),
            GetNormalizedWeaponSecondaryInput(),
            GetMuzzleLocation(m_main_weapon),
            GetMuzzleDirection(m_main_weapon),
            GetReticleCoordinates());
        m_main_weapon->Activate(
            m_main_weapon->GetPowerToBeUsedBasedOnInputs(time, frame_dt),
            time,
            frame_dt);
    }

    // set the port weapon inputs and activate
    if (m_port_weapon != NULL)
    {
        m_port_weapon->SetInputs(
            GetNormalizedPortWeaponPrimaryInput(),
            GetNormalizedPortWeaponSecondaryInput(),
            GetMuzzleLocation(m_port_weapon),
            GetMuzzleDirection(m_port_weapon),
            m_port_reticle_coordinates);
        m_port_weapon->Activate(
            m_port_weapon->GetPowerToBeUsedBasedOnInputs(time, frame_dt),
            time,
            frame_dt);
    }
    // remove the port tractor beam from the world if necessary
    if (m_port_weapon != m_port_tractor &&
        m_port_tractor_beam.GetIsValid() &&
        m_port_tractor_beam->GetIsInWorld())
    {
        m_port_tractor_beam->ScheduleForRemovalFromWorld(0.0f);
    }

    // set the starboard weapon inputs and activate
    if (m_starboard_weapon != NULL)
    {
        m_starboard_weapon->SetInputs(
            GetNormalizedStarboardWeaponPrimaryInput(),
            GetNormalizedStarboardWeaponSecondaryInput(),
            GetMuzzleLocation(m_starboard_weapon),
            GetMuzzleDirection(m_starboard_weapon),
            m_starboard_reticle_coordinates);
        m_starboard_weapon->Activate(
            m_starboard_weapon->GetPowerToBeUsedBasedOnInputs(time, frame_dt),
            time,
            frame_dt);
    }
    // remove the starboard tractor beam from the world if necessary
    if (m_starboard_weapon != m_starboard_tractor &&
        m_starboard_tractor_beam.GetIsValid() &&
        m_starboard_tractor_beam->GetIsInWorld())
    {
        m_starboard_tractor_beam->ScheduleForRemovalFromWorld(0.0f);
    }

    // set the aft weapon inputs and activate
    if (m_aft_weapon != NULL)
    {
        m_aft_weapon->SetInputs(
            GetNormalizedAftWeaponPrimaryInput(),
            GetNormalizedAftWeaponSecondaryInput(),
            GetMuzzleLocation(m_aft_weapon),
            GetMuzzleDirection(m_aft_weapon),
            FloatVector2::ms_zero); // reticle coords don't matter here
        m_aft_weapon->Activate(
            m_aft_weapon->GetPowerToBeUsedBasedOnInputs(time, frame_dt),
            time,
            frame_dt);
    }

    ResetInputs();
}

void Demi::Die (
    Entity *const killer,
    Entity *const kill_medium,
    FloatVector2 const &kill_location,
    FloatVector2 const &kill_normal,
    Float const kill_force,
    DamageType const kill_type,
    Float const time,
    Float const frame_dt)
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

    // remove the port tractor beam, if it exists
    if (m_port_tractor_beam.GetIsValid() && m_port_tractor_beam->GetIsInWorld())
        m_port_tractor_beam->ScheduleForRemovalFromWorld(0.0f);

    // remove the starboard tractor beam, if it exists
    if (m_starboard_tractor_beam.GetIsValid() && m_starboard_tractor_beam->GetIsInWorld())
        m_starboard_tractor_beam->ScheduleForRemovalFromWorld(0.0f);
}

FloatVector2 Demi::GetMuzzleLocation (Weapon const *weapon) const
{
    ASSERT1(weapon != NULL)
    if (weapon == m_gauss_gun || weapon == m_flame_thrower || weapon == m_missile_launcher)
    {
        return EnemyShip::GetMuzzleLocation(weapon);
    }
    else if (weapon == m_port_tractor || weapon == m_port_flame_thrower || weapon == m_port_missile_launcher)
    {
        return GetTranslation() + GetScaleFactor() * Math::UnitVector(GetAngle() + ms_side_port_angle);
    }
    else if (weapon == m_starboard_tractor || weapon == m_starboard_flame_thrower || weapon == m_starboard_missile_launcher)
    {
        return GetTranslation() + GetScaleFactor() * Math::UnitVector(GetAngle() - ms_side_port_angle);
    }
    else if (weapon == m_aft_enemy_spawner || weapon == m_aft_flame_thrower || weapon == m_aft_missile_launcher)
    {
        return GetTranslation() - GetScaleFactor() * Math::UnitVector(GetAngle());
    }
    else
    {
        ASSERT1(false && "Unknown weapon")
        return FloatVector2::ms_zero;
    }
}

FloatVector2 Demi::GetMuzzleDirection (Weapon const *weapon) const
{
    if (weapon == m_gauss_gun || weapon == m_flame_thrower || weapon == m_missile_launcher)
    {
        FloatVector2 muzzle_location(
            GetObjectLayer()->GetAdjustedCoordinates(
                GetMuzzleLocation(weapon),
                FloatVector2::ms_zero));
        FloatVector2 reticle_offset(
            GetObjectLayer()->GetAdjustedCoordinates(
                GetReticleCoordinates(),
                muzzle_location)
            -
            muzzle_location);
        if (reticle_offset.GetLength() < 0.01f)
            return Math::UnitVector(GetAngle());

        Float reticle_angle = Math::GetCanonicalAngle(Math::Atan(reticle_offset) - GetAngle());
        if (reticle_angle < -ms_weapon_fov[GetEnemyLevel()])
            reticle_angle = -ms_weapon_fov[GetEnemyLevel()];
        else if (reticle_angle > ms_weapon_fov[GetEnemyLevel()])
            reticle_angle = ms_weapon_fov[GetEnemyLevel()];
        reticle_angle += GetAngle();

        return Math::UnitVector(reticle_angle);
    }
    else if (weapon == m_port_tractor || weapon == m_port_flame_thrower || weapon == m_port_missile_launcher)
    {
        return Math::UnitVector(GetAngle() + ms_side_port_angle);
    }
    else if (weapon == m_starboard_tractor || weapon == m_starboard_flame_thrower || weapon == m_starboard_missile_launcher)
    {
        return Math::UnitVector(GetAngle() - ms_side_port_angle);
    }
    else if (weapon == m_aft_enemy_spawner || weapon == m_aft_flame_thrower || weapon == m_aft_missile_launcher)
    {
        return Math::UnitVector(GetAngle() + 180.0f);
    }
    else
    {
        ASSERT1(false && "Unknown weapon")
        return FloatVector2::ms_zero;
    }
}

void Demi::SetTarget (Mortal *const target)
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

void Demi::PickWanderDirection (Float const time, Float const frame_dt)
{
    ASSERT1(!m_target.GetIsValid())

    m_main_weapon = NULL;
    m_port_weapon = NULL;
    m_starboard_weapon = NULL;
    m_aft_weapon = NULL;

    // update the next time to pick a wander direction
    m_next_wander_time = time + 6.0f;
    // pick a direction/speed to wander in
    m_wander_angle = Math::RandomAngle();
    m_wander_angle_low_pass = m_wander_angle;
    m_think_state = THINK_STATE(Wander);
}

void Demi::Wander (Float const time, Float const frame_dt)
{
    ASSERT1(!m_target.GetIsValid())

    static Float const s_scan_radius = 400.0f;

    // scan area for targets
    AreaTraceList area_trace_list;
    GetPhysicsHandler()->AreaTrace(
        GetObjectLayer(),
        GetTranslation(),
        s_scan_radius,
        false,
        &area_trace_list);
    // check the area trace list for targets
    for (AreaTraceListIterator it = area_trace_list.begin(),
                               it_end = area_trace_list.end();
         it != it_end;
         ++it)
    {
        Entity *entity = *it;
        ASSERT1(entity != NULL)

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
    FloatVector2 wander_velocity(ms_wander_speed[GetEnemyLevel()] * Math::UnitVector(m_wander_angle));
    MatchVelocity(wander_velocity, frame_dt);

    // the "slow angle" is used like a low-pass filter for the wander angle
    // in the above calculations.  this is necessary to avoid a feedback loop
    // due to the successive m_wander_angle-dependent calculations.
    static Float const s_slow_angle_delta_rate = 135.0f;
    Float slow_angle_delta =
        Min(s_slow_angle_delta_rate * frame_dt,
            Abs(m_wander_angle - m_wander_angle_low_pass));
    if (m_wander_angle < m_wander_angle_low_pass)
        m_wander_angle_low_pass -= slow_angle_delta;
    else
        m_wander_angle_low_pass += slow_angle_delta;

    if (time >= m_next_wander_time)
        m_think_state = THINK_STATE(PickWanderDirection);
}

void Demi::Stalk (Float const time, Float const frame_dt)
{
    if (!m_target.GetIsValid() || m_target->GetIsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }

    FloatVector2 target_position(
        GetObjectLayer()->GetAdjustedCoordinates(
            m_target->GetTranslation(),
            GetTranslation()));
    Float target_distance = (target_position - GetTranslation()).GetLength();

//     m_think_state = THINK_STATE(ChargeStart);
//     return;

    static WeightedThinkState const s_transition_near[] =
    {
        { THINK_STATE(FlameThrowSweepStart), 2 },
        {   THINK_STATE(MissileLaunchStart), 1 },
        {   THINK_STATE(SpinningEnemySpawn), 2 }
    };
    static Uint32 const s_transition_near_count = sizeof(s_transition_near) / sizeof(WeightedThinkState);
    static Uint32 const s_transition_near_total_weight = 5;

    static WeightedThinkState const s_transition_mid[] =
    {
        {            THINK_STATE(GaussGunStartAim), 10 },
        {        THINK_STATE(FlameThrowBlastStart), 20 },
        {          THINK_STATE(SpinningFlameThrow), 10 },
        {       THINK_STATE(SpinningMissileLaunch), 10 },
        { THINK_STATE(SpinningGuidedMissileLaunch),  2 },
        {        THINK_STATE(EnemySpawnBlastStart),  3 },
        {    THINK_STATE(TractorTargetCloserStart),  3 }
    };
    static Uint32 const s_transition_mid_count = sizeof(s_transition_mid) / sizeof(WeightedThinkState);
    static Uint32 const s_transition_mid_total_weight = 58;

    static WeightedThinkState const s_transition_far[] =
    {
        {     THINK_STATE(EnemySpawnBlastStart),  2 },
        { THINK_STATE(TractorTargetCloserStart),  5 },
        {              THINK_STATE(ChargeStart), 10 }
    };
    static Uint32 const s_transition_far_count = sizeof(s_transition_far) / sizeof(WeightedThinkState);
    static Uint32 const s_transition_far_total_weight = 17;

    WeightedThinkState const *transition = NULL;
    Uint32 transition_count = 0;
    Uint32 transition_total_weight = 0;

    if (target_distance < ms_target_near_range_distance[GetEnemyLevel()])
    {
        transition = s_transition_near;
        transition_count = s_transition_near_count;
        transition_total_weight = s_transition_near_total_weight;
    }
    else if (target_distance < ms_target_mid_range_distance[GetEnemyLevel()])
    {
        transition = s_transition_mid;
        transition_count = s_transition_mid_count;
        transition_total_weight = s_transition_mid_total_weight;
    }
    else
    {
        transition = s_transition_far;
        transition_count = s_transition_far_count;
        transition_total_weight = s_transition_far_total_weight;
    }

    ASSERT1(transition != NULL)
    ASSERT1(transition_count > 0)
    ASSERT1(transition_total_weight > 0)

    Uint32 seed = Math::RandomUint16() % transition_total_weight;
    Uint32 i;
    for (i = 0; i < transition_count; seed -= transition[i].m_weight, ++i)
    {
        ASSERT1(seed < transition_total_weight)
        if (seed < transition[i].m_weight)
        {
            m_think_state = transition[i].m_think_state;
            break;
        }
    }
    ASSERT1(i < transition_count)
}

void Demi::PauseStart (Float const time, Float const frame_dt)
{
    m_start_time = time;
    m_think_state = THINK_STATE(PauseContinue);
}

void Demi::PauseContinue (Float const time, Float const frame_dt)
{
    if (time >= m_start_time + ms_pause_duration[GetEnemyLevel()])
    {
        m_think_state = THINK_STATE(Stalk);
        return;
    }
}

void Demi::ChargeStart (Float const time, Float const frame_dt)
{
    if (!m_target.GetIsValid() || m_target->GetIsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }

    // record the time we picked where to aim
    m_start_time = time;
    // record the direction to charge in
    FloatVector2 target_position(
        GetObjectLayer()->GetAdjustedCoordinates(
            m_target->GetTranslation(),
            GetTranslation()));
    m_charge_velocity = target_position - GetTranslation();
    ASSERT1(!m_charge_velocity.GetIsZero())
    m_charge_velocity.Normalize();
    m_charge_velocity *= 300.0f;

    // transition to and call ChargeAccelerate
    m_think_state = THINK_STATE(ChargeAccelerate);
    ChargeAccelerate(time, frame_dt);
}

void Demi::ChargeAccelerate (Float const time, Float const frame_dt)
{
    if (time >= m_start_time + 1.0f)
    {
        m_start_time = time;
        m_think_state = THINK_STATE(ChargeCoast);
        return;
    }

    MatchVelocity(m_charge_velocity, frame_dt, 10.0f * ms_engine_thrust[GetEnemyLevel()]);
}

void Demi::ChargeCoast (Float const time, Float const frame_dt)
{
    if (time >= m_start_time + 2.0f)
    {
        m_start_time = time;
        m_think_state = THINK_STATE(ChargeDecelerate);
    }
    else if (m_target.GetIsValid() && !m_target->GetIsDead())
    {
        FloatVector2 target_position(
            GetObjectLayer()->GetAdjustedCoordinates(
                m_target->GetTranslation(),
                GetTranslation()));
        Float target_distance = (target_position - GetTranslation()).GetLength();

        static Float const s_threshold_distance = 200.0f;
        if (target_distance <= s_threshold_distance)
        {
            m_start_time = time;
            m_think_state = THINK_STATE(ChargeDecelerate);
        }
    }
}

void Demi::ChargeDecelerate (Float const time, Float const frame_dt)
{
    if (GetSpeed() < 10.0f)
    {
        m_start_time = time;
        m_think_state = THINK_STATE(PauseStart);
        return;
    }

    MatchVelocity(FloatVector2::ms_zero, frame_dt, 10.0f * ms_engine_thrust[GetEnemyLevel()]);
}

void Demi::GaussGunStartAim (Float const time, Float const frame_dt)
{
    if (!m_target.GetIsValid() || m_target->GetIsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }

    m_main_weapon = m_gauss_gun;

    // record the time we picked where to aim
    m_start_time = time;

    ASSERT1(!m_reticle_effect.GetIsValid() || !m_reticle_effect->GetIsInWorld())
    // ensure the reticle effect is allocated (lazy allocation)
    if (!m_reticle_effect.GetIsValid())
        m_reticle_effect =
            SpawnReticleEffect(
                GetWorld(),
                GetObjectLayer(),
                Color(1.0f, 0.0f, 0.0f, 0.5f))->GetReference();
    // if the reticle effect is already allocated but not in the world, re-add it.
    else if (!m_reticle_effect->GetIsInWorld())
        m_reticle_effect->AddBackIntoWorld();
    ASSERT1(m_reticle_effect.GetIsValid() && m_reticle_effect->GetIsInWorld())

    // initialize the reticle coordinates
    m_reticle_effect->SnapToLocationAndSetScaleFactor(
        GetMuzzleLocation(m_gauss_gun),
        0.5f * ms_gauss_gun_reticle_scale_factor[GetEnemyLevel()]);
    SetReticleCoordinates(m_reticle_effect->GetTranslation());

    // transition to and call GaussGunContinueAim
    m_think_state = THINK_STATE(GaussGunContinueAim);
    GaussGunContinueAim(time, frame_dt);
}

void Demi::GaussGunContinueAim (Float const time, Float const frame_dt)
{
    ASSERT1(m_reticle_effect.GetIsValid() && m_reticle_effect->GetIsInWorld())
    ASSERT1(m_main_weapon == m_gauss_gun)

    if (!m_target.GetIsValid() || m_target->GetIsDead())
    {
        ASSERT1(m_reticle_effect.GetIsValid() && m_reticle_effect->GetIsInWorld())
        m_reticle_effect->ScheduleForRemovalFromWorld(0.0f);
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }

    FloatVector2 muzzle_location(
        GetObjectLayer()->GetAdjustedCoordinates(
            GetMuzzleLocation(m_gauss_gun),
            FloatVector2::ms_zero));
    FloatVector2 target_position(
        GetObjectLayer()->GetAdjustedCoordinates(
            m_target->GetTranslation(),
            muzzle_location));
    FloatVector2 reticle_target_position(
        GetObjectLayer()->GetAdjustedCoordinates(
            m_target->GetTranslation(),
            m_reticle_effect->GetTranslation()));
    FloatVector2 aim_direction(reticle_target_position - m_reticle_effect->GetTranslation());
    Float target_distance = (target_position - muzzle_location).GetLength();
    Float reticle_target_distance = aim_direction.GetLength();
    Float distance_parameter =
        reticle_target_distance /
        GaussGun::ms_range[m_gauss_gun->GetUpgradeLevel()];

    // if the target went out of range, or if we're wasting time
    // transition back to sum'm
    if (target_distance > GaussGun::ms_range[m_gauss_gun->GetUpgradeLevel()] ||
        time > m_start_time + ms_gauss_gun_max_duration[GetEnemyLevel()])
    {
        ASSERT1(m_reticle_effect.GetIsValid() && m_reticle_effect->GetIsInWorld())
        m_reticle_effect->ScheduleForRemovalFromWorld(0.0f);
        m_think_state = THINK_STATE(PauseStart);
        return;
    }

    SetReticleCoordinates(m_reticle_effect->GetTranslation());
    m_reticle_effect->SetScaleFactor(
        ms_gauss_gun_reticle_scale_factor[GetEnemyLevel()] *
        (1.0f - 0.5f * distance_parameter));

    ASSERT1(ms_gauss_gun_aim_error_radius[GetEnemyLevel()] > 0.0f)
    if (reticle_target_distance <= ms_gauss_gun_aim_error_radius[GetEnemyLevel()])
    {
        m_think_state = THINK_STATE(GaussGunFire);
        return;
    }
    else
    {
        aim_direction.Normalize();
        m_reticle_effect->SetVelocity(
            ms_gauss_gun_aim_max_speed[GetEnemyLevel()] * aim_direction +
            GetVelocity());
    }
}

void Demi::GaussGunFire (Float const time, Float const frame_dt)
{
    ASSERT1(m_reticle_effect.GetIsValid() && m_reticle_effect->GetIsInWorld())
    m_reticle_effect->ScheduleForRemovalFromWorld(0.0f);

    if (!m_target.GetIsValid() || m_target->GetIsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }

    SetReticleCoordinates(m_reticle_effect->GetTranslation());
    SetWeaponPrimaryInput(UINT8_UPPER_BOUND);

    m_think_state = THINK_STATE(PauseStart);
    SetNextTimeToThink(time + 0.3f);
}

void Demi::FlameThrowSweepStart (Float const time, Float const frame_dt)
{
    m_main_weapon = m_flame_thrower;

    // record the time we started throwing flames
    m_start_time = time;

    // transition to and call FlameThrowSweepContinue
    m_think_state = THINK_STATE(FlameThrowSweepContinue);
    FlameThrowSweepContinue(time, frame_dt);
}

void Demi::FlameThrowSweepContinue (Float const time, Float const frame_dt)
{
    if (time >= m_start_time + ms_flame_throw_sweep_duration[GetEnemyLevel()])
    {
        m_think_state = THINK_STATE(PauseStart);
        return;
    }

    // aim (the constant coefficient is the left/right sweeping speed)
    Float aim_parameter = 180.0f * (time - m_start_time);
    Float aim_angle = ms_weapon_fov[GetEnemyLevel()] * Math::Cos(aim_parameter) + GetAngle();
    // and fire
    SetReticleCoordinates(GetMuzzleLocation(m_main_weapon) + Math::UnitVector(aim_angle));
    SetWeaponPrimaryInput(UINT8_UPPER_BOUND);
}

void Demi::FlameThrowBlastStart (Float const time, Float const frame_dt)
{
    m_main_weapon = m_flame_thrower;
    m_port_weapon = m_port_flame_thrower;
    m_starboard_weapon = m_starboard_flame_thrower;
    m_aft_weapon = m_aft_flame_thrower;

    // record the time we started throwing flames
    m_start_time = time;

    // transition to and call FlameThrowBlastContinue
    m_think_state = THINK_STATE(FlameThrowBlastContinue);
    FlameThrowBlastContinue(time, frame_dt);
}

void Demi::FlameThrowBlastContinue (Float const time, Float const frame_dt)
{
    if (time >= m_start_time + ms_flame_throw_blast_duration[GetEnemyLevel()])
    {
        m_think_state = THINK_STATE(PauseStart);
        return;
    }

    SetReticleCoordinates(GetMuzzleLocation(m_main_weapon) + Math::UnitVector(GetAngle()));
    SetWeaponPrimaryInput(UINT8_UPPER_BOUND);
    SetPortWeaponPrimaryInput(UINT8_UPPER_BOUND);
    SetStarboardWeaponPrimaryInput(UINT8_UPPER_BOUND);
    SetAftWeaponPrimaryInput(UINT8_UPPER_BOUND);
}

void Demi::MissileLaunchStart (Float const time, Float const frame_dt)
{
    if (!m_target.GetIsValid() || m_target->GetIsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }

    m_main_weapon = m_missile_launcher;

    // record the time we started launching missiles
    m_start_time = time;

    // transition to and call MissileLaunchContinue
    m_think_state = THINK_STATE(MissileLaunchContinue);
    MissileLaunchContinue(time, frame_dt);
}

void Demi::MissileLaunchContinue (Float const time, Float const frame_dt)
{
    if (time >= m_start_time + ms_missile_launch_duration[GetEnemyLevel()])
    {
        m_think_state = THINK_STATE(PauseStart);
        return;
    }

    FloatVector2 target_position(
        GetObjectLayer()->GetAdjustedCoordinates(
            m_target->GetTranslation(),
            GetTranslation()));

    // TODO: more accurate aiming for higher enemy levels

    SetReticleCoordinates(target_position);
    SetWeaponPrimaryInput(UINT8_UPPER_BOUND);
}

void Demi::EnemySpawnBlastStart (Float const time, Float const frame_dt)
{
    m_aft_enemy_spawner->SetFireRateOverride(5.0f);
    m_aft_weapon = m_aft_enemy_spawner;
    // record the time we started spawning enemies
    m_start_time = time;
    // transition to and call EnemySpawnBlastContinue
    m_think_state = THINK_STATE(EnemySpawnBlastContinue);
    EnemySpawnBlastContinue(time, frame_dt);
}

void Demi::EnemySpawnBlastContinue (Float const time, Float const frame_dt)
{
    if (time >= m_start_time + ms_enemy_spawn_blast_duration[GetEnemyLevel()])
    {
        m_aft_enemy_spawner->ClearFireRateOverride();
        m_think_state = THINK_STATE(PauseStart);
        return;
    }

    SetAftWeaponPrimaryInput(UINT8_UPPER_BOUND);
}

void Demi::SpinningFlameThrow (Float const time, Float const frame_dt)
{
    // set the appropriate weapons
    m_main_weapon = m_flame_thrower;
    m_port_weapon = m_port_flame_thrower;
    m_starboard_weapon = m_starboard_flame_thrower;
    m_aft_weapon = m_aft_flame_thrower;
    m_spin_accelerate_through_angle = 360.0f;
    m_spin_acceleration_duration = ms_spinning_attack_acceleration_duration[GetEnemyLevel()];
    m_spin_duration = ms_spinning_attack_duration[GetEnemyLevel()];
    m_spinning_attack_uses_secondary_fire = false;
    // transition to and call SpinningAttackStart
    m_think_state = THINK_STATE(SpinningAttackStart);
    SpinningAttackStart(time, frame_dt);
}

void Demi::SpinningMissileLaunch (Float const time, Float const frame_dt)
{
    // set the appropriate weapons
    m_main_weapon = m_missile_launcher;
    m_port_weapon = m_port_missile_launcher;
    m_starboard_weapon = m_starboard_missile_launcher;
    m_aft_weapon = m_aft_missile_launcher;
    m_spin_accelerate_through_angle = 360.0f;
    m_spin_acceleration_duration = ms_spinning_attack_acceleration_duration[GetEnemyLevel()];
    m_spin_duration = ms_spinning_attack_duration[GetEnemyLevel()];
    m_spinning_attack_uses_secondary_fire = false;
    // transition to and call SpinningAttackStart
    m_think_state = THINK_STATE(SpinningAttackStart);
    SpinningAttackStart(time, frame_dt);
}

void Demi::SpinningGuidedMissileLaunch (Float const time, Float const frame_dt)
{
    // set the appropriate weapons
    m_main_weapon = m_missile_launcher;
    m_port_weapon = m_port_missile_launcher;
    m_starboard_weapon = m_starboard_missile_launcher;
    m_aft_weapon = m_aft_missile_launcher;
    m_spin_accelerate_through_angle = 360.0f;
    m_spin_acceleration_duration = ms_spinning_attack_acceleration_duration[GetEnemyLevel()];
    m_spin_duration = ms_spinning_attack_duration[GetEnemyLevel()];
    m_spinning_attack_uses_secondary_fire = true;
    // transition to and call SpinningAttackStart
    m_think_state = THINK_STATE(SpinningAttackStart);
    SpinningAttackStart(time, frame_dt);
}

void Demi::SpinningEnemySpawn (Float const time, Float const frame_dt)
{
    // set the appropriate weapons
    m_main_weapon = NULL;
    m_port_weapon = NULL;
    m_starboard_weapon = NULL;
    m_aft_weapon = m_aft_enemy_spawner;
    m_spin_accelerate_through_angle = 90.0f;
    m_spin_acceleration_duration = ms_spinning_attack_acceleration_duration[GetEnemyLevel()];
    ASSERT1(m_spin_acceleration_duration > 0.0f)
    Float v = 2.0f * m_spin_accelerate_through_angle / m_spin_acceleration_duration;
    ASSERT1(v > 0.0f)
    m_spin_duration = (360.0f - 2.0f * m_spin_accelerate_through_angle) / v;
    m_spinning_attack_uses_secondary_fire = false;
    // transition to and call SpinningAttackStart
    m_think_state = THINK_STATE(SpinningAttackStart);
    SpinningAttackStart(time, frame_dt);
}

void Demi::SpinningAttackStart (Float const time, Float const frame_dt)
{
    // disable the tractor beam think functions
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

void Demi::SpinningAttackAccelerate (Float const time, Float const frame_dt)
{
    // if we're done accelerating the spin, transition to SpinningAttackFire
    ASSERT1(m_spin_acceleration_duration >= 0.0f)
    if (time >= m_start_time + m_spin_acceleration_duration)
    {
        m_start_time = time;
        m_think_state = THINK_STATE(SpinningAttackFire);
        return;
    }

    // calculate the angular acceleration (we want to accelerate
    // through one whole revolution up to the spinning speed).
    // accel = 2*angle/T^2
    ASSERT1(m_spin_accelerate_through_angle > 0.0f)
    ASSERT1(m_spin_acceleration_duration > 0.0f)
    Float const angular_acceleration =
        m_spin_direction * 2.0f * m_spin_accelerate_through_angle /
        (m_spin_acceleration_duration * m_spin_acceleration_duration);
    // accelerate the spin
    SetAngularVelocity(GetAngularVelocity() + angular_acceleration * frame_dt);
}

void Demi::SpinningAttackFire (Float const time, Float const frame_dt)
{
    // if we're done firing, start decelerating
    ASSERT1(m_spin_duration >= 0.0f)
    if (time >= m_start_time + m_spin_duration)
    {
        m_start_time = time;
        m_think_state = THINK_STATE(SpinningAttackDecelerate);
        return;
    }

    SetReticleCoordinates((1.0f + GetScaleFactor()) * Math::UnitVector(GetAngle()));
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

void Demi::SpinningAttackDecelerate (Float const time, Float const frame_dt)
{
    // if we're done decelerating the spin, we're done with this attack
    ASSERT1(m_spin_acceleration_duration >= 0.0f)
    if (time >= m_start_time + m_spin_acceleration_duration)
    {
        SetAngularVelocity(0.0f);
        m_think_state = THINK_STATE(PauseStart);
        m_port_tractor_think_state = THINK_STATE(PortTractorDeflectStuff);
        m_starboard_tractor_think_state = THINK_STATE(StarboardTractorDeflectStuff);
        return;
    }

    // calculate the angular deceleration (we want to decelerate
    // through one whole revolution down to 0 angular speed).
    ASSERT1(m_spin_accelerate_through_angle > 0.0f)
    ASSERT1(m_spin_acceleration_duration > 0.0f)
    Float const angular_acceleration =
        -m_spin_direction * 2.0f * m_spin_accelerate_through_angle /
        (m_spin_acceleration_duration * m_spin_acceleration_duration);
    // accelerate the spin
    SetAngularVelocity(GetAngularVelocity() + angular_acceleration * frame_dt);
}

void Demi::TractorTargetCloserStart (Float const time, Float const frame_dt)
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

void Demi::TractorTargetCloserContinue (Float const time, Float const frame_dt)
{
    if (!m_target.GetIsValid() || m_target->GetIsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        m_port_tractor_think_state = THINK_STATE(PortTractorDeflectStuff);
        m_starboard_tractor_think_state = THINK_STATE(StarboardTractorDeflectStuff);
        return;
    }

    if (time >= m_start_time + ms_tractor_target_closer_duration[GetEnemyLevel()])
    {
        m_think_state = THINK_STATE(FlameThrowSweepStart);
        m_port_tractor_think_state = NULL;
        m_starboard_tractor_think_state = NULL;
        return;
    }
}

void Demi::PortTractorDeflectStuff (Float const time, Float const frame_dt)
{
    m_port_weapon = m_port_tractor;

    FloatVector2 muzzle_location(GetMuzzleLocation(m_port_tractor));
    Entity *best_target =
        FindTractorDeflectTarget(
            muzzle_location,
            GetMuzzleDirection(m_port_tractor),
            time,
            frame_dt);

    if (best_target != NULL)
    {
        muzzle_location =
            GetObjectLayer()->GetAdjustedCoordinates(
                muzzle_location,
                FloatVector2::ms_zero);
        FloatVector2 target_position(
            GetObjectLayer()->GetAdjustedCoordinates(
                best_target->GetTranslation(),
                muzzle_location));
        // aim the tractor and set it to push
        SetPortReticleCoordinates(target_position);
        SetPortWeaponSecondaryInput(UINT8_UPPER_BOUND);
    }
}

void Demi::StarboardTractorDeflectStuff (Float const time, Float const frame_dt)
{
    m_starboard_weapon = m_starboard_tractor;

    FloatVector2 muzzle_location(GetMuzzleLocation(m_starboard_tractor));
    Entity *best_target =
        FindTractorDeflectTarget(
            muzzle_location,
            GetMuzzleDirection(m_starboard_tractor),
            time,
            frame_dt);

    if (best_target != NULL)
    {
        muzzle_location =
            GetObjectLayer()->GetAdjustedCoordinates(
                muzzle_location,
                FloatVector2::ms_zero);
        FloatVector2 target_position(
            GetObjectLayer()->GetAdjustedCoordinates(
                best_target->GetTranslation(),
                muzzle_location));
        // aim the tractor and set it to push
        SetStarboardReticleCoordinates(target_position);
        SetStarboardWeaponSecondaryInput(UINT8_UPPER_BOUND);
    }
}

void Demi::PortTractorPullTargetCloser (Float const time, Float const frame_dt)
{
    if (!m_target.GetIsValid() || m_target->GetIsDead())
        return;

    m_port_weapon = m_port_tractor;

    FloatVector2 muzzle_location(
        GetObjectLayer()->GetAdjustedCoordinates(
            GetMuzzleLocation(m_port_tractor),
            FloatVector2::ms_zero));
    FloatVector2 target_position(
        GetObjectLayer()->GetAdjustedCoordinates(
            m_target->GetTranslation(),
            muzzle_location));
    // aim the tractor and set it to pull
    SetPortReticleCoordinates(target_position);
    SetPortWeaponPrimaryInput(UINT8_UPPER_BOUND);
    SetPortWeaponSecondaryInput(UINT8_UPPER_BOUND);
}

void Demi::StarboardTractorPullTargetCloser (Float const time, Float const frame_dt)
{
    if (!m_target.GetIsValid() || m_target->GetIsDead())
        return;

    m_starboard_weapon = m_starboard_tractor;

    FloatVector2 muzzle_location(
        GetObjectLayer()->GetAdjustedCoordinates(
            GetMuzzleLocation(m_starboard_tractor),
            FloatVector2::ms_zero));
    FloatVector2 target_position(
        GetObjectLayer()->GetAdjustedCoordinates(
            m_target->GetTranslation(),
            muzzle_location));
    // aim the tractor and set it to pull
    SetStarboardReticleCoordinates(target_position);
    SetStarboardWeaponPrimaryInput(UINT8_UPPER_BOUND);
    SetStarboardWeaponSecondaryInput(UINT8_UPPER_BOUND);
}

void Demi::MatchVelocity (FloatVector2 const &velocity, Float const frame_dt, Float max_thrust)
{
    // calculate what thrust is required to match the desired velocity
    FloatVector2 velocity_differential =
        velocity - (GetVelocity() + frame_dt * GetForce() / GetFirstMoment());
    FloatVector2 thrust_vector = GetFirstMoment() * velocity_differential / frame_dt;
    if (!thrust_vector.GetIsZero())
    {
        if (max_thrust < 0.0f)
            max_thrust = ms_engine_thrust[GetEnemyLevel()];

        Float thrust_force = thrust_vector.GetLength();
        if (thrust_force > max_thrust)
            thrust_vector = max_thrust * thrust_vector.GetNormalization();

        AccumulateForce(thrust_vector);
    }
}

Entity *Demi::FindTractorDeflectTarget (
    FloatVector2 const &muzzle_location,
    FloatVector2 const &muzzle_direction,
    Float const time,
    Float const frame_dt)
{
    Float scan_radius = 0.5f * (ms_tractor_beam_radius[GetEnemyLevel()] + GetScaleFactor());
    AreaTraceList area_trace_list;
    GetPhysicsHandler()->AreaTrace(
        GetObjectLayer(),
        muzzle_location + scan_radius * muzzle_direction.GetNormalization(),
        scan_radius,
        false,
        &area_trace_list);

    Entity *best_target = NULL;
    Sint32 best_target_priority = 0;
    for (AreaTraceListIterator it = area_trace_list.begin(),
                               it_end = area_trace_list.end();
         it != it_end;
         ++it)
    {
        Entity *entity = *it;
        ASSERT1(entity != NULL)

        // we don't want to deflect ourselves (it would cancel out anyway)
        if (entity == this)
            continue;

        Sint32 potential_target_priority = -1;

        // only target stuff that's about to collide with us (TODO: verify this works)
        Float collision_time = GetCollisionTime(entity, 2.0f);
        if (collision_time >= 0.0f)
        {
            if (entity->GetIsExplosive() &&
                entity->GetEntityType() != ET_ENEMY_MISSILE &&
                entity->GetEntityType() != ET_GUIDED_ENEMY_MISSILE)
            {
                potential_target_priority = 30 + DStaticCast<Explosive *>(entity)->GetWeaponLevel();
            }
            else if (entity->GetIsBallistic())
            {
                potential_target_priority = 20 + DStaticCast<Ballistic *>(entity)->GetWeaponLevel();
            }
            else if (entity->GetEntityType() == ET_ASTEROID)
            {
                Float target_weight = Min(entity->GetFirstMoment(), 2000.0f) - 1000.0f;
                target_weight *= target_weight;
                target_weight /= -1000000.0f;
                target_weight += 1.0;
                // target weight should now be in [0.0f, 1.0f], and entities with
                // masses closer to 1000 will be weighted closer to 1.0f.
                potential_target_priority = 10 + static_cast<Sint32>(9.0f * target_weight);
            }
        }

        if (best_target == NULL ||
            potential_target_priority > best_target_priority)
        {
            best_target = entity;
            best_target_priority = potential_target_priority;
        }
    }

    return best_target;
}

} // end of namespace Dis
