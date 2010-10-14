// ///////////////////////////////////////////////////////////////////////////
// xrb_gl.hpp by Victor Dods, created 2005/06/04
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_GL_HPP_)
#define _XRB_GL_HPP_

#include "xrb.hpp"

#if defined(__MACOSX__)
    #include <OpenGL/gl.h>
#elif defined(__IPHONEOS__)
    #include <OpenGLES/ES1/gl.h>
    #include <OpenGLES/ES1/glext.h>
    #define glOrtho glOrthof
    #define glClearDepth glClearDepthf
#else
    #include <GL/gl.h>
#endif

#include <map>
#include <vector>

#include "xrb_color.hpp"
#include "xrb_resourcelibrary.hpp"
#include "xrb_screencoord.hpp"

namespace Xrb
{

class Color;
class GlTexture;
class GlTextureAtlas;
class Texture;

// openGL-controlling singleton -- the main reason this exists is to control
// texture atlases.  GL_TEXTURE0 will be the active texture unit in general.
class Gl
{
public:

    Gl ();
    ~Gl ();

    // this is necessary for the cooperation of Singleton::InitializeGl and
    // the construction of the opaque white texture and various other texture
    // unit setup.  this method should be used only by Singleton::InitializeGl.
    void FinishInitialization ();

    // ///////////////////////////////////////////////////////////////////////
    // static methods
    // ///////////////////////////////////////////////////////////////////////

    /** Only the first boolean value of a multi-valued property will be returned.
      * Also, I lied about it being a frontend to glGetBooleanv.  It's actually
      * a frontend to Gl::Integer.
      * @brief Convenience frontend to glGetBooleanv for getting a single value.
      */
    static bool Boolean (GLenum name);
    /** Only the first integer value of a multi-valued property will be returned.
      * @brief Convenience frontend to glGetIntegerv for getting a single value.
      */
    static GLint Integer (GLenum name);

    // ///////////////////////////////////////////////////////////////////////
    // non-static "actually public" methods
    // ///////////////////////////////////////////////////////////////////////

#define USE_GL_STATE_WRAPPERS 1

#if USE_GL_STATE_WRAPPERS
    /// Frontend for glIsEnabled (used to avoid unnecessary changes of GL state)
    static bool IsEnabled (GLenum cap) { return Singleton::Gl().IsEnabled_(cap); }
    /// Frontend for glEnable (used to avoid unnecessary changes of GL state)
    static void Enable (GLenum cap) { Singleton::Gl().Enable_(cap); }
    /// Frontend for glDisable (used to avoid unnecessary changes of GL state)
    static void Disable (GLenum cap) { Singleton::Gl().Disable_(cap); }
    /// Frontend for glEnableClientState (used to avoid unnecessary changes of GL state)
    static void EnableClientState (GLenum cap) { Singleton::Gl().EnableClientState_(cap); }
    /// Frontend for glDisableClientState (used to avoid unnecessary changes of GL state)
    static void DisableClientState (GLenum cap) { Singleton::Gl().DisableClientState_(cap); }

    /// Returns the currently active texture unit (e.g. GL_TEXTURE0).
    static GLenum ActiveTexture () { return Singleton::Gl().ActiveTexture_(); }
    /// Frontend for glActiveTexture (used to avoid unnecessary changes of GL state)
    static void ActiveTexture (GLenum texture) { Singleton::Gl().ActiveTexture_(texture); }

    /// Returns currently active client texture unit (e.g. GL_TEXTURE0).
    static GLenum ClientActiveTexture () { return Singleton::Gl().ClientActiveTexture_(); }
    /// Frontend for glActiveTexture (used to avoid unnecessary changes of GL state)
    static void ClientActiveTexture (GLenum texture) { Singleton::Gl().ClientActiveTexture_(texture); }
#else // !USE_GL_STATE_WRAPPERS

    #define INLINE_GL_CALLS 0

