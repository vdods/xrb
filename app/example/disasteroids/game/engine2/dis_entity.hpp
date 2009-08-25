// ///////////////////////////////////////////////////////////////////////////
// dis_entity.hpp by Victor Dods, created 2005/11/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_ENTITY_HPP_)
#define _DIS_ENTITY_HPP_

#include "xrb_engine2_entity.hpp"

#include "dis_enums.hpp"
#include "dis_entityreference.hpp"
#include "xrb_polynomial.hpp"

using namespace Xrb;

namespace Dis
{

class PhysicsHandler;
class World;

// abstract baseclass for all disasteroids game object classes
class Entity : public Engine2::Entity
{
public:

    Entity (EntityType entity_type, CollisionType collision_type);
    virtual ~Entity ()
    {
        if (m_reference.IsValid())
            m_reference.NullifyEntity();
    }

    // ///////////////////////////////////////////////////////////////////////
    // public accessors
    // ///////////////////////////////////////////////////////////////////////

    EntityType GetEntityType () const { return m_entity_type; }
    Float NextTimeToThink () const { return m_next_time_to_think; }
    EntityReference<Entity> const &GetReference ()
    {
        if (!m_reference.IsValid())
        {
            m_reference.SetInstance(new EntityInstance(this));
            ASSERT1(m_reference.IsValid());
            ASSERT1(*m_reference == this);
        }
        return m_reference;
    }
    CollisionType GetCollisionType () const { return m_collision_type; }
    Float Elasticity () const { return m_elasticity; }
    Float Mass () const { return m_mass; }
    FloatVector2 const &Velocity () const { return m_velocity; }
    Float Speed () const { return m_velocity.Length(); }
    FloatVector2 Momentum () const { return m_mass * m_velocity; }
    FloatVector2 const &Force () const { return m_force; }
    Float AngularVelocity () const { return m_angular_velocity; }

    Dis::World *GetWorld () const;
    Dis::PhysicsHandler const *GetPhysicsHandler () const;
    Dis::PhysicsHandler *GetPhysicsHandler ();

    FloatVector2 AmbientVelocity (
        Float scan_area_radius,
        Entity const *ignore_me) const;
    static bool ShouldApplyCollisionForces (
        Entity const *entity1,
        Entity const *entity2)
    {
        if (entity1 == NULL || entity2 == NULL)
            return true;

        if (entity2->IsPowerup() &&
            (entity1->IsPlayerShip() ||
             entity1->GetEntityType() == ET_BALLISTIC))
            return false;

        if (entity1->IsPowerup() &&
            (entity2->IsPlayerShip() ||
             entity2->GetEntityType() == ET_BALLISTIC))
            return false;

        return true;
    }

    // ///////////////////////////////////////////////////////////////////////
    // public modifiers
    // ///////////////////////////////////////////////////////////////////////

