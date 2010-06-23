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
    bool is_wrapped,
    Float object_layer_side_length,
    Float half_object_layer_side_length) const
{
    if (is_wrapped)
    {
        ASSERT1(object_layer_side_length > 0.0f);
        ASSERT1(half_object_layer_side_length > 0.0f);
    }

    // if there are no objects here or below, just return false
    if (SubordinateObjectCount() == 0)
        return false;

    // return false if the area doesn't intersect this node
    if (!DoesAreaOverlapQuadBounds(
            area_center,
            area_radius,
            is_wrapped,
            object_layer_side_length,
            half_object_layer_side_length))
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

        FloatVector2 object_translation(object->Translation());
        FloatVector2 adjusted_area_center(area_center);

        // only need to adjust coordinates in wrapped space
        if (is_wrapped)
        {
            if (adjusted_area_center[Dim::X] < object_translation[Dim::X] - half_object_layer_side_length)
                adjusted_area_center[Dim::X] += object_layer_side_length;
            else if (adjusted_area_center[Dim::X] > object_translation[Dim::X] + half_object_layer_side_length)
                adjusted_area_center[Dim::X] -= object_layer_side_length;

            if (adjusted_area_center[Dim::Y] < object_translation[Dim::Y] - half_object_layer_side_length)
                adjusted_area_center[Dim::Y] += object_layer_side_length;
            else if (adjusted_area_center[Dim::Y] > object_translation[Dim::Y] + half_object_layer_side_length)
                adjusted_area_center[Dim::Y] -= object_layer_side_length;
        }

        if ((object_translation - adjusted_area_center).Length()
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
                    is_wrapped,
                    object_layer_side_length,
                    half_object_layer_side_length);
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
    LineTraceBindingSet *line_trace_binding_set,
    bool is_wrapped,
    Float object_layer_side_length,
    Float half_object_layer_side_length)
{
    ASSERT1(!trace_vector.IsZero());
    ASSERT1(trace_radius >= 0.0f);
    ASSERT1(line_trace_binding_set != NULL);

    if (is_wrapped)
    {
        ASSERT1(object_layer_side_length > 0.0f);
        ASSERT1(half_object_layer_side_length > 0.0f);
    }

    // if this quad node has no subordinates, return
    if (SubordinateObjectCount() == 0)
        return;

    // if this quad node doesn't intersect the line, return
    Float a = trace_vector | trace_vector;
    FloatVector2 trace_center(trace_start + 0.5f * trace_vector);
    {
        FloatVector2 adjusted_center(Center());

        // only need to adjust coordinates in wrapped space
        if (is_wrapped)
        {
            if (adjusted_center[Dim::X] - trace_center[Dim::X] > half_object_layer_side_length)
                adjusted_center[Dim::X] -= object_layer_side_length;
            else if (adjusted_center[Dim::X] - trace_center[Dim::X] < -half_object_layer_side_length)
                adjusted_center[Dim::X] += object_layer_side_length;

            if (adjusted_center[Dim::Y] - trace_center[Dim::Y] > half_object_layer_side_length)
                adjusted_center[Dim::Y] -= object_layer_side_length;
            else if (adjusted_center[Dim::Y] - trace_center[Dim::Y] < -half_object_layer_side_length)
                adjusted_center[Dim::Y] += object_layer_side_length;
        }

        FloatVector2 p_minus_c = trace_start - adjusted_center;
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

        FloatVector2 adjusted_entity_translation(entity->Translation());

        // only need to adjust coordinates in wrapped space
        if (is_wrapped)
        {
            if (adjusted_entity_translation[Dim::X] - trace_center[Dim::X] > half_object_layer_side_length)
                adjusted_entity_translation[Dim::X] -= object_layer_side_length;
            else if (adjusted_entity_translation[Dim::X] - trace_center[Dim::X] < -half_object_layer_side_length)
                adjusted_entity_translation[Dim::X] += object_layer_side_length;

            if (adjusted_entity_translation[Dim::Y] - trace_center[Dim::Y] > half_object_layer_side_length)
                adjusted_entity_translation[Dim::Y] -= object_layer_side_length;
            else if (adjusted_entity_translation[Dim::Y] - trace_center[Dim::Y] < -half_object_layer_side_length)
                adjusted_entity_translation[Dim::Y] += object_layer_side_length;
        }

        // check the trace line against the object
        FloatVector2 p_minus_c = trace_start - adjusted_entity_translation;
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

        line_trace_binding_set->insert(LineTraceBinding(Max(0.0f, t0), entity));
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
                is_wrapped,
                object_layer_side_length,
                half_object_layer_side_length);
}

