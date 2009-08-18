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

class GLTexture;

/** @brief Provides convenience functions for interacting with the OpenGL API.
  */
namespace GL
{
    GLTexture const &GLTexture_OpaqueWhite ();

    /** Screen calls this in @ref Xrb::Screen::Create -- you shouldn't need
      * to call it directly.
      * @brief Sets up a bunch of rendering parameters and so forth.
      */
    void Initialize ();

    /** @brief Returns the current GL matrix mode.
      */
    GLint MatrixMode ();
    /** This function is not to be confused with @a MaxMatrixStackDepth
      * @brief Returns the current matrix stack depth of the given matrix mode.
      * @param matrix_mode The matrix mode of the desired stack depth.  Valid
      *                    values are GL_COLOR_MATRIX, GL_MODELVIEW,
      *                    GL_PROJECTION, and GL_TEXTURE.
      */
    GLint MatrixStackDepth (GLenum matrix_mode);
    /** @brief Returns the maximum matrix stack depth of the given matrix mode.
      * @param matrix_mode The matrix mode of the desired max stack depth.
      *                    Valid values are GL_COLOR_MATRIX, GL_MODELVIEW,
      *                    GL_PROJECTION, and GL_TEXTURE.
      */
    GLint MaxMatrixStackDepth (GLenum matrix_mode);
    /** @brief Returns true if GL_TEXTURE_2D is enabled
      */
    bool IsTexture2dOn ();

    /** Widget calls during @ref Xrb::Widget::Draw -- you shouldn't need to
      * call it directly.
      * @brief Used by Widget when setting the clipping planes and viewport.
      */
    void SetClipRect (ScreenCoordRect const &clip_rect);
} // end of namespace GL

} // end of namespace Xrb

#endif // !defined(_XRB_GL_HPP_)

