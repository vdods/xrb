// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_objectlayer.cpp by Victor Dods, created 2005/03/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_mapeditor2_objectlayer.hpp"

#include "xrb_mapeditor2_compoundvertex.hpp"
#include "xrb_mapeditor2_entity.hpp"
#include "xrb_mapeditor2_polygon.hpp"
#include "xrb_mapeditor2_visibilityquadtree.hpp"
#include "xrb_mapeditor2_world.hpp"
#include "xrb_serializer.hpp"

namespace Xrb
{

MapEditor2::ObjectLayer *MapEditor2::ObjectLayer::Create (
    MapEditor2::World *const owner_world,
    bool const wrapped,
    Float const side_length,
    Uint32 const tree_depth,
    Float const z_depth)
{
    ASSERT1(owner_world != NULL);
    ASSERT1(side_length > 0.0f);
    ASSERT1(tree_depth > 0);

    ObjectLayer *retval =
        new ObjectLayer(
            owner_world,
            wrapped,
            side_length,
            z_depth);

    retval->m_quad_tree =
        new VisibilityQuadTree(
            FloatVector2::ms_zero,
            0.5f*side_length,
            tree_depth);

    return retval;
}

MapEditor2::ObjectLayer *MapEditor2::ObjectLayer::Create (
    Serializer &serializer,
    MapEditor2::World *const owner_world)
{
    ASSERT1(owner_world != NULL);

    Float side_length;
    Float z_depth;
    serializer.ReadFloat(&side_length);
    serializer.ReadFloat(&z_depth);
    bool wrapped = serializer.ReadBool();
    ObjectLayer *retval =
        new ObjectLayer(
            owner_world,
            wrapped,
            side_length,
            z_depth);

    VisibilityQuadTree *quad_tree = VisibilityQuadTree::Create(serializer);
    retval->m_quad_tree = quad_tree;
    quad_tree->ReadObjects(serializer, retval);
    // TODO: figure out what the fuck is wrong with this --
    // why retval->m_quad_tree->ReadObjects(...) doesn't
    // call MapEditor2::VisibilityQuadTree::ReadObjects.
//     // this call just constructs the quadtree nodes,
//     retval->m_quad_tree = VisibilityQuadTree::Create(serializer);
//     // the objects have to be added here
//     retval->m_quad_tree->ReadObjects(serializer, retval);

    return retval;
}

MapEditor2::World *MapEditor2::ObjectLayer::GetOwnerMapEditorWorld () const
{
    World *retval = dynamic_cast<World *>(GetOwnerWorld());
    ASSERT1(GetOwnerWorld() == NULL || retval != NULL);
    return retval;
}

MapEditor2::Object *MapEditor2::ObjectLayer::GetSingleSelectedObject ()
{
    ASSERT1(GetSelectedObjectCount() == 1);
    ASSERT1(GetSelectedObjectCount() == m_object_selection_set.size());
    return DStaticCast<Object *>(*m_object_selection_set.begin());
}

MapEditor2::Object *MapEditor2::ObjectLayer::GetSingleSelectedNonEntity ()
{
    ASSERT1(GetSelectedNonEntityCount() == 1);
    ASSERT1(GetSelectedNonEntityCount() == m_object_selection_set.size());
    ASSERT1(dynamic_cast<Entity *>(*m_object_selection_set.begin()) == NULL);
    return DStaticCast<Object *>(*m_object_selection_set.begin());
}

MapEditor2::Entity *MapEditor2::ObjectLayer::GetSingleSelectedEntity ()
{
    ASSERT1(GetSelectedEntityCount() == 1);
    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Entity *entity =
            dynamic_cast<Entity *>(*it);
        if (entity != NULL)
            return entity;
    }
    ASSERT1(false && "Counted number of selected entities does not match reality");
    return NULL;
}

MapEditor2::Compound *MapEditor2::ObjectLayer::GetSingleSelectedCompound ()
{
    ASSERT1(GetSelectedCompoundCount() == 1);
    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Compound *compound =
            dynamic_cast<Compound *>(*it);
        if (compound != NULL)
            return compound;
    }
    ASSERT1(false && "Counted number of selected compounds does not match reality");
    return NULL;
}

MapEditor2::ObjectLayer::ObjectLayer (
    World *const owner_world,
    bool const wrapped,
    Float const side_length,
    Float const z_depth)
    :
    Engine2::ObjectLayer(owner_world, wrapped, side_length, z_depth),
    m_sender_object_selection_set_changed(this),
    m_sender_selected_object_count_changed(this),
    m_sender_object_selection_set_origin_changed(this),
    m_sender_selected_entity_count_changed(this),
    m_sender_object_selection_set_first_moment_changed(this),
    m_sender_object_selection_set_center_of_gravity_changed(this),
    m_sender_object_selection_set_velocity_changed(this),
    m_sender_object_selection_set_second_moment_changed(this),
    m_sender_object_selection_set_angular_velocity_changed(this),
    m_sender_object_selection_set_elasticity_changed(this),
    m_sender_object_selection_set_density_changed(this),
    m_sender_selected_compound_count_changed(this),
    m_sender_selected_vertex_count_changed(this),
    m_sender_selected_polygon_count_changed(this)
{
    m_selected_object_count = 0;
    m_object_selection_set_origin = FloatVector2::ms_zero;
    
    m_selected_entity_count = 0;
    m_object_selection_set_first_moment = 0.0f;
    m_object_selection_set_center_of_gravity = FloatVector2::ms_zero;
    m_object_selection_set_velocity = FloatVector2::ms_zero;
    m_object_selection_set_second_moment = 0.0f;
    m_object_selection_set_angular_velocity = 0.0f;
    m_object_selection_set_elasticity = 0.0f;
    m_object_selection_set_density = 0.0f;

    m_selected_compound_count = 0;
    
    m_selected_vertex_count = 0;

    m_selected_polygon_count = 0;
}

MapEditor2::VisibilityQuadTree *MapEditor2::ObjectLayer::GetMapEditorQuadTree () const
{
    VisibilityQuadTree *retval = dynamic_cast<VisibilityQuadTree *>(GetQuadTree());
    ASSERT1(GetQuadTree() == NULL || retval != NULL);
    return retval;
}

void MapEditor2::ObjectLayer::DrawMetrics (
    RenderContext const &render_context,
    FloatMatrix2 const &world_to_screen,
    Float const pixels_in_view_radius,
    FloatVector2 const &view_center,
    Float const view_radius,
    MapEditor2::Object::MetricMode const metric_mode)
{
    ASSERT2(pixels_in_view_radius > 0.0);
    ASSERT2(view_radius > 0.0);

    if (GetIsWrapped())
        GetMapEditorQuadTree()->DrawMetricsWrapped(
            render_context,
            world_to_screen,
            pixels_in_view_radius,
            view_center,
            view_radius,
            metric_mode);
    else
        GetMapEditorQuadTree()->DrawMetrics(
            render_context,
            world_to_screen,
            pixels_in_view_radius,
            view_center,
            view_radius,
            metric_mode);
}

