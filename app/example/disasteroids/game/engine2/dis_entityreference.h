// ///////////////////////////////////////////////////////////////////////////
// dis_entityreference.h by Victor Dods, created 2006/02/21
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

class Entity;

// the constructor in EntityInstance is private so only
// EntityReference<> and Entity can use it.
class EntityInstance
{
private:

    inline EntityInstance (Entity *entity)
    {
        ASSERT1(entity != NULL);
        m_entity = entity;
        m_reference_count = 0;
    }
    inline ~EntityInstance ()
    {
        ASSERT1(m_reference_count == 0);
    }

    inline Entity const *operator * () const { return m_entity; }
    inline Entity *operator * () { return m_entity; }

    inline bool GetIsValid () const { return m_entity != NULL; }
    inline Uint32 GetReferenceCount () const { return m_reference_count; }

    // these should be called only by EntityReference
    inline void NullifyEntity ()
    {
        ASSERT1(m_entity != NULL);
        m_entity = NULL;
    }
    inline void IncrementReferenceCount ()
    {
        ++m_reference_count;
    }
    inline void DecrementReferenceCount ()
    {
        ASSERT1(m_reference_count > 0);
        --m_reference_count;
    }

    Entity *m_entity;
    Uint32 m_reference_count;

    friend class Entity;
    template <typename EntitySubclass> friend class EntityReference;
}; // end of class EntityInstance

template <typename EntitySubclass>
class EntityReference
{
public:

    static EntityReference<EntitySubclass> const ms_null;

    inline EntityReference ()
    {
        m_entity_instance = NULL;
    }
    template <typename OperandEntitySubclass>
    inline EntityReference (EntityReference<OperandEntitySubclass> const &entity_reference)
    {
        // check that either the entity_instance is NULL, the game object
        // is NULL, or the template type cast is valid.
        ASSERT1(entity_reference.m_entity_instance == NULL ||
                !entity_reference.m_entity_instance->GetIsValid() ||
                dynamic_cast<EntitySubclass *>(**entity_reference.m_entity_instance) != NULL);
        // copy the entity_instance
        m_entity_instance = entity_reference.m_entity_instance;
        if (m_entity_instance != NULL)
            m_entity_instance->IncrementReferenceCount();
    }
    // this copy constructor is necessary because the above, templatized
    // constructor does not act as a copy constructor
    inline EntityReference (EntityReference<EntitySubclass> const &entity_reference)
    {
        // copy the entity_instance
        m_entity_instance = entity_reference.m_entity_instance;
        if (m_entity_instance != NULL)
            m_entity_instance->IncrementReferenceCount();
    }    
    inline ~EntityReference ()
    {
        Release();
    }

    template <typename OperandEntitySubclass>
    inline bool operator == (EntityReference<OperandEntitySubclass> const &entity_reference) const
    {
        return m_entity_instance == entity_reference.m_entity_instance;
    }
    template <typename OperandEntitySubclass>
    inline bool operator != (EntityReference<OperandEntitySubclass> const &entity_reference) const
    {
        return m_entity_instance != entity_reference.m_entity_instance;
    }

    template <typename OperandEntitySubclass>
    inline void operator = (EntityReference<OperandEntitySubclass> const &entity_reference)
    {
        // check that either the entity_instance is NULL, the game object
        // is NULL, or the template type cast is valid.
        ASSERT1(entity_reference.m_entity_instance == NULL ||
                !entity_reference.m_entity_instance->GetIsValid() ||
                dynamic_cast<EntitySubclass *>(**entity_reference.m_entity_instance) != NULL);

        // decrement the old instance's ref count (if it exists)
        if (m_entity_instance != NULL)
            m_entity_instance->DecrementReferenceCount();                
        // copy the entity_instance and increment the instance's ref
        // count (if it exists)
        m_entity_instance = entity_reference.m_entity_instance;
        if (m_entity_instance != NULL)
            m_entity_instance->IncrementReferenceCount();
    }
    // this assignment operator overload is necessary because the above,
    // templatized assignment operator overload goes all wacky when used
    // with the same EntitySubClass.
    inline void operator = (EntityReference<EntitySubclass> const &entity_reference)
    {
        // decrement the old instance's ref count (if it exists)
        if (m_entity_instance != NULL)
            m_entity_instance->DecrementReferenceCount();
        // copy the entity_instance and increment the instance's ref
        // count (if it exists)
        m_entity_instance = entity_reference.m_entity_instance;
        if (m_entity_instance != NULL)
            m_entity_instance->IncrementReferenceCount();
    }

    inline EntitySubclass const *operator * () const
    {
        if (m_entity_instance != NULL && m_entity_instance->GetIsValid())
            return static_cast<EntitySubclass const *>(**m_entity_instance);
        else
            return NULL;
    }
    inline EntitySubclass *operator * ()
    {
        if (m_entity_instance != NULL && m_entity_instance->GetIsValid())
            return static_cast<EntitySubclass *>(**m_entity_instance);
        else
            return NULL;
    }
    inline EntitySubclass const *operator -> () const
    {
        ASSERT1(m_entity_instance != NULL);
        ASSERT1(m_entity_instance->GetIsValid());
        return static_cast<EntitySubclass const *>(**m_entity_instance);
    }
    inline EntitySubclass *operator -> ()
    {
        ASSERT1(m_entity_instance != NULL);
        ASSERT1(m_entity_instance->GetIsValid());
        return static_cast<EntitySubclass *>(**m_entity_instance);
    }

    inline bool GetIsValid () const
    {
        return m_entity_instance != NULL && m_entity_instance->GetIsValid();
    }

    inline void SetInstance (EntityInstance *entity_instance)
    {
        ASSERT1(entity_instance != NULL);

        // decrement the old instance's ref count (if it exists)
        if (m_entity_instance != NULL)
            m_entity_instance->DecrementReferenceCount();

        // set the game object instance and increment the ref count
        m_entity_instance = entity_instance;
        m_entity_instance->IncrementReferenceCount();
    }
    inline void Release ()
    {
        if (m_entity_instance != NULL)
        {
            m_entity_instance->DecrementReferenceCount();
            if (m_entity_instance->GetReferenceCount() == 0)
                delete m_entity_instance;
            m_entity_instance = NULL;
        }
    }

private:

    // this should only be called by ~Entity    
    inline void NullifyEntity ()
    {
        ASSERT1(m_entity_instance != NULL);
        m_entity_instance->NullifyEntity();
    }

    EntityInstance *m_entity_instance;

    friend class Entity;
    template <typename OtherEntitySubclass> friend class EntityReference;
}; // end of class EntityReference

template <typename EntitySubclass>
EntityReference<EntitySubclass> const EntityReference<EntitySubclass>::ms_null;

} // end of namespace Dis

#endif // !defined(_DIS_GAMEOBJECTREFERENCE_H_)

