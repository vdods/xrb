// ///////////////////////////////////////////////////////////////////////////
// dis_gameobjectreference.h by Victor Dods, created 2006/02/21
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_GAMEOBJECTREFERENCE_H_)
#define _DIS_GAMEOBJECTREFERENCE_H_

#include "xrb.h"

using namespace Xrb;

namespace Dis
{

class GameObject;

// the constructor in GameObjectInstance is private so only
// GameObjectReference<> and GameObject can use it.
class GameObjectInstance
{
private:

    inline GameObjectInstance (GameObject *game_object)
    {
        ASSERT1(game_object != NULL)
        m_game_object = game_object;
        m_reference_count = 0;
    }
    inline ~GameObjectInstance ()
    {
        ASSERT1(m_reference_count == 0)
    }

    inline GameObject const *operator * () const { return m_game_object; }
    inline GameObject *operator * () { return m_game_object; }

    inline bool GetIsValid () const { return m_game_object != NULL; }
    inline Uint32 GetReferenceCount () const { return m_reference_count; }

    // these should be called only by GameObjectReference
    inline void NullifyGameObject ()
    {
        ASSERT1(m_game_object != NULL)
        m_game_object = NULL;
    }
    inline void IncrementReferenceCount ()
    {
        ++m_reference_count;
    }
    inline void DecrementReferenceCount ()
    {
        ASSERT1(m_reference_count > 0)
        --m_reference_count;
    }

    GameObject *m_game_object;
    Uint32 m_reference_count;

    friend class GameObject;
    template <typename GameObjectSubclass> friend class GameObjectReference;
}; // end of class GameObjectInstance

template <typename GameObjectSubclass>
class GameObjectReference
{
public:

    static GameObjectReference<GameObjectSubclass> const ms_null;

    inline GameObjectReference ()
    {
        m_game_object_instance = NULL;
    }
    template <typename OperandGameObjectSubclass>
    inline GameObjectReference (GameObjectReference<OperandGameObjectSubclass> const &game_object_reference)
    {
        // check that either the game_object_instance is NULL, the game object
        // is NULL, or the template type cast is valid.
        ASSERT1(game_object_reference.m_game_object_instance == NULL ||
                !game_object_reference.m_game_object_instance->GetIsValid() ||
                dynamic_cast<GameObjectSubclass *>(**game_object_reference.m_game_object_instance) != NULL)
        // copy the game_object_instance
        m_game_object_instance = game_object_reference.m_game_object_instance;
        if (m_game_object_instance != NULL)
            m_game_object_instance->IncrementReferenceCount();
    }
    // this copy constructor is necessary because the above, templatized
    // constructor does not act as a copy constructor
    inline GameObjectReference (GameObjectReference<GameObjectSubclass> const &game_object_reference)
    {
        // copy the game_object_instance
        m_game_object_instance = game_object_reference.m_game_object_instance;
        if (m_game_object_instance != NULL)
            m_game_object_instance->IncrementReferenceCount();
    }    
    inline ~GameObjectReference ()
    {
        Release();
    }

    template <typename OperandGameObjectSubclass>
    inline bool operator == (GameObjectReference<OperandGameObjectSubclass> const &game_object_reference) const
    {
        return m_game_object_instance == game_object_reference.m_game_object_instance;
    }
    template <typename OperandGameObjectSubclass>
    inline bool operator != (GameObjectReference<OperandGameObjectSubclass> const &game_object_reference) const
    {
        return m_game_object_instance != game_object_reference.m_game_object_instance;
    }

    template <typename OperandGameObjectSubclass>
    inline void operator = (GameObjectReference<OperandGameObjectSubclass> const &game_object_reference)
    {
        // check that either the game_object_instance is NULL, the game object
        // is NULL, or the template type cast is valid.
        ASSERT1(game_object_reference.m_game_object_instance == NULL ||
                !game_object_reference.m_game_object_instance->GetIsValid() ||
                dynamic_cast<GameObjectSubclass *>(**game_object_reference.m_game_object_instance) != NULL)

        // decrement the old instance's ref count (if it exists)
        if (m_game_object_instance != NULL)
            m_game_object_instance->DecrementReferenceCount();
                
        // copy the game_object_instance and increment the instance's ref
        // count (if it exists)
        m_game_object_instance = game_object_reference.m_game_object_instance;
        if (m_game_object_instance != NULL)
            m_game_object_instance->IncrementReferenceCount();
    }

    inline GameObjectSubclass const *operator * () const
    {
        if (m_game_object_instance != NULL && m_game_object_instance->GetIsValid())
            return static_cast<GameObjectSubclass const *>(**m_game_object_instance);
        else
            return NULL;
    }
    inline GameObjectSubclass *operator * ()
    {
        if (m_game_object_instance != NULL && m_game_object_instance->GetIsValid())
            return static_cast<GameObjectSubclass *>(**m_game_object_instance);
        else
            return NULL;
    }
    inline GameObjectSubclass const *operator -> () const
    {
        ASSERT1(m_game_object_instance != NULL)
        ASSERT1(m_game_object_instance->GetIsValid())
        return static_cast<GameObjectSubclass const *>(**m_game_object_instance);
    }
    inline GameObjectSubclass *operator -> ()
    {
        ASSERT1(m_game_object_instance != NULL)
        ASSERT1(m_game_object_instance->GetIsValid())
        return static_cast<GameObjectSubclass *>(**m_game_object_instance);
    }

    inline bool GetIsValid () const
    {
        return m_game_object_instance != NULL && m_game_object_instance->GetIsValid();
    }

    inline void SetInstance (GameObjectInstance *game_object_instance)
    {
        ASSERT1(game_object_instance != NULL)

        // decrement the old instance's ref count (if it exists)
        if (m_game_object_instance != NULL)
            m_game_object_instance->DecrementReferenceCount();

        // set the game object instance and increment the ref count
        m_game_object_instance = game_object_instance;
        m_game_object_instance->IncrementReferenceCount();
    }
    inline void Release ()
    {
        if (m_game_object_instance != NULL)
        {
            m_game_object_instance->DecrementReferenceCount();
            if (m_game_object_instance->GetReferenceCount() == 0)
                delete m_game_object_instance;
            m_game_object_instance = NULL;
        }
    }

private:

    // this should only be called by ~GameObject    
    inline void NullifyGameObject ()
    {
        ASSERT1(m_game_object_instance != NULL)
        m_game_object_instance->NullifyGameObject();
    }

    GameObjectInstance *m_game_object_instance;

    friend class GameObject;
    template <typename OtherGameObjectSubclass> friend class GameObjectReference;
}; // end of class GameObjectReference

template <typename GameObjectSubclass>
GameObjectReference<GameObjectSubclass> const GameObjectReference<GameObjectSubclass>::ms_null;

} // end of namespace Dis

#endif // !defined(_DIS_GAMEOBJECTREFERENCE_H_)

