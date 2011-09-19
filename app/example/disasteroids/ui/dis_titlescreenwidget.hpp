// ///////////////////////////////////////////////////////////////////////////
// dis_titlescreenwidget.hpp by Victor Dods, created 2006/02/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_TITLESCREENWIDGET_HPP_)
#define _DIS_TITLESCREENWIDGET_HPP_

#include "xrb_containerwidget.hpp"

#include "dis_highscores.hpp"
#include "xrb_dialog.hpp"
#include "xrb_statemachine.hpp"

using namespace Xrb;

namespace Xrb {

class Label;
class Button;

}

namespace Dis {

class HighScoresWidget;
class ControlsPanel;

class TitleScreenWidget : public ContainerWidget
{
public:

    TitleScreenWidget (bool immediately_show_high_scores, bool show_best_points_high_scores_first, WidgetContext &context);
    virtual ~TitleScreenWidget ();

    SignalSender0 const *SenderStartGame ();
    SignalSender0 const *SenderQuitGame ();

    void SetHighScores (HighScores const &high_scores)
    {
        m_high_scores = high_scores;
    }

protected:

    virtual void HandleFrame ();

    virtual bool ProcessStateMachineInputEvent (EventStateMachineInput const *e);

private:

    void ActivateControlsDialog ();
    void ControlsDialogReturned (Dialog::ButtonID button_id);

    // ///////////////////////////////////////////////////////////////////////
    // begin state machine stuff

    enum
    {
        IN_TIME_OUT = 0,
    };

    bool StateGameDemo (StateMachineInput);
    bool StateDisplayFirstHighScores (StateMachineInput);
    bool StatePauseBetweenHighScores (StateMachineInput);
    bool StateDisplaySecondHighScores (StateMachineInput);

    void ScheduleStateMachineInput (StateMachineInput input, Float time_delay);
    void CancelScheduledStateMachineInput ();

    StateMachine<TitleScreenWidget> m_state_machine;

    // end state machine stuff
    // ///////////////////////////////////////////////////////////////////////

    Label *m_logo_label;
    // TODO: this should be a GameWidget
    Label *m_game_widget_dummy;
    HighScoresWidget *m_high_scores_widget;
    Button *m_start_button;
    Button *m_controls_button;
    Button *m_quit_button;
    ControlsPanel *m_controls_panel;

    bool const m_immediately_show_high_scores;
    bool const m_show_best_points_high_scores_first;
    HighScores m_high_scores;

    SignalReceiver0 m_internal_receiver_activate_controls_dialog;
    SignalReceiver1<Dialog::ButtonID> m_internal_receiver_controls_dialog_returned;
}; // end of class TitleScreenWidget

} // end of namespace Dis

#endif // !defined(_DIS_TITLESCREENWIDGET_HPP_)

