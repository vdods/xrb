// ///////////////////////////////////////////////////////////////////////////
// dis_linetracebinding.h by Victor Dods, created 2005/11/26
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_LINETRACEBINDING_H_)
#define _DIS_LINETRACEBINDING_H_

#include "xrb.h"

#include <set>

using namespace Xrb;

namespace Dis
{

class GameObject;

struct LineTraceBinding
{
    Float m_time;
    GameObject *m_game_object;

    inline LineTraceBinding (Float const time, GameObject *game_object)
    {
        m_time = time;
        m_game_object = game_object;
    }
}; // end of struct LineTraceBinding

struct OrderLineTraceBindingsByTime
{
    inline bool operator () (
        LineTraceBinding const &binding0,
        LineTraceBinding const &binding1)
    {
        ASSERT1(binding0.m_game_object != binding1.m_game_object)
        return binding0.m_time < binding1.m_time;        
    }
}; // end of struct OrderEntitiesByTraceTime

typedef std::set<LineTraceBinding, OrderLineTraceBindingsByTime> LineTraceBindingSet;
typedef LineTraceBindingSet::iterator LineTraceBindingSetIterator;

} // end of namespace Dis

#endif // !defined(_DIS_LINETRACEBINDING_H_)

