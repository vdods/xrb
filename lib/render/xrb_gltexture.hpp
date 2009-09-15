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

#include "xrb_gl.hpp"
#include "xrb_resourceloadparameters.hpp"
#include "xrb_screencoord.hpp"

namespace Xrb
{

class Texture;

class GlTextureLoadParameters : public ResourceLoadParameters
{
public:

    Uint32 const m_flags;

    enum
    {
        NONE                = 0,
        USES_SEPARATE_ATLAS = (1 << 0)
    };

    GlTextureLoadParameters (Uint32 flags = NONE) : m_flags(flags) { }

    bool UsesSeparateAtlas () const { return (m_flags & USES_SEPARATE_ATLAS) != 0; }

    virtual std::string Name () const;
    virtual bool IsLessThan (ResourceLoadParameters const &other_parameters) const;
    virtual void Print (FILE *fptr) const;
}; // end of class GlTextureLoadParameters

/** Creating a texture using this class will load the texture data into the
  * texture memory using OpenGL, and provide a texture handle, with which
  * the texture will be referred to when rendering it.
  * @brief Texture object which can be used directly to render using OpenGL.
  */
class GlTexture
{
public:

    /** Causes the texture to be unloaded from texture memory.
      * @brief Destructor.
      */
    ~GlTexture ();

    /** Loads the image given by the path into a Texture object,
      * creates the OpenGL mipmaps and gets a handle to the OpenGL texture.
      * @brief Creates a new GlTexture object from a texture loaded from
      *        the given path, using the given load parameters.
      */
    static GlTexture *Create (std::string const &path, ResourceLoadParameters const *parameters);
    /** This is actually a frontend to Singleton::Gl().CreateGlTexture.
      * @brief Creates a new GlTexture object from the given texture, using
      *        the given load parameters.
      */
    static GlTexture *Create (Texture const &texture, GlTextureLoadParameters const &load_parameters);

    GlTextureAtlas const &Atlas () const { return m_atlas; }
    GlTextureAtlas &Atlas () { return m_atlas; }
    /** This will be used to return the handle when specifying the texture
      * to bind to GL_TEXTURE_2D when rendering textures.
      * @brief Returns the OpenGL texture handle for this texture.
      */
    GLuint Handle () const;
    /** @brief Returns the size vector of the texture.
      */
    ScreenCoordVector2 const &Size () const { return m_size; }
    /** @brief Returns the width of the texture.
      */
    ScreenCoord Width () const { return m_size[Dim::X]; }
    /** @brief Returns the height of the texture.
      */
    ScreenCoord Height () const { return m_size[Dim::Y]; }
    /** @brief Returns the texture's atlas texture coordinate offset.
      */
    ScreenCoordVector2 const &TextureCoordOffset () const { return m_texture_coord_offset; }
    /** @brief Returns the GlTextureLoadParameters used to create this GlTexture.
      */
    GlTextureLoadParameters const &LoadParameters () const { return m_load_parameters; }

private:

    // for use only by GlTextureAtlas
    GlTexture (
        GlTextureAtlas &atlas,
        ScreenCoordVector2 const &size,
        ScreenCoordVector2 const &texture_coord_offset,
        GlTextureLoadParameters const &load_parameters)
        :
        m_atlas(atlas),
        m_size(size),
        m_texture_coord_offset(texture_coord_offset),
        m_load_parameters(load_parameters)
    { }

    GlTextureAtlas &m_atlas;
    ScreenCoordVector2 const m_size;
    ScreenCoordVector2 const m_texture_coord_offset;
    GlTextureLoadParameters const m_load_parameters;

    friend class GlTextureAtlas;
}; // end of class GlTexture

} // end of namespace Xrb

#endif // !defined(_XRB_GLTEXTURE_HPP_)

