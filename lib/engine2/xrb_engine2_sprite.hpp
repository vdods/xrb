// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_sprite.hpp by Victor Dods, created 2004/08/07
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_SPRITE_HPP_)
#define _XRB_ENGINE2_SPRITE_HPP_

#include "xrb.hpp"

#include <string>

#include "xrb_engine2_object.hpp"
#include "xrb_gltexture.hpp"
#include "xrb_resourcelibrary.hpp"

namespace Xrb
{

namespace Engine2
{

// implements drawing functions, paired with the physicality of Object
class Sprite : public Object
{
public:

    virtual ~Sprite () { }

    // ///////////////////////////////////////////////////////////////////
    // public serialization functions
    // ///////////////////////////////////////////////////////////////////

    static Sprite *Create (std::string const &texture_path);
    // create an instance of this class by reading from the given Serializer
    static Sprite *Create (Serializer &serializer);
    // makes calls to WriteClassSpecific for this and all superclasses
    virtual void Write (Serializer &serializer) const;

    // ///////////////////////////////////////////////////////////////////
    // public Object interface methods
    // ///////////////////////////////////////////////////////////////////

    // draws this sprite
    virtual void Draw (Object::DrawData const &draw_data, Float alpha_mask) const;

    // ///////////////////////////////////////////////////////////////////
    // public accessors and modifiers
    // ///////////////////////////////////////////////////////////////////

    // returns the sprite texture
    Resource<GlTexture> const &GetTexture () const { return m_gltexture; }
    // returns true iff this is a "round" sprite (see comment
    // above m_is_round).
    bool IsRound () const { return m_is_round; }
    // returns the relative physical sizes (component-wise ratios of the
    // physical geometry scale factors over the visible geometry scale
    // factors).
    FloatVector2 const &PhysicalSizeRatios () const
    {
        return m_physical_size_ratios;
    }
    // returns the relative physical size -- only valid when both
    // components of the size ratios are equal.
    // see @c PhysicalSizeRatios
    Float PhysicalSizeRatio () const
    {
        ASSERT1(m_physical_size_ratios[Dim::X] == m_physical_size_ratios[Dim::Y]);
        return m_physical_size_ratios[Dim::X];
    }
    // returns the calculated scale factors for the physical geometry,
    // based upon the object's scale factors, and the physical size ratios.
    FloatVector2 PhysicalScaleFactors () const { return m_physical_size_ratios * ScaleFactors(); }
    // returns the calculated scale factor for the physical geometry,
    // based upon the object's scale factor, and the physical size ratio.
    Float PhysicalScaleFactor () const { return PhysicalSizeRatio() * ScaleFactor(); }

    // sets the physical size ratios
    void SetPhysicalSizeRatios (FloatVector2 const &physical_size_ratio);
    // sets the physical size ratios (both components to the given value)
    void SetPhysicalSizeRatio (Float physical_size_ratio);

    // resets the physical size ratios to [1, 1]
    void ResetPhysicalSizeRatios () { SetPhysicalSizeRatio(1.0f); }

protected:

    // protected constructor so you must use Create()
    Sprite (Resource<GlTexture> const &gltexture);

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
    // protected Object interface methods
    // ///////////////////////////////////////////////////////////////////

    virtual void CalculateRadius (QuadTreeType quad_tree_type) const;

    // ///////////////////////////////////////////////////////////////////
    // utility methods and other crap
    // ///////////////////////////////////////////////////////////////////

    void RenderGlTexture (Object::DrawData const &draw_data, Float alpha_mask, GlTexture const &gltexture) const;
    // copies the properties of the given object to this object
    void CloneProperties (Object const *object);

    // the texture to apply to this
    Resource<GlTexture> m_gltexture;
    // if true, indicates that the effective area of the sprite is the
    // circle/ellipse inscribed in the square/rectangle of the sprite
    // texture as it would appear in the world (this can be used for
    // planets and such).  if false, the effective area is the rectangular
    // area as indicated by the texture.  this property can be used or
    // ignored by the PhysicsHandler subclass as appropriate.
    bool m_is_round;
    // the component-wise ratios of the physical to the visible scales
    FloatVector2 m_physical_size_ratios;
}; // end of class Engine2::Sprite

} // end of namespace Engine2

} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_SPRITE_HPP_)
