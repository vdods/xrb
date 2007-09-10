// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_events.h by Victor Dods, created 2005/11/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_EVENTS_H_)
#define _XRB_ENGINE2_EVENTS_H_

#include "xrb_event.h"

namespace Xrb
{

namespace Engine2
{

class Entity;
class ObjectLayer;

bool MatchEntity (Event const *event, Entity *entity_to_match);

class EventEntity : public Event
{
public:

    EventEntity (Entity *entity, Float time, EventType event_type)
        :
        Event(time, event_type)
    {
        ASSERT1(entity != NULL);
        m_entity = entity;
    }
    virtual ~EventEntity () { }

    inline Entity *GetEntity () const { return m_entity; }

    inline void NullifyEntity () const
    {
        ASSERT1(m_entity != NULL);
        m_entity = NULL;
    }

private:

    mutable Entity *m_entity;
}; // end of class Engine2::EntityEvent

} // end of namespace Engine2

} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_EVENTS_H_)

