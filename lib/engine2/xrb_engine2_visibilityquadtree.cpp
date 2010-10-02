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

    fprintf(stderr, "VisibilityQuadTree::ReadObjects();\n");
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

// it's faster to not use depth test
#define USE_DEPTH_TEST 0

Uint32 VisibilityQuadTree::Draw (
    RenderContext const &render_context,
    FloatMatrix2 const &world_to_screen,
    Float const pixels_in_view_radius,
    FloatVector2 const &view_center,
    Float const view_radius,
    ObjectVector *const object_collection_vector)
{
    ASSERT1(object_collection_vector != NULL);
    ASSERT1(m_parent == NULL);

    // clear the object collection vector and create the draw loop functor
    object_collection_vector->clear();
    Object::DrawLoopFunctor
        draw_loop_functor(
            render_context,
            world_to_screen,
            pixels_in_view_radius,
            view_center,
            view_radius,
            true,
            object_collection_vector,
            GetQuadTreeType());

    // collect objects to draw
    {
#if USE_DEPTH_TEST
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glClearDepth(1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
        // TODO: look into glPolygonOffset, as this might save having
        // to clear the depth buffer for each ObjectLayer
        // TODO: use glDepthRange to set the per-ObjectLayer depth range
#endif // USE_DEPTH_TEST

        Draw(draw_loop_functor);
    }

    // sort objects back-to-front and draw them
    {
#if USE_DEPTH_TEST
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
#endif // USE_DEPTH_TEST

        if (!object_collection_vector->empty())
            std::sort(
                &(*object_collection_vector)[0],
                &(*object_collection_vector)[0]+object_collection_vector->size(),
                Object::ObjectDepthOrder());

        draw_loop_functor.SetIsObjectCollectionPass(false);
        std::for_each(
            draw_loop_functor.GetObjectCollectionVector()->begin(),
            draw_loop_functor.GetObjectCollectionVector()->end(),
            draw_loop_functor);
    }

    object_collection_vector->clear();

    return draw_loop_functor.DrawnObjectCount();
}

Uint32 VisibilityQuadTree::DrawWrapped (
    RenderContext const &render_context,
    FloatMatrix2 const &world_to_screen,
    Float const pixels_in_view_radius,
    FloatVector2 const &view_center,
    Float const view_radius,
    ObjectVector *const object_collection_vector)
{
    ASSERT1(object_collection_vector != NULL);
    ASSERT1(m_parent == NULL);

    // clear the object collection vector and create the draw loop functor
    object_collection_vector->clear();
    Object::DrawLoopFunctor
        draw_loop_functor(
            render_context,
            world_to_screen,
            pixels_in_view_radius,
            view_center,
            view_radius,
            true,
            object_collection_vector,
            GetQuadTreeType());

    DrawWrapped(draw_loop_functor);
    return draw_loop_functor.DrawnObjectCount();
}

void VisibilityQuadTree::DrawBounds (
    RenderContext const &render_context,
    Color const &color)
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

void VisibilityQuadTree::DrawTreeBounds (
    RenderContext const &render_context,
    Color const &color)
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

void VisibilityQuadTree::Draw (Object::DrawLoopFunctor const &draw_loop_functor)
{
    // if there are no objects here or below, just return
    if (SubordinateObjectCount() == 0)
        return;

    ASSERT2(draw_loop_functor.PixelsInViewRadius() > 0.0f);
    ASSERT2(draw_loop_functor.ViewRadius() > 0.0f);

    // return if the view doesn't intersect this node
    if (!DoesAreaOverlapQuadBounds(draw_loop_functor.ViewCenter(), draw_loop_functor.ViewRadius()))
        return;

    // don't draw quadtrees whose radii are lower than the
    // gs_radius_limit_lower threshold -- a form of distance culling,
    // which gives a huge speedup and allows zooming to any level
    // maintain a consistent framerate.
    if (draw_loop_functor.PixelsInViewRadius() * Radius()
        <
        draw_loop_functor.ViewRadius() * Object::DrawLoopFunctor::ms_radius_limit_lower)
    {
        return;
    }

//     DrawBounds(draw_loop_functor.ObjectDrawData().GetRenderContext(), Color(1.0, 1.0, 0.0, 1.0));

    std::for_each(m_object_set.begin(), m_object_set.end(), draw_loop_functor);

    // if there are child nodes, call Draw on each
    if (HasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            Child<VisibilityQuadTree>(i)->Draw(draw_loop_functor);
}

void VisibilityQuadTree::DrawWrapped (Object::DrawLoopFunctor draw_loop_functor)
{
    // if there are no objects here or below, just return
    if (SubordinateObjectCount() == 0)
        return;

    ASSERT1(Parent() == NULL);
    ASSERT2(draw_loop_functor.PixelsInViewRadius() > 0.0f);
    ASSERT2(draw_loop_functor.ViewRadius() > 0.0f);
    ASSERT2(m_half_side_length > 0.0f);

    Float side_length = SideLength();
    Float radius_sum = 2.0f*Radius() + draw_loop_functor.ViewRadius();
    Float top = floor((draw_loop_functor.ViewCenter()[Dim::Y]+radius_sum)/side_length);
    Float bottom = ceil((draw_loop_functor.ViewCenter()[Dim::Y]-radius_sum)/side_length);
    Float left = ceil((draw_loop_functor.ViewCenter()[Dim::X]-radius_sum)/side_length);
    Float right = floor((draw_loop_functor.ViewCenter()[Dim::X]+radius_sum)/side_length);
    FloatVector2 old_view_center(draw_loop_functor.ViewCenter());
    FloatVector2 view_offset;
    ObjectVector &object_collection_vector = *draw_loop_functor.GetObjectCollectionVector();

    for (Float x = left; x <= right; x += 1.0f)
    {
        for (Float y = bottom; y <= top; y += 1.0f)
        {
            view_offset.SetComponents(side_length*x, side_length*y);
            // this IF statement culls quadtree nodes that are outside of the
            // circular view radius from the square grid of nodes to be drawn
            if ((old_view_center - view_offset).LengthSquared() < radius_sum*radius_sum)
            {
                draw_loop_functor.SetViewCenter(old_view_center - view_offset);

                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                glTranslatef(
                    view_offset[Dim::X],
                    view_offset[Dim::Y],
                    0.0f);

                // clear the object collection vector and collect objects to draw
                {
#if USE_DEPTH_TEST
                    glEnable(GL_DEPTH_TEST);
                    glDepthMask(GL_TRUE);
                    glClearDepth(1.0f);
                    glClear(GL_DEPTH_BUFFER_BIT);
                    // TODO: look into glPolygonOffset, as this might save having
                    // to clear the depth buffer for each ObjectLayer
                    // TODO: use glDepthRange to set the per-ObjectLayer depth range
#endif // USE_DEPTH_TEST

                    object_collection_vector.clear();
                    draw_loop_functor.SetIsObjectCollectionPass(true);
                    Draw(draw_loop_functor);
                }

                // sort objects back-to-front and draw them
                {
#if USE_DEPTH_TEST
                    glDisable(GL_DEPTH_TEST);
                    glDepthMask(GL_FALSE);
#endif // USE_DEPTH_TEST

                    if (!object_collection_vector.empty())
                    {
                        std::sort(
                            &object_collection_vector[0],
                            &object_collection_vector[0]+object_collection_vector.size(),
                            Object::ObjectDepthOrder());
                        /*
                        // remove duplicates from the object collection vector, which should be
                        // all contiguous because of the above sorting.
                        Uint32 write = 0;
                        Uint32 read = 0;
                        Uint32 size = object_collection_vector.size();
                        while (read < size)
                        {
                            Object const *skip_object = object_collection_vector[read];
                            object_collection_vector[write] = skip_object;
                            ++write;
                            ++read;
                            while (read < size && object_collection_vector[read] == skip_object)
                                ++read;
                        }
                        object_collection_vector.resize(write);
                        */
                    }

                    draw_loop_functor.SetIsObjectCollectionPass(false);
                    std::for_each(
                        object_collection_vector.begin(),
                        object_collection_vector.end(),
                        draw_loop_functor);
                }
            }
        }
    }

    object_collection_vector.clear();
}

} // end of namespace Engine2
} // end of namespace Xrb
