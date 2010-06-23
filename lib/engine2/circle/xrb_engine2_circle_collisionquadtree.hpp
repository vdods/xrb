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

namespace Xrb {
namespace Engine2 {
namespace Circle {

class CollisionQuadTree : public QuadTree
{
public:

    CollisionQuadTree (
        FloatVector2 const &center,
        Float half_side_length,
        Uint8 depth);
    virtual ~CollisionQuadTree () { }

    static CollisionQuadTree *Create (Float half_side_length, Uint8 depth);

    bool DoesAreaOverlapAnyEntity (
        FloatVector2 const &area_center,
        Float area_radius,
        bool check_nonsolid_collision_entities,
        bool is_wrapped = false,
        Float object_layer_side_length = -1.0f, // irrelevant for non-wrapped space
        Float half_object_layer_side_length = -1.0f) const;

    void LineTrace (
        FloatVector2 const &trace_start,
        FloatVector2 const &trace_vector,
        Float trace_radius,
        bool check_nonsolid_collision_entities,
        LineTraceBindingSet *line_trace_binding_set,
        bool is_wrapped = false,
        Float object_layer_side_length = -1.0f, // irrelevant for non-wrapped space
        Float half_object_layer_side_length = -1.0f);

    void AreaTrace (
        FloatVector2 const &trace_area_center,
        Float trace_area_radius,
        bool check_nonsolid_collision_entities,
        AreaTraceList *area_trace_list,
        bool is_wrapped = false,
        Float object_layer_side_length = -1.0f, // irrelevant for non-wrapped space
        Float half_object_layer_side_length = -1.0f);

    void CollideEntity (
        Entity *entity,
        Float frame_dt,
        CollisionPairList *collision_pair_list,
        bool is_wrapped = false,
        Float object_layer_side_length = -1.0f); // irrelevant for non-wrapped space

protected:

    CollisionQuadTree (CollisionQuadTree *parent) : QuadTree(parent) { }

private:

    class CollideEntityLoopFunctor;

    void CollideEntity (CollideEntityLoopFunctor &functor);

    // instead of passing all these parameters into each call of a quad
    // tree recursion, package them up into a nice functor object.
    class CollideEntityLoopFunctor
    {
    public:

        CollideEntityLoopFunctor (
            Entity *entity,
            Float frame_dt,
            CollisionPairList *collision_pair_list,
            bool is_wrapped,
            Float object_layer_side_length) // irrelevant for non-wrapped space
            :
            m_entity(entity),
            m_frame_dt(frame_dt),
            m_collision_pair_list(collision_pair_list),
            m_is_wrapped(is_wrapped),
            m_object_layer_side_length(object_layer_side_length),
            m_half_object_layer_side_length(0.5f*object_layer_side_length)
        {
            ASSERT1(m_entity != NULL);
            ASSERT1(m_collision_pair_list != NULL);
            ASSERT1(entity->GetCollisionType() != CT_NO_COLLISION);
            if (m_is_wrapped)
                ASSERT1(m_object_layer_side_length > 0.0f);
        }

        // this is the business end of the functor, and is what actually
        // does the collision detection.
        void operator () (Object *object);

        Entity *GetEntity () const { return m_entity; }
        Float ObjectLayerSideLength () const { return m_object_layer_side_length; }
        Float HalfObjectLayerSideLength () const { return m_half_object_layer_side_length; }
        bool IsWrapped () const { return m_is_wrapped; }

    private:

        Entity *m_entity;
        Float m_frame_dt;
        CollisionPairList *m_collision_pair_list;
        bool m_is_wrapped;
        Float m_object_layer_side_length;
        Float m_half_object_layer_side_length;
    }; // end of class CollisionQuadTree::CollideEntityLoopFunctor
}; // end of class CollisionQuadTree

} // end of namespace Circle
} // end of namespace Engine2
} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_CIRCLE_COLLISIONQUADTREE_HPP_)
