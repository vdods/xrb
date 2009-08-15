// ///////////////////////////////////////////////////////////////////////////
// xrb_texture.cpp by Victor Dods, created 2005/04/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_texture.hpp"

#include <string.h>

#include "SDL_image.h"
// #include "png.h"

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
//     retval->m_surface = LoadPNG(filename.c_str());
    if (retval->m_surface == NULL)
    {
        fprintf(stderr, "Texture::Create(); error while loading \"%s\"\n", filename.c_str());
        DeleteAndNullify(retval);
        return NULL;
    }

    // TODO: real pixel format coping
    ASSERT1(retval->m_surface->format->BitsPerPixel == 32);

    return retval;
}

Texture *Texture::Create (
    ScreenCoordVector2 const &size,
    bool const zero_out_the_data)
{
    Texture *retval = NULL;

    ASSERT1(size[Dim::X] > 0 && size[Dim::Y] > 0);

    retval = new Texture();
    retval->m_surface = SDL_CreateRGBSurface(0, size[Dim::X], size[Dim::Y], 32, SDL_RMASK, SDL_GMASK, SDL_BMASK, SDL_AMASK);
    if (retval->m_surface == NULL)
        DeleteAndNullify(retval);
    else if (zero_out_the_data)
        memset(retval->Data(), 0, retval->DataLength());

    return retval;
}

Texture::Texture ()
{
    m_surface = NULL;
}
/*
SDL_Surface *Texture::LoadPNG (std::string const &filename)
{
    // the code in this function is based on the code from example.c
    // in the libpng documentation.

    png_structp png_ptr;
    png_infop info_ptr;
    unsigned int sig_read = 0;
    FILE *fp;

    if ((fp = fopen(filename.c_str(), "rb")) == NULL)
    {
        fprintf(stderr, "Texture::LoadPNG(\"%s\"); error opening file\n", filename.c_str());
        return NULL;
    }

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
    {
        fclose(fp);
        fprintf(stderr, "Texture::LoadPNG(\"%s\"); error reading PNG file\n", filename.c_str());
        return NULL;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        fclose(fp);
        fprintf(stderr, "Texture::LoadPNG(\"%s\"); error reading PNG file\n", filename.c_str());
        return NULL;
    }

    // Set error handling if you are using the setjmp/longjmp method (this is
    // the normal method of doing things with libpng).  REQUIRED unless you
    // set up your own error handlers in the png_create_read_struct() earlier.
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
        fprintf(stderr, "Texture::LoadPNG(\"%s\"); error reading PNG file\n", filename.c_str());
        return NULL;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, sig_read);
    png_read_info(png_ptr, info_ptr);

    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, int_p_NULL, int_p_NULL);

    // create the SDL_Surface
    SDL_Surface *surface = SDL_CreateRGBSurface(0, width, height, 32, SDL_RMASK, SDL_GMASK, SDL_BMASK, SDL_AMASK);
    if (surface == NULL)
    {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
        fprintf(stderr, "Texture::LoadPNG(\"%s\"); error creating SDL_Surface\n", filename.c_str());
        return NULL;
    }

    // Expand paletted colors into true RGB triplets
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);

    // Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_gray_1_2_4_to_8(png_ptr);

    // Expand paletted or RGB images with transparency to full alpha channels
    // so the data will be available as RGBA quartets.
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);

    png_read_update_info(png_ptr, info_ptr);
    png_bytep *row_pointers = (png_bytepp)png_malloc(png_ptr, height*sizeof(png_bytep));
    for (Uint32 row = 0; row < height; row++)
        row_pointers[row] = png_bytep(surface->pixels) + row*surface->pitch;

    png_read_image(png_ptr, row_pointers);
    png_read_end(png_ptr, info_ptr);
    // At this point you have read the entire image

    if (color_type != PNG_COLOR_TYPE_RGB && color_type != PNG_COLOR_TYPE_RGB_ALPHA)
    {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
        SDL_FreeSurface(surface);
        fprintf(stderr, "Texture::LoadPNG(\"%s\"); unsupported PNG color type\n", filename.c_str());
        return NULL;
    }

    // close the file and the png stuff
    fclose(fp);

    // now we're done with the png stuff
    png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);

    return surface;
}
*/
} // end of namespace Xrb
