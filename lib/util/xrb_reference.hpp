// ///////////////////////////////////////////////////////////////////////////
// xrb_reference.hpp by Victor Dods, created 2005/06/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_REFERENCE_HPP_)
#define _XRB_REFERENCE_HPP_

#include "xrb.hpp"

namespace Xrb
{

// ///////////////////////////////////////////////////////////////////////////
// Instance<T>
// ///////////////////////////////////////////////////////////////////////////

template <typename T>
class Instance
{
public:

    inline Instance ()
    {
        m_reference_count = 0;
    }
    inline Instance (T const &content)
        :
        m_content(content)
    {
        m_reference_count = 0;
    }
    inline Instance (Instance<T> const &source)
    {
        ASSERT0(false && "Don't use Instance<T> this way");
    }
    inline ~Instance ()
    {
        ASSERT1(m_reference_count == 0);
    }

    inline void operator = (Instance<T> const &operand)
    {
        ASSERT0(false && "Don't use Instance<T> this way");
    }

    inline T const &operator * () const
    {
        return m_content;
    }
    inline T &operator * ()
    {
        return m_content;
    }
    inline T const *operator -> () const
    {
        return &m_content;
    }
    inline T *operator -> ()
    {
        return &m_content;
    }

    inline Uint32 GetReferenceCount () const
    {
        return m_reference_count;
    }

    inline void IncrementReferenceCount ()
    {
        ASSERT1(m_reference_count < 0xFFFFFFFF);
        ++m_reference_count;
    }
    inline void DecrementReferenceCount ()
    {
        ASSERT1(m_reference_count > 0);
        --m_reference_count;
    }

private:

    Uint32 m_reference_count;
    T m_content;
}; // end of class Instance<T>

// ///////////////////////////////////////////////////////////////////////////
// Reference<T>
// ///////////////////////////////////////////////////////////////////////////

template <typename T>
class Reference
{
public:

    inline Reference ()
    {
        m_instance = NULL;
    }
    inline Reference (Instance<T> *const instance)
    {
        ASSERT1(instance != NULL);
        m_instance = instance;
        m_instance->IncrementReferenceCount();
    }
    inline Reference (Reference<T> const &source)
    {
        m_instance = source.m_instance;
        if (m_instance != NULL)
            m_instance->IncrementReferenceCount();
    }
    inline ~Reference ()
    {
        if (m_instance != NULL)
            m_instance->DecrementReferenceCount();
    }

    inline void operator = (Instance<T> *const instance)
    {
        if (m_instance != NULL)
            m_instance->DecrementReferenceCount();

        ASSERT1(instance != NULL);
        m_instance = instance;
        if (m_instance != NULL)
            m_instance->IncrementReferenceCount();
    }
    inline void operator = (Reference<T> const &operand)
    {
        if (m_instance != NULL)
            m_instance->DecrementReferenceCount();

        m_instance = operand.m_instance;
        if (m_instance != NULL)
            m_instance->IncrementReferenceCount();
    }

    inline T const &operator * () const
    {
        ASSERT1(m_instance != NULL);
        Instance<T> const *instance = m_instance;
        return **instance;
    }
    inline T &operator * ()
    {
        ASSERT1(m_instance != NULL);
        return **m_instance;
    }

    inline T const *operator -> () const
    {
        ASSERT1(m_instance != NULL);
        Instance<T> const *instance = m_instance;
        return &**instance;
    }
    inline T *operator -> ()
    {
        ASSERT1(m_instance != NULL);
        return &**m_instance;
    }

    inline bool operator == (Instance<T> *const instance)
    {
        return m_instance == instance;
    }

    inline bool GetIsValid () const
    {
        return m_instance != NULL;
    }
    inline Uint32 GetReferenceCount () const
    {
        if (m_instance != NULL)
            return m_instance->GetReferenceCount();
        else
            return 0;
    }
    inline void Release ()
    {
        if (m_instance != NULL)
            m_instance->DecrementReferenceCount();
        m_instance = NULL;
    }

private:

    Instance<T> *m_instance;
}; // end of class Reference<T>

} // end of namespace Xrb

#endif // !defined(_XRB_REFERENCE_HPP_)

