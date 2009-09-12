// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_polygon.hpp by Victor Dods, created 2005/06/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_POLYGON_HPP_)
#define _XRB_ENGINE2_POLYGON_HPP_

#include "xrb.hpp"

#include <vector>

#include "xrb_gltexture.hpp"
#include "xrb_resourcelibrary.hpp"
#include "xrb_serializer.hpp"
#include "xrb_vector.hpp"

namespace Xrb
{

namespace Engine2
{

struct Polygon
{
    struct Vertex
    {
        FloatVector2 *m_model_coordinate;
        FloatVector2 m_texture_coordinate;
    }; // end of struct Polygon::Vertex

    Uint32 m_vertex_count;
    // list of vertices of the polygon (ordered counterclockwise)
    Vertex *m_vertex_array;
    // the texture to draw the polygon with (an invalid Resource indicates
    // that no texture will be drawn and the polygon is invisible).
    Resource<GlTexture> m_texture;
    // the area of this polygon
    Float m_area;

    inline Polygon ()
    {
        m_vertex_count = 0;
        m_vertex_array = NULL;
        m_area = 0.0f;
    }

    inline FloatVector2 const &GetVertex (Uint32 const index) const
    {
        ASSERT1(index < m_vertex_count);
        return *(m_vertex_array[index].m_model_coordinate);
    }
    inline FloatVector2 const &TextureCoordinate (Uint32 const index) const
    {
        ASSERT1(index < m_vertex_count);
        return m_vertex_array[index].m_texture_coordinate;
    }

    Float Area () const;
    bool IsCounterclockwise () const;
    bool IsConvex () const;
    inline bool IsDegenerate () const
    {
        return m_area == 0.0f;
    }

    // draws this Polygon -- it is assumed that the GL modelview matrix
    // and the color mask is already correctly set up.
    void Draw () const;
    // clones the properties of the given polygon to this one.  the vertex
    // arrays of the source and destination compounds are also given, so
    // that the vertex pointers can be properly transferred to this
    // polygon.
    void CloneProperties (
        Polygon const *source_polygon,
        FloatVector2 *source_vertex_array,
        FloatVector2 *destination_vertex_array);
    // reads from a serializer and fills the contents of this polygon
    void Read (
        Serializer &serializer,
        FloatVector2 *compound_vertex_array);
    // writes this polygon to the serializer.  a pointer to the owner
    // compound's vertex array is necessary so that the indices of
    // the polygon's vertices can be written.
    void Write (
        Serializer &serializer,
        FloatVector2 const *compound_vertex_array) const;

}; // end of struct Engine2::Polygon

} // end of namespace Engine2

} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_POLYGON_HPP_)

