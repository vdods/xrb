// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_polygon.cpp by Victor Dods, created 2005/07/03
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_mapeditor2_polygon.hpp"

#include "xrb_gl.hpp"
#include "xrb_mapeditor2_compound.hpp"
#include "xrb_mapeditor2_objectlayer.hpp"
#include "xrb_serializer.hpp"

namespace Xrb
{

MapEditor2::Polygon::Polygon (Compound *const owner_compound)
{
    ASSERT1(owner_compound != NULL);
    m_is_selected = false;
    m_owner_compound = owner_compound;
}

MapEditor2::Polygon *MapEditor2::Polygon::CreateClone () const
{
    ASSERT1(m_owner_compound != NULL);
    Polygon *retval = new Polygon(m_owner_compound);

    for (VertexListConstIterator it = m_vertex_list.begin(),
                                 it_end = m_vertex_list.end();
         it != it_end;
         ++it)
    {
        Vertex const &vertex = *it;
        // this does a copy by value
        retval->m_vertex_list.push_back(vertex);
    }
    
    retval->m_texture = m_texture;

    return retval;
}

Uint32 MapEditor2::Polygon::GetSelectedVertexCount () const
{
    Uint32 selected_vertex_count = 0;
    for (VertexListConstIterator it = m_vertex_list.begin(),
                                 it_end = m_vertex_list.end();
         it != it_end;
         ++it)
    {
        Vertex const &vertex = *it;
        if (vertex.m_compound_vertex->m_is_selected)
            ++selected_vertex_count;
    }
    return selected_vertex_count;
}

FloatVector2 const &MapEditor2::Polygon::GetVertex (Uint32 index) const
{
    VertexListConstIterator it = m_vertex_list.begin();
    VertexListConstIterator it_end = m_vertex_list.end();
    while (index != 0 && it != it_end)
    {
        --index;
        ++it;
    }
    ASSERT1(it != it_end);
    return it->m_compound_vertex->m_coordinate;
}

Float MapEditor2::Polygon::Area () const
{
    Float area = 0.0f;
    Uint32 vertex_count = GetVertexCount();
    ASSERT1(vertex_count > 0);

    if (vertex_count < 3)
        return area;

    for (Uint32 i = 0; i < vertex_count; ++i)
        area += GetVertex(i) & GetVertex((i + 1) % vertex_count);

    return 0.5f * area;
}

bool MapEditor2::Polygon::IsPointInside (FloatVector2 const &point) const
{
    Uint32 vertex_count = GetVertexCount();
    ASSERT1(vertex_count > 0);
    
    // trivially false (no area to land a point inside)
    if (vertex_count == 1)
        return false;

    ASSERT1(IsCounterclockwise());
    ASSERT1(IsConvex());

    // test the point being inside the angle made by
    // each corner of the polygon.
    for (Uint32 i = 0; i < vertex_count; ++i)
    {
        FloatVector2 const &vertex0 = GetVertex((i + 0) % vertex_count);
        FloatVector2 const &vertex1 = GetVertex((i + 1) % vertex_count);
        FloatVector2 const &vertex2 = GetVertex((i + 2) % vertex_count);
        Float corner_dot_product =
            (vertex0 - vertex1).Normalization() |
            (vertex2 - vertex1).Normalization();
        Float point_dot_product  =
            (vertex0 - vertex1).Normalization() |
            (point   - vertex1).Normalization();
        if (point_dot_product < corner_dot_product)
            return false;
    }

    // if it passed all the tests, it's convex.
    return true;   
}

bool MapEditor2::Polygon::IsCounterclockwise () const
{
    Uint32 vertex_count = GetVertexCount();
    ASSERT1(vertex_count > 0);

    // trivially true
    if (vertex_count < 3)
        return true;

    // all triangles must have positive normal
    for (Uint32 i = 0; i < vertex_count; ++i)
    {
        Float normal =
            (GetVertex((i + 1) % vertex_count) - GetVertex(i)) &
            (GetVertex((i + 2) % vertex_count) - GetVertex(i));
        if (normal < 0.0f)
            return false;
    }

    // if it passed all the tests, it's counterclockwise.
    return true;
}

bool MapEditor2::Polygon::IsConvex () const
{
    Uint32 vertex_count = GetVertexCount();
    ASSERT1(vertex_count > 0);

    // trivially true
    if (vertex_count <= 3)
        return true;

    // the rest of the triangles must have the same winding
    // as the first one.
    Float normal =
        (GetVertex(1) - GetVertex(0)) & (GetVertex(2) - GetVertex(0));
    bool triangle_is_counterclockwise = normal >= 0.0f;

    for (Uint32 i = 1; i < vertex_count; ++i)
    {
        normal =
            (GetVertex((i + 1) % vertex_count) - GetVertex(i)) &
            (GetVertex((i + 2) % vertex_count) - GetVertex(i));
        // if the winding on one of the triangles differs, then
        // it's not a convex polygon.
        if (triangle_is_counterclockwise && normal < 0.0f)
            return false;
    }

    // if it passed all the tests, it's convex.
    return true;
}

bool MapEditor2::Polygon::AreSelectedVerticesContiguous () const
{
    // count the number of transitions.  if it is <= 2, then the
    // selected vertices are in a contiguous segment.
    Uint32 selected_vertex_transitions = 0;
    bool previous_selected_state =
        (*m_vertex_list.begin()).m_compound_vertex->m_is_selected;
    for (VertexListConstIterator it = m_vertex_list.begin(),
                                 it_end = m_vertex_list.end();
         it != it_end;
         ++it)
    {
        Vertex const &vertex = *it;
        if (vertex.m_compound_vertex->m_is_selected != previous_selected_state)
            ++selected_vertex_transitions;
        previous_selected_state = vertex.m_compound_vertex->m_is_selected;
    }
    return selected_vertex_transitions <= 2;
}

void MapEditor2::Polygon::WeldSelectedVertices (
    Instance<CompoundVertex> *const welded_vertex_instance)
{
    ASSERT1(welded_vertex_instance != NULL);
    ASSERT2(AreSelectedVerticesContiguous());

    bool welded_vertex_is_added_in = false;
    for (VertexListIterator it = m_vertex_list.begin(),
                            it_end = m_vertex_list.end();
         it != it_end;
         /* the iterator incrementing is handled by hand, below */)
    {
        Vertex &vertex = *it;
        if (vertex.m_compound_vertex->m_is_selected)
        {
            // if the welded vertex hasn't been added yet, do so
            if (!welded_vertex_is_added_in)
            {
                Vertex welded_vertex;
                welded_vertex.m_compound_vertex = welded_vertex_instance;
                welded_vertex.m_texture_coordinate =
                    vertex.m_texture_coordinate;
                m_vertex_list.insert(it, welded_vertex);

                welded_vertex_is_added_in = true;
            }

            // remove the selected vertex.
            VertexListIterator remove_it = it;
            ++it;
            m_vertex_list.erase(remove_it);
        }
        else
        {
            ++it;
        }
    }
}

void MapEditor2::Polygon::Draw () const
{
    ASSERT1(GL::MatrixMode() == GL_MODELVIEW);
    ASSERT1(GL::IsTexture2dOn());

    if (!m_texture.IsValid())
        return;

    glBindTexture(GL_TEXTURE_2D, m_texture->Handle());

    glBegin(GL_POLYGON);

    for (VertexListConstIterator it = m_vertex_list.begin(),
                                 it_end = m_vertex_list.end();
         it != it_end;
         ++it)
    {
        Vertex const &vertex = *it;
        glTexCoord2fv(vertex.m_texture_coordinate.m);
        glVertex2fv(vertex.m_compound_vertex->m_coordinate.m);
    }

    glEnd();
}

void MapEditor2::Polygon::DrawMetrics () const
{
    ASSERT1(GL::MatrixMode() == GL_MODELVIEW);
    ASSERT1(!GL::IsTexture2dOn());

    glBegin(GL_LINE_LOOP);

    for (VertexListConstIterator it = m_vertex_list.begin(),
                                 it_end = m_vertex_list.end();
         it != it_end;
         ++it)
    {
        Vertex const &vertex = *it;
        glTexCoord2fv(vertex.m_texture_coordinate.m);
        glVertex2fv(vertex.m_compound_vertex->m_coordinate.m);
    }

    glEnd();
}

void MapEditor2::Polygon::ReassignVertices (
    Compound const *const owner_compound)
{
    ASSERT1(owner_compound != NULL);
    ASSERT1(!m_vertex_list.empty());
    for (VertexListIterator it = m_vertex_list.begin(),
                            it_end = m_vertex_list.end();
         it != it_end;
         ++it)
    {
        Vertex &vertex = *it;
        // switch the compound vertex reference to point to the corresponding
        // indexed vertex of owner_compound.
        vertex.m_compound_vertex =
            owner_compound->GetVertexInstance(vertex.m_compound_vertex->m_index);
    }    
}

bool MapEditor2::Polygon::ReplaceVertexWithNewVertex (
    Instance<CompoundVertex> *const vertex_to_replace,
    Instance<CompoundVertex> *const vertex_to_replace_with)
{
    ASSERT1(vertex_to_replace != NULL);
    ASSERT1(vertex_to_replace_with != NULL);

    for (VertexListIterator it = m_vertex_list.begin(),
                            it_end = m_vertex_list.end();
         it != it_end;
         ++it)
    {
        Vertex &vertex = *it;
        if (vertex.m_compound_vertex == vertex_to_replace)
        {
            vertex.m_compound_vertex = vertex_to_replace_with;
            return true;
        }   
    }

    return false;
}

void MapEditor2::Polygon::SetVertexSelectionStateFromSelectionOwnerPolygonCount (
    ObjectLayer *const object_layer)
{
    ASSERT1(object_layer != NULL);
    ASSERT1(!m_vertex_list.empty());
    for (VertexListIterator it = m_vertex_list.begin(),
                            it_end = m_vertex_list.end();
         it != it_end;
         ++it)
    {
        Vertex &vertex = *it;
        vertex.m_compound_vertex->SetSelectionStateFromSelectionOwnerPolygonCount(object_layer);
    }    
}

void MapEditor2::Polygon::IncrementSelectedOwnerPolygonCount (
    ObjectLayer *const object_layer)
{
    ASSERT1(object_layer != NULL);
    ASSERT1(!m_vertex_list.empty());
    for (VertexListIterator it = m_vertex_list.begin(),
                            it_end = m_vertex_list.end();
         it != it_end;
         ++it)
    {
        Vertex &vertex = *it;
        vertex.m_compound_vertex->IncrementSelectedOwnerPolygonCount(object_layer);
    }    
}

void MapEditor2::Polygon::DecrementSelectedOwnerPolygonCount (
    ObjectLayer *const object_layer)
{
    ASSERT1(object_layer != NULL);
    ASSERT1(!m_vertex_list.empty());
    for (VertexListIterator it = m_vertex_list.begin(),
                            it_end = m_vertex_list.end();
         it != it_end;
         ++it)
    {
        Vertex &vertex = *it;
        vertex.m_compound_vertex->DecrementSelectedOwnerPolygonCount(object_layer);
    }    
}

void MapEditor2::Polygon::SetIsSelected (
    bool const is_selected,
    ObjectLayer *const object_layer)
{
    if (m_is_selected != is_selected)
    {
        if (is_selected)
            AddToPolygonSelectionSet(object_layer);
        else
            RemoveFromPolygonSelectionSet(object_layer);
        ASSERT1(m_is_selected == is_selected);
    }
}

void MapEditor2::Polygon::ToggleIsSelected (
    ObjectLayer *const object_layer)
{
    DEBUG1_CODE(bool old_is_selected = m_is_selected);
    if (m_is_selected)
        RemoveFromPolygonSelectionSet(object_layer);
    else
        AddToPolygonSelectionSet(object_layer);
    ASSERT1(m_is_selected != old_is_selected);
}

void MapEditor2::Polygon::AddToPolygonSelectionSet (
    ObjectLayer *const object_layer)
{
    ASSERT1(object_layer != NULL);
    object_layer->AddPolygonToPolygonSelectionSet(this);
}

void MapEditor2::Polygon::RemoveFromPolygonSelectionSet (
    ObjectLayer *const object_layer)
{
    ASSERT1(object_layer != NULL);
    object_layer->RemovePolygonFromPolygonSelectionSet(this);
}

void MapEditor2::Polygon::Read (Serializer &serializer)
{
    ASSERT1(serializer.GetIODirection() == IOD_READ);
    ASSERT1(m_vertex_list.empty());
    ASSERT1(!m_texture.IsValid());
    ASSERT1(m_owner_compound != NULL);

    Uint32 vertex_count = serializer.ReadUint32();
    ASSERT1(vertex_count >= 3);
    for (Uint32 i = 0; i < vertex_count; ++i)
    {
        Vertex vertex;
        vertex.m_compound_vertex =
            m_owner_compound->GetVertexInstance(serializer.ReadUint32());
        serializer.ReadFloatVector2(&vertex.m_texture_coordinate);
        m_vertex_list.push_back(vertex);
    }
    m_texture =
        Singletons::ResourceLibrary().
            LoadFilename<GLTexture>(
                GLTexture::Create,
                serializer.ReadStdString());

    ASSERT1(IsCounterclockwise());
    ASSERT1(IsConvex());
    ASSERT1(!IsDegenerate());
}

void MapEditor2::Polygon::Write (Serializer &serializer) const
{
    ASSERT1(serializer.GetIODirection() == IOD_WRITE);
    ASSERT1(!m_vertex_list.empty());
    ASSERT1(m_texture.IsValid());
    ASSERT1(m_owner_compound != NULL);

    Uint32 vertex_count = m_vertex_list.size();
    ASSERT1(vertex_count >= 3);
    serializer.WriteUint32(vertex_count);
    for (VertexListConstIterator it = m_vertex_list.begin(),
                                 it_end = m_vertex_list.end();
         it != it_end;
         ++it)
    {
        Vertex const &vertex = *it;
        ASSERT1(vertex.m_compound_vertex->m_index != UINT32_UPPER_BOUND);
        serializer.WriteUint32(vertex.m_compound_vertex->m_index);
        serializer.WriteFloatVector2(vertex.m_texture_coordinate);
    }
    serializer.WriteStdString(m_texture.Filename());
}

} // end of namespace Xrb
