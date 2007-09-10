// ///////////////////////////////////////////////////////////////////////////
// xrb_gl.cpp by Victor Dods, created 2005/06/04
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_gl.h"

namespace Xrb
{

void GL::Initialize ()
{
//     fprintf(stderr, "GL::Initialize();\n");
//
//     GLint max_texture_size;
//     glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
//     fprintf(stderr, "\tGL_MAX_TEXTURE_SIZE = %d\n", max_texture_size);

    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);

    glClearColor(0, 0, 0, 0);
    glShadeModel(GL_SMOOTH);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_LSB_FIRST, 0);
    glPixelStorei(GL_UNPACK_LSB_FIRST, 0);
}

GLint GL::GetMatrixMode ()
{
    GLint matrix_mode;
    glGetIntegerv(GL_MATRIX_MODE, &matrix_mode);
    return matrix_mode;
}

GLint GL::GetMatrixStackDepth (GLenum const matrix_mode)
{
    GLint stack_depth;
    switch (matrix_mode)
    {
        case GL_COLOR:      glGetIntegerv(GL_COLOR_MATRIX_STACK_DEPTH, &stack_depth); break;
        case GL_MODELVIEW:  glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &stack_depth); break;
        case GL_PROJECTION: glGetIntegerv(GL_PROJECTION_STACK_DEPTH, &stack_depth); break;
        case GL_TEXTURE:    glGetIntegerv(GL_TEXTURE_STACK_DEPTH, &stack_depth); break;
        default: ASSERT0(false && "Invalid matrix mode"); break;
    }
    return stack_depth;
}

GLint GL::GetMaxMatrixStackDepth (GLenum const matrix_mode)
{
    GLint max_stack_depth;
    switch (matrix_mode)
    {
        case GL_COLOR:      glGetIntegerv(GL_MAX_COLOR_MATRIX_STACK_DEPTH, &max_stack_depth); break;
        case GL_MODELVIEW:  glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &max_stack_depth); break;
        case GL_PROJECTION: glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, &max_stack_depth); break;
        case GL_TEXTURE:    glGetIntegerv(GL_MAX_TEXTURE_STACK_DEPTH, &max_stack_depth); break;
        default: ASSERT0(false && "Invalid matrix mode"); break;
    }
    return max_stack_depth;
}

bool GL::GetIsTexture2dOn ()
{
    GLboolean is_texture_2d_on;
    glGetBooleanv(GL_TEXTURE_2D, &is_texture_2d_on);
    return is_texture_2d_on == GL_TRUE;
}

void GL::SetClipRect (ScreenCoordRect const &clip_rect)
{
    ASSERT1(clip_rect.GetIsValid());

    // set up the GL projection matrix here.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(
        clip_rect.GetLeft(), clip_rect.GetRight(),
        clip_rect.GetBottom(), clip_rect.GetTop(),
        -1.0, 1.0); // these values (-1, 1) are arbitrary

    // set up the viewport which is the rectangle on screen which
    // will be rendered to.  this also properly sets up the clipping
    // planes.
    glViewport(
        clip_rect.GetLeft(),
        clip_rect.GetBottom(),
        clip_rect.GetWidth(),
        clip_rect.GetHeight());
}

} // end of namespace Xrb
