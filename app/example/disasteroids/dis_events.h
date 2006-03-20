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

    // custom event types
    enum
    {
        // World events
        SET_GAME_STATE = 0,
        // Master events
        ACTIVATE_TITLE_SCREEN,
        DEACTIVATE_TITLE_SCREEN,
        ACTIVATE_GAME,
        DEACTIVATE_GAME,
        QUIT_REQUESTED
    }; // end of EventBase::SubType

    EventBase (CustomType custom_type, Float time)
        :
        EventCustom(custom_type, time)
    { }
    virtual ~EventBase () { }
}; // end of class EventBase

class EventSetGameState : public EventBase
{
public:

    EventSetGameState (World::GameState game_state, Float time)
        :
        EventBase(SET_GAME_STATE, time),
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

