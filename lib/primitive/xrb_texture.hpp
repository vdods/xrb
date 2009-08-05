// ///////////////////////////////////////////////////////////////////////////
// xrb_texture.hpp by Victor Dods, created 2005/04/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_TEXTURE_HPP_)
#define _XRB_TEXTURE_HPP_

#include "xrb.hpp"

#include <string>

#include "xrb_screencoord.hpp"

namespace Xrb
{

// notes:
//
// the png reading function should detect what channels/bit depths are
// used and set some enums appropriately.  these enums will be read later
// by GLTextureHandle when glTexImage2D is called, so that the correct
// flags are passed.
//

/** Texture objects should be instantiated by using one of the two Create
  * methods provided.
  *
  * File I/O is done using the PNG file format.
  *
  * The raw texture data can be accessed directly through Data.
  *
  * (not yet implemented:) The texture data can be stored/read/written
  * in several bit-depth formats, given by @ref Xrb::Texture::Format.
  *
  * @brief Directly stores, reads, and writes rasterized, pixel data.
  */
class Texture
{
public:

    // most of the PNG formats map onto a subset of these, but the
    // mapping is not necessarily invertible.  these formats
    // are geared more towards the OpenGL formats.
    enum Format
    {
        GRAYSCALE1 = 0,     // 1 bit grayscale (monochrome)
        GRAYSCALE4,         // 4 bit grayscale
        GRAYSCALE8,         // 8 bit grayscale
        GRAYSCALE16,        // 16 bit grayscale

        GRAYSCALE_ALPHA1,   // 1 bit grayscale (monochrome) + 1 bit alpha
        GRAYSCALE_ALPHA4,   // 4 bit grayscale + 4 bit alpha
        GRAYSCALE_ALPHA8,   // 8 bit grayscale + 8 bit alpha
        GRAYSCALE_ALPHA16,  // 16 bit grayscale + 16 bit alpha

        RGB332,             // 3 bit red, 3 bit green, 2 bit blue
        RGB4,               // 4 bit red, 4 bit green, 4 bit blue
        RGB565,             // 5 bit red, 6 bit green, 5 bit blue
        RGB8,               // 8 bit red, 8 bit green, 8 bit blue
        RGB16,              // 16 bit red, 16 bit green, 16 bit blue

        RGB5_A1,            // 5 bit red, 5 bit green, 5 bit blue, 1 bit alpha
        RGBA4,              // 4 bit red, 4 bit green, 4 bit blue, 4 bit alpha
        RGBA8,              // 8 bit red, 8 bit green, 8 bit blue, 8 bit alpha
        RGBA16,             // 16 bit red, 16 bit green, 16 bit blue, 16 bit alpha

        FORMAT_COUNT
    }; // end enum Format

    ~Texture ();

    static Texture *Create (std::string const &filename);
    static Texture *Create (ScreenCoordVector2 const &size, bool zero_out_the_data);
    //TODO void Write (std::string const &filename);

    inline ScreenCoordVector2 GetSize () const
    {
        ASSERT1(m_surface != NULL);
        return ScreenCoordVector2(m_surface->w, m_surface->h);
    }
    inline ScreenCoord GetWidth () const
    {
        ASSERT1(m_surface != NULL);
        return static_cast<ScreenCoord>(m_surface->w);
    }
    inline ScreenCoord GetHeight () const
    {
        ASSERT1(m_surface != NULL);
        return static_cast<ScreenCoord>(m_surface->h);
    }
    inline Uint32 DataLength () const
    {
        ASSERT1(m_surface != NULL);
        return static_cast<Uint32>(m_surface->pitch) * static_cast<Uint32>(m_surface->h);
    }
    inline Uint8 *Data () const
    {
        ASSERT1(m_surface != NULL);
        return static_cast<Uint8 *>(m_surface->pixels);
    }

private:

    // private constructor so you must use Create()
    Texture ();

    SDL_Surface *m_surface;
}; // end of class Texture

} // end of namespace Xrb

#endif // !defined(_XRB_TEXTURE_HPP_)