void CollisionQuadTree::AreaTrace (
    FloatVector2 const &trace_area_center,
    Float trace_area_radius,
    bool check_nonsolid_collision_entities,
    AreaTraceList *area_trace_list,
    bool is_wrapped,
    Float object_layer_side_length,
    Float half_object_layer_side_length)
{
    ASSERT1(trace_area_radius > 0.0f);
    ASSERT1(area_trace_list != NULL);

    if (is_wrapped)
    {
        ASSERT1(object_layer_side_length > 0.0f);
        ASSERT1(half_object_layer_side_length > 0.0f);
    }

    // if this quad node has no subordinates, return
    if (SubordinateObjectCount() == 0)
        return;

    // if this quad node doesn't intersect the line, return
    if (!DoesAreaOverlapQuadBounds(
            trace_area_center,
            trace_area_radius,
            is_wrapped,
            object_layer_side_length,
            half_object_layer_side_length))
        return;

    // call this function on the child nodes, if they exist
    if (HasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            Child<CollisionQuadTree>(i)->AreaTrace(
                trace_area_center,
                trace_area_radius,
                check_nonsolid_collision_entities,
                area_trace_list,
                is_wrapped,
                object_layer_side_length,
                half_object_layer_side_length);

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

        FloatVector2 adjusted_entity_translation(entity->Translation());

        // only need to adjust coordinates in wrapped space
        if (is_wrapped)
        {
            if (adjusted_entity_translation[Dim::X] - trace_area_center[Dim::X] > half_object_layer_side_length)
                adjusted_entity_translation[Dim::X] -= object_layer_side_length;
            else if (adjusted_entity_translation[Dim::X] - trace_area_center[Dim::X] < -half_object_layer_side_length)
                adjusted_entity_translation[Dim::X] += object_layer_side_length;

            if (adjusted_entity_translation[Dim::Y] - trace_area_center[Dim::Y] > half_object_layer_side_length)
                adjusted_entity_translation[Dim::Y] -= object_layer_side_length;
            else if (adjusted_entity_translation[Dim::Y] - trace_area_center[Dim::Y] < -half_object_layer_side_length)
                adjusted_entity_translation[Dim::Y] += object_layer_side_length;
        }

        // don't add it if the entity isn't touching the trace area
        FloatVector2 center_to_center(adjusted_entity_translation - trace_area_center);
        if (center_to_center.Length() >= entity->Radius(GetQuadTreeType()) + trace_area_radius)
            continue;

        area_trace_list->push_back(entity);
    }
}

void CollisionQuadTree::CollideEntity (
    Entity *const entity,
    Float frame_dt,
    CollisionPairList *collision_pair_list,
    bool is_wrapped,
    Float object_layer_side_length)
{
    ASSERT1(entity != NULL);
    ASSERT1(entity->GetCollisionType() != Engine2::Circle::CT_NO_COLLISION);
    ASSERT1(collision_pair_list != NULL);

    CollideEntityLoopFunctor
        functor(
            entity,
            frame_dt,
            collision_pair_list,
            is_wrapped,
            object_layer_side_length);
    CollideEntity(functor);
}