// ///////////////////////////////////////////////////////////////////////////
// object selection set public functions
// ///////////////////////////////////////////////////////////////////////////

void MapEditor2::ObjectLayer::SelectSmallestObjectTouchingPoint (
    FloatVector2 const &point,
    MapEditor2::Object::SelectionOperation const selection_operation)
{
    Object *object =
        GetMapEditorQuadTree()->GetSmallestMapEditorObjectTouchingPoint(point);
    if (object != NULL)
        object->ApplyObjectSelectionOperation(selection_operation, true);

    m_sender_object_selection_set_changed.Signal();
}

void MapEditor2::ObjectLayer::SelectObjectsByCircle (
    FloatVector2 const &center,
    Float const radius,
    MapEditor2::Object::SelectionOperation const selection_operation,
    bool const select_touching)
{
    GetMapEditorQuadTree()->SelectObjectsByCircle(
        center,
        radius,
        selection_operation,
        select_touching);

    UpdateObjectsAndEntitiesProperties();
    m_sender_object_selection_set_changed.Signal();
}

void MapEditor2::ObjectLayer::SelectAllObjects ()
{
    GetMapEditorQuadTree()->SelectAllObjects(false);

    UpdateObjectsAndEntitiesProperties();
    m_sender_object_selection_set_changed.Signal();
}

void MapEditor2::ObjectLayer::InvertObjectSelectionSet ()
{
    GetMapEditorQuadTree()->SelectAllObjects(true);

    UpdateObjectsAndEntitiesProperties();
    m_sender_object_selection_set_changed.Signal();
}

void MapEditor2::ObjectLayer::ClearObjectSelectionSet ()
{
    // deselect all objects in the object selection set
    ObjectSetIterator it = m_object_selection_set.begin();
    ObjectSetIterator it_end = m_object_selection_set.end();
    while (it != it_end)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        ASSERT1(object->GetIsSelected());
        ++it;
        object->SetIsSelected(false);
    }

    UpdateObjectsAndEntitiesProperties();
    m_sender_object_selection_set_changed.Signal();
}

void MapEditor2::ObjectLayer::ObjectSelectionSetClone (FloatVector2 const &position_offset)
{
    if (m_object_selection_set.size() == 0)
        return;

    // clone all selected objects and add them to an array
    Uint32 const set_size = m_object_selection_set.size();
#if defined(WIN32)
    Object **objects_to_clone = new Object *[set_size];
#else
    Object *objects_to_clone[set_size];
#endif
    Uint32 number_of_objects_to_clone = 0;

    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        ASSERT1(number_of_objects_to_clone < set_size);
        Engine2::Object *cloned_object = object->CreateClone();
        objects_to_clone[number_of_objects_to_clone] =
            dynamic_cast<Object *>(cloned_object);
        ASSERT1(objects_to_clone[number_of_objects_to_clone] != NULL);
        objects_to_clone[number_of_objects_to_clone]->Translate(position_offset);
        ++number_of_objects_to_clone;
    }
    ASSERT1(number_of_objects_to_clone == set_size);

    // clear the current object selection set
    ClearObjectSelectionSet();

    // add the cloned objects into the layer and select each
    for (Uint32 i = 0; i < number_of_objects_to_clone; ++i) {
        GetOwnerMapEditorWorld()->AddObject(objects_to_clone[i], this);
        objects_to_clone[i]->SetIsSelected(true);
    }

    m_sender_object_selection_set_changed.Signal();

#if defined(WIN32)
    Delete(objects_to_clone);
#endif
}

void MapEditor2::ObjectLayer::ObjectSelectionSetDelete ()
{
    if (m_object_selection_set.size() == 0)
        return;

    // make sure to clear the compound and vertex selection sets
    // so we don't have any dangling pointers
    ClearPolygonSelectionSet();
    ClearVertexSelectionSet();
        
    // store all the objects to be deleted into an array
    Uint32 const set_size = m_object_selection_set.size();
#if defined(WIN32)
    Object **objects_to_delete = new Object *[set_size];
#else
    Object *objects_to_delete[set_size];
#endif
    Uint32 number_of_objects_to_delete = 0;

    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        ASSERT1(number_of_objects_to_delete < set_size);
        objects_to_delete[number_of_objects_to_delete] = object;
        ++number_of_objects_to_delete;
    }
    ASSERT1(number_of_objects_to_delete == set_size);

    // clear the current object selection set
    ClearObjectSelectionSet();

    // delete the collected objects
    for (Uint32 i = 0; i < number_of_objects_to_delete; ++i) {
        GetOwnerMapEditorWorld()->RemoveObject(objects_to_delete[i]);
        Delete(objects_to_delete[i]);
    }

    UpdateObjectsAndEntitiesProperties();
    m_sender_object_selection_set_changed.Signal();

#if defined(WIN32)
    Delete(objects_to_delete);
#endif
}

void MapEditor2::ObjectLayer::ObjectSelectionSetTranslate (FloatVector2 const &translation_delta)
{
    if (translation_delta.GetIsZero())
        return;

    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        object->Translate(translation_delta);
        object->ReAddMapEditorObjectToQuadTree(Engine2::QTT_VISIBILITY);
    }

    UpdateObjectsAndEntitiesProperties();
}

void MapEditor2::ObjectLayer::ObjectSelectionSetScale (
    Float const scale_factor_delta,
    FloatVector2 const &transformation_origin,
    MapEditor2::Object::TransformationMode const transformation_mode)
{
    ASSERT1(scale_factor_delta > 0.0);

    if (scale_factor_delta == 1.0)
        return;

    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        object->ObjectSelectionSetScale(
            scale_factor_delta,
            transformation_origin,
            transformation_mode);
        object->ReAddMapEditorObjectToQuadTree(Engine2::QTT_VISIBILITY);
    }

    UpdateObjectsAndEntitiesProperties();
}

void MapEditor2::ObjectLayer::ObjectSelectionSetRotate (
    Float const angle_delta,
    FloatVector2 const &transformation_origin,
    MapEditor2::Object::TransformationMode const transformation_mode)
{
    if (angle_delta == 0.0)
        return;

    FloatMatrix2 rotation_transformation(FloatMatrix2::ms_identity);
    rotation_transformation.Rotate(angle_delta);

    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        object->ObjectSelectionSetRotate(
            angle_delta,
            transformation_origin,
            transformation_mode,
            rotation_transformation);
        object->ReAddMapEditorObjectToQuadTree(Engine2::QTT_VISIBILITY);
    }

    UpdateObjectsAndEntitiesProperties();
}

