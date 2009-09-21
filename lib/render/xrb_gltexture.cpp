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
#include "xrb_texture.hpp"

namespace Xrb
{

// ///////////////////////////////////////////////////////////////////////////
// GlTexture::LoadParameters
// ///////////////////////////////////////////////////////////////////////////

std::string GlTexture::LoadParameters::ResourceName () const
{
    return "Xrb::GlTexture";
}

bool GlTexture::LoadParameters::IsLessThan (ResourceLoadParameters const &p) const
{
    LoadParameters const &rhs = *DStaticCast<LoadParameters const *>(&p);
    int comparison = m_path.compare(rhs.m_path);
    if (comparison < 0)
        return true;
    else if (comparison > 0)
        return false;
    else
        return m_flags < rhs.m_flags;
}

void GlTexture::LoadParameters::Fallback ()
{
    // get rid of the path; a blank path will indicate the "missing" texture
    m_path.clear();
    // also clear the flags, since we probably don't want to load the "missing"
    // texture into a separate atlas.
    m_flags = NONE;
}

void GlTexture::LoadParameters::Print (FILE *fptr) const
{
    fprintf(fptr, "path = \"%s\"", m_path.c_str());
    if (m_flags != NONE)
    {
        fprintf(fptr, ", flags = ");
        if (UsesSeparateAtlas())
            fprintf(fptr, " USES_SEPARATE_ATLAS");
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
        Texture *missing = Texture::Create(ScreenCoordVector2(2, 2), true);
        missing->Data()[ 0] =   0; missing->Data()[ 1] = 255; missing->Data()[ 2] =   0; missing->Data()[ 3] = 255;
        missing->Data()[ 4] = 255; missing->Data()[ 5] =   0; missing->Data()[ 6] = 255; missing->Data()[ 7] = 255;
        missing->Data()[ 8] = 255; missing->Data()[ 9] =   0; missing->Data()[10] = 255; missing->Data()[11] = 255;
        missing->Data()[12] =   0; missing->Data()[13] = 255; missing->Data()[14] =   0; missing->Data()[15] = 255;
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
    ASSERT1(retval != NULL); // not allowed to fail at this point
    delete texture;
    return retval;
}

GlTexture *GlTexture::Create (Texture const &texture, Uint32 flags)
{
    return Singleton::Gl().CreateGlTexture(texture, flags);
}

} // end of namespace Xrb
