// ///////////////////////////////////////////////////////////////////////////
// dis_revulsion.cpp by Victor Dods, created 2006/01/24
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_revulsion.hpp"

#include "dis_effect.hpp"
#include "dis_spawn.hpp"
#include "dis_weapon.hpp"
#include "dis_world.hpp"
#include "xrb_engine2_circle_physicshandler.hpp"
#include "xrb_engine2_objectlayer.hpp"
#include "xrb_polynomial.hpp"

using namespace Xrb;

#define THINK_STATE(x) &Revulsion::x

namespace Dis {

Float const Revulsion::ms_max_health[ENEMY_LEVEL_COUNT] = { 15.0f, 60.0f, 240.0f, 960.0f };
Float const Revulsion::ms_engine_thrust[ENEMY_LEVEL_COUNT] = { 8000.0f, 14000.0f, 32000.0f, 72000.0f };
Float const Revulsion::ms_max_angular_velocity[ENEMY_LEVEL_COUNT] = { 360.0f, 360.0f, 360.0f, 360.0f };
Float const Revulsion::ms_ship_radius[ENEMY_LEVEL_COUNT] = { 10.0f, 12.0f, 15.0f, 18.0f };
Float const Revulsion::ms_baseline_mass[ENEMY_LEVEL_COUNT] = { 40.0f, 80.0f, 160.0f, 320.0f };
Float const Revulsion::ms_damage_dissipation_rate[ENEMY_LEVEL_COUNT] = { 0.5f, 1.0f, 2.0f, 4.0f };
Float const Revulsion::ms_weapon_impact_damage[ENEMY_LEVEL_COUNT] = { 8.0f, 16.0f, 28.0f, 50.0f };
Float const Revulsion::ms_target_aim_angle_flee_limit[ENEMY_LEVEL_COUNT] = { 60.0f, 50.0f, 40.0f, 30.0f };
Float const Revulsion::ms_target_aim_angle_trail_limit[ENEMY_LEVEL_COUNT] = { 110.0f, 120.0f, 130.0f, 140.0f };
Float const Revulsion::ms_preferred_location_distance_tolerance[ENEMY_LEVEL_COUNT] = { 50.0f, 75.0f, 100.0f, 150.0f };
Time::Delta const Revulsion::ms_aim_duration[ENEMY_LEVEL_COUNT] = { 0.7f, 0.7f, 0.7f, 0.7f };
Float const Revulsion::ms_aim_error_radius[ENEMY_LEVEL_COUNT] = { 25.0f, 20.0f, 15.0f, 10.0f };
Float const Revulsion::ms_flee_speed[ENEMY_LEVEL_COUNT] = { 150.0f, 200.0f, 250.0f, 300.0f };
Float const Revulsion::ms_wander_speed[ENEMY_LEVEL_COUNT] = { 100.0f, 100.0f, 100.0f, 100.0f };

Revulsion::Revulsion (Uint8 enemy_level)
    :
    EnemyShip(enemy_level, ms_max_health[enemy_level], ET_REVULSION)
{
    m_think_state = THINK_STATE(PickWanderDirection);

    m_weapon = new GaussGun(0);
    m_weapon->Equip(this);
    m_weapon->SetImpactDamageOverride(ms_weapon_impact_damage[EnemyLevel()]);

    SetStrength(D_MINING_LASER|D_COLLISION);
    SetDamageDissipationRate(ms_damage_dissipation_rate[EnemyLevel()]);
}

Revulsion::~Revulsion ()
{
    ASSERT1(m_weapon != NULL);
    Delete(m_weapon);

    if (m_reticle_effect.IsValid())
    {
        if (m_reticle_effect->IsInWorld())
            m_reticle_effect->RemoveFromWorld();
        delete m_reticle_effect->OwnerObject();
    }
}

void Revulsion::Think (Time time, Time::Delta frame_dt)
{
    // can't think if we're dead.
    if (IsDead())
        return;

    bool is_disabled = IsDisabled();
    Ship::Think(time, frame_dt);
    if (is_disabled)
    {
        if (m_reticle_effect.IsValid() && m_reticle_effect->IsInWorld())
            m_reticle_effect->ScheduleForRemovalFromWorld(0.0f);

        // if disabled, then reset the think state to PickWanderDirection (a way out for
        // players that are being ganged up on)
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }

    // call the think state function (which will set the inputs)
    (this->*m_think_state)(time, frame_dt);

    // since enemy ships do not use the PlayerShip device code, engines
    // weapons, etc must be activated/simulated manually here.

    AimShipAtReticleCoordinates(frame_dt);
    // apply ship thrust in the appropriate direction
    AccumulateForce(
        NormalizedEngineUpDownInput() *
        ms_engine_thrust[EnemyLevel()] *
        Math::UnitVector(Angle()));
    // set the weapon inputs and activate
    m_weapon->SetInputs(
        NormalizedWeaponPrimaryInput(),
        NormalizedWeaponSecondaryInput(),
        MuzzleLocation(m_weapon),
        MuzzleDirection(m_weapon),
        ReticleCoordinates());
    m_weapon->Activate(
        m_weapon->PowerToBeUsedBasedOnInputs(false, false, time, frame_dt),
        false, // no attack boost
        false, // no defense boost
        time,
        frame_dt);

    ResetInputs();
}

void Revulsion::Die (
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

    // remove the reticle effect, if it exists
    if (m_reticle_effect.IsValid() && m_reticle_effect->IsInWorld())
        m_reticle_effect->ScheduleForRemovalFromWorld(0.0f);
}

void Revulsion::SetTarget (Mortal *target)
{
    if (target == NULL)
        m_target.Release();
    else
    {
        m_target = target->GetReference();
        m_think_state = THINK_STATE(TrailTarget);
    }
}

void Revulsion::PickWanderDirection (Time time, Time::Delta frame_dt)
{
    // update the next time to pick a wander direction
    m_next_wander_time = time + 6.0f;
    // pick a direction/speed to wander in
    m_wander_angle = Math::RandomAngle();
    m_wander_angle_derivative = 0.0f;
    m_think_state = THINK_STATE(Wander);
}

void Revulsion::Wander (Time time, Time::Delta frame_dt)
{
    static Float const s_scan_radius = 200.0f;
    static Time::Delta const s_collision_lookahead_dt = 3.0f;

    // scan area for targets
    Engine2::Circle::AreaTraceList area_trace_list;
    ASSERT1(GetObjectLayer() != NULL);
    GetPhysicsHandler()->AreaTrace(
        *GetObjectLayer(),
        Translation(),
        s_scan_radius,
        false,
        area_trace_list);
    // check the area trace list for targets and collisions
    Time::Delta collision_dt = -1.0f;
    Entity *collision_entity = NULL;
    for (Engine2::Circle::AreaTraceList::iterator it = area_trace_list.begin(),
                                                  it_end = area_trace_list.end();
         it != it_end;
         ++it)
    {
        Entity *entity = DStaticCast<Entity *>(*it);
        ASSERT1(entity != NULL);

        // ignore ourselves
        if (entity == this)
            continue;

        // if this entity is a solitary, set m_target and transition
        // to TrailTarget
        if (entity->GetEntityType() == ET_SOLITARY)
        {
            m_target = entity->GetReference();
            m_think_state = THINK_STATE(TrailTarget);
            return;
        }
        // otherwise if we will collide with something in the next short
        // while, perform collision avoidance calculations
        else
        {
            Float potential_collision_dt = CollisionTime(entity, s_collision_lookahead_dt);
            if (potential_collision_dt >= 0.0f &&
                (collision_entity == NULL || potential_collision_dt < collision_dt))
            {
                collision_dt = potential_collision_dt;
                collision_entity = entity;
            }
        }
    }

    // if there is an imminent collision, pick a new direction to avoid it
    if (collision_entity != NULL)
    {
        FloatVector2 delta_velocity(collision_entity->Velocity() - Velocity());
        FloatVector2 perpendicular_velocity(PerpendicularVector2(delta_velocity));
        ASSERT1(!perpendicular_velocity.IsZero());
        if ((perpendicular_velocity | Velocity()) > -(perpendicular_velocity | Velocity()))
            m_wander_angle = Math::Arg(perpendicular_velocity);
        else
            m_wander_angle = Math::Arg(-perpendicular_velocity);
        m_next_wander_time = time + 6.0f;
    }

    // incrementally accelerate up to the wander direction/speed
    FloatVector2 wander_velocity(ms_wander_speed[EnemyLevel()] * Math::UnitVector(m_wander_angle));
    MatchVelocity(wander_velocity, frame_dt);

    m_wander_angle += m_wander_angle_derivative * frame_dt;

    if (time >= m_next_wander_time)
    {
        m_wander_angle_derivative = Math::RandomFloat(-30.0f, 30.0f);
        m_next_wander_time = time + 6.0f;
    }
}

void Revulsion::TrailTarget (Time time, Time::Delta frame_dt)
{
    ASSERT1(!m_reticle_effect.IsValid() || !m_reticle_effect->IsInWorld());

    if (!m_target.IsValid() || m_target->IsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }

    Float target_aim_angle = TargetAimAngle();
    // if we're too close to the target's line of fire, transition to FleeTarget
    if (target_aim_angle >= -ms_target_aim_angle_flee_limit[EnemyLevel()] &&
        target_aim_angle <=  ms_target_aim_angle_flee_limit[EnemyLevel()])
    {
        m_think_state = THINK_STATE(FleeTarget);
        return;
    }

    // figure out where to go to get behind the target
    FloatVector2 preferred_location(-0.5f * GaussGun::ms_range[m_weapon->UpgradeLevel()] * Math::UnitVector(m_target->Angle()) + m_target->Translation());

    // if we're close enough to the spot behind the target (and we're
    // ready to fire), transition to fire
    Float distance_to_preferred_location = GetObjectLayer()->AdjustedDistance(preferred_location, Translation());
    if (distance_to_preferred_location <= ms_preferred_location_distance_tolerance[EnemyLevel()] &&
        m_weapon->ReadinessStatus(false, time) == 1.0f)
    {
        m_think_state = THINK_STATE(StartAimAtTarget);
        return;
    }

    // otherwise, plot an intercept course with the preferred location

    Float interceptor_acceleration =
        ms_engine_thrust[EnemyLevel()] / Mass();
    FloatVector2 p(GetObjectLayer()->AdjustedDifference(preferred_location, Translation()));
    FloatVector2 v(m_target->Velocity() - Velocity());
    FloatVector2 a(m_target->Force() / m_target->Mass());

    Polynomial poly;
    poly.Set(4, a.LengthSquared() - interceptor_acceleration*interceptor_acceleration);
    poly.Set(3, 4.0f * (a | v));
    poly.Set(2, 4.0f * ((a | p) + (v | v)));
    poly.Set(1, 8.0f * (p | v));
    poly.Set(0, 4.0f * (p | p));

    Polynomial::SolutionSet solution_set;
    poly.Solve(&solution_set, 0.001f);

    Float collision_dt = -1.0f;
    for (Polynomial::SolutionSet::iterator it = solution_set.begin(),
                                         it_end = solution_set.end();
         it != it_end;
         ++it)
    {
        if (*it >= 0.0f)
        {
            collision_dt = *it;
            break;
        }
    }

    if (collision_dt <= 0.0f)
    {
        // if no acceptable solution, just do dumb approach
        SetReticleCoordinates(preferred_location);
    }
    else
    {
        FloatVector2 real_approach_direction((2.0f*p + 2.0f*v*collision_dt + a*Sqr(collision_dt)) / (interceptor_acceleration*Sqr(collision_dt)));
        SetReticleCoordinates(Translation() + real_approach_direction);
    }
    SetEngineUpDownInput(SINT8_UPPER_BOUND);
}

void Revulsion::StartAimAtTarget (Time time, Time::Delta frame_dt)
{
    if (!m_target.IsValid() || m_target->IsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }

    // record the time we picked where to aim
    m_aim_start_time = time;

    // figure out where to aim (here is where the aim accuracy is determined)
    m_aim_delta =
        ms_aim_error_radius[EnemyLevel()] *
        Math::Sqrt(Math::RandomFloat(0.0f, 1.0f)) *
        Math::UnitVector(Math::RandomAngle());

    ASSERT1(!m_reticle_effect.IsValid() || !m_reticle_effect->IsInWorld());
    // ensure the reticle effect is allocated (lazy allocation)
    if (!m_reticle_effect.IsValid())
        m_reticle_effect = SpawnReticleEffect(GetObjectLayer(), Color(1.0f, 0.0f, 0.0f, 0.5f))->GetReference();
    // if the reticle effect is already allocated but not in the world, re-add it.
    else if (!m_reticle_effect->IsInWorld())
        m_reticle_effect->AddBackIntoWorld();
    ASSERT1(m_reticle_effect.IsValid() && m_reticle_effect->IsInWorld());

    // transition to and call ContinueAimAtTarget
    m_think_state = THINK_STATE(ContinueAimAtTarget);
    ContinueAimAtTarget(time, frame_dt);
}

void Revulsion::ContinueAimAtTarget (Time time, Time::Delta frame_dt)
{
    if (!m_target.IsValid() || m_target->IsDead())
    {
        ASSERT1(m_reticle_effect.IsValid() && m_reticle_effect->IsInWorld());
        m_reticle_effect->ScheduleForRemovalFromWorld(0.0f);
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }

    Float target_aim_angle = TargetAimAngle();
    // if we're too close to the target's line of fire, transition to FleeTarget
    if (target_aim_angle >= -ms_target_aim_angle_flee_limit[EnemyLevel()] &&
        target_aim_angle <=  ms_target_aim_angle_flee_limit[EnemyLevel()])
    {
        ASSERT1(m_reticle_effect.IsValid() && m_reticle_effect->IsInWorld());
        m_reticle_effect->ScheduleForRemovalFromWorld(0.0f);
        m_think_state = THINK_STATE(FleeTarget);
        return;
    }

    ASSERT1(ms_aim_duration[EnemyLevel()] > 0.0f);
    Float aim_time_parameter = Min(1.0f, 0.25f + 0.75f * (time - m_aim_start_time) / ms_aim_duration[EnemyLevel()]);
    SetReticleCoordinates(m_target->Translation() + m_aim_delta);

    // update the reticle's location and scale factor
    ASSERT1(m_reticle_effect.IsValid() && m_reticle_effect->IsInWorld());
    static Float const s_final_reticle_radius = 20.0f;
    m_reticle_effect->SnapToLocationAndSetScaleFactor(
        aim_time_parameter * ReticleCoordinates() + (1.0f - aim_time_parameter) * Translation(),
        s_final_reticle_radius * aim_time_parameter);

    // if the aim duration has elapsed, transition to FireAtTarget
    if (time - m_aim_start_time >= ms_aim_duration[EnemyLevel()])
        m_think_state = THINK_STATE(FireAtTarget);
}

void Revulsion::FireAtTarget (Time time, Time::Delta frame_dt)
{
    ASSERT1(m_reticle_effect.IsValid() && m_reticle_effect->IsInWorld());
    m_reticle_effect->ScheduleForRemovalFromWorld(0.0f);

    if (!m_target.IsValid() || m_target->IsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }

    SetReticleCoordinates(m_target->Translation() + m_aim_delta);
    SetWeaponPrimaryInput(UINT8_UPPER_BOUND);

    m_think_state = THINK_STATE(TrailTarget);
    SetNextTimeToThink(time + 0.3f);
}

void Revulsion::FleeTarget (Time time, Time::Delta frame_dt)
{
    if (!m_target.IsValid() || m_target->IsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }

    Float target_aim_angle = TargetAimAngle();
    // if we're far enough behind the target, transition to TrailTarget
    if (target_aim_angle <= -ms_target_aim_angle_trail_limit[EnemyLevel()] ||
        target_aim_angle >=  ms_target_aim_angle_trail_limit[EnemyLevel()])
    {
        m_think_state = THINK_STATE(TrailTarget);
        return;
    }

    FloatVector2 negative_position_delta(GetObjectLayer()->AdjustedDifference(Translation(), m_target->Translation()));
    FloatVector2 desired_velocity(
        ms_flee_speed[EnemyLevel()] *
        PerpendicularVector2(negative_position_delta).Normalization());
    MatchVelocity(desired_velocity, frame_dt);
}

Float Revulsion::TargetAimAngle () const
{
    ASSERT1(m_target.IsValid());
    FloatVector2 target_delta(GetObjectLayer()->AdjustedDifference(Translation(), m_target->Translation()));
    return Math::Arg(target_delta) - Math::CanonicalAngle(m_target->Angle());
}

void Revulsion::MatchVelocity (FloatVector2 const &velocity, Time::Delta frame_dt)
{
    // calculate what thrust is required to match the desired velocity
    FloatVector2 velocity_differential =
        velocity - (Velocity() + frame_dt * Force() / Mass());
    FloatVector2 thrust_vector = Mass() * velocity_differential / frame_dt;
    if (thrust_vector.LengthSquared() > 0.001f)
    {
        Float thrust_force = thrust_vector.Length();
        if (thrust_force > ms_engine_thrust[EnemyLevel()])
            thrust_vector = ms_engine_thrust[EnemyLevel()] * thrust_vector.Normalization();
        thrust_force = thrust_vector.Length();

        SetReticleCoordinates(Translation() + thrust_vector.Normalization());
        SetEngineUpDownInput(
            static_cast<Sint8>(
                SINT8_UPPER_BOUND * thrust_force /
                ms_engine_thrust[EnemyLevel()]));
    }
}

} // end of namespace Dis
