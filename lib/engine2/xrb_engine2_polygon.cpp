// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_polygon.cpp by Victor Dods, created 2005/06/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_polygon.hpp"

#include "xrb_gl.hpp"

namespace Xrb
{

Float Engine2::Polygon::Area () const
{
    Float area = 0.0f;

    if (m_vertex_count < 3)
        return area;

    for (Uint32 i = 0; i < m_vertex_count; ++i)
        area += GetVertex(i) & GetVertex((i + 1) % m_vertex_count);

    return 0.5f * area;
}

bool Engine2::Polygon::IsCounterclockwise () const
{
    // trivially true
    if (m_vertex_count < 3)
        return true;

    // all triangles must have positive normal
    for (Uint32 i = 0; i < m_vertex_count; ++i)
    {
        Float normal =
            (GetVertex((i + 1) % m_vertex_count) - GetVertex(i)) &
            (GetVertex((i + 2) % m_vertex_count) - GetVertex(i));
        if (normal < 0.0f)
            return false;
    }

    // if it passed all the tests, it's counterclockwise.
    return true;
}

bool Engine2::Polygon::IsConvex () const
{
    // trivially true
    if (m_vertex_count <= 3)
        return true;

    // the rest of the triangles must have the same winding
    // as the first one.
    Float normal =
        (GetVertex(1) - GetVertex(0)) & (GetVertex(2) - GetVertex(0));
    bool triangle_is_counterclockwise = normal >= 0.0f;

    for (Uint32 i = 1; i < m_vertex_count; ++i)
    {
        normal =
            (GetVertex((i + 1) % m_vertex_count) - GetVertex(i)) &
            (GetVertex((i + 2) % m_vertex_count) - GetVertex(i));
        // if the winding on one of the triangles differs, then
        // it's not a convex polygon.
        if (triangle_is_counterclockwise && normal < 0.0f)
            return false;
    }

    // if it passed all the tests, it's convex.
    return true;
}

void Engine2::Polygon::Draw () const
{
    ASSERT1(Gl::Integer(GL_MATRIX_MODE) == GL_MODELVIEW);
    ASSERT1(Gl::Boolean(GL_TEXTURE_2D));

    glBindTexture(GL_TEXTURE_2D, m_texture->Handle());

    // TODO: use small static array instead of new'ing
    {
        FloatVector2 *vertex_array = new FloatVector2[m_vertex_count];
        FloatVector2 *texture_coord_array = new FloatVector2[m_vertex_count];

        for (Uint32 i = 0; i < m_vertex_count; ++i)
        {
            texture_coord_array[i] = TextureCoordinate(i);
            vertex_array[i] = GetVertex(i);
        }

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(2, GL_FLOAT, 0, vertex_array);

        glClientActiveTexture(GL_TEXTURE0);
        glTexCoordPointer(2, GL_FLOAT, 0, texture_coord_array);
        glClientActiveTexture(GL_TEXTURE1);
        glTexCoordPointer(2, GL_FLOAT, 0, texture_coord_array);

        glDrawArrays(GL_TRIANGLE_FAN, 0, m_vertex_count);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

        delete[] vertex_array;
    }
}

void Engine2::Polygon::CloneProperties (
    Polygon const *const source_polygon,
    FloatVector2 *const source_vertex_array,
    FloatVector2 *const destination_vertex_array)
{
    ASSERT1(m_vertex_count == 0);
    ASSERT1(m_vertex_array == NULL);
    ASSERT1(!m_texture.IsValid());

    ASSERT1(source_polygon != NULL);
    ASSERT1(source_polygon->m_vertex_count >= 3);

    ASSERT1(destination_vertex_array != NULL);
    ASSERT1(source_vertex_array != NULL);

    m_vertex_count = source_polygon->m_vertex_count;
    m_vertex_array = new Vertex[m_vertex_count];
    for (Uint32 i = 0; i < m_vertex_count; ++i)
    {
        m_vertex_array[i] = source_polygon->m_vertex_array[i];
        m_vertex_array[i].m_model_coordinate +=
            destination_vertex_array - source_vertex_array;
    }

    m_texture = source_polygon->m_texture;
    m_area = source_polygon->m_area;

    ASSERT1(IsCounterclockwise());
    ASSERT1(IsConvex());
    ASSERT1(!IsDegenerate());
    ASSERT1(m_texture.IsValid());
}

void Engine2::Polygon::Read (
    Serializer &serializer,
    FloatVector2 *compound_vertex_array)
{
    ASSERT1(serializer.Direction() == IOD_READ);
    ASSERT1(compound_vertex_array != NULL);

    ASSERT1(m_vertex_count == 0);
    ASSERT1(m_vertex_array == NULL);
    ASSERT1(!m_texture.IsValid());

    serializer.Read<Uint32>(m_vertex_count);
    ASSERT1(m_vertex_count >= 3);
    m_vertex_array = new Vertex[m_vertex_count];
    for (Uint32 i = 0; i < m_vertex_count; ++i)
    {
        m_vertex_array[i].m_model_coordinate = compound_vertex_array + serializer.Read<Uint32>();
        serializer.ReadAggregate<FloatVector2>(m_vertex_array[i].m_texture_coordinate);
    }
    m_texture =
        Singleton::ResourceLibrary().
            LoadPath<GLTexture>(GLTexture::Create, serializer.ReadAggregate<std::string>());
    m_area = Area();

    ASSERT1(IsCounterclockwise());
    ASSERT1(IsConvex());
    ASSERT1(!IsDegenerate());
    ASSERT1(m_texture.IsValid());
}

void Engine2::Polygon::Write (
    Serializer &serializer,
    FloatVector2 const *const compound_vertex_array) const
{
    ASSERT1(serializer.Direction() == IOD_WRITE);
    ASSERT1(compound_vertex_array != NULL);
    ASSERT1(m_vertex_count >= 3);

    serializer.Write<Uint32>(m_vertex_count);
    for (Uint32 i = 0; i < m_vertex_count; ++i)
    {
        serializer.Write<Uint32>(m_vertex_array[i].m_model_coordinate - compound_vertex_array);
        serializer.WriteAggregate<FloatVector2>(m_vertex_array[i].m_texture_coordinate);
    }
    serializer.WriteAggregate<std::string>(m_texture.Path());
}

} // end of namespace Xrb
