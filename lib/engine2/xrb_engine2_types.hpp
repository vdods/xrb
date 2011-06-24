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

#include "xrb_color.hpp"
#include "xrb_vector.hpp"

namespace Xrb {

class RenderContext;
class Serializer;

namespace Engine2 {

class Entity;
class Object;
class ObjectLayer;

typedef Uint32 EntityWorldIndex;
typedef Entity *(*CreateEntityFunction)(Serializer &);

enum
{
    ENTITY_IS_NOT_IN_WORLD = static_cast<EntityWorldIndex>(-1)
};

// used for storing objects (with data relevant to sorting) for DrawObjectCollector
struct DrawObject
{
    Object const *m_object;
    Uint32 m_color_bias_rgba; // color bias munged through the render context, in 32 bit RGBA format
    Uint32 m_color_mask_rgba; // color mask munged through the render context, with distance fade applied, in 32 bit RGBA format

    DrawObject (Object const *object, Uint32 color_bias_rgba, Uint32 color_mask_rgba)
        :
        m_object(object),
        m_color_bias_rgba(color_bias_rgba),
        m_color_mask_rgba(color_mask_rgba)
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
    RenderContext const *m_render_context;
    Float m_pixels_in_view_radius;
    FloatVector2 m_view_center;
    Float m_view_radius;
    ObjectLayer const *m_object_layer;

    DrawObjectCollector ();

    // this is the functorial method which collects vectors from quadtree nodes
    void operator () (Object const *object);
}; // end of struct Engine2::DrawObjectCollector

} // end of namespace Engine2
} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_TYPES_HPP_)

