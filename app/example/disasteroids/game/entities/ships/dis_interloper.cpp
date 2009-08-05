// ///////////////////////////////////////////////////////////////////////////
// dis_interloper.cpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_interloper.hpp"

#include "dis_engine.hpp"
#include "dis_physicshandler.hpp"
#include "dis_powergenerator.hpp"
#include "xrb_engine2_objectlayer.hpp"
#include "xrb_polynomial.hpp"

using namespace Xrb;

#define THINK_STATE(x) &Interloper::x

namespace Dis
{

Float const Interloper::ms_max_health[ENEMY_LEVEL_COUNT] = { 10.0f, 40.0f, 160.0f, 640.0f };
Float const Interloper::ms_engine_thrust[ENEMY_LEVEL_COUNT] = { 7300.0f, 18000.0f, 40000.0f, 88000.0f };
Float const Interloper::ms_max_angular_velocity[ENEMY_LEVEL_COUNT] = { 360.0f, 360.0f, 360.0f, 360.0f };
Float const Interloper::ms_scale_factor[ENEMY_LEVEL_COUNT] = { 10.0f, 12.0f, 15.0f, 18.0f };
Float const Interloper::ms_baseline_first_moment[ENEMY_LEVEL_COUNT] = { 40.0f, 80.0f, 160.0f, 320.0f };
Float const Interloper::ms_damage_dissipation_rate[ENEMY_LEVEL_COUNT] = { 0.5f, 0.7f, 1.2f, 2.5f };
Float const Interloper::ms_wander_speed[ENEMY_LEVEL_COUNT] = { 150.0f, 150.0f, 150.0f, 150.0f };

Interloper::Interloper (Uint8 const enemy_level)
    :
    EnemyShip(enemy_level, ms_max_health[enemy_level], ET_INTERLOPER)
{
    m_think_state = THINK_STATE(PickWanderDirection);

    SetStrength(D_MINING_LASER);
    SetImmunity(D_COLLISION);
    SetDamageDissipationRate(ms_damage_dissipation_rate[GetEnemyLevel()]);

    m_flock_leader_weight = 0.0f;
}

Interloper::~Interloper ()
{
}

void Interloper::Think (Float const time, Float const frame_dt)
{
    // can't think if we're dead.
    if (IsDead())
        return;

    // decay the flock leader weight
    {
        static Float const s_flock_leader_weight_halflife = 1.5f;
        m_flock_leader_weight *= Math::Pow(0.5f, frame_dt / s_flock_leader_weight_halflife);
        if (m_flock_leader_weight > 1000.0f)
            m_flock_leader_weight = 1000.0f;
        if (m_flock_leader_weight < -1000.0f)
            m_flock_leader_weight = -1000.0f;
    }

    bool is_disabled = IsDisabled();
    Ship::Think(time, frame_dt);
    if (is_disabled)
    {
        // if disabled, then reset the think state to PickWanderDirection (a way out for
        // players that are being ganged up on)
        m_think_state = THINK_STATE(PickWanderDirection);
        // reset other stuff
        m_flock_leader_weight = 0.0f;
        m_closest_flock_member.Release();
        return;
    }

    // call the think state function (which will set the inputs)
    (this->*m_think_state)(time, frame_dt);

    // since enemy ships do not use the PlayerShip device code, engines
    // weapons, etc must be activated/simulated manually here.

    // this special handling is done because we don't accumulate force
    // necessarily in the direction the ship is aiming.
    if (!GetVelocity().IsZero())
        AimShipAtCoordinates(GetTranslation() + GetVelocity().GetNormalization(), frame_dt);
    // apply ship thrust in the appropriate direction
    FloatVector2 thrust_direction(GetReticleCoordinates() - GetTranslation());
    if (thrust_direction.GetLengthSquared() < 0.001f)
        thrust_direction = Math::UnitVector(Angle());
    else
        thrust_direction.Normalize();
    AccumulateForce(
        GetNormalizedEngineUpDownInput() *
        ms_engine_thrust[GetEnemyLevel()] *
        thrust_direction);

    ResetInputs();
}

void Interloper::Collide (
    Entity *const collider,
    FloatVector2 const &collision_location,
    FloatVector2 const &collision_normal,
    Float const collision_force,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(collider != NULL);

    // TODO: add the extra damage "weapon" for the front of the ship

    if (m_target.IsValid() && collider->GetEntityType() == ET_SOLITARY)
    {
        // if the enemy level is ___, do extra damage and spawn effects


        m_time_at_retreat_start = time;
        m_think_state = THINK_STATE(Retreat);
        // make sure to save the target, because we'll charge at it again.
    }

    Mortal::Collide(
        collider,
        collision_location,
        collision_normal,
        collision_force,
        time,
        frame_dt);
}

void Interloper::SetTarget (Mortal *const target)
{
    if (target == NULL)
        m_target.Release();
    else
    {
        m_target = target->GetReference();
        m_think_state = THINK_STATE(Charge);
    }
}

void Interloper::PickWanderDirection (Float const time, Float const frame_dt)
{
    ASSERT1(!m_closest_flock_member.IsValid());

    // update the next time to pick a wander direction
    m_next_wander_time = time + 6.0f;
    // pick a direction/speed to wander in
    m_wander_angle = Math::RandomAngle();
    m_wander_angle_derivative = 0.0f;
    m_think_state = THINK_STATE(Wander);
}

void Interloper::Wander (Float const time, Float const frame_dt)
{
    ASSERT1(!m_closest_flock_member.IsValid());

    static Float const s_scan_radius = 200.0f;
    static Float const s_collision_lookahead_time = 3.0f;

    // scan area for targets
    AreaTraceList area_trace_list;
    GetPhysicsHandler()->AreaTrace(
        GetObjectLayer(),
        GetTranslation(),
        s_scan_radius,
        false,
        &area_trace_list);
    // check the area trace list for targets and collisions
    Float collision_time = -1.0f;
    Entity *collision_entity = NULL;
    bool found_flock = false;
    for (AreaTraceListIterator it = area_trace_list.begin(),
                               it_end = area_trace_list.end();
         it != it_end;
         ++it)
    {
        Entity *entity = *it;
        ASSERT1(entity != NULL);

        // ignore ourselves
        if (entity == this)
            continue;

        // if this entity is a solitary, set m_target and transition
        // to Charge
        if (entity->GetEntityType() == ET_SOLITARY)
        {
            m_target = entity->GetReference();
            m_think_state = THINK_STATE(Charge);
            return;
        }
        // otherwise if this entity is an interloper, transition to Flock
        else if (entity->GetEntityType() == ET_INTERLOPER &&
                 !IsFlockLeader())
        {
            found_flock = true;
        }
        // otherwise if we will collide with something in the next short
        // while, perform collision avoidance calculations
        else
        {
            Float potential_collision_time = CollisionTime(entity, s_collision_lookahead_time);
            if (potential_collision_time >= 0.0f &&
                (collision_entity == NULL || potential_collision_time < collision_time))
            {
                collision_time = potential_collision_time;
                collision_entity = entity;
            }
        }
    }

    // we don't want to transition to Flock inside the above loop,
    // because scanning for Solitaries takes precedence over flocking.
    if (found_flock)
    {
        m_think_state = THINK_STATE(Flock);
        return;
    }

    // if there is an imminent collision, pick a new direction to avoid it
    if (collision_entity != NULL)
    {
        FloatVector2 delta_velocity(collision_entity->GetVelocity() - GetVelocity());
        FloatVector2 perpendicular_velocity(GetPerpendicularVector2(delta_velocity));
        ASSERT1(!perpendicular_velocity.IsZero());
        if ((perpendicular_velocity | GetVelocity()) > -(perpendicular_velocity | GetVelocity()))
            m_wander_angle = Math::Atan(perpendicular_velocity);
        else
            m_wander_angle = Math::Atan(-perpendicular_velocity);
        m_next_wander_time = time + 6.0f;
    }

    // incrementally accelerate up to the wander direction/speed
    FloatVector2 wander_velocity(ms_wander_speed[GetEnemyLevel()] * Math::UnitVector(m_wander_angle));
    MatchVelocity(wander_velocity, frame_dt);
    SetReticleCoordinates(GetTranslation() + Math::UnitVector(m_wander_angle));

    m_wander_angle += m_wander_angle_derivative * frame_dt;

    if (time >= m_next_wander_time)
    {
        m_wander_angle_derivative = Math::RandomFloat(-30.0f, 30.0f);
        m_next_wander_time = time + 6.0f;
    }
}

void Interloper::Flock (Float time, Float frame_dt)
{
    if (IsFlockLeader())
    {
        m_think_state = THINK_STATE(Wander);
        m_next_wander_time = time + 6.0f;
        m_closest_flock_member.Release();
        return;
    }

    // the reach of this scan shouldn't allow entities of more
    // than 1/2 of the ObjectLayer size to be scanned, otherwise
    // the center of gravity calculations will fail (because
    // ObjectLayer::AdjustedCoordinates() must be called on
    // each entity's translation).
    static Float const s_lookahead_scan_distance = 220.0f;
    static Float const s_scan_radius = 200.0f;

    // scan the area in front of us
    AreaTraceList area_trace_list;
    GetPhysicsHandler()->AreaTrace(
        GetObjectLayer(),
        GetTranslation() + s_lookahead_scan_distance * Math::UnitVector(Angle()),
        s_scan_radius,
        false,
        &area_trace_list);
    // iterate through -- check for targets and do flock calculations
    FloatVector2 flock_center_of_gravity(FloatVector2::ms_zero);
    Interloper *closest_flock_member = NULL;
    Float closest_flock_member_distance = -1.0f;
    Float flock_mass = 0.0f;
    Uint32 flock_member_count = 0;
    for (AreaTraceListIterator it = area_trace_list.begin(),
                               it_end = area_trace_list.end();
         it != it_end;
         ++it)
    {
        Entity *entity = *it;
        ASSERT1(entity != NULL);

        // ignore ourselves
        if (entity == this)
            continue;

        // if this entity is a solitary, set m_target and transition
        // to Charge
        if (entity->GetEntityType() == ET_SOLITARY)
        {
            m_target = entity->GetReference();
            m_think_state = THINK_STATE(Charge);
            m_closest_flock_member.Release();
            return;
        }
        // otherwise if this entity is an interloper, include it
        // in the flock calculations
        else if (entity->GetEntityType() == ET_INTERLOPER)
        {
            // add flock leader weight to the Interloper we're following
            // and remove flock leader weight from ourselves.
            Interloper *interloper = DStaticCast<Interloper *>(entity);

            FloatVector2 interloper_position(
                GetObjectLayer()->AdjustedCoordinates(
                    interloper->GetTranslation(),
                    GetTranslation()));
            Float interloper_distance = (interloper_position - GetTranslation()).GetLength();

            if (closest_flock_member == NULL ||
                interloper_distance < closest_flock_member_distance)
            {
                closest_flock_member = interloper;
                closest_flock_member_distance = interloper_distance;
            }

            flock_center_of_gravity += interloper->GetFirstMoment() * interloper_position;
            flock_mass += interloper->GetFirstMoment();
            ++flock_member_count;
        }
        // TODO: decide if there should be collision avoidance
    }

    // if no flock, transition back to Wander
    if (closest_flock_member == NULL)
    {
        m_think_state = THINK_STATE(Wander);
        m_next_wander_time = time + 6.0f;
        m_closest_flock_member.Release();
        return;
    }

    m_closest_flock_member = closest_flock_member->GetReference();
    if (!IsFlockLeader())
    {
        m_flock_leader_weight -= static_cast<Float>(flock_member_count) * frame_dt;
        AddFlockLeaderWeight(frame_dt);
    }

    // divide mass out of the accumulated center of gravity
    flock_center_of_gravity /= flock_mass;

    // the goal is to travel at the same velocity as the flock, while
    // maintaining a position relative to the closest flock member.
    FloatVector2 flock_center_offset(flock_center_of_gravity - GetTranslation());
    if (flock_center_offset.GetLength() >= 0.5f)
    {
        // TODO: keep X distance away from closest flock member
        FloatVector2 flock_center_direction(flock_center_offset.GetNormalization());
        MatchVelocity(ms_wander_speed[GetEnemyLevel()] * flock_center_direction, frame_dt);
        SetReticleCoordinates(GetTranslation() + flock_center_direction);
    }
    else
        SetReticleCoordinates(GetTranslation() + Math::UnitVector(Angle()));
}

void Interloper::Charge (Float const time, Float const frame_dt)
{
    ASSERT1(!m_closest_flock_member.IsValid());

    if (!m_target.IsValid() || m_target->IsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }

    FloatVector2 target_position(GetObjectLayer()->AdjustedCoordinates(m_target->GetTranslation(), GetTranslation()));

    // adjust our course to hit the target (use a different course-
    // setting method for each enemy level).

    // level 0 aims directly at the target
    if (GetEnemyLevel() == 0)
    {
        SetReticleCoordinates(target_position);
    }
    // level 1 plots a near-intercept course with the target, not accounting for acceleration.
    // level 2 plots an intercept course with the the target, not accounting for acceleration.
    // level 3 plots a direct intercept course with the target, accounting for acceleration.
    else
    {
        Float interceptor_acceleration =
            ms_engine_thrust[GetEnemyLevel()] / GetFirstMoment();
        FloatVector2 p(target_position - GetTranslation());
        FloatVector2 v;
        FloatVector2 a;
        if (GetEnemyLevel() == 1)
        {
            v = 0.5f * (m_target->GetVelocity() - GetVelocity());
            a = FloatVector2::ms_zero;
        }
        else if (GetEnemyLevel() == 2)
        {
            v = m_target->GetVelocity() - GetVelocity();
            a = FloatVector2::ms_zero;
        }
        else
        {
            v = m_target->GetVelocity() - GetVelocity();
            a = m_target->GetForce() / m_target->GetFirstMoment();
        }

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
            SetReticleCoordinates(target_position);
        }
        else
        {
            FloatVector2 real_approach_direction((2.0f*p + 2.0f*v*T + a*T*T) / (interceptor_acceleration*T*T));
            SetReticleCoordinates(GetTranslation() + real_approach_direction);
        }
    }
    SetEngineUpDownInput(SINT8_UPPER_BOUND);
}

