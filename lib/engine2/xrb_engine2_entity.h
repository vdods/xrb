// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_entity.h by Victor Dods, created 2004/07/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_ENTITY_H_)
#define _XRB_ENGINE2_ENTITY_H_

#include "xrb.h"

#include <string>

#include "xrb_engine2_object.h"

namespace Xrb
{

namespace Engine2
{

    class EntityGuts;
    class ObjectLayer;
    class PhysicsHandler;
    
    // "entity" is a perfectly good word to describe what this class is...
    // so what if it's a ripoff of quake3? :)
    class Entity : public virtual Object
    {
    public:
    
        typedef Sint32 Index;
    
        virtual ~Entity ();

        // ///////////////////////////////////////////////////////////////////
        // public serialization functions
        // ///////////////////////////////////////////////////////////////////
        
        // write a Create function once plain Entities are supported
        
        // makes calls to WriteClassSpecific for this and all superclasses
        virtual void Write (Serializer &serializer) const;
            
        // ///////////////////////////////////////////////////////////////////
        // public accessors
        // ///////////////////////////////////////////////////////////////////

        PhysicsHandler *GetPhysicsHandler ();
        inline EntityGuts *GetEntityGuts () 
        {
            return m_entity_guts;
        }
        inline Index GetNumber () const
        {
            return m_number;
        }
        inline std::string const &GetName () const
        {
            return m_name;
        }
        inline FloatVector2 const &GetWrappedOffset () const
        {
            return m_wrapped_offset;
        }
        inline FloatVector2 GetUnwrappedTranslation () const
        {
            return GetTranslation() + m_wrapped_offset;
        }
        inline bool GetIsInWorld () const
        {
            return m_number >= 0;
        }
        inline Float GetElasticity () const
        {
            return m_elasticity;
        }
        inline Float GetFirstMoment () const
        {
            return m_first_moment;
        }
        inline Float GetSpeed () const
        {
            return m_velocity.GetLength();
        }
        inline FloatVector2 const &GetVelocity () const
        {
            return m_velocity;
        }
        inline FloatVector2 GetMomentum () const
        {
            return m_first_moment * m_velocity;
        }
        inline Float GetSecondMoment () const
        {
            return m_second_moment;
        }
        inline Float GetAngularVelocity () const
        {
            return m_angular_velocity;
        }
        inline Float GetAngularMomentum () const
        {
            return m_second_moment * m_angular_velocity;
        }
        inline bool GetAppliesGravity () const
        {
            return m_applies_gravity;
        }
        inline bool GetReactsToGravity () const
        {
            return m_reacts_to_gravity;
        }
        inline CollisionType GetCollisionType () const
        {
            return m_collision_type;
        }
    
        // returns the force vector which has been accumulated during this frame
        inline FloatVector2 const &GetForce () const
        {
            return m_force;
        }
        // returns the torque value which has been accumulated during this frame
        inline Float GetTorque () const
        {
            return m_torque;
        }

        // ///////////////////////////////////////////////////////////////////
        // public modifiers
        // ///////////////////////////////////////////////////////////////////
        
        void SetEntityGuts (EntityGuts *const entity_guts);
        inline void SetNumber (Index const number)
        {
            m_number = number;
        }
        inline void SetName (std::string const &name)
        {
            m_name = name;
        }
        inline void SetWrappedOffset (FloatVector2 const &wrapped_offset)
        {
            ASSERT_NAN_SANITY_CHECK(Math::IsFinite(wrapped_offset[Dim::X]))
            ASSERT_NAN_SANITY_CHECK(Math::IsFinite(wrapped_offset[Dim::Y]))
            m_wrapped_offset = wrapped_offset;
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
            m_velocity.m[index] = value;
        }
        inline void SetSecondMoment (Float const second_moment)
        {
            ASSERT1(second_moment > 0.0f)
            ASSERT_NAN_SANITY_CHECK(Math::IsFinite(second_moment))
            m_second_moment = second_moment;
        }
        inline void SetAngularVelocity (Float const angular_velocity)
        {
            ASSERT_NAN_SANITY_CHECK(Math::IsFinite(angular_velocity))
            m_angular_velocity = angular_velocity;
        }
        void SetAppliesGravity (bool applies_gravity);
        void SetReactsToGravity (bool reacts_to_gravity);
        void SetCollisionType (CollisionType collision_type);
    
        inline void SetForce (FloatVector2 const &force)
        {
            ASSERT_NAN_SANITY_CHECK(Math::IsFinite(force[Dim::X]))
            ASSERT_NAN_SANITY_CHECK(Math::IsFinite(force[Dim::Y]))
            m_force = force;
        }
        inline void SetTorque (Float const torque)
        {
            ASSERT_NAN_SANITY_CHECK(Math::IsFinite(torque))
            m_torque = torque;
        }

        // ///////////////////////////////////////////////////////////////////
        // public overrides from Engine2::Object
        // ///////////////////////////////////////////////////////////////////
        
        virtual void SetScaleFactors (FloatVector2 const &scale_factors);
        virtual void SetScaleFactors (Float r, Float s);
        virtual void SetScaleFactor (Float scale_factor);
        virtual void Scale (FloatVector2 const &scale_factors);
        virtual void Scale (Float r, Float s);
        virtual void Scale (Float scale_factor);
        virtual void ResetScale ();

        // ///////////////////////////////////////////////////////////////////
        // public procedures
        // ///////////////////////////////////////////////////////////////////

