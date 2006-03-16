// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_world.h by Victor Dods, created 2004/06/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_WORLD_H_)
#define _XRB_ENGINE2_WORLD_H_

#include "xrb.h"

#include <list>
#include <set>
#include <vector>

#include "xrb_engine2_entity.h"
#include "xrb_eventhandler.h"
#include "xrb_framehandler.h"

namespace Xrb
{

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
namespace Engine2
{

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
            DEFAULT_ENTITY_CAPACITY = 0x1000
        };
    
        typedef std::list<ObjectLayer *> ObjectLayerList;
        typedef ObjectLayerList::iterator ObjectLayerListIterator;
        typedef ObjectLayerList::const_iterator ObjectLayerListConstIterator;
        typedef ObjectLayerList::reverse_iterator ObjectLayerListReverseIterator;
    
        virtual ~World ();
    
        static World *Create (
            Serializer &serializer,
            EventQueue *owner_event_queue,
            PhysicsHandler *physics_handler);
        static World *CreateEmpty (
            EventQueue *owner_event_queue,
            PhysicsHandler *physics_handler,
            Uint32 entity_capacity = DEFAULT_ENTITY_CAPACITY);
        void Write (Serializer &serializer) const;
    
        inline ObjectLayerList &GetObjectLayerList ()
        {
            return m_object_layer_list;
        }
        inline Uint32 GetEntityCapacity () const
        {
            return m_entity_vector.capacity();
        }
        virtual ObjectLayer const *GetMainObjectLayer () const
        {
            return m_main_object_layer;
        }
        virtual ObjectLayer *GetMainObjectLayer ()
        {
            return m_main_object_layer;
        }
        inline PhysicsHandler *GetPhysicsHandler ()
        {
            return m_physics_handler;
        }
        inline Float GetTimescale ()
        {
            return m_timescale;
        }

        inline void SetTimescale (Float timescale)
        {
            ASSERT1(timescale >= 0.0f)
            m_timescale = timescale;
        }

        // you must call this method after creating a WorldView
        void AttachWorldView (WorldView *world_view);
        // you must call this method before destroying a WorldView
        void DetachWorldView (WorldView *world_view);
        
        void AddObjectLayer (ObjectLayer *object_layer);
        void SetMainObjectLayer (ObjectLayer *main_object_layer);
            
        void AddEntity (Entity *entity, ObjectLayer *object_layer);
        void AddObject (Object *object, ObjectLayer *object_layer);
        void RemoveEntity (Entity *entity);

    protected:

        World (
            EventQueue *owner_event_queue,
            PhysicsHandler *physics_handler,
            Uint32 entity_capacity = DEFAULT_ENTITY_CAPACITY);

        virtual Uint32 GetMainObjectLayerIndex () const;
    
        virtual void SetMainObjectLayerIndex (Uint32 index);

        // process an event    
        virtual bool ProcessEventOverride (Event const *e);
        // this calculates one frame, called by the game loop
        virtual void ProcessFrameOverride ();
        void IncrementLowestAvailableEntityNumber ();
        void UpdateLowestAvailableEntityNumber (Sint32 removed_entity_number);
    
        // this function should be called only from constructors (or Create)
        void Read (Serializer &serializer);

        // this function is called after the worldview is added to the
        // worldview list during AttachWorldView.
        virtual void HandleAttachWorldView (WorldView *world_view) { }
        // this function is called before the worldview is removed from the
        // worldview list during DetachWorldView.
        virtual void HandleDetachWorldView (WorldView *world_view) { }
        
        virtual void ReadObjectLayers (Serializer &serializer);
        virtual void ReadEntitiesBelongingToLayer (
            Serializer &serializer,
            ObjectLayer *object_layer);
    
        typedef std::list<WorldView *> WorldViewList;
        typedef WorldViewList::iterator WorldViewListIterator;
                
        // list of WorldViews
        WorldViewList m_world_view_list;
        // list of ObjectLayers, starting at the back-most layer
        ObjectLayerList m_object_layer_list;
        // the "main" object layer
        ObjectLayer *m_main_object_layer;
        // controls how fast world time moves compared to real time
        Float m_timescale;
    
    private:
    
        void WriteObjectLayers (Serializer &serializer) const;
        void WriteEntitiesBelongingToLayer (
            Serializer &serializer,
            ObjectLayer const *object_layer) const;

        typedef std::vector<Entity *> EntityVector;
        typedef EntityVector::iterator EntityVectorIterator;
        typedef EntityVector::const_iterator EntityVectorConstIterator;

        // entity array
        EntityVector m_entity_vector;
        // lowest available entity number
        Entity::Index m_lowest_available_entity_number;
        // the physics handler for this world
        PhysicsHandler *m_physics_handler;    
    }; // end of class Engine2::World

} // end of namespace Engine2

} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_WORLD_H_)
