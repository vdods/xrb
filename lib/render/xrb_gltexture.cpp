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
#include "xrb_util.hpp"

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
    LoadParameters const &rhs = p.As<LoadParameters>();
    int comparison = m_path.compare(rhs.m_path);
    if (comparison < 0)
        return true;
    else if (comparison > 0)
        return false;
    else
        return m_flags < rhs.m_flags;
}

bool GlTexture::LoadParameters::IsFallback () const
{
    return m_path == "internal://missing";
}

void GlTexture::LoadParameters::Fallback ()
{
    // sentinel value for the internal (non-filesystem based) "missing" texture
    m_path = "internal://missing";
    // also clear the flags, since we probably don't want to load the "missing"
    // texture into a separate atlas.  TODO: change this to the default flags
    // once default flags exist
    m_flags = NONE;
}

void GlTexture::LoadParameters::Print (std::ostream &stream) const
{
    stream << "path = \"" << m_path << '"';
    if (m_flags != NONE)
    {
        stream << ", flags =";
        if (UsesSeparateAtlas())
            stream << " USES_SEPARATE_ATLAS";
        if (MipmapsDisabled())
            stream << " MIPMAPS_DISABLED";
        if (UsesFilterNearest())
            stream << " USES_FILTER_NEAREST";
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
    LoadParameters const &load_parameters = p.As<LoadParameters>();

    // the "missing" texture is an ugly green and red 2x2 checkerboard.
    // the "missing_alternate" texture is an ugly yellow and blue 2x2 checkerboard, and is used in the "missing" animation.
    if (load_parameters.Path() == "internal://missing" || load_parameters.Path() == "internal://missing_alternate")
    {
        static Uint8 const s_ugly_texture_data[4*4] =
        {
              0, 255,   0, 255,
            255,   0,   0, 255,
            255,   0,   0, 255,
              0, 255,   0, 255,
        };
        static Uint8 const s_ugly_texture_data_alternate[4*4] =
        {
              0,   0, 255, 255,
            255, 255,   0, 255,
            255, 255,   0, 255,
              0,   0, 255, 255,
        };
        Texture *missing = Texture::Create(ScreenCoordVector2(2, 2), Texture::CLEAR);
        memcpy(missing->Data(),
               (load_parameters.Path() == "internal://missing" ? s_ugly_texture_data : s_ugly_texture_data_alternate),
               sizeof(s_ugly_texture_data)); // NOTE: this depends on Texture using RGBA
        GlTexture *gltexture_missing = Create(*missing, load_parameters.Flags());
        ASSERT1(gltexture_missing != NULL); // this creation shouldn't fail
        delete missing;
        return gltexture_missing;
    }
    else if (load_parameters.Path().find("internal://#") == 0) // hex code for a 1x1 texture with that color
    {
        // parse the hex code out of the string
        std::string hex_code(load_parameters.Path().substr(12)); // 12 is the length of the string "internal://#"
        if (hex_code.length() != 6 && hex_code.length() != 8)
        {
            load_parameters.SetErrorMessage("invalid hex code length (must have the form #RRGGBB or #RRGGBBAA)");
            return NULL;
        }
        Uint8 rgba[4] = {0, 0, 0, 0xFF};
        Uint8 *ptr = rgba;
        for (Uint32 i = 0; i < hex_code.length(); ++i)
        {
            if (!Util::IsHexDigit(hex_code[i]))
            {
                load_parameters.SetErrorMessage(FORMAT("invalid hex code (non-hex digit found)"));
                return NULL;
            }
            *ptr = (*ptr << 4) + Util::HexValue(hex_code[i]);
            if (i % 2 == 1) // increment every other time
                ++ptr;
        }
        // create the texture
        Texture *hex_code_colored = Texture::Create(ScreenCoordVector2(1, 1), Texture::CLEAR);
        memcpy(hex_code_colored->Data(), rgba, 4); // NOTE: this depends on Texture using RGBA
        GlTexture *gltexture_hex_code_colored = Create(*hex_code_colored, load_parameters.Flags());
        ASSERT1(gltexture_hex_code_colored != NULL); // this creation shouldn't fail
        delete hex_code_colored;
        return gltexture_hex_code_colored;
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
