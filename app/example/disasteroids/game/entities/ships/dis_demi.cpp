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

#include "dis_effect.h"
#include "dis_physicshandler.h"
#include "dis_spawn.h"
#include "dis_weapon.h"
#include "dis_world.h"
#include "xrb_engine2_objectlayer.h"

using namespace Xrb;

#define THINK_STATE(x) &Demi::x

namespace Dis
{

Float const Demi::ms_max_health[ENEMY_LEVEL_COUNT] = { 4000.0f, 6000.0f, 9000.0f, 13500.0f };
Float const Demi::ms_engine_thrust[ENEMY_LEVEL_COUNT] = { 500000.0f, 700000.0f, 925000.0f, 1200000.0f };
Float const Demi::ms_scale_factor[ENEMY_LEVEL_COUNT] = { 55.0f, 65.0f, 75.0f, 85.0f };
Float const Demi::ms_baseline_first_moment[ENEMY_LEVEL_COUNT] = { 10000.0f, 14000.0f, 18500.0f, 24000.0f };
Float const Demi::ms_damage_dissipation_rate[ENEMY_LEVEL_COUNT] = { 0.5f, 1.0f, 2.0f, 4.0f };
Float const Demi::ms_wander_speed[ENEMY_LEVEL_COUNT] = { 30.0f, 40.0f, 50.0f, 60.0f };
Float const Demi::ms_main_weapon_fov[ENEMY_LEVEL_COUNT] = { 45.0f, 45.0f, 45.0f, 45.0f };
Float const Demi::ms_spinning_attack_acceleration_duration[ENEMY_LEVEL_COUNT] = { 0.75f, 0.75f, 0.75f, 0.75f };
Float const Demi::ms_gauss_gun_impact_damage[ENEMY_LEVEL_COUNT] = { 20.0f, 40.0f, 80.0f, 160.0f };
Float const Demi::ms_gauss_gun_aim_error_radius[ENEMY_LEVEL_COUNT] = { 25.0f, 20.0f, 15.0f, 10.0f };
Float const Demi::ms_gauss_gun_aim_max_speed[ENEMY_LEVEL_COUNT] = { 100.0f, 100.0f, 100.0f, 100.0f };
Float const Demi::ms_gauss_gun_reticle_scale_factor[ENEMY_LEVEL_COUNT] = { 20.0f, 20.0f, 20.0f, 20.0f };
Float const Demi::ms_flame_throw_sweep_duration[ENEMY_LEVEL_COUNT] = { 3.0f, 3.0f, 3.0f, 3.0f };
Float const Demi::ms_flame_throw_blast_duration[ENEMY_LEVEL_COUNT] = { 0.15f, 0.15f, 0.15f, 0.15f };
Float const Demi::ms_missile_launch_duration[ENEMY_LEVEL_COUNT] = { 1.0f, 1.0f, 1.0f, 1.0f };
Float const Demi::ms_spinning_missile_launch_duration[ENEMY_LEVEL_COUNT] = { 2.0f, 2.0f, 2.0f, 2.0f };

Float const Demi::ms_side_port_angle = 64.7f;

Demi::Demi (Uint8 const enemy_level)
    :
    EnemyShip(enemy_level, ms_max_health[enemy_level], ET_DEMI)
{
    m_think_state = THINK_STATE(PickWanderDirection);

    m_main_weapon = NULL;
    m_port_weapon = NULL;
    m_starboard_weapon = NULL;
    m_aft_weapon = NULL;

    // main weapon setup
    {
        // the gauss gun weapon level is only used to indicate
        // weapon range and fire rate
        m_gauss_gun = new GaussGun(3);
        m_gauss_gun->Equip(this);
        m_gauss_gun->SetImpactDamageOverride(ms_gauss_gun_impact_damage[GetEnemyLevel()]);

        // TODO: overrides for flame thrower damage/etc
        m_flame_thrower = new FlameThrower(3);
        m_flame_thrower->Equip(this);

        // TODO: overrides for missile launcher damage/etc
        m_missile_launcher = new MissileLauncher(0);
        m_missile_launcher->Equip(this);
        m_missile_launcher->SetSpawnEnemyMissiles(true);
    }

    // port-side weapon setup
    {
        m_port_tractor = new Tractor(3);
        m_port_tractor->Equip(this);

        m_port_flame_thrower = new FlameThrower(3);
        m_port_flame_thrower->Equip(this);

        m_port_missile_launcher = new MissileLauncher(0);
        m_port_missile_launcher->Equip(this);
        m_port_missile_launcher->SetSpawnEnemyMissiles(true);
    }

    // starboard-side weapon setup
    {
        m_starboard_tractor = new Tractor(3);
        m_starboard_tractor->Equip(this);

        m_starboard_flame_thrower = new FlameThrower(3);
        m_starboard_flame_thrower->Equip(this);

        m_starboard_missile_launcher = new MissileLauncher(0);
        m_starboard_missile_launcher->Equip(this);
        m_starboard_missile_launcher->SetSpawnEnemyMissiles(true);
    }

    // aft-port weapon setup
    {
        // TODO: real enemy spawner
        m_aft_enemy_spawner = new PeaShooter(3);
        m_aft_enemy_spawner->Equip(this);

        m_aft_flame_thrower = new FlameThrower(3);
        m_aft_flame_thrower->Equip(this);

        m_aft_missile_launcher = new MissileLauncher(0);
        m_aft_missile_launcher->Equip(this);
        m_aft_missile_launcher->SetSpawnEnemyMissiles(true);
    }

    SetStrength(D_MINING_LASER);
    SetImmunity(D_COLLISION);
    SetDamageDissipationRate(ms_damage_dissipation_rate[GetEnemyLevel()]);
}

Demi::~Demi ()
{
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
}

void Demi::Think (Float const time, Float const frame_dt)
{
    // can't think if we're dead.
    if (GetIsDead())
        return;

    Ship::Think(time, frame_dt);

    // NOTE: Devourment can't be disabled -- the
    // disabled code would go here otherwise

    // call the think state function (which will set the inputs)
    (this->*m_think_state)(time, frame_dt);

    // since enemy ships do not use the PlayerShip device code, engines
    // weapons, etc must be activated/simulated manually here.

    if (GetAngularVelocity() == 0.0f)
    {
        if (m_target.GetIsValid())
            AimShipAtCoordinates(m_target->GetTranslation());
        else if (GetVelocity().GetLengthSquared() > 0.001f)
            SetAngle(Math::Atan(GetVelocity()));
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
            GetNormalizedWeaponPrimaryInput(),
            GetNormalizedWeaponSecondaryInput(),
            GetMuzzleLocation(m_port_weapon),
            GetMuzzleDirection(m_port_weapon),
            GetReticleCoordinates());
        m_port_weapon->Activate(
            m_port_weapon->GetPowerToBeUsedBasedOnInputs(time, frame_dt),
            time,
            frame_dt);
    }
    // set the starboard weapon inputs and activate
    if (m_starboard_weapon != NULL)
    {
        m_starboard_weapon->SetInputs(
            GetNormalizedWeaponPrimaryInput(),
            GetNormalizedWeaponSecondaryInput(),
            GetMuzzleLocation(m_starboard_weapon),
            GetMuzzleDirection(m_starboard_weapon),
            GetReticleCoordinates());
        m_starboard_weapon->Activate(
            m_starboard_weapon->GetPowerToBeUsedBasedOnInputs(time, frame_dt),
            time,
            frame_dt);
    }
    // set the aft weapon inputs and activate
    if (m_aft_weapon != NULL)
    {
        m_aft_weapon->SetInputs(
            GetNormalizedWeaponPrimaryInput(),
            GetNormalizedWeaponSecondaryInput(),
            GetMuzzleLocation(m_aft_weapon),
            GetMuzzleDirection(m_aft_weapon),
            GetReticleCoordinates());
        m_aft_weapon->Activate(
            m_aft_weapon->GetPowerToBeUsedBasedOnInputs(time, frame_dt),
            time,
            frame_dt);
    }