void Interloper::Retreat (Float const time, Float const frame_dt)
{
    ASSERT1(!m_closest_flock_member.IsValid());

    static Float const s_retreat_time = 1.0f;

    if (!m_target.IsValid() || m_target->IsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }

    // if the retreat time has elapsed, transition back to Charge.
    if (m_time_at_retreat_start + s_retreat_time <= time)
    {
        m_think_state = THINK_STATE(Charge);
        return;
    }

    // set a course parallel to and slightly away from the target
    FloatVector2 target_position(GetObjectLayer()->AdjustedCoordinates(m_target->GetTranslation(), GetTranslation()));
    SetReticleCoordinates(GetTranslation() + (GetTranslation() - target_position).GetNormalization());
    SetEngineUpDownInput(SINT8_UPPER_BOUND);
}

void Interloper::MatchVelocity (FloatVector2 const &velocity, Float const frame_dt)
{
    // this is the fake force-accumulating thrust code

    // calculate what thrust is required to match the desired velocity
    FloatVector2 velocity_differential =
        velocity - (GetVelocity() + frame_dt * GetForce() / GetFirstMoment());
    FloatVector2 thrust_vector = GetFirstMoment() * velocity_differential / frame_dt;
    if (thrust_vector.GetLength() > 0.01f)
    {
        Float thrust_force = thrust_vector.GetLength();
        if (thrust_force > ms_engine_thrust[GetEnemyLevel()])
            thrust_vector = ms_engine_thrust[GetEnemyLevel()] * thrust_vector.GetNormalization();

        AccumulateForce(thrust_vector);
    }
}

void Interloper::AddFlockLeaderWeight (Float const weight)
{
    Uint32 iteration_count = 0;
    Interloper *current = this;
    do
    {
        current->m_flock_leader_weight += weight;
        current = *current->m_closest_flock_member;
        ++iteration_count;
    }
    while (current != NULL && current != this && iteration_count < 10);
}

} // end of namespace Dis
