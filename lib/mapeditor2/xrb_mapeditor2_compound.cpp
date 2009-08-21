// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_compound.cpp by Victor Dods, created 2005/06/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_mapeditor2_compound.hpp"

#include "xrb_gl.hpp"
#include "xrb_mapeditor2_compoundentity.hpp"
#include "xrb_mapeditor2_objectlayer.hpp"
#include "xrb_mapeditor2_polygon.hpp"
#include "xrb_serializer.hpp"

namespace Xrb
{

MapEditor2::Compound::Compound (
    FloatVector2 const &polygon_center,
    Float const polygon_radius,
    Float polygon_angle,
    Uint32 const vertex_count,
    Resource<GLTexture> const &texture)
    :
    Engine2::Object(),
    Object(),
    Engine2::Compound()
{
    AddDrawnPolygon(
        FloatVector2::ms_zero,
        polygon_radius,
        polygon_angle,
        vertex_count,
        texture);

    SetTranslation(polygon_center);
}

MapEditor2::Compound::~Compound ()
{
    // must delete the polygons first, because they refer to
    // the compound's instanced vertices.
    for (PolygonList::iterator it = m_polygon_list.begin(),
                             it_end = m_polygon_list.end();
         it != it_end;
         ++it)
    {
        Polygon *polygon = *it;
        ASSERT1(polygon != NULL);
        Delete(polygon);
    }

    // the reference counts on all the vertices must be 0 upon deletion.
    for (VertexList::iterator it = m_vertex_list.begin(),
                            it_end = m_vertex_list.end();
         it != it_end;
         ++it)
    {
        Instance<CompoundVertex> *vertex_instance = *it;
        ASSERT1(vertex_instance != NULL);
        Delete(vertex_instance);
    }
}

MapEditor2::Compound *MapEditor2::Compound::CreateCompoundNonEntityClone (
    CompoundEntity const *const compound_entity)
{
    ASSERT1(compound_entity != NULL);
    Compound *retval = new Compound();
    retval->Engine2::Object::CloneProperties(compound_entity);
    retval->Object::CloneProperties(compound_entity);
    retval->Compound::CloneProperties(compound_entity);
    return retval;
}

/*
MapEditor2::Compound *MapEditor2::Compound::Create (std::string const &prefab_path)
{
}
*/

MapEditor2::Compound *MapEditor2::Compound::Create (Serializer &serializer)
{
    Compound *retval = new Compound();

    retval->Engine2::Object::ReadClassSpecific(serializer);
    retval->Compound::ReadClassSpecific(serializer);

    return retval;
}

void MapEditor2::Compound::Write (Serializer &serializer) const
{
    WriteSubType(serializer);

    Engine2::Object::WriteClassSpecific(serializer);
    Compound::WriteClassSpecific(serializer);
}

Engine2::Object *MapEditor2::Compound::CreateClone () const
{
    Compound *retval = new Compound();

    // skip Engine2::Compound::CloneProperties, because MapEditor2::Compound
    // doesn't use the same guts.
    retval->Engine2::Object::CloneProperties(this);
    retval->Object::CloneProperties(this);
    retval->Compound::CloneProperties(this);

    return static_cast<Engine2::Object *>(retval);
}

MapEditor2::Entity *MapEditor2::Compound::CreateEntityClone () const
{
    return static_cast<Entity *>(CompoundEntity::CreateCompoundEntityClone(this));
}

void MapEditor2::Compound::Draw (
    DrawData const &draw_data,
    Float const alpha_mask) const
{
    // set up the gl modelview matrix
    glMatrixMode(GL_MODELVIEW);
    // we have to push the matrix here (instead of loading the
    // identity) because for wrapped spaces, the wrapped offset
    // is set in the GL matrix.
    glPushMatrix();

    // model-to-world transformation (this seems backwards,
    // but for some reason it's correct).
    glTranslatef(
        Translation()[Dim::X],
        Translation()[Dim::Y],
        0.0f);
    glRotatef(Angle(), 0.0f, 0.0f, 1.0f);
    glScalef(
        ScaleFactors()[Dim::X],
        ScaleFactors()[Dim::Y],
        1.0f);

    // set the color mask
    Color color_mask(draw_data.GetRenderContext().ColorMask());
    color_mask[Dim::A] *= alpha_mask;
    glColor4fv(color_mask.m);

    // enable texture mapping (because the polygon and sprite drawing
    // functions require it.
    glEnable(GL_TEXTURE_2D);

    for (PolygonList::const_iterator it = m_polygon_list.begin(),
                                  it_end = m_polygon_list.end();
         it != it_end;
         ++it)
    {
        Polygon *polygon = *it;
        ASSERT1(polygon != NULL);
        polygon->Draw();
    }

    glPopMatrix();
}

void MapEditor2::Compound::DrawMetrics (
    DrawData const &draw_data,
    Float const alpha_mask,
    MetricMode const metric_mode)
{
    Object::DrawMetrics(draw_data, alpha_mask, metric_mode);

    // set up the gl modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // model-to-world transformation (this seems backwards,
    // but for some reason it's correct).
    glTranslatef(
        Translation()[Dim::X],
        Translation()[Dim::Y],
        0.0f);
    glRotatef(Angle(), 0.0f, 0.0f, 1.0f);
    glScalef(
        ScaleFactors()[Dim::X],
        ScaleFactors()[Dim::Y],
        1.0f);

//     Color color(IsSelected() ?
//                 SelectedMetricsColor() :
//                 UnselectedMetricsColor());
//     color[Dim::A] *= alpha_mask;

//     glColor4fv(color.m);

    glDisable(GL_TEXTURE_2D);

    // draw the polygon lines (unselected first, then selected).

    for (PolygonList::const_iterator it = m_polygon_list.begin(),
                                  it_end = m_polygon_list.end();
         it != it_end;
         ++it)
    {
        Polygon *polygon = *it;
        ASSERT1(polygon != NULL);
        
        if (!polygon->m_is_selected || metric_mode == Object::MM_POLYGONS)
        {
            glColor4fv(UnselectedMetricsColor().m);
            polygon->DrawMetrics();
        }
    }

    for (PolygonList::const_iterator it = m_polygon_list.begin(),
                                    it_end = m_polygon_list.end();
         it != it_end;
         ++it)
    {
        Polygon *polygon = *it;
        ASSERT1(polygon != NULL);
        
        if (polygon->m_is_selected && metric_mode == Object::MM_POLYGONS)
        {
            glColor4f(0.0, 1.0, 1.0, 1.0);
            polygon->DrawMetrics();
        }
    }

    // if the metric mode is vertices, draw the vertices
    bool vertices_can_be_selected =
        MapEditorObjectLayer()->SelectedObjectCount() == 0 ||
        IsSelected();
    if (metric_mode == Object::MM_VERTICES && vertices_can_be_selected)
    {
        // draw the black outline for the vertices
        glPointSize(6.0f);
        glBegin(GL_POINTS);
        glColor4f(0.0, 0.0, 0.0, 1.0);
        for (VertexList::iterator it = m_vertex_list.begin(),
                                it_end = m_vertex_list.end();
             it != it_end;
             ++it)
        {
            Instance<CompoundVertex> *vertex_instance = *it;
            ASSERT1(vertex_instance != NULL);
            CompoundVertex const &vertex = **vertex_instance;
    
            glVertex2fv(vertex.m_coordinate.m);
        }        
        glEnd();

        // draw the colored vertex (unselected first, then selected).
        glPointSize(4.0f);
        glBegin(GL_POINTS);
        for (VertexList::iterator it = m_vertex_list.begin(),
                                it_end = m_vertex_list.end();
             it != it_end;
             ++it)
        {
            Instance<CompoundVertex> *vertex_instance = *it;
            ASSERT1(vertex_instance != NULL);
            CompoundVertex const &vertex = **vertex_instance;
    
            if (!vertex.m_is_selected)
            {
                glColor4fv(UnselectedMetricsColor().m);
                glVertex2fv(vertex.m_coordinate.m);
            }
        }
        for (VertexList::iterator it = m_vertex_list.begin(),
                                it_end = m_vertex_list.end();
             it != it_end;
             ++it)
        {
            Instance<CompoundVertex> *vertex_instance = *it;
            ASSERT1(vertex_instance != NULL);
            CompoundVertex const &vertex = **vertex_instance;
    
            if (vertex.m_is_selected)
            {
                glColor4f(0.0, 1.0, 1.0, 1.0);
                glVertex2fv(vertex.m_coordinate.m);
            }
        }
        glEnd();
    }
}

Uint32 MapEditor2::Compound::SelectedPolygonCount () const
{
    Uint32 selected_polygon_count = 0;

    for (PolygonList::const_iterator it = m_polygon_list.begin(),
                                  it_end = m_polygon_list.end();
         it != it_end;
         ++it)
    {
        Polygon *polygon = *it;
        ASSERT1(polygon != NULL);
        if (polygon->m_is_selected)
            ++selected_polygon_count;
    }

    return selected_polygon_count;
}

Instance<MapEditor2::CompoundVertex> *MapEditor2::Compound::VertexInstance (
    Uint32 index) const
{
    VertexList::const_iterator it = m_vertex_list.begin();
    VertexList::const_iterator it_end = m_vertex_list.end();
    while (index != 0 && it != it_end)
    {
        --index;
        ++it;
    }
    ASSERT1(it != it_end);
    Instance<CompoundVertex> *vertex_instance = *it;
    ASSERT1(vertex_instance != NULL);
    return vertex_instance;
}

MapEditor2::Polygon *MapEditor2::Compound::SmallestPolygonTouchingPoint (
    FloatVector2 const &point) const
{
    Polygon *smallest = NULL;
    Float smallest_area = 0.0f;

    // calculate what the point is in model-space
    FloatVector2 model_space_point(point);
    model_space_point *= TransformationInverse();

    for (PolygonList::const_iterator it = m_polygon_list.begin(),
                                  it_end = m_polygon_list.end();
         it != it_end;
         ++it)
    {
        Polygon *polygon = *it;
        ASSERT1(polygon != NULL);
        
        // skip polygons that don't contain the point
        if (!polygon->IsPointInside(model_space_point))
            continue;
            
        // if this is the first one, set the smallest to this one
        if (smallest == NULL)
        {
            smallest = polygon;
            smallest_area = smallest->Area();
        }
        // otherwise only take a polygon with a smaller area
        else
        {
            Float area = polygon->Area();
            if (area < smallest_area)
            {
                smallest = polygon;
                smallest_area = area;
            }
        }
    }

    return smallest;
}

void MapEditor2::Compound::ComputeNearestVertex (
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

    // vertices can only be selected if there are no selected objects,
    // or if there are, they are from the object selection set.
    bool vertices_can_be_selected =
        MapEditorObjectLayer()->SelectedObjectCount() == 0 ||
        IsSelected();
    if (!vertices_can_be_selected)
        return;
    
    for (VertexList::iterator it = m_vertex_list.begin(),
                            it_end = m_vertex_list.end();
         it != it_end;
         ++it)
    {
        Instance<CompoundVertex> *vertex_instance = *it;
        ASSERT1(vertex_instance != NULL);
        CompoundVertex &compound_vertex = **vertex_instance;

        Float distance = ((Transformation() * compound_vertex.m_coordinate) - center).Length();
        if (distance <= radius &&
            (distance < *nearest_distance || *nearest_distance < 0.0f))
        {
            *compound_containing_nearest = this;
            *nearest_vertex = &compound_vertex;
            *nearest_distance = distance;
        }
    }
}

MapEditor2::Compound::WeldReturnStatus MapEditor2::Compound::WeldSelectedVertices ()
{
    ObjectLayer *const object_layer = MapEditorObjectLayer();
    ASSERT1(object_layer != NULL);

    // first check that no polygon has more than n-2 vertices
    // selected, because that would result in it becoming degenerate.
    // also check that for each polygon, the selected vertices are
    // contiguous, otherwise a "pinch" will occur and the polygon
    // will end up concave, and possibly partially degenerate.
    // also, during this loop, count up the number of selected polygons
    // that have selected vertices, so that the selected owner polygon
    // count can be calculated for the new vertex.
    Uint32 polygons_with_selected_vertices_count = 0;
    for (PolygonList::iterator it = m_polygon_list.begin(),
                             it_end = m_polygon_list.end();
         it != it_end;
         ++it)
    {
        Polygon *polygon = *it;
        ASSERT1(polygon != NULL);
        
        if (polygon->SelectedVertexCount() > polygon->VertexCount() - 2)
            return W_WOULD_CAUSE_DEGENERATION;

        if (!polygon->AreSelectedVerticesContiguous())
            return W_WOULD_CAUSE_PINCH;

        if (polygon->m_is_selected && polygon->SelectedVertexCount() > 0)
            ++polygons_with_selected_vertices_count;
    }
    
    // find the mean position of the selected vertices.  that will
    // be the resulting, single welded vertex.
    Uint32 selected_vertex_count = 0;
    FloatVector2 selected_vertex_mean(FloatVector2::ms_zero);
    for (VertexList::iterator it = m_vertex_list.begin(),
                            it_end = m_vertex_list.end();
         it != it_end;
         ++it)
    {
        Instance<CompoundVertex> *vertex_instance = *it;
        ASSERT1(vertex_instance != NULL);
        CompoundVertex const &vertex = **vertex_instance;
        if (vertex.m_is_selected)
        {
            selected_vertex_mean += vertex.m_coordinate;
            ++selected_vertex_count;
        }
    }
    selected_vertex_mean /= static_cast<Float>(selected_vertex_count);

    // create the new resulting vertex
    Instance<CompoundVertex> *welded_vertex_instance =
        new Instance<CompoundVertex>(CompoundVertex(this));
    (*welded_vertex_instance)->m_coordinate = selected_vertex_mean;
    (*welded_vertex_instance)->m_selected_owner_polygon_count =
        polygons_with_selected_vertices_count;
    m_vertex_list.push_back(welded_vertex_instance);

    // replace the selected vertices in the polygons with the newly
    // created welded vertex.
    for (PolygonList::iterator it = m_polygon_list.begin(),
                             it_end = m_polygon_list.end();
         it != it_end;
         ++it)
    {
        Polygon *polygon = *it;
        ASSERT1(polygon != NULL);
        polygon->WeldSelectedVertices(welded_vertex_instance);
    }
    
    // at this point, all the selected vertex instances should have a
    // reference count of 0.

    // unselect and delete the selected vertices
    for (VertexList::iterator it = m_vertex_list.begin(),
                            it_end = m_vertex_list.end();
         it != it_end;
         /* the iterator incrementing is handled by hand, below */)
    {
        Instance<CompoundVertex> *vertex_instance = *it;
        ASSERT1(vertex_instance != NULL);
        CompoundVertex &vertex = **vertex_instance;
        if (vertex.m_is_selected)
        {
            vertex.RemoveFromVertexSelectionSet(object_layer);
            VertexList::iterator remove_it = it;
            ++it;
            // delete the vertex instance itself
            Delete(vertex_instance);
            // remove the list iterator
            m_vertex_list.erase(remove_it);
        }
        else
        {
            ++it;
        }
    }

    // the contents of the compound have changed, so the Object
    // radius needs to be recalculated.
    IndicateRadiiNeedToBeRecalculated();
    
    return W_SUCCESSFUL;
}

void MapEditor2::Compound::UnweldSelectedVertices ()
{
    for (VertexList::iterator it = m_vertex_list.begin(),
                            it_end = m_vertex_list.end();
         it != it_end;
         ++it)
    {
        Instance<CompoundVertex> *vertex_instance = *it;
        ASSERT1(vertex_instance != NULL);
        ASSERT1(vertex_instance->ReferenceCount() > 0);
        CompoundVertex const &vertex = **vertex_instance;

        if (vertex.m_is_selected)
        {
            while (vertex_instance->ReferenceCount() > 1)
            {
                // create the instanced vertex and insert it in front of
                // the current vertex list iterator (so we don't iterate
                // over it).
                Instance<CompoundVertex> *unwelded_vertex_instance =
                    new Instance<CompoundVertex>(CompoundVertex(this));
                (*unwelded_vertex_instance)->m_coordinate = vertex.m_coordinate;
                m_vertex_list.insert(it, unwelded_vertex_instance);
                // all the unselected polygons which refer to the vertex being
                // unwelded must now replace the respective vertex with
                // the newly created one.
                ReplaceVertexWithNewVertex(
                    vertex_instance,
                    unwelded_vertex_instance,
                    true);
            }
        }
    }    
}

void MapEditor2::Compound::AddDrawnPolygon (
    FloatVector2 const &polygon_center,
    Float const polygon_radius,
    Float polygon_angle,
    Uint32 const vertex_count,
    Resource<GLTexture> const &texture)
{
    ASSERT1(vertex_count >= 3);
    ASSERT1(texture.IsValid());

    Instance<CompoundVertex> *vertex_instance;

    Polygon *polygon;
    Polygon::Vertex polygon_vertex;

    polygon = new Polygon(this);
    polygon->m_texture = texture;
    
    // create the vertices    
    FloatVector2 vertex;
    Float const angle_delta = 360.0f / static_cast<Float>(vertex_count);
    for (Uint32 i = 0; i < vertex_count; ++i)
    {
        vertex =
            polygon_center +
            polygon_radius * Math::UnitVector(polygon_angle);
        vertex *= TransformationInverse();

        // create the instanced vertex
        vertex_instance = new Instance<CompoundVertex>(CompoundVertex(this));
        (*vertex_instance)->m_coordinate = vertex;
        m_vertex_list.push_back(vertex_instance);
        // add it to the polygon
        polygon_vertex.m_compound_vertex = vertex_instance;
        polygon_vertex.m_texture_coordinate = vertex;
        polygon->m_vertex_list.push_back(polygon_vertex);        
        
        polygon_angle += angle_delta;
    }

    m_polygon_list.push_back(polygon);

    IndicateRadiiNeedToBeRecalculated();
}

void MapEditor2::Compound::UnweldSelectedPolygons ()
{
    for (VertexList::iterator it = m_vertex_list.begin(),
                            it_end = m_vertex_list.end();
         it != it_end;
         ++it)
    {
        Instance<CompoundVertex> *vertex_instance = *it;
        ASSERT1(vertex_instance != NULL);
        CompoundVertex const &vertex = **vertex_instance;

        if (vertex.m_is_selected)
        {
            ASSERT1(vertex.m_selected_owner_polygon_count > 0);
            // if the selected owner polygon count is less than the
            // vertex instance's reference count, at least one unselected
            // polygon refers to it, and therefore this vertex must be
            // unwelded (duplicated for the unselected polygon(s)).
            if (vertex.m_selected_owner_polygon_count <
                (*vertex_instance).ReferenceCount())
            {
                // create the instanced vertex and insert it in front of
                // the current vertex list iterator (so we don't iterate
                // over it).
                Instance<CompoundVertex> *unwelded_vertex_instance =
                    new Instance<CompoundVertex>(CompoundVertex(this));
                (*unwelded_vertex_instance)->m_coordinate = vertex.m_coordinate;
                m_vertex_list.insert(it, unwelded_vertex_instance);
                // all the unselected polygons which refer to the vertex being
                // unwelded must now replace the respective vertex with
                // the newly created one.
                ReplaceVertexWithNewVertex(
                    vertex_instance,
                    unwelded_vertex_instance,
                    false);
            }
        }
    }
}

void MapEditor2::Compound::DeleteSelectedPolygons ()
{
    // we don't want to allow deleting all the polygons, leaving
    // an empty and inaccessable object.
    ASSERT1(SelectedPolygonCount() < PolygonCount());

    // iterate through the polygon list and delete selected polygons.
    for (PolygonList::iterator it = m_polygon_list.begin(),
                             it_end = m_polygon_list.end();
         it != it_end;
         /* the iterator incrementing is handled by hand, below */)
    {
        Polygon *polygon = *it;
        ASSERT1(polygon != NULL);

        // delete selected polygons (this will automatically
        // decrement their vertices' reference counts)
        if (polygon->m_is_selected)
        {
            polygon->RemoveFromPolygonSelectionSet(MapEditorObjectLayer());
            Delete(polygon);
            PolygonList::iterator remove_it = it;
            ++it;
            m_polygon_list.erase(remove_it);
        }
        else
        {
            ++it;
        }
    }

    // go through the vertex list and delete any vertices that have a
    // reference count of zero.
    for (VertexList::iterator it = m_vertex_list.begin(),
                            it_end = m_vertex_list.end();
         it != it_end;
         /* the iterator incrementing is handled by hand, below */)
    {
        Instance<CompoundVertex> *vertex_instance = *it;
        ASSERT1(vertex_instance != NULL);
        if (vertex_instance->ReferenceCount() == 0)
        {
            Delete(vertex_instance);
            VertexList::iterator remove_it = it;
            ++it;
            m_vertex_list.erase(remove_it);
        }
        else
        {
            ++it;
        }
    }

    // the contents of the compound have changed, so the radius
    // needs to be recalculated.
    IndicateRadiiNeedToBeRecalculated();
}

void MapEditor2::Compound::ApplyVertexSelectionOperation(
    FloatVector2 const &center,
    Float const radius,
    MapEditor2::Object::SelectionOperation const selection_operation)
{
    // vertices can only be selected if there are no selected objects,
    // or if there are, they are from the object selection set.
    bool vertices_can_be_selected =
        MapEditorObjectLayer()->SelectedObjectCount() == 0 ||
        IsSelected();
    if (!vertices_can_be_selected)
        return;

    for (VertexList::iterator it = m_vertex_list.begin(),
                            it_end = m_vertex_list.end();
         it != it_end;
         ++it)
    {
        Instance<CompoundVertex> *vertex_instance = *it;
        ASSERT1(vertex_instance != NULL);
        CompoundVertex &vertex = **vertex_instance;

        // determine if the current vertex is inside the selection circle
        Float vertex_distance = (center - Transformation() * vertex.m_coordinate).Length();
        bool vertex_is_in_operand_set = vertex_distance <= radius;
        
        if (vertex_is_in_operand_set)
        {
            switch (selection_operation)
            {
                case SO_EQUALS: vertex.SetIsSelected(true, MapEditorObjectLayer());  break;
                case SO_IOR:    vertex.SetIsSelected(true, MapEditorObjectLayer());  break;
                case SO_MINUS:  vertex.SetIsSelected(false, MapEditorObjectLayer()); break;
                case SO_XOR:    vertex.ToggleIsSelected(MapEditorObjectLayer());     break;
                case SO_AND:                                                            break;
                default:        ASSERT1(false && "Invalid selection operation");        break;
            }
        }
        else
        {
            switch (selection_operation)
            {
                case SO_EQUALS: vertex.SetIsSelected(false, MapEditorObjectLayer()); break;
                case SO_IOR:                                                            break;
                case SO_MINUS:                                                          break;
                case SO_XOR:                                                            break;
                case SO_AND:    vertex.SetIsSelected(false, MapEditorObjectLayer()); break;
                default:        ASSERT1(false && "Invalid selection operation");        break;
            }
        }
    }
}

void MapEditor2::Compound::ApplyVertexSelectionOperation (
    MapEditor2::CompoundVertex *const vertex_to_select,
    MapEditor2::Object::SelectionOperation const selection_operation)
{
    ASSERT1(vertex_to_select != NULL);

    // vertices can only be selected if there are no selected objects,
    // or if there are, they are from the object selection set.
    bool vertices_can_be_selected =
        MapEditorObjectLayer()->SelectedObjectCount() == 0 ||
        IsSelected();
    if (!vertices_can_be_selected)
        return;

    for (VertexList::iterator it = m_vertex_list.begin(),
                            it_end = m_vertex_list.end();
         it != it_end;
         ++it)
    {
        Instance<CompoundVertex> *vertex_instance = *it;
        ASSERT1(vertex_instance != NULL);
        CompoundVertex &vertex = **vertex_instance;

        if (vertex_to_select != &vertex)
            continue;

        switch (selection_operation)
        {
            case SO_EQUALS: vertex.SetIsSelected(true, MapEditorObjectLayer());  break;
            case SO_IOR:    vertex.SetIsSelected(true, MapEditorObjectLayer());  break;
            case SO_MINUS:  vertex.SetIsSelected(false, MapEditorObjectLayer()); break;
            case SO_XOR:    vertex.ToggleIsSelected(MapEditorObjectLayer());     break;
            case SO_AND:                                                            break;
            default:        ASSERT1(false && "Invalid selection operation");        break;
        }

        // only one vertex can be selected, so once it is, quit.
        return;
    }
}

void MapEditor2::Compound::SelectAllVertices (bool const toggle_selection)
{
    ObjectLayer *const object_layer = MapEditorObjectLayer();
    ASSERT1(object_layer != NULL);

    for (VertexList::iterator it = m_vertex_list.begin(),
                            it_end = m_vertex_list.end();
         it != it_end;
         ++it)
    {
        Instance<CompoundVertex> *vertex_instance = *it;
        ASSERT1(vertex_instance != NULL);
        CompoundVertex &vertex = **vertex_instance;

        if (toggle_selection)
            vertex.ToggleIsSelected(object_layer);
        else if (!vertex.m_is_selected)
            vertex.AddToVertexSelectionSet(object_layer);
    }
}

void MapEditor2::Compound::TranslateVertex (
    MapEditor2::CompoundVertex *const vertex,
    FloatVector2 const &translation_delta)
{
    ASSERT1(vertex != NULL);
    ASSERT1(vertex->m_owner_compound == this);

    // this vector is where the vertex should end up in world-space
    FloatVector2 resulting_coordinate = Transformation() * vertex->m_coordinate + translation_delta;
    // set the vertex coordinate (in model-space) to the model-space
    // pre-image of the resulting coordinate.
    vertex->m_coordinate = TransformationInverse() * resulting_coordinate;
    
    // the contents of the compound have changed, so its radius
    // will have to be recalculated
    IndicateRadiiNeedToBeRecalculated();
}

void MapEditor2::Compound::ScaleVertex (
    MapEditor2::CompoundVertex *const vertex,
    Float const scale_factor_delta,
    FloatVector2 const &transformation_origin)
{
    ASSERT1(vertex != NULL);
    ASSERT1(vertex->m_owner_compound == this);

    // this vector is where the vertex should end up in world-space
    FloatVector2 resulting_coordinate =
        scale_factor_delta *
        (Transformation() * vertex->m_coordinate - transformation_origin) +
        transformation_origin;
    // set the vertex coordinate (in model-space) to the model-space
    // pre-image of the resulting coordinate.
    vertex->m_coordinate = TransformationInverse() * resulting_coordinate;
        
    // the contents of the compound have changed, so its radius
    // will have to be recalculated
    IndicateRadiiNeedToBeRecalculated();
}

void MapEditor2::Compound::RotateVertex (
    CompoundVertex *const vertex,
    FloatMatrix2 const &rotation_transformation,
    FloatVector2 const &transformation_origin)
{
    ASSERT1(vertex != NULL);
    ASSERT1(vertex->m_owner_compound == this);

    // this vector is where the vertex should end up in world-space
    FloatVector2 resulting_coordinate =
        rotation_transformation * (Transformation() * vertex->m_coordinate);
    // set the vertex coordinate (in model-space) to the model-space
    // pre-image of the resulting coordinate.
    vertex->m_coordinate = TransformationInverse() * resulting_coordinate;
    
    // the contents of the compound have changed, so its radius
    // will have to be recalculated
    IndicateRadiiNeedToBeRecalculated();
}

void MapEditor2::Compound::SelectAllPolygons (bool const toggle_selection)
{
    ObjectLayer *const object_layer = MapEditorObjectLayer();
    ASSERT1(object_layer != NULL);

    for (PolygonList::iterator it = m_polygon_list.begin(),
                             it_end = m_polygon_list.end();
         it != it_end;
         ++it)
    {
        Polygon *polygon = *it;
        ASSERT1(polygon != NULL);
        
        if (toggle_selection)
            polygon->ToggleIsSelected(object_layer);
        else if (!polygon->m_is_selected)
            polygon->AddToPolygonSelectionSet(object_layer);
    }
}

void MapEditor2::Compound::SetVertexSelectionStateFromSelectionOwnerPolygonCount ()
{
    ObjectLayer *const object_layer = MapEditorObjectLayer();
    ASSERT1(object_layer != NULL);

    for (PolygonList::iterator it = m_polygon_list.begin(),
                             it_end = m_polygon_list.end();
         it != it_end;
         ++it)
    {
        Polygon *polygon = *it;
        ASSERT1(polygon != NULL);
        
        polygon->SetVertexSelectionStateFromSelectionOwnerPolygonCount(object_layer);
    }
}

MapEditor2::Compound::Compound ()
    :
    Engine2::Object(),
    Object(),
    Engine2::Compound()
{
}

void MapEditor2::Compound::ReadClassSpecific (Serializer &serializer)
{
    ASSERT1(serializer.GetIODirection() == IOD_READ);
    ASSERT1(m_vertex_list.empty());
    ASSERT1(m_polygon_list.empty());

    Uint32 vertex_count = serializer.ReadUint32();
    ASSERT1(vertex_count > 0);
    for (Uint32 i = 0; i < vertex_count; ++i)
    {
        Instance<CompoundVertex> *vertex_instance =
            new Instance<CompoundVertex>(CompoundVertex(this));
        serializer.ReadFloatVector2(&(*vertex_instance)->m_coordinate);
        m_vertex_list.push_back(vertex_instance);
    }

    Uint32 polygon_count = serializer.ReadUint32();
    ASSERT1(polygon_count > 0);
    for (Uint32 i = 0; i < polygon_count; ++i)
    {
        Polygon *polygon = new Polygon(this);
        polygon->Read(serializer);
        m_polygon_list.push_back(polygon);
    }
}

void MapEditor2::Compound::WriteClassSpecific (Serializer &serializer) const
{
    ASSERT1(serializer.GetIODirection() == IOD_WRITE);
    ASSERT1(!m_vertex_list.empty());
    ASSERT1(!m_polygon_list.empty());

    AssignIndicesToVertices();

    Uint32 vertex_count = m_vertex_list.size();
    ASSERT1(vertex_count > 0);
    serializer.WriteUint32(vertex_count);
    for (VertexList::const_iterator it = m_vertex_list.begin(),
                                 it_end = m_vertex_list.end();
         it != it_end;
         ++it)
    {
        Instance<CompoundVertex> *vertex_instance = *it;
        ASSERT1(vertex_instance != NULL);
        serializer.WriteFloatVector2((*vertex_instance)->m_coordinate);
        ASSERT1(vertex_count-- > 0);
    }
    ASSERT1(vertex_count == 0);

    Uint32 polygon_count = m_polygon_list.size();
    ASSERT1(polygon_count > 0);
    serializer.WriteUint32(polygon_count);
    for (PolygonList::const_iterator it = m_polygon_list.begin(),
                                  it_end = m_polygon_list.end();
         it != it_end;
         ++it)
    {
        Polygon const *polygon = *it;
        ASSERT1(polygon != NULL);
        polygon->Write(serializer);
    }

    ResetVertexIndices();
}

void MapEditor2::Compound::CalculateVisibleRadius () const
{
    m_radius = 0.0f;
    Uint32 vertex_count = VertexCount();

    for (Uint32 i = 0; i < vertex_count; ++i)
    {
        m_radius =
            Max(m_radius,
                (Transformation() * GetVertex(i) -
                 Transformation() * FloatVector2::ms_zero).Length());
    }
}

void MapEditor2::Compound::CloneProperties (Engine2::Object const *const object)
{
    Compound const *compound = dynamic_cast<Compound const *>(object);
    ASSERT1(compound != NULL);

    // make sure the vertices of compound are assigned
    compound->AssignIndicesToVertices();
    
    // clone the vertices
    for (VertexList::const_iterator it = compound->m_vertex_list.begin(),
                                 it_end = compound->m_vertex_list.end();
         it != it_end;
         ++it)
    {
        Instance<CompoundVertex> *vertex_instance = *it;
        ASSERT1(vertex_instance != NULL);
        CompoundVertex const &source_vertex = **vertex_instance;
        // copy every important property of the source vertex to the new one
        CompoundVertex cloned_vertex(this);
        cloned_vertex.m_coordinate = source_vertex.m_coordinate;
        // add it to the vertex list
        m_vertex_list.push_back(new Instance<CompoundVertex>(cloned_vertex));
    }

    // clone the polygons
    for (PolygonList::const_iterator it = compound->m_polygon_list.begin(),
                                  it_end = compound->m_polygon_list.end();
         it != it_end;
         ++it)
    {
        Polygon *polygon = *it;
        ASSERT1(polygon != NULL);
        Polygon *cloned_polygon = polygon->CreateClone();
        ASSERT1(cloned_polygon != NULL);

        // at this point, the compound vertices referenced by the
        // cloned polygon refer to the vertices in compound (the one
        // passed in via object).  these references need to be changed
        // over to refer to this Compound's vertices.
        cloned_polygon->ReassignVertices(this);

        // the cloned polygons "owner" compound is also still pointing
        // at the source compound.  change it to point at this compound.
        cloned_polygon->m_owner_compound = this;
        
        // processing is done, add it to this Compound's polygon list
        m_polygon_list.push_back(cloned_polygon);
    }
}

FloatVector2 const &MapEditor2::Compound::GetVertex (Uint32 const index) const
{
    Instance<CompoundVertex> *vertex_instance = VertexInstance(index);
    CompoundVertex const &vertex = **vertex_instance;
    return vertex.m_coordinate;
}

void MapEditor2::Compound::AssignIndicesToVertices () const
{
    Uint32 index = 0;
    for (VertexList::const_iterator it = m_vertex_list.begin(),
                                 it_end = m_vertex_list.end();
         it != it_end;
         ++it, ++index)
    {
        Instance<CompoundVertex> *vertex_instance = *it;
        ASSERT1(vertex_instance != NULL);
        CompoundVertex const &vertex = **vertex_instance;
        ASSERT1(vertex.m_owner_compound == this);
        vertex.m_index = index;
    }
}

void MapEditor2::Compound::ResetVertexIndices () const
{
    for (VertexList::const_iterator it = m_vertex_list.begin(),
                                 it_end = m_vertex_list.end();
         it != it_end;
         ++it)
    {
        Instance<CompoundVertex> *vertex_instance = *it;
        ASSERT1(vertex_instance != NULL);
        CompoundVertex const &vertex = **vertex_instance;
        ASSERT1(vertex.m_owner_compound == this);
        vertex.m_index = UINT32_UPPER_BOUND;
    }
}

void MapEditor2::Compound::ReplaceVertexWithNewVertex (
    Instance<CompoundVertex> *const vertex_to_replace,
    Instance<CompoundVertex> *const vertex_to_replace_with,
    bool const use_unwelding_behavior)
{
    ASSERT1(vertex_to_replace != NULL);
    ASSERT1(vertex_to_replace_with != NULL);

    for (PolygonList::iterator it = m_polygon_list.begin(),
                             it_end = m_polygon_list.end();
         it != it_end;
         ++it)
    {
        Polygon *polygon = *it;
        ASSERT1(polygon != NULL);
        
        if (use_unwelding_behavior)
        {
            if (polygon->ReplaceVertexWithNewVertex(
                    vertex_to_replace,
                    vertex_to_replace_with))
            {
                // make sure the selected owner polygon count is consistent
                // for the newly created vertex.
                if (polygon->m_is_selected)
                    (*vertex_to_replace_with)->m_selected_owner_polygon_count = 1;
                else
                    ASSERT1((*vertex_to_replace_with)->m_selected_owner_polygon_count == 0);
                return;
            }
        }
        else if (!polygon->m_is_selected)
        {
            polygon->ReplaceVertexWithNewVertex(
                vertex_to_replace,
                vertex_to_replace_with);
        }
    } 
}

} // end of namespace Xrb
