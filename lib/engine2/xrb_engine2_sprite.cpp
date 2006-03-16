// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_sprite.cpp by Victor Dods, created 2004/08/07
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_sprite.h"

#include "xrb_render.h"
#include "xrb_serializer.h"

namespace Xrb
{

Engine2::Sprite *Engine2::Sprite::Create (std::string const &texture_filename)
{
    Resource<GLTexture> texture =
        Singletons::ResourceLibrary()->LoadFilename<GLTexture>(
            GLTexture::Create,
            texture_filename);
    if (!texture.GetIsValid())
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
    WriteSubType(serializer);
    // call WriteClassSpecific for this and all superclasses
    Object::WriteClassSpecific(serializer);
    Sprite::WriteClassSpecific(serializer);
}

Engine2::Object *Engine2::Sprite::CreateClone () const
{
    ASSERT1(m_texture.GetIsValid())
    
    Sprite *retval = new Sprite(m_texture);
    
    retval->Object::CloneProperties(this);
    retval->Sprite::CloneProperties(this);

    return static_cast<Object *>(retval);
}

void Engine2::Sprite::Draw (
    Engine2::Object::DrawData const &draw_data,
    Float const alpha_mask) const
{
    // don't do anything if there's no texture
    if (!m_texture.GetIsValid())
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
        GetTranslation()[Dim::X],
        GetTranslation()[Dim::Y],
        0.0f);
    glRotatef(GetAngle(), 0.0f, 0.0f, 1.0f);
    glScalef(
        GetScaleFactors()[Dim::X],
        GetScaleFactors()[Dim::Y],
        1.0f);

    // set the color mask
    Color color_mask(draw_data.GetRenderContext().GetMaskedColor(m_color_mask));
    color_mask[Dim::A] *= alpha_mask;
    glColor4fv(color_mask.m);

    // enable texture mapping and bind the sprite texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_texture->GetHandle());

    static FloatVector2 const s_tex_coord0(0.0f, 0.0f);
    static FloatVector2 const s_tex_coord1(0.0f, 1.0f);
    static FloatVector2 const s_tex_coord2(1.0f, 1.0f);
    static FloatVector2 const s_tex_coord3(1.0f, 0.0f);

    static FloatVector2 const s_vertex0(-1.0f,  1.0f);
    static FloatVector2 const s_vertex1(-1.0f, -1.0f);
    static FloatVector2 const s_vertex2( 1.0f, -1.0f);
    static FloatVector2 const s_vertex3( 1.0f,  1.0f);

    // draw the sprite quad
    glBegin(GL_QUADS);
        glTexCoord2fv(s_tex_coord0.m);
        glVertex2fv(s_vertex0.m);

        glTexCoord2fv(s_tex_coord1.m);
        glVertex2fv(s_vertex1.m);

        glTexCoord2fv(s_tex_coord2.m);
        glVertex2fv(s_vertex2.m);

        glTexCoord2fv(s_tex_coord3.m);
        glVertex2fv(s_vertex3.m);
    glEnd();

    glPopMatrix();
}

Engine2::Sprite::Sprite (Resource<GLTexture> const &texture)
    :
    Object()
{
    m_sub_type = Object::ST_SPRITE;
    m_texture = texture;
    m_is_round = true;
    m_color_mask = Color(1.0, 1.0, 1.0, 1.0);
}

void Engine2::Sprite::ReadClassSpecific (Serializer &serializer)
{
    // read in the guts
    m_texture =
        Singletons::ResourceLibrary()->LoadFilename<GLTexture>(
            GLTexture::Create,
            serializer.ReadStdString());
    m_is_round = serializer.ReadBool();
    m_color_mask = serializer.ReadColor();
    IndicateRadiusNeedsToBeRecalculated();

    ASSERT1(m_texture.GetIsValid())
}

void Engine2::Sprite::WriteClassSpecific (Serializer &serializer) const
{
    ASSERT1(m_texture.GetIsValid())

    // write out the guts
    serializer.WriteStdString(m_texture.GetFilename());
    serializer.WriteBool(m_is_round);
    serializer.WriteColor(m_color_mask);
}

void Engine2::Sprite::CalculateRadius () const
{
    if (m_is_round)
        m_radius = Max(GetScaleFactors()[Dim::X], GetScaleFactors()[Dim::Y]);
    else
        m_radius = GetScaleFactors().GetLength();
}

void Engine2::Sprite::CloneProperties (Engine2::Object const *const object)
{
    Sprite const *sprite = dynamic_cast<Sprite const *>(object);
    ASSERT1(sprite != NULL)

    m_texture = sprite->m_texture;
    m_is_round = sprite->m_is_round;
    m_color_mask = sprite->m_color_mask;
    IndicateRadiusNeedsToBeRecalculated();
}

} // end of namespace Xrb
