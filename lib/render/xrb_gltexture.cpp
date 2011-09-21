// ///////////////////////////////////////////////////////////////////////////
// xrb_gltexture.cpp by Victor Dods, created 2005/06/04
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_gltexture.hpp"

#include "xrb_gltextureatlas.hpp"
#include "xrb_math.hpp"
#include "xrb_texture.hpp"

namespace Xrb
{

// ///////////////////////////////////////////////////////////////////////////
// GlTexture::LoadParameters
// ///////////////////////////////////////////////////////////////////////////

GlTexture::LoadParameters::LoadParameters (std::string const &path, Uint32 flags)
    :
    m_path(path),
    m_flags(flags)
{ }

std::string GlTexture::LoadParameters::ResourceName () const
{
    return "Xrb::GlTexture";
}

bool GlTexture::LoadParameters::IsLessThan (ResourceLoadParameters const &p) const
{
    LoadParameters const &rhs = *DStaticCast<LoadParameters const *>(&p);
    return m_path < rhs.m_path
           ||
           (m_path == rhs.m_path && m_flags < rhs.m_flags);
}

void GlTexture::LoadParameters::Fallback ()
{
    // get rid of the path; a blank path will indicate the "missing" texture
    m_path.clear();
    // also clear the flags, since we probably don't want to load the "missing"
    // texture into a separate atlas.  TODO: change this to the default flags
    // once default flags exist
    m_flags = NONE;
}

void GlTexture::LoadParameters::Print (FILE *fptr) const
{
    fprintf(fptr, "path = \"%s\"", m_path.c_str());
    if (m_flags != NONE)
    {
        fprintf(fptr, ", flags =");
        if (UsesSeparateAtlas())
            fprintf(fptr, " USES_SEPARATE_ATLAS");
        if (MipmapsDisabled())
            fprintf(fptr, " MIPMAPS_DISABLED");
        if (UsesFilterNearest())
            fprintf(fptr, " USES_FILTER_NEAREST");
    }
}

// ///////////////////////////////////////////////////////////////////////////
// GlTexture
// ///////////////////////////////////////////////////////////////////////////

GlTexture::~GlTexture ()
{
    Singleton::Gl().UnregisterGlTexture(*this);
}

GlTexture *GlTexture::Create (ResourceLoadParameters const &p)
{
    LoadParameters const &load_parameters = *DStaticCast<LoadParameters const *>(&p);

    // an empty path indicates we should create the "missing" texture
    // which is an ugly green and pink 2x2 checkerboard.
    if (load_parameters.Path().empty())
    {
        static Uint8 const s_ugly_texture_data[4*4] =
        {
              0, 255,   0, 255,
            255,   0, 255, 255,
            255,   0, 255, 255,
              0, 255,   0, 255,
        };
        Texture *missing = Texture::Create(ScreenCoordVector2(2, 2), Texture::CLEAR);
        memcpy(missing->Data(), s_ugly_texture_data, sizeof(s_ugly_texture_data));
        GlTexture *gltexture_missing = Create(*missing, load_parameters.Flags());
        ASSERT1(gltexture_missing != NULL); // this creation shouldn't fail
        delete missing;
        return gltexture_missing;
    }

    // otherwise try to load the given path
    Texture *texture = Texture::Create(load_parameters.Path());
    if (texture == NULL)
        return NULL;

    GlTexture *retval = Create(*texture, load_parameters.Flags());
    // retval could be NULL at this point (e.g. if the texture was non-square,
    // non-power-of-2-sized and did not use USES_SEPARATE_ATLAS).
    delete texture;
    return retval;
}

GlTexture *GlTexture::Create (Texture const &texture, Uint32 flags)
{
    return Singleton::Gl().CreateGlTexture(texture, flags);
}

ScreenCoordVector2 GlTexture::TextureCoordinateBottomLeft () const
{
    return ScreenCoordVector2(m_texture_coordinate_array[0], m_texture_coordinate_array[1]);
}

ScreenCoordVector2 GlTexture::TextureCoordinateCenter () const
{
    ASSERT1(Math::IsEven(m_texture_coordinate_array[0] + m_texture_coordinate_array[2]));
    ASSERT1(Math::IsEven(m_texture_coordinate_array[1] + m_texture_coordinate_array[5]));
    return ScreenCoordVector2(
        (m_texture_coordinate_array[0] + m_texture_coordinate_array[2]) / 2,  // avg of left and right
        (m_texture_coordinate_array[1] + m_texture_coordinate_array[5]) / 2); // avg of bottom and top
}

GlTexture::GlTexture (
    GlTextureAtlas &atlas,
    ScreenCoordVector2 const &size,
    ScreenCoordRect const &texture_coordinate_rect,
    Uint32 flags)
    :
    m_atlas(atlas),
    m_size(size),
    m_flags(flags)
{
    ASSERT1(texture_coordinate_rect.Left() >= SINT16_LOWER_BOUND);
    ASSERT1(texture_coordinate_rect.Left() <= SINT16_UPPER_BOUND);
    ASSERT1(texture_coordinate_rect.Right() >= SINT16_LOWER_BOUND);
    ASSERT1(texture_coordinate_rect.Right() <= SINT16_UPPER_BOUND);
    ASSERT1(texture_coordinate_rect.Top() >= SINT16_LOWER_BOUND);
    ASSERT1(texture_coordinate_rect.Top() <= SINT16_UPPER_BOUND);
    ASSERT1(texture_coordinate_rect.Bottom() >= SINT16_LOWER_BOUND);
    ASSERT1(texture_coordinate_rect.Bottom() <= SINT16_UPPER_BOUND);
    ASSERT1(texture_coordinate_rect.Left() <= texture_coordinate_rect.Right());
    ASSERT1(texture_coordinate_rect.Bottom() <= texture_coordinate_rect.Top());

    m_texture_coordinate_array[0] = texture_coordinate_rect.Left();
    m_texture_coordinate_array[1] = texture_coordinate_rect.Bottom();

    m_texture_coordinate_array[2] = texture_coordinate_rect.Right();
    m_texture_coordinate_array[3] = texture_coordinate_rect.Bottom();

    m_texture_coordinate_array[4] = texture_coordinate_rect.Left();
    m_texture_coordinate_array[5] = texture_coordinate_rect.Top();

    m_texture_coordinate_array[6] = texture_coordinate_rect.Right();
    m_texture_coordinate_array[7] = texture_coordinate_rect.Top();
}

} // end of namespace Xrb
