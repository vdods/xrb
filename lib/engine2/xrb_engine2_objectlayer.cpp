// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_objectlayer.cpp by Victor Dods, created 2004/07/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_objectlayer.h"

#include "xrb_engine2_entity.h"
#include "xrb_engine2_visibilityquadtree.h"
#include "xrb_serializer.h"

namespace Xrb
{

Engine2::ObjectLayer::~ObjectLayer ()
{
    Delete(m_quad_tree);
}

Engine2::ObjectLayer *Engine2::ObjectLayer::Create (
    Engine2::World *const owner_world,
    bool const is_wrapped,
    Float const side_length,
    Uint32 const tree_depth,
    Float const z_depth)
{
    ASSERT1(owner_world != NULL)
    ASSERT1(side_length > 0.0)
    ASSERT1(tree_depth > 0)

    ObjectLayer *retval =
        new ObjectLayer(
            owner_world,
            is_wrapped,
            side_length,
            z_depth);

    retval->m_quad_tree =
        new VisibilityQuadTree(
            FloatVector2::ms_zero,
            0.5f*side_length,
            tree_depth);

    return retval;
}

Engine2::ObjectLayer *Engine2::ObjectLayer::Create (
    Serializer &serializer,
    Engine2::World *const owner_world)
{
    ASSERT1(owner_world != NULL)

    Float side_length;
    Float z_depth;
    serializer.ReadFloat(&side_length);
    serializer.ReadFloat(&z_depth);
    bool is_wrapped = serializer.ReadBool();
    ObjectLayer *retval =
        new ObjectLayer(
            owner_world,
            is_wrapped,
            side_length,
            z_depth);

    // this call just constructs the quadtree nodes,
    retval->m_quad_tree = VisibilityQuadTree::Create(serializer);
    // the objects have to be added here
    retval->m_quad_tree->ReadObjects(serializer, retval);

    return retval;
}

Engine2::Object *Engine2::ObjectLayer::GetSmallestObjectTouchingPoint (
    FloatVector2 const &point) const
{
    return m_quad_tree->GetSmallestObjectTouchingPoint(point);
}

bool Engine2::ObjectLayer::GetDoesAreaOverlapAnyObject (
    FloatVector2 const &area_center,
    Float const area_radius) const
{
    ASSERT1(m_quad_tree != NULL)
    if (GetIsWrapped())
        return m_quad_tree->GetDoesAreaOverlapAnyObjectWrapped(
            area_center,
            area_radius,
            GetSideLength(),
            0.5f * GetSideLength());
    else
        return m_quad_tree->GetDoesAreaOverlapAnyObject(area_center, area_radius);
}

FloatVector2 Engine2::ObjectLayer::GetNormalizedCoordinates (
    FloatVector2 const &coordinates) const
{
    FloatVector2 normalized_coordinates(coordinates);
    
    Float const half_object_layer_side_length = 0.5f * GetSideLength();
    if (GetIsWrapped())
    {
        if (normalized_coordinates[Dim::X] < -half_object_layer_side_length)
            normalized_coordinates[Dim::X] += GetSideLength();
        else if (normalized_coordinates[Dim::X] > half_object_layer_side_length)
            normalized_coordinates[Dim::X] -= GetSideLength();
            
        if (normalized_coordinates[Dim::Y] < -half_object_layer_side_length)
            normalized_coordinates[Dim::Y] += GetSideLength();
        else if (normalized_coordinates[Dim::Y] > half_object_layer_side_length)
            normalized_coordinates[Dim::Y] -= GetSideLength();            
    }
    else
    {
        if (normalized_coordinates[Dim::X] < -half_object_layer_side_length)
            normalized_coordinates[Dim::X] = -half_object_layer_side_length;
        else if (normalized_coordinates[Dim::X] > half_object_layer_side_length)
            normalized_coordinates[Dim::X] = half_object_layer_side_length;
            
        if (normalized_coordinates[Dim::Y] < -half_object_layer_side_length)
            normalized_coordinates[Dim::Y] = -half_object_layer_side_length;
        else if (normalized_coordinates[Dim::Y] > half_object_layer_side_length)
            normalized_coordinates[Dim::Y] = half_object_layer_side_length;
    }
    
    return normalized_coordinates;
}

FloatVector2 Engine2::ObjectLayer::GetAdjustedCoordinates (
    FloatVector2 const &coordinates,
    FloatVector2 const &reference_coordinates) const
{
    if (GetIsWrapped())
    {
        Float const half_object_layer_side_length = 0.5f * GetSideLength();
        FloatVector2 adjusted_coordinates(coordinates);

        ASSERT1(reference_coordinates[Dim::X] >= -half_object_layer_side_length)
        ASSERT1(reference_coordinates[Dim::X] <=  half_object_layer_side_length)
        ASSERT1(reference_coordinates[Dim::X] >= -half_object_layer_side_length)
        ASSERT1(reference_coordinates[Dim::Y] <=  half_object_layer_side_length)
        
        if (adjusted_coordinates[Dim::X] < reference_coordinates[Dim::X] - half_object_layer_side_length)
            adjusted_coordinates[Dim::X] += GetSideLength();
        else if (adjusted_coordinates[Dim::X] > reference_coordinates[Dim::X] + half_object_layer_side_length)
            adjusted_coordinates[Dim::X] -= GetSideLength();
        
        if (adjusted_coordinates[Dim::Y] < reference_coordinates[Dim::Y] - half_object_layer_side_length)
            adjusted_coordinates[Dim::Y] += GetSideLength();
        else if (adjusted_coordinates[Dim::Y] > reference_coordinates[Dim::Y] + half_object_layer_side_length)
            adjusted_coordinates[Dim::Y] -= GetSideLength();

        return adjusted_coordinates;
    }
    else
        return coordinates;
}

void Engine2::ObjectLayer::Write (Serializer &serializer) const
{
    // write the ObjectLayer's overhead information
    serializer.WriteFloat(m_side_length);
    serializer.WriteFloat(m_z_depth);
    serializer.WriteBool(m_is_wrapped);

    ASSERT1(m_quad_tree != NULL)
    // write the quad tree structure
    m_quad_tree->WriteStructure(serializer);
    // write the objects the quadtree contains
    Uint32 number_of_non_entities_written = m_quad_tree->WriteObjects(serializer);
    ASSERT1(number_of_non_entities_written == m_quad_tree->GetSubordinateNonEntityCount())
}

Uint32 Engine2::ObjectLayer::Draw (
    RenderContext const &render_context,
    FloatMatrix2 const &world_to_screen,
    Float const pixels_in_view_radius,
    FloatVector2 const &view_center,
    Float const &view_radius)
{
    ASSERT2(view_radius > 0.0)

    if (m_is_wrapped)
        return m_quad_tree->DrawWrapped(render_context, world_to_screen, pixels_in_view_radius, view_center, view_radius);
    else
        return m_quad_tree->Draw(render_context, world_to_screen, pixels_in_view_radius, view_center, view_radius);
}

void Engine2::ObjectLayer::AddObject (Engine2::Object *const object)
{
    ASSERT1(object != NULL)
    ASSERT1(m_quad_tree != NULL)
    ASSERT1(object->GetOwnerQuadTree(QTT_VISIBILITY) == NULL)

    HandleContainmentOrWrapping(object);
    // attempt to add it to the quad tree
    bool add_success = m_quad_tree->AddObject(object);
    // set the object's layer
    object->SetObjectLayer(this);

    ASSERT1(add_success)
    ASSERT1(object->GetOwnerQuadTree(QTT_VISIBILITY) != NULL)
}

void Engine2::ObjectLayer::RemoveObject (Engine2::Object *const object)
{
    ASSERT1(object != NULL)
    ASSERT1(object->GetObjectLayer() == this)
    ASSERT1(object->GetOwnerQuadTree(QTT_VISIBILITY) != NULL)
    object->GetOwnerQuadTree(QTT_VISIBILITY)->RemoveObject(object);
}

void Engine2::ObjectLayer::HandleContainmentOrWrapping (Object *object)
{
    ASSERT1(object != NULL)
    ASSERT1(m_quad_tree != NULL)
    
    // make sure that it's inside the layer's bounds first
    Entity *entity = dynamic_cast<Entity *>(object);
    if (m_is_wrapped)
    {
        if (entity != NULL)
            WrapEntity(entity);
        else
            WrapTransform2(&object->GetTransform());
    }
    else
    {
        if (entity != NULL)
            ContainEntity(entity);
        else
            ContainTransform2(&object->GetTransform());
    }
}

void Engine2::ObjectLayer::ContainVector2 (FloatVector2 *const vector) const
{
    ASSERT1(vector != NULL)

    for (Uint32 i = 0; i <= 1; ++i)
    {
        if (vector->m[i] > m_half_side_length)
            vector->m[i] = m_half_side_length;
        else if (vector->m[i] < -m_half_side_length)
            vector->m[i] = -m_half_side_length;
    }
}

void Engine2::ObjectLayer::ContainTransform2 (FloatTransform2 *const transform) const
{
    ASSERT1(transform != NULL)

    FloatVector2 translation(transform->GetTranslation());
    ContainVector2(&translation);
    transform->SetTranslation(translation);
}

void Engine2::ObjectLayer::ContainEntity (Engine2::Entity *const entity) const
{
    ASSERT1(entity != NULL)

    FloatVector2 translation(entity->GetTranslation());

    for (Uint32 i = 0; i <= 1; ++i)
    {
        if (translation.m[i] > m_half_side_length)
        {
            translation.m[i] = m_half_side_length;
            entity->SetVelocityComponent(i, 0.0);
        }
        else if (translation.m[i] < -m_half_side_length)
        {
            translation.m[i] = -m_half_side_length;
            entity->SetVelocityComponent(i, 0.0);
        }
    }

    entity->SetTranslation(translation);
}

void Engine2::ObjectLayer::WrapVector2 (FloatVector2 *const vector) const
{
    ASSERT1(vector != NULL)

    for (Uint32 i = 0; i <= 1; ++i)
    {
        if (vector->m[i] > m_half_side_length)
            vector->m[i] -= m_side_length *
                            (int)((vector->m[i] + m_half_side_length) /
                            m_side_length);

        if (vector->m[i] < -m_half_side_length)
            vector->m[i] -= m_side_length *
                            (int)((vector->m[i] - m_half_side_length) /
                            m_side_length);
    }
}

void Engine2::ObjectLayer::WrapTransform2 (FloatTransform2 *const transform) const
{
    ASSERT1(transform != NULL)

    FloatVector2 translation(transform->GetTranslation());
    WrapVector2(&translation);
    transform->SetTranslation(translation);
}

void Engine2::ObjectLayer::WrapEntity (Entity *const entity) const
{
    ASSERT1(entity != NULL)

    FloatVector2 previous_translation(entity->GetTranslation());
    WrapTransform2(&entity->GetTransform());
    entity->AccumulateWrappedOffset(previous_translation - entity->GetTranslation());
}

Engine2::ObjectLayer::ObjectLayer (
    Engine2::World *const owner_world,
    bool const is_wrapped,
    Float const side_length,
    Float const z_depth)
{
    ASSERT1(owner_world != NULL)
    ASSERT1(side_length > 0.0f)
    ASSERT1(z_depth >= 0.0f)

    m_owner_world = owner_world;
    m_is_wrapped = is_wrapped;
    m_side_length = side_length;
    m_half_side_length = 0.5f * side_length;
    m_z_depth = z_depth;
    m_quad_tree = NULL;
}

} // end of namespace Xrb
