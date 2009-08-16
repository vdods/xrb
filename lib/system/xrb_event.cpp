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
        "DELETE_CHILD_WIDGET",
        "QUIT",
        "STATE_MACHINE_INPUT",
        "ENGINE2_DELETE_ENTITY",
        "ENGINE2_REMOVE_ENTITY_FROM_WORLD"
        "CUSTOM"
    };
    DEBUG1_CODE(static Uint32 const s_event_type_name_count = sizeof(s_event_type_name) / sizeof(std::string));
    ASSERT1(static_cast<Uint32>(event_type) < s_event_type_name_count);
    return s_event_type_name[static_cast<Uint32>(event_type)];
}

Event *Event::CreateEventFromSDLEvent (
    SDL_Event const *const e,
    Screen const *const screen,
    Float const time)
{
    ASSERT1(e != NULL);
    ASSERT1(screen != NULL);
    ASSERT1(time >= 0.0);

    Event *retval = NULL;

    switch (e->type)
    {
        case SDL_KEYDOWN:
            retval = new EventKeyDown(Key::Code(e->key.keysym.sym), Key::Modifier(e->key.keysym.mod), time);
            break;

        case SDL_KEYUP:
            retval = new EventKeyUp(Key::Code(e->key.keysym.sym), Key::Modifier(e->key.keysym.mod), time);
            break;

        case SDL_MOUSEBUTTONDOWN:
        {
            SDL_MouseButtonEvent const *mouse_button_event =
                reinterpret_cast<SDL_MouseButtonEvent const *>(e);
            if (mouse_button_event->button == SDL_BUTTON_WHEELUP ||
                mouse_button_event->button == SDL_BUTTON_WHEELDOWN)
            {
                retval = new EventMouseWheel(
                    Key::Code(mouse_button_event->button),
                    mouse_button_event->x,
                    mouse_button_event->y,
                    Singleton::InputState().Modifier(),
                    screen,
                    time);
            }
            else
            {
                retval = new EventMouseButtonDown(
                    Key::Code(mouse_button_event->button),
                    mouse_button_event->x,
                    mouse_button_event->y,
                    Singleton::InputState().Modifier(),
                    screen,
                    time);
            }
            break;
        }

        case SDL_MOUSEBUTTONUP:
        {
            SDL_MouseButtonEvent const *mouse_button_event =
                reinterpret_cast<SDL_MouseButtonEvent const *>(e);
            if (mouse_button_event->button == SDL_BUTTON_WHEELUP ||
                mouse_button_event->button == SDL_BUTTON_WHEELDOWN)
            {
                // yes, virginia, there IS a reason this
                // function returns null sometimes.
                retval = NULL;
            }
            else
            {
                retval = new EventMouseButtonUp(
                    Key::Code(mouse_button_event->button),
                    mouse_button_event->x,
                    mouse_button_event->y,
                    Singleton::InputState().Modifier(),
                    screen,
                    time);
            }
            break;
        }

        case SDL_MOUSEMOTION:
        {
            SDL_MouseMotionEvent const *mouse_motion_event =
                reinterpret_cast<SDL_MouseMotionEvent const *>(e);
            retval = new EventMouseMotion(
                Singleton::InputState().IsKeyPressed(Key::LEFTMOUSE),
                Singleton::InputState().IsKeyPressed(Key::MIDDLEMOUSE),
                Singleton::InputState().IsKeyPressed(Key::RIGHTMOUSE),
                mouse_motion_event->x,
                mouse_motion_event->y,
                mouse_motion_event->xrel,
                -mouse_motion_event->yrel, // yrel coord is negative to get right-handed coords
                Singleton::InputState().Modifier(),
                screen,
                time);
            break;
        }

        case SDL_JOYAXISMOTION:
        case SDL_JOYBALLMOTION:
        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
        case SDL_JOYHATMOTION:
            // forget it for now
            break;

        case SDL_QUIT:
            retval = new EventQuit(time);
            break;

        case SDL_ACTIVEEVENT:
        case SDL_SYSWMEVENT:
        case SDL_VIDEORESIZE:
        case SDL_VIDEOEXPOSE:
            // ignore these events
            break;

        case SDL_USEREVENT:
            ASSERT1(false &&
                    "Bad! BAD human! you shouldn't be making "
                    "SDL_USEREVENTs.  Create a subclass "
                    "of EventCustom instead.");
            break;

        default:
            ASSERT1(false && "Unknown event type");
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
