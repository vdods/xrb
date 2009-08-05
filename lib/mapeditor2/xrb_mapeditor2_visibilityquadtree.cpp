// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_visibilityquadtree.cpp by Victor Dods, created 2005/03/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_mapeditor2_visibilityquadtree.hpp"

#include "xrb_mapeditor2_compound.hpp"
#include "xrb_mapeditor2_entity.hpp"
#include "xrb_mapeditor2_objectlayer.hpp"
#include "xrb_mapeditor2_polygon.hpp"
#include "xrb_screen.hpp"
#include "xrb_serializer.hpp"

namespace Xrb
{

MapEditor2::VisibilityQuadTree::VisibilityQuadTree (
    FloatVector2 const &center,
    Float half_side_length,
    Uint8 depth)
    :
    Engine2::VisibilityQuadTree(NULL)
{
    Initialize<VisibilityQuadTree>(center, half_side_length, depth);
    SetQuadTreeType(Engine2::QTT_VISIBILITY);
}

MapEditor2::VisibilityQuadTree *MapEditor2::VisibilityQuadTree::Create (Serializer &serializer)
{
    VisibilityQuadTree *retval = new VisibilityQuadTree(NULL);

    retval->ReadStructure(serializer);
    retval->SetQuadTreeType(Engine2::QTT_VISIBILITY);
    // the objects are left to be read once this new quadtree
    // is returned to the objectlayer

    return retval;
}

MapEditor2::Polygon *MapEditor2::VisibilityQuadTree::GetSmallestMapEditorPolygonTouchingPoint (
    FloatVector2 const &point,
    bool const mask_by_object_selection_set)
{
    Polygon *retval = NULL;

    // if this node contains no objects, early out
    if (GetSubordinateObjectCount() == 0)
        return retval;

    // if the point is outside the reaches of this quad node, early out
    if ((point - m_center).GetLengthSquared() > 4.0*m_radius*m_radius)
        return retval;

    Polygon *smallest_candidate;

    // get the smallest object from the children
    if (m_child[0] != NULL)
    {
        for (Uint8 i = 0; i < 4; ++i)
        {
            ASSERT2(m_child[i] != NULL);
            smallest_candidate =
                DStaticCast<MapEditor2::VisibilityQuadTree *>(m_child[i])->
                    GetSmallestMapEditorPolygonTouchingPoint(
                        point,
                        mask_by_object_selection_set);
            if (retval == NULL ||
                smallest_candidate != NULL &&
                smallest_candidate->Area() < retval->Area())
                retval = smallest_candidate;
        }
    }

    // check against all the objects owned by this node
    for (ObjectSetIterator it = m_object_set.begin(),
                           it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Engine2::Object *object = *it;
        ASSERT1(object != NULL);
        Compound *compound = dynamic_cast<Compound *>(object);
        if (compound == NULL)
            continue;

        if (mask_by_object_selection_set && !compound->IsSelected())
            continue;
            
        smallest_candidate = compound->GetSmallestPolygonTouchingPoint(point);
        // if the compound returned a polygon,
        if (smallest_candidate != NULL)
            // and either retval is null, or smallest_candidate is smaller
            if (retval == NULL ||
                smallest_candidate->Area() < retval->Area())
                // assign it as the smallest
                retval = smallest_candidate;
    }

    return retval;
}

void MapEditor2::VisibilityQuadTree::ReadStructure (Serializer &serializer)
{
    // write the VisibilityQuadTree's structure info
    serializer.ReadFloatVector2(&m_center);
    serializer.ReadFloat(&m_half_side_length);
    serializer.ReadFloat(&m_radius);
    bool has_children = serializer.ReadBool();
    // if there are children, recursively call this function on them
    if (has_children)
    {
        for (Uint8 i = 0; i < 4; ++i)
        {
            ASSERT1(m_child[i] == NULL);
            VisibilityQuadTree *child = new VisibilityQuadTree(this);
            ASSERT1(child != NULL);
            child->ReadStructure(serializer);
            m_child[i] = child;
        }
    }
}

void MapEditor2::VisibilityQuadTree::ReadObjects (
    Serializer &serializer,
    MapEditor2::ObjectLayer *const object_layer)
{
    ASSERT1(object_layer != NULL);

    fprintf(stderr, "MapEditor2::VisibilityQuadTree::ReadObjects();\n");
    Uint32 non_entity_count = serializer.ReadUint32();
    while (non_entity_count > 0) {
        Object *object = Object::Create(serializer);
        ASSERT1(dynamic_cast<Entity *>(object) == NULL);
        object_layer->AddObject(object);
        --non_entity_count;
    }
}

void MapEditor2::VisibilityQuadTree::DrawMetrics (
    RenderContext const &render_context,
    FloatMatrix2 const &world_to_screen,
    Float const pixels_in_view_radius,
    FloatVector2 const &view_center,
    Float const view_radius,
    Object::MetricMode const metric_mode)
{
    DrawLoopFunctor
        draw_data(
            render_context,
            world_to_screen,
            pixels_in_view_radius,
            view_center,
            view_radius);
    DrawMetrics(draw_data, metric_mode);
}

void MapEditor2::VisibilityQuadTree::DrawMetricsWrapped (
    RenderContext const &render_context,
    FloatMatrix2 const &world_to_screen,
    Float const pixels_in_view_radius,
    FloatVector2 const &view_center,
    Float const view_radius,
    Object::MetricMode const metric_mode)
{
    DrawLoopFunctor
        draw_data(
            render_context,
            world_to_screen,
            pixels_in_view_radius,
            view_center,
            view_radius);
    DrawMetricsWrapped(draw_data, metric_mode);
}

void MapEditor2::VisibilityQuadTree::SelectObjectsByCircle (
    FloatVector2 const &center,
    Float const radius,
    Object::SelectionOperation selection_operation,
    bool const select_touching)
{
    // check that this quad node meets the requirements of its
    // objects touching/being-inside the given circle
    if (select_touching)
    {
        // if the circle is not touching the reach of this quadnode,
        // do an early out.
        if ((center - GetCenter()).GetLength() > radius + 2.0f*GetRadius())
            return;
    }
    else
    {
        // if the circle is not touching the exact bounding circle
        // of this quad node, early out
        if ((center - GetCenter()).GetLength() > radius + GetRadius())
            return;
    }

    // iterate through all objects at this node and perform the
    // requested selection action upon each.
    for (ObjectSetIterator it = m_object_set.begin(),
                           it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        bool object_is_in_operand_set;
        Float object_distance;
        Object *object = dynamic_cast<Object *>(*it);
        ASSERT1(object != NULL);
        ASSERT1(object->GetOwnerMapEditorQuadTree() == this);

        // check if the object meets the selection criteria
        object_distance =
            (center - object->GetTranslation()).GetLength();
        if (select_touching)
            object_is_in_operand_set =
                object_distance <= radius + object->GetVisibleRadius();
        else
            object_is_in_operand_set =
                object_distance <= radius - object->GetVisibleRadius();

        // perform the requested selection operation on the object
        object->ApplyObjectSelectionOperation(
            selection_operation,
            object_is_in_operand_set);
    }

    // if there are child nodes, call SelectAllObjects on each
    if (GetHasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            GetChild<VisibilityQuadTree>(i)->
                SelectObjectsByCircle(
                    center,
                    radius,
                    selection_operation,
                    select_touching);
}

void MapEditor2::VisibilityQuadTree::SelectAllObjects (bool const toggle_selection)
{
    // iterate through all objects at this node and perform the
    // requested selection action upon each.
    for (ObjectSetIterator it = m_object_set.begin(),
                           it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Object *object = dynamic_cast<Object *>(*it);
        ASSERT1(object != NULL);
        if (toggle_selection)
            object->ToggleIsSelected();
        else
            object->SetIsSelected(true);
    }

    // if there are child nodes, call SelectAllObjects on each
    if (GetHasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            GetChild<VisibilityQuadTree>(i)->SelectAllObjects(toggle_selection);
}

void MapEditor2::VisibilityQuadTree::SelectVerticesByCircle (
    FloatVector2 const &center,
    Float const radius,
    MapEditor2::Object::SelectionOperation const selection_operation)
{
    // check that this quad node meets the requirements of its
    // objects touching the given circle
    
    // if the circle is not touching the reach of this quadnode,
    // do an early out.
    if ((center - GetCenter()).GetLength() > radius + 2.0f*GetRadius())
        return;

    // iterate through all objects at this node and perform the
    // requested selection action upon each.
    for (ObjectSetIterator it = m_object_set.begin(),
                           it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Object *object = dynamic_cast<Object *>(*it);
        ASSERT1(object != NULL);
        ASSERT1(object->GetOwnerMapEditorQuadTree() == this);

        // skip Objects that are not Compounds, because only
        // Compounds can have vertices.
        Compound *compound = dynamic_cast<Compound *>(object);
        if (compound == NULL)
            continue;
        
        // perform the requested selection operation on the compound
        compound->ApplyVertexSelectionOperation(
            center,
            radius,
            selection_operation);
    }

    // if there are child nodes, call SelectVerticesByCircle on each
    if (GetHasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            GetChild<VisibilityQuadTree>(i)->
                SelectVerticesByCircle(
                    center,
                    radius,
                    selection_operation);
}

void MapEditor2::VisibilityQuadTree::SelectAllVertices (
    bool const toggle_selection,
    bool const mask_by_object_selection_set)
{
    // iterate through all objects at this node and perform the
    // requested selection action upon each.
    for (ObjectSetIterator it = m_object_set.begin(),
                           it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Object *object = dynamic_cast<Object *>(*it);
        ASSERT1(object != NULL);
        ASSERT1(object->GetOwnerMapEditorQuadTree() == this);

        // skip Objects that are not Compounds, because only
        // Compounds can have vertices.
        Compound *compound = dynamic_cast<Compound *>(object);
        if (compound == NULL)
            continue;
        
        // perform the requested selection operation on the compound
        if (!mask_by_object_selection_set || compound->IsSelected())
            compound->SelectAllVertices(toggle_selection);
    }

    // if there are child nodes, call SelectAllVertices on each
    if (GetHasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            GetChild<VisibilityQuadTree>(i)->
                SelectAllVertices(
                    toggle_selection,
                    mask_by_object_selection_set);
}

void MapEditor2::VisibilityQuadTree::ComputeNearestVertex (
    FloatVector2 const &center,
    Float const radius,
    MapEditor2::Compound **const compound_containing_nearest,
    MapEditor2::CompoundVertex **const nearest_vertex)
{
    ASSERT1(radius > 0.0f);
    ASSERT1(compound_containing_nearest != NULL);
    ASSERT1(nearest_vertex != NULL);

    *compound_containing_nearest = NULL;
    *nearest_vertex = NULL;
    // -1.0 is a required sentinel value for ComputeNearestVertexPrivate
    Float nearest_distance = -1.0f;
    ComputeNearestVertexPrivate(
        center,
        radius,
        compound_containing_nearest,
        nearest_vertex,
        &nearest_distance);
}

void MapEditor2::VisibilityQuadTree::SelectAllPolygons (
    bool const toggle_selection,
    bool const mask_by_object_selection_set)
{
    // iterate through all objects at this node and perform the
    // requested selection action upon each.
    for (ObjectSetIterator it = m_object_set.begin(),
                           it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Object *object = dynamic_cast<Object *>(*it);
        ASSERT1(object != NULL);
        ASSERT1(object->GetOwnerMapEditorQuadTree() == this);

        // skip Objects that are not Compounds, because only
        // Compounds can have vertices.
        Compound *compound = dynamic_cast<Compound *>(object);
        if (compound == NULL)
            continue;
        
        // perform the requested selection operation on the compound
        if (!mask_by_object_selection_set || compound->IsSelected())
            compound->SelectAllPolygons(toggle_selection);
    }

    // if there are child nodes, call SelectAllPolygons on each
    if (GetHasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            GetChild<VisibilityQuadTree>(i)->
                SelectAllPolygons(
                    toggle_selection,
                    mask_by_object_selection_set);
}

void MapEditor2::VisibilityQuadTree::SetVertexSelectionStateFromSelectionOwnerPolygonCount ()
{
    // iterate through all objects at this node and perform the
    // requested selection action upon each.
    for (ObjectSetIterator it = m_object_set.begin(),
                           it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Object *object = dynamic_cast<Object *>(*it);
        ASSERT1(object != NULL);
        ASSERT1(object->GetOwnerMapEditorQuadTree() == this);

        // skip Objects that are not Compounds, because only
        // Compounds can have vertices.
        Compound *compound = dynamic_cast<Compound *>(object);
        if (compound == NULL)
            continue;

        // actually do the action on the compound.        
        compound->SetVertexSelectionStateFromSelectionOwnerPolygonCount();
    }

    // if there are child nodes, call SetVertexSelectionStateFromSelectionOwnerPolygonCount on each
    if (GetHasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            GetChild<VisibilityQuadTree>(i)->
                SetVertexSelectionStateFromSelectionOwnerPolygonCount();
}

void MapEditor2::VisibilityQuadTree::DrawMetrics (
    Engine2::VisibilityQuadTree::DrawLoopFunctor const &draw_data,
    Object::MetricMode const metric_mode)
{
    // if there are no objects here or below, just return
    if (GetSubordinateObjectCount() == 0)
        return;

    ASSERT2(draw_data.GetPixelsInViewRadius() > 0.0f);
    ASSERT2(draw_data.GetViewRadius() > 0.0f);

    // constants which control the thresholds at which objects use
    // alpha fading to fade away, when they become small enough.
    Float const radius_limit_upper = 3.0f;
    Float const radius_limit_lower = 0.8f;
    ASSERT1(radius_limit_upper > radius_limit_lower);
    Float const distance_fade_slope =
        1.0f /
        (radius_limit_upper - radius_limit_lower);
    Float const distance_fade_intercept =
        radius_limit_lower /
        (radius_limit_lower - radius_limit_upper);

    // don't draw quadtrees whose radii are lower than the
    // radius_limit_lower threshold -- a form of distance culling,
    // which gives a huge speedup and allows zooming to any level
    // maintain a consistent framerate.
    if (draw_data.GetPixelsInViewRadius() * GetRadius() /
        draw_data.GetViewRadius()
        <
        radius_limit_lower)
    {
        return;
    }

    // return if the view doesn't intersect this node
    if (!GetDoesAreaOverlapQuadBounds(draw_data.GetViewCenter(), draw_data.GetViewRadius()))
        return;

    // draw all the objects' bounding circles in this node's list
    Object::DrawData object_draw_data = draw_data.GetObjectDrawData();
    Float object_radius;
    Float distance_fade;
    for (ObjectSetIterator it = m_object_set.begin(),
                           it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Object *object = dynamic_cast<Object *>(*it);
        ASSERT1(object != NULL);
        ASSERT1(object->GetOwnerMapEditorQuadTree() == this);
        // calculate the object's pixel radius on screen
        object_radius =
            draw_data.GetPixelsInViewRadius() * object->GetVisibleRadius() /
            draw_data.GetViewRadius();
        // distance culling - don't draw objects that are below the
        // radius_limit_lower threshold
        if (object_radius >= radius_limit_lower)
        {
            // calculate the alpha value of the object due to its distance.
            // sprites with radii between radius_limit_lower and
            // radius_limit_upper will be partially transparent, fading away
            // once they get to radius_limit_lower.  this gives a very
            // nice smooth transition for when the objects are not drawn
            // because they are below the lower radius threshold.
            distance_fade =
                (object_radius > radius_limit_upper) ?
                1.0f :
                (distance_fade_slope * object_radius + distance_fade_intercept);
            // actually draw the sprite
            object->DrawMetrics(
                object_draw_data,
                distance_fade,
                metric_mode);
        }
    }

    // if there are child nodes, call draw on each
    if (GetHasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            GetChild<VisibilityQuadTree>(i)->DrawMetrics(draw_data, metric_mode);
}

void MapEditor2::VisibilityQuadTree::DrawMetricsWrapped (
    Engine2::VisibilityQuadTree::DrawLoopFunctor draw_data,
    Object::MetricMode const metric_mode)
{
    // if there are no objects here or below, just return
    if (GetSubordinateObjectCount() == 0)
        return;

    ASSERT2(draw_data.GetPixelsInViewRadius() > 0.0);
    ASSERT2(draw_data.GetViewRadius() > 0.0);
    ASSERT2(m_half_side_length > 0.0);

    Float side_length = GetSideLength();
    Float radius_sum = 2.0f*GetRadius() + draw_data.GetViewRadius();
    Float top = floor((draw_data.GetViewCenter().m[1]+radius_sum)/side_length);
    Float bottom = ceil((draw_data.GetViewCenter().m[1]-radius_sum)/side_length);
    Float left = ceil((draw_data.GetViewCenter().m[0]-radius_sum)/side_length);
    Float right = floor((draw_data.GetViewCenter().m[0]+radius_sum)/side_length);
    FloatMatrix2 new_world_to_screen;
    FloatMatrix2 old_world_to_screen(draw_data.GetWorldToScreen());
    FloatVector2 view_offset;

    for (Float x = left; x <= right; x += 1.0f)
    {
        for (Float y = bottom; y <= top; y += 1.0f)
        {
            view_offset.SetComponents(side_length*x, side_length*y);
            if (view_offset.GetLengthSquared() < radius_sum*radius_sum)
            {
                new_world_to_screen.SetComponents(1.0f, 0.0f, view_offset.m[0],
                                                  0.0f, 1.0f, view_offset.m[1]);
                new_world_to_screen *= old_world_to_screen;
                draw_data.SetWorldToScreen(new_world_to_screen);
                DrawMetrics(draw_data, metric_mode);
            }
        }
    }
}

void MapEditor2::VisibilityQuadTree::ComputeNearestVertexPrivate (
    FloatVector2 const &center,
    Float const radius,
    MapEditor2::Compound **const compound_containing_nearest,
    MapEditor2::CompoundVertex **const nearest_vertex,
    Float *const nearest_distance)
{
    ASSERT1(radius > 0.0f);
    ASSERT1(compound_containing_nearest != NULL);
    ASSERT1(nearest_vertex != NULL);
    ASSERT1(nearest_distance != NULL);

    // check that this quad node meets the requirements of its
    // objects touching the given circle
    
    // if the circle is not touching the reach of this quadnode,
    // do an early out.
    if ((center - GetCenter()).GetLength() > radius + 2.0f*GetRadius())
        return;

    // iterate through all objects at this node and perform the
    // requested selection action upon each.
    for (ObjectSetIterator it = m_object_set.begin(),
                           it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Object *object = dynamic_cast<Object *>(*it);
        ASSERT1(object != NULL);
        ASSERT1(object->GetOwnerMapEditorQuadTree() == this);

        // skip Objects that are not Compounds, because only
        // Compounds can have vertices.
        Compound *compound = dynamic_cast<Compound *>(object);
        if (compound == NULL)
            continue;
        
        // perform the requested selection operation on the compound
        compound->ComputeNearestVertex(
            center,
            radius,
            compound_containing_nearest,
            nearest_vertex,
            nearest_distance);
    }

    // if there are child nodes, call ComputeNearestVertexPrivate on each
    if (GetHasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            GetChild<VisibilityQuadTree>(i)->
                ComputeNearestVertexPrivate(
                    center,
                    radius,
                    compound_containing_nearest,
                    nearest_vertex,
                    nearest_distance);
}

} // end of namespace Xrb