void MapEditor2::ObjectLayer::VertexSelectionSetTranslate (
    FloatVector2 const &translation_delta)
{
    if (translation_delta.GetIsZero())
        return;

    for (VertexSetIterator it = m_vertex_selection_set.begin(),
                           it_end = m_vertex_selection_set.end();
         it != it_end;
         ++it)
    {
        CompoundVertex *vertex = *it;
        ASSERT1(vertex != NULL);
        ASSERT1(vertex->m_owner_compound != NULL);
        vertex->m_owner_compound->TranslateVertex(vertex, translation_delta);
    }
}

void MapEditor2::ObjectLayer::VertexSelectionSetScale (
    Float const scale_factor_delta,
    FloatVector2 const &transformation_origin,
    MapEditor2::Object::TransformationMode const transformation_mode)
{
    if (scale_factor_delta == static_cast<Float>(1))
        return;

    for (VertexSetIterator it = m_vertex_selection_set.begin(),
                           it_end = m_vertex_selection_set.end();
         it != it_end;
         ++it)
    {
        CompoundVertex *vertex = *it;
        ASSERT1(vertex != NULL);
        ASSERT1(vertex->m_owner_compound != NULL);
        vertex->m_owner_compound->ScaleVertex(
            vertex,
            scale_factor_delta,
            transformation_origin);
    }
}

void MapEditor2::ObjectLayer::VertexSelectionSetRotate (
    Float const angle_delta,
    FloatVector2 const &transformation_origin,
    MapEditor2::Object::TransformationMode const transformation_mode)
{
    if (angle_delta == static_cast<Float>(0))
        return;

    FloatMatrix2 rotation_transformation(FloatMatrix2::ms_identity);
    rotation_transformation.Translate(-transformation_origin);
    rotation_transformation.Rotate(angle_delta);
    rotation_transformation.Translate(transformation_origin);
        
    for (VertexSetIterator it = m_vertex_selection_set.begin(),
                           it_end = m_vertex_selection_set.end();
         it != it_end;
         ++it)
    {
        CompoundVertex  *vertex = *it;
        ASSERT1(vertex != NULL);
        ASSERT1(vertex->m_owner_compound != NULL);
        vertex->m_owner_compound->RotateVertex(
            vertex,
            rotation_transformation,
            transformation_origin);
    }
}

void MapEditor2::ObjectLayer::ObjectSelectionSetAssignPerObjectTranslationX (
    Float const translation_x)
{
    FloatVector2 translation;
    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        translation = object->GetTranslation();
        translation[Dim::X] = translation_x;
        object->SetTranslation(translation);
    }

    UpdateObjectsAndEntitiesProperties();
}

void MapEditor2::ObjectLayer::ObjectSelectionSetAssignPerObjectTranslationY (
    Float const translation_y)
{
    FloatVector2 translation;
    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        translation = object->GetTranslation();
        translation[Dim::Y] = translation_y;
        object->SetTranslation(translation);
    }

    UpdateObjectsAndEntitiesProperties();
}

void MapEditor2::ObjectLayer::ObjectSelectionSetAssignPerObjectTranslation (
    FloatVector2 const &translation)
{
    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        object->SetTranslation(translation);
    }

    UpdateObjectsAndEntitiesProperties();
}

void MapEditor2::ObjectLayer::ObjectSelectionSetAssignPerObjectScale (Float const scale_factor)
{
    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        object->SetScaleFactor(scale_factor);
    }

    UpdateObjectsAndEntitiesProperties();
}

void MapEditor2::ObjectLayer::ObjectSelectionSetAssignPerObjectRotation (Float const angle)
{
    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        object->SetAngle(angle);
    }
}

void MapEditor2::ObjectLayer::ObjectSelectionSetAssignPerEntityFirstMoment (
    Float const first_moment)
{
    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        Entity *entity = dynamic_cast<Entity *>(object);
        if (entity != NULL)
            entity->SetFirstMoment(first_moment);
    }

    UpdateObjectsAndEntitiesProperties();
}

void MapEditor2::ObjectLayer::ObjectSelectionSetAssignPerEntityVelocity (
    FloatVector2 const &velocity)
{
    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        Entity *entity = dynamic_cast<Entity *>(object);
        if (entity != NULL)
            entity->SetVelocity(velocity);
    }

    UpdateObjectsAndEntitiesProperties();
}

void MapEditor2::ObjectLayer::ObjectSelectionSetAssignPerEntityVelocityX (
    Float const velocity_x)
{
    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        Entity *entity = dynamic_cast<Entity *>(object);
        if (entity != NULL)
            entity->SetVelocityComponent(Dim::X, velocity_x);
    }

    UpdateObjectsAndEntitiesProperties();
}

void MapEditor2::ObjectLayer::ObjectSelectionSetAssignPerEntityVelocityY (
    Float const velocity_y)
{
    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        Entity *entity = dynamic_cast<Entity *>(object);
        if (entity != NULL)
            entity->SetVelocityComponent(Dim::Y, velocity_y);
    }

    UpdateObjectsAndEntitiesProperties();
}

void MapEditor2::ObjectLayer::ObjectSelectionSetAssignPerEntitySpeed (
    Float const speed)
{
    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        Entity *entity = dynamic_cast<Entity *>(object);
        if (entity != NULL && !entity->GetVelocity().GetIsZero())
            entity->SetVelocity(speed * entity->GetVelocity().GetNormalization());
    }

    UpdateObjectsAndEntitiesProperties();
}

void MapEditor2::ObjectLayer::ObjectSelectionSetAssignPerEntityVelocityAngle (
    Float const velocity_angle)
{
    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        Entity *entity = dynamic_cast<Entity *>(object);
        if (entity != NULL)
            entity->SetVelocity(
                entity->GetVelocity().GetLength() *
                FloatVector2(
                    Math::Cos(velocity_angle),
                    Math::Sin(velocity_angle)));
    }

    UpdateObjectsAndEntitiesProperties();
}

void MapEditor2::ObjectLayer::ObjectSelectionSetAssignPerEntitySecondMoment (
    Float const second_moment)
{
    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        Entity *entity = dynamic_cast<Entity *>(object);
        if (entity != NULL)
            entity->SetSecondMoment(second_moment);
    }

    UpdateObjectsAndEntitiesProperties();
}

