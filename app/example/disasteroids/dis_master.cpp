// ///////////////////////////////////////////////////////////////////////////
// dis_master.cpp by Victor Dods, created 2006/03/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_master.hpp"

#include <stdlib.h> // for srand()

#include "dis_config.hpp"
#include "dis_events.hpp"
#include "dis_gamewidget.hpp"
#include "dis_highscorenameentrydialog.hpp"
#include "dis_resourcecache.hpp"
#include "dis_titlescreenwidget.hpp"
#include "dis_world.hpp"
#include "xrb_inputstate.hpp"
#include "xrb_pal.hpp"
#include "xrb_screen.hpp"
#include "xrb_widgetbackground.hpp"
#include "xrb_widgetcontext.hpp"
#include "xrb_stylesheet.hpp"

#define HIGH_SCORES_FILENAME "disasteroids.scores"

using namespace Xrb;

extern Dis::Config g_config;

namespace Dis {

Master::Master (Screen *const screen)
    :
    SignalHandler(),
    EventHandler(NULL),
    m_internal_receiver_accept_score(&Master::AcceptScore, this),
    m_internal_receiver_accept_name(&Master::AcceptName, this),
    m_internal_receiver_start_game(&Master::StartGame, this),
    m_internal_receiver_quit_game(&Master::QuitGame, this),
    m_internal_receiver_end_game(&Master::EndGame, this)
{
    SetOwnerEventQueue(&m_master_event_queue);

    // stow away the screen
    ASSERT1(screen != NULL);
    m_screen = screen;
    // hook up the screen's quit requested signal
    SignalHandler::Connect0(
        m_screen->SenderQuitRequested(),
        &m_internal_receiver_quit_game);
    // initialize m_is_quit_requested from m_screen's
    m_is_quit_requested = m_screen->IsQuitRequested();
    ASSERT1(m_screen->OwnerEventQueue() != NULL);
    m_minimum_framerate = 20.0f;
    m_maximum_framerate = 60.0f;
    m_real_time = Time::ms_beginning_of;
    m_game_time = Time::ms_beginning_of;

    m_title_screen_widget = NULL;
    m_show_high_scores_immediately = false;
    m_show_best_points_high_scores_first = true;

    m_game_widget = NULL;
    m_game_world = NULL;

    m_high_scores.Read(HIGH_SCORES_FILENAME);

    // set up the custom disasteroids StyleSheet.  populate it with the defaults, then override some.
    StyleSheet *style_sheet = new StyleSheet();
    style_sheet->PopulateUsingDefaults();
    
    style_sheet->SetBackground(
        StyleSheet::BackgroundType::BUTTON_IDLE,
        new WidgetBackgroundTextured3Way(
            GlTexture::Load("fs://button_left_black.png"/*, GlTexture::USES_SEPARATE_ATLAS*/), // this one is square
            GlTexture::Load("fs://button_center.png", GlTexture::USES_SEPARATE_ATLAS),
            GlTexture::Load("fs://button_right.png", GlTexture::USES_SEPARATE_ATLAS)));
    style_sheet->SetBackground(
        StyleSheet::BackgroundType::BUTTON_MOUSEOVER,
        new WidgetBackgroundTextured3Way(
            GlTexture::Load("fs://button_left_blue.png"/*, GlTexture::USES_SEPARATE_ATLAS*/), // this one is square
            GlTexture::Load("fs://button_center.png", GlTexture::USES_SEPARATE_ATLAS),
            GlTexture::Load("fs://button_right.png", GlTexture::USES_SEPARATE_ATLAS)));
    style_sheet->SetBackground(
        StyleSheet::BackgroundType::BUTTON_PRESSED,
        new WidgetBackgroundTextured3Way(
            GlTexture::Load("fs://button_left_green.png"/*, GlTexture::USES_SEPARATE_ATLAS*/), // this one is square
            GlTexture::Load("fs://button_center.png", GlTexture::USES_SEPARATE_ATLAS),
            GlTexture::Load("fs://button_right.png", GlTexture::USES_SEPARATE_ATLAS)));

    style_sheet->SetBackground(
        StyleSheet::BackgroundType::CHECK_BOX_UNCHECKED,
        new WidgetBackgroundTextured(
            GlTexture::Load("fs://radiobutton_black.png"/*, GlTexture::USES_SEPARATE_ATLAS*/))); // this one is square
    style_sheet->SetBackground(
        StyleSheet::BackgroundType::CHECK_BOX_MOUSEOVER,
        new WidgetBackgroundTextured(
            GlTexture::Load("fs://radiobutton_blue.png"/*, GlTexture::USES_SEPARATE_ATLAS*/))); // this one is square
    style_sheet->SetBackground(
        StyleSheet::BackgroundType::CHECK_BOX_PRESSED,
        new WidgetBackgroundTextured(
            GlTexture::Load("fs://radiobutton_yellow.png"/*, GlTexture::USES_SEPARATE_ATLAS*/))); // this one is square
    style_sheet->SetBackground(
        StyleSheet::BackgroundType::CHECK_BOX_CHECKED,
        new WidgetBackgroundTextured(
            GlTexture::Load("fs://radiobutton_green.png"/*, GlTexture::USES_SEPARATE_ATLAS*/))); // this one is square

    style_sheet->SetBackground(
        StyleSheet::BackgroundType::RADIO_BUTTON_UNCHECKED,
        new WidgetBackgroundTextured(
            GlTexture::Load("fs://radiobutton_black.png"/*, GlTexture::USES_SEPARATE_ATLAS*/))); // this one is square
    style_sheet->SetBackground(
        StyleSheet::BackgroundType::RADIO_BUTTON_MOUSEOVER,
        new WidgetBackgroundTextured(
            GlTexture::Load("fs://radiobutton_blue.png"/*, GlTexture::USES_SEPARATE_ATLAS*/))); // this one is square
    style_sheet->SetBackground(
        StyleSheet::BackgroundType::RADIO_BUTTON_PRESSED,
        new WidgetBackgroundTextured(
            GlTexture::Load("fs://radiobutton_yellow.png"/*, GlTexture::USES_SEPARATE_ATLAS*/))); // this one is square
    style_sheet->SetBackground(
        StyleSheet::BackgroundType::RADIO_BUTTON_CHECKED,
        new WidgetBackgroundTextured(
            GlTexture::Load("fs://radiobutton_green.png"/*, GlTexture::USES_SEPARATE_ATLAS*/))); // this one is square

    // custom StyleSheet property types:
    style_sheet->SetBackground("black_half_opaque", new WidgetBackgroundColored(Color(0.0f, 0.0f, 0.0f, 0.5f)));
    style_sheet->SetBackground("upgrade_level",     new WidgetBackgroundTextured(GlTexture::Load("fs://radiobutton_black.png")));
    style_sheet->SetBackground("game_demo",         new WidgetBackgroundColored(Color(1.0f, 0.0f, 0.0f, 1.0f))); // HIPPO TEMP
    style_sheet->SetFont("inventory_panel_small", StyleSheet::ms_fallback_font_path, 0.018f); // HIPPO use particular font path
    style_sheet->SetFont("hud_element",           StyleSheet::ms_fallback_font_path, 0.05f);  // HIPPO use particular font path
    style_sheet->SetFont("giant",                 StyleSheet::ms_fallback_font_path, 0.1f);   // HIPPO use particular font path
    
    // m_screen takes ownership of the pointer.
    m_screen->Context().SetStyleSheet(style_sheet);
}

Master::~Master ()
{
    ASSERT1(m_game_widget == NULL);
    ASSERT1(m_game_world == NULL);
    ASSERT1(m_title_screen_widget == NULL);

    m_screen = NULL;

    SetOwnerEventQueue(NULL);
}

void Master::Run ()
{
    // cache frequently-used resources for the entire execution of Run()
    ResourceCache resource_cache;

    // seed the random number generator
    srand(static_cast<Uint32>(time(NULL)));

    // start at the title screen
    ActivateTitleScreen();

    // TODO: initialization so that Run() could be called several times in a row

    m_real_time = Time::ms_beginning_of;
    Time previous_real_time = Time::ms_beginning_of;
    Time next_real_time = Time::ms_beginning_of;
    Time::Delta game_time_delta;
    while (!m_is_quit_requested)
    {
        // figure out how much time to sleep before processing the next frame
        m_real_time = Singleton::Pal().CurrentTime();
        Time::Delta seconds_to_sleep = Max(0.0f, next_real_time - m_real_time);
        Singleton::Pal().Sleep(seconds_to_sleep);
        next_real_time += 1.0f / m_maximum_framerate;

        // process the Master event queue
        m_master_event_queue.ProcessFrame(m_real_time);

        // only run the world if it's active.
        if (m_game_world != NULL)
        {
            game_time_delta = m_real_time - previous_real_time;
            if (game_time_delta > 1.0f / m_minimum_framerate)
                game_time_delta = 1.0f / m_minimum_framerate;
            ASSERT1(game_time_delta >= 0.0f);

            m_game_time += game_time_delta * m_game_world->Timescale();
        }

        previous_real_time = m_real_time;

        // framerate calculation
        m_framerate_calculator.AddFrameTime(m_real_time);
        if (m_game_widget != NULL)
            m_game_widget->SetFramerate(m_framerate_calculator.Framerate());

        // process events
        {
            Event *event = NULL;
            while ((event = Singleton::Pal().PollEvent(m_screen, m_real_time)) != NULL)
            {
                // process key events through the InputState singleton first
                if (event->IsKeyEvent() || event->IsMouseButtonEvent())
                    Singleton::InputState().ProcessEvent(*event);

                // also let the key repeater have a crack at it.
                m_key_repeater.ProcessEvent(*event);

                // let the screen (and the entire UI/view system) have the event
                m_screen->ProcessEvent(*event);
                Delete(event);
            }
        }

        // key repeater frame computations
        m_key_repeater.ProcessFrame(m_real_time);
        // dequeue and process any key repeat events generated
        ProcessKeyRepeatEvents();

        // these will store the millisecond durations for various processes
        Time::Delta world_frame_dt = 0;
        Time::Delta gui_frame_dt = 0;
        Time::Delta render_frame_dt = 0;

        // world frame
        if (m_game_world != NULL)
        {
            Time world_frame_start_time = Singleton::Pal().CurrentTime();
            m_game_world->ProcessFrame(m_game_time);
            world_frame_dt = Singleton::Pal().CurrentTime() - world_frame_start_time;
        }

        // gui frame
        {
            Time gui_frame_start_time = Singleton::Pal().CurrentTime();
            // process events from the gui event queue
            m_screen->OwnerEventQueue()->ProcessFrame(m_real_time);
            // frame computations for the UI/view system
            m_screen->ProcessFrame(m_real_time);
            // process events from the gui event queue again
            m_screen->OwnerEventQueue()->ProcessFrame(m_real_time);
            gui_frame_dt = Singleton::Pal().CurrentTime() - gui_frame_start_time;
        }

        // rendering
        {
            Time render_frame_start_time = Singleton::Pal().CurrentTime();
            m_screen->Draw(m_real_time);
            render_frame_dt = Singleton::Pal().CurrentTime() - render_frame_start_time;
        }

        // set the (previous) game loop's process durations
        if (m_game_widget != NULL)
        {
            m_game_widget->SetWorldFrameDT(world_frame_dt);
            m_game_widget->SetGUIFrameDT(gui_frame_dt);
            m_game_widget->SetRenderFrameDT(render_frame_dt);

            Uint32 bind_texture_call_count = Singleton::Gl().BindTextureCallCount();
            Uint32 bind_texture_call_hit_percent = 100;
            if (bind_texture_call_count != 0)
                bind_texture_call_hit_percent = 100 * Singleton::Gl().BindTextureCallHitCount() / bind_texture_call_count;
            m_game_widget->SetBindTextureCallCount(bind_texture_call_count);
            m_game_widget->SetBindTextureCallHitPercent(bind_texture_call_hit_percent);
            Singleton::Gl().ResetBindTextureCallCounts();

            m_game_widget->SetEntityCount(m_game_world->EntityCount());
        }
    }

    ASSERT1(m_game_widget == NULL);
    ASSERT1(m_game_world == NULL);
    ASSERT1(m_title_screen_widget == NULL);
}

bool Master::HandleEvent (Event const &e)
{
    ASSERT1(e.GetEventType() == Event::CUSTOM);

    EventBase const &event = dynamic_cast<EventBase const &>(e);
    switch (event.GetCustomType())
    {
        case EventBase::ACTIVATE_TITLE_SCREEN:   ActivateTitleScreen();      return true;
        case EventBase::DEACTIVATE_TITLE_SCREEN: DeactivateTitleScreen();    return true;
        case EventBase::ACTIVATE_GAME:           ActivateGame();             return true;
        case EventBase::DEACTIVATE_GAME:         DeactivateGame();           return true;
        case EventBase::QUIT_REQUESTED:          m_is_quit_requested = true; return true;

        default: ASSERT1(false && "This event shouldn't be used here");
    }

    return false;
}

void Master::AcceptScore (Score const &score)
{
    ASSERT1(m_game_world != NULL);

    if (m_high_scores.IsNewHighScore(score))
    {
        // create a HighScoreNameEntryDialog
        HighScoreNameEntryDialog *dialog = new HighScoreNameEntryDialog(score.Points(), score.WaveCount(), m_screen->Context());
        dialog->CenterOnWidget(*m_screen);
        m_screen->AttachAsModalChildWidget(*dialog);
        // hook up the OK/cancel button signals
        SignalHandler::Connect1(
            dialog->SenderSubmitName(),
            &m_internal_receiver_accept_name);
        // save the score off
        m_saved_score = score;
    }
    else
        m_game_world->SubmitScoreDone();
}

void Master::AcceptName (std::string const &name)
{
    ASSERT1(m_game_world != NULL);

    std::string checked_name(name);
    if (checked_name.empty())
        checked_name = "Anonymous";
    Score named_score(
        checked_name,
        m_saved_score.Points(),
        m_saved_score.WaveCount(),
        m_saved_score.Date());
    m_show_best_points_high_scores_first = m_high_scores.AddScore(named_score);
    m_high_scores.Write(HIGH_SCORES_FILENAME);
    m_game_world->SubmitScoreDone();

    m_show_high_scores_immediately = true;
}

void Master::StartGame ()
{
    ASSERT1(m_title_screen_widget != NULL);
    ASSERT1(m_game_widget == NULL);
    ASSERT1(m_game_world == NULL);

    EnqueueEvent(new EventBase(EventBase::DEACTIVATE_TITLE_SCREEN, m_real_time));
    EnqueueEvent(new EventBase(EventBase::ACTIVATE_GAME, m_real_time));
}

void Master::QuitGame ()
{
    // this can happen either at the title screen or during the game

    if (m_game_widget != NULL)
    {
        ASSERT1(m_game_world != NULL);
        ASSERT1(m_title_screen_widget == NULL);
        EnqueueEvent(new EventBase(EventBase::DEACTIVATE_GAME, m_real_time));
    }
    else
    {
        ASSERT1(m_game_world == NULL);
        ASSERT1(m_title_screen_widget != NULL);
        EnqueueEvent(new EventBase(EventBase::DEACTIVATE_TITLE_SCREEN, m_real_time));
    }

    EnqueueEvent(new EventBase(EventBase::QUIT_REQUESTED, m_real_time));
}

void Master::EndGame ()
{
    ASSERT1(m_title_screen_widget == NULL);
    ASSERT1(m_game_widget != NULL);
    ASSERT1(m_game_world != NULL);

    EnqueueEvent(new EventBase(EventBase::DEACTIVATE_GAME, m_real_time));
    EnqueueEvent(new EventBase(EventBase::ACTIVATE_TITLE_SCREEN, m_real_time));
}

void Master::ActivateTitleScreen ()
{
    ASSERT1(m_game_widget == NULL);
    ASSERT1(m_game_world == NULL);
    ASSERT1(m_title_screen_widget == NULL);

    // create a new title screen and set it as the main widget
    m_title_screen_widget = new TitleScreenWidget(m_show_high_scores_immediately, m_show_best_points_high_scores_first, m_screen->Context());
    m_title_screen_widget->SetHighScores(m_high_scores);
    m_screen->AttachChild(m_title_screen_widget);
    m_screen->SetMainWidget(m_title_screen_widget);

    m_show_high_scores_immediately = false;

    // hook up the necessary signals
    SignalHandler::Connect0(
        m_title_screen_widget->SenderStartGame(),
        &m_internal_receiver_start_game);
    SignalHandler::Connect0(
        m_title_screen_widget->SenderQuitGame(),
        &m_internal_receiver_quit_game);
}

void Master::DeactivateTitleScreen ()
{
    ASSERT1(m_game_widget == NULL);
    ASSERT1(m_game_world == NULL);
    ASSERT1(m_title_screen_widget != NULL);

    // detach, delete and nullify the title screen
    m_title_screen_widget->DetachFromParent();
    DeleteAndNullify(m_title_screen_widget);
}

void Master::ActivateGame ()
{
    ASSERT1(m_game_widget == NULL);
    ASSERT1(m_game_world == NULL);
    ASSERT1(m_title_screen_widget == NULL);

    // create the game world
    m_game_world = World::Create(g_config.GetDifficultyLevel());
    // create the game widget
    m_game_widget = new GameWidget(m_game_world, m_screen->Context());
    m_screen->AttachChild(m_game_widget);
    // set it as the main widget
    m_screen->SetMainWidget(m_game_widget);
    // reset the game time
    m_game_time = Time::ms_beginning_of;

    SignalHandler::Connect1(
        m_game_world->SenderSubmitScore(),
        &m_internal_receiver_accept_score);
    SignalHandler::Connect0(
        m_game_world->SenderEndGame(),
        &m_internal_receiver_end_game);

    SignalHandler::Connect0(
        m_game_widget->SenderQuitGame(),
        &m_internal_receiver_quit_game);
}

void Master::DeactivateGame ()
{
    ASSERT1(m_game_widget != NULL);
    ASSERT1(m_game_world != NULL);
    ASSERT1(m_title_screen_widget == NULL);

    // detach the game widget from its parent, then delete the game widget
    // and world, in that order and nullify them.
    m_game_widget->DetachFromParent();
    DeleteAndNullify(m_game_widget);
    DeleteAndNullify(m_game_world);
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
        m_screen->OwnerEventQueue()->EnqueueEvent(*m_screen, event);
    }
}

} // end of namespace Dis

