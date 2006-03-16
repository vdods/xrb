// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_entityguts.h by Victor Dods, created 2005/10/21
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_ENTITYGUTS_H_)
#define _XRB_ENGINE2_ENTITYGUTS_H_

#include "xrb.h"

#include "xrb_engine2_entity.h"
#include "xrb_engine2_object.h"
#include "xrb_serializer.h"

namespace Xrb
{

class Serializer;

namespace Engine2
{

    class Entity;
    
    /** @brief Abstract interface for game-specific data stored in Entity.
      */
    class EntityGuts
    {
    public:

        typedef EntityGuts *(*ReadFunctionType)(Serializer &);
        
        EntityGuts ()
        {
            m_owner_entity = NULL;
        }
        virtual ~EntityGuts ()
        {
            ASSERT1(m_owner_entity == NULL)
        }

        static EntityGuts *Create (ReadFunctionType ReadFunction, Serializer &serializer)
        {
            ASSERT1(serializer.GetIODirection() == IOD_READ)
            return ReadFunction(serializer);
        }
        virtual void Write (Serializer &serializer) const = 0;

        inline Entity *GetOwnerEntity ()
        {
            return m_owner_entity;
        }

        // this function will be called when m_owner_entity is set to any
        // non-NULL value (when this EntityGuts is attached to an Entity)
        virtual void HandleNewOwnerEntity ()
        {
            ASSERT1(m_owner_entity != NULL)
        }
        // this function will be called right before this object is deleted
        // due to being ScheduleForDeletion'ed.
        virtual void HandleScheduledDeletion (Float time) { }
        
        // ///////////////////////////////////////////////////////////////////
        // Object method frontends
        // ///////////////////////////////////////////////////////////////////

