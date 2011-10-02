// ///////////////////////////////////////////////////////////////////////////
// dis_entityreference.hpp by Victor Dods, created 2006/02/21
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_GAMEOBJECTREFERENCE_HPP_)
#define _DIS_GAMEOBJECTREFERENCE_HPP_

#include "xrb.hpp"

using namespace Xrb;

namespace Dis {

class Entity;

// the constructor in EntityInstance is private so only
// EntityReference<> and Entity can use it.
class EntityInstance
{
private:

    EntityInstance (Entity *entity)
    {
        ASSERT1(entity != NULL);
        m_entity = entity;
        m_reference_count = 0;
    }
    ~EntityInstance ()
    {
        ASSERT1(m_reference_count == 0);
    }

    Entity const *operator * () const { return m_entity; }
    Entity *operator * () { return m_entity; }

    bool IsValid () const { return m_entity != NULL; }
    Uint32 ReferenceCount () const { return m_reference_count; }

    // these should be called only by EntityReference
    void NullifyEntity ()
    {
        ASSERT1(m_entity != NULL);
        m_entity = NULL;
    }
    void IncrementReferenceCount ()
    {
        ++m_reference_count;
    }
    void DecrementReferenceCount ()
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

    EntityReference ()
    {
        m_entity_instance = NULL;
    }
    template <typename OperandEntitySubclass>
    EntityReference (EntityReference<OperandEntitySubclass> const &entity_reference)
    {
        // check that either the entity_instance is NULL, the game object
        // is NULL, or the template type cast is valid.
        ASSERT1(entity_reference.m_entity_instance == NULL ||
                !entity_reference.m_entity_instance->IsValid() ||
                dynamic_cast<EntitySubclass *>(**entity_reference.m_entity_instance) != NULL);
        // copy the entity_instance
        m_entity_instance = entity_reference.m_entity_instance;
        if (m_entity_instance != NULL)
            m_entity_instance->IncrementReferenceCount();
    }
    // this copy constructor is necessary because the above, templatized
    // constructor does not act as a copy constructor
    EntityReference (EntityReference<EntitySubclass> const &entity_reference)
    {
        // copy the entity_instance
        m_entity_instance = entity_reference.m_entity_instance;
        if (m_entity_instance != NULL)
            m_entity_instance->IncrementReferenceCount();
    }    
    ~EntityReference ()
    {
        Release();
    }

    template <typename OperandEntitySubclass>
    bool operator == (EntityReference<OperandEntitySubclass> const &entity_reference) const
    {
        return m_entity_instance == entity_reference.m_entity_instance;
    }
    template <typename OperandEntitySubclass>
    bool operator != (EntityReference<OperandEntitySubclass> const &entity_reference) const
    {
        return m_entity_instance != entity_reference.m_entity_instance;
    }

    template <typename OperandEntitySubclass>
    void operator = (EntityReference<OperandEntitySubclass> const &entity_reference)
    {
        // check that either the entity_instance is NULL, the game object
        // is NULL, or the template type cast is valid.
        ASSERT1(entity_reference.m_entity_instance == NULL ||
                !entity_reference.m_entity_instance->IsValid() ||
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
    void operator = (EntityReference<EntitySubclass> const &entity_reference)
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

    EntitySubclass const *operator * () const
    {
        if (m_entity_instance != NULL && m_entity_instance->IsValid())
            return static_cast<EntitySubclass const *>(**m_entity_instance);
        else
            return NULL;
    }
    EntitySubclass *operator * ()
    {
        if (m_entity_instance != NULL && m_entity_instance->IsValid())
            return static_cast<EntitySubclass *>(**m_entity_instance);
        else
            return NULL;
    }
    EntitySubclass const *operator -> () const
    {
        ASSERT1(m_entity_instance != NULL);
        ASSERT1(m_entity_instance->IsValid());
        return static_cast<EntitySubclass const *>(**m_entity_instance);
    }
    EntitySubclass *operator -> ()
    {
        ASSERT1(m_entity_instance != NULL);
        ASSERT1(m_entity_instance->IsValid());
        return static_cast<EntitySubclass *>(**m_entity_instance);
    }

    bool IsValid () const
    {
        return m_entity_instance != NULL && m_entity_instance->IsValid();
    }

    void SetInstance (EntityInstance *entity_instance)
    {
        ASSERT1(entity_instance != NULL);

        // decrement the old instance's ref count (if it exists)
        if (m_entity_instance != NULL)
            m_entity_instance->DecrementReferenceCount();

        // set the game object instance and increment the ref count
        m_entity_instance = entity_instance;
        m_entity_instance->IncrementReferenceCount();
    }
    void Release ()
    {
        if (m_entity_instance != NULL)
        {
            m_entity_instance->DecrementReferenceCount();
            if (m_entity_instance->ReferenceCount() == 0)
                delete m_entity_instance;
            m_entity_instance = NULL;
        }
    }

private:

    // this should only be called by ~Entity    
    void NullifyEntity ()
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

#endif // !defined(_DIS_GAMEOBJECTREFERENCE_HPP_)