void MapEditor2::ObjectLayer::ObjectSelectionSetAssignPerEntityAngularVelocity (
    Float const angular_velocity)
{
    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        Entity *entity = dynamic_cast<Entity *>(object);
        if (entity != NULL)
            entity->SetAngularVelocity(angular_velocity);
    }

    UpdateObjectsAndEntitiesProperties();
}

void MapEditor2::ObjectLayer::ObjectSelectionSetAssignPerEntityElasticity (
    Float const elasticity)
{
    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        Entity *entity = dynamic_cast<Entity *>(object);
        if (entity != NULL)
            entity->SetElasticity(elasticity);
    }

    UpdateObjectsAndEntitiesProperties();
}

void MapEditor2::ObjectLayer::ObjectSelectionSetAssignPerEntityDensity (
    Float const density)
{
    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        Entity *entity = dynamic_cast<Entity *>(object);

        // temp hack for now (until real area computation is done
        // temp hack for now (until real area computation is done
        // temp hack for now (until real area computation is done
        // temp hack for now (until real area computation is done
        // temp hack for now (until real area computation is done
        // temp hack for now (until real area computation is done

        if (entity != NULL)
            entity->SetFirstMoment(
                density * static_cast<Float>(M_PI) * entity->GetVisibleRadius() * entity->GetVisibleRadius());
    }

    UpdateObjectsAndEntitiesProperties();
}

void MapEditor2::ObjectLayer::ObjectSelectionSetAssignPerEntityAppliesGravity (
    bool const applies_gravity)
{
    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        Entity *entity = dynamic_cast<Entity *>(object);
        if (entity != NULL)
        {
            GetOwnerMapEditorWorld()->RemoveEntity(entity);
            entity->SetAppliesGravity(applies_gravity);
            GetOwnerMapEditorWorld()->AddEntity(entity, this);
        }
    }
}

void MapEditor2::ObjectLayer::ObjectSelectionSetAssignPerEntityReactsToGravity (
    bool const reacts_to_gravity)
{
    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        Entity *entity = dynamic_cast<Entity *>(object);
        if (entity != NULL)
        {
            GetOwnerMapEditorWorld()->RemoveEntity(entity);
            entity->SetReactsToGravity(reacts_to_gravity);
            GetOwnerMapEditorWorld()->AddEntity(entity, this);
        }
    }
}

void MapEditor2::ObjectLayer::ObjectSelectionSetScaleFirstMoment (
    Float const first_moment_scale_factor)
{
    ASSERT1(first_moment_scale_factor > 0.0f);

    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        Entity *entity = dynamic_cast<Entity *>(object);

        if (entity != NULL)
            entity->SetFirstMoment(
                first_moment_scale_factor * entity->GetFirstMoment());
    }

    UpdateObjectsAndEntitiesProperties();
}

void MapEditor2::ObjectLayer::ObjectSelectionSetTranslateVelocity (
    FloatVector2 const &translation_delta)
{
    if (translation_delta.GetIsZero())
        return;

    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        Entity *entity = dynamic_cast<Entity *>(object);
        if (entity != NULL)
            entity->AccumulateVelocity(translation_delta);
    }
}

void MapEditor2::ObjectLayer::ObjectSelectionSetScaleVelocity (
    Float const scale_factor_delta,
    FloatVector2 const &transformation_origin,
    MapEditor2::Object::TransformationMode const transformation_mode)
{
    if (scale_factor_delta == 1.0)
        return;

    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        Entity *entity = dynamic_cast<Entity *>(object);
        if (entity != NULL)
            entity->ObjectSelectionSetScaleVelocity(
                scale_factor_delta,
                transformation_origin,
                transformation_mode);
    }
}

void MapEditor2::ObjectLayer::ObjectSelectionSetRotateVelocity (
    Float const angle_delta,
    FloatVector2 const &transformation_origin,
    MapEditor2::Object::TransformationMode const transformation_mode)
{
    if (angle_delta == 0.0)
        return;

    FloatMatrix2 rotation_transformation(FloatMatrix2::ms_identity);
    rotation_transformation.Rotate(angle_delta);

    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        Entity *entity = dynamic_cast<Entity *>(object);
        if (entity != NULL)
            entity->ObjectSelectionSetRotateVelocity(
                angle_delta,
                transformation_origin,
                transformation_mode,
                rotation_transformation);
    }
}

void MapEditor2::ObjectLayer::ObjectSelectionSetAccumulateAngularVelocity (
    Float const angle_delta)
{
    if (angle_delta == 0.0)
        return;

    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        Entity *entity = dynamic_cast<Entity *>(object);
        if (entity != NULL)
            entity->AccumulateAngularVelocity(angle_delta);
    }
}

void MapEditor2::ObjectLayer::ObjectSelectionSetScaleAngularVelocity (
    Float const scale_factor_delta)
{
    if (scale_factor_delta == 1.0)
        return;

    for (ObjectSetIterator it = m_object_selection_set.begin(),
                           it_end = m_object_selection_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        Entity *entity = dynamic_cast<Entity *>(object);
        if (entity != NULL)
            entity->SetAngularVelocity(
                scale_factor_delta * entity->GetAngularVelocity());
    }
}

void MapEditor2::ObjectLayer::ForceObjectSelectionSetSignals ()
{
    UpdateObjectsAndEntitiesProperties();

    m_sender_object_selection_set_origin_changed.Signal(
        m_object_selection_set_origin);
    m_sender_object_selection_set_first_moment_changed.Signal(
        m_object_selection_set_first_moment);
    m_sender_object_selection_set_velocity_changed.Signal(
        m_object_selection_set_velocity);
    m_sender_object_selection_set_second_moment_changed.Signal(
        m_object_selection_set_second_moment);
    m_sender_object_selection_set_angular_velocity_changed.Signal(
        m_object_selection_set_angular_velocity);
    m_sender_object_selection_set_elasticity_changed.Signal(
        m_object_selection_set_elasticity);
    m_sender_object_selection_set_density_changed.Signal(
        m_object_selection_set_density);
}

// ///////////////////////////////////////////////////////////////////////////
// vertex selection set public functions
// ///////////////////////////////////////////////////////////////////////////

void MapEditor2::ObjectLayer::SelectVerticesByCircle (
    FloatVector2 const &center,
    Float const radius,
    MapEditor2::Object::SelectionOperation const selection_operation)
{
    GetMapEditorQuadTree()->SelectVerticesByCircle(
        center,
        radius,
        selection_operation);
}

