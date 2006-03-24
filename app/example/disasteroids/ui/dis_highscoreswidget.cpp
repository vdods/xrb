// ///////////////////////////////////////////////////////////////////////////
// dis_highscoreswidget.cpp by Victor Dods, created 2006/03/22
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_highscoreswidget.h"

#include "dis_util.h"
#include "xrb_layout.h"
#include "xrb_util.h"

using namespace Xrb;

namespace Dis
{

HighScoresWidget::HighScoresWidget (Widget *const parent)
    :
    Widget(parent, "HighScoresWidget")
{
    Layout *main_layout = new Layout(VERTICAL, this, "main HighScoresWidget layout");

    m_title_label = new Label("x", main_layout);
    m_title_label->SetIsSizeFixedToTextSize(true);

    Layout *scores_layout = new Layout(ROW, 4, main_layout, "HighScoresWidget grid layout");

    for (Uint32 row = 0; row < HighScores::MAX_HIGH_SCORES; ++row)
    {
        Label *position_label = new Label(Util::StringPrintf("#%u", row+1), scores_layout);
        position_label->SetIsSizeFixedToTextSize(true);
        position_label->SetFontHeightRatio(0.025f);

        m_name_label[row] = new Label("x", scores_layout);
        m_name_label[row]->SetIsSizeFixedToTextSize(true);
        m_name_label[row]->SetFontHeightRatio(0.025f);
        
        m_time_alive_label[row] = new Label("x", scores_layout);
        m_time_alive_label[row]->SetIsSizeFixedToTextSize(true);
        m_time_alive_label[row]->SetFontHeightRatio(0.025f);
        
        m_points_label[row] = new ValueLabel<Uint32>("%u", Util::TextToUint32, scores_layout);
        m_points_label[row]->SetValue(0);
        m_points_label[row]->SetIsSizeFixedToTextSize(true);
        m_points_label[row]->SetFontHeightRatio(0.025f);
    }

    fprintf(stderr, "HighScoresWidget size before: ");
    FprintVector(stderr, GetSize());
    SetMainWidget(main_layout);
    fprintf(stderr, "HighScoresWidget size after: ");
    FprintVector(stderr, GetSize());
}

void HighScoresWidget::Update (
    HighScores const &high_scores,
    HighScoresWidget::Mode const mode)
{
    m_title_label->SetText((mode == M_BEST_POINTS) ? "HIGH SCORES - BEST POINTS" : "HIGH SCORES - BEST TIME ALIVE");

    for (Uint32 row = 0; row < HighScores::MAX_HIGH_SCORES; ++row)
    {
        Score const &score =
            (mode == M_BEST_POINTS) ?
            high_scores.GetBestPointsScore(row) :
            high_scores.GetBestTimeAliveScore(row);

        m_name_label[row]->SetText(score.GetName());
        m_time_alive_label[row]->SetText(GetFormattedTimeString(score.GetTimeAlive()));
        m_points_label[row]->SetValue(score.GetPoints());
    }
}
    
} // end of namespace Dis

