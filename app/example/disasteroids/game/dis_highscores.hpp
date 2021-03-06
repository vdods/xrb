// ///////////////////////////////////////////////////////////////////////////
// dis_highscores.hpp by Victor Dods, created 2006/03/19
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_HIGHSCORES_HPP_)
#define _DIS_HIGHSCORES_HPP_

#include "xrb.hpp"

#include <list>
#include <set>
#include <string>
#include <time.h>

using namespace Xrb;

namespace Dis {

class Score
{
public:

    Score () { }
    Score (std::string const &name, Uint32 points, Uint32 wave_count, time_t date)
        :
        m_name(name),
        m_points(points),
        m_wave_count(wave_count),
        m_date(date),
        m_hash(ComputeHash(name, points, wave_count, date))
    { }

    std::string const &Name () const { return m_name; }
    Uint32 Points () const { return m_points; }
    Uint32 WaveCount () const { return m_wave_count; }
    time_t Date () const { return m_date; }
    Uint32 Hash () const { return m_hash; }

private:

    static Uint32 ComputeHash (std::string const &name, Uint32 points, Uint32 wave_count, time_t date);

    std::string m_name;
    Uint32 m_points;
    Uint32 m_wave_count;
    time_t m_date;
    Uint32 m_hash;
}; // end of class Score

// orders by points, then wave count, then time-alive, and then date (earlier is better)
struct ScoreOrderByPoints
{
    bool operator () (Score const &s0, Score const &s1) const
    {
        return s0.Points() > s1.Points()
               ||
               (s0.Points() == s1.Points() && s0.WaveCount() > s1.WaveCount())
               ||
               (s0.WaveCount() == s1.WaveCount() && s0.Date() < s1.Date());
    }
}; // end of struct ScoreOrderByPoints

// orders by wave count, then points, then time-alive, and then date (earlier is better)
struct ScoreOrderByWaveCount
{
    bool operator () (Score const &s0, Score const &s1) const
    {
        return s0.WaveCount() > s1.WaveCount()
               ||
               (s0.WaveCount() == s1.WaveCount() && s0.Points() > s1.Points())
               ||
               (s0.Points() == s1.Points() && s0.Date() < s1.Date());
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

    bool IsNewHighScore (Score const &score);
    Score const &BestPointsScore (Uint32 index) const;
    Score const &BestWaveCountScore (Uint32 index) const;

    // the return value is true iff the points rank was
    // equal to or better than the wave count rank
    bool AddScore (Score const &score);

    void Read (std::string const &path);
    void Write (std::string const &path);

private:

    typedef std::set<Score, ScoreOrderByPoints> BestPointsScoreSet;
    typedef std::set<Score, ScoreOrderByWaveCount> BestWaveCountScoreSet;
    typedef std::list<Score> ScoreList;

    BestPointsScoreSet m_best_points_score_set;
    BestWaveCountScoreSet m_best_wave_count_score_set;
    ScoreList m_score_list;
}; // end of class HighScores

} // end of namespace Dis

#endif // !defined(_DIS_HIGHSCORES_HPP_)

