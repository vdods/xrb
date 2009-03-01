// ///////////////////////////////////////////////////////////////////////////
// dis_highscoreswidget.cpp by Victor Dods, created 2006/03/22
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_highscoreswidget.hpp"

#include "dis_util.hpp"
#include "xrb_layout.hpp"
#include "xrb_util.hpp"

using namespace Xrb;

namespace Dis
{

HighScoresWidget::HighScoresWidget (ContainerWidget *const parent)
    :
    ContainerWidget(parent, "HighScoresWidget")
{
    Layout *main_layout = new Layout(VERTICAL, this, "main HighScoresWidget layout");
    main_layout->SetIsUsingZeroedFrameMargins(false);

    m_title_label = new Label("x", main_layout);
    m_title_label->SetIsSizeFixedToTextSize(true);
    m_title_label->SetFontHeightRatio(0.025f);

    Layout *scores_layout = new Layout(ROW, 4, main_layout, "HighScoresWidget grid layout");

    for (Uint32 row = 0; row < HighScores::MAX_HIGH_SCORES; ++row)
    {
        Label *position_label = new Label(Util::StringPrintf("#%u", row+1), scores_layout);
        position_label->SetFontHeightRatio(0.025f);
        position_label->SetAlignment(Dim::X, RIGHT);

        m_name_label[row] = new Label("x", scores_layout);
        m_name_label[row]->SetFontHeightRatio(0.025f);

        m_wave_count_label[row] = new ValueLabel<Uint32>("WAVE %u", Util::TextToUint32, scores_layout);
        m_wave_count_label[row]->SetFontHeightRatio(0.025f);
        m_wave_count_label[row]->SetAlignment(Dim::X, RIGHT);

        m_points_label[row] = new ValueLabel<Uint32>("%u", Util::TextToUint32, scores_layout);
        m_points_label[row]->SetValue(0);
        m_points_label[row]->SetFontHeightRatio(0.025f);
        m_points_label[row]->SetAlignment(Dim::X, RIGHT);
    }

    SetMainWidget(main_layout);
}

void HighScoresWidget::Update (
    HighScores const &high_scores,
    HighScoresWidget::Mode const mode)
{
    m_title_label->SetText((mode == M_BEST_POINTS) ? "HIGHEST SCORE" : "HIGHEST WAVE");

    for (Uint32 row = 0; row < HighScores::MAX_HIGH_SCORES; ++row)
    {
        Score const &score =
            (mode == M_BEST_POINTS) ?
            high_scores.GetBestPointsScore(row) :
            high_scores.GetBestWaveCountScore(row);

        m_name_label[row]->SetText(score.GetName());
        m_wave_count_label[row]->SetValue(score.GetWaveCount());
        m_points_label[row]->SetValue(score.GetPoints());
    }
}

} // end of namespace Dis

