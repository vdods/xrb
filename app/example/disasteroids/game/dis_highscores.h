// ///////////////////////////////////////////////////////////////////////////
// dis_highscores.h by Victor Dods, created 2006/03/19
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_HIGHSCORES_H_)
#define _DIS_HIGHSCORES_H_

#include "xrb.h"

#include <set>
#include <string>
#include <time.h>

using namespace Xrb;

namespace Dis
{

class Score
{
public:

    Score () { }
    Score (
        std::string const &name,
        Uint32 points,
        Uint32 wave_count,
        time_t date)
        :
        m_name(name),
        m_points(points),
        m_wave_count(wave_count),
        m_date(date)
    { }

    inline std::string const &GetName () const { return m_name; }
    inline Uint32 GetPoints () const { return m_points; }
    inline Uint32 GetWaveCount () const { return m_wave_count; }
    inline time_t GetDate () const { return m_date; }

private:

    std::string m_name;
    Uint32 m_points;
    Uint32 m_wave_count;
    time_t m_date;
}; // end of class Score

// orders by points, then wave count, then time-alive, and then date (earlier is better)
struct ScoreOrderByPoints
{
    bool operator () (Score const &s0, Score const &s1) const
    {
        return s0.GetPoints() > s1.GetPoints()
               ||
               s0.GetPoints() == s1.GetPoints() && s0.GetWaveCount() > s1.GetWaveCount()
               ||
               s0.GetWaveCount() == s1.GetWaveCount() && s0.GetDate() < s1.GetDate();
    }
}; // end of struct ScoreOrderByPoints

// orders by wave count, then points, then time-alive, and then date (earlier is better)
struct ScoreOrderByWaveCount
{
    bool operator () (Score const &s0, Score const &s1) const
    {
        return s0.GetWaveCount() > s1.GetWaveCount()
               ||
               s0.GetWaveCount() == s1.GetWaveCount() && s0.GetPoints() > s1.GetPoints()
               ||
               s0.GetPoints() == s1.GetPoints() && s0.GetDate() < s1.GetDate();
    }
}; // end of struct ScoreOrderByWaveCount

class HighScores
{
public:

    enum
    {
        MAX_HIGH_SCORES = 10
    };

    HighScores ();
    HighScores (HighScores const &high_scores);
    ~HighScores ();

    void operator = (HighScores const &high_scores);

    bool GetIsNewHighScore (Score const &score);
    Score const &GetBestPointsScore (Uint32 index) const;
    Score const &GetBestWaveCountScore (Uint32 index) const;

    void AddScore (Score const &score);

    // TEMP
    void Print (FILE *fptr) const;

private:

    typedef std::set<Score, ScoreOrderByPoints> BestPointsScoreSet;
    typedef BestPointsScoreSet::const_iterator BestPointsScoreSetConstIterator;

    typedef std::set<Score, ScoreOrderByWaveCount> BestWaveCountScoreSet;
    typedef BestWaveCountScoreSet::const_iterator BestWaveCountScoreSetConstIterator;

    BestPointsScoreSet m_best_points_score_set;
    BestWaveCountScoreSet m_best_wave_count_score_set;
}; // end of class HighScores

} // end of namespace Dis

#endif // !defined(_DIS_HIGHSCORES_H_)