    ResetInputs();
}

FloatVector2 Demi::GetMuzzleLocation (Weapon const *weapon) const
{
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
        if (reticle_offset.GetIsZero())
            return Math::UnitVector(GetAngle());

        Float reticle_angle = Math::Atan(reticle_offset);
        ASSERT1(reticle_angle >= -180.0f && reticle_angle <= 180.0f)
        Float canonical_angle = Math::GetCanonicalAngle(GetAngle());
        reticle_angle -= canonical_angle;
        if (reticle_angle < -ms_main_weapon_fov[GetEnemyLevel()])
            reticle_angle = -ms_main_weapon_fov[GetEnemyLevel()];
        else if (reticle_angle > ms_main_weapon_fov[GetEnemyLevel()])
            reticle_angle = ms_main_weapon_fov[GetEnemyLevel()];
        reticle_angle += canonical_angle;

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
    }
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
//             m_think_state = THINK_STATE(GaussGunStartAim);
//             m_think_state = THINK_STATE(FlameThrowSweepStart);
//             m_think_state = THINK_STATE(FlameThrowBlastStart);
//             m_think_state = THINK_STATE(MissileLaunchStart);
            m_think_state = THINK_STATE(SpinningMissileLaunchStart);
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
            Math::Abs(m_wander_angle - m_wander_angle_low_pass));
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

    // DO LATER
}