        // moves an entity which has changed position/size to the correct
        // quadnode, using its current quadnode as the starting point
        // for high efficiency.        
        void ReAddToQuadTree (QuadTreeType quad_tree_type);
            
        inline void AccumulateWrappedOffset (FloatVector2 const &wrapped_offset_delta)
        {
            ASSERT_NAN_SANITY_CHECK(Math::IsFinite(wrapped_offset_delta[Dim::X]))
            ASSERT_NAN_SANITY_CHECK(Math::IsFinite(wrapped_offset_delta[Dim::Y]))
            m_wrapped_offset += wrapped_offset_delta;
        }
        // adds the given vector to the velocity
        inline void AccumulateVelocity (FloatVector2 const &velocity)
        {
            ASSERT_NAN_SANITY_CHECK(Math::IsFinite(velocity[Dim::X]))
            ASSERT_NAN_SANITY_CHECK(Math::IsFinite(velocity[Dim::Y]))
            m_velocity += velocity;
        }
        // adds the given value to the angular velocity
        inline void AccumulateAngularVelocity (Float const angular_velocity)
        {
            ASSERT_NAN_SANITY_CHECK(Math::IsFinite(angular_velocity))
            m_angular_velocity += angular_velocity;
        }
        // accumulates velocity, given a momentum impulse
        inline void AccumulateMomentum (FloatVector2 const &momentum_impulse)
        {
            ASSERT_NAN_SANITY_CHECK(Math::IsFinite(momentum_impulse[Dim::X]))
            ASSERT_NAN_SANITY_CHECK(Math::IsFinite(momentum_impulse[Dim::Y]))
            m_velocity += momentum_impulse / m_first_moment;
        }
        // accumulates angular velocity, given an angular momentum impulse
        inline void AccumulateAngularMomentum (Float const angular_momentum_impulse)
        {
            ASSERT_NAN_SANITY_CHECK(Math::IsFinite(angular_momentum_impulse))
            m_angular_velocity += angular_momentum_impulse / m_second_moment;
        }
        // adds the given vector to the accumulating force vector for this frame
        inline void AccumulateForce (FloatVector2 const &force)
        {
            ASSERT_NAN_SANITY_CHECK(Math::IsFinite(force[Dim::X]))
            ASSERT_NAN_SANITY_CHECK(Math::IsFinite(force[Dim::Y]))
            m_force += force;
        }
        // adds the given torque to the accumulating torque value for this frame
        inline void AccumulateTorque (Float const torque)
        {
            ASSERT_NAN_SANITY_CHECK(Math::IsFinite(torque))
            m_torque += torque;
        }
        // resets the force vector
        inline void ResetForce ()
        {
            m_force = FloatVector2::ms_zero;
        }
        // resets the torque value
        inline void ResetTorque ()
        {
            m_torque = 0.0f;
        }
        // remove an entity from the world and physics handler (but don't delete it).
        void RemoveFromWorld ();
        // add a previously removed entity back into the world and object
        // layer it previously resided in.  must not already be in the world.
        void AddBackIntoWorld ();
        // resets the ent num to the sentinel 'not in world' number of -1
        inline void ResetNumber ()
        {
            m_number = -1;
        }

        // ///////////////////////////////////////////////////////////////////
        // event-scheduling procedures
        // ///////////////////////////////////////////////////////////////////

        void ScheduleForRemovalFromWorld (Float time_delay);
        
        void ScheduleForDeletion (Float time_delay);
        
    protected:
    
        // protected constructor so that you must use Create()
        Entity ();
    
        // ///////////////////////////////////////////////////////////////////
        // protected serialization functions
        // ///////////////////////////////////////////////////////////////////
        
        // does the guts of serializing reading for this class (doesn't read
        // the object subtype)
        void ReadClassSpecific (Serializer &serializer);
        // does the guts of serializing writing for this class (doesn't write
        // the object subtype)
        void WriteClassSpecific (Serializer &serializer) const;
        
        // ///////////////////////////////////////////////////////////////////
        
        // copies the properties of the given object to this object
        void CloneProperties (Object const *object);
        
        // elasticity factor (nominally between 0.0 and 1.0)
        Float m_elasticity;
        // entity's first moment of inertia (mass)
        Float m_first_moment;
        // velocity of the entity
        FloatVector2 m_velocity;
        // entity's second moment of inertia ('angular mass')
        Float m_second_moment;
        // angular velocity of the entity (positive is counterclockwise)
        Float m_angular_velocity;
        
    private:

        // does code common to each of the scale-factor-changing Object method overrides
        void HandleChangedRadius (Float old_radius, Float new_radius);
    
        // the interface class pointer to the game-specific guts of this entity.
        EntityGuts *m_entity_guts;
        // entity number (index in World::m_entity_vector)
        Index m_number;
        // the name of the entity (used for debugging mainly)
        std::string m_name;
        // the offset vector for wrapped object layers
        FloatVector2 m_wrapped_offset;
        // temporary storage of force applied to the entity for usage in calculations
        FloatVector2 m_force;
        // temporary storage of torque applied to the entity for usage in calculations
        Float m_torque;
        // flag to indicate if this entity applies gravity to others
        bool m_applies_gravity;
        // flag to indicate if this entity reacts to gravity from other entities
        bool m_reacts_to_gravity;
        // indicates the collision response which the geometry in this entity provides
        CollisionType m_collision_type;
    }; // end of class Engine2::Entity

} // end of namespace Engine2

} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_ENTITY_H_)
