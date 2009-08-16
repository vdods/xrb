// ///////////////////////////////////////////////////////////////////////////
// xrb_sdlpal.cpp by Victor Dods, created 2009/08/16
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_sdlpal.hpp"

#include "SDL.h"
#include "xrb_event.hpp"
#include "xrb_input_events.hpp"
#include "xrb_inputstate.hpp"

namespace Xrb
{

Pal *SDLPal::Create ()
{
    return new SDLPal();
}

Pal::Status SDLPal::Initialize ()
{
    // initialize video (no parachute so we get core dumps)
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE) < 0)
    {
        fprintf(stderr, "SDLPal::Initialize(); unable to initialize SDL.  error: %s\n", SDL_GetError());
        return FAILURE;
    }

    // set a window title (i dunno what the icon string is)
    SDL_WM_SetCaption("TODO: change me", "icon thingy");

    return SUCCESS;
}

void SDLPal::Shutdown ()
{
    // make sure input isn't grabbed
    SDL_WM_GrabInput(SDL_GRAB_OFF);
    // call SDL's cleanup func
    SDL_Quit();
}

Pal::Status SDLPal::InitializeVideo (Uint16 width, Uint16 height, Uint8 bit_depth, bool fullscreen)
{
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    Uint32 video_mode_flags = SDL_OPENGL;
    if (fullscreen)
        video_mode_flags |= SDL_FULLSCREEN;

    if (NULL == SDL_SetVideoMode(width, height, bit_depth, video_mode_flags))
    {
        fprintf(stderr, "SDLPal::InitializeVideo(); could not set the requested video mode\n");
        return FAILURE;
    }

    return SUCCESS;
}

void SDLPal::ShutdownVideo ()
{
    // nothing needs to be done here for SDL
}

Uint32 SDLPal::CurrentTime ()
{
    return SDL_GetTicks();
}

void SDLPal::Sleep (Uint32 milliseconds_to_sleep)
{
    SDL_Delay(milliseconds_to_sleep);
}

void SDLPal::FinishFrame ()
{
    SDL_GL_SwapBuffers();
}

Event *SDLPal::PollEvent (Screen const *screen, Float time)
{
    ASSERT1(screen != NULL);
    ASSERT1(time >= 0.0);

    // loop until we've constructed an Xrb::Event, or until there are
    // no SDL events in the event queue.  this weird loop is necessary
    // because some SDL events will be ignored, and returning NULL
    // from this method indicates that the event queue is empty, so we'll
    // have to keep polling until we get a non-ignored event.
    Event *retval = NULL;
    SDL_Event e;
    while (retval == NULL && SDL_PollEvent(&e) != 0)
    {
        switch (e.type)
        {
            case SDL_KEYDOWN:
                retval = new EventKeyDown(Key::Code(e.key.keysym.sym), Key::Modifier(e.key.keysym.mod), time);
                break;

            case SDL_KEYUP:
                retval = new EventKeyUp(Key::Code(e.key.keysym.sym), Key::Modifier(e.key.keysym.mod), time);
                break;

            case SDL_MOUSEBUTTONDOWN:
            {
                SDL_MouseButtonEvent const &mouse_button_event = e.button;
                if (mouse_button_event.button == SDL_BUTTON_WHEELUP ||
                    mouse_button_event.button == SDL_BUTTON_WHEELDOWN)
                {
                    retval = new EventMouseWheel(
                        Key::Code(mouse_button_event.button),
                        mouse_button_event.x,
                        mouse_button_event.y,
                        Singleton::InputState().Modifier(),
                        screen,
                        time);
                }
                else
                {
                    retval = new EventMouseButtonDown(
                        Key::Code(mouse_button_event.button),
                        mouse_button_event.x,
                        mouse_button_event.y,
                        Singleton::InputState().Modifier(),
                        screen,
                        time);
                }
                break;
            }

            case SDL_MOUSEBUTTONUP:
            {
                SDL_MouseButtonEvent const &mouse_button_event = e.button;
                if (mouse_button_event.button == SDL_BUTTON_WHEELUP ||
                    mouse_button_event.button == SDL_BUTTON_WHEELDOWN)
                {
                    // mousewheel button-up events are superfluous
                    retval = NULL;
                }
                else
                {
                    retval = new EventMouseButtonUp(
                        Key::Code(mouse_button_event.button),
                        mouse_button_event.x,
                        mouse_button_event.y,
                        Singleton::InputState().Modifier(),
                        screen,
                        time);
                }
                break;
            }

            case SDL_MOUSEMOTION:
            {
                SDL_MouseMotionEvent const &mouse_motion_event = e.motion;
                retval = new EventMouseMotion(
                    Singleton::InputState().IsKeyPressed(Key::LEFTMOUSE),
                    Singleton::InputState().IsKeyPressed(Key::MIDDLEMOUSE),
                    Singleton::InputState().IsKeyPressed(Key::RIGHTMOUSE),
                    mouse_motion_event.x,
                    mouse_motion_event.y,
                    mouse_motion_event.xrel,
                    -mouse_motion_event.yrel, // yrel coord is negative to get right-handed coords
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
    }

    // this should only return NULL if the event queue is empty and we didn't
    // encounter any relevant events.
    return retval;
}

Texture *SDLPal::LoadImage (char const *image_path)
{
    return NULL; // TODO
}

Pal::Status SDLPal::SaveImage (char const *image_path, Texture const &texture)
{
    return FAILURE; // TODO
}

} // end of namespace Xrb
