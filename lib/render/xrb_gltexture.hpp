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
#include "xrb_resourcelibrary.hpp"
#include "xrb_resourceloadparameters.hpp"
#include "xrb_screencoord.hpp"

namespace Xrb
{

class Texture;

/** Creating a texture using this class will load the texture data into the
  * texture memory using OpenGL, and provide a texture handle, with which
  * the texture will be referred to when rendering it.
  * @brief Texture object which can be used directly to render using OpenGL.
  */
class GlTexture
{
public:

    enum
    {
        NONE                = 0,
        USES_SEPARATE_ATLAS = (1 << 0)
    };

    class LoadParameters : public ResourceLoadParameters
    {
    public:

        LoadParameters (std::string const &path, Uint32 flags = NONE);

        std::string const &Path () const { return m_path; }
        Uint32 Flags () const { return m_flags; }
        bool UsesSeparateAtlas () const { return (m_flags & USES_SEPARATE_ATLAS) != 0; }

        virtual std::string ResourceName () const;
        virtual bool IsLessThan (ResourceLoadParameters const &p) const;
        virtual void Fallback ();
        virtual void Print (FILE *fptr) const;

    private:

        std::string m_path;
        Uint32 m_flags;
    }; // end of class GlTexture::LoadParameters

    /** Causes the texture to be unloaded from texture memory.
      * @brief Destructor.
      */
    ~GlTexture ();

    /** NOTE: Unless you know what you're doing, use this instead of Create.
      * Generally in application code, you'll only use Resource<GlTexture>
      * instead of GlTexture directly.  This is a frontend to
      * Singleton::ResourceLibrary().Load<GlTexture>()
      * @brief Loads a GlTexture via the ResourceLibrary (convenience function)
      */
    static Resource<GlTexture> Load (std::string const &path, Uint32 flags = NONE)
    {
        return Singleton::ResourceLibrary().Load<GlTexture>(GlTexture::Create, new LoadParameters(path, flags));
    }
    static Resource<GlTexture> LoadMissing ()
    {
        LoadParameters *load_parameters = new LoadParameters("");
        load_parameters->Fallback();
        return Singleton::ResourceLibrary().Load<GlTexture>(GlTexture::Create, load_parameters);
    }

    /** Loads the image given by the path into a Texture object,
      * creates the OpenGL mipmaps and gets a handle to the OpenGL texture.
      * @brief Creates a new GlTexture object from a texture loaded using
      *        the given load parameters.
      */
    static GlTexture *Create (ResourceLoadParameters const &p);
    /** This is actually a frontend to Singleton::Gl().CreateGlTexture.
      * The parameters after texture have the same usage as
      * GlTextureLoadParameters.
      * @brief Creates a new GlTexture object from the given texture.
      */
    static GlTexture *Create (Texture const &texture, Uint32 flags = NONE);

    GlTextureAtlas const &Atlas () const { return m_atlas; }
    GlTextureAtlas &Atlas () { return m_atlas; }
    ScreenCoordVector2 const &Size () const { return m_size; }
    ScreenCoord Width () const { return m_size[Dim::X]; }
    ScreenCoord Height () const { return m_size[Dim::Y]; }
    ScreenCoordVector2 TextureCoordinateBottomLeft () const;
    Sint16 const *TextureCoordinateArray () const { return m_texture_coordinate_array; }
    bool UsesSeparateAtlas () const { return (m_flags & USES_SEPARATE_ATLAS) != 0; }

private:

    // this is really only useful for square, power-of-2-sized textures placed in
    // a GlTextureAtlas (i.e. only GlTextureAtlas needs to use this).
    ScreenCoordVector2 TextureCoordinateCenter () const;

    // for use only by GlTextureAtlas
    GlTexture (
        GlTextureAtlas &atlas,
        ScreenCoordVector2 const &size,
        ScreenCoordRect const &texture_coordinate_rect,
        Uint32 flags);

    GlTextureAtlas &m_atlas;
    ScreenCoordVector2 const m_size;
    // for use directly in opengl as texture coordinates
    Sint16 m_texture_coordinate_array[8]; // TODO: all 8 different rotations and orientations?
    Uint32 const m_flags;

    friend class GlTextureAtlas;
}; // end of class GlTexture

} // end of namespace Xrb

#endif // !defined(_XRB_GLTEXTURE_HPP_)

