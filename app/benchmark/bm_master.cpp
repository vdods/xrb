// ///////////////////////////////////////////////////////////////////////////
// bm_master.cpp by Victor Dods, created 2009/08/24
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "bm_master.hpp"

#include <stdlib.h> // for srand()

#include "bm_config.hpp"
#include "xrb_button.hpp"
#include "xrb_engine2_world.hpp"
#include "xrb_engine2_worldviewwidget.hpp"
#include "xrb_eventqueue.hpp"
#include "xrb_inputstate.hpp"
#include "xrb_label.hpp"
#include "xrb_layout.hpp"
#include "xrb_lineedit.hpp"
#include "xrb_pal.hpp"
#include "xrb_resourcelibrary.hpp"
#include "xrb_screen.hpp"
#include "xrb_transformation.hpp"

using namespace Xrb;

extern Bm::Config g_config;

namespace Bm
{

Master::Master (Screen *screen)
    :
//     SignalHandler(),
//     EventHandler(NULL),
    m_screen(screen)
{
//     SetOwnerEventQueue(&m_master_event_queue);

    ASSERT1(m_screen != NULL);
    ASSERT1(m_screen->OwnerEventQueue() != NULL);
    m_minimum_framerate = 20.0f;
    m_maximum_framerate = 60.0f;
    m_real_time = 0.0f;

//     m_game_widget = NULL;
    m_world = NULL;
}

Master::~Master ()
{
//     ASSERT1(m_game_widget == NULL);
    ASSERT1(m_world == NULL);

//     SetOwnerEventQueue(NULL);
}

void Master::Run ()
{
    // seed the random number generator
    srand(static_cast<Uint32>(time(NULL)));

    /*
    benchmark design

    2 modes - paused and animated.
        paused will show a specific configuration of the world
        animated will be the same objects as in paused, but moving around.
            layered widget - background is world view widget, foreground is transparent label
            world view widget

            foreground transparent label
                a couple of different font sizes and colors

        things that need to be tested:
            color mask
            color bias
            transparency
            font sizes
            widget layouts
            non-entity objects
                foreground, background
                transparent, non
            entity objects
                scaling, rotating, translating (motion)
                transparent, non
            worldview
                zooming, rotating, translating, fading to black, fading to white

        have a control panel to pop up with buttons to initiate different tests (or maybe make it all streamlined/time-compactified so the user doesn't have to do anything)

    2 panes - reference and rendered
        reference is a screenshot of what the paused state should look like
        rendered is the real-time rendered scene as described above.

    ability to take screenshot of paused rendered side (or maybe both sides?)

    main widget
    +------------------------------------------------------------------------+
    | FRAMERATE and other info                                               |
    | +--------------------------------+  +--------------------------------+ |
    | |                                |  |                                | |
    | |                                |  |                                | |
    | |                                |  |                                | |
    | |                                |  |                                | |
    | |                                |  |                                | |
    | |                                |  |                                | |
    | |                                |  |                                | |
    | |                                |  |                                | |
    | |         reference frame        |  |         render frame           | |
    | |                                |  |                                | |
    | |                                |  |                                | |
    | |                                |  |                                | |
    | |                                |  |                                | |
    | |                                |  |                                | |
    | |                                |  |                                | |
    | |                                |  |                                | |
    | +--------------------------------+  +--------------------------------+ |
    | CONTROL PANEL              TAKE SCREENSHOT          STILL/ANIMATE MODE |
    +------------------------------------------------------------------------+


    control panel widget (might not actually fill up the whole screen)
    +------------------------------------------------------------------------+
    |                                                                        |
    |                                                                        |
    |                                                                        |
    |                                                                        |
    |                                                                        |
    |                                                                        |
    |                                                                        |
    |                                                                        |
    |                                                                        |
    |                                                                        |
    |                                                                        |
    |                                                                        |
    |                                                                        |
    |                                                                        |
    |                                                                        |
    |                                                                        |
    |                                                                        |
    |                                                                        |
    |                                                                        |
    +------------------------------------------------------------------------+

    TODO features to put on the control panel:
        framerate limiting/unlimiting
        video resolution and fullscreen
        a description of what's happening in the animated rendered screen (i.e. everything that's being tested)
        maybe a set of checkboxes for people to record what features work/don't work

    */








        Layout *main_layout = new Layout(VERTICAL, m_screen, "main layout");

        Layout *frame_layout = new Layout(HORIZONTAL, main_layout, "frame layout");

        Label *reference_frame_label = new Label(Singleton::ResourceLibrary().LoadPath<GlTexture>(GlTexture::Create, "resources/solitary_small.png"), frame_layout, "reference frame label");
        reference_frame_label->SetPictureKeepsAspectRatio(true);
        /*Engine2::WorldViewWidget *world_view_widget = */new Engine2::WorldViewWidget(frame_layout);

        Layout *button_layout = new Layout(HORIZONTAL, main_layout, "button layout");

        Button *control_panel_button = new Button("Control Panel", button_layout, "control panel button");
        Button *take_screenshot_button = new Button("Take Screenshot", button_layout, "take screenshot button");
        Button *mode_toggle_button = new Button("Still-frame Mode", button_layout, "mode toggle button");

        control_panel_button->SetIsHeightFixedToTextHeight(true);
        take_screenshot_button->SetIsHeightFixedToTextHeight(true);
        mode_toggle_button->SetIsHeightFixedToTextHeight(true);

/*
        SignalHandler::Connect0(
            disable_label_via_press_button->SenderPressed(),
            mabel_the_disabled_label->ReceiverDisable());
        SignalHandler::Connect0(
            enable_label_via_press_button->SenderPressed(),
            mabel_the_disabled_label->ReceiverEnable());
        SignalHandler::Connect0(
            disable_label_via_release_button->SenderReleased(),
            mabel_the_disabled_label->ReceiverDisable());
        SignalHandler::Connect0(
            enable_label_via_release_button->SenderReleased(),
            mabel_the_disabled_label->ReceiverEnable());

        SignalHandler::Connect0(
            quit_button->SenderReleased(),
            m_screen->ReceiverRequestQuit());

        SignalHandler::Connect1(
            enter_text_line_edit->SenderTextUpdated(),
            verbatim_label->ReceiverSetText());
        SignalHandler::Connect1(
            enter_text_line_edit->SenderTextUpdatedV(),
            &Transformation::Lowercase,
            lowercase_label->ReceiverSetTextV());
        SignalHandler::Connect1(
            enter_text_line_edit->SenderTextUpdatedV(),
            &Transformation::Uppercase,
            uppercase_label->ReceiverSetTextV());
*/



        m_screen->SetMainWidget(main_layout);






















//     // create the game world
//     m_world = World::Create(g_config.GetDifficultyLevel());
//     // create the game widget
//     m_game_widget = new GameWidget(m_world, m_screen);
//     // set it as the main widget
//     m_screen->SetMainWidget(m_game_widget);
    // reset the game time
    m_game_time = 0.0f;









    m_real_time = 0.0f;
    Float previous_real_time = 0.0f;
    Float next_real_time = 0.0f;
    Float game_time_delta;
    while (!m_screen->IsQuitRequested())
    {
        // figure out how much time to sleep before processing the next frame
        m_real_time = 0.001f * Singleton::Pal().CurrentTime();
        Sint32 milliseconds_to_sleep = Max(0, static_cast<Sint32>(1000.0f * (next_real_time - m_real_time)));
        Singleton::Pal().Sleep(milliseconds_to_sleep);
        next_real_time += 1.0f / m_maximum_framerate;

//         // process the Master event queue
//         m_master_event_queue.ProcessFrame(m_real_time);

        // only run the world if it's active.
        if (m_world != NULL)
        {
            game_time_delta = m_real_time - previous_real_time;
            if (game_time_delta > 1.0f / m_minimum_framerate)
                game_time_delta = 1.0f / m_minimum_framerate;
            ASSERT1(game_time_delta >= 0.0f);

            m_game_time += game_time_delta * 1.0f;//m_world->Timescale();
        }

        previous_real_time = m_real_time;

        // framerate calculation
        m_framerate_calculator.AddFrameTime(m_real_time);
//         if (m_game_widget != NULL)
//             m_game_widget->SetFramerate(m_framerate_calculator.Framerate());

        // process events
        {
            Event *event = NULL;
            while ((event = Singleton::Pal().PollEvent(m_screen, m_real_time)) != NULL)
            {
                // process key events through the InputState singleton first
                if (event->IsKeyEvent() || event->IsMouseButtonEvent())
                    Singleton::InputState().ProcessEvent(event);

                // also let the key repeater have a crack at it.
                m_key_repeater.ProcessEvent(event);

                // let the screen (and the entire UI/view system) have the event
                m_screen->ProcessEvent(event);
                Delete(event);
            }
        }

        // key repeater frame computations
        m_key_repeater.ProcessFrame(m_real_time);
        // dequeue and process any key repeat events generated
        ProcessKeyRepeatEvents();

        // these will store the millisecond durations for various processes
        Uint32 world_frame_time = 0;
        Uint32 gui_frame_time = 0;
        Uint32 render_frame_time = 0;

        // world frame
        if (m_world != NULL)
        {
            Uint32 world_frame_start_time = Singleton::Pal().CurrentTime();
            //m_world->ProcessFrame(m_game_time);
            world_frame_time = Singleton::Pal().CurrentTime() - world_frame_start_time;
        }

        // gui frame
        {
            Uint32 gui_frame_start_time = Singleton::Pal().CurrentTime();
            // process events from the gui event queue
            m_screen->OwnerEventQueue()->ProcessFrame(m_real_time);
            // frame computations for the UI/view system
            m_screen->ProcessFrame(m_real_time);
            // process events from the gui event queue again
            m_screen->OwnerEventQueue()->ProcessFrame(m_real_time);
            gui_frame_time = Singleton::Pal().CurrentTime() - gui_frame_start_time;
        }

        // rendering
        {
            Uint32 render_frame_start_time = Singleton::Pal().CurrentTime();
            m_screen->Draw(m_real_time);
            render_frame_time = Singleton::Pal().CurrentTime() - render_frame_start_time;
        }

        // set the (previous) game loop's process durations
//         if (m_game_widget != NULL)
//         {
//             m_game_widget->SetWorldFrameTime(world_frame_time);
//             m_game_widget->SetGUIFrameTime(gui_frame_time);
//             m_game_widget->SetRenderFrameTime(render_frame_time);
//             m_game_widget->SetEntityCount(m_world->EntityCount());
//         }
    }

    // delete the game widget and world, in that order, and nullify them.
//     DeleteAndNullify(m_game_widget);
//     DeleteAndNullify(m_world);

//     ASSERT1(m_game_widget == NULL);
    ASSERT1(m_world == NULL);
}

bool Master::HandleEvent (Event const *const e)
{
/*
    ASSERT1(e != NULL);
    ASSERT1(e->GetEventType() == Event::CUSTOM);

    EventBase const *event = DStaticCast<EventBase const *>(e);
    switch (event->GetCustomType())
    {
        case EventBase::ACTIVATE_TITLE_SCREEN:   ActivateTitleScreen();      return true;
        case EventBase::DEACTIVATE_TITLE_SCREEN: DeactivateTitleScreen();    return true;
        case EventBase::ACTIVATE_GAME:           ActivateGame();             return true;
        case EventBase::DEACTIVATE_GAME:         DeactivateGame();           return true;
        case EventBase::QUIT_REQUESTED:          m_is_quit_requested = true; return true;

        default: ASSERT1(false && "This event shouldn't be used here");
    }
*/
    return false;
}

void Master::ProcessKeyRepeatEvents ()
{
    ASSERT1(m_screen != NULL);

    // dequeue and process any key repeat events generated
    while (!m_key_repeater.IsEventQueueEmpty())
    {
        // dequeue event
        EventKeyRepeat *event = m_key_repeater.DequeueEvent();
        ASSERT1(event != NULL);
        // process event
        m_screen->OwnerEventQueue()->EnqueueEvent(m_screen, event);
    }
}

void Master::CreateWorld ()
{
    ASSERT1(m_world == NULL);
    m_world = Engine2::World::CreateEmpty(NULL); // no PhysicsHandler

    // create the contents of the world
}

void Master::DestroyWorld ()
{
    ASSERT1(m_world != NULL);
    // TODO: anything else need to be done?
    DeleteAndNullify(m_world);
}

} // end of namespace Bm

