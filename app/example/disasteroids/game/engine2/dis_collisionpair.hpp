// ///////////////////////////////////////////////////////////////////////////
// dis_collisionpair.hpp by Victor Dods, created 2005/11/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_COLLISIONPAIR_HPP_)
#define _DIS_COLLISIONPAIR_HPP_

#include "xrb_vector.hpp"

#include <vector>

using namespace Xrb;

namespace Dis
{

class Entity;

struct CollisionPair
{
    Entity *m_entity0;
    Entity *m_entity1;
    FloatVector2 m_collision_location;
    // the collision normal points towards entity 0
    FloatVector2 m_collision_normal;
    Float m_collision_force;

    inline CollisionPair (
        Entity *const entity0,
        Entity *const entity1,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float const collision_force)
        :
        m_entity0(entity0),
        m_entity1(entity1),
        m_collision_location(collision_location),
        m_collision_normal(collision_normal),
        m_collision_force(collision_force)
    {
        ASSERT2(m_entity0 != NULL);
        ASSERT2(m_entity1 != NULL);
        ASSERT2(Epsilon(m_collision_normal.Length(), 1.0f, 0.0001f));
    }
}; // end of struct CollisionPair

typedef std::vector<CollisionPair> CollisionPairList;
typedef CollisionPairList::iterator CollisionPairListIterator;

} // end of namespace Dis

#endif // !defined(_DIS_COLLISIONPAIR_HPP_)

