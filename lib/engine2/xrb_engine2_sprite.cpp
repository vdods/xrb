// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_sprite.cpp by Victor Dods, created 2004/08/07
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_sprite.hpp"

#include "xrb_gl.hpp"
#include "xrb_render.hpp"
#include "xrb_serializer.hpp"

namespace Xrb
{

Engine2::Sprite *Engine2::Sprite::Create (std::string const &texture_path)
{
    Resource<GLTexture> texture =
        Singleton::ResourceLibrary().LoadPath<GLTexture>(
            GLTexture::Create,
            texture_path);
    if (!texture.IsValid())
        return NULL;

    return new Sprite(texture);
}

Engine2::Sprite *Engine2::Sprite::Create (Serializer &serializer)
{
    Sprite *retval = new Sprite(Resource<GLTexture>());

    // call ReadClassSpecific for this and all superclasses
    retval->Object::ReadClassSpecific(serializer);
    retval->Sprite::ReadClassSpecific(serializer);

    return retval;
}

void Engine2::Sprite::Write (Serializer &serializer) const
{
    WriteObjectType(serializer);
    // call WriteClassSpecific for this and all superclasses
    Object::WriteClassSpecific(serializer);
    Sprite::WriteClassSpecific(serializer);
}

void Engine2::Sprite::Draw (
    Engine2::Object::DrawData const &draw_data,
    Float const alpha_mask) const
{
    if (draw_data.GetRenderContext().MaskAndBiasWouldResultInNoOp())
        return;

    // don't do anything if there's no texture
    if (!m_texture.IsValid())
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

    // calculate the bias color
    Color bias_color(draw_data.GetRenderContext().BlendedBiasColor(BiasColor()));
    // calculate the color mask
    Color color_mask(draw_data.GetRenderContext().MaskedColor(ColorMask()));
    color_mask[Dim::A] *= alpha_mask;

    Render::SetupTextureUnits(m_texture->Handle(), color_mask, bias_color);

    // draw the sprite with a triangle strip using glDrawArrays
    {
        static FloatVector2 const s_texture_coord_array[4] =
        {
            FloatVector2(0.0f, 1.0f),
            FloatVector2(1.0f, 1.0f),
            FloatVector2(0.0f, 0.0f),
            FloatVector2(1.0f, 0.0f)
        };
        static FloatVector2 const s_vertex_array[4] =
        {
            FloatVector2(-1.0f, -1.0f),
            FloatVector2( 1.0f, -1.0f),
            FloatVector2(-1.0f,  1.0f),
            FloatVector2( 1.0f,  1.0f)
        };

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(2, GL_FLOAT, 0, s_vertex_array);

        glClientActiveTexture(GL_TEXTURE0);
        glTexCoordPointer(2, GL_FLOAT, 0, s_texture_coord_array);
        glClientActiveTexture(GL_TEXTURE1);
        glTexCoordPointer(2, GL_FLOAT, 0, s_texture_coord_array);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // this seems to be unnecessary, but there's probably a good reason for it.
        glDisableClientState(GL_VERTEX_ARRAY); 
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    glPopMatrix();
}

void Engine2::Sprite::SetPhysicalSizeRatios (FloatVector2 const &physical_size_ratios)
{
    ASSERT1(physical_size_ratios[Dim::X] > 0.0f);
    ASSERT1(physical_size_ratios[Dim::Y] > 0.0f);
    m_physical_size_ratios = physical_size_ratios;
    IndicateRadiiNeedToBeRecalculated();
}

void Engine2::Sprite::SetPhysicalSizeRatio (Float const physical_size_ratio)
{
    m_physical_size_ratios.SetComponents(physical_size_ratio, physical_size_ratio);
    IndicateRadiiNeedToBeRecalculated();
}

Engine2::Sprite::Sprite (Resource<GLTexture> const &texture)
    :
    Object(OT_SPRITE),
    m_physical_size_ratios(1.0f, 1.0f)
{
    m_texture = texture;
    m_is_round = true;
}

void Engine2::Sprite::ReadClassSpecific (Serializer &serializer)
{
    // read in the guts
    m_texture =
        Singleton::ResourceLibrary().LoadPath<GLTexture>(
            GLTexture::Create,
            serializer.ReadStdString());
    m_is_round = serializer.ReadBool();
    serializer.ReadFloatVector2(&m_physical_size_ratios);
    IndicateRadiiNeedToBeRecalculated();

    ASSERT1(m_texture.IsValid());
}

void Engine2::Sprite::WriteClassSpecific (Serializer &serializer) const
{
    ASSERT1(m_texture.IsValid());

    // write out the guts
    serializer.WriteStdString(m_texture.Path());
    serializer.WriteBool(m_is_round);
    serializer.WriteFloatVector2(m_physical_size_ratios);
}

void Engine2::Sprite::CalculateRadius (QuadTreeType const quad_tree_type) const
{
    switch (quad_tree_type)
    {
        case QTT_VISIBILITY:
            if (m_is_round)
                m_radius[quad_tree_type] = Max(ScaleFactors()[Dim::X], ScaleFactors()[Dim::Y]);
            else
                m_radius[quad_tree_type] = ScaleFactors().Length();
            break;

        case QTT_PHYSICS_HANDLER:
            if (m_is_round)
                m_radius[quad_tree_type] =
                    Max(ScaleFactors()[Dim::X] * m_physical_size_ratios[Dim::X],
                        ScaleFactors()[Dim::Y] * m_physical_size_ratios[Dim::Y]);
            else
                m_radius[quad_tree_type] = (ScaleFactors() * m_physical_size_ratios).Length();
            break;

        default:
            ASSERT0(false && "Invalid QuadTreeType");
            break;
    }
}

void Engine2::Sprite::CloneProperties (Engine2::Object const *const object)
{
    ASSERT1(object->GetObjectType() == OT_SPRITE);
    Sprite const *sprite = DStaticCast<Sprite const *>(object);
    ASSERT1(sprite != NULL);

    m_texture = sprite->m_texture;
    m_is_round = sprite->m_is_round;
    m_physical_size_ratios = sprite->m_physical_size_ratios;
    IndicateRadiiNeedToBeRecalculated();
}

} // end of namespace Xrb