// TEMP
// TEMP
// TEMP
void Demi::PauseStart (Float const time, Float const frame_dt)
{
    m_attack_start_time = time;
    m_think_state = THINK_STATE(PauseContinue);
}
void Demi::PauseContinue (Float const time, Float const frame_dt)
{
    if (time >= m_attack_start_time + 1.0f)
    {
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }
}
// TEMP
// TEMP
// TEMP

void Demi::GaussGunStartAim (Float const time, Float const frame_dt)
{
    if (!m_target.GetIsValid() || m_target->GetIsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(PauseStart);
        return;
    }

    m_main_weapon = m_gauss_gun;

    // record the time we picked where to aim
    m_attack_start_time = time;

    ASSERT1(!m_reticle_effect.GetIsValid() || !m_reticle_effect->GetIsInWorld())
    // ensure the reticle effect is allocated (lazy allocation)
    if (!m_reticle_effect.GetIsValid())
        m_reticle_effect = SpawnReticleEffect(GetWorld(), GetObjectLayer(), Color(1.0f, 0.0f, 0.0f, 0.5f))->GetReference();
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
        m_think_state = THINK_STATE(PauseStart);
        return;
    }

    // TODO: maybe add a time-out (we don't want to waste all our time aiming)

    FloatVector2 target_position(
        GetObjectLayer()->GetAdjustedCoordinates(
            m_target->GetTranslation(),
            m_reticle_effect->GetTranslation()));
    FloatVector2 aim_direction(target_position - m_reticle_effect->GetTranslation());
    Float target_distance = aim_direction.GetLength();
    Float distance_parameter = target_distance / GaussGun::ms_range[m_gauss_gun->GetUpgradeLevel()];

    // if the target went out of range, transition back to sum'm
    if (distance_parameter > 1.0f)
    {
        // TEMP - go to Stalk for real
        ASSERT1(m_reticle_effect.GetIsValid() && m_reticle_effect->GetIsInWorld())
        m_reticle_effect->ScheduleForRemovalFromWorld(0.0f);
        m_target.Release();
        m_think_state = THINK_STATE(PauseStart);
        return;
    }

    SetReticleCoordinates(m_reticle_effect->GetTranslation());
    m_reticle_effect->SetScaleFactor(ms_gauss_gun_reticle_scale_factor[GetEnemyLevel()] * (1.0f - 0.5f * distance_parameter));

    ASSERT1(ms_gauss_gun_aim_error_radius[GetEnemyLevel()] > 0.0f)
    if (target_distance <= ms_gauss_gun_aim_error_radius[GetEnemyLevel()])
    {
        m_think_state = THINK_STATE(GaussGunFire);
        return;
    }
    else
    {
        aim_direction.Normalize();
        // TODO: should this velocity be relative to the Demi?
        m_reticle_effect->SetVelocity(ms_gauss_gun_aim_max_speed[GetEnemyLevel()] * aim_direction);
    }
}

