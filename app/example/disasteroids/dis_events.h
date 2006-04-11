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
        // Master events
        ACTIVATE_TITLE_SCREEN = 0,
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

} // end of namespace Dis

#endif // !defined(_DIS_EVENTS_H_)

