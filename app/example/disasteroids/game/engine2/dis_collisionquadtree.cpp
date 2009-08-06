// ///////////////////////////////////////////////////////////////////////////
// dis_collisionquadtree.cpp by Victor Dods, created 2005/11/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_collisionquadtree.hpp"

#include <algorithm>

#include "dis_entity.hpp"

using namespace Xrb;

namespace Dis
{

CollisionQuadTree::CollisionQuadTree (
    FloatVector2 const &center,
    Float const half_side_length,
    Uint8 const depth)
    :
    Engine2::QuadTree(NULL)
{
    Initialize<CollisionQuadTree>(center, half_side_length, depth);
    SetQuadTreeType(Engine2::QTT_PHYSICS_HANDLER);
}

CollisionQuadTree *CollisionQuadTree::Create (
    Float const half_side_length,
    Uint8 const depth)
{
    return new CollisionQuadTree(FloatVector2::ms_zero, half_side_length, depth);
}

bool CollisionQuadTree::DoesAreaOverlapAnyEntity (
    FloatVector2 const &area_center,
    Float const area_radius,
    bool const check_nonsolid_collision_entities) const
{
    // if there are no objects here or below, just return false
    if (GetSubordinateObjectCount() == 0)
        return false;

    // return false if the area doesn't intersect this node
    if (!DoesAreaOverlapQuadBounds(area_center, area_radius))
        return false;

    // check if the area overlaps any object in this node's list.
    for (ObjectSetConstIterator it = m_object_set.begin(),
                                it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Engine2::Object const *object = *it;
        ASSERT1(object != NULL);
        ASSERT1(object->GetOwnerQuadTree(Engine2::QTT_PHYSICS_HANDLER) == this);
        if ((object->GetTranslation() - area_center).GetLength()
            <
            (object->GetRadius(GetQuadTreeType()) + area_radius))
        {
            Entity const *entity = DStaticCast<Entity const *>(object->GetEntity());
            ASSERT1(entity != NULL);
            if (entity->GetCollisionType() == CT_SOLID_COLLISION)
                return true;
            else if (check_nonsolid_collision_entities &&
                     entity->GetCollisionType() == CT_NONSOLID_COLLISION)
                return true;
        }
    }

    // if there are child nodes, call this method on each until one returns true
    if (HasChildren())
        return
            Child<CollisionQuadTree>(0)->DoesAreaOverlapAnyEntity(
                area_center,
                area_radius,
                check_nonsolid_collision_entities)
            ||
            Child<CollisionQuadTree>(1)->DoesAreaOverlapAnyEntity(
                area_center,
                area_radius,
                check_nonsolid_collision_entities)
            ||
            Child<CollisionQuadTree>(2)->DoesAreaOverlapAnyEntity(
                area_center,
                area_radius,
                check_nonsolid_collision_entities)
            ||
            Child<CollisionQuadTree>(3)->DoesAreaOverlapAnyEntity(
                area_center,
                area_radius,
                check_nonsolid_collision_entities);
    else
        return false;
}

bool CollisionQuadTree::DoesAreaOverlapAnyEntityWrapped (
    FloatVector2 const &area_center,
    Float const area_radius,
    bool const check_nonsolid_collision_entities,
    Float const object_layer_side_length,
    Float const half_object_layer_side_length) const
{
    // if there are no objects here or below, just return false
    if (GetSubordinateObjectCount() == 0)
        return false;

    // return false if the area doesn't intersect this node
    if (!DoesAreaOverlapQuadBoundsWrapped(
            area_center,
            area_radius,
            object_layer_side_length,
            half_object_layer_side_length))
        return false;

    // check if the area overlaps any object in this node's list.
    for (ObjectSetConstIterator it = m_object_set.begin(),
                                it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Engine2::Object const *object = *it;
        ASSERT1(object != NULL);
        ASSERT1(object->GetOwnerQuadTree(Engine2::QTT_PHYSICS_HANDLER) == this);

        FloatVector2 object_translation(object->GetTranslation());
        FloatVector2 adjusted_area_center(area_center);

        if (adjusted_area_center[Dim::X] < object_translation[Dim::X] - half_object_layer_side_length)
            adjusted_area_center[Dim::X] += object_layer_side_length;
        else if (adjusted_area_center[Dim::X] > object_translation[Dim::X] + half_object_layer_side_length)
            adjusted_area_center[Dim::X] -= object_layer_side_length;

        if (adjusted_area_center[Dim::Y] < object_translation[Dim::Y] - half_object_layer_side_length)
            adjusted_area_center[Dim::Y] += object_layer_side_length;
        else if (adjusted_area_center[Dim::Y] > object_translation[Dim::Y] + half_object_layer_side_length)
            adjusted_area_center[Dim::Y] -= object_layer_side_length;

        if ((object_translation - adjusted_area_center).GetLength()
            <
            (object->GetRadius(GetQuadTreeType()) + area_radius))
        {
            Entity const *entity = DStaticCast<Entity const *>(object->GetEntity());
            ASSERT1(entity != NULL);
            if (entity->GetCollisionType() == CT_SOLID_COLLISION)
                return true;
            else if (check_nonsolid_collision_entities &&
                     entity->GetCollisionType() == CT_NONSOLID_COLLISION)
                return true;
        }
    }

    // if there are child nodes, call this method on each until one returns true
    if (HasChildren())
        return
            Child<CollisionQuadTree>(0)->DoesAreaOverlapAnyEntityWrapped(
                area_center,
                area_radius,
                check_nonsolid_collision_entities,
                object_layer_side_length,
                half_object_layer_side_length)
            ||
            Child<CollisionQuadTree>(1)->DoesAreaOverlapAnyEntityWrapped(
                area_center,
                area_radius,
                check_nonsolid_collision_entities,
                object_layer_side_length,
                half_object_layer_side_length)
            ||
            Child<CollisionQuadTree>(2)->DoesAreaOverlapAnyEntityWrapped(
                area_center,
                area_radius,
                check_nonsolid_collision_entities,
                object_layer_side_length,
                half_object_layer_side_length)
            ||
            Child<CollisionQuadTree>(3)->DoesAreaOverlapAnyEntityWrapped(
                area_center,
                area_radius,
                check_nonsolid_collision_entities,
                object_layer_side_length,
                half_object_layer_side_length);
    else
        return false;
}

void CollisionQuadTree::LineTrace (
    FloatVector2 const &trace_start,
    FloatVector2 const &trace_vector,
    Float const trace_radius,
    bool const check_nonsolid_collision_entities,
    LineTraceBindingSet *const line_trace_binding_set)
{
    ASSERT1(!trace_vector.IsZero());
    ASSERT1(trace_radius >= 0.0f);
    ASSERT1(line_trace_binding_set != NULL);

    // if this quad node has no subordinates, return
    if (GetSubordinateObjectCount() == 0)
        return;

    // if this quad node doesn't intersect the line, return
    Float a = trace_vector | trace_vector;
    {
        FloatVector2 p_minus_c = trace_start - Center();
        Float R = GetRadius() + trace_radius;
        Float b = p_minus_c | trace_vector;
        Float c = (p_minus_c | p_minus_c) - R * R;
        Float determinant = b * b - a * c;
        if (determinant < 0.0f)
            return;
    }

    // check the line against the objects in this node
    for (ObjectSetIterator it = m_object_set.begin(),
                           it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Engine2::Object *object = *it;
        ASSERT1(object != NULL);
        ASSERT2(object->GetOwnerQuadTree(Engine2::QTT_PHYSICS_HANDLER) == this);

        Entity *entity = DStaticCast<Entity *>(object->GetEntity());
        ASSERT1(entity != NULL);

        // don't check nonsolid collision entities if
        // check_nonsolid_collision_entities isn't set.
        ASSERT1(entity->GetCollisionType() != CT_NO_COLLISION);
        if (entity->GetCollisionType() == CT_NONSOLID_COLLISION &&
            !check_nonsolid_collision_entities)
            continue;

        // check the trace line against the object
        FloatVector2 p_minus_c = trace_start - entity->GetTranslation();
        Float R = entity->GetRadius(GetQuadTreeType()) + trace_radius;
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
                line_trace_binding_set);
}

void CollisionQuadTree::LineTraceWrapped (
    FloatVector2 const &trace_start,
    FloatVector2 const &trace_vector,
    Float const trace_radius,
    bool const check_nonsolid_collision_entities,
    LineTraceBindingSet *const line_trace_binding_set,
    Float const object_layer_side_length,
    Float const half_object_layer_side_length)
{
    ASSERT1(!trace_vector.IsZero());
    ASSERT1(trace_radius >= 0.0f);
    ASSERT1(line_trace_binding_set != NULL);

    // if this quad node has no subordinates, return
    if (GetSubordinateObjectCount() == 0)
        return;

    // if this quad node doesn't intersect the line, return
    Float a = trace_vector | trace_vector;
    FloatVector2 trace_center(trace_start + 0.5f * trace_vector);
    {
        FloatVector2 adjusted_center(Center());

        if (adjusted_center[Dim::X] - trace_center[Dim::X] > half_object_layer_side_length)
            adjusted_center[Dim::X] -= object_layer_side_length;
        else if (adjusted_center[Dim::X] - trace_center[Dim::X] < -half_object_layer_side_length)
            adjusted_center[Dim::X] += object_layer_side_length;

        if (adjusted_center[Dim::Y] - trace_center[Dim::Y] > half_object_layer_side_length)
            adjusted_center[Dim::Y] -= object_layer_side_length;
        else if (adjusted_center[Dim::Y] - trace_center[Dim::Y] < -half_object_layer_side_length)
            adjusted_center[Dim::Y] += object_layer_side_length;

        FloatVector2 p_minus_c = trace_start - adjusted_center;
        Float R = 2.0f * GetRadius() + trace_radius;
        Float b = p_minus_c | trace_vector;
        Float c = (p_minus_c | p_minus_c) - R * R;
        Float determinant = b * b - a * c;
        if (determinant < 0.0f)
            return;
    }

    // check the line against the objects in this node
    for (ObjectSetIterator it = m_object_set.begin(),
                           it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Engine2::Object *object = *it;
        ASSERT1(object != NULL);
        ASSERT2(object->GetOwnerQuadTree(Engine2::QTT_PHYSICS_HANDLER) == this);

        Entity *entity = DStaticCast<Entity *>(object->GetEntity());
        ASSERT1(entity != NULL);

        // don't check nonsolid collision entities if
        // check_nonsolid_collision_entities isn't set.
        ASSERT1(entity->GetCollisionType() != CT_NO_COLLISION);
        if (entity->GetCollisionType() == CT_NONSOLID_COLLISION &&
            !check_nonsolid_collision_entities)
            continue;

        FloatVector2 adjusted_entity_translation(entity->GetTranslation());

        if (adjusted_entity_translation[Dim::X] - trace_center[Dim::X] > half_object_layer_side_length)
            adjusted_entity_translation[Dim::X] -= object_layer_side_length;
        else if (adjusted_entity_translation[Dim::X] - trace_center[Dim::X] < -half_object_layer_side_length)
            adjusted_entity_translation[Dim::X] += object_layer_side_length;

        if (adjusted_entity_translation[Dim::Y] - trace_center[Dim::Y] > half_object_layer_side_length)
            adjusted_entity_translation[Dim::Y] -= object_layer_side_length;
        else if (adjusted_entity_translation[Dim::Y] - trace_center[Dim::Y] < -half_object_layer_side_length)
            adjusted_entity_translation[Dim::Y] += object_layer_side_length;

        // check the trace line against the object
        FloatVector2 p_minus_c = trace_start - adjusted_entity_translation;
        Float R = entity->GetRadius(GetQuadTreeType()) + trace_radius;
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
            Child<CollisionQuadTree>(i)->LineTraceWrapped(
                trace_start,
                trace_vector,
                trace_radius,
                check_nonsolid_collision_entities,
                line_trace_binding_set,
                object_layer_side_length,
                half_object_layer_side_length);
}

void CollisionQuadTree::AreaTrace (
    FloatVector2 const &trace_area_center,
    Float const trace_area_radius,
    bool const check_nonsolid_collision_entities,
    AreaTraceList *const area_trace_list)
{
    ASSERT1(trace_area_radius > 0.0f);
    ASSERT1(area_trace_list != NULL);

    // if this quad node has no subordinates, return
    if (GetSubordinateObjectCount() == 0)
        return;

    // if this quad node doesn't intersect the line, return
    if (!DoesAreaOverlapQuadBounds(trace_area_center, trace_area_radius))
        return;

    // call this function on the child nodes, if they exist
    if (HasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            Child<CollisionQuadTree>(i)->AreaTrace(
                trace_area_center,
                trace_area_radius,
                check_nonsolid_collision_entities,
                area_trace_list);

    // check the line against the objects in this node
    for (ObjectSetIterator it = m_object_set.begin(),
                           it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Engine2::Object *object = *it;
        ASSERT1(object != NULL);
        ASSERT2(object->GetOwnerQuadTree(Engine2::QTT_PHYSICS_HANDLER) == this);

        Entity *entity = DStaticCast<Entity *>(object->GetEntity());
        ASSERT1(entity != NULL);

        // don't check nonsolid collision entities if
        // check_nonsolid_collision_entities isn't set.
        ASSERT1(entity->GetCollisionType() != CT_NO_COLLISION);
        if (entity->GetCollisionType() == CT_NONSOLID_COLLISION &&
            !check_nonsolid_collision_entities)
            continue;

        // don't add it if the entity isn't touching the trace area
        FloatVector2 center_to_center(entity->GetTranslation() - trace_area_center);
        if (center_to_center.GetLength() >= entity->GetRadius(GetQuadTreeType()) + trace_area_radius)
            continue;

        area_trace_list->push_back(entity);
    }
}

void CollisionQuadTree::AreaTraceWrapped (
    FloatVector2 const &trace_area_center,
    Float const trace_area_radius,
    bool const check_nonsolid_collision_entities,
    AreaTraceList *const area_trace_list,
    Float const object_layer_side_length,
    Float const half_object_layer_side_length)
{
    ASSERT1(trace_area_radius > 0.0f);
    ASSERT1(area_trace_list != NULL);

    // if this quad node has no subordinates, return
    if (GetSubordinateObjectCount() == 0)
        return;

    // if this quad node doesn't intersect the line, return
    if (!DoesAreaOverlapQuadBoundsWrapped(
            trace_area_center,
            trace_area_radius,
            object_layer_side_length,
            half_object_layer_side_length))
        return;

    // call this function on the child nodes, if they exist
    if (HasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            Child<CollisionQuadTree>(i)->AreaTraceWrapped(
                trace_area_center,
                trace_area_radius,
                check_nonsolid_collision_entities,
                area_trace_list,
                object_layer_side_length,
                half_object_layer_side_length);

    // check the line against the objects in this node
    for (ObjectSetIterator it = m_object_set.begin(),
                           it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Engine2::Object *object = *it;
        ASSERT1(object != NULL);
        ASSERT2(object->GetOwnerQuadTree(Engine2::QTT_PHYSICS_HANDLER) == this);

        Entity *entity = DStaticCast<Entity *>(object->GetEntity());
        ASSERT1(entity != NULL);

        // don't check nonsolid collision entities if
        // check_nonsolid_collision_entities isn't set.
        ASSERT1(entity->GetCollisionType() != CT_NO_COLLISION);
        if (entity->GetCollisionType() == CT_NONSOLID_COLLISION &&
            !check_nonsolid_collision_entities)
            continue;

        FloatVector2 adjusted_entity_translation(entity->GetTranslation());

        if (adjusted_entity_translation[Dim::X] - trace_area_center[Dim::X] > half_object_layer_side_length)
            adjusted_entity_translation[Dim::X] -= object_layer_side_length;
        else if (adjusted_entity_translation[Dim::X] - trace_area_center[Dim::X] < -half_object_layer_side_length)
            adjusted_entity_translation[Dim::X] += object_layer_side_length;

        if (adjusted_entity_translation[Dim::Y] - trace_area_center[Dim::Y] > half_object_layer_side_length)
            adjusted_entity_translation[Dim::Y] -= object_layer_side_length;
        else if (adjusted_entity_translation[Dim::Y] - trace_area_center[Dim::Y] < -half_object_layer_side_length)
            adjusted_entity_translation[Dim::Y] += object_layer_side_length;

        // don't add it if the entity isn't touching the trace area
        FloatVector2 center_to_center(adjusted_entity_translation - trace_area_center);
        if (center_to_center.GetLength() >= entity->GetRadius(GetQuadTreeType()) + trace_area_radius)
            continue;

        area_trace_list->push_back(entity);
    }
}

void CollisionQuadTree::CollideEntity (
    Entity *const entity,
    Float const frame_dt,
    CollisionPairList *const collision_pair_list)
{
    ASSERT1(entity != NULL);
    ASSERT1(entity->GetCollisionType() != CT_NO_COLLISION);
    ASSERT1(collision_pair_list != NULL);

    Float const adjusted_dt = frame_dt;//1.0f/40.0f;
    Float const dt_squared = adjusted_dt * adjusted_dt;

    // if there are no objects here or below, just return
    if (GetSubordinateObjectCount() == 0)
        return;

    // return if the area doesn't intersect this node
    if (!DoesAreaOverlapQuadBounds(entity->GetTranslation(), entity->GetRadius(GetQuadTreeType())))
        return;

    // if there are child nodes, call CollideEntity on each
    if (HasChildren())
    {
        for (Uint8 i = 0; i < 4; ++i)
            Child<CollisionQuadTree>(i)->CollideEntity(
                entity,
                frame_dt,
                collision_pair_list);

        // if the minimum object size for this node is larger than the
        // collision entity, return (because it will skip all objects
        // below in the loop anyway)
        if (!IsAllowableObjectRadius(entity->GetOwnerObject()))
            return;
    }

    // check if the entity overlaps any object in this node's list.
    for (ObjectSetIterator it = m_object_set.begin(),
                           it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Engine2::Object *object = *it;
        ASSERT1(object != NULL);
        ASSERT2(object->GetOwnerQuadTree(Engine2::QTT_PHYSICS_HANDLER) == this);

        // don't collide the entity with itself
        if (object == entity->GetOwnerObject())
            continue;

        // this is a quick and easy way to avoid calculating
        // the same collision pair twice
        if (object->GetRadius(GetQuadTreeType()) > entity->GetRadius(GetQuadTreeType()))
            continue;
        else if (object->GetRadius(GetQuadTreeType()) == entity->GetRadius(GetQuadTreeType()) &&
                 object > entity->GetOwnerObject())
            continue;

        Float r = entity->GetRadius(GetQuadTreeType()) + object->GetRadius(GetQuadTreeType());
        FloatVector2 P = entity->GetTranslation() - object->GetTranslation();

        if (P.GetLength() >= r)
            continue;

        Entity *other_entity = DStaticCast<Entity *>(object->GetEntity());
        ASSERT1(other_entity != NULL);

        // calculate the collision

        FloatVector2 V = entity->GetVelocity() - other_entity->GetVelocity();
        FloatVector2 collision_location(
            (other_entity->GetScaleFactor() * entity->GetTranslation() + entity->GetScaleFactor() * other_entity->GetTranslation())
            /
            (entity->GetScaleFactor() + other_entity->GetScaleFactor()));
        FloatVector2 collision_normal;
        if (P.IsZero())
            collision_normal = FloatVector2(1.0f, 0.0f);
        else
            collision_normal = P.GetNormalization();
        Float collision_force = 0.0f;

        if ((V | P) < 0.0f && // and if the distance between the two is closing
            entity->GetCollisionType() == CT_SOLID_COLLISION && // and if they're both solid
            other_entity->GetCollisionType() == CT_SOLID_COLLISION &&
            Entity::GetShouldApplyCollisionForces(entity, other_entity)) // and if this isn't an exception to the rule
        {
            Float M = 1.0f / entity->GetMass() + 1.0f / other_entity->GetMass();
            FloatVector2 Q(P + adjusted_dt*V);
            FloatVector2 A(dt_squared*M*collision_normal);

            Float a = A | A;
            Float b = 2.0f * (Q | A);
            Float c = (Q | Q) - r*r;
            Float discriminant = b*b - 4.0f*a*c;
            if (discriminant >= 0.0f)
            {
                Float temp0 = sqrt(discriminant);
                Float temp1 = 2.0f * a;

                Float force0 = 0.8f * (-b - temp0) / temp1;
                Float force1 = 0.8f * (-b + temp0) / temp1;

                Float min_force = Min(force0, force1);
                Float max_force = Max(force0, force1);
                if (min_force > 0.0f)
                    collision_force = min_force;
                else if (max_force > 0.0f)
                    collision_force = max_force;
                else
                    collision_force = 0.0f;

                collision_force *= (1.0f + entity->Elasticity() * other_entity->Elasticity());

                entity->AccumulateForce(collision_force*collision_normal);
                other_entity->AccumulateForce(-collision_force*collision_normal);
            }
        }

        // record the collision in the collision pair list.
        collision_pair_list->push_back(
            CollisionPair(
                entity,
                other_entity,
                collision_location,
                collision_normal,
                collision_force));
    }
}

void CollisionQuadTree::CollideEntityWrappedLoopFunctor::operator () (Engine2::Object *const object)
{
    // don't collide the entity with itself
    if (object == m_entity->GetOwnerObject())
        return;

    // this is a quick and easy way to avoid calculating
    // the same collision pair twice
    if (object->GetRadius(m_quad_tree_type) > m_entity->GetRadius(m_quad_tree_type)
        ||
        (object->GetRadius(m_quad_tree_type) == m_entity->GetRadius(m_quad_tree_type) &&
         object > m_entity->GetOwnerObject()))
        return;

    FloatVector2 ce0_translation(m_entity->GetTranslation());
    FloatVector2 ce1_translation(object->GetTranslation());

    if (ce1_translation[Dim::X] - ce0_translation[Dim::X] > m_half_object_layer_side_length)
        ce1_translation[Dim::X] -= m_object_layer_side_length;
    else if (ce1_translation[Dim::X] - ce0_translation[Dim::X] < -m_half_object_layer_side_length)
        ce1_translation[Dim::X] += m_object_layer_side_length;

    if (ce1_translation[Dim::Y] - ce0_translation[Dim::Y] > m_half_object_layer_side_length)
        ce1_translation[Dim::Y] -= m_object_layer_side_length;
    else if (ce1_translation[Dim::Y] - ce0_translation[Dim::Y] < -m_half_object_layer_side_length)
        ce1_translation[Dim::Y] += m_object_layer_side_length;

    Float r = m_entity->GetRadius(m_quad_tree_type) + object->GetRadius(m_quad_tree_type);
    FloatVector2 P = ce0_translation - ce1_translation;

    if (P.GetLength() >= r)
        return;

    Entity *other_entity = DStaticCast<Entity *>(object->GetEntity());
    ASSERT1(other_entity != NULL);

    // calculate the collision

    FloatVector2 V = m_entity->GetVelocity() - other_entity->GetVelocity();
    FloatVector2 collision_location(
        (other_entity->GetScaleFactor() * ce0_translation + m_entity->GetScaleFactor() * ce1_translation)
        /
        (m_entity->GetScaleFactor() + other_entity->GetScaleFactor()));
    FloatVector2 collision_normal;
    if (P.IsZero())
        collision_normal = FloatVector2(1.0f, 0.0f);
    else
        collision_normal = P.GetNormalization();
    Float collision_force = 0.0f;

    if ((V | P) < 0.0f && // and if the distance between the two is closing
        m_entity->GetCollisionType() == CT_SOLID_COLLISION && // and if they're both solid
        other_entity->GetCollisionType() == CT_SOLID_COLLISION &&
        Entity::GetShouldApplyCollisionForces(m_entity, other_entity)) // and if this isn't an exception to the rule
    {
        Float M = 1.0f / m_entity->GetMass() + 1.0f / other_entity->GetMass();
        FloatVector2 Q(P + m_frame_dt*V);
        FloatVector2 A(m_frame_dt_squared*M*collision_normal);

        Float a = A | A;
        Float b = 2.0f * (Q | A);
        Float c = (Q | Q) - r*r;
        Float discriminant = b*b - 4.0f*a*c;
        if (discriminant >= 0.0f)
        {
            Float temp0 = sqrt(discriminant);
            Float temp1 = 2.0f * a;

            Float force0 = 0.8f * (-b - temp0) / temp1;
            Float force1 = 0.8f * (-b + temp0) / temp1;

            Float min_force = Min(force0, force1);
            Float max_force = Max(force0, force1);
            if (min_force > 0.0f)
                collision_force = min_force;
            else if (max_force > 0.0f)
                collision_force = max_force;
            else
                collision_force = 0.0f;

            collision_force *= (1.0f + m_entity->Elasticity() * other_entity->Elasticity());

            m_entity->AccumulateForce(collision_force*collision_normal);
            other_entity->AccumulateForce(-collision_force*collision_normal);
        }
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

void CollisionQuadTree::CollideEntityWrapped (
    Entity *const entity,
    Float const frame_dt,
    CollisionPairList *const collision_pair_list,
    Float const object_layer_side_length)
{
    CollideEntityWrappedLoopFunctor
        functor(
            entity,
            frame_dt,
            collision_pair_list,
            object_layer_side_length,
            GetQuadTreeType());
    CollideEntityWrapped(functor);
}

void CollisionQuadTree::CollideEntityWrapped (CollisionQuadTree::CollideEntityWrappedLoopFunctor &functor)
{
    // if there are no objects here or below, just return
    if (GetSubordinateObjectCount() == 0)
        return;

    // return if the area doesn't intersect this node
    if (!DoesAreaOverlapQuadBoundsWrapped(
            functor.GetEntity()->GetTranslation(),
            functor.GetEntity()->GetRadius(GetQuadTreeType()),
            functor.GetObjectLayerSideLength(),
            functor.HalfObjectLayerSideLength()))
        return;

    // if there are child nodes, call CollideEntityWrapped on each
    if (HasChildren())
    {
        for (Uint8 i = 0; i < 4; ++i)
            Child<CollisionQuadTree>(i)->CollideEntityWrapped(functor);

        // if the minimum object size for this node is larger than the
        // collision entity, return (because it will skip all objects
        // below in the loop anyway)
        if (!IsAllowableObjectRadius(functor.GetEntity()->GetOwnerObject()))
            return;
    }

    std::for_each(m_object_set.begin(), m_object_set.end(), functor);
}

} // end of namespace Dis