void Demi::GaussGunFire (Float const time, Float const frame_dt)
{
    ASSERT1(m_reticle_effect.GetIsValid() && m_reticle_effect->GetIsInWorld())
    m_reticle_effect->ScheduleForRemovalFromWorld(0.0f);

    if (!m_target.GetIsValid() || m_target->GetIsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(PauseStart);
        return;
    }

    FloatVector2 target_position(
        GetObjectLayer()->GetAdjustedCoordinates(
            m_target->GetTranslation(),
            m_reticle_effect->GetTranslation()));

    SetReticleCoordinates(m_reticle_effect->GetTranslation());
    SetWeaponPrimaryInput(UINT8_UPPER_BOUND);

    m_target.Release();
    m_think_state = THINK_STATE(PickWanderDirection);
    SetNextTimeToThink(time + 0.3f);
}

void Demi::FlameThrowSweepStart (Float const time, Float const frame_dt)
{
    m_main_weapon = m_flame_thrower;

    // record the time we started throwing flames
    m_attack_start_time = time;

    // transition to and call FlameThrowSweepContinue
    m_think_state = THINK_STATE(FlameThrowSweepContinue);
    FlameThrowSweepContinue(time, frame_dt);
}

void Demi::FlameThrowSweepContinue (Float const time, Float const frame_dt)
{
    if (time >= m_attack_start_time + ms_flame_throw_sweep_duration[GetEnemyLevel()])
    {
        m_target.Release();
        m_think_state = THINK_STATE(PauseStart);
        return;
    }

    // aim (the constant coefficient is the left/right sweeping speed)
    Float aim_parameter = 180.0f * (time - m_attack_start_time);
    Float aim_angle = ms_main_weapon_fov[GetEnemyLevel()] * Math::Cos(aim_parameter) + GetAngle();
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
    m_attack_start_time = time;

    // transition to and call FlameThrowBlastContinue
    m_think_state = THINK_STATE(FlameThrowBlastContinue);
    FlameThrowBlastContinue(time, frame_dt);
}

void Demi::FlameThrowBlastContinue (Float const time, Float const frame_dt)
{
    if (time >= m_attack_start_time + ms_flame_throw_blast_duration[GetEnemyLevel()])
    {
        m_target.Release();
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
        m_think_state = THINK_STATE(PauseStart);
        return;
    }

    m_main_weapon = m_missile_launcher;

    // record the time we started launching missiles
    m_attack_start_time = time;

    // transition to and call MissileLaunchContinue
    m_think_state = THINK_STATE(MissileLaunchContinue);
    MissileLaunchContinue(time, frame_dt);
}

void Demi::MissileLaunchContinue (Float const time, Float const frame_dt)
{
    if (time >= m_attack_start_time + ms_missile_launch_duration[GetEnemyLevel()])
    {
        m_target.Release();
        m_think_state = THINK_STATE(PauseStart);
        return;
    }

    FloatVector2 target_position(
        GetObjectLayer()->GetAdjustedCoordinates(
            m_target->GetTranslation(),
            GetTranslation()));

    SetReticleCoordinates(target_position);
    SetWeaponPrimaryInput(UINT8_UPPER_BOUND);
}

void Demi::SpinningMissileLaunchStart (Float const time, Float const frame_dt)
{
    m_main_weapon = m_missile_launcher;
    m_port_weapon = m_port_missile_launcher;
    m_starboard_weapon = m_starboard_missile_launcher;
    m_aft_weapon = m_aft_missile_launcher;

    // record the time we started launching missiles
    m_attack_start_time = time;
    // randomly decide to go counter/clockwise
    m_spin_direction = (Math::RandomUint16()&1) == 0 ? 1.0f : -1.0f;

    fprintf(stderr, "Demi::SpinningMissileLaunchStart();\n");

    fprintf(stderr, "Demi::SpinningMissileLaunchAccelerate();\n");

    // transition to and call SpinningMissileLaunchAccelerate
    m_think_state = THINK_STATE(SpinningMissileLaunchAccelerate);
    SpinningMissileLaunchAccelerate(time, frame_dt);
}