    #if INLINE_GL_CALLS
    /// Frontend for glIsEnabled (used to avoid unnecessary changes of GL state)
    inline static bool IsEnabled (GLenum cap) { return glIsEnabled(cap) == GL_TRUE; }
    /// Frontend for glEnable (used to avoid unnecessary changes of GL state)
    inline static void Enable (GLenum cap) { glEnable(cap); }
    /// Frontend for glDisable (used to avoid unnecessary changes of GL state)
    inline static void Disable (GLenum cap) { glDisable(cap); }
    /// Frontend for glEnableClientState (used to avoid unnecessary changes of GL state)
    inline static void EnableClientState (GLenum cap) { glEnableClientState(cap); }
    /// Frontend for glDisableClientState (used to avoid unnecessary changes of GL state)
    inline static void DisableClientState (GLenum cap) { glDisableClientState(cap); }

    /// Returns the currently active texture unit (e.g. GL_TEXTURE0).
    inline static GLenum ActiveTexture () { return Integer(GL_ACTIVE_TEXTURE); }
    /// Frontend for glActiveTexture (used to avoid unnecessary changes of GL state)
    inline static void ActiveTexture (GLenum texture) { glActiveTexture(texture); }

    /// Returns currently active client texture unit (e.g. GL_TEXTURE0).
    inline static GLenum ClientActiveTexture () { return Integer(GL_CLIENT_ACTIVE_TEXTURE); }
    /// Frontend for glActiveTexture (used to avoid unnecessary changes of GL state)
    inline static void ClientActiveTexture (GLenum texture) { glClientActiveTexture(texture); }
    #else // !INLINE_GL_CALLS
    /// Frontend for glIsEnabled (used to avoid unnecessary changes of GL state)
    static bool IsEnabled (GLenum cap);
    /// Frontend for glEnable (used to avoid unnecessary changes of GL state)
    static void Enable (GLenum cap);
    /// Frontend for glDisable (used to avoid unnecessary changes of GL state)
    static void Disable (GLenum cap);
    /// Frontend for glEnableClientState (used to avoid unnecessary changes of GL state)
    static void EnableClientState (GLenum cap);
    /// Frontend for glDisableClientState (used to avoid unnecessary changes of GL state)
    static void DisableClientState (GLenum cap);

    /// Returns the currently active texture unit (e.g. GL_TEXTURE0).
    static GLenum ActiveTexture ();
    /// Frontend for glActiveTexture (used to avoid unnecessary changes of GL state)
    static void ActiveTexture (GLenum texture);

    /// Returns currently active client texture unit (e.g. GL_TEXTURE0).
    static GLenum ClientActiveTexture ();
    /// Frontend for glActiveTexture (used to avoid unnecessary changes of GL state)
    static void ClientActiveTexture (GLenum texture);
    #endif

#endif

    // ///////////////////////////////////////////////////////////////////////
    // non-static "public but not for general use" methods
    // ///////////////////////////////////////////////////////////////////////

    /** This texture is used for color biasing in multitexture mode.
      * @brief Returns the 1x1 opaque white utility texture.
      */
    GlTexture const &GlTexture_OpaqueWhite () const
    {
        ASSERT1(m_gltexture_opaque_white != NULL);
        return *m_gltexture_opaque_white;
    }

    /** For use only by GlTexture.
      * @brief Unregisters a texture-atlased GlTexture.
      */
    void UnregisterGlTexture (GlTexture &gltexture);

    /// Used by the GlTexture version of SetupTextureUnits.
    void SetupTextureUnit0 (GlTexture const &gltexture, Color const &color_mask, Color const &color_bias);
    /// Used by the GlTexture version of SetupTextureUnits.
    void SetupTextureUnit1 (Color const &color_bias);

