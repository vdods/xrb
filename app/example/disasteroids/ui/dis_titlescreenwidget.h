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

using namespace Xrb;

namespace Xrb
{
class Label;
class Button;
}

namespace Dis
{

class TitleScreenWidget : public Widget
{
public:

    TitleScreenWidget (Widget *parent);
    virtual ~TitleScreenWidget ();

    SignalSender0 const *SenderStartGame ();
    SignalSender0 const *SenderQuitGame ();
    
    void GoToOptions ();
    
private:

    Label *m_logo_label;
    // TODO: this should be a GameWidget
    Label *m_game_widget_dummy;
    // TODO: this should be a HighScoresWidget
    Label *m_high_scores_widget_dummy;
    Button *m_start_button;
    Button *m_options_button;
    Button *m_quit_button;

    SignalReceiver0 m_internal_receiver_go_to_options;
}; // end of class TitleScreenWidget

} // end of namespace Dis

#endif // !defined(_DIS_TITLESCREENWIDGET_H_)

