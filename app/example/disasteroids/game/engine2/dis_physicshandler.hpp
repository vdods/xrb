// ///////////////////////////////////////////////////////////////////////////
// dis_physicshandler.hpp by Victor Dods, created 2005/11/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_PHYSICSHANDLER_HPP_)
#define _DIS_PHYSICSHANDLER_HPP_

#include "xrb_engine2_physicshandler.hpp"

#include <set>

#include "dis_areatracelist.hpp"
#include "dis_collisionpair.hpp"
#include "dis_linetracebinding.hpp"
#include "xrb_vector.hpp"

using namespace Xrb;

namespace Dis
{

class CollisionQuadTree;
class Entity;

class PhysicsHandler : public Engine2::PhysicsHandler
{
public:

    PhysicsHandler ();
    virtual ~PhysicsHandler ();

    // ///////////////////////////////////////////////////////////////////////
    // Engine2::PhysicsHandler public method overrides
    // ///////////////////////////////////////////////////////////////////////

    // this does wrapped object layer checking
    bool DoesAreaOverlapAnyEntityInObjectLayer (
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

protected:

    virtual void HandleFrame ();

private:

    typedef std::set<Entity *> EntitySet;

    void UpdateVelocities ();
    void UpdatePositions ();
    void HandleInterpenetrations ();

    // set of all added entities
    EntitySet m_entity_set;

    // the list of collision pairs for this frame
    CollisionPairList m_collision_pair_list;

    // keeps track of the main object layer (really only used to make
    // sure that all entities are added to the main object layer)
    Engine2::ObjectLayer *m_main_object_layer;
    // the quadtree used in collision detection and other spatial shit
    CollisionQuadTree *m_quad_tree;
}; // end of class PhysicsHandler

} // end of namespace Dis

#endif // !defined(_DIS_PHYSICSHANDLER_HPP_)