    void SetNextTimeToThink (Float next_time_to_think)
    {
        m_next_time_to_think = next_time_to_think;
    }
    void SetElasticity (Float const elasticity)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(elasticity));
        m_elasticity = (elasticity >= 0.0f) ? elasticity : 0.0f;
    }
    void SetMass (Float const mass)
    {
        ASSERT1(mass > 0.0f);
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(mass));
        m_mass = mass;
    }
    void SetVelocity (FloatVector2 const &velocity)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(velocity[Dim::X]));
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(velocity[Dim::Y]));
        m_velocity = velocity;
    }
    void SetVelocityComponent (Uint32 const index, Float const value)
    {
        ASSERT1(index <= 1);
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(value));
        m_velocity[index] = value;
    }
    void SetMomentum (FloatVector2 const &momentum)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(momentum[Dim::X]));
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(momentum[Dim::Y]));
        m_velocity = momentum / m_mass;
    }
    void SetForce (FloatVector2 const &force)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(force[Dim::X]));
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(force[Dim::Y]));
        m_force = force;
    }
    void SetAngularVelocity (Float const angular_velocity)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(angular_velocity));
        m_angular_velocity = angular_velocity;
    }

    // ///////////////////////////////////////////////////////////////////////
    // public procedures
    // ///////////////////////////////////////////////////////////////////////

    // adds the given vector to the velocity
    void AccumulateVelocity (FloatVector2 const &velocity)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(velocity[Dim::X]));
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(velocity[Dim::Y]));
        m_velocity += velocity;
    }
    // accumulates velocity, given a momentum impulse
    void AccumulateMomentum (FloatVector2 const &momentum_impulse)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(momentum_impulse[Dim::X]));
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(momentum_impulse[Dim::Y]));
        m_velocity += momentum_impulse / m_mass;
    }
    // adds the given vector to the accumulating force vector for this frame
    void AccumulateForce (FloatVector2 const &force)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(force[Dim::X]));
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(force[Dim::Y]));
        m_force += force;
    }
    // resets the force vector to zero
    void ResetForce ()
    {
        m_force = FloatVector2::ms_zero;
    }
    // adds the given value to the angular velocity
    void AccumulateAngularVelocity (Float const angular_velocity)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(angular_velocity));
        m_angular_velocity += angular_velocity;
    }

    void ApplyInterceptCourseAcceleration (
        Entity *target,
        Float maximum_thrust_force,
        bool apply_force_on_target_also,
        bool reverse_thrust);
    void ApplyInterceptCourseAcceleration (
        Entity *target,
        Float maximum_thrust_force,
        bool apply_force_on_target_also,
        bool reverse_thrust,
        Polynomial::SolutionSet *solution_set);
    void ApplyInterceptCourseAcceleration (
        FloatVector2 const &target_position,
        FloatVector2 const &target_velocity,
        FloatVector2 const &target_acceleration,
        Float maximum_thrust_force,
        bool apply_force_on_target_also,
        bool reverse_thrust);
    // this one does all the heavy lifting, returning the force vector to
    // be applied to the target (which will be zero if
    // apply_force_on_target_also is false).
    FloatVector2 ApplyInterceptCourseAcceleration (
        FloatVector2 const &target_position,
        FloatVector2 const &target_velocity,
        FloatVector2 const &target_acceleration,
        Float maximum_thrust_force,
        bool apply_force_on_target_also,
        bool reverse_thrust,
        Polynomial::SolutionSet *solution_set);

    // ///////////////////////////////////////////////////////////////////////
    // public interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual bool IsMortal () const { return false; }
    virtual bool IsShip () const { return false; }
    virtual bool IsPlayerShip () const { return false; }
    virtual bool IsEnemyShip () const { return false; }
    virtual bool IsExplosive () const { return false; }
    virtual bool IsPowerup () const { return false; }
    virtual bool IsEffect () const { return false; }
    virtual bool IsBallistic () const { return false; }

    virtual void Think (Float time, Float frame_dt)
    {
        // if this Think method is not overridden, then don't Think often.
        SetNextTimeToThink(time + 10000.0f);
    }
    // the collision normal value points towards the entity
    virtual void Collide (
        Entity *collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
        Float time,
        Float frame_dt)
    { }

    // ///////////////////////////////////////////////////////////////////////
    // public Entity interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual void Write (Serializer &serializer) const { }
    virtual void HandleObjectLayerContainment (bool component_x, bool component_y);

protected:

    // does the calculation to see if/when the given entity will collide
    // with this one.
    Float CollisionTime (Entity *entity, Float lookahead_time) const;

    // ///////////////////////////////////////////////////////////////////////
    // protected Entity interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual void HandleNewOwnerObject () { }
    virtual void CloneProperties (Engine2::Entity const *entity) { }

private:

    EntityType const m_entity_type;
    Float m_next_time_to_think;
    EntityReference<Entity> m_reference;
    // indicates the collision response which the geometry in this entity provides
    CollisionType const m_collision_type;
    // elasticity factor (nominally between 0.0 and 1.0)
    Float m_elasticity;
    // entity's first moment of inertia (mass)
    Float m_mass;
    // velocity of the entity
    FloatVector2 m_velocity;
    // temporary storage of force applied to the entity for usage in calculations
    FloatVector2 m_force;
    // angular velocity of the entity (positive is counterclockwise)
    Float m_angular_velocity;
}; // end of class Entity

} // end of namespace Dis

#endif // !defined(_DIS_ENTITY_HPP_)

