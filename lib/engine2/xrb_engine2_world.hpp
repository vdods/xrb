// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_world.hpp by Victor Dods, created 2004/06/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_WORLD_HPP_)
#define _XRB_ENGINE2_WORLD_HPP_

#include "xrb.hpp"

#include <map>
#include <list>
#include <vector>

#include "xrb_engine2_entity.hpp"
#include "xrb_eventhandler.hpp"
#include "xrb_eventqueue.hpp"
#include "xrb_framehandler.hpp"

namespace Lvd {
namespace Xml {

struct Element;

} // end of namespace Xml
} // end of namespace Lvd

namespace Xrb {

class AttributeSet;
class Serializer;

/** Contains all of the "server side" code, i.e. the game world, collision
  * detection, physics, any other code that directly uses the contents of
  * the game world, as well as the WorldView and WorldViewWidget components
  * of the render stack.
  *
  * Everything provided by libxrb in the Engine2 namespace is stored in the
  * @ref section_lib_engine2 "/lib/engine2/" directory, with the exception
  * of WorldView and WorldViewWidget (client-side components), which are
  * stored in @ref section_lib_render "/lib/render/".
  *
  * @brief Contains all of the 2D-engine-specific code.
  */
namespace Engine2 {

class ObjectLayer;
class Object;
class PhysicsHandler;
class WorldView;

// The World class embodies the physical state of sprites in the world,
// and is view-independent.  This allows rendering-independent physics
// and gameplay to be computed.
class World : public EventHandler, public FrameHandler
{
public:

    enum
    {
        MAXIMUM_ENTITY_CAPACITY = ENTITY_IS_NOT_IN_WORLD - 1,
        DEFAULT_ENTITY_CAPACITY = 0x1000
    };

    typedef std::list<ObjectLayer *> ObjectLayerList;

    virtual ~World ();

    static World *Create (
        Serializer &serializer,
        CreateEntityFunction CreateEntity,
        PhysicsHandler *physics_handler);
    static World *CreateEmpty (
        PhysicsHandler *physics_handler,
        EntityWorldIndex entity_capacity = DEFAULT_ENTITY_CAPACITY);
    void Write (Serializer &serializer) const;

    // ///////////////////////////////////////////////////////////////////////
    // these are only used in LoadSvgIntoWorld.

    // give the world a chance to look at the <svg> (root) element
    virtual void ProcessSvgRootElement (Lvd::Xml::Element const &svg) throw(std::string);
    // must create and return a pointer to an ObjectLayer with the specified
    // properties, but gives the world a chance to process the corresponding
    // <g> element.  do not add the created ObjectLayer to the world, this will
    // be done by LoadSvgIntoWorld.
    virtual ObjectLayer *CreateObjectLayer (
        Float side_length,
        Uint32 quadtree_depth,
        Float z_depth,
        std::string const &name,
        Lvd::Xml::Element const &g) throw(std::string);
    // must create and return a pointer to (a subclass of) Entity.  entity_type
    // and entity_name are the attribute values of xrb_entity_type and
    // xrb_entity_name respectively.  future_owner_object is the already-created
    // Object instance (e.g. Sprite) which the returned Entity will be attached
    // to.  object_layer is the ObjectLayer this Entity will be added to.
    // finally, image is the <image> element corresponding to this entity, giving
    // the world a chance to process it.
    virtual Entity *CreateEntity (
        std::string const &entity_type,
        std::string const &entity_name,
        Object &future_owner_object,
        ObjectLayer &object_layer,
        Lvd::Xml::Element const &image) throw(std::string);

    // end of LoadSvgIntoWorld helper methods
    // ///////////////////////////////////////////////////////////////////////

