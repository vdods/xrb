// ///////////////////////////////////////////////////////////////////////////
// xrb_gltexture.hpp by Victor Dods, created 2005/06/04
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_GLTEXTURE_HPP_)
#define _XRB_GLTEXTURE_HPP_

#include "xrb.hpp"

#include <string>

#include "xrb_screencoord.hpp"

namespace Xrb
{

// notes:
//
// when glTexImage2D is called, the (currently nonexistent) singleton for
// video options will be read in order to determine what the proper internal
// format to specify (i.e. full quality 32 bit RGBA, or space-saving 16 bit
// RGBA, etc).
//

class Texture;

/** Creating a texture using this class will load the texture data into the
  * texture memory using OpenGL, and provide a texture handle, with which
  * the texture will be referred to when rendering it.
  * @brief Texture object which can be used directly to render using OpenGL.
  */
class GLTexture
{
public:

    /** Causes the texture to be unloaded from texture memory.
      * @brief Destructor.
      */
    ~GLTexture ();

    /** Loads the image given by the filename into a Texture object,
      * creates the OpenGL mipmaps and gets a handle to the OpenGL texture.
      * @brief Create a new GLTexture object from a texture loaded from
      *        the given filename.
      */
    static GLTexture *Create (std::string const &filename);
    /** Loads the image given by the filename into a Texture object,
      * creates the OpenGL mipmaps and gets a handle to the OpenGL texture.
      * @brief Create a new GLTexture object from an already-loaded Texture
      *        object.
      */
    static GLTexture *Create (Texture *texture);

    /** This will be used to return the handle when specifying the texture
      * to bind to GL_TEXTURE_2D when rendering textures.
      * @brief Returns the OpenGL texture handle for this texture.
      */
    inline GLuint Handle () const
    {
        return m_handle;
    }
    /** @brief Returns the size vector of the texture.
      */
    inline ScreenCoordVector2 const &Size () const
    {
        return m_size;
    }
    /** @brief Returns the width of the texture.
      */
    inline ScreenCoord Width () const
    {
        return m_size[Dim::X];
    }
    /** @brief Returns the height of the texture.
      */
    inline ScreenCoord Height () const
    {
        return m_size[Dim::Y];
    }

private:

    // private constructor so you must use Create()
    GLTexture ();

    void GenerateTexture (Texture *texture, bool delete_original_texture);
    void DeleteTexture ();

    GLuint m_handle;
    ScreenCoordVector2 m_size;
}; // end of class GLTexture

} // end of namespace Xrb

#endif // !defined(_XRB_GLTEXTURE_HPP_)