void MapEditor2::ObjectLayer::SelectNearestVertex (
    FloatVector2 const &center,
    Float const radius,
    MapEditor2::Object::SelectionOperation const selection_operation)
{
    Compound *compound_containing_nearest;
    CompoundVertex *nearest_vertex;
    GetMapEditorQuadTree()->
        ComputeNearestVertex(
            center,
            radius,
            &compound_containing_nearest,
            &nearest_vertex);

    if (compound_containing_nearest != NULL)
        compound_containing_nearest->ApplyVertexSelectionOperation(
            nearest_vertex,
            selection_operation);
}

void MapEditor2::ObjectLayer::SelectAllVertices (bool const mask_by_object_selection_set)
{
    GetMapEditorQuadTree()->SelectAllVertices(false, mask_by_object_selection_set);
}

void MapEditor2::ObjectLayer::InvertVertexSelectionSet (bool const mask_by_object_selection_set)
{
    GetMapEditorQuadTree()->SelectAllVertices(true, mask_by_object_selection_set);
}

void MapEditor2::ObjectLayer::ClearVertexSelectionSet ()
{
    // deselect all vertices in the vertex selection set
    for (VertexSetIterator it = m_vertex_selection_set.begin(),
                           it_end = m_vertex_selection_set.end();
         it != it_end;
         ++it)
    {
        CompoundVertex *vertex = *it;
        ASSERT1(vertex != NULL);
        ASSERT1(vertex->m_is_selected);
        vertex->m_is_selected = false;
        --m_selected_vertex_count;
    }
    m_vertex_selection_set.clear();
    ASSERT1(m_selected_vertex_count == 0);
}

void MapEditor2::ObjectLayer::MaskSelectedVerticesByObjectSelectionSet ()
{
    for (VertexSetIterator it = m_vertex_selection_set.begin(),
                           it_end = m_vertex_selection_set.end();
         it != it_end;
         ++it)
    {
        CompoundVertex *vertex = *it;
        ASSERT1(vertex != NULL);
        ASSERT1(vertex->m_is_selected);
        ASSERT1(vertex->m_owner_compound != NULL);
        if (!vertex->m_owner_compound->GetIsSelected())
            RemoveVertexFromVertexSelectionSet(vertex);
    }
}

void MapEditor2::ObjectLayer::UnweldSelectedVertices ()
{
    if (GetSelectedVertexCount() == 0)
        return;

    // create a unique set of all the compounds that have selected vertices
    std::set<Compound *> compounds_with_selected_vertices_set;
    std::set<Compound *>::iterator compounds_set_iterator_end =
        compounds_with_selected_vertices_set.end();
    
    for (VertexSetIterator it = m_vertex_selection_set.begin(),
                           it_end = m_vertex_selection_set.end();
         it != it_end;
         ++it)
    {
        CompoundVertex *vertex = *it;
        ASSERT1(vertex != NULL);
        ASSERT1(vertex->m_owner_compound != NULL);
        std::set<Compound *>::iterator compounds_set_iterator =
            compounds_with_selected_vertices_set.find(vertex->m_owner_compound);
        if (compounds_set_iterator == compounds_set_iterator_end)
            compounds_with_selected_vertices_set.insert(vertex->m_owner_compound);
    }

    // the set has been created.  now call UnweldSelectedVertices on each
    for (std::set<Compound *>::iterator it = compounds_with_selected_vertices_set.begin();
         it != compounds_set_iterator_end;
         ++it)
    {
        Compound *compound = *it;
        ASSERT1(compound != NULL);
        compound->UnweldSelectedVertices();
    }
}

// ///////////////////////////////////////////////////////////////////////////
// polygon selection set public functions
// ///////////////////////////////////////////////////////////////////////////

void MapEditor2::ObjectLayer::SelectSmallestPolygonTouchingPoint (
    FloatVector2 const &point,
    Object::SelectionOperation const selection_operation)
{
    bool mask_by_object_selection_set = GetSelectedObjectCount() > 0;
    Polygon *polygon =
        GetMapEditorQuadTree()->
            GetSmallestMapEditorPolygonTouchingPoint(
                point,
                mask_by_object_selection_set);
    if (polygon != NULL)
    {
        switch (selection_operation)
        {
            case Object::SO_EQUALS: polygon->SetIsSelected(true, this);                    break;
            case Object::SO_IOR:    polygon->SetIsSelected(true, this);                    break;
            case Object::SO_MINUS:  polygon->SetIsSelected(false, this);                   break;
            case Object::SO_XOR:    polygon->SetIsSelected(!polygon->m_is_selected, this); break;
            case Object::SO_AND:                                                           break;
            default:        ASSERT1(false && "Invalid selection operation");               break;
        }
    }
}

void MapEditor2::ObjectLayer::SelectAllPolygons (bool const mask_by_object_selection_set)
{
    GetMapEditorQuadTree()->SelectAllPolygons(false, mask_by_object_selection_set);
}

void MapEditor2::ObjectLayer::InvertPolygonSelectionSet (bool const mask_by_object_selection_set)
{
    GetMapEditorQuadTree()->SelectAllPolygons(true, mask_by_object_selection_set);
}

void MapEditor2::ObjectLayer::ClearPolygonSelectionSet ()
{
    // deselect all polygons in the polygon selection set
    for (PolygonSetIterator it = m_polygon_selection_set.begin(),
                            it_end = m_polygon_selection_set.end();
         it != it_end;
         ++it)
    {
        Polygon *polygon = *it;
        ASSERT1(polygon != NULL);
        ASSERT1(polygon->m_is_selected);
        polygon->m_is_selected = false;
        polygon->DecrementSelectedOwnerPolygonCount(this);
        --m_selected_polygon_count;
    }
    m_polygon_selection_set.clear();
    ASSERT1(m_selected_polygon_count == 0);
}

void MapEditor2::ObjectLayer::MaskSelectedPolygonsByObjectSelectionSet ()
{
    for (PolygonSetIterator it = m_polygon_selection_set.begin(),
                            it_end = m_polygon_selection_set.end();
         it != it_end;
         ++it)
    {
        Polygon *polygon = *it;
        ASSERT1(polygon != NULL);
        ASSERT1(polygon->m_is_selected);
        ASSERT1(polygon->m_owner_compound != NULL);
        if (!polygon->m_owner_compound->GetIsSelected())
            RemovePolygonFromPolygonSelectionSet(polygon);
    }
}

void MapEditor2::ObjectLayer::SetVertexSelectionStateFromSelectionOwnerPolygonCount ()
{
    GetMapEditorQuadTree()->SetVertexSelectionStateFromSelectionOwnerPolygonCount();
}

