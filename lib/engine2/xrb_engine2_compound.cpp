// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_compound.cpp by Victor Dods, created 2005/06/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_compound.h"

#include "xrb_engine2_polygon.h"
#include "xrb_rendercontext.h"

namespace Xrb
{

Engine2::Compound::~Compound ()
{
    DeleteArray(m_vertex_array);
    DeleteArray(m_polygon_array);
}
/*
Engine2::Compound *Engine2::Compound::Create (std::string const &compound_filename)
{
}
*/

Engine2::Compound *Engine2::Compound::Create (Serializer &serializer)
{
    Compound *retval = new Compound();

    // call ReadClassSpecific for this and all superclasses
    retval->Object::ReadClassSpecific(serializer);
    retval->Compound::ReadClassSpecific(serializer);

    return retval;
}

void Engine2::Compound::Write (Serializer &serializer) const
{
    WriteObjectType(serializer);
    // call WriteClassSpecific for this and all superclasses
    Object::WriteClassSpecific(serializer);
    Compound::WriteClassSpecific(serializer);
}

void Engine2::Compound::Draw (
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
        GetTranslation()[Dim::X],
        GetTranslation()[Dim::Y],
        GetZDepth());
    glRotatef(GetAngle(), 0.0f, 0.0f, 1.0f);
    glScalef(
        GetScaleFactors()[Dim::X],
        GetScaleFactors()[Dim::Y],
        1.0f);

    // set the color mask
    Color color_mask(draw_data.GetRenderContext().GetMaskedColor(GetColorMask()));
    color_mask[Dim::A] *= alpha_mask;
    glColor4fv(color_mask.m);

    // enable texture mapping (because the polygon drawing
    // function requires it)
    glEnable(GL_TEXTURE_2D);

    for (Uint32 i = 0; i < m_polygon_count; ++i)
        m_polygon_array[i].Draw();

    glPopMatrix();
}

Engine2::Compound::Compound ()
    :
    Engine2::Object(OT_COMPOUND)
{
    m_vertex_count = 0;
    m_vertex_array = NULL;
    m_polygon_count = 0;
    m_polygon_array = NULL;
}

void Engine2::Compound::ReadClassSpecific (Serializer &serializer)
{
    ASSERT1(serializer.GetIODirection() == IOD_READ)
    ASSERT1(m_vertex_count == 0)
    ASSERT1(m_vertex_array == NULL)

    m_vertex_count = serializer.ReadUint32();
    ASSERT1(m_vertex_count > 0)
    m_vertex_array = new FloatVector2[m_vertex_count];
    for (Uint32 i = 0; i < m_vertex_count; ++i)
        serializer.ReadFloatVector2(&m_vertex_array[i]);

    m_polygon_count = serializer.ReadUint32();
    ASSERT1(m_polygon_count > 0)
    m_polygon_array = new Polygon[m_polygon_count];
    for (Uint32 i = 0; i < m_polygon_count; ++i)
        m_polygon_array[i].Read(serializer, m_vertex_array);
}

void Engine2::Compound::WriteClassSpecific (Serializer &serializer) const
{
    ASSERT1(serializer.GetIODirection() == IOD_WRITE)
    ASSERT1(m_vertex_count > 0)
    ASSERT1(m_vertex_array != NULL)
    ASSERT1(m_polygon_count > 0)
    ASSERT1(m_polygon_array != NULL)

    serializer.WriteUint32(m_vertex_count);
    for (Uint32 i = 0; i < m_vertex_count; ++i)
        serializer.WriteFloatVector2(m_vertex_array[i]);

    serializer.WriteUint32(m_polygon_count);
    for (Uint32 i = 0; i < m_polygon_count; ++i)
        m_polygon_array[i].Write(serializer, m_vertex_array);
}

void Engine2::Compound::CalculateRadius (QuadTreeType quad_tree_type) const
{
    ASSERT1(QTT_COUNT == 2)
    // TODO: real code that checks for collision-only polygons - for now
    // just calculate the radius the same way for visible and physical

    // get the distance from the origin to the furthest vertex
    m_radius[quad_tree_type] = 0.0f;
    for (Uint32 i = 0; i < m_vertex_count; ++i)
    {
        m_radius[quad_tree_type] = Max(
            m_radius[quad_tree_type],
            (GetTransformation() * m_vertex_array[i] -
             GetTransformation() * FloatVector2::ms_zero).GetLength());
    }
}

void Engine2::Compound::CloneProperties (Engine2::Object const *const object)
{
    ASSERT1(object->GetObjectType() == OT_COMPOUND)
    Compound const *compound = DStaticCast<Compound const *>(object);
    ASSERT1(compound != NULL)

    ASSERT1(m_vertex_count == 0)
    ASSERT1(m_vertex_array == NULL)

    m_vertex_count = compound->m_vertex_count;
    m_vertex_array = new FloatVector2[m_vertex_count];
    for (Uint32 i = 0; i < m_vertex_count; ++i)
        m_vertex_array[i] = compound->m_vertex_array[i];

    m_polygon_count = compound->m_polygon_count;
    m_polygon_array = new Polygon[m_polygon_count];
    for (Uint32 i = 0; i < m_polygon_count; ++i)
        m_polygon_array[i].CloneProperties(
            &compound->m_polygon_array[i],
            compound->m_vertex_array,
            m_vertex_array);
}

} // end of namespace Xrb