    /** This setup is done in so many places that it was deemed
      * function-worthy, eliminating possible copy/paste errors.
      * @brief Sets up texture units 0 and 1 for texturing, color masking
      *        and color biasing.
      * @param texture_handle_to_bind_to_unit_0 The openGL texture handle to
      *                                         bind to texture unit 0.
      * @param color_mask The masking color to modulate the bound texture with.
      * @param color_bias The color bias to interpolate the results of
      *                   texture unit 0 with.
      */
    void SetupTextureUnits (
        GlTexture const &gltexture,
        Color const &color_mask,
        Color const &color_bias);
    /** This setup is done in so many places that it was deemed
      * function-worthy, eliminating possible copy/paste errors.
      * @brief Sets up opengl for non-textured rendering, using the given
      *        color mask and color bias.
      * @param color_mask The masking color (base color).
      * @param color_bias The color bias to interpolate the masking color with.
      */
    void SetupTextureUnits (
        Color const &color_mask,
        Color const &color_bias);

    // use these instead of glBindTexture.
    void BindAtlas (GlTextureAtlas const &atlas);
    void EnsureAtlasIsNotBound (GlTextureAtlas const &atlas);

    // these are useful for profiling the number of calls to glBindTexture
    // (which is a relatively expensive call, so minimizing this is good).
    Uint32 BindTextureCallHitCount () const { return m_bind_texture_call_hit_count; }
    Uint32 BindTextureCallMissCount () const { return m_bind_texture_call_miss_count; }
    Uint32 BindTextureCallCount () const { return m_bind_texture_call_hit_count + m_bind_texture_call_miss_count; }
    void ResetBindTextureCallCounts ();

    // these are useful for checking how efficiently packed the texture atlases are.
    Uint32 AllocatedTextureByteCount () const;
    Uint32 UsedTextureByteCount () const;

    // really only useful for developers -- helpful for actually
    // seeing how well your atlases are packed.
    void DumpAtlases (std::string const &path_prefix) const;

private:

    /// Backend for the static IsEnabled method.
    bool IsEnabled_ (GLenum cap);
    /// Backend for the static Enable method.
    void Enable_ (GLenum cap);
    /// Backend for the static Disable method.
    void Disable_ (GLenum cap);
    /// Backend for the static EnableClientState method.
    void EnableClientState_ (GLenum cap);
    /// Backend for the static DisableClientState method.
    void DisableClientState_ (GLenum cap);
    /// Backend for the static ActiveTexture accessor.
    GLenum ActiveTexture_ () { return m_active_texture; }
    /// Backend for the static ActiveTexture method.
    void ActiveTexture_ (GLenum texture);
    /// Backend for the static ClientActiveTexture accessor.
    GLenum ClientActiveTexture_ () { return m_client_active_texture; }
    /// Backend for the static ClientActiveTexture method.
    void ClientActiveTexture_ (GLenum texture);

    /// Ensures m_enable_map[cap] exists and contains the correct value.
    bool &EnableMapValue (GLenum cap);
    /// Allows access to the enabled state of GL_TEXTURE_2D for each texture unit
    bool IsTexture2dEnabled (GLenum texture);
    /// Specifies which capabilities apply to glEnableClientState.  All others apply to glEnable.
    static bool IsClientTextureUnitCapability (GLenum cap);
    /// Creates a texture-atlased GlTexture instance.  For use only by GlTexture.
    GlTexture *CreateGlTexture (Texture const &texture, Uint32 gltexture_flags);

    void AddAtlas (GlTextureAtlas *atlas);

    typedef std::map<GLenum, bool> EnableMap;
    typedef std::vector<GlTextureAtlas *> AtlasVector;

    EnableMap m_enable_map;
    GLenum m_active_texture;
    GLenum m_client_active_texture;
    Color m_texture_unit_1_env_color;

    GlTexture *m_gltexture_opaque_white;
    AtlasVector m_atlas;
    GlTextureAtlas const *m_atlas_bound_to_unit_0;
    Uint32 m_bind_texture_call_hit_count;
    Uint32 m_bind_texture_call_miss_count;

    friend class GlTexture;
};

} // end of namespace Xrb

#endif // !defined(_XRB_GL_HPP_)