void MapEditor2::ObjectLayer::UnweldSelectedPolygons ()
{
    if (GetSelectedPolygonCount() == 0)
        return;

    // create a unique set of all the compounds that have selected polygons
    std::set<Compound *> compounds_with_selected_polygons_set;
    std::set<Compound *>::iterator compounds_set_iterator_end =
        compounds_with_selected_polygons_set.end();
    
    for (PolygonSetIterator it = m_polygon_selection_set.begin(),
                            it_end = m_polygon_selection_set.end();
         it != it_end;
         ++it)
    {
        Polygon *polygon = *it;
        ASSERT1(polygon != NULL);
        ASSERT1(polygon->m_owner_compound != NULL);
        std::set<Compound *>::iterator compounds_set_iterator =
            compounds_with_selected_polygons_set.find(polygon->m_owner_compound);
        if (compounds_set_iterator == compounds_set_iterator_end)
            compounds_with_selected_polygons_set.insert(polygon->m_owner_compound);
    }

    // the set has been created.  now call UnweldSelectedPolygons on each
    for (std::set<Compound *>::iterator it = compounds_with_selected_polygons_set.begin();
         it != compounds_set_iterator_end;
         ++it)
    {
        Compound *compound = *it;
        ASSERT1(compound != NULL);
        compound->UnweldSelectedPolygons();
    }
}

void MapEditor2::ObjectLayer::DeleteSelectedPolygons ()
{
    if (GetSelectedPolygonCount() == 0)
        return;

    // create a unique set of all the compounds that have selected polygons
    std::set<Compound *> compounds_with_selected_polygons_set;
    std::set<Compound *>::iterator compounds_set_iterator_end =
        compounds_with_selected_polygons_set.end();
    
    for (PolygonSetIterator it = m_polygon_selection_set.begin(),
                            it_end = m_polygon_selection_set.end();
         it != it_end;
         ++it)
    {
        Polygon *polygon = *it;
        ASSERT1(polygon != NULL);
        ASSERT1(polygon->m_owner_compound != NULL);
        std::set<Compound *>::iterator compounds_set_iterator =
            compounds_with_selected_polygons_set.find(polygon->m_owner_compound);
        if (compounds_set_iterator == compounds_set_iterator_end)
            compounds_with_selected_polygons_set.insert(polygon->m_owner_compound);
    }

    // the set has been created.  check if the deletions will result in
    // at least one compound being left with zero polygons.  if they do,
    // then disallow the operation and early-out.
    for (std::set<Compound *>::iterator it = compounds_with_selected_polygons_set.begin();
         it != compounds_set_iterator_end;
         ++it)
    {
        Compound *compound = *it;
        ASSERT1(compound != NULL);

        ASSERT1(compound->GetSelectedPolygonCount() <= compound->GetPolygonCount());
        if (compound->GetSelectedPolygonCount() == compound->GetPolygonCount())
            return;
    }

    // now call DeleteSelectedPolygons on each compound in the constructed set
    for (std::set<Compound *>::iterator it = compounds_with_selected_polygons_set.begin();
         it != compounds_set_iterator_end;
         ++it)
    {
        Compound *compound = *it;
        ASSERT1(compound != NULL);
        
        compound->DeleteSelectedPolygons();
    }

    // the selected polygons will have removed themselves from the
    // polygon selection set.
    ASSERT1(GetSelectedPolygonCount() == 0);
}

// ///////////////////////////////////////////////////////////////////////////
// object selection set private functions
// ///////////////////////////////////////////////////////////////////////////

void MapEditor2::ObjectLayer::SetSelectedObjectCount (
    Uint32 const selected_object_count)
{
    if (m_selected_object_count != selected_object_count)
    {
        m_selected_object_count = selected_object_count;
        m_sender_selected_object_count_changed.Signal(m_selected_object_count);
    }
}

void MapEditor2::ObjectLayer::SetObjectSelectionSetOrigin (
    FloatVector2 const &object_selection_set_origin)
{
    if (m_object_selection_set_origin != object_selection_set_origin)
    {
        m_object_selection_set_origin = object_selection_set_origin;
        m_sender_object_selection_set_origin_changed.Signal(m_object_selection_set_origin);
    }
}

void MapEditor2::ObjectLayer::SetSelectedEntityCount (
    Uint32 const selected_entity_count)
{
    if (m_selected_entity_count != selected_entity_count)
    {
        m_selected_entity_count = selected_entity_count;
        m_sender_selected_entity_count_changed.Signal(m_selected_entity_count);
    }
}

void MapEditor2::ObjectLayer::SetObjectSelectionSetFirstMoment (
    Float const object_selection_set_first_moment)
{
    if (m_object_selection_set_first_moment != object_selection_set_first_moment)
    {
        m_object_selection_set_first_moment = object_selection_set_first_moment;
        m_sender_object_selection_set_first_moment_changed.Signal(m_object_selection_set_first_moment);
    }
}

void MapEditor2::ObjectLayer::SetObjectSelectionSetCenterOfGravity (
    FloatVector2 const &object_selection_set_center_of_gravity)
{
    if (m_object_selection_set_center_of_gravity != object_selection_set_center_of_gravity)
    {
        m_object_selection_set_center_of_gravity = object_selection_set_center_of_gravity;
        m_sender_object_selection_set_center_of_gravity_changed.Signal(m_object_selection_set_center_of_gravity);
    }
}

bool MapEditor2::ObjectLayer::AddObjectToObjectSelectionSet (MapEditor2::Object *const object)
{
    ASSERT1(object != NULL);
    ASSERT1(object->GetOwnerQuadTree(Engine2::QTT_VISIBILITY)->GetRootNode() == GetQuadTree());
    ASSERT1(object->GetObjectLayer() == this);
    ASSERT1(!object->GetIsSelected());

    // put the object in the object selection set
    m_object_selection_set.insert(object);
    // quick recalculation of the number of selected objects and the
    // object selection set origin
    Uint32 selected_object_count = m_selected_object_count;
    FloatVector2 object_selection_set_origin(m_object_selection_set_origin);
    FloatVector2 object_selection_set_center_of_gravity(m_object_selection_set_center_of_gravity);

    Uint32 selected_entity_count = m_selected_entity_count;
    Float object_selection_set_first_moment = m_object_selection_set_first_moment;

    Uint32 selected_compound_count = m_selected_compound_count;
    
    object_selection_set_origin *= (Float)selected_object_count;
    object_selection_set_origin += object->GetTranslation();
    ++selected_object_count;
    object_selection_set_origin /= (Float)selected_object_count;

    Entity *entity = dynamic_cast<Entity *>(object);
    if (entity != NULL)
    {
        object_selection_set_center_of_gravity =
            (m_object_selection_set_center_of_gravity * object_selection_set_first_moment +
             entity->GetTranslation() * entity->GetFirstMoment())
            /
            (object_selection_set_first_moment + entity->GetFirstMoment());

        ++selected_entity_count;
        object_selection_set_first_moment += entity->GetFirstMoment();
    }

    Compound *compound = dynamic_cast<Compound *>(object);
    if (compound != NULL)
        ++selected_compound_count;
    
    // set the calculated values
    SetSelectedObjectCount(selected_object_count);
    SetObjectSelectionSetOrigin(object_selection_set_origin);

    SetSelectedEntityCount(selected_entity_count);
    SetObjectSelectionSetFirstMoment(object_selection_set_first_moment);
    SetObjectSelectionSetCenterOfGravity(object_selection_set_center_of_gravity);

    SetSelectedCompoundCount(selected_compound_count);
    
    UpdateObjectSelectionSetVelocity();
    UpdateObjectSelectionSetSecondMoment();
    UpdateObjectSelectionSetAngularVelocity();
    UpdateObjectSelectionSetElasticity();
    UpdateObjectSelectionSetDensity();

    // return with success
    return true;
}

