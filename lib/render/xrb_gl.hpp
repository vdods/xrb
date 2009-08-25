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

#include "xrb_screencoord.hpp"

namespace Xrb
{

class Color;
class GLTexture;

// openGL-controlling singleton -- the main reason this exists is to control
// texture atlases.
class Gl
{
public:

    Gl ();
    ~Gl ();

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

    /** Widget calls during @ref Xrb::Widget::Draw -- you shouldn't need to
      * call it directly.
      * @brief Used by Widget when setting the clipping planes and viewport.
      */
    static void SetClipRect (ScreenCoordRect const &clip_rect);

    // ///////////////////////////////////////////////////////////////////////
    // non-static methods
    // ///////////////////////////////////////////////////////////////////////

    /** This texture is permanently bound to texture unit 1, used for color
      * biasing in multitexture mode.
      * @brief Returns the 1x1 opaque white utility texture.
      */
    GLTexture const *GLTexture_OpaqueWhite () const
    {
        ASSERT1(m_gltexture_opaque_white != NULL);
        return m_gltexture_opaque_white;
    }

    /** This setup is done in so many places that it was deemed 
      * function-worthy, eliminating possible copy/paste errors.
      * @brief Sets up texture units 0 and 1 for texturing, color masking
      *        and color biasing.
      * @param texture_to_bind_to_unit_0 The GLTexture to bind to texture unit 0.
      * @param color_mask The masking color modulate the bound texture with.
      * @param color_bias The color bias to interpolate the results of
      *                   texture unit 0 with.
      * @note When this function returns, texture unit 1 will be active.
      */
    void SetupTextureUnits (
        GLTexture const *texture_to_bind_to_unit_0,
        Color const &color_mask,
        Color const &color_bias); 

    // these are useful for profiling the number of calls to glBindTexture
    // (which is a relatively expensive call, so minimizing this is good).
    Uint32 BindTextureCallHitCount () const { return m_bind_texture_call_hit_count; }
    Uint32 BindTextureCallMissCount () const { return m_bind_texture_call_miss_count; }
    Uint32 BindTextureCallCount () const { return m_bind_texture_call_hit_count + m_bind_texture_call_miss_count; }
    void ResetBindTextureCallCounts ();

private:

    GLTexture *m_gltexture_opaque_white;
    GLTexture const *m_gltexture_bound_to_unit_0;

    Uint32 m_bind_texture_call_hit_count;
    Uint32 m_bind_texture_call_miss_count;
};

} // end of namespace Xrb

#endif // !defined(_XRB_GL_HPP_)

