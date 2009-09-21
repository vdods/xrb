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

#include <vector>

#include "xrb_gl.hpp"
#include "xrb_screencoord.hpp"

namespace Xrb
{

class GlTexture;
class Texture;

class GlTextureAtlas
{
public:

    GlTextureAtlas (ScreenCoordVector2 const &size);
    ~GlTextureAtlas ();

    ScreenCoordVector2 const &Size () const { return m_size; }
    GLuint Handle () const { return m_handle; }

    // attempts to find space for the given texture.  if space is found, the
    // texture is placed, and an appropriate GlTexture is returned.  otherwise
    // NULL is returned.
    GlTexture *PlaceTexture (Texture const &texture, Uint32 gltexture_flags);
    // deallocates space in the allocation bitmap (freeing the space)
    void UnplaceTexture (GlTexture const &gltexture);

private:

    // slot coords are right-handed, starting from bottom left
    bool IsAllocated (ScreenCoordVector2 const &slot, ScreenCoordVector2 const &slot_size) const;
    // x and y coords are right-handed, starting from bottom left
    bool IsAllocated (ScreenCoord x, ScreenCoord y) const
    {
        ASSERT1(x >= 0 && x < m_size[Dim::X]);
        ASSERT1(y >= 0 && y < m_size[Dim::Y]);
        return m_allocation_bitmap[y * m_size[Dim::X] + x];
    }

    // sets the appropriate bits in the allocation bitmap.
    void Allocate (ScreenCoordVector2 const &slot, ScreenCoordVector2 const &slot_size);
    // sets the appropriate bit in the allocation bitmap
    void Allocate (ScreenCoord x, ScreenCoord y)
    {
        ASSERT1(x >= 0 && x < m_size[Dim::X]);
        ASSERT1(y >= 0 && y < m_size[Dim::Y]);
        ASSERT1(m_allocation_bitmap[y * m_size[Dim::X] + x] == false);
        m_allocation_bitmap[y * m_size[Dim::X] + x] = true;
    }

    typedef std::vector<bool> AllocationBitmap;

    ScreenCoordVector2 const m_size;
    GLuint m_handle;
    AllocationBitmap m_allocation_bitmap;
}; // end of class GlTextureAtlas

} // end of namespace Xrb

#endif // !defined(_XRB_GLTEXTUREATLAS_HPP_)

