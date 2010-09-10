// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_circle_entity.cpp by Victor Dods, created 2005/11/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_circle_entity.hpp"

#include "xrb_engine2_circle_physicshandler.hpp"
#include "xrb_engine2_objectlayer.hpp"
#include "xrb_engine2_world.hpp"

namespace Xrb {
namespace Engine2 {
namespace Circle {

Entity::Entity (CollisionType collision_type)
    :
    Engine2::Entity(),
    m_collision_type(collision_type)
{
    ASSERT1(m_collision_type < CT_COUNT);
    m_next_time_to_think = 0.0f;
    m_elasticity = 1.0f;
    m_mass = 1.0f;
    m_velocity = FloatVector2::ms_zero;
    m_force = FloatVector2::ms_zero;
    m_angular_velocity = 0.0f;
}

PhysicsHandler const *Entity::GetPhysicsHandler () const
{
    return DStaticCast<PhysicsHandler *>(GetWorld()->GetPhysicsHandler());
}

PhysicsHandler *Entity::GetPhysicsHandler ()
{
    return DStaticCast<PhysicsHandler *>(GetWorld()->GetPhysicsHandler());
}

FloatVector2 Entity::AmbientVelocity (Float scan_area_radius) const
{
    FloatVector2 ambient_momentum;
    Float ambient_mass;

    GetPhysicsHandler()->CalculateAmbientMomentum(
        GetObjectLayer(),
        Translation(),
        scan_area_radius,
        this, // ignore_me
        ambient_momentum,
        ambient_mass);

    ASSERT1(ambient_mass >= 0.0f);

    // if no objects were encountered, then the ambient velocity
    // should just be the velocity of this game object.
    if (ambient_mass == 0.0f)
        return Velocity();
    // otherwise, divide the momentum by the mass to get velocity
    else
        return ambient_momentum / ambient_mass;
}

void Entity::CloneProperties (Entity const &entity)
{
    Engine2::Entity::CloneProperties(entity);

    m_next_time_to_think = entity.m_next_time_to_think;
//     m_collision_type = entity.m_collision_type; // don't copy this -- unless it becomes a problem
    m_elasticity = entity.m_elasticity;
    m_mass = entity.m_mass;
    m_velocity = entity.m_velocity;
    m_force = entity.m_force;
    m_angular_velocity = entity.m_angular_velocity;
}

void Entity::ApplyInterceptCourseAcceleration (
    Entity *target,
    Float maximum_thrust_force,
    bool apply_force_on_target_also,
    bool reverse_thrust)
{
    Polynomial::SolutionSet solution_set;
    ApplyInterceptCourseAcceleration(
        target,
        maximum_thrust_force,
        apply_force_on_target_also,
        reverse_thrust,
        &solution_set);
}

void Entity::ApplyInterceptCourseAcceleration (
    Entity *target,
    Float maximum_thrust_force,
    bool apply_force_on_target_also,
    bool reverse_thrust,
    Polynomial::SolutionSet *solution_set)
{
    ASSERT1(target != NULL);
    FloatVector2 target_force_vector(
        ApplyInterceptCourseAcceleration(
            target->Translation(),
            target->Velocity(),
            target->Force() / target->Mass(),
            maximum_thrust_force,
            apply_force_on_target_also,
            reverse_thrust,
            solution_set));
    if (apply_force_on_target_also)
        target->AccumulateForce(target_force_vector);
}

void Entity::ApplyInterceptCourseAcceleration (
    FloatVector2 const &target_position,
    FloatVector2 const &target_velocity,
    FloatVector2 const &target_acceleration,
    Float maximum_thrust_force,
    bool apply_force_on_target_also,
    bool reverse_thrust)
{
    Polynomial::SolutionSet solution_set;
    ApplyInterceptCourseAcceleration(
        target_position,
        target_velocity,
        target_acceleration,
        maximum_thrust_force,
        apply_force_on_target_also,
        reverse_thrust,
        &solution_set);
}

FloatVector2 Entity::ApplyInterceptCourseAcceleration (
    FloatVector2 const &target_position,
    FloatVector2 const &target_velocity,
    FloatVector2 const &target_acceleration,
    Float maximum_thrust_force,
    bool apply_force_on_target_also,
    bool reverse_thrust,
    Polynomial::SolutionSet *solution_set)
{
    ASSERT1(maximum_thrust_force > 0.0f);
    ASSERT1(solution_set != NULL);
    ASSERT1(solution_set->empty());

    FloatVector2 p1(GetObjectLayer()->AdjustedCoordinates(target_position, Translation()));
    FloatVector2 p(p1 - Translation());
    FloatVector2 v(target_velocity - Velocity());
    FloatVector2 a(target_acceleration);
    Float interceptor_acceleration = maximum_thrust_force / Mass();

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
        return FloatVector2::ms_zero;

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
        return FloatVector2::ms_zero;

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

    return apply_force_on_target_also ? -force_vector : FloatVector2::ms_zero;
}

void Entity::HandleObjectLayerContainment (bool component_x, bool component_y)
{
    if (component_x)
        m_velocity[Dim::X] = 0.0f;
    if (component_y)
        m_velocity[Dim::Y] = 0.0f;
}

Float Entity::CollisionTime (Entity const *entity, Float lookahead_time) const
{
    ASSERT1(entity != NULL);

    FloatVector2 adjusted_entity_translation(
        GetObjectLayer()->AdjustedCoordinates(
            entity->Translation(),
            Translation()));

    FloatVector2 p(Translation() - adjusted_entity_translation);
    FloatVector2 v(Velocity() - entity->Velocity());
    Float R = Radius(QTT_PHYSICS_HANDLER) + entity->Radius(QTT_PHYSICS_HANDLER);

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

} // end of namespace Circle
} // end of namespace Engine2
} // end of namespace Xrb
