// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_animatedsprite.cpp by Victor Dods, created 2004/08/07
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_animatedsprite.hpp"

#include "xrb_engine2_world.hpp"
#include "xrb_gltextureatlas.hpp"
#include "xrb_rendercontext.hpp"

namespace Xrb {
namespace Engine2 {

AnimatedSprite *AnimatedSprite::Create (std::string const &animation_sequence_path, Float current_time, Uint32 gltexture_flags)
{
    Resource<Animation::Sequence> animation_sequence = Animation::Sequence::Load(animation_sequence_path, gltexture_flags);
    return new AnimatedSprite(animation_sequence, current_time);
}

AnimatedSprite *AnimatedSprite::Create (Serializer &serializer)
{
    AnimatedSprite *retval = new AnimatedSprite(Resource<Animation::Sequence>(), 0.0f);

    // call ReadClassSpecific for this and all superclasses
    retval->Object::ReadClassSpecific(serializer);
    retval->Sprite::ReadClassSpecific(serializer);
    retval->AnimatedSprite::ReadClassSpecific(serializer);

    return retval;
}

void AnimatedSprite::Write (Serializer &serializer) const
{
    WriteObjectType(serializer);
    // call WriteClassSpecific for this and all superclasses
    Object::WriteClassSpecific(serializer);
    Sprite::WriteClassSpecific(serializer);
    AnimatedSprite::WriteClassSpecific(serializer);
}

void AnimatedSprite::Draw (DrawData const &draw_data) const
{
    if (draw_data.m_render_context.MaskAndBiasWouldResultInNoOp())
        return;

    // don't do anything if there's no texture
    if (!m_animation.GetSequence().IsValid())
        return;

    RenderGlTexture(draw_data, m_animation.Frame(GetWorld()->MostRecentFrameTime()));
}

Object *AnimatedSprite::Clone () const
{
    AnimatedSprite *retval = new AnimatedSprite(m_animation);
    retval->CloneProperties(*this);
    return retval;
}

Uint32 AnimatedSprite::GlTextureAtlasHandle () const
{
    return m_animation.Frame(GetWorld()->MostRecentFrameTime()).Atlas().Handle();
}

AnimatedSprite::AnimatedSprite (Resource<Animation::Sequence> const &animation_sequence, Float current_time)
    :
    Sprite(Resource<GlTexture>()), // invalid texture, since we don't use it
    m_animation(animation_sequence, animation_sequence->DefaultType(), animation_sequence->DefaultDuration(), current_time)
{ }

void AnimatedSprite::ReadClassSpecific (Serializer &serializer)
{
    // TODO
    ASSERT1(false);
}

void AnimatedSprite::WriteClassSpecific (Serializer &serializer) const
{
    // TODO
    ASSERT1(false);
}

void AnimatedSprite::CloneProperties (AnimatedSprite const &animated_sprite)
{
    Sprite::CloneProperties(animated_sprite);

    m_animation = animated_sprite.m_animation;
}

AnimatedSprite::AnimatedSprite (Animation const &animation)
    :
    Sprite(Resource<GlTexture>()), // invalid texture, since we don't use it
    m_animation(animation)
{ }

} // end of namespace Engine2
} // end of namespace Xrb
