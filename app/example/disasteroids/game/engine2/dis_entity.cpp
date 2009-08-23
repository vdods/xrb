// ///////////////////////////////////////////////////////////////////////////
// dis_entity.cpp by Victor Dods, created 2005/11/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_entity.hpp"

#include "dis_physicshandler.hpp"
#include "dis_world.hpp"
#include "xrb_engine2_objectlayer.hpp"
#include "xrb_serializer.hpp"

using namespace Xrb;

namespace Dis
{

Entity::Entity (EntityType const entity_type, CollisionType const collision_type)
    :
    Engine2::Entity(),
    m_entity_type(entity_type),
    m_collision_type(collision_type)
{
    ASSERT1(m_entity_type < ET_COUNT);
    ASSERT1(m_collision_type < CT_COUNT);
    m_next_time_to_think = 0.0f;
    m_elasticity = 1.0f;
    m_mass = 1.0f;
    m_velocity = FloatVector2::ms_zero;
    m_force = FloatVector2::ms_zero;
    m_angular_velocity = 0.0f;
}

Dis::World *Entity::GetWorld () const
{
    return DStaticCast<Dis::World *>(OwnerObject()->GetWorld());
}

Dis::PhysicsHandler const *Entity::GetPhysicsHandler () const
{
    return GetWorld()->GetPhysicsHandler();
}

Dis::PhysicsHandler *Entity::GetPhysicsHandler ()
{
    return GetWorld()->GetPhysicsHandler();
}

FloatVector2 Entity::AmbientVelocity (
    Float const scan_area_radius,
    Entity const *const ignore_me) const
{
    // do an area trace
    AreaTraceList area_trace_list;
    GetPhysicsHandler()->AreaTrace(
        GetObjectLayer(),
        Translation(),
        scan_area_radius,
        false,
        &area_trace_list);

    // calculate the ambient velocity
    FloatVector2 total_momentum(FloatVector2::ms_zero);
    Float total_mass = 0.0f;
    for (AreaTraceList::iterator it = area_trace_list.begin(),
                               it_end = area_trace_list.end();
         it != it_end;
         ++it)
    {
        Entity const *entity = *it;
        // ignore NULL game objects
        if (entity == NULL)
            continue;
        // the object shouldn't include itself in the ambient velocity scan
        else if (entity == this)
            continue;
        // if it matches the ignore object, skip it.
        else if (entity == ignore_me)
            continue;

        total_momentum += entity->Momentum();
        total_mass += entity->Mass();
    }

    // if no objects were encountered, then the ambient velocity
    // should just be the velocity of this game object.
    if (total_mass == 0.0f)
        return Velocity();
    // otherwise, divide the total momentum by the total mass to get velocity
    else
        return total_momentum / total_mass;
}

void Entity::ApplyInterceptCourseAcceleration (
    Entity *const target,
    Float const maximum_thrust_force,
    bool const apply_force_on_target_also,
    bool const reverse_thrust,
    Polynomial::SolutionSet *const solution_set)
{
    ASSERT1(target != NULL);
    ASSERT1(maximum_thrust_force > 0.0f);
    ASSERT1(solution_set != NULL);
    ASSERT1(solution_set->empty());

    FloatVector2 p1(
        target->GetObjectLayer()->AdjustedCoordinates(
            Translation(),
            target->Translation()));
    FloatVector2 p(target->Translation() - p1);
    FloatVector2 v(target->Velocity() - Velocity());
    FloatVector2 a(target->Force() / target->Mass());
    Float interceptor_acceleration =
        maximum_thrust_force / Mass();

    Polynomial poly;
    if (apply_force_on_target_also)
    {
        // this one is for when the force is applied between the tractoree and tractoror
        poly.Set(4, 0.25f * a.LengthSquared() - interceptor_acceleration*interceptor_acceleration);
        poly.Set(3, (a | v));
        poly.Set(2, ((a | p) + (v | v)));
        poly.Set(1, 2.0f * (p | v));
        poly.Set(0, (p | p));
    }
    else
    {
        // this one is for when the force is only on the tractoree and not the tractoror
        poly.Set(4, a.LengthSquared() - interceptor_acceleration*interceptor_acceleration);
        poly.Set(3, 4.0f * (a | v));
        poly.Set(2, 4.0f * ((a | p) + (v | v)));
        poly.Set(1, 8.0f * (p | v));
        poly.Set(0, 4.0f * (p | p));
    }

    poly.Solve(solution_set, 0.0001f);

    // if an intercept course is not possible, just return
    if (solution_set->empty())
        return;

    Float T = -1.0f;
    for (Polynomial::SolutionSet::iterator it = solution_set->begin(),
                                         it_end = solution_set->end();
         it != it_end;
         ++it)
    {
        if (*it > 0.0f)
        {
            T = *it;
            break;
        }
    }
    // make sure to clear out the solution set, now that we have T
    solution_set->clear();
    // if an intercept course is not possible in the future, just return.
    if (T <= 0.0f)
        return;

    FloatVector2 force_vector;
    if (apply_force_on_target_also)
        // this one is for when the force is applied between the tractoree and tractoror
        force_vector = (p + v*T + 0.5f*a*T*T) / (T*T) * Mass();
    else
        // this one is for when the force is only on the tractoree and not the tractoror
        force_vector = (2.0f*p + 2.0f*v*T + a*T*T) / (T*T) * Mass();

    // if we want reverse thrust (push instead of pull), negate the force vector
    if (reverse_thrust)
        force_vector = -force_vector;

    // exert the force on one body or both, depending on the value of
    // apply_force_on_target_also.  if it is true, apply the force equally
    // in opposite directions.
    AccumulateForce(force_vector);
    if (apply_force_on_target_also)
        target->AccumulateForce(-force_vector);
}

void Entity::HandleObjectLayerContainment (bool const component_x, bool const component_y)
{
    if (component_x)
        m_velocity[Dim::X] = 0.0f;
    if (component_y)
        m_velocity[Dim::Y] = 0.0f;
}

Float Entity::CollisionTime (Entity *const entity, Float const lookahead_time) const
{
    FloatVector2 adjusted_entity_translation(
        GetObjectLayer()->AdjustedCoordinates(
            entity->Translation(),
            Translation()));

    FloatVector2 p(Translation() - adjusted_entity_translation);
    FloatVector2 v(Velocity() - entity->Velocity());
    Float R = Radius(Engine2::QTT_PHYSICS_HANDLER) + entity->Radius(Engine2::QTT_PHYSICS_HANDLER);

    Polynomial poly;
    poly.Set(2, v | v);
    poly.Set(1, 2.0f * (p | v));
    poly.Set(0, (p | p) - R*R);
    Polynomial::SolutionSet solution_set;
    poly.Solve(&solution_set, 0.0001f);

    Float T = -1.0f;
    for (Polynomial::SolutionSet::iterator it = solution_set.begin(),
                                         it_end = solution_set.end();
         it != it_end;
         ++it)
    {
        if (*it > 0.0f && *it <= lookahead_time)
        {
            T = *it;
            break;
        }
    }
    return T;
}

} // end of namespace Dis
