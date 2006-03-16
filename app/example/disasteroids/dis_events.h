// ///////////////////////////////////////////////////////////////////////////
// dis_events.h by Victor Dods, created 2006/01/11
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_EVENTS_H_)
#define _DIS_EVENTS_H_

#include "xrb_event.h"

#include "dis_world.h"

using namespace Xrb;

namespace Dis
{

class EventBase : public EventCustom
{
public:

    enum SubType
    {
        // World events
        ST_SET_GAME_STATE = 0,
        // Master events
        ST_ACTIVATE_TITLE_SCREEN,
        ST_DEACTIVATE_TITLE_SCREEN,
        ST_ACTIVATE_GAME,
        ST_DEACTIVATE_GAME,
        ST_QUIT_REQUESTED,

        ST_COUNT
    }; // end of EventBase::SubType

    EventBase (SubType sub_type, Float time)
        :
        EventCustom(time),
        m_sub_type(sub_type)
    {
        ASSERT1(m_sub_type < ST_COUNT)
    }
    virtual ~EventBase () { }

    inline SubType GetSubType () const { return m_sub_type; }

private:

    SubType m_sub_type;
}; // end of class EventBase

class EventSetGameState : public EventBase
{
public:

    EventSetGameState (World::GameState game_state, Float time)
        :
        EventBase(ST_SET_GAME_STATE, time),
        m_game_state(game_state)
    {
        ASSERT1(m_game_state < World::GS_COUNT)
    }

    inline World::GameState GetGameState () const { return m_game_state; }

private:

    World::GameState m_game_state;
}; // end of class EventSetGameState

} // end of namespace Dis

#endif // !defined(_DIS_EVENTS_H_)

