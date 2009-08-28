// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_circle_types.hpp by Victor Dods, created 2005/11/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_CIRCLE_TYPES_HPP_)
#define _XRB_ENGINE2_CIRCLE_TYPES_HPP_

#include "xrb.hpp"

#include <list>
#include <set>
#include <vector>

#include "xrb_vector.hpp"

namespace Xrb {
namespace Engine2 {
namespace Circle {

class Entity;

typedef std::list<Entity *> AreaTraceList;

// a typedef for a function which allows the game to specify exemptions
// to collisions (e.g. powerups and bullets, which just look stupid if
// they are allowed to collide and react with each other).
typedef bool (*CollisionExemptionFunction)(Entity const *, Entity const *);

// a typedef for specifying the maximum speed a particular entity may
// travel, a speed limit which is enforced by the PhysicsHandler.  in
// disasteroids, for example, CT_SOLID_COLLISION objects' max speed is
// 350, while other collision types' max speed is 800.
typedef Float (*MaxEntitySpeedFunction)(Entity const *);

struct CollisionPair
{
    Entity *m_entity0;
    Entity *m_entity1;
    FloatVector2 m_collision_location;
    // the collision normal points towards entity 0
    FloatVector2 m_collision_normal;
    Float m_collision_force;

    CollisionPair (
        Entity *entity0,
        Entity *entity1,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force)
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

/** For example, an intangible debris particle entity would likely use
  * CT_NO_COLLISION so that it does not effect other entities and
  * waste processing time.  A normal object such as a spaceship would have
  * CT_SOLID_COLLISION, and collide and bounce off of other entities
  * with CT_SOLID_COLLISION.  A fireball entity may use the value
  * CT_NONSOLID_COLLISION so that it still records collisions (which
  * in this case could damage the player's entity), but does not physically
  * bounce off of solid objects (the fireball would pass over the spaceship).
  * @brief Used by Engine2::Entity for indicating the collision properties
  *        of an entity subclass' geometry.
  */
enum CollisionType
{
    /// Indicates that the entity does not collide with others.
    CT_NO_COLLISION = 0,
    /// Indicates the entity records collisions and imparts collision forces.
    CT_SOLID_COLLISION,
    /// Indicates the entity records collisions without imparting collision forces.
    CT_NONSOLID_COLLISION,

    /// Number of collision types
    CT_COUNT
}; // end of enum CollisionType

struct LineTraceBinding
{
    Float m_time;
    Entity *m_entity;

    LineTraceBinding (Float time, Entity *entity)
    {
        m_time = time;
        m_entity = entity;
    }
}; // end of struct LineTraceBinding

struct OrderLineTraceBindingsByTime
{
    bool operator () (
        LineTraceBinding const &binding0,
        LineTraceBinding const &binding1)
    {
        ASSERT1(binding0.m_entity != binding1.m_entity);
        return binding0.m_time < binding1.m_time;
    }
}; // end of struct OrderEntitiesByTraceTime

typedef std::set<LineTraceBinding, OrderLineTraceBindingsByTime> LineTraceBindingSet;

} // end of namespace Circle
} // end of namespace Engine2
} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_CIRCLE_TYPES_HPP_)

