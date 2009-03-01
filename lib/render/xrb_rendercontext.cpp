// ///////////////////////////////////////////////////////////////////////////
// xrb_rendercontext.cpp by Victor Dods, created 2005/02/07
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_rendercontext.hpp"

#include "xrb_gl.hpp"

namespace Xrb
{

bool RenderContext::MaskAndBiasWouldResultInNoOp () const
{
    return m_bias_color[Dim::A] == 0.0f && m_color_mask[Dim::A] == 0.0f;
}

bool RenderContext::MaskAndBiasWouldResultInNoOp (ColorCoord color_alpha_channel_value) const
{
    return m_bias_color[Dim::A] == 0.0f && (m_color_mask[Dim::A] == 0.0f || color_alpha_channel_value == 0.0f);
}

void RenderContext::SetupGLClipRect () const
{
    GL::SetClipRect(m_clip_rect);
}

} // end of namespace Xrb
