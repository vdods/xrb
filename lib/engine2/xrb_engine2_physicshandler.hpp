// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_physicshandler.hpp by Victor Dods, created 2005/04/14
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_PHYSICSHANDLER_HPP_)
#define _XRB_ENGINE2_PHYSICSHANDLER_HPP_

#include "xrb.hpp"

#include "xrb_engine2_enums.hpp"
#include "xrb_framehandler.hpp"
#include "xrb_vector.hpp"

namespace Xrb
{

namespace Engine2
{

class Entity;
class ObjectLayer;
class World;

// abstract interface class for use by World.
class PhysicsHandler : public FrameHandler
{
public:

    PhysicsHandler ()
    {
        m_owner_world = NULL;
    }
    virtual ~PhysicsHandler ()
    {
        m_owner_world = NULL;
    }

    inline World *GetOwnerWorld () const
    {
        return m_owner_world;
    }

    virtual void AddObjectLayer (ObjectLayer *object_layer) = 0;
    virtual void SetMainObjectLayer (ObjectLayer *object_layer) = 0;

    virtual void AddEntity (Entity *entity) = 0;
    virtual void RemoveEntity (Entity *entity) = 0;

protected:

    virtual void HandleFrame () = 0;

    inline void SetOwnerWorld (World *const owner_world)
    {
        ASSERT1(owner_world != NULL);
        m_owner_world = owner_world;
    }

private:

    World *m_owner_world;

    // this is so World can set m_owner_world
    // the physics handler is passed to it.
    friend class World;
}; // end of class Engine2::PhysicsHandler

} // end of namespace Engine2

} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_PHYSICSHANDLER_HPP_)
