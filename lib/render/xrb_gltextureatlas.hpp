// ///////////////////////////////////////////////////////////////////////////
// xrb_gltextureatlas.hpp by Victor Dods, created 2009/09/11
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_GLTEXTUREATLAS_HPP_)
#define _XRB_GLTEXTUREATLAS_HPP_

#include "xrb.hpp"

#include <set>

#include "xrb_gl.hpp"
#include "xrb_screencoord.hpp"

namespace Xrb
{

class GlTexture;
class Texture;

// you (the game developer) shouldn't need to use this class
// directly, it's all handled by the Gl singleton.
class GlTextureAtlas
{
public:

    GlTextureAtlas (ScreenCoordVector2 const &size, Uint32 gltexture_flags);
    ~GlTextureAtlas ();

    ScreenCoordVector2 const &Size () const { return m_size; }
    Uint32 GlTextureFlags () const { return m_flags; }
    GLuint Handle () const { return m_handle; }
    Uint32 GlTextureCount () const { return m_placed_gltexture_set.size(); }
    Uint32 AllocatedTextureByteCount () const { return m_allocated_texture_byte_count; }
    Uint32 UsedTextureByteCount () const { return m_used_texture_byte_count; }

    // attempts to find space for the given texture.  if space is found, the
    // texture is placed, and an appropriate GlTexture is returned.  otherwise
    // NULL is returned.
    GlTexture *AttemptToPlaceTexture (Texture const &texture, Uint32 gltexture_flags);
    // deallocates space in the allocation bitmap (freeing the space)
    void UnplaceTexture (GlTexture const &gltexture);

#if XRB_PLATFORM != XRB_PLATFORM_IPHONE
    /** This method is only supported on non-iphone platforms -- openGL ES does
      * not support the operations necessary to easily retrieve texture data
      * (it is possible, but annoying, and this method is really only useful
      * for game devs during development, not to the end user).
      * @brief Dumps the pixel data contents of this GlTextureAtlas to a newly
      *        allocated Texture.
      * @param mipmap_level The desired mipmap level.  Default is 0.
      */
    Texture *Dump (Uint32 mipmap_level = 0) const;
#endif // XRB_PLATFORM != XRB_PLATFORM_IPHONE

private:

    void AssertThatTextureJives (Texture const &texture) const;
    ScreenCoordVector2 CenterBegin (ScreenCoordVector2 const &texture_size) const;
    ScreenCoordVector2 CenterEnd (ScreenCoordVector2 const &texture_size) const;
    ScreenCoordVector2 MinimumSpaceBetween (GlTexture const &gltexture, Texture const &texture) const;
    bool ThereIsEnoughSpaceFor (Texture const &texture, ScreenCoordVector2 const &center) const;
    GlTexture *ActuallyPlaceTexture (Texture const &texture, ScreenCoordVector2 center);
    void PlaceMipmapAndBorder (Uint32 mipmap_level, Texture const &mipmap, ScreenCoordVector2 const &mipmap_center);
    ScreenCoordVector2 CalculateBorderLocation (
        Uint32 which_border,
        Uint32 border_mask,
        ScreenCoordVector2 const &atlas_mipmap_size,
        ScreenCoordVector2 const &mipmap_size,
        ScreenCoordVector2 const &mipmap_center) const;

    enum { LEFT = (1 << 0), RIGHT = (1 << 1), BOTTOM = (1 << 2), TOP = (1 << 3) };

    static Texture *CreateBorderTexture (Texture const &texture, Uint32 which_border, Uint32 border_mask);
    static Uint32 CountTextureBytes (ScreenCoordVector2 level_0_mipmap_size);

    struct GlTextureOrder
    {
        bool operator () (GlTexture const *left, GlTexture const *right) const;
    }; // end of struct GlTextureAtlas::GlTextureOrder

    typedef std::set<GlTexture const *, GlTextureOrder> PlacedGlTextureSet;

    ScreenCoordVector2 const m_size;
    Uint32 const m_flags;
    GLuint m_handle;
    PlacedGlTextureSet m_placed_gltexture_set;
    // the number of bytes of texture memory used by this atlas, including
    // all mipmap levels.  might be inaccurate for non-power-of-2-sized
    // textures, size i don't really know how opengl implements those mipmaps.
    Uint32 m_allocated_texture_byte_count;
    // the number of actually used bytes on all mipmap levels (this
    // doesn't include border or spacing pixels).
    Uint32 m_used_texture_byte_count;
}; // end of class GlTextureAtlas

} // end of namespace Xrb

#endif // !defined(_XRB_GLTEXTUREATLAS_HPP_)

