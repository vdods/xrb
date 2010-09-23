// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_circle_physicshandler.hpp by Victor Dods, created 2005/11/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_CIRCLE_PHYSICSHANDLER_HPP_)
#define _XRB_ENGINE2_CIRCLE_PHYSICSHANDLER_HPP_

#include "xrb.hpp"

#include <set>

#include "xrb_engine2_circle_types.hpp"
#include "xrb_engine2_physicshandler.hpp"
#include "xrb_vector.hpp"

namespace Xrb {
namespace Engine2 {
namespace Circle {

class CollisionQuadTree;
class Entity;

class PhysicsHandler : public Engine2::PhysicsHandler
{
public:

    PhysicsHandler ();
    virtual ~PhysicsHandler ();

    // this does wrapped object layer checking
    bool DoesAreaOverlapAnyEntityInObjectLayer (
        ObjectLayer const *object_layer,
        FloatVector2 const &area_center,
        Float area_radius,
        bool check_nonsolid_collision_entities) const;

    // does a line trace in the quadtree
    void LineTrace (
        ObjectLayer const *object_layer,
        FloatVector2 const &trace_start,
        FloatVector2 const &trace_vector,
        Float trace_radius,
        bool check_nonsolid_collision_entities,
        LineTraceBindingSet *line_trace_binding_set) const;

    // does an area trace in the quadtree
    void AreaTrace (
        ObjectLayer const *object_layer,
        FloatVector2 trace_area_center,
        Float trace_area_radius,
        bool check_nonsolid_collision_entities,
        AreaTraceList *area_trace_list) const;

    // gives the local momentum (and mass) in a given radius
    void CalculateAmbientMomentum (
        ObjectLayer const *object_layer,
        FloatVector2 const &scan_area_center,
        Float scan_area_radius,
        Entity const *ignore_me,
        FloatVector2 &ambient_momentum,
        Float &ambient_mass) const;

    // ///////////////////////////////////////////////////////////////////////
    // Engine2::PhysicsHandler public method overrides
    // ///////////////////////////////////////////////////////////////////////

    virtual void AddObjectLayer (ObjectLayer *object_layer);
    virtual void SetMainObjectLayer (ObjectLayer *object_layer);

    virtual void AddEntity (Engine2::Entity *entity);
    virtual void RemoveEntity (Engine2::Entity *entity);

    // return true iff there should be no physical collision response between the given entities.
    // Collide is still called on the entities even if they're exempt.
    virtual bool CollisionExemption (Entity const &entity0, Entity const &entity1) const
    {
        // no exemptions by default
        return false;
    }
    // return the upper speed limit for entities (Entity velocity will be
    // capped at this value).
    virtual Float MaxSpeed (Entity const &entity) const
    {
        // should be effectively infinity (i.e. no max speed)
        return 1.0e10f;
    }
    // perform physical collision response between two entities with given
    // collision conditions.  the return value should be the [absolute value
    // of the] collision force (imparted on both collision entities).
    // offset_0_to_1 is the precalculated difference vector
    // entity1.Translation() - entity0.Translation(), adjusted to account for
    // ObjectLayer wrapping.  collision_normal_0_to_1 is the normalized
    // value of offset_0_to_1, or an arbitrary value if offset_0_to_1 is zero.
    virtual Float CollisionResponse (
        Entity &entity0,
        Entity &entity1,
        FloatVector2 const &offset_0_to_1,
        Float frame_dt,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal_0_to_1);

protected:

    virtual void HandleFrame ();

    typedef std::set<Entity *> EntitySet;

    // set of all added entities
    EntitySet m_entity_set;

private:

    void UpdateVelocities ();
    void UpdatePositions ();
    void HandleInterpenetrations ();

    // the list of collision pairs for this frame
    CollisionPairList m_collision_pair_list;
    // keeps track of the main object layer (really only used to make
    // sure that all entities are added to the main object layer)
    ObjectLayer *m_main_object_layer;
    // the quadtree used in collision detection and other spatial shit
    CollisionQuadTree *m_quad_tree;
}; // end of class PhysicsHandler

} // end of namespace Circle
} // end of namespace Engine2
} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_CIRCLE_PHYSICSHANDLER_HPP_)
