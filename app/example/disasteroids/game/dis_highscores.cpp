// ///////////////////////////////////////////////////////////////////////////
// dis_highscores.cpp by Victor Dods, created 2006/03/19
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_highscores.hpp"

#include <fstream>

#include "dis_util.hpp"
#include "xrb_parse_datafile_parser.hpp"
#include "xrb_parse_datafile_value.hpp"

using namespace Xrb;
using namespace Parse;

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

bool HighScores::IsNewHighScore (Score const &score)
{
    ASSERT1(!m_best_points_score_set.empty());
    ASSERT1(!m_best_wave_count_score_set.empty());

    ScoreOrderByPoints score_order_by_points;
    ScoreOrderByWaveCount score_order_by_wave_count;

    // make sure it doesn't exactly match any existing high score (this is
    // valid because of the date member of Score).
    ASSERT1(m_best_points_score_set.find(score) == m_best_points_score_set.end());
    ASSERT1(m_best_wave_count_score_set.find(score) == m_best_wave_count_score_set.end());

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

Score const &HighScores::BestPointsScore (Uint32 index) const
{
    BestPointsScoreSet::const_iterator it = m_best_points_score_set.begin();
    while (index > 0)
    {
        --index;
        ++it;
        ASSERT1(it != m_best_points_score_set.end());
    }
    return *it;
}

Score const &HighScores::BestWaveCountScore (Uint32 index) const
{
    BestWaveCountScoreSet::const_iterator it = m_best_wave_count_score_set.begin();
    while (index > 0)
    {
        --index;
        ++it;
        ASSERT1(it != m_best_wave_count_score_set.end());
    }
    return *it;
}

bool HighScores::AddScore (Score const &score)
{
    ASSERT1(!score.Name().empty());
    ASSERT1(IsNewHighScore(score));

    ScoreOrderByPoints score_order_by_points;
    ScoreOrderByWaveCount score_order_by_wave_count;

    Uint32 points_rank = 0;
    Uint32 wave_count_rank = 0;

    for (BestPointsScoreSet::const_iterator it = m_best_points_score_set.begin(),
                                            it_end = m_best_points_score_set.end();
         it != it_end && !score_order_by_points(score, *it);
         ++it)
    {
        ++points_rank;
    }

    for (BestWaveCountScoreSet::const_iterator it = m_best_wave_count_score_set.begin(),
                                               it_end = m_best_wave_count_score_set.end();
         it != it_end && !score_order_by_wave_count(score, *it);
         ++it)
    {
        ++wave_count_rank;
    }

    // if the score surpasses the worst best-points, add it and remove the worst score
    if (m_best_points_score_set.empty())
    {
        points_rank = 0;
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
        wave_count_rank = 0;
        m_best_wave_count_score_set.insert(score);
    }
    else if (score_order_by_wave_count(score, *m_best_wave_count_score_set.rbegin()))
    {
        StlContainerEraseRBegin(m_best_wave_count_score_set);
        m_best_wave_count_score_set.insert(score);
    }

    // always add it to the score list
    m_score_list.push_back(score);

    return points_rank <= wave_count_rank;
}

void HighScores::Read (std::string const &path)
{
    DataFile::Parser parser;
    if (parser.Parse(path) == DataFile::Parser::RC_SUCCESS)
    {
        DataFile::Structure const *root = parser.AcceptedStructure();

        // we're looking for a structure called high_scores, which is an
        // array of structures each with elements
        // name (string)
        // points (unsigned integer)
        // wave_count (unsigned integer)
        // date (unsigned integer)
        // hash (unsigned integer)

        DataFile::Array const *high_scores;
        try { high_scores = root->PathElementArray("|high_scores"); }
        catch (...) { /* if no high scores, quit. */ return; }

        for (Uint32 i = 0; i < high_scores->ElementCount(); ++i)
        {
            DataFile::Value const *high_score = high_scores->Element(i);
            try
            {
                Score score(
                    high_score->PathElementString("|name"),
                    high_score->PathElementUnsignedInteger("|points"),
                    high_score->PathElementUnsignedInteger("|wave_count"),
                    high_score->PathElementUnsignedInteger("|date"));
                Uint32 hash = high_score->PathElementUnsignedInteger("|hash");
                // this check is to prevent people from editing the high scores
                // file to add fake high scores
                if (hash == score.Hash())
                    AddScore(score);
            }
            catch (...) { /* ignore malformed high scores */ }
        }
    }
}

void HighScores::Write (std::string const &path)
{
    std::ofstream stream;
    stream.open(path.c_str());
    if (!stream.is_open())
    {
        std::cerr << "HighScores::Write(); could not open file \"" << path << "\" for writing" << std::endl;
        return;
    }

    DataFile::Structure *root = new DataFile::Structure();
    for (ScoreList::const_iterator it = m_score_list.begin(), it_end = m_score_list.end(); it != it_end; ++it)
    {
        try {
            root->SetPathElementString("|high_scores|+|name", it->Name());
            root->SetPathElementUnsignedInteger("|high_scores|$|points", it->Points());
            root->SetPathElementUnsignedInteger("|high_scores|$|wave_count", it->WaveCount());
            root->SetPathElementUnsignedInteger("|high_scores|$|date", static_cast<Uint32>(it->Date()));
            root->SetPathElementUnsignedInteger("|high_scores|$|hash", it->Hash());
        } catch (...) {
            ASSERT1(false && "this should never happen");
        }
    }

    IndentFormatter formatter(stream, "    ");
    root->Print(formatter);
    Delete(root);
    stream.close();
}

} // end of namespace Dis

