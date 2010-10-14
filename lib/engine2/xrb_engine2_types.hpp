// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_types.hpp by Victor Dods, created 2006/04/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_TYPES_HPP_)
#define _XRB_ENGINE2_TYPES_HPP_

#include "xrb.hpp"

#include <vector>

#include "xrb_vector.hpp"

namespace Xrb {

class Serializer;

namespace Engine2 {

class Entity;
class Object;

typedef Uint32 EntityWorldIndex;
typedef Entity *(*CreateEntityFunction)(Serializer &);

enum
{
    ENTITY_IS_NOT_IN_WORLD = static_cast<EntityWorldIndex>(-1)
};

// used for storing objects (and distance fade alpha values) for DrawObjectCollector
// TODO: precompute render context related stuff like blended color bias and mask
struct DrawObject
{
    Object const *m_object;
    Float m_distance_fade;
    // add color bias and mask so totally accurate ordering can be done

    DrawObject (Object const *object, Float distance_fade)
        :
        m_object(object),
        m_distance_fade(distance_fade)
    {
        ASSERT1(m_object != NULL);
    }
}; // end of struct Engine2::DrawObject

// used to put a partial order on objects of type DrawObject.
struct DrawObjectOrder
{
    bool operator () (DrawObject const &l, DrawObject const &r);
}; // end of struct Engine2::DrawObjectOrder

typedef std::vector<DrawObject> DrawObjectVector;

// used for collecting objects-to-draw, sorting them for z-depth order
// and other considerations to allow draw operations to be batched together.
struct DrawObjectCollector
{
    DrawObjectVector m_draw_object;
    Float m_pixels_in_view_radius;
    FloatVector2 m_view_center;
    Float m_view_radius;

    DrawObjectCollector ();

    // this is the functorial method which collects vectors from quadtree nodes
    void operator () (Object const *object);
}; // end of struct Engine2::DrawObjectCollector

} // end of namespace Engine2
} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_TYPES_HPP_)