    Uint32 EntityCapacity () const { ASSERT1(m_entity_vector.capacity() == m_entity_vector.size()); return m_entity_vector.size(); }
    Uint32 EntityCount () const { return m_entity_count; }
    Entity const *GetEntity (Uint32 index) const { ASSERT1(index < m_entity_vector.size()); return m_entity_vector[index]; }
    Entity *GetEntity (Uint32 index) { ASSERT1(index < m_entity_vector.size()); return m_entity_vector[index]; }
    PhysicsHandler const *GetPhysicsHandler () const { return m_physics_handler; }
    PhysicsHandler *GetPhysicsHandler () { return m_physics_handler; }
    ObjectLayerList &GetObjectLayerList () { return m_object_layer_list; }
    ObjectLayer const *GetObjectLayerByName (std::string const &name) const;
    ObjectLayer *GetObjectLayerByName (std::string const &name);
    virtual ObjectLayer const *MainObjectLayer () const { return m_main_object_layer; }
    virtual ObjectLayer *MainObjectLayer () { return m_main_object_layer; }
    Float Timescale () { return m_timescale; }

    void SetTimescale (Float timescale)
    {
        ASSERT1(timescale >= 0.0f);
        m_timescale = timescale;
    }

    // you must call this method after creating a WorldView
    void AttachWorldView (WorldView *world_view);
    // you must call this method before destroying a WorldView
    void DetachWorldView (WorldView *world_view);

    void AddObjectLayer (ObjectLayer *object_layer);
    void SetMainObjectLayer (ObjectLayer *main_object_layer);

    // for adding static objects only (because it will be added only to
    // the object layer, which will delete it upon destruction.
    // there is no way to remove a static object).
    void AddStaticObject (Object *static_object, ObjectLayer *object_layer);

    // for adding dynamic objects only (causes them to be added to
    // the dynamic object vector and the physics handler).
    void AddDynamicObject (Object *dynamic_object, ObjectLayer *object_layer);
    // for removing a dynamic object (causes them to be removed from
    // the dynamic object vector and the physics handler).
    void RemoveDynamicObject (Object *dynamic_object);

protected:

    World (
        PhysicsHandler *physics_handler,
        EntityWorldIndex entity_capacity = DEFAULT_ENTITY_CAPACITY);

    virtual Uint32 MainObjectLayerIndex () const;

    virtual void SetMainObjectLayerIndex (Uint32 index);

    // process an event
    virtual bool HandleEvent (Event const *e);
    // this calculates one frame, called by the game loop
    virtual void HandleFrame ();
    void IncrementLowestAvailableEntityIndex ();
    void UpdateLowestAvailableEntityIndex (EntityWorldIndex removed_entity_index);

    // this function should be called only from constructors (or Create)
    void Read (Serializer &serializer, CreateEntityFunction CreateEntity);

    // this function is called after the worldview is added to the
    // worldview list during AttachWorldView.
    virtual void HandleAttachWorldView (WorldView *world_view) { }
    // this function is called before the worldview is removed from the
    // worldview list during DetachWorldView.
    virtual void HandleDetachWorldView (WorldView *world_view) { }

    virtual void ReadObjectLayers (
        Serializer &serializer,
        CreateEntityFunction CreateEntity);
    virtual void ReadDynamicObjectsBelongingToLayer (
        Serializer &serializer,
        ObjectLayer *object_layer,
        CreateEntityFunction CreateEntity);

    typedef std::list<WorldView *> WorldViewList;

    // list of WorldViews
    WorldViewList m_world_view_list;
    // the physics handler for this world
    PhysicsHandler *const m_physics_handler;
    // list of ObjectLayers, starting at the back-most layer
    ObjectLayerList m_object_layer_list;
    // the "main" object layer
    ObjectLayer *m_main_object_layer;
    // controls how fast world time moves compared to real time
    Float m_timescale;

private:

    void WriteObjectLayers (Serializer &serializer) const;
    void WriteDynamicObjectsBelongingToLayer (
        Serializer &serializer,
        ObjectLayer const *object_layer) const;

    typedef std::vector<Entity *> EntityVector;

    // array of dynamic objects (Object with attached Entity)
    EntityVector m_entity_vector;
    // lowest available entity number
    EntityWorldIndex m_lowest_available_entity_index;
    // number of entities in this World
    Uint32 m_entity_count;
    // the world's own private event queue
    EventQueue m_owner_event_queue;
}; // end of class Engine2::World

} // end of namespace Engine2
} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_WORLD_HPP_)
