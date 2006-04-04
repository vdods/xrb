// ///////////////////////////////////////////////////////////////////////////
// dis_entity.h by Victor Dods, created 2005/11/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_GAMEOBJECT_H_)
#define _DIS_GAMEOBJECT_H_

#include "xrb_engine2_entity.h"

#include "dis_enums.h"
#include "dis_entityreference.h"
#include "xrb_polynomial.h"

using namespace Xrb;

namespace Dis
{

class PhysicsHandler;
class World;

// abstract baseclass for all disasteroids game object classes
class Entity : public Engine2::Entity
{
public:

    Entity (EntityType type, CollisionType collision_type);
    virtual ~Entity ()
    {
        if (m_reference.GetIsValid())
            m_reference.NullifyEntity();
    }
    
    // ///////////////////////////////////////////////////////////////////////
    // public accessors
    // ///////////////////////////////////////////////////////////////////////

    inline EntityType GetEntityType () const { return m_type; }
    inline Float GetNextTimeToThink () const { return m_next_time_to_think; }
    inline EntityReference<Entity> const &GetReference ()
    {
        if (!m_reference.GetIsValid())
        {
            m_reference.SetInstance(new EntityInstance(this));
            ASSERT1(m_reference.GetIsValid())
        }
        return m_reference;
    }
    inline CollisionType GetCollisionType () const { return m_collision_type; }
    inline Float GetElasticity () const { return m_elasticity; }
    inline Float GetFirstMoment () const { return m_first_moment; }
    inline FloatVector2 const &GetVelocity () const { return m_velocity; }
    inline Float GetSpeed () const { return m_velocity.GetLength(); }
    inline FloatVector2 GetMomentum () const { return m_first_moment * m_velocity; }
    inline Float GetAngularVelocity () const { return m_angular_velocity; }
    inline FloatVector2 const &GetForce () const { return m_force; }

    Dis::World *GetWorld () const;
    Dis::PhysicsHandler const *GetPhysicsHandler () const;
    Dis::PhysicsHandler *GetPhysicsHandler ();
    
    FloatVector2 GetAmbientVelocity (
        Float scan_area_radius,
        Entity const *ignore_me) const;
    static inline bool GetShouldApplyCollisionForces (
        Entity const *entity1,
        Entity const *entity2)
    {
        if (entity1 == NULL || entity2 == NULL)
            return true;

        if (entity2->GetIsPowerup() &&
            (entity1->GetIsPlayerShip() ||
             entity1->GetEntityType() == ET_BALLISTIC))
            return false;
            
        if (entity1->GetIsPowerup() &&
            (entity2->GetIsPlayerShip() ||
             entity2->GetEntityType() == ET_BALLISTIC))
            return false;

        return true;
    }
    
    // ///////////////////////////////////////////////////////////////////////
    // public modifiers
    // ///////////////////////////////////////////////////////////////////////
    
    inline void SetNextTimeToThink (Float next_time_to_think)
    {
        m_next_time_to_think = next_time_to_think;
    }    
    inline void SetElasticity (Float const elasticity)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(elasticity))
        m_elasticity = (elasticity >= 0.0f) ? elasticity : 0.0f;
    }
    inline void SetFirstMoment (Float const first_moment)
    {
        ASSERT1(first_moment > 0.0f)
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(first_moment))
        m_first_moment = first_moment;
    }
    inline void SetVelocity (FloatVector2 const &velocity)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(velocity[Dim::X]))
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(velocity[Dim::Y]))
        m_velocity = velocity;
    }
    inline void SetVelocityComponent (Uint32 const index, Float const value)
    {
        ASSERT1(index <= 1)
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(value))
        m_velocity[index] = value;
    }
    inline void SetForce (FloatVector2 const &force)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(force[Dim::X]))
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(force[Dim::Y]))
        m_force = force;
    }
    inline void SetAngularVelocity (Float const angular_velocity)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(angular_velocity))
        m_angular_velocity = angular_velocity;
    }

    // ///////////////////////////////////////////////////////////////////////
    // public procedures
    // ///////////////////////////////////////////////////////////////////////

    // adds the given vector to the velocity
    inline void AccumulateVelocity (FloatVector2 const &velocity)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(velocity[Dim::X]))
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(velocity[Dim::Y]))
        m_velocity += velocity;
    }
    // accumulates velocity, given a momentum impulse
    inline void AccumulateMomentum (FloatVector2 const &momentum_impulse)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(momentum_impulse[Dim::X]))
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(momentum_impulse[Dim::Y]))
        m_velocity += momentum_impulse / m_first_moment;
    }
    // adds the given vector to the accumulating force vector for this frame
    inline void AccumulateForce (FloatVector2 const &force)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(force[Dim::X]))
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(force[Dim::Y]))
        m_force += force;
    }
    // resets the force vector to zero
    inline void ResetForce ()
    {
        m_force = FloatVector2::ms_zero;
    }
    // adds the given value to the angular velocity
    inline void AccumulateAngularVelocity (Float const angular_velocity)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(angular_velocity))
        m_angular_velocity += angular_velocity;
    }

    inline void ApplyInterceptCourseAcceleration (
        Entity *target,
        Float maximum_thrust_force,
        bool apply_force_on_target_also)
    {
        Polynomial::SolutionSet solution_set;
        ApplyInterceptCourseAcceleration(
            target,
            maximum_thrust_force,
            apply_force_on_target_also,
            &solution_set);
    }
    void ApplyInterceptCourseAcceleration (
        Entity *target,
        Float maximum_thrust_force,
        bool apply_force_on_target_also,
        Polynomial::SolutionSet *solution_set);

    // ///////////////////////////////////////////////////////////////////////
    // public interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual bool GetIsMortal () const { return false; }
    virtual bool GetIsShip () const { return false; }
    virtual bool GetIsPlayerShip () const { return false; }
    virtual bool GetIsEnemyShip () const { return false; }
    virtual bool GetIsExplosive () const { return false; }
    virtual bool GetIsPowerup () const { return false; }
    virtual bool GetIsEffect () const { return false; }
    virtual bool GetIsBallistic () const { return false; }

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

    // this is the Engine2::CreateEntityFunction to create 
    // Entity subclass objects from a serializer.
    static Engine2::Entity *Create (Serializer &serializer);
    virtual void Write (Serializer &serializer) const;
    virtual void HandleObjectLayerContainment (bool component_x, bool component_y);
    virtual void HandleScheduledDeletion (Float time);

protected:

    static EntityType ReadEntityType (Serializer &serializer);
    void WriteEntityType (Serializer &serializer) const;

    // ///////////////////////////////////////////////////////////////////////
    // protected Entity interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual void HandleNewOwnerObject () { }
    virtual void CloneProperties (Engine2::Entity const *entity) { }

private:

    EntityType const m_type;
    Float m_next_time_to_think;
    EntityReference<Entity> m_reference;
    // indicates the collision response which the geometry in this entity provides
    CollisionType const m_collision_type;
    // elasticity factor (nominally between 0.0 and 1.0)
    Float m_elasticity;
    // entity's first moment of inertia (mass)
    Float m_first_moment;
    // velocity of the entity
    FloatVector2 m_velocity;
    // temporary storage of force applied to the entity for usage in calculations
    FloatVector2 m_force;
    // angular velocity of the entity (positive is counterclockwise)
    Float m_angular_velocity;
}; // end of class Entity

} // end of namespace Dis

#endif // !defined(_DIS_GAMEOBJECT_H_)

