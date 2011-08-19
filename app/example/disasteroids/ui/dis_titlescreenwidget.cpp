// ///////////////////////////////////////////////////////////////////////////
// dis_titlescreenwidget.cpp by Victor Dods, created 2006/02/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_titlescreenwidget.hpp"

#include "dis_config.hpp"
#include "dis_highscoreswidget.hpp"
#include "dis_controlspanel.hpp"
#include "xrb_button.hpp"
#include "xrb_cellpaddingwidget.hpp"
#include "xrb_dialog.hpp"
#include "xrb_eventqueue.hpp"
#include "xrb_label.hpp"
#include "xrb_layout.hpp"
#include "xrb_screen.hpp"
#include "xrb_widgetbackground.hpp" // TEMP
#include "xrb_widgetstack.hpp"

using namespace Xrb;

extern Dis::Config g_config;

namespace Dis
{

TitleScreenWidget::TitleScreenWidget (
    bool const immediately_show_high_scores,
    bool const show_best_points_high_scores_first,
    ContainerWidget *const parent)
    :
    ContainerWidget(parent, "TitleScreenWidget"),
    m_state_machine(this),
    m_immediately_show_high_scores(immediately_show_high_scores),
    m_show_best_points_high_scores_first(show_best_points_high_scores_first),
    m_internal_receiver_activate_controls_dialog(&TitleScreenWidget::ActivateControlsDialog, this),
    m_internal_receiver_controls_dialog_returned(&TitleScreenWidget::ControlsDialogReturned, this)
{
    Layout *main_layout = new Layout(VERTICAL, this, "main title screen layout");
    main_layout->SetIsUsingZeroedLayoutSpacingMargins(true);

    CellPaddingWidget *logo_padding_widget =
        new CellPaddingWidget(main_layout, "logo padding widget");
    logo_padding_widget->FixHeightRatio(0.20f);

    m_logo_label = new Label("DISASTEROIDS", logo_padding_widget, "logo label");
    m_logo_label->SetFontHeightRatio(0.10f);

    WidgetStack *center_panel_widget_stack = new WidgetStack(main_layout, "center panel widget stack");

    m_game_widget_dummy = new Label("game demo widget", center_panel_widget_stack, "game widget dummy");
    m_game_widget_dummy->SetFontHeightRatio(0.10f);
    m_game_widget_dummy->SetBackground(new WidgetBackgroundColored(Color(1.0f, 0.0f, 0.0f, 1.0f)));

    CellPaddingWidget *high_scores_padding_widget = new CellPaddingWidget(center_panel_widget_stack, "high scores padding widget");

    m_high_scores_widget = new HighScoresWidget(high_scores_padding_widget);
    m_high_scores_widget->Hide();
    m_high_scores_widget->SetBackground(new WidgetBackgroundColored(Color(0.0f, 0.0f, 0.0f, 0.5f)));

    Layout *stuff_layout = new Layout(VERTICAL, main_layout, "stuff layout");
    stuff_layout->FixHeightRatio(0.20f);
    stuff_layout->SetIsUsingZeroedLayoutSpacingMargins(true);

    CellPaddingWidget *controls_padding_widget =
        new CellPaddingWidget(stuff_layout, "controls padding widget");

    Layout *controls_layout = new Layout(HORIZONTAL, controls_padding_widget, "controls layout");

    m_start_button = new Button("START", controls_layout, "start button");
    m_start_button->SetIsHeightFixedToTextHeight(true);

    m_controls_button = new Button("CONTROLS", controls_layout, "controls button");
    m_controls_button->SetIsHeightFixedToTextHeight(true);

    m_quit_button = new Button("QUIT", controls_layout, "quit button");
    m_quit_button->SetIsHeightFixedToTextHeight(true);

    Layout *footnotes_layout = new Layout(HORIZONTAL, stuff_layout, "footnotes_layout layout");
    footnotes_layout->SetIsUsingZeroedLayoutSpacingMargins(true);

    Label *credits_label = new Label("By Victor Dods", footnotes_layout, "credits label");
    credits_label->SetIsHeightFixedToTextHeight(true);
    credits_label->SetAlignment(Dim::X, LEFT);

    Label *library_label = new Label("Part of the XuqRijBuh Game Engine", footnotes_layout, "library label");
    library_label->SetIsHeightFixedToTextHeight(true);
    library_label->SetAlignment(Dim::X, RIGHT);

    SetMainWidget(main_layout);

    SignalHandler::Connect0(
        m_controls_button->SenderReleased(),
        &m_internal_receiver_activate_controls_dialog);

    m_controls_panel = NULL;
}

TitleScreenWidget::~TitleScreenWidget ()
{
    m_state_machine.Shutdown();
}

SignalSender0 const *TitleScreenWidget::SenderStartGame ()
{
    ASSERT1(m_start_button != NULL);
    return m_start_button->SenderReleased();
}

SignalSender0 const *TitleScreenWidget::SenderQuitGame ()
{
    ASSERT1(m_quit_button != NULL);
    return m_quit_button->SenderReleased();
}

void TitleScreenWidget::HandleFrame ()
{
    ContainerWidget::HandleFrame();

    if (!m_state_machine.IsInitialized())
        m_state_machine.Initialize(
            m_immediately_show_high_scores ?
            &TitleScreenWidget::StateDisplayFirstHighScores :
            &TitleScreenWidget::StateGameDemo);
}

bool TitleScreenWidget::ProcessStateMachineInputEvent (EventStateMachineInput const *e)
{
    ASSERT1(e != NULL);
    m_state_machine.RunCurrentState(e->GetInput());
    return true;
}

void TitleScreenWidget::ActivateControlsDialog ()
{
    ASSERT1(m_controls_panel == NULL);

    // create the dialog and add a new ControlsPanel to it
    Dialog *controls_dialog = new Dialog(Dialog::DT_OK_CANCEL, this, "controls dialog");
    m_controls_panel = new ControlsPanel(controls_dialog->DialogLayout());
    // initialize the ControlsPanel with the Config values
    m_controls_panel->ReadValuesFromConfig(g_config);
    // make the dialog full-screen
    ASSERT1(TopLevelParent() != NULL);
    controls_dialog->MoveToAndResize(TopLevelParent()->ScreenRect());
    // connect up the dialog OK button to ControlsDialogReturnedOK
    SignalHandler::Connect1(
        controls_dialog->SenderDialogReturned(),
        &m_internal_receiver_controls_dialog_returned);
}

void TitleScreenWidget::ControlsDialogReturned (Dialog::ButtonID const button_id)
{
    ASSERT1(m_controls_panel != NULL);

    // only save the ControlsPanel values back into the Config if OK button was hit
    if (button_id == Dialog::ID_OK)
        m_controls_panel->WriteValuesToConfig(&g_config);

    m_controls_panel = NULL;
}

// ///////////////////////////////////////////////////////////////////////////
// begin state machine stuff
// ///////////////////////////////////////////////////////////////////////////

#define STATE_MACHINE_STATUS(state_name) \
    /* if (input == SM_ENTER) \
        fprintf(stderr, "TitleScreenWidget: --> " state_name "\n"); \
    else if (input == SM_EXIT) \
        fprintf(stderr, "TitleScreenWidget: <-- " state_name "\n"); \
    else \
        fprintf(stderr, "TitleScreenWidget: input: %u\n", input);*/

#define TRANSITION_TO(x) m_state_machine.SetNextState(&TitleScreenWidget::x)

bool TitleScreenWidget::StateGameDemo (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StateGameDemo")
    ASSERT1(m_high_scores_widget->IsHidden());
    switch (input)
    {
        case SM_ENTER:
            ScheduleStateMachineInput(IN_TIME_OUT, 20.0f);
            return true;

        case IN_TIME_OUT:
            TRANSITION_TO(StateDisplayFirstHighScores);
            return true;
    }
    return false;
}

bool TitleScreenWidget::StateDisplayFirstHighScores (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StateDisplayFirstHighScores")
    switch (input)
    {
        case SM_ENTER:
            ASSERT1(m_high_scores_widget->IsHidden());
            m_high_scores_widget->Update(
                m_high_scores,
                m_show_best_points_high_scores_first ?
                    HighScoresWidget::M_BEST_POINTS :
                    HighScoresWidget::M_BEST_WAVE_COUNT);
            m_high_scores_widget->Show();
            ScheduleStateMachineInput(IN_TIME_OUT, 10.0f);
            return true;

        case IN_TIME_OUT:
            ASSERT1(!m_high_scores_widget->IsHidden());
            m_high_scores_widget->Hide();
            TRANSITION_TO(StatePauseBetweenHighScores);
            return true;
    }
    return false;
}

bool TitleScreenWidget::StatePauseBetweenHighScores (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StatePauseBetweenHighScores")
    ASSERT1(m_high_scores_widget->IsHidden());
    switch (input)
    {
        case SM_ENTER:
            ScheduleStateMachineInput(IN_TIME_OUT, 1.0f);
            return true;

        case IN_TIME_OUT:
            TRANSITION_TO(StateDisplaySecondHighScores);
            return true;
    }
    return false;
}

bool TitleScreenWidget::StateDisplaySecondHighScores (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StateDisplaySecondHighScores")
    switch (input)
    {
        case SM_ENTER:
            ASSERT1(m_high_scores_widget->IsHidden());
            m_high_scores_widget->Update(
                m_high_scores,
                m_show_best_points_high_scores_first ?
                    HighScoresWidget::M_BEST_WAVE_COUNT :
                    HighScoresWidget::M_BEST_POINTS);
            m_high_scores_widget->Show();
            ScheduleStateMachineInput(IN_TIME_OUT, 10.0f);
            return true;

        case IN_TIME_OUT:
            ASSERT1(!m_high_scores_widget->IsHidden());
            m_high_scores_widget->Hide();
            TRANSITION_TO(StateGameDemo);
            return true;
    }
    return false;
}

void TitleScreenWidget::ScheduleStateMachineInput (StateMachineInput const input, Float const time_delay)
{
    CancelScheduledStateMachineInput();
    EnqueueEvent(new EventStateMachineInput(input, MostRecentFrameTime() + time_delay));
}

void TitleScreenWidget::CancelScheduledStateMachineInput ()
{
    OwnerEventQueue()->ScheduleMatchingEventsForDeletion(
        MatchEventType,
        Event::STATE_MACHINE_INPUT);
}

// ///////////////////////////////////////////////////////////////////////////
// end state machine stuff
// ///////////////////////////////////////////////////////////////////////////

} // end of namespace Dis

