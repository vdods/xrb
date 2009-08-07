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

#include "dis_mortal.hpp"
#include "dis_physicshandler.hpp"

using namespace Xrb;

namespace Dis
{

void Ballistic::Think (Float const time, Float const frame_dt)
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
                    SetAngle(Math::Atan(velocity_delta));
            }

            // peform a line trace for frame_dt/2 time's worth
            // of velocity in front and behind this ballistic
            {
                FloatVector2 trace_vector(frame_dt * Velocity());
                FloatVector2 trace_start(Translation() - 0.5f * trace_vector);
                LineTraceBindingSet line_trace_binding_set;
                GetPhysicsHandler()->LineTrace(
                    GetObjectLayer(),
                    trace_start,
                    trace_vector,
                    PhysicalRadius(),
                    false,
                    &line_trace_binding_set);

                FloatVector2 collision_normal(trace_vector.Normalization());
                for (LineTraceBindingSet::iterator it = line_trace_binding_set.begin(),
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

                    FloatVector2 collision_location(trace_start + it->m_time * trace_vector);
                    bool there_was_a_collision =
                        CollidePrivate(
                            it->m_entity,
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
    Entity *const collider,
    FloatVector2 const &collision_location,
    FloatVector2 const &collision_normal,
    Float const collision_force,
    Float const time,
    Float const frame_dt)
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
    Entity *const collider,
    FloatVector2 const &collision_location,
    FloatVector2 const &collision_normal,
    Float const collision_force,
    Float const time,
    Float const frame_dt,
    bool const was_collision_from_line_trace)
{
    ASSERT1(collider != NULL);

    // we only care about hitting solid things
    if (collider->GetCollisionType() == CT_NONSOLID_COLLISION)
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

    // it hit something, so get rid of it.
    ScheduleForDeletion(0.0f);
    return true;
}

} // end of namespace Dis

