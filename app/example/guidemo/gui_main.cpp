// ///////////////////////////////////////////////////////////////////////////
// gui_main.cpp by Victor Dods, created 2006/06/04
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb.hpp"

#include "gui_masterwidget.hpp"
#include "xrb_eventqueue.hpp"
#include "xrb_inputstate.hpp"
#include "xrb_keyrepeater.hpp"
#include "xrb_screen.hpp"

// TEMP
#include "xrb_arithmeticparser.h"
#include "xrb_math.hpp"
// TEMP

using namespace Xrb;

#define FULLSCREEN 0

// // exit handler
// void Exit ()
// {
//     Singleton::Pal().Shutdown();
//     Singleton::Shutdown();
// }

struct ExpressionValuePair
{
    std::string m_expression;
    Float m_value;
};

int main (int argc, char **argv)
{
    fprintf(stderr, "\nmain();\n");

    ExpressionValuePair const pair[] =
    {
        { "0", 0.0f },
        { "1", 1.0f },
        { "+3", +3.0f },
        { "-2", -2.0f },
        { "1+2", 1.0f + 2.0f },
        { "1-2", 1.0f - 2.0f },
        { "3*4", 3.0f * 4.0f },
        { "3/4", 3.0f / 4.0f },
        { "3^4", Math::Pow(3.0f, 4.0f) },
        { "0^4", Math::Pow(0.0f, 4.0f) },
        { "3^0", Math::Pow(3.0f, 0.0f) },
        { "0^0", Math::Pow(0.0f, 0.0f) },
        { "2+3*4-5/-7^2", 2.0f + 3.0f * 4.0f - 5.0f / -Math::Pow(7.0f, 2.0f) },
        { "2.34", 2.34f },
        { "2.34e2", 2.34e2f },
        { "2.", 2.f },
        { "2.e2", 2.e2f },
        { "2e2", 2e2f },
        { "2e-2", 2e-2f },
        { ".34", .34f },
        { "0.5", 0.5f },
        { "2^0.5", Math::Pow(2.0f, 0.5f) },
        { "-2^0.5", -Math::Pow(2.0f, 0.5f) },
        { "(8)", (8.0f) },
        { "1-2-3", 1.0f - 2.0f - 3.0f },
        { "(1-2-3)", (1.0f - 2.0f - 3.0f) },
        { "(1-2)-3", (1.0f - 2.0f) - 3.0f },
        { "1-(2-3)", 1.0f - (2.0f - 3.0f) },
        { "1/0", Math::Nan() },
        { "2+(1/0)", Math::Nan() },
        { "2-(1/0)", Math::Nan() },
        { "2*(1/0)", Math::Nan() },
        { "2/(1/0)", Math::Nan() },
        { "2^(1/0)", Math::Nan() },
        { "(1/0)+(1/0)", Math::Nan() },
        { "(-2)^0.5", Math::Pow(-2.0f, 0.5f) },
    };
    Uint32 const pair_count = LENGTHOF(pair);

    ArithmeticParser parser;

    for (Uint32 i = 0; i < pair_count; ++i)
    {
        Float expression_value = parser.Parse(pair[i].m_expression);
        fprintf(
            stderr,
            "%s - expression = \"%s\", actual = %g, calculated = %g\n",
            pair[i].m_value == expression_value || (!Math::IsFinite(pair[i].m_value) && !Math::IsFinite(expression_value)) ?
                "  PASS  " :
                "! FAIL !",
            pair[i].m_expression.c_str(),
            pair[i].m_value,
            expression_value);
    }

    return 0;
/*
    Singleton::Initialize();

    // register on-exit function, which will be called after main() has returned.
    atexit(Exit);

    Singleton::Pal().Initialize("XuqRijBuh GUI System Demo");

    // init the screen
    Screen *screen = Screen::Create(
        1024,
        768,
        32,
        (FULLSCREEN ? true : false));
    if (screen == NULL)
    {
        fprintf(stderr, "failed during Screen creation\n");
        return -2;
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
            Singleton::Pal().Sleep(milliseconds_to_sleep);
            real_time = 0.001f * Singleton::Pal().CurrentTime();
            next_real_time += 1.0f / maximum_framerate;

            // process events
            {
                Event *event = NULL;
                while ((event = Singleton::Pal().PollEvent(screen, real_time)) != NULL)
                {
                    // process key events through the InputState singleton first
                    if (event->IsKeyEvent() || event->IsMouseButtonEvent())
                        Singleton::InputState().ProcessEvent(event);

                    // also let the key repeater have a crack at it.
                    key_repeater.ProcessEvent(event);

                    // let the screen (and the entire UI/view system) have the event
                    screen->ProcessEvent(event);
                    Delete(event);
                }
            }

            // key repeater frame computations
            key_repeater.ProcessFrame(real_time);
            // dequeue and process any key repeat events generated
            {
                // dequeue and process any key repeat events generated
                while (!key_repeater.IsEventQueueEmpty())
                {
                    // dequeue event
                    EventKeyRepeat *event = key_repeater.DequeueEvent();
                    ASSERT1(event != NULL);
                    // process event
                    screen->OwnerEventQueue()->EnqueueEvent(screen, event);
                }
            }

            // these will store the millisecond durations for various processes
            Uint32 gui_frame_time = 0;
            Uint32 render_frame_time = 0;

            // gui frame
            {
                Uint32 gui_frame_start_time = Singleton::Pal().CurrentTime();
                // process events from the gui event queue
                screen->OwnerEventQueue()->ProcessFrame(real_time);
                // frame computations for the UI/view system
                screen->ProcessFrame(real_time);
                // process events from the gui event queue again
                screen->OwnerEventQueue()->ProcessFrame(real_time);
                gui_frame_time = Singleton::Pal().CurrentTime() - gui_frame_start_time;
            }

            // rendering
            {
                Uint32 render_frame_start_time = Singleton::Pal().CurrentTime();
                screen->Draw();
                render_frame_time = Singleton::Pal().CurrentTime() - render_frame_start_time;
            }

            // check if we should quit
            is_quit_requested = screen->IsQuitRequested();
        }
    }

    Delete(screen);

    // return with no error condition
    return 0;
*/
}