bool MapEditor2::ObjectLayer::RemoveObjectFromObjectSelectionSet (MapEditor2::Object *const object)
{
    ASSERT1(object != NULL);
    ASSERT1(object->GetOwnerQuadTree(Engine2::QTT_VISIBILITY)->GetRootNode() == GetQuadTree());
    ASSERT1(object->GetObjectLayer() == this);
    ASSERT1(object->GetIsSelected());

    // find the object
    ObjectSetIterator it = m_object_selection_set.find(object);
    // if it wasn't found, return with failure
    if (it == m_object_selection_set.end())
        return false;

    // remove this found object
    m_object_selection_set.erase(it);
    // quick recalculation of the number of selected objects and the
    // object selection set origin
    Uint32 selected_object_count = m_selected_object_count;
    FloatVector2 object_selection_set_origin(m_object_selection_set_origin);
    FloatVector2 object_selection_set_center_of_gravity(m_object_selection_set_center_of_gravity);

    Uint32 selected_entity_count = m_selected_entity_count;
    Float object_selection_set_first_moment = m_object_selection_set_first_moment;

    Uint32 selected_compound_count = m_selected_compound_count;

    // to avoid a divide by zero
    if (selected_object_count > 1)
    {
        object_selection_set_origin *= (Float)selected_object_count;
        object_selection_set_origin -= object->GetTranslation();
        --selected_object_count;
        object_selection_set_origin /= (Float)selected_object_count;

        Entity *entity = dynamic_cast<Entity *>(object);
        if (entity != NULL)
        {
            object_selection_set_center_of_gravity =
                (m_object_selection_set_center_of_gravity * object_selection_set_first_moment -
                entity->GetTranslation() * entity->GetFirstMoment())
                /
                (object_selection_set_first_moment + entity->GetFirstMoment());

            --selected_entity_count;
            object_selection_set_first_moment -= entity->GetFirstMoment();
        }

        Compound *compound = dynamic_cast<Compound *>(object);
        if (compound != NULL)
            --selected_compound_count;
    }
    else
    {
        selected_object_count = 0;
        object_selection_set_origin = FloatVector2::ms_zero;

        selected_entity_count = 0;
        object_selection_set_first_moment = 0.0;
        object_selection_set_center_of_gravity = FloatVector2::ms_zero;

        selected_compound_count = 0;
    }

    // set the calculated values
    SetSelectedObjectCount(selected_object_count);
    SetObjectSelectionSetOrigin(object_selection_set_origin);

    SetSelectedEntityCount(selected_entity_count);
    SetObjectSelectionSetFirstMoment(object_selection_set_first_moment);
    SetObjectSelectionSetCenterOfGravity(object_selection_set_center_of_gravity);

    SetSelectedCompoundCount(selected_compound_count);
    
    UpdateObjectSelectionSetVelocity();
    UpdateObjectSelectionSetSecondMoment();
    UpdateObjectSelectionSetAngularVelocity();
    UpdateObjectSelectionSetElasticity();
    UpdateObjectSelectionSetDensity();

    // return with success
    return true;
}

void MapEditor2::ObjectLayer::UpdateObjectsAndEntitiesProperties ()
{
    Uint32 selected_object_count = 0;
    FloatVector2 object_selection_set_origin(FloatVector2::ms_zero);

    Uint32 selected_entity_count = 0;
    Float object_selection_set_first_moment = 0.0;
    FloatVector2 object_selection_set_center_of_gravity = FloatVector2::ms_zero;

    Uint32 selected_compound_count = 0;

    if (!GetIsObjectSelectionSetEmpty())
    {
        // return the algebraic average of all the selected
        // objects' origins
        for (ObjectSetIterator it = m_object_selection_set.begin(),
                               it_end = m_object_selection_set.end();
             it != it_end;
             ++it)
        {
            Object *object = *it;
            ASSERT1(object != NULL);

            object_selection_set_origin += object->GetTranslation();
            ++selected_object_count;

            Entity *entity = dynamic_cast<Entity *>(object);
            if (entity != NULL)
            {
                object_selection_set_center_of_gravity +=
                    entity->GetTranslation() * entity->GetFirstMoment();

                ++selected_entity_count;
                object_selection_set_first_moment += entity->GetFirstMoment();
            }

            Compound *compound = dynamic_cast<Compound *>(object);
            if (compound != NULL)
                ++selected_compound_count;
        }

        object_selection_set_origin /= static_cast<Float>(selected_object_count);
        if (selected_entity_count > 0)
        {
            ASSERT1(object_selection_set_first_moment > 0.0f);
            object_selection_set_center_of_gravity /= object_selection_set_first_moment;
        }
    }

    SetSelectedObjectCount(selected_object_count);
    SetObjectSelectionSetOrigin(object_selection_set_origin);

    SetSelectedEntityCount(selected_entity_count);
    SetObjectSelectionSetFirstMoment(object_selection_set_first_moment);
    SetObjectSelectionSetCenterOfGravity(object_selection_set_center_of_gravity);

    SetSelectedCompoundCount(selected_compound_count);
    
    UpdateObjectSelectionSetVelocity();
    UpdateObjectSelectionSetSecondMoment();
    UpdateObjectSelectionSetAngularVelocity();
    UpdateObjectSelectionSetElasticity();
    UpdateObjectSelectionSetDensity();
}

void MapEditor2::ObjectLayer::UpdateObjectSelectionSetVelocity ()
{
    if (GetSelectedEntityCount() != 1)
        return;

    FloatVector2 object_selection_set_velocity = GetSingleSelectedEntity()->GetVelocity();
    if (m_object_selection_set_velocity != object_selection_set_velocity)
    {
        m_object_selection_set_velocity = object_selection_set_velocity;
        m_sender_object_selection_set_velocity_changed.Signal(m_object_selection_set_velocity);
    }
}

