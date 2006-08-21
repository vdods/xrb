// ///////////////////////////////////////////////////////////////////////////
// xrb_event.cpp by Victor Dods, created 2004/09/11
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_event.h"

#include "xrb_gui_events.h"
#include "xrb_input_events.h"
#include "xrb_input.h"

namespace Xrb
{

Event::~Event () { }

std::string const &Event::GetName (EventType const event_type)
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
        "DELETE_CHILD_WIDGET",
        "ACTIVE",
        "RESIZE",
        "EXPOSE",
        "QUIT",
        "SYSWM",
        "STATE_MACHINE_INPUT",
        "ENGINE2_DELETE_ENTITY",
        "ENGINE2_REMOVE_ENTITY_FROM_WORLD"
        "CUSTOM"
    };
    static Uint32 const s_event_type_name_count = sizeof(s_event_type_name) / sizeof(std::string);

    ASSERT1(event_type < s_event_type_name_count)
    return s_event_type_name[event_type];
}

Event *Event::CreateEventFromSDLEvent (
    SDL_Event const *const e,
    Screen const *const screen,
    Float const time)
{
    ASSERT1(e != NULL)
    ASSERT1(screen != NULL)
    ASSERT1(time >= 0.0)

    Event *retval = NULL;

    switch (e->type)
    {
        case SDL_ACTIVEEVENT:
            retval = new EventActive(
                reinterpret_cast<SDL_ActiveEvent const *>(e),
                time);
            break;

        case SDL_KEYDOWN:
            retval = new EventKeyDown(
                reinterpret_cast<SDL_KeyboardEvent const *>(e),
                time);
            break;

        case SDL_KEYUP:
            retval = new EventKeyUp(
                reinterpret_cast<SDL_KeyboardEvent const *>(e),
                time);
            break;

        case SDL_MOUSEBUTTONDOWN:
        {
            SDL_MouseButtonEvent const *mouse_button_event =
                reinterpret_cast<SDL_MouseButtonEvent const *>(e);
            if (mouse_button_event->button == Key::MOUSEWHEELUP ||
                mouse_button_event->button == Key::MOUSEWHEELDOWN)
            {
                retval = new EventMouseWheel(
                    mouse_button_event,
                    Singletons::Input().GetModifiers(),
                    screen,
                    time);
            }
            else
            {
                retval = new EventMouseButtonDown(
                    mouse_button_event,
                    Singletons::Input().GetModifiers(),
                    screen,
                    time);
            }
            break;
        }

        case SDL_MOUSEBUTTONUP:
        {
            SDL_MouseButtonEvent const *mouse_button_event =
                reinterpret_cast<SDL_MouseButtonEvent const *>(e);
            if (mouse_button_event->button == Key::MOUSEWHEELUP ||
                mouse_button_event->button == Key::MOUSEWHEELDOWN)
            {
                // yes, virginia, there IS a reason this
                // function returns null sometimes.
                retval = NULL;
            }
            else
            {
                retval = new EventMouseButtonUp(
                    mouse_button_event,
                    Singletons::Input().GetModifiers(),
                    screen,
                    time);
            }
            break;
        }

        case SDL_MOUSEMOTION:
            retval = new EventMouseMotion(
                reinterpret_cast<SDL_MouseMotionEvent const *>(e),
                Singletons::Input().GetModifiers(),
                screen,
                time);
            break;

        case SDL_JOYAXISMOTION:
            retval = new EventJoyAxis(
                reinterpret_cast<SDL_JoyAxisEvent const *>(e),
                time);
            break;

        case SDL_JOYBALLMOTION:
            retval = new EventJoyBall(
                reinterpret_cast<SDL_JoyBallEvent const *>(e),
                time);
            break;

        case SDL_JOYBUTTONDOWN:
            retval = new EventJoyButtonDown(
                reinterpret_cast<SDL_JoyButtonEvent const *>(e),
                time);
            break;

        case SDL_JOYBUTTONUP:
            retval = new EventJoyButtonUp(
                reinterpret_cast<SDL_JoyButtonEvent const *>(e),
                time);
            break;

        case SDL_JOYHATMOTION:
            retval = new EventJoyHat(
                reinterpret_cast<SDL_JoyHatEvent const *>(e),
                time);
            break;

        case SDL_QUIT:
            retval = new EventQuit(
                reinterpret_cast<SDL_QuitEvent const *>(e),
                time);
            break;

        case SDL_SYSWMEVENT:
            retval = new EventSysWM(
                reinterpret_cast<SDL_SysWMEvent const *>(e),
                time);
            break;

        case SDL_VIDEORESIZE:
            retval = new EventResize(
                reinterpret_cast<SDL_ResizeEvent const *>(e),
                time);
            break;

        case SDL_VIDEOEXPOSE:
            retval = new EventExpose(
                reinterpret_cast<SDL_ExposeEvent const *>(e),
                time);
            break;

        case SDL_USEREVENT:
            ASSERT1(false &&
                    "Bad! BAD human! you shouldn't be making "
                    "SDL_USEREVENTs.  Create a subclass "
                    "of EventCustom instead.")
            break;

        default:
            ASSERT1(false && "Unknown event type")
            break;
    }

    return retval;
}

EventCustom::~EventCustom () { }

// ///////////////////////////////////////////////////////////////////////////
// event-matching functions for use in EventQueue
// ///////////////////////////////////////////////////////////////////////////

bool MatchEventType (Event const *event, Event::EventType const event_type)
{
    ASSERT1(event != NULL)

    return event->GetEventType() == event_type;
}

bool MatchCustomType (Event const *event, EventCustom::CustomType const custom_type)
{
    ASSERT1(event != NULL)

    return dynamic_cast<EventCustom const *>(event) != NULL &&
           static_cast<EventCustom const *>(event)->GetCustomType() == custom_type;
}

} // end of namespace Xrb
