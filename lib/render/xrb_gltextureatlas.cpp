// ///////////////////////////////////////////////////////////////////////////
// xrb_gltextureatlas.cpp by Victor Dods, created 2009/09/11
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_gltextureatlas.hpp"

#include "xrb_gltexture.hpp"
#include "xrb_math.hpp"
#include "xrb_resourcelibrary.hpp"
#include "xrb_texture.hpp"

namespace Xrb
{

GlTextureAtlas::GlTextureAtlas (ScreenCoordVector2 const &size)
    :
    m_size(size),
    m_allocation_bitmap(m_size[Dim::X] * m_size[Dim::Y], false) // all empty
{
    ASSERT1(Math::IsAPowerOf2(m_size[Dim::X]));
    ASSERT1(Math::IsAPowerOf2(m_size[Dim::Y]));

    glGenTextures(1, &m_handle);
    ASSERT1(m_handle > 0);
    // the fact that m_handle and m_size have been initialized already
    // is critical for the success of the call to BindAtlas.
    Singleton::Gl().BindAtlas(*this);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // there are restrictions by openGL ES (specifically for the PowerVR MBX
    // platform as used by the ipod touch and older versions of iphones).
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // this is the other option within the restriction as described above.
    // it has lower visual quality, but may be faster.
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

    // this causes mipmaps to be generated whenever the base (level 0) texture is changed
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    // allocate the base (level 0) texture
    glTexImage2D(
        GL_TEXTURE_2D,      // target
        0,                  // level
        GL_RGBA,            // internal format
        m_size[Dim::X],     // width
        m_size[Dim::Y],     // height
        0,                  // width of the border (must be 0 or 1)
        GL_RGBA,            // format of the input pixel data
        GL_UNSIGNED_BYTE,   // data type of the input pixel data
        NULL);              // NULL pixel data indicates openGL should allocate the data
}

GlTextureAtlas::~GlTextureAtlas ()
{
    // just make sure there's nothing allocated
    for (AllocationBitmap::iterator it = m_allocation_bitmap.begin(),
                                    it_end = m_allocation_bitmap.end();
         it != it_end;
         ++it)
    {
        if (*it != false)
        {
            fprintf(stderr, "Dangling GlTexture during GlTextureAtlas destruction.  ResourceLibrary inventory:\n");
            Singleton::ResourceLibrary().PrintInventory(stderr, 1);
            break;
        }
    }

    Singleton::Gl().EnsureAtlasIsNotBound(*this);

    ASSERT1(m_handle > 0);
    glDeleteTextures(1, &m_handle);
}

GlTexture *GlTextureAtlas::PlaceTexture (Texture const &texture, Uint32 gltexture_flags)
{
    ASSERT1(Math::IsAPowerOf2(texture.Width()) && "can only use power-of-2-sized textures in GlTextureAtlas");
    ASSERT1(Math::IsAPowerOf2(texture.Height()) && "can only use power-of-2-sized textures in GlTextureAtlas");

    if (texture.Width() > m_size[Dim::X])
        return NULL;
    if (texture.Height() > m_size[Dim::Y])
        return NULL;

    ScreenCoordVector2 slot_count(m_size[Dim::X] / texture.Width(), m_size[Dim::Y] / texture.Height());
    ScreenCoordVector2 slot_size(texture.Width(), texture.Height());
    ScreenCoordVector2 slot;

    // TODO: different traversal pattern (start at corner and work outward)
    for (slot[Dim::Y] = 0; slot[Dim::Y] < slot_count[Dim::Y]; ++slot[Dim::Y])
        for (slot[Dim::X] = 0; slot[Dim::X] < slot_count[Dim::X]; ++slot[Dim::X])
            if (!IsAllocated(slot, slot_size))
            {
                // this slot is free, so place the texture here
                Allocate(slot, slot_size);
                Singleton::Gl().BindAtlas(*this);
                glTexSubImage2D(
                    GL_TEXTURE_2D,
                    0,                              // mipmap level
                    slot[Dim::X]*slot_size[Dim::X], // x offset
                    slot[Dim::Y]*slot_size[Dim::Y], // y offset
                    slot_size[Dim::X],              // width
                    slot_size[Dim::Y],              // height
                    GL_RGBA,                        // format of the input pixel data
                    GL_UNSIGNED_BYTE,               // data type of the input pixel data
                    texture.Data());                // pixel data
                return new GlTexture(
                    *this,
                    texture.Size(),
                    ScreenCoordVector2(slot[Dim::X]*slot_size[Dim::X], slot[Dim::Y]*slot_size[Dim::Y]),
                    gltexture_flags);
            }

    // SORRY, no slot worked
    return NULL;
}

void GlTextureAtlas::UnplaceTexture (GlTexture const &gltexture)
{
    ASSERT1(&gltexture.Atlas() == this);
    for (ScreenCoord y = gltexture.TextureCoordOffset()[Dim::Y];
         y < gltexture.TextureCoordOffset()[Dim::Y] + gltexture.Size()[Dim::Y];
         ++y)
    {
        for (ScreenCoord x = gltexture.TextureCoordOffset()[Dim::X];
             x < gltexture.TextureCoordOffset()[Dim::X] + gltexture.Size()[Dim::X];
             ++x)
        {
            ASSERT1(m_allocation_bitmap[y * m_size[Dim::X] + x] == true);
            m_allocation_bitmap[y * m_size[Dim::X] + x] = false;
        }
    }
}

bool GlTextureAtlas::IsAllocated (ScreenCoordVector2 const &slot, ScreenCoordVector2 const &slot_size) const
{
    ASSERT1(slot_size[Dim::X] > 0 && slot_size[Dim::X] <= m_size[Dim::X]);
    ASSERT1(slot_size[Dim::Y] > 0 && slot_size[Dim::Y] <= m_size[Dim::Y]);
    ASSERT1(Math::IsAPowerOf2(slot_size[Dim::X]));
    ASSERT1(Math::IsAPowerOf2(slot_size[Dim::Y]));

    ASSERT1(slot[Dim::X] >= 0 && (slot[Dim::X]+1)*slot_size[Dim::X] <= m_size[Dim::X]);
    ASSERT1(slot[Dim::Y] >= 0 && (slot[Dim::Y]+1)*slot_size[Dim::Y] <= m_size[Dim::Y]);

    for (ScreenCoord y = slot[Dim::Y]*slot_size[Dim::Y]; y < (slot[Dim::Y]+1)*slot_size[Dim::Y]; ++y)
        for (ScreenCoord x = slot[Dim::X]*slot_size[Dim::X]; x < (slot[Dim::X]+1)*slot_size[Dim::X]; ++x)
            if (IsAllocated(x, y))
                return true;

    return false;
}

void GlTextureAtlas::Allocate (ScreenCoordVector2 const &slot, ScreenCoordVector2 const &slot_size)
{
    ASSERT1(slot_size[Dim::X] > 0 && slot_size[Dim::X] <= m_size[Dim::X]);
    ASSERT1(slot_size[Dim::Y] > 0 && slot_size[Dim::Y] <= m_size[Dim::Y]);
    ASSERT1(Math::IsAPowerOf2(slot_size[Dim::X]));
    ASSERT1(Math::IsAPowerOf2(slot_size[Dim::Y]));

    ASSERT1(slot[Dim::X] >= 0 && (slot[Dim::X]+1)*slot_size[Dim::X] <= m_size[Dim::X]);
    ASSERT1(slot[Dim::Y] >= 0 && (slot[Dim::Y]+1)*slot_size[Dim::Y] <= m_size[Dim::Y]);

    for (ScreenCoord y = slot[Dim::Y]*slot_size[Dim::Y]; y < (slot[Dim::Y]+1)*slot_size[Dim::Y]; ++y)
        for (ScreenCoord x = slot[Dim::X]*slot_size[Dim::X]; x < (slot[Dim::X]+1)*slot_size[Dim::X]; ++x)
            Allocate(x, y);
}

} // end of namespace Xrb