void CollisionQuadTree::CollideEntity (CollisionQuadTree::CollideEntityLoopFunctor &functor)
{
    // if there are no objects here or below, just return
    if (SubordinateObjectCount() == 0)
        return;

    // return if the area doesn't intersect this node
    if (!DoesAreaOverlapQuadBounds(
            functor.GetEntity()->Translation(),
            functor.GetEntity()->Radius(GetQuadTreeType()),
            functor.IsWrapped(),
            functor.ObjectLayerSideLength(),
            functor.HalfObjectLayerSideLength()))
        return;

    // if there are child nodes, call CollideEntity on each
    if (HasChildren())
    {
        for (Uint8 i = 0; i < 4; ++i)
            Child<CollisionQuadTree>(i)->CollideEntity(functor);

        // if the minimum object size for this node is larger than the
        // collision entity, return (because it will skip all objects
        // below in the loop anyway)
        if (!IsAllowableObjectRadius(functor.GetEntity()->OwnerObject()))
            return;
    }

    // here is the actual entity loop
    std::for_each(m_object_set.begin(), m_object_set.end(), functor);
}

void CollisionQuadTree::CollideEntityLoopFunctor::operator () (Object *object)
{
    // don't collide the entity with itself
    if (object == m_entity->OwnerObject())
        return;

    // this is a quick and easy way to avoid calculating
    // the same collision pair twice
    if (object->Radius(QTT_PHYSICS_HANDLER) > m_entity->Radius(QTT_PHYSICS_HANDLER)
        ||
        (object->Radius(QTT_PHYSICS_HANDLER) == m_entity->Radius(QTT_PHYSICS_HANDLER) &&
         object > m_entity->OwnerObject())) // yes, this is pointer comparison
        return;

    FloatVector2 ce0_translation(m_entity->Translation());
    FloatVector2 ce1_translation(object->Translation());

    // only need to adjust coordinates if we're dealing with wrapped space
    if (m_is_wrapped)
    {
        if (ce1_translation[Dim::X] - ce0_translation[Dim::X] > m_half_object_layer_side_length)
            ce1_translation[Dim::X] -= m_object_layer_side_length;
        else if (ce1_translation[Dim::X] - ce0_translation[Dim::X] < -m_half_object_layer_side_length)
            ce1_translation[Dim::X] += m_object_layer_side_length;

        if (ce1_translation[Dim::Y] - ce0_translation[Dim::Y] > m_half_object_layer_side_length)
            ce1_translation[Dim::Y] -= m_object_layer_side_length;
        else if (ce1_translation[Dim::Y] - ce0_translation[Dim::Y] < -m_half_object_layer_side_length)
            ce1_translation[Dim::Y] += m_object_layer_side_length;
    }

    Float r = m_entity->Radius(QTT_PHYSICS_HANDLER) + object->Radius(QTT_PHYSICS_HANDLER);
    FloatVector2 P = ce0_translation - ce1_translation;

    if (P.Length() >= r)
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
    FloatVector2 collision_normal;
    if (P.IsZero())
        collision_normal = FloatVector2(1.0f, 0.0f);
    else
        collision_normal = P.Normalization();
    Float collision_force = 0.0f; // to be determined by PhysicsHandler::CollisionResponse

    // check if we should proceed with physical collision response
    if (m_entity->GetCollisionType() == Engine2::Circle::CT_SOLID_COLLISION && // if they're both solid
        other_entity->GetCollisionType() == Engine2::Circle::CT_SOLID_COLLISION &&
        !physics_handler.CollisionExemption(*m_entity, *other_entity)) // and if this isn't an exception
    {
        collision_force = physics_handler.CollisionResponse(*m_entity, *other_entity, P, m_frame_dt, collision_location, collision_normal);
    }

    // record the collision in the collision pair list.
    m_collision_pair_list->push_back(
        CollisionPair(
            m_entity,
            other_entity,
            collision_location,
            collision_normal,
            collision_force));
}

} // end of namespace Circle
} // end of namespace Engine2
} // end of namespace Xrb
