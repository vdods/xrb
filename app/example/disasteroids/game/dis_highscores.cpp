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

#include "dis_util.h"
#include "xrb_datafileparser.h"
#include "xrb_datafilevalue.h"

using namespace Xrb;

namespace Dis
{

// ///////////////////////////////////////////////////////////////////////////
// Score
// ///////////////////////////////////////////////////////////////////////////

Uint32 Score::ComputeHash (
    std::string const &name,
    Uint32 const points,
    Uint32 const wave_count,
    time_t const date)
{
    Uint32 hash = points ^ wave_count ^ static_cast<Uint32>(date);
    for (Uint32 i = 0; i < name.length(); ++i)
        hash ^= hash * 11 + static_cast<Uint32>(name[i] - 'a');
    return hash;
}

// ///////////////////////////////////////////////////////////////////////////
// HighScores
// ///////////////////////////////////////////////////////////////////////////

HighScores::HighScores ()
{
    // add some modest fake scores
    for (Uint32 i = 1; i <= MAX_HIGH_SCORES; ++i)
    {
        Score fake_score("XuqRijBuh", 500 * i, (i-1)/2+1, static_cast<time_t>(0));
        m_best_points_score_set.insert(fake_score);
        m_best_wave_count_score_set.insert(fake_score);
    }
}

HighScores::HighScores (HighScores const &high_scores)
{
    m_best_points_score_set = high_scores.m_best_points_score_set;
    m_best_wave_count_score_set = high_scores.m_best_wave_count_score_set;
    m_score_list = high_scores.m_score_list;
}

HighScores::~HighScores ()
{
}

void HighScores::operator = (HighScores const &high_scores)
{
    m_best_points_score_set = high_scores.m_best_points_score_set;
    m_best_wave_count_score_set = high_scores.m_best_wave_count_score_set;
    m_score_list = high_scores.m_score_list;
}

bool HighScores::GetIsNewHighScore (Score const &score)
{
    ASSERT1(!m_best_points_score_set.empty())
    ASSERT1(!m_best_wave_count_score_set.empty())

    ScoreOrderByPoints score_order_by_points;
    ScoreOrderByWaveCount score_order_by_wave_count;

    // make sure it doesn't exactly match any existing high score (this is
    // valid because of the date member of Score).
    ASSERT1(m_best_points_score_set.find(score) == m_best_points_score_set.end())
    ASSERT1(m_best_wave_count_score_set.find(score) == m_best_wave_count_score_set.end())

    // if the score surpasses the worst best-points, it's a new high score
    if (m_best_points_score_set.empty() ||
        score_order_by_points(score, *m_best_points_score_set.rbegin()))
    {
        return true;
    }

    // if the score surpasses the worst best-time-alive, it's a new high score
    if (m_best_wave_count_score_set.empty() ||
        score_order_by_wave_count(score, *m_best_wave_count_score_set.rbegin()))
    {
        return true;
    }

    // otherwise not
    return false;
}

Score const &HighScores::GetBestPointsScore (Uint32 index) const
{
    BestPointsScoreSetConstIterator it = m_best_points_score_set.begin();
    while (index > 0)
    {
        --index;
        ++it;
        ASSERT1(it != m_best_points_score_set.end());
    }
    return *it;
}

Score const &HighScores::GetBestWaveCountScore (Uint32 index) const
{
    BestWaveCountScoreSetConstIterator it = m_best_wave_count_score_set.begin();
    while (index > 0)
    {
        --index;
        ++it;
        ASSERT1(it != m_best_wave_count_score_set.end());
    }
    return *it;
}

void HighScores::AddScore (Score const &score)
{
    ASSERT1(!score.GetName().empty())
    ASSERT1(GetIsNewHighScore(score))

    ScoreOrderByPoints score_order_by_points;
    ScoreOrderByWaveCount score_order_by_wave_count;

    // if the score surpasses the worst best-points, add it and remove the worst score
    if (m_best_points_score_set.empty())
    {
        m_best_points_score_set.insert(score);
    }
    else if (score_order_by_points(score, *m_best_points_score_set.rbegin()))
    {
        StlContainerEraseRBegin(m_best_points_score_set);
        m_best_points_score_set.insert(score);
    }

    // if the score surpasses the worst best-time-alive, add it and remove the worst score
    if (m_best_wave_count_score_set.empty())
    {
        m_best_wave_count_score_set.insert(score);
    }
    else if (score_order_by_wave_count(score, *m_best_wave_count_score_set.rbegin()))
    {
        StlContainerEraseRBegin(m_best_wave_count_score_set);
        m_best_wave_count_score_set.insert(score);
    }

    // always add it to the score list
    m_score_list.push_back(score);
}

void HighScores::Read (std::string const &filename)
{
    DataFileParser parser;
    if (parser.Parse(filename) == DataFileParser::RC_SUCCESS)
    {
        DataFileStructure const *root = parser.GetAcceptedStructure();

        // we're looking for a structure called high_scores, which is an
        // array of structures each with elements
        // name (string)
        // points (unsigned integer)
        // wave_count (unsigned integer)
        // date (unsigned integer)
        // hash (unsigned integer)

        DataFileArray const *high_scores = root->GetPathElementArray("|high_scores");
        if (high_scores == NULL)
            return;

        for (Uint32 i = 0; i < high_scores->GetElementCount(); ++i)
        {
            DataFileValue const *high_score = high_scores->GetElement(i);
            try
            {
                Score score(
                    high_score->GetPathElementString("|name"),
                    high_score->GetPathElementUint32("|points"),
                    high_score->GetPathElementUint32("|wave_count"),
                    high_score->GetPathElementUint32("|date"));
                Uint32 hash = high_score->GetPathElementUint32("|hash");
                // this check is to prevent people from editing the high scores
                // file to add fake high scores
                if (hash == score.GetHash())
                    AddScore(score);
            }
            catch (...)
            {
                // ignore malformed high scores
            }
        }
    }
}

void HighScores::Write (std::string const &filename)
{
    FILE *fptr = fopen(filename.c_str(), "wt");
    if (fptr == NULL)
        return;

    DataFileStructure *root = new DataFileStructure();
    for (ScoreListConstIterator it = m_score_list.begin(),
                                it_end = m_score_list.end();
         it != it_end;
         ++it)
    {
        root->SetPathElementString("|high_scores|+|name", it->GetName());
        root->SetPathElementUint32("|high_scores|$|points", it->GetPoints());
        root->SetPathElementUint32("|high_scores|$|wave_count", it->GetWaveCount());
        root->SetPathElementUint32("|high_scores|$|date", static_cast<Uint32>(it->GetDate()));
        root->SetPathElementUint32("|high_scores|$|hash", it->GetHash());
    }

    IndentFormatter formatter(fptr, "    ");
    root->Print(formatter);
    Delete(root);
    fclose(fptr);
}

} // end of namespace Dis

