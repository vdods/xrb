// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_visibilityquadtree.hpp by Victor Dods, created 2004/06/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_VISIBILITYQUADTREE_HPP_)
#define _XRB_ENGINE2_VISIBILITYQUADTREE_HPP_

#include "xrb.hpp"

#include <set>

#include "xrb_color.hpp"
#include "xrb_engine2_quadtree.hpp"
#include "xrb_engine2_types.hpp"
#include "xrb_math.hpp"
#include "xrb_matrix2.hpp"

namespace Xrb {

class RenderContext;
class Screen;
class Serializer;

namespace Engine2 {

// The VisibilityQuadTree class implements a 2D space-organizing structure
// known as a quad tree.  The idea is to improve visibility/collision by
// eliminating a large number of drawing candidates quickly.  It should be
// noted that it uses circle-intersections to determine containment, which
// will give a boost in speed due to the ease of calculating radiuses (as
// opposed to messier rotatable rectangle intersection calculations).
class VisibilityQuadTree : public QuadTree
{
public:

    VisibilityQuadTree (
        FloatVector2 const &center,
        Float half_side_length,
        Uint8 depth);
    virtual ~VisibilityQuadTree () { }

    static VisibilityQuadTree *Create (Serializer &serializer);

    virtual void ReadStructure (Serializer &serializer);
    void WriteStructure (Serializer &serializer) const;
    virtual void ReadObjects (
        Serializer &serializer,
        ObjectLayer *object_layer);
    Uint32 WriteObjects (Serializer &serializer) const;

    Uint32 Draw (
        RenderContext const &render_context,
        FloatMatrix2 const &world_to_screen,
        Float pixels_in_view_radius,
        FloatVector2 const &view_center,
        Float view_radius,
        ObjectVector *object_collection_vector);
    Uint32 DrawWrapped (
        RenderContext const &render_context,
        FloatMatrix2 const &world_to_screen,
        Float pixels_in_view_radius,
        FloatVector2 const &view_center,
        Float view_radius,
        ObjectVector *object_collection_vector);

    // draw lines where the bounds of this quadtree node are
    void DrawBounds (
        RenderContext const &render_context,
        Color const &color);
    // draw the entire quadtree's bounding lines
    void DrawTreeBounds (
        RenderContext const &render_context,
        Color const &color);

protected:

    // for use in Create
    VisibilityQuadTree (VisibilityQuadTree *parent) : QuadTree(parent) { }

private:

    void Draw (Object::DrawLoopFunctor const &draw_data);
    void DrawWrapped (Object::DrawLoopFunctor draw_data);
}; // end of class Engine2::VisibilityQuadTree

} // end of namespace Engine2
} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_VISIBILITYQUADTREE_HPP_)
