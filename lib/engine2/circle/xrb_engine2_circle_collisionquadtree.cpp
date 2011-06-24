// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_circle_collisionquadtree.cpp by Victor Dods, created 2005/11/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_circle_collisionquadtree.hpp"

#include <algorithm>

#include "xrb_engine2_circle_entity.hpp"
#include "xrb_engine2_circle_physicshandler.hpp"
#include "xrb_engine2_objectlayer.hpp"

namespace Xrb {
namespace Engine2 {
namespace Circle {

CollisionQuadTree::CollisionQuadTree (
    FloatVector2 const &center,
    Float half_side_length,
    Uint8 depth)
    :
    QuadTree(NULL)
{
    Initialize<CollisionQuadTree>(center, half_side_length, depth);
    SetQuadTreeType(QTT_PHYSICS_HANDLER);
}

CollisionQuadTree *CollisionQuadTree::Create (Float half_side_length, Uint8 depth)
{
    return new CollisionQuadTree(FloatVector2::ms_zero, half_side_length, depth);
}

bool CollisionQuadTree::DoesAreaOverlapAnyEntity (
    FloatVector2 const &area_center,
    Float area_radius,
    bool check_nonsolid_collision_entities,
    ObjectLayer const &object_layer) const
{
    // if there are no objects here or below, just return false
    if (SubordinateObjectCount() == 0)
        return false;

    // return false if the area doesn't intersect this node
    if (!DoesAreaOverlapQuadBounds(area_center, area_radius, object_layer, false))
        return false;

    // check if the area overlaps any object in this node's list.
    for (ObjectSet::const_iterator it = m_object_set.begin(),
                                   it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Object const *object = *it;
        ASSERT1(object != NULL);
        ASSERT1(object->OwnerQuadTree(QTT_PHYSICS_HANDLER) == this);

        if (object_layer.AdjustedDistance(object->Translation(), area_center)
            <
            (object->Radius(GetQuadTreeType()) + area_radius))
        {
            Entity const *entity = DStaticCast<Entity const *>(object->GetEntity());
            ASSERT1(entity != NULL);
            if (entity->GetCollisionType() == Engine2::Circle::CT_SOLID_COLLISION)
                return true;
            else if (check_nonsolid_collision_entities &&
                     entity->GetCollisionType() == Engine2::Circle::CT_NONSOLID_COLLISION)
                return true;
        }
    }

    // if there are child nodes, call this method on each until one returns true
    if (HasChildren())
    {
        bool retval = false;
        for (Uint32 i = 0; i < 4; ++i)
            retval = retval ||
                Child<CollisionQuadTree>(i)->DoesAreaOverlapAnyEntity(
                    area_center,
                    area_radius,
                    check_nonsolid_collision_entities,
                    object_layer);
        return retval;
    }
    else
        return false;
}

void CollisionQuadTree::LineTrace (
    FloatVector2 const &trace_start,
    FloatVector2 const &trace_vector,
    Float trace_radius,
    bool check_nonsolid_collision_entities,
    LineTraceBindingSet &line_trace_binding_set,
    ObjectLayer const &object_layer) const
{
    ASSERT1(!trace_vector.IsZero());
    ASSERT1(trace_radius >= 0.0f);

    // if this quad node has no subordinates, return
    if (SubordinateObjectCount() == 0)
        return;

    // if this quad node doesn't intersect the line, return
    Float a = trace_vector | trace_vector;
    FloatVector2 trace_center(trace_start + 0.5f * trace_vector);
    {
        FloatVector2 p_minus_c = object_layer.AdjustedDifference(trace_start, Center());
        Float R = 2.0f * Radius() + trace_radius;
        Float b = p_minus_c | trace_vector;
        Float c = (p_minus_c | p_minus_c) - R * R;
        Float determinant = b * b - a * c;
        if (determinant < 0.0f)
            return;
    }

    // check the line against the objects in this node
    for (ObjectSet::iterator it = m_object_set.begin(),
                             it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        ASSERT2(object->OwnerQuadTree(QTT_PHYSICS_HANDLER) == this);

        Entity *entity = DStaticCast<Entity *>(object->GetEntity());
        ASSERT1(entity != NULL);

        // don't check nonsolid collision entities if
        // check_nonsolid_collision_entities isn't set.
        ASSERT1(entity->GetCollisionType() != Engine2::Circle::CT_NO_COLLISION);
        if (entity->GetCollisionType() == Engine2::Circle::CT_NONSOLID_COLLISION &&
            !check_nonsolid_collision_entities)
            continue;

        FloatVector2 p_minus_c = object_layer.AdjustedDifference(trace_start, entity->Translation());
        Float R = entity->Radius(GetQuadTreeType()) + trace_radius;
        // a is calculated above
        Float b = p_minus_c | trace_vector;
        Float c = (p_minus_c | p_minus_c) - R * R;
        Float determinant = b * b - a * c;
        if (determinant < 0.0f)
            continue;

        Float radical_part = Math::Sqrt(determinant);

        Float t0 = (-b - radical_part) / a;
        if (t0 > 1.0f)
            continue;

        Float t1 = (-b + radical_part) / a;
        if (t1 < 0.0f)
            continue;

        line_trace_binding_set.insert(LineTraceBinding(Max(0.0f, t0), entity));
    }

    // call this function on the child nodes, if they exist
    if (HasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            Child<CollisionQuadTree>(i)->LineTrace(
                trace_start,
                trace_vector,
                trace_radius,
                check_nonsolid_collision_entities,
                line_trace_binding_set,
                object_layer);
}

void CollisionQuadTree::AreaTrace (
    FloatVector2 const &trace_area_center,
    Float trace_area_radius,
    bool check_nonsolid_collision_entities,
    AreaTraceList &area_trace_list,
    ObjectLayer const &object_layer) const
{
    ASSERT1(trace_area_radius > 0.0f);

    // if this quad node has no subordinates, return
    if (SubordinateObjectCount() == 0)
        return;

    // if this quad node doesn't intersect the area, return
    if (!DoesAreaOverlapQuadBounds(trace_area_center, trace_area_radius, object_layer, false))
        return;

    // call this function on the child nodes, if they exist
    if (HasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            Child<CollisionQuadTree>(i)->AreaTrace(
                trace_area_center,
                trace_area_radius,
                check_nonsolid_collision_entities,
                area_trace_list,
                object_layer);

    // check the line against the objects in this node
    for (ObjectSet::iterator it = m_object_set.begin(),
                             it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        ASSERT2(object->OwnerQuadTree(QTT_PHYSICS_HANDLER) == this);

        Entity *entity = DStaticCast<Entity *>(object->GetEntity());
        ASSERT1(entity != NULL);

        // don't check nonsolid collision entities if
        // check_nonsolid_collision_entities isn't set.
        ASSERT1(entity->GetCollisionType() != Engine2::Circle::CT_NO_COLLISION);
        if (entity->GetCollisionType() == Engine2::Circle::CT_NONSOLID_COLLISION &&
            !check_nonsolid_collision_entities)
            continue;

        FloatVector2 center_to_center(object_layer.AdjustedDifference(entity->Translation(), trace_area_center));
        if (center_to_center.Length() >= entity->Radius(GetQuadTreeType()) + trace_area_radius)
            continue;

        area_trace_list.push_back(entity);
    }
}

void CollisionQuadTree::CollideEntity (
    Entity *const entity,
    Float frame_dt,
    CollisionPairList &collision_pair_list) const
{
    ASSERT1(entity != NULL);
    ASSERT1(entity->GetCollisionType() != Engine2::Circle::CT_NO_COLLISION);

    CollideEntityLoopFunctor functor(entity, frame_dt, collision_pair_list);
    CollideEntity(functor);
}

void CollisionQuadTree::CollideEntity (CollisionQuadTree::CollideEntityLoopFunctor &functor) const
{
    // if there are no objects here or below, just return
    if (SubordinateObjectCount() == 0)
        return;

    // return if the area doesn't intersect this node
    if (!DoesAreaOverlapQuadBounds(functor.GetEntity()->Translation(), functor.GetEntity()->Radius(GetQuadTreeType()), functor.GetObjectLayer(), false))
        return;

    // here is the actual entity loop
    std::for_each(m_object_set.begin(), m_object_set.end(), functor);

    // if there are child nodes, call CollideEntity on each
    if (HasChildren())
    {
        // NOTE: CollideEntity only takes an entity as far down the quad
        // tree as its size allows (i.e. if the entity would be placed at
        // this level in the quad tree, CollideEntity does not traverse
        // any further).  this is so a lot of redundant collision checks are
        // avoided.
        if (IsAllowableSizedObject(*functor.GetEntity()->OwnerObject()))
            return;

        for (Uint8 i = 0; i < 4; ++i)
            Child<CollisionQuadTree>(i)->CollideEntity(functor);
    }
}

void CollisionQuadTree::CollideEntityLoopFunctor::operator () (Object *object)
{
    // because it is possible to have CollideEntity call this functor twice on the
    // same UN-ordered pair (i.e. call it on (x,y) and also (y,x)), we must avoid
    // calculating the collision twice.  the heuristic will be size and then pointer
    // value -- size is used because that's the discriminant in CollideEntity.
    // the pointer value comparison will prevent colliding an entity with itself.
    // the size comparison must necessarily be the following, due to the nature of
    // CollideEntity.
    if (m_entity->Radius(QTT_PHYSICS_HANDLER) > object->Radius(QTT_PHYSICS_HANDLER)
        ||
        (m_entity->Radius(QTT_PHYSICS_HANDLER) == object->Radius(QTT_PHYSICS_HANDLER)
         && m_entity->OwnerObject() >= object)) // yes, this is a pointer comparison.
    {
        return;
    }

    FloatVector2 ce0_translation(m_entity->Translation());
    FloatVector2 ce1_translation(object->Translation());
    Float r = m_entity->Radius(QTT_PHYSICS_HANDLER) + object->Radius(QTT_PHYSICS_HANDLER);
    FloatVector2 offset_0_to_1(m_object_layer.AdjustedDifference(ce1_translation, ce0_translation));

    if (offset_0_to_1.LengthSquared() >= Sqr(r))
        return;

    // at this point, a collision has happened (the entities are overlapping).

    Entity *other_entity = DStaticCast<Entity *>(object->GetEntity());
    ASSERT1(other_entity != NULL);
    ASSERT1(m_entity->GetPhysicsHandler() != NULL);
    ASSERT1(m_entity->GetPhysicsHandler() == other_entity->GetPhysicsHandler());
    PhysicsHandler &physics_handler = *m_entity->GetPhysicsHandler();

    FloatVector2 collision_location(
        (other_entity->ScaleFactor() * ce0_translation + m_entity->ScaleFactor() * ce1_translation)
        /
        (m_entity->ScaleFactor() + other_entity->ScaleFactor()));
    FloatVector2 collision_normal_0_to_1;
    if (offset_0_to_1.IsZero())
        collision_normal_0_to_1 = FloatVector2(1.0f, 0.0f);
    else
        collision_normal_0_to_1 = offset_0_to_1.Normalization();
    Float collision_force = 0.0f; // to be determined by PhysicsHandler::CollisionResponse

    // check if we should proceed with physical collision response
    if (m_entity->GetCollisionType() == Engine2::Circle::CT_SOLID_COLLISION && // if they're both solid
        other_entity->GetCollisionType() == Engine2::Circle::CT_SOLID_COLLISION &&
        !physics_handler.CollisionExemption(*m_entity, *other_entity)) // and if this isn't an exception
    {
        collision_force = physics_handler.CollisionResponse(*m_entity, *other_entity, offset_0_to_1, m_frame_dt, collision_location, collision_normal_0_to_1);
    }

    // record the collision in the collision pair list.
    m_collision_pair_list.push_back(
        CollisionPair(
            m_entity,
            other_entity,
            collision_location,
            collision_normal_0_to_1,
            collision_force));
}

} // end of namespace Circle
} // end of namespace Engine2
} // end of namespace Xrb
