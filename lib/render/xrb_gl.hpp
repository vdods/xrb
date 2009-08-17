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

// ///////////////////////////////////////////////////////////////////////////
// a bunch of platform-specific defines and whatnot necessary to include the
// correct openGL headers.  reference: SDL.
// ///////////////////////////////////////////////////////////////////////////

#ifdef __WIN32__
    // don't include a bunch of useless winblows crap.
    #define WIN32_LEAN_AND_MEAN
    // apparently defining NOMINMAX fixes some problems winblows has (reference: SDL)
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
#endif

#if defined(__APPLE__)
    // lets us know what version of Mac OS X we're compiling on
    #include "AvailabilityMacros.h"
    #ifdef MAC_OS_X_VERSION_10_3
        #include "targetconditionals.h"
        #if TARGET_OS_IPHONE
            #undef __IPHONEOS__
            #define __IPHONEOS__ 1
            #undef __MACOSX__
        #else
            #undef __MACOSX__
            #define __MACOSX__  1
        #endif
    #else
        #undef __MACOSX__
        #define __MACOSX__  1
    #endif
#endif

// headers for openGL
#if defined(__MACOSX__)
    #include <OpenGL/gl.h>
#else
    #include <GL/gl.h>
#endif

// ///////////////////////////////////////////////////////////////////////////
// end of ugliness
// ///////////////////////////////////////////////////////////////////////////

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

