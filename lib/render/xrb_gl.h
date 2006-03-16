// ///////////////////////////////////////////////////////////////////////////
// xrb_gl.h by Victor Dods, created 2005/06/04
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_GL_H_)
#define _XRB_GL_H_

#include "xrb.h"

#include "xrb_screencoord.h"

namespace Xrb
{

/** @brief Provides convenience functions for interacting with the
  *        less-than-attractive OpenGL API.
  *        
  */
namespace GL
{
    /** Screen calls this in @ref Xrb::Screen::Create -- you shouldn't need
      * to call it directly.
      * @brief Sets up a bunch of rendering parameters and so forth.
      */
    void Initialize ();

    /** @brief Returns the current GL matrix mode.
      */
    GLint GetMatrixMode ();
    /** @brief Returns true if GL_TEXTURE_2D is enabled
      */
    bool GetIsTexture2dOn ();
    
    /** Widget calls during @ref Xrb::Widget::Draw -- you shouldn't need to
      * call it directly.
      * @brief Used by Widget when setting the clipping planes and viewport.
      */
    void SetClipRect (ScreenCoordRect const &clip_rect);
} // end of namespace GL

} // end of namespace Xrb

#endif // !defined(_XRB_GL_H_)

