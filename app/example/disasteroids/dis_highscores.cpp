// ///////////////////////////////////////////////////////////////////////////
// dis_highscores.cpp by Victor Dods, created 2006/03/19
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_highscores.h"

using namespace Xrb;

namespace Dis
{

HighScores::HighScores ()
{
    // add some modest fake scores
    for (Uint32 i = 1; i <= MAX_HIGH_SCORES; ++i)
    {
        Score fake_score("XuqRijBuh", 500 * i, 10.0f * i, static_cast<time_t>(0));
        m_best_points_score_set.insert(fake_score);
        m_best_time_alive_score_set.insert(fake_score);
    }
}

HighScores::~HighScores ()
{
}

bool HighScores::GetIsNewHighScore (Score const &score)
{
    ASSERT1(!m_best_points_score_set.empty())
    ASSERT1(!m_best_time_alive_score_set.empty())

    ScoreOrderByPoints score_order_by_points;
    ScoreOrderByTimeAlive score_order_by_time_alive;

    // make sure it doesn't exactly match any existing high score (this is
    // valid because of the date member of Score).
    ASSERT1(m_best_points_score_set.find(score) == m_best_points_score_set.end())
    ASSERT1(m_best_time_alive_score_set.find(score) == m_best_time_alive_score_set.end())
    
    // if the score surpasses the worst best-points, it's a new high score
    if (score_order_by_points(score, *m_best_points_score_set.rbegin()))
        return true;

    // if the score surpasses the worst best-time-alive, it's a new high score
    if (score_order_by_time_alive(score, *m_best_time_alive_score_set.rbegin()))
        return true;

    // otherwise not
    return false;
}

void HighScores::AddScore (Score const &score)
{
    ASSERT1(!score.GetName().empty())
    ASSERT1(GetIsNewHighScore(score))

    ScoreOrderByPoints score_order_by_points;
    ScoreOrderByTimeAlive score_order_by_time_alive;
    
    // if the score surpasses the worst best-points, add it and remove the worst score
    if (score_order_by_points(score, *m_best_points_score_set.rbegin()))
    {
        m_best_points_score_set.insert(score);
        m_best_points_score_set.erase(*m_best_points_score_set.rbegin());
    }

    // if the score surpasses the worst best-time-alive, add it and remove the worst score
    if (score_order_by_time_alive(score, *m_best_time_alive_score_set.rbegin()))
    {
        m_best_time_alive_score_set.insert(score);
        m_best_time_alive_score_set.erase(*m_best_time_alive_score_set.rbegin());
    }
}

void HighScores::Print (FILE *const fptr) const
{
    ASSERT1(fptr != NULL)

    Uint32 position;
    
    fprintf(fptr, "best by number of points\n");
    position = 1;
    for (BestPointsScoreSetConstIterator it = m_best_points_score_set.begin(),
                                         it_end = m_best_points_score_set.end();
         it != it_end;
         ++it)
    {
        Score const &score = *it;
        Uint32 game_time_seconds = static_cast<Uint32>(score.GetTimeAlive());
        Uint32 minutes_alive = game_time_seconds / 60;
        Uint32 seconds_alive = game_time_seconds % 60;
        Uint32 centiseconds_alive = static_cast<Uint32>(100.0f * score.GetTimeAlive()) % 100;
        fprintf(
            fptr,
            "#%2u - %40s - %10u - %02u:%02u.%02u\n",
            position++,
            score.GetName().c_str(),
            score.GetPoints(),
            minutes_alive,
            seconds_alive,
            centiseconds_alive);
    }
    fprintf(fptr, "\n");
    
    fprintf(fptr, "best by time alive\n");
    position = 1;
    for (BestTimeAliveScoreSetConstIterator it = m_best_time_alive_score_set.begin(),
                                            it_end = m_best_time_alive_score_set.end();
         it != it_end;
         ++it)
    {
        Score const &score = *it;
        Uint32 game_time_seconds = static_cast<Uint32>(score.GetTimeAlive());
        Uint32 minutes_alive = game_time_seconds / 60;
        Uint32 seconds_alive = game_time_seconds % 60;
        Uint32 centiseconds_alive = static_cast<Uint32>(100.0f * score.GetTimeAlive()) % 100;
        fprintf(
            fptr,
            "#%2u - %40s - %02u:%02u.%02u - %10u\n",
            position++,
            score.GetName().c_str(),
            minutes_alive,
            seconds_alive,
            centiseconds_alive,
            score.GetPoints());
    }
    fprintf(fptr, "\n");
}

} // end of namespace Dis

