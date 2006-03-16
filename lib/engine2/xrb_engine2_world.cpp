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

#include "xrb_engine2_entityguts.h"
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
    ASSERT1(m_world_view_list.empty())

    for (Entity::Index i = 0;
         i < static_cast<Entity::Index>(m_entity_vector.size());
         ++i)
    {
        Entity *entity = m_entity_vector[i];
        if (entity != NULL)
        {
            RemoveEntity(entity);
            Delete(entity);
        }
    }

    Delete(m_physics_handler);

    for (ObjectLayerListIterator it = m_object_layer_list.begin(),
                                 it_end = m_object_layer_list.end();
         it != it_end;
         ++it)
    {
        ObjectLayer *object_layer = *it;
        ASSERT1(object_layer != NULL)
        Delete(object_layer);
    }
    m_object_layer_list.clear();
}

Engine2::World *Engine2::World::Create (
    Serializer &serializer,
    EventQueue *const owner_event_queue,
    PhysicsHandler *const physics_handler)
{
    ASSERT1(serializer.GetIsOpen())
    ASSERT1(serializer.GetIODirection() == IOD_READ)
    ASSERT1(owner_event_queue != NULL)
    ASSERT1(physics_handler != NULL)

    Uint32 entity_capacity = serializer.ReadUint32();
    ASSERT1(entity_capacity > 0)
    World *retval = new World(owner_event_queue, physics_handler, entity_capacity);
    retval->Read(serializer);

    return retval;
}

Engine2::World *Engine2::World::CreateEmpty (
    EventQueue *const owner_event_queue,
    PhysicsHandler *const physics_handler,
    Uint32 const entity_capacity)
{
    ASSERT1(owner_event_queue != NULL)
    ASSERT1(physics_handler != NULL)
    ASSERT1(entity_capacity > 0)
    return new World(owner_event_queue, physics_handler, entity_capacity);
}

void Engine2::World::Write (Serializer &serializer) const
{
    serializer.WriteUint32(GetEntityCapacity());
    serializer.WriteUint32(GetMainObjectLayerIndex());
    WriteObjectLayers(serializer);
}

void Engine2::World::AttachWorldView (WorldView *const world_view)
{
    ASSERT1(world_view != NULL)
    ASSERT1(world_view->GetWorld() == NULL)
    ASSERT1(std::find(m_world_view_list.begin(), m_world_view_list.end(), world_view)
            ==
            m_world_view_list.end())
    world_view->SetWorld(this);
    m_world_view_list.push_back(world_view);
    HandleAttachWorldView(world_view);
}

void Engine2::World::DetachWorldView (WorldView *const world_view)
{
    ASSERT1(world_view != NULL)
    ASSERT1(world_view->GetWorld() == this)
    HandleDetachWorldView(world_view);
    WorldViewListIterator it =
        std::find(m_world_view_list.begin(), m_world_view_list.end(), world_view);
    ASSERT1(it != m_world_view_list.end())
    m_world_view_list.erase(it);
    world_view->SetWorld(NULL);
}

void Engine2::World::AddObjectLayer (Engine2::ObjectLayer *const object_layer)
{
    ASSERT1(object_layer != NULL)
    m_object_layer_list.push_back(object_layer);
    ASSERT1(m_physics_handler != NULL)
    m_physics_handler->AddObjectLayer(object_layer);
}

void Engine2::World::SetMainObjectLayer (Engine2::ObjectLayer *main_object_layer)
{
    ASSERT1(main_object_layer != NULL)
    ASSERT1(main_object_layer->GetOwnerWorld() == this)
    m_main_object_layer = main_object_layer;
    ASSERT1(m_physics_handler != NULL)
    m_physics_handler->SetMainObjectLayer(m_main_object_layer);
}

void Engine2::World::AddEntity (
    Engine2::Entity *const entity,
    Engine2::ObjectLayer *const object_layer)
{
    ASSERT1(entity != NULL)
    ASSERT1(object_layer != NULL)

    if (m_lowest_available_entity_number == (Sint32)m_entity_vector.size())
    {
        ASSERT0(false && "World::AddEntity(); entity array full")
        return;
    }

    // add it to the entity array
    m_entity_vector[m_lowest_available_entity_number] = entity;
    // set the entity's entnum
    entity->SetNumber((Sint32)m_lowest_available_entity_number);
    // increment the lowest entnum
    IncrementLowestAvailableEntityNumber();

    // set the 'owning' layer of the object
    entity->SetObjectLayer(object_layer);
    // add the object to the layer
    object_layer->AddObject(entity);

    // add the entity to the physics handler (must be done after adding
    // it to the object layer)
    ASSERT1(m_physics_handler != NULL)
    m_physics_handler->AddEntity(entity);

    ASSERT1(entity->GetOwnerQuadTree(QTT_VISIBILITY) != NULL)
    // there is no assertion that the entity belongs to a physics handler's
    // quadtree here, because the physics handler may not use a quadtree.
}

