// ///////////////////////////////////////////////////////////////////////////
// dis_physicshandler.h by Victor Dods, created 2005/11/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_PHYSICSHANDLER_H_)
#define _DIS_PHYSICSHANDLER_H_

#include "xrb_engine2_physicshandler.h"

#include <set>

#include "dis_areatracelist.h"
#include "dis_collisionpair.h"
#include "dis_linetracebinding.h"
#include "xrb_vector.h"

using namespace Xrb;

namespace Dis
{

class CollisionQuadTree;

class PhysicsHandler : public Engine2::PhysicsHandler
{
public:

    PhysicsHandler ();
    virtual ~PhysicsHandler ();

    // ///////////////////////////////////////////////////////////////////////
    // Engine2::PhysicsHandler public method overrides
    // ///////////////////////////////////////////////////////////////////////

    // this does wrapped object layer checking
    bool GetDoesAreaOverlapAnyEntityInObjectLayer (
        Engine2::ObjectLayer const *object_layer,
        FloatVector2 const &area_center,
        Float area_radius,
        bool check_nonsolid_collision_entities) const;

    // does a line trace in the quadtree
    void LineTrace (
        Engine2::ObjectLayer const *object_layer,
        FloatVector2 const &trace_start,
        FloatVector2 const &trace_vector,
        Float trace_radius,
        bool check_nonsolid_collision_entities,
        LineTraceBindingSet *line_trace_binding_set) const;
        
    // does an area trace in the quadtree
    void AreaTrace (
        Engine2::ObjectLayer const *object_layer,
        FloatVector2 trace_area_center,
        Float trace_area_radius,
        bool check_nonsolid_collision_entities,
        AreaTraceList *area_trace_list) const;
        
    virtual void AddObjectLayer (Engine2::ObjectLayer *object_layer);
    virtual void SetMainObjectLayer (Engine2::ObjectLayer *object_layer);
        
    virtual void AddEntity (Engine2::Entity *entity);
    virtual void RemoveEntity (Engine2::Entity *entity);

    virtual void HandleChangedEntityRadius (
        Engine2::Entity *entity,
        Float old_radius,
        Float new_radius);
    virtual void HandleChangedEntityCollisionType (
        Engine2::Entity *entity,
        Engine2::CollisionType old_collision_type,
        Engine2::CollisionType new_collision_type);
    virtual void HandleChangedEntityAppliesGravity (
        Engine2::Entity *entity,
        bool old_applies_gravity,
        bool new_applies_gravity);
    virtual void HandleChangedEntityReactsToGravity (
        Engine2::Entity *entity,
        bool old_reacts_to_gravity,
        bool new_reacts_to_gravity);

protected:

    virtual void ProcessFrameOverride ();
            
private:

    typedef std::set<Engine2::Entity *> EntitySet;
    typedef EntitySet::iterator EntitySetIterator;

    void ApplyGravitationalForce ();
    void UpdateVelocities ();
    void UpdatePositions ();
    void HandleInterpenetrations ();
    void HandleInterpenetrationsWrapped ();
    void HandleInterpenetrationsUsingCollisionQuadTree ();
    void HandleInterpenetrationsUsingCollisionQuadTreeWrapped ();
    void CrossProductBetweenGravitationalEntitySets (
        EntitySet *set0,
        EntitySet *set1);

    // set of all added entities
    EntitySet m_entity_set;

    // set of applies-gravity entities
    EntitySet m_applies_gravity_set;
    // set of applies-gravity and reacts-to-gravity entities
    EntitySet m_applies_and_reacts_to_gravity_set;
    // set of reacts-to-gravity entities
    EntitySet m_reacts_to_gravity_set;

    // the list of collision pairs for this frame
    CollisionPairList m_collision_pair_list;
    
    // the universal gravitational constant
    Float m_gravitational_constant;

    // keeps track of the main object layer (really only used to make
    // sure that all entities are added to the main object layer)
    Engine2::ObjectLayer *m_main_object_layer;
    // the quadtree used in collision detection and other spatial shit
    CollisionQuadTree *m_quad_tree;
}; // end of class PhysicsHandler

} // end of namespace Dis

#endif // !defined(_DIS_PHYSICSHANDLER_H_)
