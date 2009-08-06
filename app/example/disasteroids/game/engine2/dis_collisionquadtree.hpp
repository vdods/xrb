// ///////////////////////////////////////////////////////////////////////////
// dis_collisionquadtree.hpp by Victor Dods, created 2005/11/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_COLLISIONQUADTREE_HPP_)
#define _DIS_COLLISIONQUADTREE_HPP_

#include "xrb_engine2_quadtree.hpp"

#include "dis_areatracelist.hpp"
#include "dis_collisionpair.hpp"
#include "dis_entity.hpp"
#include "dis_linetracebinding.hpp"

using namespace Xrb;

namespace Dis
{

class CollisionQuadTree : public Engine2::QuadTree
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
        bool check_nonsolid_collision_entities) const;
    bool DoesAreaOverlapAnyEntityWrapped (
        FloatVector2 const &area_center,
        Float area_radius,
        bool check_nonsolid_collision_entities,
        Float object_layer_side_length,
        Float half_object_layer_side_length) const;

    void LineTrace (
        FloatVector2 const &trace_start,
        FloatVector2 const &trace_vector,
        Float trace_radius,
        bool check_nonsolid_collision_entities,
        LineTraceBindingSet *line_trace_binding_set);
    void LineTraceWrapped (
        FloatVector2 const &trace_start,
        FloatVector2 const &trace_vector,
        Float trace_radius,
        bool check_nonsolid_collision_entities,
        LineTraceBindingSet *line_trace_binding_set,
        Float object_layer_side_length,
        Float half_object_layer_side_length);

    void AreaTrace (
        FloatVector2 const &trace_area_center,
        Float trace_area_radius,
        bool check_nonsolid_collision_entities,
        AreaTraceList *area_trace_list);
    void AreaTraceWrapped (
        FloatVector2 const &trace_area_center,
        Float trace_area_radius,
        bool check_nonsolid_collision_entities,
        AreaTraceList *area_trace_list,
        Float object_layer_side_length,
        Float half_object_layer_side_length);

    void CollideEntity (
        Entity *entity,
        Float frame_dt,
        CollisionPairList *collision_pair_list);
    void CollideEntityWrapped (
        Entity *entity,
        Float frame_dt,
        CollisionPairList *collision_pair_list,
        Float object_layer_side_length);

protected:

    CollisionQuadTree (CollisionQuadTree *parent)
        :
        Engine2::QuadTree(parent)
    {
    }

private:

    class CollideEntityWrappedLoopFunctor
    {
    public:

        CollideEntityWrappedLoopFunctor (
            Entity *entity,
            Float frame_dt,
            CollisionPairList *collision_pair_list,
            Float object_layer_side_length,
            Engine2::QuadTreeType quad_tree_type)
            :
            m_entity(entity),
            m_frame_dt(frame_dt),
            m_frame_dt_squared(frame_dt*frame_dt),
            m_collision_pair_list(collision_pair_list),
            m_object_layer_side_length(object_layer_side_length),
            m_half_object_layer_side_length(0.5f*object_layer_side_length),
            m_quad_tree_type(quad_tree_type)
        {
            ASSERT1(m_entity != NULL);
            ASSERT1(m_collision_pair_list != NULL);
            ASSERT1(entity->GetCollisionType() != CT_NO_COLLISION);
        }

        void operator () (Engine2::Object *object);

        inline Entity *GetEntity () { return m_entity; }
        inline Float ObjectLayerSideLength () { return m_object_layer_side_length; }
        inline Float HalfObjectLayerSideLength () { return m_half_object_layer_side_length; }
                
    private:
    
        Entity *m_entity;
        Float m_frame_dt;
        Float m_frame_dt_squared;
        CollisionPairList *m_collision_pair_list;
        Float m_object_layer_side_length;
        Float m_half_object_layer_side_length;
        Engine2::QuadTreeType m_quad_tree_type;
    }; // end of class CollideEntityWrappedLoopFunctor

    void CollideEntityWrapped (CollideEntityWrappedLoopFunctor &functor);
}; // end of class CollisionQuadTree

} // end of namespace Dis

#endif // !defined(_DIS_COLLISIONQUADTREE_HPP_)