void MapEditor2::ObjectLayer::UpdateObjectSelectionSetSecondMoment ()
{
    if (GetSelectedEntityCount() != 1)
        return;

    Float object_selection_set_second_moment =
        GetSingleSelectedEntity()->GetSecondMoment();

    if (m_object_selection_set_second_moment != object_selection_set_second_moment)
    {
        m_object_selection_set_second_moment = object_selection_set_second_moment;
        m_sender_object_selection_set_second_moment_changed.Signal(
            m_object_selection_set_second_moment);
    }
}

void MapEditor2::ObjectLayer::UpdateObjectSelectionSetAngularVelocity ()
{
    if (GetSelectedEntityCount() != 1)
        return;

    Float object_selection_set_angular_velocity = GetSingleSelectedEntity()->GetAngularVelocity();
    if (m_object_selection_set_angular_velocity != object_selection_set_angular_velocity)
    {
        m_object_selection_set_angular_velocity = object_selection_set_angular_velocity;
        m_sender_object_selection_set_angular_velocity_changed.Signal(m_object_selection_set_angular_velocity);
    }
}

void MapEditor2::ObjectLayer::UpdateObjectSelectionSetElasticity ()
{
    if (GetSelectedEntityCount() != 1)
        return;

    Float object_selection_set_elasticity = GetSingleSelectedEntity()->GetElasticity();
    if (m_object_selection_set_elasticity != object_selection_set_elasticity)
    {
        m_object_selection_set_elasticity = object_selection_set_elasticity;
        m_sender_object_selection_set_elasticity_changed.Signal(m_object_selection_set_elasticity);
    }
}

void MapEditor2::ObjectLayer::UpdateObjectSelectionSetDensity ()
{
    if (GetSelectedEntityCount() != 1)
        return;

    // temp hack until real density stuff is done
    // temp hack until real density stuff is done
    // temp hack until real density stuff is done
    // temp hack until real density stuff is done
    // temp hack until real density stuff is done
    // temp hack until real density stuff is done
    // temp hack until real density stuff is done
    // temp hack until real density stuff is done
    // temp hack until real density stuff is done
    // temp hack until real density stuff is done

    Entity *entity = GetSingleSelectedEntity();
    Float object_selection_set_density =
        entity->GetFirstMoment() /
        static_cast<Float>(M_PI) * entity->GetVisibleRadius() * entity->GetVisibleRadius();
    if (m_object_selection_set_density != object_selection_set_density)
    {
        m_object_selection_set_density = object_selection_set_density;
        m_sender_object_selection_set_density_changed.Signal(m_object_selection_set_density);
    }
}

// ///////////////////////////////////////////////////////////////////////////
// compound selection set private functions
// ///////////////////////////////////////////////////////////////////////////

void MapEditor2::ObjectLayer::SetSelectedCompoundCount (
    Uint32 const selected_compound_count)
{
    if (m_selected_compound_count != selected_compound_count)
    {
        m_selected_compound_count = selected_compound_count;
        m_sender_selected_compound_count_changed.Signal(m_selected_compound_count);
    }
}

// ///////////////////////////////////////////////////////////////////////////
// vertex selection set private functions
// ///////////////////////////////////////////////////////////////////////////

void MapEditor2::ObjectLayer::SetSelectedVertexCount (
    Uint32 const selected_vertex_count)
{
    if (m_selected_vertex_count != selected_vertex_count)
    {
        m_selected_vertex_count = selected_vertex_count;
        m_sender_selected_vertex_count_changed.Signal(m_selected_vertex_count);
    }
}

void MapEditor2::ObjectLayer::AddVertexToVertexSelectionSet (
    MapEditor2::CompoundVertex *const vertex)
{
    ASSERT1(vertex != NULL);
    ASSERT1(!vertex->m_is_selected);
    ASSERT1(m_vertex_selection_set.find(vertex) == m_vertex_selection_set.end());
    vertex->m_is_selected = true;
    m_vertex_selection_set.insert(vertex);
    ASSERT1(m_selected_vertex_count < UINT32_UPPER_BOUND);
    SetSelectedVertexCount(m_selected_vertex_count + 1);
}

void MapEditor2::ObjectLayer::RemoveVertexFromVertexSelectionSet (
    MapEditor2::CompoundVertex *const vertex)
{
    ASSERT1(vertex != NULL);
    ASSERT1(vertex->m_is_selected);
    VertexSetIterator it = m_vertex_selection_set.find(vertex);
    ASSERT1(it != m_vertex_selection_set.end());
    m_vertex_selection_set.erase(it);
    vertex->m_is_selected = false;
    ASSERT1(m_selected_vertex_count > 0);
    SetSelectedVertexCount(m_selected_vertex_count - 1);
}

// ///////////////////////////////////////////////////////////////////////////
// polygon selection set private functions
// ///////////////////////////////////////////////////////////////////////////

void MapEditor2::ObjectLayer::SetSelectedPolygonCount (
    Uint32 const selected_polygon_count)
{
    if (m_selected_polygon_count != selected_polygon_count)
    {
        m_selected_polygon_count = selected_polygon_count;
        m_sender_selected_polygon_count_changed.Signal(m_selected_polygon_count);
    }
}

void MapEditor2::ObjectLayer::AddPolygonToPolygonSelectionSet (
    MapEditor2::Polygon *const polygon)
{
    ASSERT1(polygon != NULL);
    ASSERT1(!polygon->m_is_selected);
    ASSERT1(m_polygon_selection_set.find(polygon) == m_polygon_selection_set.end());
    polygon->m_is_selected = true;
    polygon->IncrementSelectedOwnerPolygonCount(this);
    m_polygon_selection_set.insert(polygon);
    ASSERT1(m_selected_polygon_count < UINT32_UPPER_BOUND);
    SetSelectedPolygonCount(m_selected_polygon_count + 1);
}

void MapEditor2::ObjectLayer::RemovePolygonFromPolygonSelectionSet (
    MapEditor2::Polygon *const polygon)
{
    ASSERT1(polygon != NULL);
    ASSERT1(polygon->m_is_selected);
    PolygonSetIterator it = m_polygon_selection_set.find(polygon);
    ASSERT1(it != m_polygon_selection_set.end());
    m_polygon_selection_set.erase(it);
    polygon->m_is_selected = false;
    polygon->DecrementSelectedOwnerPolygonCount(this);
    ASSERT1(m_selected_polygon_count > 0);
    SetSelectedPolygonCount(m_selected_polygon_count - 1);
}

} // end of namespace Xrb
