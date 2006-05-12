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
Float const Demi::ms_gauss_gun_impact_damage[ENEMY_LEVEL_COUNT] = { 20.0f, 40.0f, 80.0f, 160.0f };
Float const Demi::ms_gauss_gun_aim_error_radius[ENEMY_LEVEL_COUNT] = { 25.0f, 20.0f, 15.0f, 10.0f };
Float const Demi::ms_gauss_gun_aim_max_speed[ENEMY_LEVEL_COUNT] = { 100.0f, 100.0f, 100.0f, 100.0f };
Float const Demi::ms_gauss_gun_reticle_scale_factor[ENEMY_LEVEL_COUNT] = { 20.0f, 20.0f, 20.0f, 20.0f };

Demi::Demi (Uint8 const enemy_level)
    :
    EnemyShip(enemy_level, ms_max_health[enemy_level], ET_DEMI)
{
    m_think_state = THINK_STATE(PickWanderDirection);

    m_main_weapon = NULL;

    // the gauss gun weapon level is only used to indicate
    // weapon range and fire rate
    m_gauss_gun = new GaussGun(1);
    m_gauss_gun->Equip(this);
    m_gauss_gun->SetImpactDamageOverride(ms_gauss_gun_impact_damage[GetEnemyLevel()]);

    // TODO: overrides for flame thrower damage/etc
    m_flame_thrower = new FlameThrower(3);
    m_flame_thrower->Equip(this);

    // TODO: overrides for missile launcher damage/etc
    m_missile_launcher = new MissileLauncher(3);
    m_missile_launcher->Equip(this);

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

    if (m_main_weapon != NULL)
    {
        // set the main weapon inputs and activate
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

    if (m_target.GetIsValid())
        AimShipAtCoordinates(m_target->GetTranslation());
    else if (GetVelocity().GetLengthSquared() > 0.001f)
        SetAngle(Math::Atan(GetVelocity()));

    ResetInputs();
}
/*
FloatVector2 Demi::GetMuzzleLocation (Weapon const *weapon) const
{
}

FloatVector2 Demi::GetMuzzleDirection (Weapon const *weapon) const
{
}
*/
void Demi::PickWanderDirection (Float const time, Float const frame_dt)
{
    ASSERT1(!m_target.GetIsValid())

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
            m_think_state = THINK_STATE(GaussGunStartAim);
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
        m_think_state = THINK_STATE(PickWanderDirection);
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
        m_think_state = THINK_STATE(PickWanderDirection);
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
        m_think_state = THINK_STATE(PickWanderDirection);
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
