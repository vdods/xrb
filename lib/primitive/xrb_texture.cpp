// ///////////////////////////////////////////////////////////////////////////
// xrb_texture.cpp by Victor Dods, created 2005/04/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_texture.h"

#include <string.h>

#include "SDL_image.h"

#if defined(WORDS_BIGENDIAN)
    #define SDL_RMASK 0xFF000000
    #define SDL_GMASK 0x00FF0000
    #define SDL_BMASK 0x0000FF00
    #define SDL_AMASK 0x000000FF
#else // !defined(WORDS_BIGENDIAN)
    #define SDL_RMASK 0x000000FF
    #define SDL_GMASK 0x0000FF00
    #define SDL_BMASK 0x00FF0000
    #define SDL_AMASK 0xFF000000
#endif // !defined(WORDS_BIGENDIAN)

namespace Xrb
{

Texture::~Texture ()
{
    if (m_surface != NULL)
        SDL_FreeSurface(m_surface);
}

Texture *Texture::Create (std::string const &filename)
{
    Texture *retval = NULL;

    if (filename.length() == 0)
        return retval;

    retval = new Texture();
    retval->m_surface = IMG_Load(filename.c_str());
    if (retval->m_surface == NULL)
    {
        fprintf(stderr, "Texture::Create(); error while loading \"%s\"\n", filename.c_str());
        DeleteAndNullify(retval);
    }

    // TODO: real pixel format coping
    ASSERT1(retval->m_surface->format->BitsPerPixel == 32)
        
    return retval;
}

Texture *Texture::Create (
    ScreenCoordVector2 const &size,
    bool const zero_out_the_data)
{
    Texture *retval = NULL;

    ASSERT1(size[Dim::X] > 0 && size[Dim::Y] > 0)

    retval = new Texture();
    retval->m_surface = SDL_CreateRGBSurface(0, size[Dim::X], size[Dim::Y], 32, SDL_RMASK, SDL_GMASK, SDL_BMASK, SDL_AMASK);
    if (retval->m_surface == NULL)
        DeleteAndNullify(retval);
    else if (zero_out_the_data)
        memset(retval->GetData(), 0, retval->GetDataLength());
    
    return retval;
}

Texture::Texture ()
{
    m_surface = NULL;
}

} // end of namespace Xrb
