// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_animatedsprite.hpp by Victor Dods, created 2009/09/16
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_ANIMATEDSPRITE_HPP_)
#define _XRB_ENGINE2_ANIMATEDSPRITE_HPP_

#include "xrb.hpp"

#include "xrb_animation.hpp"
#include "xrb_engine2_sprite.hpp"

namespace Xrb {
namespace Engine2 {

// same as Sprite, but instead of a fixed texture, an Animation is used.
class AnimatedSprite : public Sprite
{
public:

    virtual ~AnimatedSprite () { }

    // ///////////////////////////////////////////////////////////////////
    // public serialization functions
    // ///////////////////////////////////////////////////////////////////

    static AnimatedSprite *Create (std::string const &animation_sequence_path, Time current_time, Uint32 gltexture_flags = GlTexture::NONE);
    // create an instance of this class by reading from the given Serializer
    static AnimatedSprite *Create (Serializer &serializer);
    // makes calls to WriteClassSpecific for this and all superclasses
    virtual void Write (Serializer &serializer) const;

    // ///////////////////////////////////////////////////////////////////
    // public Object interface methods
    // ///////////////////////////////////////////////////////////////////

    // draws this sprite
    virtual void Draw (DrawData const &draw_data) const;
    // create a clone of this object (animated sprite)
    virtual Object *Clone () const;

    virtual Uint32 GlTextureAtlasHandle () const;

protected:

    // protected constructor so you must use Create()
    AnimatedSprite (Resource<Animation::Sequence> const &animation_sequence, Time current_time);

    // ///////////////////////////////////////////////////////////////////
    // protected serialization functions
    // ///////////////////////////////////////////////////////////////////

    // does the guts of serializing reading for this class (doesn't read
    // the object subtype)
    void ReadClassSpecific (Serializer &serializer);
    // does the guts of serializing writing for this class (doesn't write
    // the object subtype)
    void WriteClassSpecific (Serializer &serializer) const;

    // ///////////////////////////////////////////////////////////////////

    // copies the properties of the given animated sprite to this one
    void CloneProperties (AnimatedSprite const &animated_sprite);

    Animation m_animation;

private:

    // for use only in Clone()
    AnimatedSprite (Animation const &animation);
}; // end of class Engine2::Sprite

} // end of namespace Engine2
} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_ANIMATEDSPRITE_HPP_)
