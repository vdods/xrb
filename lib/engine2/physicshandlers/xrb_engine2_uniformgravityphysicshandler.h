// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_uniformgravityphysicshandler.h by Victor Dods, created 2005/04/16
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_UNIFORMGRAVITYPHYSICSHANDLER_H_)
#define _XRB_ENGINE2_UNIFORMGRAVITYPHYSICSHANDLER_H_

#include "xrb.h"

#include <set>

#include "xrb_engine2_physicshandler.h"
#include "xrb_vector.h"

namespace Xrb
{

namespace Engine2
{
    
    class Entity;
    
    class UniformGravityPhysicsHandler : public PhysicsHandler
    {
    public:
    
        UniformGravityPhysicsHandler ();
        virtual ~UniformGravityPhysicsHandler ();

        virtual void AddObjectLayer (ObjectLayer *object_layer) { }
        virtual void SetMainObjectLayer (ObjectLayer *object_layer) { }
                    
        virtual void AddEntity (Entity *entity);
        virtual void RemoveEntity (Entity *entity);

        virtual void HandleChangedEntityRadius (
            Entity *entity,
            Float old_radius,
            Float new_radius)
        { }
        virtual void HandleChangedEntityCollisionType (
            Entity *entity,
            CollisionType old_collision_type,
            CollisionType new_collision_type)
        { }
        virtual void HandleChangedEntityAppliesGravity (
            Entity *entity,
            bool old_applies_gravity,
            bool new_applies_gravity)
        { }
        virtual void HandleChangedEntityReactsToGravity (
            Entity *entity,
            bool old_reacts_to_gravity,
            bool new_reacts_to_gravity);

    protected:
    
        virtual void ProcessFrameOverride ();
                        
    private:
    
        void ApplyGravitationalForce ();
        void UpdateVelocities ();
        void UpdatePositions ();
        void HandleCollisions ();
        void HandleInterpenetrations ();
    
        typedef std::set<Entity *> EntitySet;
        typedef EntitySet::iterator EntitySetIterator;
    
        // set of all added entities
        EntitySet m_entity_set;
    
        // set of reacts-to-gravity entities
        EntitySet m_reacts_to_gravity_set;
    
        // the acceleration vector which describes the uniform gravity
        FloatVector2 m_gravitational_acceleration;    
    }; // end of class Engine2::UniformGravityPhysicsHandler

} // end of namespace Engine2

} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_UNIFORMGRAVITYPHYSICSHANDLER_H_)
