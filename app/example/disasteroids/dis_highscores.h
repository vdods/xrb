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

    Score (std::string const &name, Uint32 points, Float time_alive, time_t date)
        :
        m_name(name),
        m_points(points),
        m_time_alive(time_alive),
        m_date(date)
    { }

    inline std::string const &GetName () const { return m_name; }
    inline Uint32 GetPoints () const { return m_points; }
    inline Float GetTimeAlive () const { return m_time_alive; }
    inline time_t GetDate () const { return m_date; }

private:

    std::string const m_name;
    Uint32 const m_points;
    Float const m_time_alive;
    time_t const m_date;
}; // end of class Score

// orders by points, then time-alive, and then date (earlier is better)
struct ScoreOrderByPoints
{
    bool operator () (Score const &s0, Score const &s1)
    {
        if (s0.GetPoints() > s1.GetPoints())
            return true;
        else if (s0.GetPoints() == s1.GetPoints())
        {
            if (s0.GetTimeAlive() > s1.GetTimeAlive())
                return true;
            else if (s0.GetTimeAlive() == s1.GetTimeAlive())
                return s0.GetDate() < s1.GetDate();
            else
                return false;
        }
        else
            return false;
    }
}; // end of struct ScoreOrderByPoints

// orders by time-alive, then points, and then date (earlier is better)
struct ScoreOrderByTimeAlive
{
    bool operator () (Score const &s0, Score const &s1)
    {
        if (s0.GetTimeAlive() > s1.GetTimeAlive())
            return true;
        else if (s0.GetTimeAlive() == s1.GetTimeAlive())
        {
            if (s0.GetPoints() > s1.GetPoints())
                return true;
            else if (s0.GetPoints() == s1.GetPoints())
                return s0.GetDate() < s1.GetDate();
            else
                return false;
        }
        else
            return false;
    }
}; // end of struct ScoreOrderByTimeAlive

class HighScores
{
public:

    enum
    {
        MAX_HIGH_SCORES = 10
    };

    HighScores ();
    ~HighScores ();

    bool GetIsNewHighScore (Score const &score);

    void AddScore (Score const &score);

    // TEMP
    void Print (FILE *fptr) const;
    
private:

    typedef std::set<Score, ScoreOrderByPoints> BestPointsScoreSet;
    typedef BestPointsScoreSet::const_iterator BestPointsScoreSetConstIterator;

    typedef std::set<Score, ScoreOrderByTimeAlive> BestTimeAliveScoreSet;
    typedef BestTimeAliveScoreSet::const_iterator BestTimeAliveScoreSetConstIterator;

    BestPointsScoreSet m_best_points_score_set;
    BestTimeAliveScoreSet m_best_time_alive_score_set;
}; // end of class HighScores

} // end of namespace Dis

#endif // !defined(_DIS_HIGHSCORES_H_)

