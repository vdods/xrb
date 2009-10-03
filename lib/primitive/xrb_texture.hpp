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

#include "xrb_pal.hpp"
#include "xrb_screencoord.hpp"

namespace Xrb
{

/** Texture objects should be instantiated by using one of the many
  * lovely Create methods provided.
  *
  * File I/O is done using the PNG file format.
  *
  * The raw texture data can be accessed directly through Data.
  *
  * @brief Directly stores, reads, and writes rasterized, pixel data.
  */
class Texture
{
public:

    enum InitialState { UNINITIALIZED = 0, CLEAR = 1 };

    ~Texture ();

    static Texture *Create (std::string const &path);
    static Texture *Create (ScreenCoordVector2 const &size, InitialState initial_state);
    // creates a texture using the given size and pixel buffer.  the returned
    // Texture object will take ownership of the buffer, and delete[] it upon destruction.
    // this method trusts you to hand in a pointer to a sufficiently large pixel buffer.
    static Texture *Create (ScreenCoordVector2 const &size, Uint8 *data);
    // creates a mipmap of this texture, having exactly half the dimensions.
    // the texture width and height are required to each be a power of 2 that
    // is greater than 1.
    Texture *CreateMipmap () const;

    // this is really just a convenience frontend to Singleton::Pal().SaveImage
    Pal::Status Save (std::string const &path) const;

    ScreenCoordVector2 Size () const
    {
        ASSERT1(m_data != NULL && "uninitialized Texture");
        return m_size;
    }
    ScreenCoord Width () const
    {
        ASSERT1(m_data != NULL && "uninitialized Texture");
        return m_size[Dim::X];
    }
    ScreenCoord Height () const
    {
        ASSERT1(m_data != NULL && "uninitialized Texture");
        return m_size[Dim::Y];
    }
    Uint8 BitDepth () const
    {
        ASSERT1(m_data != NULL && "uninitialized Texture");
        return m_bit_depth;
    }
    Uint32 DataLength () const
    {
        ASSERT1(m_data != NULL && "uninitialized Texture");
        return m_data_length;
    }
    Uint8 *Data () const
    {
        ASSERT1(m_data != NULL && "uninitialized Texture");
        return m_data;
    }
    Uint8 *Pixel (ScreenCoord x, ScreenCoord y) const
    {
        ASSERT1(m_data != NULL && "uninitialized Texture");
        ASSERT1(x >= 0 && x < m_size[Dim::X]);
        ASSERT1(y >= 0 && y < m_size[Dim::Y]);
        return m_data + (y*m_size[Dim::X]+x)*4;
    }

private:

    // private constructor so you must use Create()
    Texture ();

    ScreenCoordVector2 m_size;
    Uint8 m_bit_depth;
    Uint32 m_data_length;
    Uint8 *m_data;
}; // end of class Texture

} // end of namespace Xrb

#endif // !defined(_XRB_TEXTURE_HPP_)
