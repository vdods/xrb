// ///////////////////////////////////////////////////////////////////////////
// dis_gameobject.cpp by Victor Dods, created 2005/11/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_gameobject.h"

#include "dis_physicshandler.h"
#include "xrb_engine2_objectlayer.h"
#include "xrb_serializer.h"

using namespace Xrb;

namespace Dis
{

Engine2::EntityGuts *GameObject::Create (Serializer &serializer)
{
    return NULL; // TEMP
    /*
    ASSERT1(serializer.GetIODirection() == IOD_READ)

    Engine2::EntityGuts *retval;
    Type type = ReadType(serializer);
    switch (type)
    {
        case PLAYER_SHIP:
            retval = PlayerShip::Create(serializer);
            break;

        case BULLET:
            retval = Bullet::Create(serializer);
            break;

        case ASTEROID:
            retval = Asteroid::Create(serializer);
            break;

        case HEALTH_TRIGGER:
            retval = HealthTrigger::Create(serializer);
            break;

        case EXPLOSION:
            retval = Explosion::Create(serializer);
            break;

        default:
            retval = NULL;
            break;
    }
    return retval;
    */
}

FloatVector2 GameObject::GetAmbientVelocity (
    Float const scan_area_radius,
    GameObject const *const ignore_me) const
{
    // do an area trace
    AreaTraceList area_trace_list;
    GetPhysicsHandler<Dis::PhysicsHandler>()->AreaTrace(
        GetObjectLayer(),
        GetTranslation(),
        scan_area_radius,
        false,
        &area_trace_list);
    
    // calculate the ambient velocity
    FloatVector2 total_momentum(FloatVector2::ms_zero);
    Float total_mass = 0.0f;
    for (AreaTraceListIterator it = area_trace_list.begin(),
                               it_end = area_trace_list.end();
         it != it_end;
         ++it)
    {
        GameObject const *game_object = *it;
        // ignore NULL game objects
        if (game_object == NULL)
            continue;
        // the object shouldn't include itself in the ambient velocity scan
        else if (game_object == this)
            continue;
        // if it matches the ignore object, skip it.
        else if (game_object == ignore_me)
            continue;
            
        total_momentum += game_object->GetMomentum();
        total_mass += game_object->GetFirstMoment();
    }

    // if no objects were encountered, then the ambient velocity
    // should just be the velocity of this game object.
    if (total_mass == 0.0f)
        return GetVelocity();
    // otherwise, divide the total momentum by the total mass to get velocity
    else
        return total_momentum / total_mass;
}

void GameObject::ApplyInterceptCourseAcceleration (
    GameObject *const target,
    Float const maximum_thrust_force,
    bool const apply_force_on_target_also,
    Polynomial::SolutionSet *const solution_set)
{
    ASSERT1(target != NULL)
    ASSERT1(maximum_thrust_force > 0.0f)
    ASSERT1(solution_set != NULL)
    ASSERT1(solution_set->empty())

    FloatVector2 p1(
        target->GetObjectLayer()->GetAdjustedCoordinates(
            GetTranslation(),
            target->GetTranslation()));
    FloatVector2 p(target->GetTranslation() - p1);
    FloatVector2 v(target->GetVelocity() - GetVelocity());
    FloatVector2 a(target->GetForce() / target->GetFirstMoment());
    Float interceptor_acceleration =
        maximum_thrust_force / GetFirstMoment();

    Polynomial poly;
    if (apply_force_on_target_also)
    {
        // this one is for when the force is applied between the tractoree and tractoror
        poly.Set(4, 0.25f * a.GetLengthSquared() - interceptor_acceleration*interceptor_acceleration);
        poly.Set(3, (a | v));
        poly.Set(2, ((a | p) + (v | v)));
        poly.Set(1, 2.0f * (p | v));
        poly.Set(0, (p | p));
    }
    else
    {
        // this one is for when the force is only on the tractoree and not the tractoror
        poly.Set(4, a.GetLengthSquared() - interceptor_acceleration*interceptor_acceleration);
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
    for (Polynomial::SolutionSetIterator it = solution_set->begin(),
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
        force_vector = (p + v*T + 0.5f*a*T*T) / (T*T) * GetFirstMoment();
    else
        // this one is for when the force is only on the tractoree and not the tractoror
        force_vector = (2.0f*p + 2.0f*v*T + a*T*T) / (T*T) * GetFirstMoment();

    // exert the force on one body or both, depending on the value of
    // apply_force_on_target_also.  if it is true, apply the force equally
    // in opposite directions.
    AccumulateForce(force_vector);
    if (apply_force_on_target_also)
        target->AccumulateForce(-force_vector);
}

GameObject::Type GameObject::ReadType (Serializer &serializer)
{
    ASSERT1(serializer.GetIODirection() == IOD_READ)
    ASSERT1(T_COUNT < 0x100)
    return static_cast<Type>(serializer.ReadUint8());
}

void GameObject::WriteType (Serializer &serializer) const
{
    ASSERT1(serializer.GetIODirection() == IOD_WRITE)
    ASSERT1(T_COUNT < 0x100)
    serializer.WriteUint8(static_cast<Uint8>(m_type));
}

} // end of namespace Dis
