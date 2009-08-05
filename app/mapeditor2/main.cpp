// ///////////////////////////////////////////////////////////////////////////
// main.cpp by Victor Dods, created 2004/05/31
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb.hpp"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "xrb_eventqueue.hpp"
#include "xrb_input.hpp"
#include "xrb_keyrepeater.hpp"
#include "xrb_mapeditor2_mainwidget.hpp"
#include "xrb_screen.hpp"

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

void ProcessKeyRepeatEvents (
    KeyRepeater *key_repeater,
    Screen *screen,
    EventQueue *event_queue);

int main (int argc, char **argv)
{
    fprintf(stderr, "\nmain();\n");

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
    SDL_WM_SetCaption("XuqRijBuh 2D Map Editor", "icon thingy");

    // the game loop's event queue
    EventQueue event_queue;
    // the KeyRepeater which generates EventKeyRepeat events
    KeyRepeater key_repeater;

    // init the screen
    Screen *screen = Screen::Create(
        &event_queue,
        1400,
        1100,
        32,
        (FULLSCREEN ? SDL_FULLSCREEN : 0));
    if (screen == NULL)
    {
        fprintf(stderr, "unable to initialize video mode\n");
        exit(-2);
    }

    MapEditor2::MainWidget *map_editor = new MapEditor2::MainWidget(screen);
    screen->SetMainWidget(map_editor);

    FramerateCalculator framerate_calculator;

    // seed the random number generator
    srand(time(0));

    // fps limiter
    Float time = 0.0;
    Float max_fps = 30000.0;
    Float next_time = 0.0;
    Event *event;
    while (1)
    {
        do
        {
            time = 0.001 * SDL_GetTicks();
        }
        while (time < next_time);
        next_time = time + 1.0 / max_fps;

        framerate_calculator.AddFrameTime(time);

//         time += 1.0/20.0; // hack to make engine deterministic
//         fprintf(stderr, "main(); time = %e\n", time);

//         // world and screen-hierarchy frames
//         world->ProcessFrame(time);

        // key repeater frame computations
        key_repeater.ProcessFrame(time);
        // dequeue and process any key repeat events generated
        ProcessKeyRepeatEvents(&key_repeater, screen, &event_queue);

        // process events from the event queue (using the newly-incremented
        // frame time)
        event_queue.ProcessFrame(time);

        // frame computations for the UI/view system
        screen->ProcessFrame(time);

        // quit condition
        if (screen->IsQuitRequested())
            break;

        // actually draw the shit
        screen->Draw();

        // process events
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            event = Event::CreateEventFromSDLEvent(&e, screen, time);

            // if it was a dud, skip this loop
            if (event == NULL)
                continue;

            // make sure to process key events through the key binds first
            if (event->IsKeyEvent() || event->IsMouseButtonEvent())
                Singletons::Input().ProcessEvent(event);

            // also let the key repeater have a crack at it.
            key_repeater.ProcessEvent(event);

            // let the screen (and the entire UI/view system have the event
            screen->ProcessEvent(event);
            Delete(event);

            // dequeue and process any key repeat events generated
            ProcessKeyRepeatEvents(&key_repeater, screen, &event_queue);
        }

        // process events from the event queue (before the frame time increments)
        event_queue.ProcessFrame(time);
    }

    // gracefully delete the screen (this will delete the map editor main widget)
    Delete(screen);

    Singletons::Shutdown();

    // return with no error condition
    exit(0);
}

void ProcessKeyRepeatEvents (
    KeyRepeater *const key_repeater,
    Screen *const screen,
    EventQueue *const event_queue)
{
    ASSERT1(key_repeater != NULL);
    ASSERT1(screen != NULL);
    ASSERT1(event_queue != NULL);

    EventKeyRepeat *e;

    // dequeue and process any key repeat events generated
    while (!key_repeater->IsEventQueueEmpty())
    {
        // dequeue event
        e = key_repeater->DequeueEvent();
        ASSERT1(e != NULL);
        // process event
        event_queue->EnqueueEvent(screen, e);
    }
}
