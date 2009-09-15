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
// GlTextureLoadParameters
// ///////////////////////////////////////////////////////////////////////////

std::string GlTextureLoadParameters::Name () const
{
    return "Xrb::GlTextureLoadParameters";
}

bool GlTextureLoadParameters::IsLessThan (ResourceLoadParameters const &other_parameters) const
{
    GlTextureLoadParameters const &other = *DStaticCast<GlTextureLoadParameters const *>(&other_parameters);
    return m_flags < other.m_flags;
}

void GlTextureLoadParameters::Print (FILE *fptr) const
{
    fprintf(stderr, "flags =");
    if (m_flags == NONE)
        fprintf(stderr, " NONE");
    if (UsesSeparateAtlas())
        fprintf(stderr, " USES_SEPARATE_ATLAS");
}

// ///////////////////////////////////////////////////////////////////////////
// GlTexture
// ///////////////////////////////////////////////////////////////////////////

GlTexture::~GlTexture ()
{
    Singleton::Gl().UnregisterGlTexture(*this);
}

GlTexture *GlTexture::Create (std::string const &path, ResourceLoadParameters const *parameters)
{
    GlTexture *retval = NULL;

    Texture *texture = Texture::Create(path);
    if (texture == NULL)
        return retval;

    GlTextureLoadParameters default_load_parameters;
    GlTextureLoadParameters const *params = &default_load_parameters;
    if (parameters != NULL)
        params = DStaticCast<GlTextureLoadParameters const *>(parameters);
    retval = Singleton::Gl().CreateGlTexture(*texture, *params);
    delete texture;

    return retval;
}

GlTexture *GlTexture::Create (Texture const &texture, GlTextureLoadParameters const &load_parameters)
{
    return Singleton::Gl().CreateGlTexture(texture, load_parameters);
}

GLuint GlTexture::Handle () const
{
    return m_atlas.Handle();
}

} // end of namespace Xrb
