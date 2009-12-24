// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_enums.hpp by Victor Dods, created 2005/11/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_ENUMS_HPP_)
#define _XRB_ENGINE2_ENUMS_HPP_

#include "xrb.hpp"

namespace Xrb {
namespace Engine2 {

/** @brief Denotes all possible types of Object (subclasses).
  */
enum ObjectType
{
    OT_OBJECT = 0,
    OT_SPRITE,
    OT_ANIMATED_SPRITE,
    OT_COMPOUND,

    OT_COUNT
}; // end of enum Engine2::ObjectType

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
    QTT_COUNT
}; // end of enum Engine2::QuadTreeType

} // end of namespace Engine2
} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_ENUMS_HPP_)

