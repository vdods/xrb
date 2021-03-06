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

namespace Dis {

HighScoresWidget::HighScoresWidget (WidgetContext &context)
    :
    ContainerWidget(context, "HighScoresWidget")
{
    Layout *main_layout = new Layout(VERTICAL, Context(), "main HighScoresWidget layout");
    main_layout->SetIsUsingZeroedFrameMargins(false);
    {
        m_title_label = new Label("x", Context());
        m_title_label->SetIsSizeFixedToTextSize(true);
        main_layout->AttachChild(m_title_label);

        Layout *scores_layout = new Layout(ROW, 4, Context(), "HighScoresWidget grid layout");
        for (Uint32 row = 0; row < HighScores::MAX_HIGH_SCORES; ++row)
        {
            Label *position_label = new Label(Util::StringPrintf("#%u", row+1), Context());
            position_label->SetAlignment(Dim::X, RIGHT);
            scores_layout->AttachChild(position_label);

            m_name_label[row] = new Label("x", Context());
            scores_layout->AttachChild(m_name_label[row]);

            m_wave_count_label[row] = new ValueLabel<Uint32>("WAVE %u", Util::TextToUint<Uint32>, Context());
            m_wave_count_label[row]->SetAlignment(Dim::X, RIGHT);
            scores_layout->AttachChild(m_wave_count_label[row]);

            m_points_label[row] = new ValueLabel<Uint32>("%u", Util::TextToUint<Uint32>, Context());
            m_points_label[row]->SetValue(0);
            m_points_label[row]->SetAlignment(Dim::X, RIGHT);
            scores_layout->AttachChild(m_points_label[row]);
        }
        main_layout->AttachChild(scores_layout);
    }
    this->AttachChild(main_layout);
    SetMainWidget(main_layout);
}

void HighScoresWidget::Update (HighScores const &high_scores, HighScoresWidget::Mode mode)
{
    m_title_label->SetText((mode == M_BEST_POINTS) ? "HIGHEST SCORE" : "HIGHEST WAVE");

    for (Uint32 row = 0; row < HighScores::MAX_HIGH_SCORES; ++row)
    {
        Score const &score =
            (mode == M_BEST_POINTS) ?
            high_scores.BestPointsScore(row) :
            high_scores.BestWaveCountScore(row);

        m_name_label[row]->SetText(score.Name());
        m_wave_count_label[row]->SetValue(score.WaveCount());
        m_points_label[row]->SetValue(score.Points());
    }
}

} // end of namespace Dis

