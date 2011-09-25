// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_visibilityquadtree.cpp by Victor Dods, created 2004/06/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_visibilityquadtree.hpp"

#include <algorithm>
#include <cstdlib>

#include "xrb_engine2_entity.hpp"
#include "xrb_engine2_objectlayer.hpp"
#include "xrb_gl.hpp"
#include "xrb_render.hpp"
#include "xrb_rendercontext.hpp"
#include "xrb_serializer.hpp"

namespace Xrb {
namespace Engine2 {

// ///////////////////////////////////////////////////////////////////////////
// VisibilityQuadTree
// ///////////////////////////////////////////////////////////////////////////

VisibilityQuadTree::VisibilityQuadTree (
    FloatVector2 const &center,
    Float const half_side_length,
    Uint8 const depth)
    :
    QuadTree(NULL)
{
    Initialize<VisibilityQuadTree>(center, half_side_length, depth);
    SetQuadTreeType(QTT_VISIBILITY);
}

VisibilityQuadTree *VisibilityQuadTree::Create (Serializer &serializer)
{
    VisibilityQuadTree *retval = new VisibilityQuadTree(NULL);

    retval->ReadStructure(serializer);
    retval->SetQuadTreeType(QTT_VISIBILITY);
    // the objects are left to be read once this new quadtree
    // is returned to the objectlayer

    return retval;
}

void VisibilityQuadTree::ReadStructure (Serializer &serializer)
{
    // write the VisibilityQuadTree's structure info
    serializer.ReadAggregate<FloatVector2>(m_center);
    serializer.Read<Float>(m_half_side_length);
    serializer.Read<Float>(m_radius);
    bool has_children = serializer.Read<bool>();
    // if there are children, recursively call this function on them
    if (has_children)
    {
        for (Uint8 i = 0; i < 4; ++i)
        {
            ASSERT1(m_child[i] == NULL);
            m_child[i] = new VisibilityQuadTree(this);
            Child<VisibilityQuadTree>(i)->ReadStructure(serializer);
        }
    }
}

void VisibilityQuadTree::WriteStructure (Serializer &serializer) const
{
    // write the VisibilityQuadTree's structure info
    serializer.WriteAggregate<FloatVector2>(m_center);
    serializer.Write<Float>(m_half_side_length);
    serializer.Write<Float>(m_radius);
    serializer.Write<bool>(HasChildren());
    // if there are children, recursively call this function on them
    if (HasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            Child<VisibilityQuadTree>(i)->WriteStructure(serializer);
}

void VisibilityQuadTree::ReadObjects (
    Serializer &serializer,
    ObjectLayer *const object_layer)
{
    ASSERT1(object_layer != NULL);

    std::cerr << "VisibilityQuadTree::ReadObjects();" << std::endl;
    Uint32 static_object_count;
    serializer.Read<Uint32>(static_object_count);
    while (static_object_count > 0)
    {
        // it's ok to pass NULL as CreateEntity because it won't be used,
        // because these are all static objects (or at least better be)
        Object *object = Object::Create(serializer, NULL);
        ASSERT1(!object->IsDynamic());
        object_layer->AddObject(object);
        --static_object_count;
    }
}

Uint32 VisibilityQuadTree::WriteObjects (Serializer &serializer) const
{
    // if this is the top level node, write out the number of
    // subordinate non-entities
    if (Parent() == NULL)
        serializer.Write<Uint32>(SubordinateStaticObjectCount());

    // the number of non-entities written
    Uint32 retval = 0;

    // write out the non-entities that this quad node contains
    for (ObjectSet::const_iterator it = m_object_set.begin(),
                                   it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Object const *object = *it;
        ASSERT1(object != NULL);
        if (!object->IsDynamic())
        {
            object->Write(serializer);
            ++retval;
        }
    }

    // if there are children, recursively call this function on them
    if (HasChildren())
    {
        for (Uint8 i = 0; i < 4; ++i)
        {
            ASSERT1(m_child[i]->Parent() == this);
            retval += Child<VisibilityQuadTree>(i)->WriteObjects(serializer);
        }
    }

    return retval;
}

Uint32 VisibilityQuadTree::Draw (
    RenderContext const &render_context,
    FloatMatrix2 const &world_to_screen,
    DrawObjectCollector &draw_object_collector,
    ObjectLayer const &object_layer) const
{
    ASSERT1(m_parent == NULL && "this can only be called on the root node");
    ASSERT1(draw_object_collector.m_object_layer != NULL);

    if (SubordinateObjectCount() == 0)
        return 0;

    // clear the collection vector and collect objects to draw
    draw_object_collector.m_draw_object.clear();
    CollectDrawObjects(draw_object_collector);
    // sort objects (back-to-front, and then by other criteria)
//     std::sort(draw_object_collector.m_draw_object.begin(), draw_object_collector.m_draw_object.end(), DrawObjectOrder());
    // NOTE: qsort is being used because std::sort was handing me bad pointers.
    std::qsort(&draw_object_collector.m_draw_object[0], draw_object_collector.m_draw_object.size(), sizeof(DrawObject), DrawObject::Compare);

    // now draw them
    Object::DrawData draw_data(render_context, world_to_screen);
    for (DrawObjectVector::const_iterator it = draw_object_collector.m_draw_object.begin(),
                                          it_end = draw_object_collector.m_draw_object.end();
         it != it_end;
         ++it)
    {
        DrawObject const &draw_object = *it;
        ASSERT3(draw_object.m_object != NULL);
        draw_data.m_color_bias = Color(draw_object.m_color_bias_rgba);
        draw_data.m_color_mask = Color(draw_object.m_color_mask_rgba);
        draw_object.m_object->Draw(draw_data);
    }
    Uint32 drawn_object_count = draw_object_collector.m_draw_object.size();
    draw_object_collector.m_draw_object.clear(); // might as well clear the vector again
    return drawn_object_count;
}

Uint32 VisibilityQuadTree::DrawWrapped (
    RenderContext const &render_context,
    FloatMatrix2 const &world_to_screen,
    DrawObjectCollector &draw_object_collector,
    ObjectLayer const &object_layer) const
{
    ASSERT1(m_parent == NULL && "this can only be called on the root node");

    Uint32 drawn_object_count = 0;

    // if there are no objects, just return
    if (SubordinateObjectCount() == 0)
        return drawn_object_count;

    ASSERT2(m_half_side_length > 0.0f);

    Float side_length = SideLength();
    Float radius_sum = 2.0f*Radius() + draw_object_collector.m_view_radius;
    Float top = floor((draw_object_collector.m_view_center[Dim::Y]+radius_sum)/side_length);
    Float bottom = ceil((draw_object_collector.m_view_center[Dim::Y]-radius_sum)/side_length);
    Float left = ceil((draw_object_collector.m_view_center[Dim::X]-radius_sum)/side_length);
    Float right = floor((draw_object_collector.m_view_center[Dim::X]+radius_sum)/side_length);
    FloatVector2 old_view_center(draw_object_collector.m_view_center);
    FloatVector2 view_offset;

    for (Float x = left; x <= right; x += 1.0f)
    {
        for (Float y = bottom; y <= top; y += 1.0f)
        {
            view_offset.SetComponents(side_length*x, side_length*y);
            // this IF statement culls quadtree nodes that are outside of the
            // circular view radius from the square grid of nodes to be drawn
            if ((old_view_center - view_offset).LengthSquared() < Sqr(radius_sum))
            {
                draw_object_collector.m_view_center = old_view_center - view_offset;

                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                glTranslatef(
                    view_offset[Dim::X],
                    view_offset[Dim::Y],
                    0.0f);

                // call the non-wrapped Draw.  NOTE: the world_to_screen transformation here is wrong
                // by a translation of view_offset.
                drawn_object_count += Draw(render_context, world_to_screen, draw_object_collector, object_layer);
            }
        }
    }

    return drawn_object_count;
}

void VisibilityQuadTree::DrawBounds (RenderContext const &render_context, Color const &color) const
{
    Float r_o2 = m_radius * 0.5f * Math::Sqrt(2.0f);
    FloatVector2 upper_right( r_o2,  r_o2);
    FloatVector2 upper_left (-r_o2,  r_o2);
    FloatVector2 lower_left (-r_o2, -r_o2);
    FloatVector2 lower_right( r_o2, -r_o2);
    upper_right += m_center;
    upper_left  += m_center;
    lower_left  += m_center;
    lower_right += m_center;
    Render::DrawLine(render_context, upper_right, upper_left,  color);
    Render::DrawLine(render_context, upper_left,  lower_left,  color);
    Render::DrawLine(render_context, lower_left,  lower_right, color);
    Render::DrawLine(render_context, lower_right, upper_right, color);
}

void VisibilityQuadTree::DrawTreeBounds (RenderContext const &render_context, Color const &color) const
{
    if (m_parent == NULL)
        DrawBounds(render_context, color);

    Float r_o2 = m_radius * 0.5f * Math::Sqrt(2.0f);
    FloatVector2 top   (  0.0,  r_o2);
    FloatVector2 bottom(  0.0, -r_o2);
    FloatVector2 left  (-r_o2,   0.0);
    FloatVector2 right ( r_o2,   0.0);
    top    += m_center;
    bottom += m_center;
    left   += m_center;
    right  += m_center;

    Render::DrawLine(render_context, top, bottom, color);
    Render::DrawLine(render_context, left, right, color);

    if (HasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            Child<VisibilityQuadTree>(i)->DrawTreeBounds(render_context, color);
}

void VisibilityQuadTree::CollectDrawObjects (DrawObjectCollector &draw_object_collector) const
{
    // if there are no objects here or below, just return
    if (SubordinateObjectCount() == 0)
        return;

    // return if the view doesn't intersect this node
    ASSERT3(draw_object_collector.m_object_layer != NULL);
    // disable the wrapping adjustment (i.e. calls to ObjectLayer::AdjustedDifference and friends),
    // so that drawing will work properly.
    if (!DoesAreaOverlapQuadBounds(draw_object_collector.m_view_center, draw_object_collector.m_view_radius, *draw_object_collector.m_object_layer, true))
        return;

    // don't draw quadtrees whose radii are lower than the
    // gs_radius_limit_lower threshold -- a form of distance culling,
    // which gives a huge speedup and allows zooming to any level
    // maintain a consistent framerate.
    if (draw_object_collector.m_pixels_in_view_radius * Radius()
        <
        draw_object_collector.m_view_radius * Object::ms_radius_limit_lower)
    {
        return;
    }

//     DrawBounds(draw_loop_functor.GetRenderContext(), Color(1.0, 1.0, 0.0, 1.0));

    // it is necessary to specify the type 'DrawObjectCollector &' here specifically
    // as a reference, because by default std::for_each will accept draw_object_collector
    // by value, and consequently the additions to its m_draw_object vector will
    // be lost when that locally scoped object is destroyed at the end of std::for_each.
    // we want 'the' instance of draw_object_collector (not a copy) to be used on each
    // element of the set.
    std::for_each<ObjectSet::const_iterator, DrawObjectCollector &>(m_object_set.begin(), m_object_set.end(), draw_object_collector);

    // if there are child nodes, call Draw on each
    if (HasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            Child<VisibilityQuadTree>(i)->CollectDrawObjects(draw_object_collector);
}

} // end of namespace Engine2
} // end of namespace Xrb
