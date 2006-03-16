// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_enums.h by Victor Dods, created 2005/11/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_ENUMS_H_)
#define _XRB_ENGINE2_ENUMS_H_

#include "xrb.h"

namespace Xrb
{

namespace Engine2
{

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
        CT_NONSOLID_COLLISION
    }; // end of enum Engine2::CollisionType
    
    /** @brief Enumerates the different types of quadtree owners which can be
      * stored in the owner quad tree array in Engine2::Object.
      */
    enum QuadTreeType
    {
        /// Indicates the quadtree in Engine2::ObjectLayer, used for visibility culling.
        QTT_VISIBILITY = 0,
        /// Indicates a quadtree that may be used in PhysicsHandler implementations.
        QTT_PHYSICS_HANDLER,
    
        /// Gives the total number of quad tree types.
        QTT_NUM_TYPES
    }; // end of enum Engine2::QuadTreeType

} // end of namespace Engine2

} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_ENUMS_H_)

