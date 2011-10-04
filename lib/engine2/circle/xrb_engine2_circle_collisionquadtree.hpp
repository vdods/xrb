// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_circle_collisionquadtree.hpp by Victor Dods, created 2005/11/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_CIRCLE_COLLISIONQUADTREE_HPP_)
#define _XRB_ENGINE2_CIRCLE_COLLISIONQUADTREE_HPP_

#include "xrb.hpp"

#include "xrb_engine2_circle_entity.hpp"
#include "xrb_engine2_circle_types.hpp"
#include "xrb_engine2_quadtree.hpp"
#include "xrb_time.hpp"

namespace Xrb {
namespace Engine2 {
namespace Circle {

class CollisionQuadTree : public QuadTree
{
public:

    CollisionQuadTree (FloatVector2 const &center, Float half_side_length, Uint8 depth);
    virtual ~CollisionQuadTree () { }

    static CollisionQuadTree *Create (Float half_side_length, Uint8 depth);

    bool DoesAreaOverlapAnyEntity (
        FloatVector2 const &area_center,
        Float area_radius,
        bool check_nonsolid_collision_entities,
        ObjectLayer const &object_layer) const;

    void LineTrace (
        FloatVector2 const &trace_start,
        FloatVector2 const &trace_vector,
        Float trace_radius,
        bool check_nonsolid_collision_entities,
        LineTraceBindingSet &line_trace_binding_set,
        ObjectLayer const &object_layer) const;

    // will never add a NULL entity to an AreaTraceList
    void AreaTrace (
        FloatVector2 const &trace_area_center,
        Float trace_area_radius,
        bool check_nonsolid_collision_entities,
        AreaTraceList &area_trace_list,
        ObjectLayer const &object_layer) const;

    void CollideEntity (Entity &entity, Time::Delta frame_dt, CollisionPairList &collision_pair_list) const;

protected:

    CollisionQuadTree (CollisionQuadTree *parent) : QuadTree(parent) { }

private:

    class CollideEntityLoopFunctor;

    void CollideEntity (CollideEntityLoopFunctor &functor) const;

    // instead of passing all these parameters into each call of a quad
    // tree recursion, package them up into a nice functor object.
    class CollideEntityLoopFunctor
    {
    public:

        CollideEntityLoopFunctor (Entity &entity, Time::Delta frame_dt, CollisionPairList &collision_pair_list)
            :
            m_entity(entity),
            m_frame_dt(frame_dt),
            m_collision_pair_list(collision_pair_list),
            m_object_layer(*entity.GetObjectLayer())
        {
            ASSERT1(m_entity.OwnerObject() != NULL);
            ASSERT1(m_entity.GetObjectLayer() != NULL);
            ASSERT1(m_entity.GetCollisionType() != CT_NO_COLLISION);
        }

        // this is the business end of the functor, and is what actually
        // does the collision detection.
        void operator () (Object *object);

        Entity &GetEntity () const { return m_entity; }
        ObjectLayer const &GetObjectLayer () const { return m_object_layer; }

    private:

        Entity &m_entity;
        Time::Delta m_frame_dt;
        CollisionPairList &m_collision_pair_list;
        ObjectLayer const &m_object_layer;
    }; // end of class CollisionQuadTree::CollideEntityLoopFunctor
}; // end of class CollisionQuadTree

} // end of namespace Circle
} // end of namespace Engine2
} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_CIRCLE_COLLISIONQUADTREE_HPP_)
