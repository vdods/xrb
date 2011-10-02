// ///////////////////////////////////////////////////////////////////////////
// dis_ballistic.cpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_ballistic.hpp"

#include "dis_effect.hpp"
#include "dis_mortal.hpp"
#include "dis_spawn.hpp"
#include "xrb_engine2_circle_physicshandler.hpp"
#include "xrb_engine2_objectlayer.hpp"

using namespace Xrb;

namespace Dis {

void Ballistic::Think (Time time, Time::Delta frame_dt)
{
    if (m_perform_line_trace_for_accuracy)
    {
        if (m_time_at_birth + m_time_to_live <= time)
            ScheduleForDeletion(0.0f);
        else
        {
            // lazily initialize m_initial_velocity with the owner's velocity
            if (m_first_think)
            {
                m_initial_velocity = m_owner->Velocity();
                m_first_think = false;
            }

            // update the angle to reflect the direction of motion
            {
                FloatVector2 velocity_delta(Velocity() - m_initial_velocity);
                if (!velocity_delta.IsZero())
                    SetAngle(Math::Arg(velocity_delta));
            }

            // peform a line trace for frame_dt/2 time's worth
            // of velocity in front and behind this ballistic
            {
                FloatVector2 trace_vector(frame_dt * Velocity());
                FloatVector2 trace_start(Translation() - 0.5f * trace_vector);
                Engine2::Circle::LineTraceBindingSet line_trace_binding_set;
                GetPhysicsHandler()->LineTrace(
                    GetObjectLayer(),
                    trace_start,
                    trace_vector,
                    PhysicalRadius(),
                    false,
                    line_trace_binding_set);

                for (Engine2::Circle::LineTraceBindingSet::iterator
                        it = line_trace_binding_set.begin(),
                        it_end = line_trace_binding_set.end();
                     it != it_end;
                     ++it)
                {
                    // don't hit ourselves
                    if (it->m_entity == this)
                        continue;

                    // don't hit the entity that fired us
                    if (it->m_entity == *m_owner)
                        continue;

                    FloatVector2 collision_location(trace_start + it->m_trace_hit_parameter * trace_vector);
                    FloatVector2 collision_normal(GetObjectLayer()->AdjustedDifference(collision_location, it->m_entity->Translation()));
                    if (collision_normal.LengthSquared() < 0.001f)
                        collision_normal = FloatVector2(1.0f, 0.0f); // arbitrary unit vector
                    else
                        collision_normal.Normalize();
                    bool there_was_a_collision =
                        CollidePrivate(
                            DStaticCast<Entity *>(it->m_entity),
                            collision_location,
                            collision_normal,
                            0.0f,
                            time,
                            frame_dt,
                            true);
                    if (there_was_a_collision)
                        break;
                }
            }
        }
    }
    else
    {
        if (!m_first_think)
            ScheduleForDeletion(0.0f);
        else
        {
            m_first_think = false;
            SetNextTimeToThink(time + m_time_to_live);
        }
    }
}

void Ballistic::Collide (
    Entity *collider,
    FloatVector2 const &collision_location,
    FloatVector2 const &collision_normal,
    Float collision_force,
    Time time,
    Time::Delta frame_dt)
{
    CollidePrivate(
        collider,
        collision_location,
        collision_normal,
        collision_force,
        time,
        frame_dt,
        false);
}

bool Ballistic::CollidePrivate (
    Entity *collider,
    FloatVector2 const &collision_location,
    FloatVector2 const &collision_normal,
    Float collision_force,
    Time time,
    Time::Delta frame_dt,
    bool was_collision_from_line_trace)
{
    ASSERT1(collider != NULL);

    // we only care about hitting solid things
    if (collider->GetCollisionType() == Engine2::Circle::CT_NONSOLID_COLLISION)
        return false;

    // also, don't hit powerups
    if (collider->IsPowerup())
        return false;

    // if this is a smart ballistic (m_perform_line_trace_for_accuracy
    // is true), then don't hurt the owner
    if (m_perform_line_trace_for_accuracy && *m_owner == collider)
        return false;

    // if this call to CollidePrivate was the result of a LineTrace,
    // then we should call Collide on the other entity
    if (was_collision_from_line_trace)
        collider->Collide(
            this,
            collision_location,
            -collision_normal,
            collision_force,
            time,
            frame_dt);

    // if we hit a mortal, damage it.
    if (collider->IsMortal())
    {
        Mortal *mortal = DStaticCast<Mortal *>(collider);
        mortal->Damage(
            *m_owner,
            this,
            m_impact_damage,
            NULL,
            collision_location,
            collision_normal,
            collision_force,
            Mortal::D_BALLISTIC,
            time,
            frame_dt);
    }

    // spawn some sort of effect here
    switch (m_impact_effect)
    {
        case IE_NONE:
            // no effect
            break;

        case IE_PLASMA_BALL:
            SpawnSplashImpactEffect(
                GetObjectLayer(),
                "fs://plasma_ball_yellow.png",
                time,
                collision_location,
                collision_normal,
                collider->Velocity(),
                Math::RandomFloat(-20.0f, 20.0f),   // seed angle
                PhysicalRadius(),
                4,                                  // particle count
                70.0f,                              // particle spread angle
                0.2f,                               // particle time to live
                2.0f);                              // particle speed proportion
            break;

        default:
            ASSERT1(false && "invalid ImpactEffect");
            break;
    }

    // so there aren't two impact effects if both collision handlers fired
    m_impact_effect = IE_NONE;

    // it hit something, so get rid of it.
    ScheduleForDeletion(0.0f);
    return true;
}

} // end of namespace Dis