void Engine2::World::AddObject (
    Engine2::Object *const object,
    Engine2::ObjectLayer *const object_layer)
{
    ASSERT1(object != NULL)
    ASSERT1(object_layer != NULL)

    // if it's an entity, stick it into the entity vector
    Entity *entity = dynamic_cast<Entity *>(object);
    if (entity != NULL)
        AddEntity(entity, object_layer);
    else
    {
        // set the 'owning' layer of the object
        object->SetObjectLayer(object_layer);
        // add the object to the layer
        object_layer->AddObject(object);

        ASSERT1(object->GetOwnerQuadTree(QTT_VISIBILITY) != NULL)
    }
}

void Engine2::World::RemoveEntity (Engine2::Entity *const entity)
{
    ASSERT1(entity != NULL)
    ASSERT1(entity->GetIsInWorld())
    ASSERT1(entity->GetNumber() < (Sint32)m_entity_vector.size())
    ASSERT1(entity == m_entity_vector[entity->GetNumber()])

    // remove the entity from its object layer
    entity->GetObjectLayer()->RemoveObject(entity);
    // set the entry in the entity array to null
    m_entity_vector[entity->GetNumber()] = NULL;
    // if the entity's number is below the current lowest
    // available entity number, use that
    if (entity->GetNumber() < m_lowest_available_entity_number)
        m_lowest_available_entity_number = entity->GetNumber();
    // set the entnum to the sentinel 'not in array' number
    entity->ResetNumber();

    // schedule the entity's events to be deleted.
    GetOwnerEventQueue()->ScheduleMatchingEventsForDeletion(
        Engine2::MatchEntity,
        entity);
    
    // remove the entity from the physics handler
    ASSERT1(m_physics_handler != NULL)
    m_physics_handler->RemoveEntity(entity);
}

Engine2::World::World (
    EventQueue *const owner_event_queue,
    PhysicsHandler *const physics_handler,
    Uint32 const entity_capacity)
    :
    EventHandler(owner_event_queue),
    FrameHandler()
{
    ASSERT1(owner_event_queue != NULL)
    ASSERT1(physics_handler != NULL)
    ASSERT1(entity_capacity > 0)
    m_entity_vector.reserve(entity_capacity);
    m_entity_vector.resize(entity_capacity);
    ASSERT1(m_entity_vector.capacity() == entity_capacity);
    ASSERT1(m_entity_vector.size() == entity_capacity);
    for (Uint32 i = 0; i < m_entity_vector.size(); ++i)
        m_entity_vector[i] = NULL;

    // reset the event queue as a FrameHandler for use in the new World
    owner_event_queue->ResetFrameHandler();
            
    m_lowest_available_entity_number = 0;
    m_physics_handler = physics_handler;
    m_physics_handler->SetOwnerWorld(this);
    m_main_object_layer = NULL;
    m_timescale = 1.0f;
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
        ASSERT1(object_layer != NULL)
        if (object_layer == GetMainObjectLayer())
            return index;
    }

    ASSERT0(false && "No main object layer")
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
        ASSERT1(object_layer != NULL)
        if (i == index) {
            m_main_object_layer = object_layer;
            ASSERT1(m_physics_handler != NULL)
            m_physics_handler->SetMainObjectLayer(m_main_object_layer);
            return;
        }
    }

    ASSERT0(false && "Invalid index (higher than the highest object layer index)")
}

bool Engine2::World::ProcessEventOverride (Event const *const event)
{
    ASSERT1(event != NULL)
    switch (event->GetType())
    {
        case Event::ENGINE2_REMOVE_ENTITY_FROM_WORLD:
        {
            EventEntity const *event_entity =
                DStaticCast<EventEntity const *>(event);
            Entity *entity = event_entity->GetEntity();
            ASSERT1(entity != NULL)
            entity->RemoveFromWorld();
            event_entity->NullifyEntity();
            break;
        }
        
        case Event::ENGINE2_DELETE_ENTITY:
        {
            EventEntity const *event_entity =
                DStaticCast<EventEntity const *>(event);
            Entity *entity = event_entity->GetEntity();
            ASSERT1(entity != NULL)
            ASSERT1(entity->GetWorld() == this)
            ASSERT1(entity->GetIsInWorld() &&
                    "You shouldn't schedule removed entities "
                    "for deletion -- just delete them")
            RemoveEntity(entity);
            if (entity->GetEntityGuts() != NULL)
                entity->GetEntityGuts()->HandleScheduledDeletion(GetMostRecentFrameTime());
            delete entity;
            event_entity->NullifyEntity();
            break;
        }

        default:
            break;
    }

    return true;
}

