// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_world.cpp by Victor Dods, created 2004/06/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_world.h"

#include <algorithm>

#include "xrb_engine2_events.h"
#include "xrb_engine2_objectlayer.h"
#include "xrb_engine2_physicshandler.h"
#include "xrb_engine2_visibilityquadtree.h"
#include "xrb_engine2_worldview.h"
#include "xrb_eventqueue.h"
#include "xrb_serializer.h"

namespace Xrb
{

Engine2::World::~World ()
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
            RemoveDynamicObject(entity->GetOwnerObject());
            delete entity->GetOwnerObject();
        }
    }
    ASSERT1(m_entity_count == 0);

    // all entities have been removed, we can now delete the physics handler
    delete m_physics_handler;

    // delete the ObjectLayers, which will delete the static objects directly.
    for (ObjectLayerListIterator it = m_object_layer_list.begin(),
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

Engine2::World *Engine2::World::Create (
    Serializer &serializer,
    CreateEntityFunction CreateEntity,
    PhysicsHandler *const physics_handler)
{
    ASSERT1(serializer.GetIsOpen());
    ASSERT1(serializer.GetIODirection() == IOD_READ);

    Uint16 entity_capacity = serializer.ReadUint32();
    World *retval = new World(physics_handler, entity_capacity);
    // it's ok to pass NULL as CreateEntity because it won't be used
    retval->Read(serializer, NULL);

    return retval;
}

Engine2::World *Engine2::World::CreateEmpty (
    PhysicsHandler *const physics_handler,
    EntityWorldIndex const entity_capacity)
{
    return new World(physics_handler, entity_capacity);
}

void Engine2::World::Write (Serializer &serializer) const
{
    serializer.WriteUint32(GetEntityCapacity());
    serializer.WriteUint32(GetMainObjectLayerIndex());
    WriteObjectLayers(serializer);
}

void Engine2::World::AttachWorldView (WorldView *const world_view)
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

void Engine2::World::DetachWorldView (WorldView *const world_view)
{
    ASSERT1(world_view != NULL);
    ASSERT1(world_view->GetWorld() == this);
    HandleDetachWorldView(world_view);
    WorldViewListIterator it =
        std::find(m_world_view_list.begin(), m_world_view_list.end(), world_view);
    ASSERT1(it != m_world_view_list.end());
    m_world_view_list.erase(it);
    world_view->SetWorld(NULL);
}

void Engine2::World::AddObjectLayer (Engine2::ObjectLayer *const object_layer)
{
    ASSERT1(object_layer != NULL);
    m_object_layer_list.push_back(object_layer);
    if (m_physics_handler != NULL)
        m_physics_handler->AddObjectLayer(object_layer);
}

void Engine2::World::SetMainObjectLayer (Engine2::ObjectLayer *main_object_layer)
{
    ASSERT1(main_object_layer != NULL);
    ASSERT1(main_object_layer->GetOwnerWorld() == this);
    m_main_object_layer = main_object_layer;
    if (m_physics_handler != NULL)
        m_physics_handler->SetMainObjectLayer(m_main_object_layer);
}

void Engine2::World::AddStaticObject (
    Engine2::Object *const static_object,
    Engine2::ObjectLayer *const object_layer)
{
    ASSERT1(static_object != NULL);
    ASSERT1(object_layer != NULL);
    ASSERT1(!static_object->GetIsDynamic());
    // add the object to the layer
    object_layer->AddObject(static_object);
}

void Engine2::World::AddDynamicObject (
    Engine2::Object *const dynamic_object,
    Engine2::ObjectLayer *const object_layer)
{
    ASSERT1(dynamic_object != NULL);
    ASSERT1(object_layer != NULL);
    ASSERT1(dynamic_object->GetIsDynamic());
    Entity *entity = dynamic_object->GetEntity();
    ASSERT1(entity != NULL);
    ASSERT1(!entity->GetIsInWorld());

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

void Engine2::World::RemoveDynamicObject (Engine2::Object *const dynamic_object)
{
    ASSERT1(dynamic_object != NULL);
    ASSERT1(dynamic_object->GetIsDynamic());
    Entity *entity = dynamic_object->GetEntity();
    ASSERT1(entity != NULL);
    ASSERT1(entity->GetIsInWorld());
    EntityWorldIndex entity_index = entity->GetWorldIndex();
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
    GetOwnerEventQueue()->ScheduleMatchingEventsForDeletion(
        Engine2::MatchEntity,
        entity);

    // remove the entity from the physics handler
    if (m_physics_handler != NULL)
        m_physics_handler->RemoveEntity(entity);
}

Engine2::World::World (
    PhysicsHandler *const physics_handler,
    EntityWorldIndex const entity_capacity)
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

Uint32 Engine2::World::GetMainObjectLayerIndex () const
{
    Uint32 index = 0;
    for (ObjectLayerListConstIterator it = m_object_layer_list.begin(),
                                      it_end = m_object_layer_list.end();
         it != it_end;
         ++it)
    {
        ObjectLayer *object_layer = *it;
        ASSERT1(object_layer != NULL);
        if (object_layer == GetMainObjectLayer())
            return index;
    }

    ASSERT0(false && "No main object layer");
    return UINT32_UPPER_BOUND;
}

void Engine2::World::SetMainObjectLayerIndex (Uint32 const index)
{
    Uint32 i = 0;
    for (ObjectLayerListIterator it = m_object_layer_list.begin(),
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

bool Engine2::World::HandleEvent (Event const *const e)
{
    ASSERT1(e != NULL);
    switch (e->GetEventType())
    {
        case Event::ENGINE2_DELETE_ENTITY:
        {
            EventEntity const *event_entity =
                DStaticCast<EventEntity const *>(e);
            Entity *entity = event_entity->GetEntity();
            ASSERT1(entity != NULL);
            ASSERT1(entity->GetIsInWorld() &&
                    "You shouldn't schedule removed entities "
                    "for deletion -- just delete them");
            ASSERT1(entity->GetOwnerObject()->GetWorld() == this);
            RemoveDynamicObject(entity->GetOwnerObject());
            delete entity->GetOwnerObject();
            event_entity->NullifyEntity();
            break;
        }

        case Event::ENGINE2_REMOVE_ENTITY_FROM_WORLD:
        {
            EventEntity const *event_entity =
                DStaticCast<EventEntity const *>(e);
            Entity *entity = event_entity->GetEntity();
            ASSERT1(entity != NULL);
            ASSERT1(entity->GetIsInWorld() &&
                    "You can only remove entities already in the world");
            ASSERT1(entity->GetOwnerObject()->GetWorld() == this);
            RemoveDynamicObject(entity->GetOwnerObject());
            event_entity->NullifyEntity();
            break;
        }

        default:
            break;
    }

    return true;
}

void Engine2::World::HandleFrame ()
{
    ASSERT1(m_main_object_layer != NULL);
    if (m_physics_handler != NULL)
        m_physics_handler->ProcessFrame(GetFrameTime());
    m_owner_event_queue.ProcessFrame(GetFrameTime());
}

void Engine2::World::IncrementLowestAvailableEntityIndex ()
{
    EntityWorldIndex max = static_cast<EntityWorldIndex>(m_entity_vector.size());
    while (m_lowest_available_entity_index < max &&
           m_entity_vector[m_lowest_available_entity_index] != NULL)
    {
        ++m_lowest_available_entity_index;
    }
}

void Engine2::World::UpdateLowestAvailableEntityIndex (
    EntityWorldIndex const removed_entity_index)
{
    if (removed_entity_index < m_lowest_available_entity_index)
        m_lowest_available_entity_index = removed_entity_index;
}

void Engine2::World::Read (Serializer &serializer, CreateEntityFunction CreateEntity)
{
    ASSERT1(m_object_layer_list.empty());

    Uint32 main_object_layer_index = serializer.ReadUint32();
    ReadObjectLayers(serializer, CreateEntity);
    SetMainObjectLayerIndex(main_object_layer_index);
}

void Engine2::World::ReadObjectLayers (
    Serializer &serializer,
    CreateEntityFunction CreateEntity)
{
    Uint32 object_layer_list_size = serializer.ReadUint32();
    for (Uint32 i = 0; i < object_layer_list_size; ++i)
    {
        ObjectLayer *object_layer = ObjectLayer::Create(serializer, this);
        AddObjectLayer(object_layer);
        ReadDynamicObjectsBelongingToLayer(serializer, object_layer, CreateEntity);
    }
    ASSERT1(serializer.GetHasFewerThan8BitsLeft());
}

void Engine2::World::ReadDynamicObjectsBelongingToLayer (
    Serializer &serializer,
    Engine2::ObjectLayer *const object_layer,
    CreateEntityFunction CreateEntity)
{
    ASSERT1(object_layer != NULL);

    Uint32 dynamic_object_count = serializer.ReadUint32();
    while (dynamic_object_count > 0)
    {
        Object *dynamic_object = Object::Create(serializer, CreateEntity);
        ASSERT1(dynamic_object != NULL);
        ASSERT1(dynamic_object->GetIsDynamic());
        AddDynamicObject(dynamic_object, object_layer);
        --dynamic_object_count;
    }
}

void Engine2::World::WriteObjectLayers (Serializer &serializer) const
{
    serializer.WriteUint32(m_object_layer_list.size());
    for (ObjectLayerListConstIterator it = m_object_layer_list.begin(),
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

void Engine2::World::WriteDynamicObjectsBelongingToLayer (
    Serializer &serializer,
    Engine2::ObjectLayer const *const object_layer) const
{
    ASSERT1(object_layer != NULL);

    // first we have to count how many there actually are
    Uint32 dynamic_object_count = 0;
    for (EntityVectorConstIterator it = m_entity_vector.begin(),
                                   it_end = m_entity_vector.end();
         it != it_end;
         ++it)
    {
        Entity const *entity = *it;
        if (entity != NULL)
        {
            ASSERT1(entity->GetObjectLayer() != NULL);
            ASSERT1(entity->GetIsInWorld());

            // if the entity belongs to the given layer, count it
            if (entity->GetObjectLayer() == object_layer)
                ++dynamic_object_count;
        }
    }

    // write the number of dynamic objects
    serializer.WriteUint32(dynamic_object_count);
    // if there are no dynamic objects, return
    if (dynamic_object_count == 0)
        return;

    for (EntityVectorConstIterator it = m_entity_vector.begin(),
                                   it_end = m_entity_vector.end();
         it != it_end;
         ++it)
    {
        Entity const *entity = *it;
        if (entity != NULL)
        {
            ASSERT1(entity->GetObjectLayer() != NULL);
            ASSERT1(entity->GetIsInWorld());

            // if the entity belongs to the given layer, write it out
            if (entity->GetObjectLayer() == object_layer)
            {
                entity->GetOwnerObject()->Write(serializer);
                --dynamic_object_count;
            }
        }
    }
    ASSERT1(dynamic_object_count == 0);
}

} // end of namespace Xrb
