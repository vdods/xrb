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

#include "xrb_engine2_animatedsprite.hpp"
#include "xrb_gl.hpp"
#include "xrb_gltextureatlas.hpp"
#include "xrb_rendercontext.hpp"
#include "xrb_serializer.hpp"

namespace Xrb {
namespace Engine2 {

Sprite *Sprite::Create (std::string const &texture_path, Uint32 gltexture_flags)
{
    return new Sprite(GlTexture::Load(texture_path, gltexture_flags));
}

Sprite *Sprite::Create (std::string const &asset_path, Float current_time, Uint32 gltexture_flags)
{
    if (asset_path.rfind(".anim") != std::string::npos)
        return AnimatedSprite::Create(asset_path, current_time, gltexture_flags);
    else
        return Sprite::Create(asset_path, gltexture_flags);
}

Sprite *Sprite::Create (Serializer &serializer)
{
    Sprite *retval = new Sprite(Resource<GlTexture>());

    // call ReadClassSpecific for this and all superclasses
    retval->Object::ReadClassSpecific(serializer);
    retval->Sprite::ReadClassSpecific(serializer);

    return retval;
}

void Sprite::Write (Serializer &serializer) const
{
    WriteObjectType(serializer);
    // call WriteClassSpecific for this and all superclasses
    Object::WriteClassSpecific(serializer);
    Sprite::WriteClassSpecific(serializer);
}

void Sprite::Draw (DrawData const &draw_data) const
{
    if (draw_data.m_render_context.MaskAndBiasWouldResultInNoOp())
        return;

    // don't do anything if there's no texture
    if (!m_gltexture.IsValid())
        return;

    RenderGlTexture(draw_data, **m_gltexture);
}

Object *Sprite::Clone () const
{
    Sprite *retval = new Sprite(m_gltexture);
    retval->CloneProperties(*this);
    return retval;
}

Uint32 Sprite::GlTextureAtlasHandle () const
{
    if (!m_gltexture.IsValid())
        return 0; // sentinel value used by GL for 'no texture'
    else
        return m_gltexture->Atlas().Handle();
}

void Sprite::SetPhysicalSizeRatios (FloatVector2 const &physical_size_ratios)
{
    ASSERT1(physical_size_ratios[Dim::X] > 0.0f);
    ASSERT1(physical_size_ratios[Dim::Y] > 0.0f);
    m_physical_size_ratios = physical_size_ratios;
    IndicateRadiiNeedToBeRecalculated();
}

void Sprite::SetPhysicalSizeRatio (Float const physical_size_ratio)
{
    m_physical_size_ratios.SetComponents(physical_size_ratio, physical_size_ratio);
    IndicateRadiiNeedToBeRecalculated();
}

Sprite::Sprite (Resource<GlTexture> const &gltexture)
    :
    Object(OT_SPRITE),
    m_gltexture(gltexture),
    m_is_round(true),
    m_physical_size_ratios(1.0f, 1.0f)
{ }

void Sprite::ReadClassSpecific (Serializer &serializer)
{
    // read in the guts
    m_gltexture = GlTexture::Load(serializer.ReadAggregate<std::string>());
    serializer.Read<bool>(m_is_round);
    serializer.ReadAggregate<FloatVector2>(m_physical_size_ratios);
    IndicateRadiiNeedToBeRecalculated();

    ASSERT1(m_gltexture.IsValid());
}

void Sprite::WriteClassSpecific (Serializer &serializer) const
{
    ASSERT1(m_gltexture.IsValid());

    // write out the guts
    serializer.WriteAggregate<std::string>(m_gltexture.LoadParameters<GlTexture::LoadParameters>().Path());
    serializer.Write<bool>(m_is_round);
    serializer.WriteAggregate<FloatVector2>(m_physical_size_ratios);
}

void Sprite::CalculateRadius (QuadTreeType const quad_tree_type) const
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

// it's (theoretically) faster to use software transform
#define USE_SOFTWARE_TRANSFORM 1

void Sprite::RenderGlTexture (DrawData const &draw_data, GlTexture const &gltexture) const
{
#if !USE_SOFTWARE_TRANSFORM
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
#endif

    Singleton::Gl().SetupTextureUnits(gltexture, draw_data.m_color_mask, draw_data.m_color_bias);

    // draw the sprite with a triangle strip using glDrawArrays
    {
        Gl::EnableClientState(GL_VERTEX_ARRAY);
        ASSERT1(Gl::ClientActiveTexture() == GL_TEXTURE0);
        Gl::EnableClientState(GL_TEXTURE_COORD_ARRAY);

#if !USE_SOFTWARE_TRANSFORM
        static Sint16 const s_vertex_array[8] =
        {
            -1, -1,
             1, -1,
            -1,  1,
             1,  1
        };

        glVertexPointer(2, GL_SHORT, 0, s_vertex_array);
#else
        FloatVector2 vertex_array[4] =
        {
            FloatVector2(-1, -1),
            FloatVector2( 1, -1),
            FloatVector2(-1,  1),
            FloatVector2( 1,  1)
        };

        for (Uint32 i = 0; i < 4; ++i)
            vertex_array[i] = Transformation() * vertex_array[i];

        glVertexPointer(2, GL_FLOAT, 0, vertex_array);
#endif
        glTexCoordPointer(2, GL_SHORT, 0, gltexture.TextureCoordinateArray());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

#if !USE_SOFTWARE_TRANSFORM
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
#endif
}

void Sprite::CloneProperties (Sprite const &sprite)
{
    Object::CloneProperties(sprite);

    m_gltexture = sprite.m_gltexture;
    m_is_round = sprite.m_is_round;
    m_physical_size_ratios = sprite.m_physical_size_ratios;
    IndicateRadiiNeedToBeRecalculated();
}

} // end of namespace Engine2
} // end of namespace Xrb
