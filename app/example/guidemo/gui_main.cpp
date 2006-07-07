// ///////////////////////////////////////////////////////////////////////////
// gui_main.cpp by Victor Dods, created 2006/06/04
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb.h"

#include "gui_masterwidget.h"
#include "xrb_eventqueue.h"
#include "xrb_input.h"
#include "xrb_keyrepeater.h"
#include "xrb_screen.h"
#include "xrb_utf8.h" // TEMP

using namespace Xrb;

#define FULLSCREEN 0

// exit handler
void Exit ()
{
    fprintf(stderr, "Exit();\n");
    // make sure input isn't grabbed
    SDL_WM_GrabInput(SDL_GRAB_OFF);
    // call SDL's cleanup func
    SDL_Quit();
}

int main (int argc, char **argv)
{
    fprintf(stderr, "\nmain();\n");

    // TEMP
    // TEMP
    // TEMP
    {
        std::string temp;
        for (Uint32 i = 0x0; i < 0x110000; (i == 0xD7FF ? i = 0xE000 : ++i))
        {
            temp.clear();
            UTF8::AppendSequence(&temp, i);
            Uint32 result = UTF8::GetUnicode(temp.c_str());
            if (i != result)
            {
                fprintf(stderr, "failed: i = 0x%X, result = 0x%X, seq = ", i, result);
                for (Uint8 const *s = reinterpret_cast<Uint8 const *>(temp.c_str()); *s != '\0'; ++s)
                    fprintf(stderr, "0x%02X ", static_cast<Uint32>(*s));
                fprintf(stderr, "\n");
            }
            ASSERT1(i == result)
        }
        for (Uint32 i = 0xD800; i < 0x120000; (i == 0xDFFF ? i = 0x110000 : ++i))
        {
            temp.clear();
            UTF8::AppendSequence(&temp, i);
            ASSERT1(temp.empty())
        }
    }
    // TEMP
    // TEMP
    // TEMP

    Singletons::Initialize();

    // initialize video (no parachute so we get core dumps)
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE) < 0)
    {
        fprintf(stderr, "unable to initialize video.  error: %s\n", SDL_GetError());
        exit(-1);
    }

    // register on-exit function. SDL_Quit takes care of deleting the screen
    atexit(Exit);
    // set a window title (i dunno what the icon string is)
    SDL_WM_SetCaption("XuqRijBuh GUI System Demo", "icon thingy");

    // init the screen
    Screen *screen = Screen::Create(
        1024,
        768,
        32,
        (FULLSCREEN ? SDL_FULLSCREEN : 0));
    if (screen == NULL)
    {
        fprintf(stderr, "unable to initialize video mode\n");
        exit(-2);
    }

    // add the master widget as the screen's main widget
    screen->SetMainWidget(new MasterWidget(screen));

    KeyRepeater key_repeater;

    // run loop
    {
        bool is_quit_requested = false;
        Float real_time = 0.0f;
        Float next_real_time = 0.0f;
//         Float minimum_framerate = 20.0f;
        Float maximum_framerate = 60.0f;
        while (!is_quit_requested)
        {
            // figure out how much time to sleep before processing the next frame
            Sint32 milliseconds_to_sleep = Max(0, static_cast<Sint32>(1000.0f * (next_real_time - real_time)));
            SDL_Delay(milliseconds_to_sleep);
            real_time = 0.001f * SDL_GetTicks();
            next_real_time += 1.0f / maximum_framerate;

            // process SDL events
            SDL_Event sdl_event;
            while (SDL_PollEvent(&sdl_event))
            {
                Event *event = Event::CreateEventFromSDLEvent(&sdl_event, screen, real_time);

                // if it was a dud, skip this loop
                if (event == NULL)
                    continue;

                // make sure to process key events through the key binds first
                if (event->GetIsKeyEvent() || event->GetIsMouseButtonEvent())
                    Singletons::Input().ProcessEvent(event);

                // also let the key repeater have a crack at it.
                key_repeater.ProcessEvent(event);

                // let the screen (and the entire UI/view system) have the event
                screen->ProcessEvent(event);
                Delete(event);
            }

            // key repeater frame computations
            key_repeater.ProcessFrame(real_time);
            // dequeue and process any key repeat events generated
            {
                // dequeue and process any key repeat events generated
                while (!key_repeater.GetIsEventQueueEmpty())
                {
                    // dequeue event
                    EventKeyRepeat *event = key_repeater.DequeueEvent();
                    ASSERT1(event != NULL)
                    // process event
                    screen->GetOwnerEventQueue()->EnqueueEvent(screen, event);
                }
            }

            // these will store the millisecond durations for various processes
            Uint32 gui_frame_time = 0;
            Uint32 render_frame_time = 0;

            // gui frame
            {
                Uint32 gui_frame_start_time = SDL_GetTicks();
                // process events from the gui event queue
                screen->GetOwnerEventQueue()->ProcessFrame(real_time);
                // frame computations for the UI/view system
                screen->ProcessFrame(real_time);
                // process events from the gui event queue again
                screen->GetOwnerEventQueue()->ProcessFrame(real_time);
                gui_frame_time = SDL_GetTicks() - gui_frame_start_time;
            }

            // rendering
            {
                Uint32 render_frame_start_time = SDL_GetTicks();
                screen->Draw();
                render_frame_time = SDL_GetTicks() - render_frame_start_time;
            }

            // check if we should quit
            is_quit_requested = screen->GetIsQuitRequested();
        }
    }

    Delete(screen);

    Singletons::Shutdown();

    // return with no error condition
    exit(0);
}

