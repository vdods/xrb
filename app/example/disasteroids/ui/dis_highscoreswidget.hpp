// ///////////////////////////////////////////////////////////////////////////
// dis_highscoreswidget.hpp by Victor Dods, created 2006/03/22
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_HIGHSCORESWIDGET_HPP_)
#define _DIS_HIGHSCORESWIDGET_HPP_

#include "xrb_containerwidget.hpp"

#include "dis_highscores.hpp"
#include "xrb_label.hpp"
#include "xrb_valuelabel.hpp"

using namespace Xrb;

namespace Dis
{

class HighScoresWidget : public ContainerWidget
{
public:

    enum Mode
    {
        M_BEST_POINTS = 0,
        M_BEST_WAVE_COUNT,

        M_COUNT
    }; // end of enum HighScoresWidget::Mode

    HighScoresWidget (ContainerWidget *parent);

    void Update (HighScores const &high_scores, Mode mode);

private:

    Label *m_title_label;

    Label *m_name_label[HighScores::MAX_HIGH_SCORES];
    ValueLabel<Uint32> *m_wave_count_label[HighScores::MAX_HIGH_SCORES];
    ValueLabel<Uint32> *m_points_label[HighScores::MAX_HIGH_SCORES];
}; // end of class HighScoresWidget

} // end of namespace Dis

#endif // !defined(_DIS_HIGHSCORESWIDGET_HPP_)

