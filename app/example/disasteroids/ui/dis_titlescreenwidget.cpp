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

namespace Dis {

TitleScreenWidget::TitleScreenWidget (bool immediately_show_high_scores, bool show_best_points_high_scores_first, WidgetContext &context)
    :
    ContainerWidget(context, "TitleScreenWidget"),
    m_state_machine(this),
    m_immediately_show_high_scores(immediately_show_high_scores),
    m_show_best_points_high_scores_first(show_best_points_high_scores_first),
    m_internal_receiver_activate_controls_dialog(&TitleScreenWidget::ActivateControlsDialog, this),
    m_internal_receiver_controls_dialog_returned(&TitleScreenWidget::ControlsDialogReturned, this)
{
    Layout *main_layout = new Layout(VERTICAL, Context(), "main title screen layout");
    main_layout->SetIsUsingZeroedLayoutSpacingMargins(true);
    {
        CellPaddingWidget *logo_padding_widget = new CellPaddingWidget(Context(), "logo padding widget");
        logo_padding_widget->FixHeightRatio(0.20f);
        {
            m_logo_label = new Label("DISASTEROIDS", Context(), "logo label");
            m_logo_label->SetFontStyle("giant");
            logo_padding_widget->AttachChild(m_logo_label);
        }
        main_layout->AttachChild(logo_padding_widget);

        WidgetStack *center_panel_widget_stack = new WidgetStack(Context(), "center panel widget stack");
        {
            m_game_widget_dummy = new Label("game demo widget", Context(), "game widget dummy");
            m_game_widget_dummy->SetFontStyle("giant");
            m_game_widget_dummy->SetBackgroundStyle("game_demo");
            center_panel_widget_stack->AttachChild(m_game_widget_dummy);

            CellPaddingWidget *high_scores_padding_widget = new CellPaddingWidget(Context(), "high scores padding widget");
            {
                m_high_scores_widget = new HighScoresWidget(Context());
                m_high_scores_widget->SetBackgroundStyle("black_half_opaque");
                high_scores_padding_widget->AttachChild(m_high_scores_widget);
                m_high_scores_widget->Hide();
            }
            center_panel_widget_stack->AttachChild(high_scores_padding_widget);
        }
        main_layout->AttachChild(center_panel_widget_stack);

        Layout *stuff_layout = new Layout(VERTICAL, Context(), "stuff layout");
        stuff_layout->FixHeightRatio(0.20f);
        stuff_layout->SetIsUsingZeroedLayoutSpacingMargins(true);
        {
            CellPaddingWidget *controls_padding_widget = new CellPaddingWidget(Context(), "controls padding widget");
            {
                Layout *controls_layout = new Layout(HORIZONTAL, Context(), "controls layout");
                {
                    m_start_button = new Button("START", Context(), "start button");
                    m_start_button->SetIsHeightFixedToTextHeight(true);
                    controls_layout->AttachChild(m_start_button);

                    m_controls_button = new Button("CONTROLS", Context(), "controls button");
                    m_controls_button->SetIsHeightFixedToTextHeight(true);
                    controls_layout->AttachChild(m_controls_button);

                    m_quit_button = new Button("QUIT", Context(), "quit button");
                    m_quit_button->SetIsHeightFixedToTextHeight(true);
                    controls_layout->AttachChild(m_quit_button);
                }
                controls_padding_widget->AttachChild(controls_layout);
            }
            stuff_layout->AttachChild(controls_padding_widget);
            
            Layout *footnotes_layout = new Layout(HORIZONTAL, Context(), "footnotes_layout layout");
            footnotes_layout->SetIsUsingZeroedLayoutSpacingMargins(true);
            {
                Label *credits_label = new Label("By Victor Dods", Context(), "credits label");
                credits_label->SetIsHeightFixedToTextHeight(true);
                credits_label->SetAlignment(Dim::X, LEFT);
                footnotes_layout->AttachChild(credits_label);

                Label *library_label = new Label("Part of the XuqRijBuh Game Engine", Context(), "library label");
                library_label->SetIsHeightFixedToTextHeight(true);
                library_label->SetAlignment(Dim::X, RIGHT);
                footnotes_layout->AttachChild(library_label);
            }
            stuff_layout->AttachChild(footnotes_layout);
        }
        main_layout->AttachChild(stuff_layout);
    }
    this->AttachChild(main_layout);
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
    Dialog *controls_dialog = new Dialog(Dialog::DT_OK_CANCEL, Context(), "controls dialog");
    {
        m_controls_panel = new ControlsPanel(Context());
        // initialize the ControlsPanel with the Config values
        m_controls_panel->ReadValuesFromConfig(g_config);
        // attach it to the dialog
        controls_dialog->DialogLayout()->AttachChild(m_controls_panel);
        // make the dialog full-screen
        controls_dialog->MoveToAndResize(Context().GetScreen().ScreenRect());
    }
    Context().GetScreen().AttachAsModalChildWidget(*controls_dialog);

    // connect up the dialog OK button to ControlsDialogReturnedOK
    SignalHandler::Connect1(
        controls_dialog->SenderDialogReturned(),
        &m_internal_receiver_controls_dialog_returned);
}

void TitleScreenWidget::ControlsDialogReturned (Dialog::ButtonID button_id)
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

bool TitleScreenWidget::StateGameDemo (StateMachineInput input)
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

bool TitleScreenWidget::StateDisplayFirstHighScores (StateMachineInput input)
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

bool TitleScreenWidget::StatePauseBetweenHighScores (StateMachineInput input)
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

bool TitleScreenWidget::StateDisplaySecondHighScores (StateMachineInput input)
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

void TitleScreenWidget::ScheduleStateMachineInput (StateMachineInput input, Float const time_delay)
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

