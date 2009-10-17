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

/*

texture atlas theory/design

all textures are power-of-2 sized, and are centered at coordinates size*(2*k+1)
(this applies to size-1 textures also, where they consist of essentially two
half-pixels with half-pixel borders).  'b' indicates an artificial border pixel
a lower-case letter indicates the ultimate average of the whole texture (though
the diagram uses 1D textures for simplicity).

in the following, P Q R S T U are size-1 textures

size-1 textures can be right on the edge (e.g. P is centered at 0, so half of
it "sticks off the edge", which is ok since we're using GL_CLAMP_TO_EDGE in all
of this)

........XXXXXXXXXXXXXXXX................YYYYYYYYYYYYYYYY........
........XXXXXXXXXXXXXXXX......ZZZZ......YYYYYYYYYYYYYYYY........
.WW.....XXXXXXXXXXXXXXXX......ZZZZ......YYYYYYYYYYYYYYYY.....VV.
.WW..pp.XXXXXXXXXXXXXXXX.qqrr.ZZZZ.sstt.YYYYYYYYYYYYYYYY.uu..VV.    bare
bWWb.ppbXXXXXXXXXXXXXXXXbqqrrbZZZZbssttbYYYYYYYYYYYYYYYYbuu.bVVb    with borders

ww..XXXXXXXX...ZZ...YYYYYYYY..vv    bare
ww.bXXXXXXXXb.bZZb.bYYYYYYYYb.vv    with borders

..XXXX.zz.YYYY..    bare
.bXXXXbzzbYYYYb.    with borders

.XX..YY.    bare
bXXbbYYb    with borders

xxyy    bare
xxyy    with borders

..  bare                this and the level below don't matter
..  with borders

.   bare
.   with borders

there are two size-16, one size-4, two size-2, and six size-1 textures
in the above diagram.

the total amount of space is 64, the amount of used space is 50, for a
78% efficiency.

in order to place a texture, there must be at least certain distance
between its center and the centers of nearby textures.

larger tex size     smaller tex size    min distance (wrt alignment)
2^n                 2^n                 2^n * 2         this is an exceptional case
2^n                 2^(n-1)             2^n * 3/2   =   2^(n-1) + 2^(n-0)
2^n                 2^(n-2)             2^n * 1     =   2^(n-1) + 2^(n-1)
2^n                 2^(n-3)             2^n * 3/4   =   2^(n-1) + 2^(n-2)
2^n                 2^(n-4)             2^n * 5/8   =   2^(n-1) + 2^(n-3)
...

let m > n.  then the (casewise) general formula is

tex 1 size          tex 2 size      min distance (wrt alignment)
2^n                 2^n             2^(n+1)
2^m                 2^n             2^(m-1) + 2^(n+1)

my guess is that loading the larger textures first will give the best
spatial efficiency, as the smaller textures will filter down into the
large holes.

*/

namespace Xrb
{

GlTextureAtlas::GlTextureAtlas (ScreenCoordVector2 const &size, Uint32 gltexture_flags)
    :
    m_size(size),
    m_flags(gltexture_flags),
    m_allocated_texture_byte_count(0),
    m_used_texture_byte_count(0)
{
    if (UsesSeparateAtlas())
    {
        // nothing to check (yet)
    }
    else
    {
        // require that "real" atlases be power-of-2-sized.
        ASSERT1(Math::IsAPowerOf2(m_size[Dim::X]));
        ASSERT1(Math::IsAPowerOf2(m_size[Dim::Y]));
    }

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
    if (MipmapsDisabled())
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // this is the other option within the restriction as described above.
    // it has lower visual quality, but may be faster.
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    // this gets rid of almost all the texture atlas border bleeding,
    // and is probably the fastest, but has the lowest visual quality.
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

    // if a separate atlas is to be used (and mipmaps aren't disabled), then
    // let opengl take care of recalculating the mipmaps (this happens
    // automatically when level 0 is changed).
    if (UsesSeparateAtlas() && !MipmapsDisabled())
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    // otherwise, we'll calculate the mipmaps manually (or not at all)
    else
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);

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