        inline FloatVector2 const &GetTranslation () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetTranslation(); }
        inline FloatVector2 const &GetScaleFactors () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetScaleFactors(); }
        inline Float GetScaleFactor () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetScaleFactor(); }
        inline Float GetAngle () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetAngle(); }
        inline FloatTransform2 &GetTransform () { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetTransform(); }
        inline FloatTransform2 const &GetTransform () const { ASSERT3(m_owner_entity != NULL) return static_cast<Entity const *>(m_owner_entity)->GetTransform(); }
        inline FloatMatrix2 const &GetTransformation () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetTransformation(); }
        inline FloatMatrix2 GetTransformationInverse () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetTransformationInverse(); }
        inline Float GetDeterminant () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetDeterminant(); }
        inline void SetTranslation (FloatVector2 const &translation) { ASSERT3(m_owner_entity != NULL) m_owner_entity->SetTranslation(translation); m_owner_entity->ReAddToQuadTree(QTT_VISIBILITY); m_owner_entity->ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
        inline void SetTranslation (Float const x, Float const y) { ASSERT3(m_owner_entity != NULL) m_owner_entity->SetTranslation(x, y); m_owner_entity->ReAddToQuadTree(QTT_VISIBILITY); m_owner_entity->ReAddToQuadTree(QTT_PHYSICS_HANDLER);}
        inline void SetScaleFactors (FloatVector2 const &scale_factors) { ASSERT3(m_owner_entity != NULL) m_owner_entity->SetScaleFactors(scale_factors); m_owner_entity->ReAddToQuadTree(QTT_VISIBILITY); m_owner_entity->ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
        inline void SetScaleFactors (Float const r, Float const s) { ASSERT3(m_owner_entity != NULL) m_owner_entity->SetScaleFactors(r, s); m_owner_entity->ReAddToQuadTree(QTT_VISIBILITY); m_owner_entity->ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
        inline void SetScaleFactor (Float const scale_factor) { ASSERT3(m_owner_entity != NULL) m_owner_entity->SetScaleFactor(scale_factor); m_owner_entity->ReAddToQuadTree(QTT_VISIBILITY); m_owner_entity->ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
        inline void SetAngle (Float const angle) { ASSERT3(m_owner_entity != NULL) m_owner_entity->SetAngle(angle); }
        inline void Translate (FloatVector2 const &translation) { ASSERT3(m_owner_entity != NULL) m_owner_entity->Translate(translation); m_owner_entity->ReAddToQuadTree(QTT_VISIBILITY); m_owner_entity->ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
        inline void Translate (Float const x, Float const y) { ASSERT3(m_owner_entity != NULL) m_owner_entity->Translate(x, y); m_owner_entity->ReAddToQuadTree(QTT_VISIBILITY); m_owner_entity->ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
        inline void Scale (FloatVector2 const &scale_factors) { ASSERT3(m_owner_entity != NULL) m_owner_entity->Scale(scale_factors); m_owner_entity->ReAddToQuadTree(QTT_VISIBILITY); m_owner_entity->ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
        inline void Scale (Float const r, Float const s) { ASSERT3(m_owner_entity != NULL) m_owner_entity->Scale(r, s); m_owner_entity->ReAddToQuadTree(QTT_VISIBILITY); m_owner_entity->ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
        inline void Scale (Float const scale_factor) { ASSERT3(m_owner_entity != NULL) m_owner_entity->Scale(scale_factor); m_owner_entity->ReAddToQuadTree(QTT_VISIBILITY); m_owner_entity->ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
        inline void Rotate (Float const angle) { ASSERT3(m_owner_entity != NULL) m_owner_entity->Rotate(angle); }
        inline void ResetTranslation () { ASSERT3(m_owner_entity != NULL) m_owner_entity->ResetTranslation(); m_owner_entity->ReAddToQuadTree(QTT_VISIBILITY); m_owner_entity->ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
        inline void ResetScale () { ASSERT3(m_owner_entity != NULL) m_owner_entity->ResetScale(); m_owner_entity->ReAddToQuadTree(QTT_VISIBILITY); m_owner_entity->ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
        inline void ResetAngle () { ASSERT3(m_owner_entity != NULL) m_owner_entity->ResetAngle(); }
    
        inline Float GetRadius () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetRadius(); }
        inline Float GetRadiusSquared () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetRadiusSquared(); }
        inline ObjectLayer *GetObjectLayer () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetObjectLayer(); }
        inline World *GetWorld () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetWorld(); }
    
        // ///////////////////////////////////////////////////////////////////
        // Entity method frontends
        // ///////////////////////////////////////////////////////////////////

        template <typename PhysicsHandlerClass>
        inline PhysicsHandlerClass const *GetPhysicsHandler () const
        {
            return DStaticCast<PhysicsHandlerClass const *>(m_owner_entity->GetPhysicsHandler());
        }
        template <typename PhysicsHandlerClass>
        inline PhysicsHandlerClass *GetPhysicsHandler ()
        {
            return DStaticCast<PhysicsHandlerClass *>(m_owner_entity->GetPhysicsHandler());
        }
        inline std::string const &GetName () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetName(); }
        inline FloatVector2 const &GetWrappedOffset () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetWrappedOffset(); }
        inline FloatVector2 GetUnwrappedTranslation () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetUnwrappedTranslation(); }
        inline bool GetIsInWorld () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetIsInWorld(); }
        inline Float GetElasticity () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetElasticity(); }
        inline Float GetFirstMoment () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetFirstMoment(); }
        inline Float GetSpeed () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetSpeed(); }
        inline FloatVector2 const &GetVelocity () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetVelocity(); }
        inline FloatVector2 GetMomentum () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetMomentum(); }
        inline Float GetSecondMoment () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetSecondMoment(); }
        inline Float GetAngularVelocity () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetAngularVelocity(); }
        inline Float GetAngularMomentum () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetAngularMomentum(); }
        inline bool GetAppliesGravity () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetAppliesGravity(); }
        inline bool GetReactsToGravity () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetReactsToGravity(); }
        inline CollisionType GetCollisionType () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetCollisionType(); }
        inline FloatVector2 const &GetForce () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetForce(); }
        inline Float GetTorque () const { ASSERT3(m_owner_entity != NULL) return m_owner_entity->GetTorque(); }
        
        inline void SetElasticity (Float const elasticity) { ASSERT3(m_owner_entity != NULL) m_owner_entity->SetElasticity(elasticity); }
        inline void SetFirstMoment (Float const first_moment) { ASSERT3(m_owner_entity != NULL) m_owner_entity->SetFirstMoment(first_moment); }
        inline void SetVelocity (FloatVector2 const &velocity) { ASSERT3(m_owner_entity != NULL) m_owner_entity->SetVelocity(velocity); }
        inline void SetVelocityComponent (Uint32 const index, Float const value) { ASSERT3(m_owner_entity != NULL) m_owner_entity->SetVelocityComponent(index, value); }
        inline void SetSecondMoment (Float const second_moment) { ASSERT3(m_owner_entity != NULL) m_owner_entity->SetSecondMoment(second_moment); }
        inline void SetAngularVelocity (Float const angular_velocity) { ASSERT3(m_owner_entity != NULL) m_owner_entity->SetAngularVelocity(angular_velocity); }
        inline void SetAppliesGravity (bool const applies_gravity) { ASSERT3(m_owner_entity != NULL) m_owner_entity->SetAppliesGravity(applies_gravity); }
        inline void SetReactsToGravity (bool const reacts_to_gravity) { ASSERT3(m_owner_entity != NULL) m_owner_entity->SetReactsToGravity(reacts_to_gravity); }
        inline void SetCollisionType (CollisionType const collision_type) { ASSERT3(m_owner_entity != NULL) m_owner_entity->SetCollisionType(collision_type); }
        inline void SetForce (FloatVector2 const &force) { ASSERT3(m_owner_entity != NULL) m_owner_entity->SetForce(force); }
        inline void SetTorque (Float const torque) { ASSERT3(m_owner_entity != NULL) m_owner_entity->SetTorque(torque); }
        
        inline void AccumulateVelocity (FloatVector2 const &velocity) { ASSERT3(m_owner_entity != NULL) m_owner_entity->AccumulateVelocity(velocity); }
        inline void AccumulateAngularVelocity (Float const angular_velocity) { ASSERT3(m_owner_entity != NULL) m_owner_entity->AccumulateAngularVelocity(angular_velocity); }
        inline void AccumulateMomentum (FloatVector2 const &momentum_impulse) { ASSERT3(m_owner_entity != NULL) m_owner_entity->AccumulateMomentum(momentum_impulse); }
        inline void AccumulateAngularMomentum (Float const angular_momentum_impulse) { ASSERT3(m_owner_entity != NULL) m_owner_entity->AccumulateAngularMomentum(angular_momentum_impulse); }
        inline void AccumulateForce (FloatVector2 const &force) { ASSERT3(m_owner_entity != NULL) m_owner_entity->AccumulateForce(force); }
        inline void AccumulateTorque (Float const torque) { ASSERT3(m_owner_entity != NULL) m_owner_entity->AccumulateTorque(torque); }
        inline void ResetForce () { ASSERT3(m_owner_entity != NULL) m_owner_entity->ResetForce(); }
        inline void ResetTorque () { ASSERT3(m_owner_entity != NULL) m_owner_entity->ResetTorque(); }

        inline void RemoveFromWorld () { ASSERT3(m_owner_entity != NULL) m_owner_entity->RemoveFromWorld(); }
        inline void AddBackIntoWorld () { ASSERT3(m_owner_entity != NULL) m_owner_entity->AddBackIntoWorld(); }

        inline void ScheduleForRemovalFromWorld (Float const time_delay) { ASSERT3(m_owner_entity != NULL) m_owner_entity->ScheduleForRemovalFromWorld(time_delay); } 
        
        inline void ScheduleForDeletion (Float const time_delay) { ASSERT3(m_owner_entity != NULL) m_owner_entity->ScheduleForDeletion(time_delay); }
        
    private:

        Entity *m_owner_entity;

        friend class Entity;
    }; // end of class Engine2::EntityGuts

} // end of namespace Engine2

} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_ENTITYGUTS_H_)

