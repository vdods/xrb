// ///////////////////////////////////////////////////////////////////////////
// dis_titlescreenwidget.h by Victor Dods, created 2006/02/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_TITLESCREENWIDGET_H_)
#define _DIS_TITLESCREENWIDGET_H_

#include "xrb_widget.h"

#include "dis_highscores.h"
#include "xrb_statemachine.h"

using namespace Xrb;

namespace Xrb
{
class Label;
class Button;
}

namespace Dis
{

class HighScoresWidget;

class TitleScreenWidget : public Widget
{
public:

    TitleScreenWidget (
        bool immediately_show_high_scores,
        bool show_best_points_high_scores_first,
        Widget *parent);
    virtual ~TitleScreenWidget ();

    SignalSender0 const *SenderStartGame ();
    SignalSender0 const *SenderQuitGame ();

    void GoToOptions ();

    void SetHighScores (HighScores const &high_scores)
    {
        m_high_scores = high_scores;
    }

protected:

    virtual void ProcessFrameOverride ();

    virtual bool ProcessStateMachineInputEvent (EventStateMachineInput const *e);

private:

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
    Button *m_options_button;
    Button *m_quit_button;

    bool const m_immediately_show_high_scores;
    bool const m_show_best_points_high_scores_first;
    HighScores m_high_scores;

    SignalReceiver0 m_internal_receiver_go_to_options;
}; // end of class TitleScreenWidget

} // end of namespace Dis

#endif // !defined(_DIS_TITLESCREENWIDGET_H_)