void Engine2::World::ProcessFrameOverride ()
{
    ASSERT1(m_main_object_layer != NULL)
    ASSERT1(m_physics_handler != NULL)
    m_physics_handler->ProcessFrame(GetFrameTime());
}

void Engine2::World::IncrementLowestAvailableEntityNumber ()
{
    Sint32 max = (Sint32)m_entity_vector.size();
    while (m_lowest_available_entity_number < max &&
           m_entity_vector[m_lowest_available_entity_number] != NULL)
    {
        ++m_lowest_available_entity_number;
    }
}

void Engine2::World::UpdateLowestAvailableEntityNumber (
    Sint32 const removed_entity_number)
{
    ASSERT1(removed_entity_number >= 0)

    if (removed_entity_number < m_lowest_available_entity_number)
        m_lowest_available_entity_number = removed_entity_number;
}

void Engine2::World::Read (Serializer &serializer)
{
    ASSERT1(m_object_layer_list.empty())

    Uint32 main_object_layer_index = serializer.ReadUint32();
    ReadObjectLayers(serializer);
    SetMainObjectLayerIndex(main_object_layer_index);
}

void Engine2::World::ReadObjectLayers (Serializer &serializer)
{
    Uint32 object_layer_list_size = serializer.ReadUint32();
    for (Uint32 i = 0; i < object_layer_list_size; ++i)
    {
        ObjectLayer *object_layer = ObjectLayer::Create(serializer, this);
        AddObjectLayer(object_layer);
        ReadEntitiesBelongingToLayer(serializer, object_layer);
    }
    ASSERT1(serializer.GetHasFewerThan8BitsLeft());
}

void Engine2::World::ReadEntitiesBelongingToLayer (
    Serializer &serializer,
    Engine2::ObjectLayer *const object_layer)
{
    ASSERT1(object_layer != NULL)

    Uint32 entity_count = serializer.ReadUint32();
    while (entity_count > 0)
    {
        Object *object = Object::Create(serializer);
        ASSERT1(object != NULL)
        Entity *entity = dynamic_cast<Entity *>(object);
        ASSERT1(entity != NULL)
        AddEntity(entity, object_layer);
        --entity_count;
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
        ASSERT1(object_layer != NULL)
        object_layer->Write(serializer);
        WriteEntitiesBelongingToLayer(serializer, object_layer);
    }
}

void Engine2::World::WriteEntitiesBelongingToLayer (
    Serializer &serializer,
    Engine2::ObjectLayer const *const object_layer) const
{
    ASSERT1(object_layer != NULL)

    // first we have to count how many there actually are
    Uint32 entity_count = 0;
    for (EntityVectorConstIterator it = m_entity_vector.begin(),
                                   it_end = m_entity_vector.end();
         it != it_end;
         ++it)
    {
        Entity const *entity = *it;
        if (entity != NULL)
        {
            ASSERT1(entity->GetObjectLayer() != NULL)
            ASSERT1(entity->GetIsInWorld())

            // if the entity belongs to the given layer, count it
            if (entity->GetObjectLayer() == object_layer)
                ++entity_count;
        }
    }

    // write the number of entities
    serializer.WriteUint32(entity_count);
    // write the entities themselves (only if there are > 0)
    if (entity_count == 0)
        return;

    for (EntityVectorConstIterator it = m_entity_vector.begin(),
                                   it_end = m_entity_vector.end();
         it != it_end;
         ++it)
    {
        Entity const *entity = *it;
        if (entity != NULL)
        {
            ASSERT1(entity->GetObjectLayer() != NULL)
            ASSERT1(entity->GetIsInWorld())

            // if the entity belongs to the given layer, write it out
            if (entity->GetObjectLayer() == object_layer)
            {
                entity->Write(serializer);
                --entity_count;
            }
        }
    }
    ASSERT1(entity_count == 0)
}

} // end of namespace Xrb
