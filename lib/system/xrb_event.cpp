// ///////////////////////////////////////////////////////////////////////////
// xrb_event.cpp by Victor Dods, created 2004/09/11
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_event.hpp"

#include "xrb_gui_events.hpp"
#include "xrb_input_events.hpp"
#include "xrb_inputstate.hpp"

namespace Xrb
{

Event::~Event () { }

std::string const &Event::Name (EventType const event_type)
{
    static std::string const s_event_type_name[] =
    {
        "DUMMY",
        "KEYDOWN",
        "KEYUP",
        "KEYREPEAT",
        "MOUSEBUTTONDOWN",
        "MOUSEBUTTONUP",
        "MOUSEWHEEL",
        "MOUSEMOTION",
        "JOYAXIS",
        "JOYBALL",
        "JOYBUTTONDOWN",
        "JOYBUTTONUP",
        "JOYHAT",
        "FOCUS",
        "MOUSEOVER",
        "DETACH_AND_DELETE_CHILD_WIDGET",
        "QUIT",
        "STATE_MACHINE_INPUT",
        "ENGINE2_DELETE_ENTITY",
        "ENGINE2_REMOVE_ENTITY_FROM_WORLD"
        "CUSTOM"
    };
    DEBUG1_CODE(static Uint32 const s_event_type_name_count = LENGTHOF(s_event_type_name));
    ASSERT1(static_cast<Uint32>(event_type) < s_event_type_name_count);
    return s_event_type_name[static_cast<Uint32>(event_type)];
}

EventCustom::~EventCustom () { }

// ///////////////////////////////////////////////////////////////////////////
// event-matching functions for use in EventQueue
// ///////////////////////////////////////////////////////////////////////////

bool MatchEventType (Event const *event, Event::EventType const event_type)
{
    ASSERT1(event != NULL);

    return event->GetEventType() == event_type;
}

bool MatchCustomType (Event const *event, EventCustom::CustomType const custom_type)
{
    ASSERT1(event != NULL);

    return dynamic_cast<EventCustom const *>(event) != NULL &&
           static_cast<EventCustom const *>(event)->GetCustomType() == custom_type;
}

} // end of namespace Xrb