    // allocate the mipmap textures manually if necessary.
    if (!UsesSeparateAtlas() && !MipmapsDisabled())
    {
        Uint32 mipmap_level = 1;
        ScreenCoordVector2 s(m_size / 2);
        // go all the way down to 1xN (or Nx1)
        while (s[Dim::X] > 1 && s[Dim::Y] > 1)
        {
            glTexImage2D(GL_TEXTURE_2D, mipmap_level, GL_RGBA, s[Dim::X], s[Dim::Y], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            ++mipmap_level;
            s /= 2;
        }
        // go all the way down to 1x1
        while (s[Dim::X] >= 1 && s[Dim::Y] >= 1)
        {
            ASSERT1(s[Dim::X] == 1 || s[Dim::Y] == 1);
            glTexImage2D(GL_TEXTURE_2D, mipmap_level, GL_RGBA, s[Dim::X], s[Dim::Y], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            ++mipmap_level;
            if (s[Dim::X] > 1)
                s[Dim::X] /= 2;
            else
                s[Dim::Y] /= 2;
        }
    }

    // calculate and keep track of the allocated and used texture bytes
    // NOTE: if the atlas is rectangular or power-of-2-sized, then this might
    // be wrong.  it might go down to 1xN and then 1x(N/2), 1x(N/4), etc.
    // though the manual mipmapping doesn't do that.
    m_allocated_texture_byte_count = CountTextureBytes(m_size);
}

GlTextureAtlas::~GlTextureAtlas ()
{
    if (!m_placed_gltexture_set.empty())
    {
        fprintf(stderr, "Dangling GlTexture(s) during GlTextureAtlas destruction.  ResourceLibrary inventory:\n");
        Singleton::ResourceLibrary().PrintInventory(stderr, 1);
    }

    Singleton::Gl().EnsureAtlasIsNotBound(*this);

    ASSERT1(m_handle > 0);
    glDeleteTextures(1, &m_handle);
}

bool GlTextureAtlas::UsesSeparateAtlas () const
{
    return (m_flags & GlTexture::USES_SEPARATE_ATLAS) != 0;
}

bool GlTextureAtlas::MipmapsDisabled () const
{
    return (m_flags & GlTexture::MIPMAPS_DISABLED) != 0;
}

GlTexture *GlTextureAtlas::AttemptToPlaceTexture (Texture const &texture, Uint32 gltexture_flags)
{
    ASSERT1(gltexture_flags == m_flags && "incompatible atlas");
    AssertThatTextureJives(texture);

    if (texture.Width() > m_size[Dim::X] || texture.Height() > m_size[Dim::Y])
        return NULL;

    // separate (easier) handling for UsesSeparateAtlas() cases, since
    // the mipmaps are all done automatically.
    if (UsesSeparateAtlas())
    {
        ASSERT1(m_placed_gltexture_set.empty());
        Singleton::Gl().BindAtlas(*this);
        glTexSubImage2D(
            GL_TEXTURE_2D,      // target (must be GL_TEXTURE_2D)
            0,                  // mipmap level
            0,                  // x offset
            0,                  // y offset
            texture.Width(),    // width
            texture.Height(),   // height
            GL_RGBA,            // format of the input pixel data
            GL_UNSIGNED_BYTE,   // data type of the input pixel data
            texture.Data());    // pixel data
        GlTexture *retval = new GlTexture(
            *this,
            texture.Size(),
            ScreenCoordRect(texture.Size()),
            m_flags);
        m_placed_gltexture_set.insert(retval);
        m_used_texture_byte_count += CountTextureBytes(texture.Size());
        return retval;
    }

    ScreenCoordVector2 center_begin(CenterBegin(texture.Size()));
    ScreenCoordVector2 center_end(CenterEnd(texture.Size()));
    ScreenCoordVector2 center_stride(2*texture.Size());
    ScreenCoordVector2 center;
    for (center[Dim::Y] = center_begin[Dim::Y];
         center[Dim::Y] <= center_end[Dim::Y];
         center[Dim::Y] += center_stride[Dim::Y])
    {
        for (center[Dim::X] = center_begin[Dim::X];
             center[Dim::X] <= center_end[Dim::X];
             center[Dim::X] += center_stride[Dim::X])
        {
            if (ThereIsEnoughSpaceFor(texture, center))
            {
                GlTexture *retval = ActuallyPlaceTexture(texture, center);
                ASSERT1(retval != NULL);
                m_placed_gltexture_set.insert(retval);
                m_used_texture_byte_count += CountTextureBytes(texture.Size());
                return retval;
            }
        }
    }

    // SORRY, there wasn't space
    return NULL;
}

void GlTextureAtlas::UnplaceTexture (GlTexture const &gltexture)
{
    ASSERT1(&gltexture.Atlas() == this);
    ASSERT1(m_placed_gltexture_set.find(&gltexture) != m_placed_gltexture_set.end());
    m_placed_gltexture_set.erase(&gltexture);
    // update the used texture byte count
    ASSERT1(m_used_texture_byte_count >= CountTextureBytes(gltexture.Size()));
    m_used_texture_byte_count -= CountTextureBytes(gltexture.Size());
    // the texture's pixel data is still in the atlas, but it doesn't really matter.
    // TODO: it would be nice to blank it all out
}

#if XRB_PLATFORM != XRB_PLATFORM_IPHONE
Texture *GlTextureAtlas::Dump (Uint32 mipmap_level) const
{
    Singleton::Gl().BindAtlas(*this);

    GLint width, height;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, mipmap_level, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, mipmap_level, GL_TEXTURE_HEIGHT, &height);
    // the error condition will probably happen if/when mipmap_level exceeds
    // the actual max value
    if (glGetError() != GL_NO_ERROR || width == 0 || height == 0)
        return NULL;

    Texture *retval = Texture::Create(ScreenCoordVector2(width, height), Texture::UNINITIALIZED);
    glGetTexImage(GL_TEXTURE_2D, mipmap_level, GL_RGBA, GL_UNSIGNED_BYTE, retval->Data());
    return retval;
}
#endif // XRB_PLATFORM != XRB_PLATFORM_IPHONE

void GlTextureAtlas::AssertThatTextureJives (Texture const &texture) const
{
    if (UsesSeparateAtlas())
    {
        // nothing to check so far
    }
    else
    {
        ASSERT1(Math::IsAPowerOf2(texture.Width()) &&
                Math::IsAPowerOf2(texture.Height()) &&
                texture.Width() == texture.Height() &&
                "must specify GlTexture::USES_SEPARATE_ATLAS for non-square, non-power-of-2-sized textures");
    }
}

ScreenCoordVector2 GlTextureAtlas::CenterBegin (ScreenCoordVector2 const &texture_size) const
{
    ASSERT1(Math::IsAPowerOf2(texture_size[Dim::X]));
    ASSERT1(Math::IsAPowerOf2(texture_size[Dim::Y]));
    ASSERT1(texture_size[Dim::X] == texture_size[Dim::Y]);

    // if the texture size is 0, then always start at 0.  it is a special case
    // if the texture is exactly as wide/tall as the atlas -- then it will not
    // be centered at size*(2*k+1), but rather size/2.  otherwise, the center
    // starts at size.

    ScreenCoordVector2 retval;
    for (Uint32 i = 0; i < 2; ++i)
    {
        if (texture_size[i] == 1)
            retval[i] = 0;
        else if (texture_size[i] == m_size[i])
            retval[i] = texture_size[i] / 2;
        else
            retval[i] = texture_size[i];
    }
    return retval;
}

ScreenCoordVector2 GlTextureAtlas::CenterEnd (ScreenCoordVector2 const &texture_size) const
{
    ASSERT1(Math::IsAPowerOf2(texture_size[Dim::X]));
    ASSERT1(Math::IsAPowerOf2(texture_size[Dim::Y]));
    ASSERT1(texture_size[Dim::X] == texture_size[Dim::Y]);

    // if the texture size is 0, then always end at atlas_size.  it is a special
    // case if the texture is exactly as wide/tall as the atlas -- then it ends at
    // size/2, since there's only one texture width/height in the size.  otherwise,
    // the center ends where it ends (see below)

    ScreenCoordVector2 retval;
    for (Uint32 i = 0; i < 2; ++i)
    {
        if (texture_size[i] == 1)
            retval[i] = m_size[i];
        else if (texture_size[i] == m_size[i])
            retval[i] = texture_size[i] / 2;
        else
            retval[i] = texture_size[i] + 2*texture_size[i] * (m_size[i] / (2*texture_size[i]) - 1);
    }
    return retval;
}

ScreenCoordVector2 GlTextureAtlas::MinimumSpaceBetween (GlTexture const &gltexture, Texture const &texture) const
{
    ASSERT1(Math::IsAPowerOf2(texture.Width()));
    ASSERT1(Math::IsAPowerOf2(texture.Height()));
    ASSERT1(texture.Width() == texture.Height());

    ScreenCoordVector2 retval;
    for (Uint32 i = 0; i < 2; ++i)
    {
        ScreenCoord big = gltexture.Size()[i];
        ScreenCoord small = texture.Size()[i];
        if (big < small)
        {
            ScreenCoord temp = big;
            big = small;
            small = temp;
        }

        if (big == small)
            retval[i] = 2*big;
        else
            retval[i] = big/2 + small*2;
    }
    return retval;
}

bool GlTextureAtlas::ThereIsEnoughSpaceFor (Texture const &texture, ScreenCoordVector2 const &center) const
{
    ASSERT1(Math::IsAPowerOf2(texture.Width()));
    ASSERT1(Math::IsAPowerOf2(texture.Height()));
    ASSERT1(texture.Width() == texture.Height());

    for (PlacedGlTextureSet::const_iterator it = m_placed_gltexture_set.begin(),
                                            it_end = m_placed_gltexture_set.end();
         it != it_end;
         ++it)
    {
        ASSERT1(*it != NULL);
        GlTexture const &gltexture = **it;
        ScreenCoordVector2 minimum_space_between(MinimumSpaceBetween(gltexture, texture));
        if (Abs(gltexture.TextureCoordinateCenter()[Dim::X] - center[Dim::X]) < minimum_space_between[Dim::X] &&
            Abs(gltexture.TextureCoordinateCenter()[Dim::Y] - center[Dim::Y]) < minimum_space_between[Dim::Y])
        {
            return false; // it was too close to some existing gltexture
        }
    }
    // it was far enough away from all existing gltextures
    return true;
}

GlTexture *GlTextureAtlas::ActuallyPlaceTexture (Texture const &texture, ScreenCoordVector2 center)
{
    AssertThatTextureJives(texture);

    // 1xN (Nx1) textures will have "0-width" ("0-height") texture coord boxes,
    // since their texels are essentially split in half in the coordinates, so
    // basically round down to the nearest multiple of 2.  this is a way to
    // avoid having to scale the texture matrix by 2 which effectively would
    // give texture coordinate addressing resolution of 1/2 unit.
    ASSERT1(texture.Width() == 1 || Math::IsEven(texture.Width()));
    ASSERT1(texture.Height() == 1 || Math::IsEven(texture.Height()));
    ScreenCoordVector2 adjusted_texture_size(
        texture.Width() > 1 ? texture.Width() : 0,
        texture.Height() > 1 ? texture.Height() : 0);
    ScreenCoordRect texture_coordinate_rect(
        center-adjusted_texture_size/2,
        center+adjusted_texture_size/2);
    GlTexture *retval = new GlTexture(*this, adjusted_texture_size, texture_coordinate_rect, m_flags);

    // mipmap level 0
    PlaceMipmapAndBorder(0, texture, center);

    // higher mipmap levels
    Uint32 mipmap_level = 1;
    Texture *mipmap = texture.CreateMipmap();
    center /= 2;
    while (mipmap->Width() > 0 && mipmap->Height() > 0)
    {
        PlaceMipmapAndBorder(mipmap_level, *mipmap, center);

        ++mipmap_level;
        Texture *next_mipmap = mipmap->CreateMipmap();
        delete mipmap;
        mipmap = next_mipmap;
        center /= 2;
    }
    delete mipmap;

    return retval;
}

void GlTextureAtlas::PlaceMipmapAndBorder (Uint32 mipmap_level, Texture const &mipmap, ScreenCoordVector2 const &mipmap_center)
{
    Singleton::Gl().BindAtlas(*this);

    // place the pixel data (the body of the mipmap)
    if (mipmap.Width() == 1 || mipmap.Height() == 1)
    {
        // if it's a 1xN or Nx1, there's no body, it's all in the border
    }
    else
    {
        ASSERT1(Math::IsEven(mipmap.Width()));
        ASSERT1(Math::IsEven(mipmap.Height()));
        ASSERT1(mipmap_center[Dim::X] - mipmap.Width()/2 >= 0);
        ASSERT1(mipmap_center[Dim::Y] - mipmap.Height()/2 >= 0);
        glTexSubImage2D(
            GL_TEXTURE_2D,                              // target (must be GL_TEXTURE_2D)
            mipmap_level,                               // mipmap level
            mipmap_center[Dim::X] - mipmap.Width()/2,   // x offset
            mipmap_center[Dim::Y] - mipmap.Height()/2,  // y offset
            mipmap.Width(),                             // width
            mipmap.Height(),                            // height
            GL_RGBA,                                    // format of the input pixel data
            GL_UNSIGNED_BYTE,                           // data type of the input pixel data
            mipmap.Data());                             // pixel data
    }

    // place the border data
    {
        ScreenCoordVector2 atlas_mipmap_size(m_size[Dim::X] >> mipmap_level, m_size[Dim::Y] >> mipmap_level);
        ASSERT1(atlas_mipmap_size[Dim::X] > 0 && atlas_mipmap_size[Dim::Y] > 0);

        // determine which borders lie within the atlas mipmap
        Uint32 border_mask = 0;
        // this math is essentially in X.1 fixed point (since we need to deal with
        // half-units when the mipmap size is down to 1xN).
        if (2*mipmap_center[Dim::X] - mipmap.Width() > 2*0)
            border_mask |= LEFT;
        if (2*mipmap_center[Dim::X] + mipmap.Width() < 2*atlas_mipmap_size[Dim::X])
            border_mask |= RIGHT;
        if (2*mipmap_center[Dim::Y] - mipmap.Height() > 2*0)
            border_mask |= BOTTOM;
        if (2*mipmap_center[Dim::Y] - mipmap.Height() < 2*atlas_mipmap_size[Dim::Y])
            border_mask |= TOP;
        // create the border textures (these can end up being NULL) and copy their pixel
        // data into the atlas mipmap, and delete each as we go.
        for (Uint32 which_border = LEFT; which_border <= TOP; which_border <<= 1)
        {
            Texture *border = CreateBorderTexture(mipmap, which_border, border_mask);
            if (border != NULL)
            {
                ScreenCoordVector2 border_location(CalculateBorderLocation(which_border, border_mask, atlas_mipmap_size, mipmap.Size(), mipmap_center));
                // make sure the calculated border and location actually fit on the atlas mipmap
                ASSERT1(border_location[Dim::X] >= 0);
                ASSERT1(border_location[Dim::X]+border->Width() <= atlas_mipmap_size[Dim::X]);
                ASSERT1(border_location[Dim::Y] >= 0);
                ASSERT1(border_location[Dim::Y]+border->Height() <= atlas_mipmap_size[Dim::Y]);
                glTexSubImage2D(
                    GL_TEXTURE_2D,              // target (must be GL_TEXTURE_2D)
                    mipmap_level,               // mipmap level
                    border_location[Dim::X],    // x offset
                    border_location[Dim::Y],    // y offset
                    border->Width(),            // width
                    border->Height(),           // height
                    GL_RGBA,                    // format of the input pixel data
                    GL_UNSIGNED_BYTE,           // data type of the input pixel data
                    border->Data());            // pixel data
                delete border;
            }
        }
    }
}

ScreenCoordVector2 GlTextureAtlas::CalculateBorderLocation (
    Uint32 which_border,
    Uint32 border_mask,
    ScreenCoordVector2 const &atlas_mipmap_size,
    ScreenCoordVector2 const &mipmap_size,
    ScreenCoordVector2 const &mipmap_center) const
{
    ASSERT1(which_border & border_mask);
    ASSERT1(which_border == LEFT || which_border == RIGHT || which_border == TOP || which_border == BOTTOM);
    ASSERT1(mipmap_size[Dim::X] == 1 || Math::IsEven(mipmap_size[Dim::X]));
    ASSERT1(mipmap_size[Dim::Y] == 1 || Math::IsEven(mipmap_size[Dim::Y]));

    // size-0 dimensions are effectively 0 (since they're a half-pixel on each side
    // so they round down, and the border pixel becomes everything).
    ScreenCoordVector2 half_size(
        mipmap_size[Dim::X] == 1 ? 0 : mipmap_size[Dim::X]/2,
        mipmap_size[Dim::Y] == 1 ? 0 : mipmap_size[Dim::Y]/2);

    switch (which_border)
    {
        case LEFT:
            return ScreenCoordVector2(
                mipmap_center[Dim::X] - half_size[Dim::X] - 1,
                mipmap_center[Dim::Y] - half_size[Dim::Y] - (border_mask & BOTTOM ? 1 : 0));
        case RIGHT:
            return ScreenCoordVector2(
                mipmap_center[Dim::X] + half_size[Dim::X],
                mipmap_center[Dim::Y] - half_size[Dim::Y] - (border_mask & BOTTOM ? 1 : 0));
        case BOTTOM:
            return ScreenCoordVector2(
                mipmap_center[Dim::X] - half_size[Dim::X] - (border_mask & LEFT ? 1 : 0),
                mipmap_center[Dim::Y] - half_size[Dim::Y] - 1);
        case TOP:
            return ScreenCoordVector2(
                mipmap_center[Dim::X] - half_size[Dim::X] - (border_mask & LEFT ? 1 : 0),
                mipmap_center[Dim::Y] + half_size[Dim::Y]);
        default:
            ASSERT1(false);
            return ScreenCoordVector2::ms_zero;
    }
}

Texture *GlTextureAtlas::CreateBorderTexture (Texture const &texture, Uint32 which_border, Uint32 border_mask)
{
    ASSERT1(texture.Width() > 0 && texture.Height() > 0);
    ASSERT1(which_border == LEFT || which_border == RIGHT || which_border == TOP || which_border == BOTTOM);

    Texture *retval = NULL;

    if ((which_border & border_mask) == 0)
        return retval;

    if (which_border == LEFT || which_border == RIGHT)
    {
        // figure out how big the border should be
        ScreenCoord adjusted_height =
            (texture.Height() == 1 ? 0 : texture.Height()) + // 1 rounds down to 0
            (border_mask & BOTTOM ? 1 : 0) +
            (border_mask & TOP ? 1 : 0);
        ScreenCoordVector2 size(1, adjusted_height);
        // create the border Texture
        retval = Texture::Create(size, Texture::UNINITIALIZED);
        // if the texture is 0xN or Nx0, there's no point in copying pixels
        if (size[Dim::X] == 0 || size[Dim::Y] == 0)
            return retval;

        ScreenCoord x = (which_border == LEFT) ? 0 : texture.Width()-1;
        // copy the corners if necessary
        if (border_mask & BOTTOM)
            memcpy(retval->Pixel(0, 0), texture.Pixel(x, 0), 4);
        if (border_mask & TOP)
            memcpy(retval->Pixel(0, retval->Height()-1), texture.Pixel(x, texture.Height()-1), 4);
        // copy the main part of the border (the non-corner part).  this can't
        // be a single memcpy because the pixel data isn't contiguous.  only do
        // this if the texture height is not 1 (because this is a special case)
        if (texture.Height() != 1)
            for (ScreenCoord y = 0; y < texture.Height(); ++y)
                memcpy(retval->Pixel(0, y + (border_mask & BOTTOM ? 1 : 0)), texture.Pixel(x, y), 4);
    }
    else
    {
        // figure out how big the border should be
        ScreenCoord adjusted_width =
            (texture.Width() == 1 ? 0 : texture.Width()) + // 1 rounds down to 0
            (border_mask & LEFT ? 1 : 0) +
            (border_mask & RIGHT ? 1 : 0);
        ScreenCoordVector2 size(adjusted_width, 1);
        // create the border Texture
        retval = Texture::Create(size, Texture::UNINITIALIZED);
        // if the texture is 0xN or Nx0, there's no point in copying pixels
        if (size[Dim::X] == 0 || size[Dim::Y] == 0)
            return retval;

        ScreenCoord y = (which_border == BOTTOM) ? 0 : texture.Height()-1;
        // copy the corners if necessary
        if (border_mask & LEFT)
            memcpy(retval->Pixel(0, 0), texture.Pixel(0, y), 4);
        if (border_mask & RIGHT)
            memcpy(retval->Pixel(retval->Width()-1, 0), texture.Pixel(texture.Width()-1, y), 4);
        // copy the main part of the border (the non-corner part).  this can
        // be a single memcpy because the pixel data is contiguous.  only do
        // this if the texture width is not 1 (because this is a special case)
        if (texture.Width() != 1)
            memcpy(retval->Pixel(border_mask & LEFT ? 1 : 0, 0), texture.Pixel(0, y), texture.Width()*4);
    }

    return retval;
}

Uint32 GlTextureAtlas::CountTextureBytes (ScreenCoordVector2 level_0_mipmap_size)
{
    // this might be inaccurate in some cases, but as long as it's consistent, that's fine.
    Uint32 retval = 0;
    while (level_0_mipmap_size[Dim::X] > 0 && level_0_mipmap_size[Dim::Y] > 0)
    {
        retval += level_0_mipmap_size[Dim::X] * level_0_mipmap_size[Dim::Y] * 4;
        level_0_mipmap_size /= 2;
    }
    return retval;
}

bool GlTextureAtlas::GlTextureOrder::operator () (GlTexture const *left, GlTexture const *right) const
{
    ASSERT1(left != NULL);
    ASSERT1(right != NULL);

    // prefer larger textures (by area) over smaller
    if (left->Width()*left->Height() > right->Width()*right->Height())
        return true;
    if (left->Width()*left->Height() < right->Width()*right->Height())
        return false;

    // prefer taller textures over shorter textures
    if (left->Height() > right->Height())
        return true;
    if (left->Height() < right->Height())
        return false;

    // arbitrarily sort by texture coordinate arrays (only the "bottom left" corner though)

    if (left->TextureCoordinateArray()[0] < right->TextureCoordinateArray()[0])
        return true;
    if (left->TextureCoordinateArray()[0] > right->TextureCoordinateArray()[0])
        return false;

    if (left->TextureCoordinateArray()[1] < right->TextureCoordinateArray()[1])
        return true;
    if (left->TextureCoordinateArray()[1] > right->TextureCoordinateArray()[1])
        return false;

    // at this point, they have the same size and texture coord centers, so consider them equal
    return false;
}

} // end of namespace Xrb
