// ///////////////////////////////////////////////////////////////////////////
// dis_titlescreenwidget.cpp by Victor Dods, created 2006/02/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_titlescreenwidget.h"

#include "dis_events.h"
#include "dis_highscoreswidget.h"
#include "dis_optionspanel.h"
#include "xrb_button.h"
#include "xrb_cellpaddingwidget.h"
#include "xrb_dialog.h"
#include "xrb_eventqueue.h"
#include "xrb_label.h"
#include "xrb_layout.h"
#include "xrb_widgetbackground.h" // TEMP
#include "xrb_widgetstack.h"

using namespace Xrb;

namespace Dis
{

TitleScreenWidget::TitleScreenWidget (bool const immediately_show_high_scores, Widget *const parent)
    :
    Widget(parent, "TitleScreenWidget"),
    m_state_machine(this),
    m_immediately_show_high_scores(immediately_show_high_scores),
    m_internal_receiver_go_to_options(&TitleScreenWidget::GoToOptions, this)
{
    Layout *main_layout = new Layout(VERTICAL, this, "main title screen layout");
    main_layout->SetIsUsingZeroedFrameMargins(true);
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
    stuff_layout->SetIsUsingZeroedFrameMargins(true);
    stuff_layout->SetIsUsingZeroedLayoutSpacingMargins(true);
        
    CellPaddingWidget *controls_padding_widget =
        new CellPaddingWidget(stuff_layout, "controls padding widget");

    Layout *controls_layout = new Layout(HORIZONTAL, controls_padding_widget, "controls layout");

    m_start_button = new Button("START", controls_layout, "start button");
    m_start_button->SetIsHeightFixedToTextHeight(true);
    
    m_options_button = new Button("OPTIONS", controls_layout, "options button");
    m_options_button->SetIsHeightFixedToTextHeight(true);
    
    m_quit_button = new Button("QUIT", controls_layout, "quit button");
    m_quit_button->SetIsHeightFixedToTextHeight(true);

    Layout *footnotes_layout = new Layout(HORIZONTAL, stuff_layout, "footnotes_layout layout");
    footnotes_layout->SetIsUsingZeroedFrameMargins(true);
    footnotes_layout->SetIsUsingZeroedLayoutSpacingMargins(true);

    Label *credits_label = new Label("(C)opyright 2004-2006 by Victor Dods", footnotes_layout, "credits label");
    credits_label->SetIsHeightFixedToTextHeight(true);
    credits_label->SetAlignment(Dim::X, LEFT);

    Label *library_label = new Label("Part of the XuqRijBuh Game Engine", footnotes_layout, "library label");
    library_label->SetIsHeightFixedToTextHeight(true);
    library_label->SetAlignment(Dim::X, RIGHT);

    SetMainWidget(main_layout);

    SignalHandler::Connect0(
        m_options_button->SenderReleased(),
        &m_internal_receiver_go_to_options);
}

TitleScreenWidget::~TitleScreenWidget ()
{
    m_state_machine.Shutdown();
}

SignalSender0 const *TitleScreenWidget::SenderStartGame ()
{
    ASSERT1(m_start_button != NULL)
    return m_start_button->SenderReleased();
}

SignalSender0 const *TitleScreenWidget::SenderQuitGame ()
{
    ASSERT1(m_quit_button != NULL)
    return m_quit_button->SenderReleased();
}

void TitleScreenWidget::GoToOptions ()
{
    Dialog *options_dialog = new Dialog(Dialog::DT_OK_CANCEL, this, "options dialog");
    // TODO: save this OptionsPanel for later retreival of the options
/*    OptionsPanel *options_panel =*/ new OptionsPanel(options_dialog->GetDialogLayout());
    // TODO: hook up the dialog result signal
    options_dialog->Resize(options_dialog->GetParent()->GetSize() * 4 / 5);
    options_dialog->CenterOnWidget(options_dialog->GetParent());
}

void TitleScreenWidget::ProcessFrameOverride ()
{
    Widget::ProcessFrameOverride();

    if (!m_state_machine.GetIsInitialized())
        m_state_machine.Initialize(
            m_immediately_show_high_scores ?
            &TitleScreenWidget::StateDisplayBestPointsHighScores :
            &TitleScreenWidget::StateGameDemo);
}

bool TitleScreenWidget::ProcessEventOverride (Event const *const e)
{
    ASSERT1(e != NULL)

    Widget::ProcessEventOverride(e);
    
    if (e->GetType() == Event::CUSTOM)
    {
        EventBase const *dis_event = DStaticCast<EventBase const *>(e);
        switch (dis_event->GetCustomType())
        {
            case EventBase::STATE_MACHINE_INPUT:
                m_state_machine.RunCurrentState(DStaticCast<EventStateMachineInput const *>(dis_event)->GetInput());
                break;
    
            default:
                ASSERT1(false && "Unhandled custom event")
                break;
        }
        return true;
    }

    return true;
}

// ///////////////////////////////////////////////////////////////////////////
// begin state machine stuff
// ///////////////////////////////////////////////////////////////////////////

#define STATE_MACHINE_STATUS(state_name) \
    if (input == SM_ENTER) \
        fprintf(stderr, "TitleScreenWidget: --> " state_name "\n"); \
    else if (input == SM_EXIT) \
        fprintf(stderr, "TitleScreenWidget: <-- " state_name "\n"); \
    else \
        fprintf(stderr, "TitleScreenWidget: input: %u\n", input);

#define TRANSITION_TO(x) m_state_machine.RequestStateTransition(&TitleScreenWidget::x)

bool TitleScreenWidget::StateGameDemo (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StateGameDemo")
    ASSERT1(m_high_scores_widget->GetIsHidden())
    switch (input)
    {
        case SM_ENTER:
            ScheduleStateMachineInput(IN_TIME_OUT, 20.0f);
            return true;

        case IN_TIME_OUT:
            TRANSITION_TO(StateDisplayBestPointsHighScores);
            return true;
    }
    return false;
}

bool TitleScreenWidget::StateDisplayBestPointsHighScores (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StateDisplayBestPointsHighScores")
    switch (input)
    {
        case SM_ENTER:
            ASSERT1(m_high_scores_widget->GetIsHidden())
            m_high_scores_widget->Update(m_high_scores, HighScoresWidget::M_BEST_POINTS);
            m_high_scores_widget->Show();
            ScheduleStateMachineInput(IN_TIME_OUT, 10.0f);
            return true;

        case IN_TIME_OUT:
            ASSERT1(!m_high_scores_widget->GetIsHidden())
            m_high_scores_widget->Hide();
            TRANSITION_TO(StatePauseBetweenHighScores);
            return true;
    }
    return false;
}

bool TitleScreenWidget::StatePauseBetweenHighScores (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StatePauseBetweenHighScores")
    ASSERT1(m_high_scores_widget->GetIsHidden())
    switch (input)
    {
        case SM_ENTER:
            ScheduleStateMachineInput(IN_TIME_OUT, 1.0f);
            return true;

        case IN_TIME_OUT:
            TRANSITION_TO(StateDisplayBestTimeAliveHighScores);
            return true;
    }
    return false;
}

bool TitleScreenWidget::StateDisplayBestTimeAliveHighScores (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StateDisplayBestTimeAliveHighScores")
    switch (input)
    {
        case SM_ENTER:
            ASSERT1(m_high_scores_widget->GetIsHidden())
            m_high_scores_widget->Update(m_high_scores, HighScoresWidget::M_BEST_TIME_ALIVE);
            m_high_scores_widget->Show();
            ScheduleStateMachineInput(IN_TIME_OUT, 10.0f);
            return true;

        case IN_TIME_OUT:
            ASSERT1(!m_high_scores_widget->GetIsHidden())
            m_high_scores_widget->Hide();
            TRANSITION_TO(StateGameDemo);
            return true;
    }
    return false;
}

void TitleScreenWidget::ScheduleStateMachineInput (StateMachineInput const input, Float const time_delay)
{
    CancelScheduledStateMachineInput();
    EnqueueEvent(new EventStateMachineInput(input, GetMostRecentFrameTime() + time_delay));
}

void TitleScreenWidget::CancelScheduledStateMachineInput ()
{
    GetOwnerEventQueue()->ScheduleMatchingEventsForDeletion(
        MatchCustomType,
        static_cast<EventCustom::CustomType>(EventBase::STATE_MACHINE_INPUT));
}

// ///////////////////////////////////////////////////////////////////////////
// end state machine stuff
// ///////////////////////////////////////////////////////////////////////////

} // end of namespace Dis