void Demi::SpinningMissileLaunchAccelerate (Float const time, Float const frame_dt)
{
    // if we're done accelerating the spin, transition to SpinningMissileLaunchFire
    if (time >= m_attack_start_time + ms_spinning_attack_acceleration_duration[GetEnemyLevel()])
    {
        m_attack_start_time = time;
        fprintf(stderr, "Demi::SpinningMissileLaunchFire();\n");
        m_think_state = THINK_STATE(SpinningMissileLaunchFire);
        return;
    }

    // calculate the angular acceleration (we want to accelerate
    // through one whole revolution up to the spinning speed).
    // accel = 2*angle/T^2
    Float const angular_acceleration =
        m_spin_direction * 2.0f * 360.0f /
        (ms_spinning_attack_acceleration_duration[GetEnemyLevel()] *
         ms_spinning_attack_acceleration_duration[GetEnemyLevel()]);
    // accelerate the spin
    SetAngularVelocity(GetAngularVelocity() + angular_acceleration * frame_dt);
}

void Demi::SpinningMissileLaunchFire (Float const time, Float const frame_dt)
{
    // if we're done firing, start decelerating
    if (time >= m_attack_start_time + ms_spinning_missile_launch_duration[GetEnemyLevel()])
    {
        m_attack_start_time = time;
        fprintf(stderr, "Demi::SpinningMissileLaunchDecelerate();\n");
        m_think_state = THINK_STATE(SpinningMissileLaunchDecelerate);
        return;
    }

    SetReticleCoordinates(GetMuzzleLocation(m_main_weapon) + Math::UnitVector(GetAngle()));
//     SetWeaponPrimaryInput(UINT8_UPPER_BOUND);
//     SetPortWeaponPrimaryInput(UINT8_UPPER_BOUND);
//     SetStarboardWeaponPrimaryInput(UINT8_UPPER_BOUND);
//     SetAftWeaponPrimaryInput(UINT8_UPPER_BOUND);
    SetWeaponSecondaryInput(UINT8_UPPER_BOUND);
    SetPortWeaponSecondaryInput(UINT8_UPPER_BOUND);
    SetStarboardWeaponSecondaryInput(UINT8_UPPER_BOUND);
    SetAftWeaponSecondaryInput(UINT8_UPPER_BOUND);
}

void Demi::SpinningMissileLaunchDecelerate (Float const time, Float const frame_dt)
{
    // if we're done decelerating the spin, we're done with this attack
    if (time >= m_attack_start_time + ms_spinning_attack_acceleration_duration[GetEnemyLevel()])
    {
        m_target.Release();
        fprintf(stderr, "Demi::PauseStart();\n");
        SetAngularVelocity(0.0f);
        m_think_state = THINK_STATE(PauseStart);
        return;
    }

    // calculate the angular deceleration (we want to decelerate
    // through one whole revolution down to 0 angular speed).
    Float const angular_acceleration =
        -m_spin_direction * 2.0f * 360.0f /
        (ms_spinning_attack_acceleration_duration[GetEnemyLevel()] *
         ms_spinning_attack_acceleration_duration[GetEnemyLevel()]);
    // accelerate the spin
    SetAngularVelocity(GetAngularVelocity() + angular_acceleration * frame_dt);
}

void Demi::MatchVelocity (FloatVector2 const &velocity, Float const frame_dt)
{
    // calculate what thrust is required to match the desired velocity
    FloatVector2 velocity_differential =
        velocity - (GetVelocity() + frame_dt * GetForce() / GetFirstMoment());
    FloatVector2 thrust_vector = GetFirstMoment() * velocity_differential / frame_dt;
    if (!thrust_vector.GetIsZero())
    {
        Float thrust_force = thrust_vector.GetLength();
        if (thrust_force > ms_engine_thrust[GetEnemyLevel()])
            thrust_vector = ms_engine_thrust[GetEnemyLevel()] * thrust_vector.GetNormalization();

        AccumulateForce(thrust_vector);
    }
}

} // end of namespace Dis
