// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_world.cpp by Victor Dods, created 2004/06/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_world.hpp"

#include <algorithm>

#include "xrb_engine2_events.hpp"
#include "xrb_engine2_objectlayer.hpp"
#include "xrb_engine2_physicshandler.hpp"
#include "xrb_engine2_visibilityquadtree.hpp"
#include "xrb_engine2_worldview.hpp"
#include "xrb_eventqueue.hpp"
#include "xrb_serializer.hpp"

namespace Xrb {
namespace Engine2 {

World::~World ()
{
    ASSERT1(m_world_view_list.empty() && "all WorldView objects must be detached before destroying World");

    // delete the Entity/Object pairs (dynamic objects).  this will
    // handle removing them from the physics handler as well.
    for (EntityWorldIndex i = 0;
         i < static_cast<EntityWorldIndex>(m_entity_vector.size());
         ++i)
    {
        Entity *entity = m_entity_vector[i];
        if (entity != NULL)
        {
            RemoveDynamicObject(entity->OwnerObject());
            delete entity->OwnerObject();
        }
    }
    ASSERT1(m_entity_count == 0);

    // all entities have been removed, we can now delete the physics handler
    delete m_physics_handler;

    // delete the ObjectLayers, which will delete the static objects directly.
    for (ObjectLayerList::iterator it = m_object_layer_list.begin(),
                                 it_end = m_object_layer_list.end();
         it != it_end;
         ++it)
    {
        ObjectLayer *object_layer = *it;
        ASSERT1(object_layer != NULL);
        Delete(object_layer);
    }
    m_object_layer_list.clear();

    // we have to un-set the owner EventQueue, because EventHandler
    // tries to use it in its destructor (which would be bad, because
    // it goes bye-bye at the end of this destructor).
    SetOwnerEventQueue(NULL);
}

World *World::Create (
    Serializer &serializer,
    CreateEntityFunction CreateEntity,
    PhysicsHandler *const physics_handler)
{
    ASSERT1(serializer.Direction() == IOD_READ);

    Uint32 entity_capacity;
    serializer.Read<Uint32>(entity_capacity);
    World *retval = new World(physics_handler, entity_capacity);
    // it's ok to pass NULL as CreateEntity because it won't be used
    retval->Read(serializer, NULL);

    return retval;
}

World *World::CreateEmpty (
    PhysicsHandler *const physics_handler,
    EntityWorldIndex const entity_capacity)
{
    return new World(physics_handler, entity_capacity);
}

void World::Write (Serializer &serializer) const
{
    serializer.Write<Uint32>(EntityCapacity());
    serializer.Write<Uint32>(MainObjectLayerIndex());
    WriteObjectLayers(serializer);
}

void World::ProcessSvgRootElement (Lvd::Xml::Element const &svg) throw(std::string)
{
}

ObjectLayer *World::CreateObjectLayer (
    Float side_length,
    Uint32 quadtree_depth,
    Float z_depth,
    std::string const &name,
    Lvd::Xml::Element const &g) throw(std::string)
{
    return ObjectLayer::Create(
        this,                       // owner world
        false,                      // is wrapped
        side_length,                // side length
        quadtree_depth,             // (object) quadtree depth
        z_depth,                    // z depth of the layer
        name);                      // name
}

Entity *World::CreateEntity (
    std::string const &entity_type,
    std::string const &entity_name,
    Object &future_owner_object,
    ObjectLayer &object_layer,
    Lvd::Xml::Element const &image) throw(std::string)
{
    return NULL;
}

ObjectLayer const *World::GetObjectLayerByName (std::string const &name) const
{
    for (ObjectLayerList::const_iterator it = m_object_layer_list.begin(),
                                         it_end = m_object_layer_list.end();
         it != it_end;
         ++it)
    {
        ObjectLayer const *ol = *it;
        ASSERT1(ol != NULL);
        if (ol->Name() == name)
            return ol;
    }
    return NULL; // no match
}

ObjectLayer *World::GetObjectLayerByName (std::string const &name)
{
    for (ObjectLayerList::iterator it = m_object_layer_list.begin(),
                                   it_end = m_object_layer_list.end();
         it != it_end;
         ++it)
    {
        ObjectLayer *ol = *it;
        ASSERT1(ol != NULL);
        if (ol->Name() == name)
            return ol;
    }
    return NULL; // no match
}

void World::AttachWorldView (WorldView *world_view)
{
    ASSERT1(world_view != NULL);
    ASSERT1(world_view->GetWorld() == NULL);
    ASSERT1(std::find(m_world_view_list.begin(), m_world_view_list.end(), world_view)
            ==
            m_world_view_list.end());
    world_view->SetWorld(this);
    m_world_view_list.push_back(world_view);
    HandleAttachWorldView(world_view);
}

void World::DetachWorldView (WorldView *world_view)
{
    ASSERT1(world_view != NULL);
    ASSERT1(world_view->GetWorld() == this);
    HandleDetachWorldView(world_view);
    WorldViewList::iterator it =
        std::find(m_world_view_list.begin(), m_world_view_list.end(), world_view);
    ASSERT1(it != m_world_view_list.end());
    m_world_view_list.erase(it);
    world_view->SetWorld(NULL);
}

void World::AddObjectLayer (ObjectLayer *object_layer)
{
    ASSERT1(object_layer != NULL);

    // warn about any name collisions (if the object_layer's name isn't empty)
    if (object_layer->Name() != "")
    {
        for (ObjectLayerList::iterator it = m_object_layer_list.begin(),
                                       it_end = m_object_layer_list.end();
             it != it_end;
             ++it)
        {
            ObjectLayer *ol = *it;
            ASSERT1(ol != NULL);
            if (ol->Name() == object_layer->Name())
            {
                fprintf(stderr, "World::AddObjectLayer(); WARNING: repeated ObjectLayer name \"%s\" - only first such named ObjectLayer will be accessible via GetObjectLayerByName\n", object_layer->Name().c_str());
                break;
            }
        }
    }

    m_object_layer_list.push_back(object_layer);
    if (m_physics_handler != NULL)
        m_physics_handler->AddObjectLayer(object_layer);
}

void World::SetMainObjectLayer (ObjectLayer *main_object_layer)
{
    ASSERT1(main_object_layer != NULL);
    ASSERT1(main_object_layer->OwnerWorld() == this);
    m_main_object_layer = main_object_layer;
    if (m_physics_handler != NULL)
        m_physics_handler->SetMainObjectLayer(m_main_object_layer);
}

void World::AddStaticObject (Object *static_object, ObjectLayer *object_layer)
{
    ASSERT1(static_object != NULL);
    ASSERT1(object_layer != NULL);
    ASSERT1(!static_object->IsDynamic());
    // add the object to the layer
    object_layer->AddObject(static_object);
}

void World::AddDynamicObject (Object *dynamic_object, ObjectLayer *object_layer)
{
    ASSERT1(dynamic_object != NULL);
    ASSERT1(object_layer != NULL);
    ASSERT1(dynamic_object->IsDynamic());
    Entity *entity = dynamic_object->GetEntity();
    ASSERT1(entity != NULL);
    ASSERT1(!entity->IsInWorld());

    if (m_lowest_available_entity_index == static_cast<EntityWorldIndex>(m_entity_vector.size()))
    {
        ASSERT0(false && "World::AddDynamicObject(); dynamic object array full");
        return;
    }

    // add the dynamic object to the layer
    object_layer->AddObject(dynamic_object);

    // add it to the dynamic object vector
    m_entity_vector[m_lowest_available_entity_index] = entity;
    // set the index
    entity->SetWorldIndex(static_cast<EntityWorldIndex>(m_lowest_available_entity_index));
    // increment the lowest entnum
    IncrementLowestAvailableEntityIndex();
    // increment the entity count
    ASSERT1(m_entity_count < UINT32_UPPER_BOUND);
    ++m_entity_count;

    // add the entity to the physics handler (must be done after adding
    // it to the object layer)
    if (m_physics_handler != NULL)
        m_physics_handler->AddEntity(entity);
}

void World::RemoveDynamicObject (Object *dynamic_object)
{
    ASSERT1(dynamic_object != NULL);
    ASSERT1(dynamic_object->IsDynamic());
    Entity *entity = dynamic_object->GetEntity();
    ASSERT1(entity != NULL);
    ASSERT1(entity->IsInWorld());
    EntityWorldIndex entity_index = entity->WorldIndex();
    ASSERT1(entity_index < static_cast<EntityWorldIndex>(m_entity_vector.size()));
    ASSERT1(m_entity_vector[entity_index] == entity);

    // remove the dynamic object from its object layer
    dynamic_object->GetObjectLayer()->RemoveObject(dynamic_object);
    // set the entry in the entity array to null
    m_entity_vector[entity_index] = NULL;
    // if the entity's number is below the current lowest
    // available entity number, use that
    if (entity_index < m_lowest_available_entity_index)
        m_lowest_available_entity_index = entity_index;
    // set the entnum to the sentinel 'not in array' number
    entity->ResetWorldIndex();
    // decrement the entity count
    ASSERT1(m_entity_count > 0);
    --m_entity_count;

    // schedule the entity's events to be deleted.
    OwnerEventQueue()->ScheduleMatchingEventsForDeletion(MatchEntity, entity);

    // remove the entity from the physics handler
    if (m_physics_handler != NULL)
        m_physics_handler->RemoveEntity(entity);
}

Entity *World::DemoteDynamicObjectToStaticObject (Object &object)
{
    ASSERT1(object.IsDynamic());
    ObjectLayer *object_layer = object.GetObjectLayer();
    ASSERT1(object_layer != NULL);
    Entity *entity = object.GetEntity();
    ASSERT1(entity != NULL);
    ASSERT1(entity->IsInWorld());

    RemoveDynamicObject(&object);
    object.SetEntity(NULL);
    ASSERT1(!entity->IsInWorld());
    AddStaticObject(&object, object_layer);

    return entity;
}

World::World (PhysicsHandler *physics_handler, EntityWorldIndex entity_capacity)
    :
    EventHandler(NULL),
    FrameHandler(),
    m_physics_handler(physics_handler)
{
    SetOwnerEventQueue(&m_owner_event_queue);

    if (m_physics_handler != NULL)
        m_physics_handler->SetOwnerWorld(this);
    m_main_object_layer = NULL;
    m_timescale = 1.0f;
    ASSERT1(entity_capacity > 0);
    m_entity_vector.resize(Min(entity_capacity, static_cast<EntityWorldIndex>(MAXIMUM_ENTITY_CAPACITY)));
    std::fill(m_entity_vector.begin(), m_entity_vector.end(), static_cast<Entity *>(NULL));
    m_lowest_available_entity_index = 0;
    m_entity_count = 0;
}

Uint32 World::MainObjectLayerIndex () const
{
    Uint32 index = 0;
    for (ObjectLayerList::const_iterator it = m_object_layer_list.begin(),
                                         it_end = m_object_layer_list.end();
         it != it_end;
         ++it)
    {
        ObjectLayer *object_layer = *it;
        ASSERT1(object_layer != NULL);
        if (object_layer == MainObjectLayer())
            return index;
    }

    ASSERT0(false && "No main object layer");
    return UINT32_UPPER_BOUND;
}

void World::SetMainObjectLayerIndex (Uint32 const index)
{
    Uint32 i = 0;
    for (ObjectLayerList::iterator it = m_object_layer_list.begin(),
                                 it_end = m_object_layer_list.end();
         it != it_end;
         ++it)
    {
        ObjectLayer *object_layer = *it;
        ASSERT1(object_layer != NULL);
        if (i == index) {
            m_main_object_layer = object_layer;
            if (m_physics_handler != NULL)
                m_physics_handler->SetMainObjectLayer(m_main_object_layer);
            return;
        }
    }

    ASSERT0(false && "Invalid index (higher than the highest object layer index)");
}

bool World::HandleEvent (Event const &e)
{
    switch (e.GetEventType())
    {
        case Event::ENGINE2_DELETE_ENTITY:
        case Event::ENGINE2_REMOVE_ENTITY_FROM_WORLD:
        {
            EventEntity const &event_entity = dynamic_cast<EventEntity const &>(e);
            Entity *entity = event_entity.GetEntity();
            ASSERT1(entity != NULL);
            ASSERT1(entity->IsInWorld() && "You can only remove entities already in the world.  An entity not in the world can just be deleted");
            ASSERT1(entity->OwnerObject()->GetWorld() == this);
            RemoveDynamicObject(entity->OwnerObject());
            if (e.GetEventType() == Event::ENGINE2_DELETE_ENTITY)
                delete entity->OwnerObject();
            event_entity.NullifyEntity();
            break;
        }

        default:
            break;
    }

    return true;
}

void World::HandleFrame ()
{
    ASSERT1(m_main_object_layer != NULL);
    if (m_physics_handler != NULL)
        m_physics_handler->ProcessFrame(FrameTime());
    m_owner_event_queue.ProcessFrame(FrameTime());
}

void World::IncrementLowestAvailableEntityIndex ()
{
    EntityWorldIndex max = static_cast<EntityWorldIndex>(m_entity_vector.size());
    while (m_lowest_available_entity_index < max &&
           m_entity_vector[m_lowest_available_entity_index] != NULL)
    {
        ++m_lowest_available_entity_index;
    }
}

void World::UpdateLowestAvailableEntityIndex (
    EntityWorldIndex const removed_entity_index)
{
    if (removed_entity_index < m_lowest_available_entity_index)
        m_lowest_available_entity_index = removed_entity_index;
}

void World::Read (Serializer &serializer, CreateEntityFunction CreateEntity)
{
    ASSERT1(m_object_layer_list.empty());

    Uint32 main_object_layer_index;
    serializer.Read<Uint32>(main_object_layer_index);
    ReadObjectLayers(serializer, CreateEntity);
    SetMainObjectLayerIndex(main_object_layer_index);
}

void World::ReadObjectLayers (
    Serializer &serializer,
    CreateEntityFunction CreateEntity)
{
    Uint32 object_layer_list_size;
    serializer.Read<Uint32>(object_layer_list_size);
    for (Uint32 i = 0; i < object_layer_list_size; ++i)
    {
        ObjectLayer *object_layer = ObjectLayer::Create(serializer, this);
        AddObjectLayer(object_layer);
        ReadDynamicObjectsBelongingToLayer(serializer, object_layer, CreateEntity);
    }
}

void World::ReadDynamicObjectsBelongingToLayer (
    Serializer &serializer,
    ObjectLayer *const object_layer,
    CreateEntityFunction CreateEntity)
{
    ASSERT1(object_layer != NULL);

    Uint32 dynamic_object_count;
    serializer.Read<Uint32>(dynamic_object_count);
    while (dynamic_object_count > 0)
    {
        Object *dynamic_object = Object::Create(serializer, CreateEntity);
        ASSERT1(dynamic_object != NULL);
        ASSERT1(dynamic_object->IsDynamic());
        AddDynamicObject(dynamic_object, object_layer);
        --dynamic_object_count;
    }
}

void World::WriteObjectLayers (Serializer &serializer) const
{
    serializer.Write<Uint32>(m_object_layer_list.size());
    for (ObjectLayerList::const_iterator it = m_object_layer_list.begin(),
                                         it_end = m_object_layer_list.end();
         it != it_end;
         ++it)
    {
        ObjectLayer const *object_layer = *it;
        ASSERT1(object_layer != NULL);
        object_layer->Write(serializer);
        WriteDynamicObjectsBelongingToLayer(serializer, object_layer);
    }
}

void World::WriteDynamicObjectsBelongingToLayer (
    Serializer &serializer,
    ObjectLayer const *const object_layer) const
{
    ASSERT1(object_layer != NULL);

    // first we have to count how many there actually are
    Uint32 dynamic_object_count = 0;
    for (EntityVector::const_iterator it = m_entity_vector.begin(),
                                      it_end = m_entity_vector.end();
         it != it_end;
         ++it)
    {
        Entity const *entity = *it;
        if (entity != NULL)
        {
            ASSERT1(entity->GetObjectLayer() != NULL);
            ASSERT1(entity->IsInWorld());

            // if the entity belongs to the given layer, count it
            if (entity->GetObjectLayer() == object_layer)
                ++dynamic_object_count;
        }
    }

    // write the number of dynamic objects
    serializer.Write<Uint32>(dynamic_object_count);
    // if there are no dynamic objects, return
    if (dynamic_object_count == 0)
        return;

    for (EntityVector::const_iterator it = m_entity_vector.begin(),
                                      it_end = m_entity_vector.end();
         it != it_end;
         ++it)
    {
        Entity const *entity = *it;
        if (entity != NULL)
        {
            ASSERT1(entity->GetObjectLayer() != NULL);
            ASSERT1(entity->IsInWorld());

            // if the entity belongs to the given layer, write it out
            if (entity->GetObjectLayer() == object_layer)
            {
                entity->OwnerObject()->Write(serializer);
                --dynamic_object_count;
            }
        }
    }
    ASSERT1(dynamic_object_count == 0);
}

} // end of namespace Engine2
} // end of namespace Xrb
