// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_world.cpp by Victor Dods, created 2005/02/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_mapeditor2_world.hpp"

#include "xrb_eventqueue.hpp"
#include "xrb_mapeditor2_entity.hpp"
#include "xrb_mapeditor2_object.hpp"
#include "xrb_mapeditor2_objectlayer.hpp"
#include "xrb_mapeditor2_compound.hpp" // temp
#include "xrb_serializer.hpp" // temp
#include "xrb_engine2_planetaryphysicshandler.hpp" // temp

namespace Xrb
{

MapEditor2::World *MapEditor2::World::Create (Serializer &serializer)
{
    ASSERT1(serializer.GetIsOpen());
    ASSERT1(serializer.GetIODirection() == IOD_READ);

    Uint32 entity_capacity = serializer.ReadUint32();
    ASSERT1(entity_capacity > 0);
    World *retval = new World(entity_capacity);

    retval->Read(serializer);
    retval->m_saved_main_object_layer =
        DStaticCast<ObjectLayer *>(retval->GetMainObjectLayer());

    return retval;
}

MapEditor2::World *MapEditor2::World::CreateEmpty (Uint32 const entity_capacity)
{
    ASSERT1(entity_capacity > 0);
    World *retval = new World(entity_capacity);

    ObjectLayer *map_editor_object_layer = 
        ObjectLayer::Create(retval, false, 1024.0, 4, 0.0);
    retval->AddObjectLayer(map_editor_object_layer);
    retval->m_main_object_layer = map_editor_object_layer;
    retval->m_saved_main_object_layer = map_editor_object_layer;

    return retval;
}

void MapEditor2::World::IncrementMainObjectLayer ()
{
    ObjectLayerListIterator it =
        GetObjectLayerIterator(
            DStaticCast<ObjectLayer *>(m_main_object_layer));
    ObjectLayerListIterator it_end = m_object_layer_list.end();

    ASSERT1(it != it_end);

    --it;
    if (it != it_end) {
        m_main_object_layer = *it;
        ASSERT1(dynamic_cast<ObjectLayer *>(m_main_object_layer) != NULL);
        m_sender_main_object_layer_changed.Signal();
    }
}

void MapEditor2::World::DecrementMainObjectLayer ()
{
    ObjectLayerListIterator it =
        GetObjectLayerIterator(
            DStaticCast<ObjectLayer *>(m_main_object_layer));
    ObjectLayerListIterator it_end = m_object_layer_list.end();

    ASSERT1(it != it_end);

    ++it;
    if (it != it_end) {
        m_main_object_layer = *it;
        ASSERT1(dynamic_cast<ObjectLayer *>(m_main_object_layer) != NULL);
        m_sender_main_object_layer_changed.Signal();
    }
}

void MapEditor2::World::RemoveObject (MapEditor2::Object *const object)
{
    ASSERT1(object != NULL);
    ASSERT1(object->GetHasOwnerQuadTree(Engine2::QTT_VISIBILITY));

    Entity *entity = dynamic_cast<Entity *>(object);
    if (entity != NULL)
        // if the object is an entity, use the world's RemoveEntity
        RemoveEntity(entity);
    else
        // otherwise remove the object from its object layer (because
        // the world does not track non-entities).
        object->GetObjectLayer()->RemoveObject(object);
}

MapEditor2::World::World (Uint32 const entity_capacity)
    :
    Engine2::World(
        new Engine2::PlanetaryPhysicsHandler(), // temp physics handler
        entity_capacity),
    m_sender_main_object_layer_changed(this)
{
    m_is_running = false;
    m_saved_main_object_layer = NULL;
}

Uint32 MapEditor2::World::GetMainObjectLayerIndex () const
{
    Uint32 index = 0;
    for (ObjectLayerListConstIterator it = m_object_layer_list.begin(),
                                      it_end = m_object_layer_list.end();
         it != it_end;
         ++it)
    {
        ObjectLayer *map_editor_object_layer =
            DStaticCast<ObjectLayer *>(*it);

        if (map_editor_object_layer == GetSavedMainObjectLayer())
            return index;

        ++index;
    }

    ASSERT0(false && "No main object layer");
    return UINT32_UPPER_BOUND;
}

void MapEditor2::World::SetMainObjectLayerIndex (Uint32 const index)
{
    Uint32 i = 0;
    for (ObjectLayerListConstIterator it = m_object_layer_list.begin(),
                                      it_end = m_object_layer_list.end();
         it != it_end;
         ++it)
    {
        ObjectLayer *map_editor_object_layer =
            DStaticCast<ObjectLayer *>(*it);

        if (i == index)
        {
            m_saved_main_object_layer = map_editor_object_layer;
            m_main_object_layer = map_editor_object_layer;
            m_sender_main_object_layer_changed.Signal();
            return;
        }

        ++i;
    }

    ASSERT0(false && "Invalid index (higher than the highest object layer index)");
}

void MapEditor2::World::HandleFrame ()
{
    if (GetIsRunning())
        Engine2::World::HandleFrame();
}

void MapEditor2::World::ReadObjectLayers (Serializer &serializer)
{
    Uint32 object_layer_list_size = serializer.ReadUint32();
    for (Uint32 i = 0; i < object_layer_list_size; ++i)
    {
        ObjectLayer *map_editor_object_layer =
            ObjectLayer::Create(serializer, this);
        AddObjectLayer(map_editor_object_layer);
        ReadEntitiesBelongingToLayer(serializer, map_editor_object_layer);
    }
    ASSERT1(serializer.GetHasFewerThan8BitsLeft());
}

void MapEditor2::World::ReadEntitiesBelongingToLayer (
    Serializer &serializer,
    MapEditor2::ObjectLayer *const object_layer)
{
    ASSERT1(object_layer != NULL);

    Uint32 entity_count = serializer.ReadUint32();
    while (entity_count > 0)
    {
        Object *object = Object::Create(serializer);
        ASSERT1(object != NULL);
        Entity *entity = dynamic_cast<Entity *>(object);
        ASSERT1(entity != NULL);
        AddEntity(entity, object_layer);
        --entity_count;
    }
}

Engine2::World::ObjectLayerListIterator MapEditor2::World::GetObjectLayerIterator (
    MapEditor2::ObjectLayer *object_layer)
{
    ObjectLayerListIterator it;
    ObjectLayerListIterator it_end;
    for (it = m_object_layer_list.begin(),
         it_end = m_object_layer_list.end();
         it != it_end;
         ++it)
    {
        ObjectLayer *map_editor_object_layer =
            DStaticCast<ObjectLayer *>(*it);

        if (object_layer == map_editor_object_layer)
            return it;
    }

    return it_end;
}

} // end of namespace Xrb
