// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_world.hpp by Victor Dods, created 2005/02/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_MAPEDITOR2_WORLD_HPP_)
#define _XRB_MAPEDITOR2_WORLD_HPP_

#include "xrb.hpp"

#include "xrb_engine2_world.hpp"
#include "xrb_signalhandler.hpp"

namespace Xrb
{

namespace MapEditor2
{

    class Object;
    class ObjectLayer;
    
    class World : public Engine2::World, public SignalHandler
    {
    public:
    
        virtual ~World () { }
    
        // creates a World by reading from the given serializer
        static World *Create (Serializer &serializer);
        // creates an empty World
        static World *CreateEmpty (Uint32 entity_capacity = DEFAULT_ENTITY_CAPACITY);
    
        void IncrementMainObjectLayer ();
        void DecrementMainObjectLayer ();
    
        inline bool IsRunning () const
        {
            return m_is_running;
        }
        inline ObjectLayer *SavedMainObjectLayer () const
        {
            return m_saved_main_object_layer;
        }
    
        inline void SetIsRunning (bool const is_running)
        {
            m_is_running = is_running;
        }
        inline void SetSavedMainObjectLayer (
            ObjectLayer *const saved_main_object_layer)
        {
            ASSERT1(saved_main_object_layer != NULL);
            m_saved_main_object_layer = saved_main_object_layer;
        }
    
        inline SignalSender0 const *SenderMainObjectLayerChanged ()
        {
            return &m_sender_main_object_layer_changed;
        }
    
        void RemoveObject (Object *object);
    
    protected:

        World (Uint32 entity_capacity);
        
        virtual Uint32 MainObjectLayerIndex () const;
    
        virtual void SetMainObjectLayerIndex (Uint32 index);
    
        // this calculates one frame, called by the game loop
        virtual void HandleFrame ();
    
        virtual void ReadObjectLayers (Serializer &serializer);
        virtual void ReadEntitiesBelongingToLayer (
            Serializer &serializer,
            ObjectLayer *object_layer);
    
    private:
    
        ObjectLayerList::iterator ObjectLayer::iterator (ObjectLayer *object_layer);
    
        // indicates if the world is running a simulation
        bool m_is_running;
        // the object layer which is the 'main' layer, saved for
        // later, because m_main_object_layer needs to change
        // as the editor switches current editable layers
        ObjectLayer *m_saved_main_object_layer;
    
        //////////////////////////////////////////////////////////////////////////
        // SignalSenders
        SignalSender0 m_sender_main_object_layer_changed;
    }; // end of class MapEditor2::World

} // end of namespace MapEditor2

} // end of namespace Xrb

#endif // !defined(_XRB_MAPEDITOR2_WORLD_HPP_)

