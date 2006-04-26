// ///////////////////////////////////////////////////////////////////////////
// dis_revulsion.cpp by Victor Dods, created 2006/01/24
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_revulsion.h"

#include "dis_physicshandler.h"
#include "dis_spawn.h"
#include "dis_weapon.h"
#include "dis_world.h"
#include "xrb_engine2_objectlayer.h"
#include "xrb_polynomial.h"

using namespace Xrb;

#define THINK_STATE(x) &Revulsion::x

namespace Dis
{

Float const Revulsion::ms_max_health[ENEMY_LEVEL_COUNT] = { 15.0f, 30.0f, 60.0f, 120.0f };
Float const Revulsion::ms_engine_thrust[ENEMY_LEVEL_COUNT] = { 8000.0f, 14000.0f, 32000.0f, 72000.0f };
Float const Revulsion::ms_scale_factor[ENEMY_LEVEL_COUNT] = { 10.0f, 12.0f, 15.0f, 18.0f };
Float const Revulsion::ms_baseline_first_moment[ENEMY_LEVEL_COUNT] = { 40.0f, 80.0f, 160.0f, 320.0f };
Float const Revulsion::ms_damage_dissipation_rate[ENEMY_LEVEL_COUNT] = { 0.5f, 1.0f, 2.0f, 4.0f };
Float const Revulsion::ms_weapon_impact_damage[ENEMY_LEVEL_COUNT] = { 10.0f, 20.0f, 40.0f, 80.0f };
Float const Revulsion::ms_target_aim_angle_flee_limit[ENEMY_LEVEL_COUNT] = { 60.0f, 50.0f, 40.0f, 30.0f };
Float const Revulsion::ms_target_aim_angle_trail_limit[ENEMY_LEVEL_COUNT] = { 110.0f, 120.0f, 130.0f, 140.0f };
Float const Revulsion::ms_preferred_location_distance_tolerance[ENEMY_LEVEL_COUNT] = { 50.0f, 75.0f, 100.0f, 150.0f };
Float const Revulsion::ms_aim_duration[ENEMY_LEVEL_COUNT] = { 0.7f, 0.7f, 0.7f, 0.7f };
Float const Revulsion::ms_aim_error_radius[ENEMY_LEVEL_COUNT] = { 25.0f, 20.0f, 15.0f, 10.0f };
Float const Revulsion::ms_flee_speed[ENEMY_LEVEL_COUNT] = { 150.0f, 200.0f, 250.0f, 300.0f };

Revulsion::Revulsion (Uint8 const enemy_level)
    :
    EnemyShip(enemy_level, ms_max_health[enemy_level], ET_REVULSION)
{
    m_think_state = THINK_STATE(Seek);

    m_weapon = new GaussGun(0);
    m_weapon->Equip(this);
    m_weapon->SetImpactDamageOverride(ms_weapon_impact_damage[GetEnemyLevel()]);

    SetStrength(D_MINING_LASER);
    SetDamageDissipationRate(ms_damage_dissipation_rate[GetEnemyLevel()]);
}

Revulsion::~Revulsion ()
{
    ASSERT1(m_weapon != NULL)
    Delete(m_weapon);

    if (m_reticle_effect.GetIsValid())
    {
        if (m_reticle_effect->GetIsInWorld())
            m_reticle_effect->RemoveFromWorld();
        delete m_reticle_effect->GetOwnerObject();
    }
}

void Revulsion::Think (Float const time, Float const frame_dt)
{
    bool is_disabled = GetIsDisabled();
    Ship::Think(time, frame_dt);
    if (is_disabled)
    {
        if (m_reticle_effect.GetIsValid() && m_reticle_effect->GetIsInWorld())
            m_reticle_effect->RemoveFromWorld();
        
        // if disabled, then reset the think state to Seek (a way out for
        // players that are being ganged up on)
        m_think_state = THINK_STATE(Seek);
        return;
    }

    // call the think state function (which will set the inputs)
    (this->*m_think_state)(time, frame_dt);

    // since enemy ships do not use the PlayerShip device code, engines
    // weapons, etc must be activated/simulated manually here.

    AimShipAtReticleCoordinates();
    // apply ship thrust in the appropriate direction
    AccumulateForce(
        GetNormalizedEngineUpDownInput() *
        ms_engine_thrust[GetEnemyLevel()] *
        Math::UnitVector(GetAngle()));
    // set the weapon inputs and activate
    m_weapon->SetInputs(
        GetNormalizedWeaponPrimaryInput(),
        GetNormalizedWeaponSecondaryInput(),
        GetMuzzleLocation(m_weapon),
        GetMuzzleDirection(m_weapon),
        GetReticleCoordinates());
    m_weapon->Activate(
        m_weapon->GetPowerToBeUsedBasedOnInputs(time, frame_dt),
        time,
        frame_dt);

    ResetInputs();
}

void Revulsion::Die (
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

    // remove the reticle effect, if it exists
    if (m_reticle_effect.GetIsValid() && m_reticle_effect->GetIsInWorld())
        m_reticle_effect->RemoveFromWorld();
}

void Revulsion::Seek (Float const time, Float const frame_dt)
{
    static Float const s_seek_scan_radius = 400.0f;

    ASSERT1(!m_reticle_effect.GetIsValid() || !m_reticle_effect->GetIsInWorld())
    
    // do an area trace
    AreaTraceList area_trace_list;
    GetPhysicsHandler()->AreaTrace(
        GetObjectLayer(),
        GetTranslation(),
        s_seek_scan_radius,
        false,
        &area_trace_list);
        
    // check if there is a solitary in the area
    for (AreaTraceListIterator it = area_trace_list.begin(),
                               it_end = area_trace_list.end();
         it != it_end;
         ++it)
    {
        Entity *entity = *it;
        ASSERT1(entity != NULL)
        if (entity->GetEntityType() == ET_SOLITARY)
        {
            // if so, set m_target and transition to TrailTarget
            m_target = entity->GetReference();
            FloatVector2 target_position(
                GetObjectLayer()->GetAdjustedCoordinates(
                    m_target->GetTranslation(),
                    GetTranslation()));    
            m_think_state = THINK_STATE(TrailTarget);
            return;
        }
    }

    // wander around - TODO: real wandering
    SetReticleCoordinates(GetTranslation() + Math::UnitVector(GetAngle()));
    
    // if not, set next think time to a while later
    SetNextTimeToThink(time + Math::RandomFloat(0.3f, 0.5f));
}

void Revulsion::TrailTarget (Float const time, Float const frame_dt)
{
    ASSERT1(!m_reticle_effect.GetIsValid() || !m_reticle_effect->GetIsInWorld())

    if (!m_target.GetIsValid() || m_target->GetIsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(Seek);
        return;
    }
    
    FloatVector2 target_position(
        GetObjectLayer()->GetAdjustedCoordinates(
            m_target->GetTranslation(),
            GetTranslation()));    
    Float target_aim_angle = GetTargetAimAngle(target_position);
    // if we're too close to the target's line of fire, transition to FleeTarget
    if (target_aim_angle >= -ms_target_aim_angle_flee_limit[GetEnemyLevel()] &&
        target_aim_angle <=  ms_target_aim_angle_flee_limit[GetEnemyLevel()])
    {
        m_think_state = THINK_STATE(FleeTarget);
        return;
    }
    
    // figure out where to go to get behind the target
    FloatVector2 preferred_location(
        GetObjectLayer()->GetAdjustedCoordinates(
            -0.5f * GaussGun::ms_range[m_weapon->GetUpgradeLevel()] * Math::UnitVector(m_target->GetAngle()) + m_target->GetTranslation(),
            GetTranslation()));

    // if we're close enough to the spot behind the target (and we're
    // ready to fire), transition to fire
    Float distance_to_preferred_location = (preferred_location - GetTranslation()).GetLength();
    if (distance_to_preferred_location <= ms_preferred_location_distance_tolerance[GetEnemyLevel()] &&
        m_weapon->GetReadinessStatus(time) == 1.0f)
    {
        m_think_state = THINK_STATE(StartAimAtTarget);
        return;
    }

    // otherwise, plot an intercept course with the preferred location
    
    Float interceptor_acceleration =
        ms_engine_thrust[GetEnemyLevel()] / GetFirstMoment();
    FloatVector2 p(preferred_location - GetTranslation());
    FloatVector2 v(m_target->GetVelocity() - GetVelocity());
    FloatVector2 a(m_target->GetForce() / m_target->GetFirstMoment());

    Polynomial poly;
    poly.Set(4, a.GetLengthSquared() - interceptor_acceleration*interceptor_acceleration);
    poly.Set(3, 4.0f * (a | v));
    poly.Set(2, 4.0f * ((a | p) + (v | v)));
    poly.Set(1, 8.0f * (p | v));
    poly.Set(0, 4.0f * (p | p));

    Polynomial::SolutionSet solution_set;
    poly.Solve(&solution_set, 0.001f);

    Float T = -1.0f;
    for (Polynomial::SolutionSetIterator it = solution_set.begin(),
                                         it_end = solution_set.end();
         it != it_end;
         ++it)
    {
        if (*it >= 0.0f)
        {
            T = *it;
            break;
        }
    }

    if (T <= 0.0f)
    {
        // if no acceptable solution, just do dumb approach
        SetReticleCoordinates(preferred_location);
    }
    else
    {
        FloatVector2 real_approach_direction((2.0f*p + 2.0f*v*T + a*T*T) / (interceptor_acceleration*T*T));
        SetReticleCoordinates(GetTranslation() + real_approach_direction);
    }
    SetEngineUpDownInput(SINT8_UPPER_BOUND);
}

void Revulsion::StartAimAtTarget (Float time, Float frame_dt)
{
    if (!m_target.GetIsValid() || m_target->GetIsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(Seek);
        return;
    }

    // record the time we picked where to aim
    m_aim_start_time = time;

    // figure out where to aim (here is where the aim accuracy is determined)
    m_aim_delta =
        ms_aim_error_radius[GetEnemyLevel()] *
        Math::Sqrt(Math::RandomFloat(0.0f, 1.0f)) *
        Math::UnitVector(Math::RandomAngle());

    ASSERT1(!m_reticle_effect.GetIsValid() || !m_reticle_effect->GetIsInWorld())
    // ensure the reticle effect is allocated (lazy allocation)
    if (!m_reticle_effect.GetIsValid())
        m_reticle_effect = SpawnReticleEffect(GetWorld(), GetObjectLayer(), Color(1.0f, 0.0f, 0.0f, 0.5f))->GetReference();
    // if the reticle effect is already allocated but not in the world, re-add it.
    else if (!m_reticle_effect->GetIsInWorld())
        m_reticle_effect->AddBackIntoWorld();
    ASSERT1(m_reticle_effect.GetIsValid() || m_reticle_effect->GetIsInWorld())
        
    // transition to and call ContinueAimAtTarget 
    m_think_state = THINK_STATE(ContinueAimAtTarget);
    ContinueAimAtTarget(time, frame_dt);
}

void Revulsion::ContinueAimAtTarget (Float time, Float frame_dt)
{
    if (!m_target.GetIsValid() || m_target->GetIsDead())
    {
        ASSERT1(m_reticle_effect.GetIsValid() && m_reticle_effect->GetIsInWorld())
        m_reticle_effect->RemoveFromWorld();    
        m_target.Release();
        m_think_state = THINK_STATE(Seek);
        return;
    }
    
    FloatVector2 target_position(
        GetObjectLayer()->GetAdjustedCoordinates(
            m_target->GetTranslation(),
            GetTranslation()));    
    Float target_aim_angle = GetTargetAimAngle(target_position);
    // if we're too close to the target's line of fire, transition to FleeTarget
    if (target_aim_angle >= -ms_target_aim_angle_flee_limit[GetEnemyLevel()] &&
        target_aim_angle <=  ms_target_aim_angle_flee_limit[GetEnemyLevel()])
    {
        ASSERT1(m_reticle_effect.GetIsValid() && m_reticle_effect->GetIsInWorld())
        m_reticle_effect->RemoveFromWorld();    
        m_think_state = THINK_STATE(FleeTarget);
        return;
    }
                
    ASSERT1(ms_aim_duration[GetEnemyLevel()] > 0.0f)
    Float aim_time_parameter = Min(1.0f, 0.25f + 0.75f * (time - m_aim_start_time) / ms_aim_duration[GetEnemyLevel()]);
    SetReticleCoordinates(
        aim_time_parameter * (target_position + m_aim_delta) +
        (1.0f - aim_time_parameter) * GetTranslation());

    // update the reticle's location and scale factor
    ASSERT1(m_reticle_effect.GetIsValid() && m_reticle_effect->GetIsInWorld())
    static Float const s_final_reticle_radius = 20.0f;
    m_reticle_effect->SnapToLocationAndSetScaleFactor(
        GetReticleCoordinates(),
        s_final_reticle_radius * aim_time_parameter);
        
    // if the aim duration has elapsed, transition to FireAtTarget
    if (time - m_aim_start_time >= ms_aim_duration[GetEnemyLevel()])
        m_think_state = THINK_STATE(FireAtTarget);
}

void Revulsion::FireAtTarget (Float const time, Float const frame_dt)
{
    ASSERT1(m_reticle_effect.GetIsValid() && m_reticle_effect->GetIsInWorld())
    m_reticle_effect->RemoveFromWorld();
    
    if (!m_target.GetIsValid() || m_target->GetIsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(Seek);
        return;
    }

    FloatVector2 target_position(
        GetObjectLayer()->GetAdjustedCoordinates(
            m_target->GetTranslation(),
            GetTranslation()));

    // TODO: set weapon firing input and inaccurate aiming
    SetReticleCoordinates(target_position + m_aim_delta);
    SetWeaponPrimaryInput(UINT8_UPPER_BOUND);

    m_think_state = THINK_STATE(TrailTarget);
    SetNextTimeToThink(time + 0.3f);
}

void Revulsion::FleeTarget (Float const time, Float const frame_dt)
{
    if (!m_target.GetIsValid() || m_target->GetIsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(Seek);
        return;
    }

    FloatVector2 target_position(
        GetObjectLayer()->GetAdjustedCoordinates(
            m_target->GetTranslation(),
            GetTranslation()));    
    Float target_aim_angle = GetTargetAimAngle(target_position);
    // if we're far enough behind the target, transition to TrailTarget
    if (target_aim_angle <= -ms_target_aim_angle_trail_limit[GetEnemyLevel()] ||
        target_aim_angle >=  ms_target_aim_angle_trail_limit[GetEnemyLevel()])
    {
        m_think_state = THINK_STATE(TrailTarget);
        return;
    }

    FloatVector2 position_delta(GetTranslation() - target_position);
    FloatVector2 desired_velocity(ms_flee_speed[GetEnemyLevel()] * GetPerpendicularVector2(position_delta).GetNormalization());
    FloatVector2 velocity_differential(
        desired_velocity - 
        (GetVelocity() + frame_dt * GetForce() / GetFirstMoment()));
    FloatVector2 thrust_vector = GetFirstMoment() * velocity_differential / frame_dt;
    if (!thrust_vector.GetIsZero())
    {
        Float thrust_force = thrust_vector.GetLength();
        if (thrust_force > ms_engine_thrust[GetEnemyLevel()])
            thrust_vector = ms_engine_thrust[GetEnemyLevel()] * thrust_vector.GetNormalization();
        thrust_force = thrust_vector.GetLength();

        SetReticleCoordinates(GetTranslation() + Math::UnitVector(Math::Atan(thrust_vector)));
        SetEngineUpDownInput(
            static_cast<Sint8>(
                static_cast<Float>(SINT8_UPPER_BOUND) * thrust_force / ms_engine_thrust[GetEnemyLevel()]));
    }
}

} // end of namespace Dis
