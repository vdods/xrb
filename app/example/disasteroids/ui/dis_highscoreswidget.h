// ///////////////////////////////////////////////////////////////////////////
// dis_highscoreswidget.h by Victor Dods, created 2006/03/22
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_HIGHSCORESWIDGET_H_)
#define _DIS_HIGHSCORESWIDGET_H_

#include "xrb_widget.h"

#include "dis_highscores.h"
#include "xrb_label.h"
#include "xrb_valuelabel.h"

using namespace Xrb;

namespace Dis
{

class HighScoresWidget : public Widget
{
public:

    enum Mode
    {
        M_BEST_POINTS = 0,
        M_BEST_TIME_ALIVE,

        M_COUNT
    }; // end of enum HighScoresWidget::Mode

    HighScoresWidget (Widget *parent);

    void Update (HighScores const &high_scores, Mode mode);
    
private:

    Label *m_title_label;

    Label *m_name_label[HighScores::MAX_HIGH_SCORES];
    Label *m_time_alive_label[HighScores::MAX_HIGH_SCORES];
    ValueLabel<Uint32> *m_points_label[HighScores::MAX_HIGH_SCORES];
}; // end of class HighScoresWidget

} // end of namespace Dis

#endif // !defined(_DIS_HIGHSCORESWIDGET_H_)

