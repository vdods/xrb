// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_compound.cpp by Victor Dods, created 2005/06/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_compound.hpp"

#include "xrb_engine2_polygon.hpp"
#include "xrb_gl.hpp"
#include "xrb_rendercontext.hpp"

namespace Xrb {
namespace Engine2 {

Compound::~Compound ()
{
    DeleteArray(m_vertex_array);
    DeleteArray(m_polygon_array);
}

Compound *Compound::Create (Serializer &serializer)
{
    Compound *retval = new Compound();

    // call ReadClassSpecific for this and all superclasses
    retval->Object::ReadClassSpecific(serializer);
    retval->Compound::ReadClassSpecific(serializer);

    return retval;
}

void Compound::Write (Serializer &serializer) const
{
    WriteObjectType(serializer);
    // call WriteClassSpecific for this and all superclasses
    Object::WriteClassSpecific(serializer);
    Compound::WriteClassSpecific(serializer);
}

void Compound::Draw (DrawData const &draw_data) const
{
    if (draw_data.m_render_context.MaskAndBiasWouldResultInNoOp())
        return;

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
        ZDepth());
    glRotatef(Angle(), 0.0f, 0.0f, 1.0f);
    glScalef(
        ScaleFactors()[Dim::X],
        ScaleFactors()[Dim::Y],
        1.0f);

    // the opaque white texture is just a dummy.  the real texture will be bound later
    Singleton::Gl().SetupTextureUnits(Singleton::Gl().GlTexture_OpaqueWhite(), draw_data.m_color_mask, draw_data.m_color_bias);

    // switch back to texture unit 0 for Polygon's texture binding
    Singleton::Gl().ActiveTexture(GL_TEXTURE0);

    for (Uint32 i = 0; i < m_polygon_count; ++i)
        m_polygon_array[i].Draw();

    glPopMatrix();
}

Object *Compound::Clone () const
{
    Compound *retval = new Compound();
    retval->CloneProperties(*this);
    return retval;
}

Compound::Compound ()
    :
    Object(OT_COMPOUND)
{
    m_vertex_count = 0;
    m_vertex_array = NULL;
    m_polygon_count = 0;
    m_polygon_array = NULL;
}

void Compound::ReadClassSpecific (Serializer &serializer)
{
    ASSERT1(m_vertex_count == 0);
    ASSERT1(m_vertex_array == NULL);

    serializer.Read<Uint32>(m_vertex_count);
    ASSERT1(m_vertex_count > 0);
    m_vertex_array = new FloatVector2[m_vertex_count];
    for (Uint32 i = 0; i < m_vertex_count; ++i)
        serializer.ReadAggregate<FloatVector2>(m_vertex_array[i]);

    m_polygon_count = serializer.Read<Uint32>();
    ASSERT1(m_polygon_count > 0);
    m_polygon_array = new Polygon[m_polygon_count];
    for (Uint32 i = 0; i < m_polygon_count; ++i)
        m_polygon_array[i].Read(serializer, m_vertex_array);
}

void Compound::WriteClassSpecific (Serializer &serializer) const
{
    ASSERT1(serializer.IsWritable());
    ASSERT1(m_vertex_count > 0);
    ASSERT1(m_vertex_array != NULL);
    ASSERT1(m_polygon_count > 0);
    ASSERT1(m_polygon_array != NULL);

    serializer.Write<Uint32>(m_vertex_count);
    for (Uint32 i = 0; i < m_vertex_count; ++i)
        serializer.WriteAggregate<FloatVector2>(m_vertex_array[i]);

    serializer.Write<Uint32>(m_polygon_count);
    for (Uint32 i = 0; i < m_polygon_count; ++i)
        m_polygon_array[i].Write(serializer, m_vertex_array);
}

void Compound::CalculateRadius (QuadTreeType quad_tree_type) const
{
    ASSERT1(quad_tree_type == QTT_VISIBILITY || quad_tree_type == QTT_PHYSICS_HANDLER);
    // TODO: real code that checks for collision-only polygons - for now
    // just calculate the radius the same way for visible and physical

    // get the distance from the origin to the furthest vertex
    m_radius[quad_tree_type] = 0.0f;
    for (Uint32 i = 0; i < m_vertex_count; ++i)
    {
        m_radius[quad_tree_type] = Max(
            m_radius[quad_tree_type],
            (Transformation() * m_vertex_array[i] -
             Transformation() * FloatVector2::ms_zero).Length());
    }
}

void Compound::CloneProperties (Compound const &compound)
{
    Object::CloneProperties(compound);

    ASSERT1(m_vertex_count == 0);
    ASSERT1(m_vertex_array == NULL);

    delete[] m_vertex_array;
    m_vertex_count = compound.m_vertex_count;
    m_vertex_array = new FloatVector2[m_vertex_count];
    for (Uint32 i = 0; i < m_vertex_count; ++i)
        m_vertex_array[i] = compound.m_vertex_array[i];

    delete[] m_polygon_array;
    m_polygon_count = compound.m_polygon_count;
    m_polygon_array = new Polygon[m_polygon_count];
    for (Uint32 i = 0; i < m_polygon_count; ++i)
        m_polygon_array[i].CloneProperties(
            &compound.m_polygon_array[i],
            compound.m_vertex_array,
            m_vertex_array);
}

} // end of namespace Engine2
} // end of namespace Xrb

