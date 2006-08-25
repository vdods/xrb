// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_visibilityquadtree.cpp by Victor Dods, created 2004/06/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_visibilityquadtree.h"

#include <algorithm>

#include "xrb_engine2_entity.h"
#include "xrb_engine2_objectlayer.h"
#include "xrb_render.h"
#include "xrb_rendercontext.h"
#include "xrb_serializer.h"

namespace Xrb
{

// ///////////////////////////////////////////////////////////////////////////
// Engine2::VisibilityQuadTree
// ///////////////////////////////////////////////////////////////////////////

Engine2::VisibilityQuadTree::VisibilityQuadTree (
    FloatVector2 const &center,
    Float const half_side_length,
    Uint8 const depth)
    :
    QuadTree(NULL)
{
    Initialize<VisibilityQuadTree>(center, half_side_length, depth);
    SetQuadTreeType(QTT_VISIBILITY);
}

Engine2::VisibilityQuadTree *Engine2::VisibilityQuadTree::Create (Serializer &serializer)
{
    VisibilityQuadTree *retval = new VisibilityQuadTree(NULL);

    retval->ReadStructure(serializer);
    retval->SetQuadTreeType(QTT_VISIBILITY);
    // the objects are left to be read once this new quadtree
    // is returned to the objectlayer

    return retval;
}

void Engine2::VisibilityQuadTree::ReadStructure (Serializer &serializer)
{
    // write the VisibilityQuadTree's structure info
    m_center = serializer.ReadFloatVector2();
    serializer.ReadFloat(&m_half_side_length);
    serializer.ReadFloat(&m_radius);
    bool has_children = serializer.ReadBool();
    // if there are children, recursively call this function on them
    if (has_children)
    {
        for (Uint8 i = 0; i < 4; ++i)
        {
            ASSERT1(m_child[i] == NULL)
            m_child[i] = new VisibilityQuadTree(this);
            GetChild<VisibilityQuadTree>(i)->ReadStructure(serializer);
        }
    }
}

void Engine2::VisibilityQuadTree::WriteStructure (Serializer &serializer) const
{
    // write the VisibilityQuadTree's structure info
    serializer.WriteFloatVector2(m_center);
    serializer.WriteFloat(m_half_side_length);
    serializer.WriteFloat(m_radius);
    serializer.WriteBool(GetHasChildren());
    // if there are children, recursively call this function on them
    if (GetHasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            GetChild<VisibilityQuadTree>(i)->WriteStructure(serializer);
}

void Engine2::VisibilityQuadTree::ReadObjects (
    Serializer &serializer,
    Engine2::ObjectLayer *const object_layer)
{
    ASSERT1(object_layer != NULL)

    fprintf(stderr, "Engine2::VisibilityQuadTree::ReadObjects();\n");
    Uint32 static_object_count = serializer.ReadUint32();
    while (static_object_count > 0)
    {
        // it's ok to pass NULL as CreateEntity because it won't be used,
        // because these are all static objects (or at least better be)
        Object *object = Object::Create(serializer, NULL);
        ASSERT1(!object->GetIsDynamic())
        object_layer->AddObject(object);
        --static_object_count;
    }
}

Uint32 Engine2::VisibilityQuadTree::WriteObjects (Serializer &serializer) const
{
    // if this is the top level node, write out the number of
    // subordinate non-entities
    if (GetParent() == NULL)
        serializer.WriteUint32(GetSubordinateStaticObjectCount());

    // the number of non-entities written
    Uint32 retval = 0;

    // write out the non-entities that this quad node contains
    for (ObjectSetConstIterator it = m_object_set.begin(),
                                it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Object const *object = *it;
        ASSERT1(object != NULL)
        if (!object->GetIsDynamic())
        {
            object->Write(serializer);
            ++retval;
        }
    }

    // if there are children, recursively call this function on them
    if (GetHasChildren())
    {
        for (Uint8 i = 0; i < 4; ++i)
        {
            ASSERT1(m_child[i]->GetParent() == this)
            retval += GetChild<VisibilityQuadTree>(i)->WriteObjects(serializer);
        }
    }

    return retval;
}

Uint32 Engine2::VisibilityQuadTree::Draw (
    RenderContext const &render_context,
    FloatMatrix2 const &world_to_screen,
    Float const pixels_in_view_radius,
    FloatVector2 const &view_center,
    Float const view_radius,
    TransparentObjectVector *const transparent_object_vector)
{
    ASSERT1(transparent_object_vector != NULL)
    ASSERT1(m_parent == NULL)

    // clear the transparent object vector and create the draw loop functor
    transparent_object_vector->clear();
    Object::DrawLoopFunctor
        draw_loop_functor(
            render_context,
            world_to_screen,
            pixels_in_view_radius,
            view_center,
            view_radius,
            true,
            transparent_object_vector,
            GetQuadTreeType());

    // draw opaque objects while collecting transparent objects
    {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glClearDepth(1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
        // TODO: look into glPolygonOffset, as this might save having
        // to clear the depth buffer for each ObjectLayer
        // TODO: use glDepthRange to set the per-ObjectLayer depth range

        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0f);

        Draw(draw_loop_functor);
    }

    // sort opaque objects back-to-front and draw them
    {
        glDepthMask(GL_FALSE);
        glDisable(GL_ALPHA_TEST);
        glAlphaFunc(GL_ALWAYS, 0.0f);
        std::sort(
            &(*transparent_object_vector)[0],
            &(*transparent_object_vector)[0]+transparent_object_vector->size(),
            Object::TransparentObjectOrder());

        draw_loop_functor.SetIsCollectTransparentObjectPass(false);
        std::for_each(
            draw_loop_functor.GetTransparentObjectVector()->begin(),
            draw_loop_functor.GetTransparentObjectVector()->end(),
            draw_loop_functor);
    }

    // restore the GL state
    glDisable(GL_DEPTH_TEST);

    return draw_loop_functor.GetDrawnObjectCount();
}

Uint32 Engine2::VisibilityQuadTree::DrawWrapped (
    RenderContext const &render_context,
    FloatMatrix2 const &world_to_screen,
    Float const pixels_in_view_radius,
    FloatVector2 const &view_center,
    Float const view_radius,
    TransparentObjectVector *const transparent_object_vector)
{
    ASSERT1(transparent_object_vector != NULL)
    ASSERT1(m_parent == NULL)

    // clear the transparent object vector and create the draw loop functor
    transparent_object_vector->clear();
    Object::DrawLoopFunctor
        draw_loop_functor(
            render_context,
            world_to_screen,
            pixels_in_view_radius,
            view_center,
            view_radius,
            true,
            transparent_object_vector,
            GetQuadTreeType());

    // draw opaque objects while collecting transparent objects
    {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glClearDepth(1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
        // TODO: look into glPolygonOffset, as this might save having
        // to clear the depth buffer for each ObjectLayer
        // TODO: use glDepthRange to set the per-ObjectLayer depth range
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0f);

        DrawWrapped(draw_loop_functor);
    }

    // sort opaque objects back-to-front and draw them
    {
        glDepthMask(GL_FALSE);
        glDisable(GL_ALPHA_TEST);
        glAlphaFunc(GL_ALWAYS, 0.0f);
        std::sort(
            &(*transparent_object_vector)[0],
            &(*transparent_object_vector)[0]+transparent_object_vector->size(),
            Object::TransparentObjectOrder());

        draw_loop_functor.SetIsCollectTransparentObjectPass(false);
        DrawWrapped(draw_loop_functor);
    }

    // restore the GL state
    glDisable(GL_DEPTH_TEST);

    return draw_loop_functor.GetDrawnObjectCount();
}

void Engine2::VisibilityQuadTree::DrawBounds (
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

void Engine2::VisibilityQuadTree::DrawTreeBounds (
    RenderContext const &render_context,
    Color const &color)
{
    if (!m_parent)
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

    if (GetHasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            GetChild<VisibilityQuadTree>(i)->DrawTreeBounds(render_context, color);
}

void Engine2::VisibilityQuadTree::Draw (
    Engine2::Object::DrawLoopFunctor const &draw_loop_functor)
{
    // if there are no objects here or below, just return
    if (GetSubordinateObjectCount() == 0)
        return;

    ASSERT2(draw_loop_functor.GetPixelsInViewRadius() > 0.0f)
    ASSERT2(draw_loop_functor.GetViewRadius() > 0.0f)

    // don't draw quadtrees whose radii are lower than the
    // gs_radius_limit_lower threshold -- a form of distance culling,
    // which gives a huge speedup and allows zooming to any level
    // maintain a consistent framerate.
    if (draw_loop_functor.GetPixelsInViewRadius() * GetRadius()
        <
        draw_loop_functor.GetViewRadius() * Object::DrawLoopFunctor::ms_radius_limit_lower)
    {
        return;
    }

    // return if the view doesn't intersect this node
    if (!GetDoesAreaOverlapQuadBounds(draw_loop_functor.GetViewCenter(), draw_loop_functor.GetViewRadius()))
        return;

//     DrawBounds(draw_loop_functor.GetObjectDrawData().GetRenderContext(), Color(1.0, 1.0, 0.0, 1.0));

    std::for_each(m_object_set.begin(), m_object_set.end(), draw_loop_functor);

    // if there are child nodes, call Draw on each
    if (GetHasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            GetChild<VisibilityQuadTree>(i)->Draw(draw_loop_functor);
}

void Engine2::VisibilityQuadTree::DrawWrapped (
    Engine2::Object::DrawLoopFunctor draw_loop_functor)
{
    // if there are no objects here or below, just return
    if (GetSubordinateObjectCount() == 0)
        return;

    ASSERT2(draw_loop_functor.GetPixelsInViewRadius() > 0.0f)
    ASSERT2(draw_loop_functor.GetViewRadius() > 0.0f)
    ASSERT2(m_half_side_length > 0.0f)

    Float side_length = GetSideLength();
    Float radius_sum = 2.0f*GetRadius() + draw_loop_functor.GetViewRadius();
    Float top = floor((draw_loop_functor.GetViewCenter().m[1]+radius_sum)/side_length);
    Float bottom = ceil((draw_loop_functor.GetViewCenter().m[1]-radius_sum)/side_length);
    Float left = ceil((draw_loop_functor.GetViewCenter().m[0]-radius_sum)/side_length);
    Float right = floor((draw_loop_functor.GetViewCenter().m[0]+radius_sum)/side_length);
    FloatVector2 old_view_center(draw_loop_functor.GetViewCenter());
    FloatVector2 view_offset;

    glMatrixMode(GL_MODELVIEW);

    for (Float x = left; x <= right; x += 1.0)
    {
        for (Float y = bottom; y <= top; y += 1.0)
        {
            view_offset.SetComponents(side_length*x, side_length*y);
            // this IF statement culls quadtree nodes that are outside of the
            // circular view radius from the square grid of nodes to be drawn
            if ((old_view_center - view_offset).GetLengthSquared() < radius_sum*radius_sum)
            {
                draw_loop_functor.SetViewCenter(old_view_center - view_offset);

                glLoadIdentity();
                glTranslatef(
                    view_offset[Dim::X],
                    view_offset[Dim::Y],
                    0.0f);

                if (draw_loop_functor.GetIsCollectTransparentObjectPass())
                    Draw(draw_loop_functor);
                else
                    std::for_each(
                        draw_loop_functor.GetTransparentObjectVector()->begin(),
                        draw_loop_functor.GetTransparentObjectVector()->end(),
                        draw_loop_functor);
            }
        }
    }
}

} // end of namespace Xrb
