// ///////////////////////////////////////////////////////////////////////////
// xrb_rendercontext.cpp by Victor Dods, created 2005/02/07
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_rendercontext.h"

#include "xrb_gl.h"

namespace Xrb
{

void RenderContext::SetupGLClipRect ()
{
    GL::SetClipRect(m_clip_rect);
}

} // end of namespace Xrb