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

#include "xrb_texture.h"
#include "xrb_gltexture.h"

namespace Xrb
{

namespace {
    static GLTexture *g_gltexture_white = NULL;
}

void GL::Initialize ()
{
    GLint temp;

    fprintf(stderr, "GL::Initialize();\n");
    fprintf(stderr, "    GL_VENDOR = \"%s\"\n", glGetString(GL_VENDOR));
    fprintf(stderr, "    GL_RENDERER = \"%s\"\n", glGetString(GL_RENDERER));
    fprintf(stderr, "    GL_VERSION = \"%s\"\n", glGetString(GL_VERSION));
    fprintf(stderr, "    GL_EXTENSIONS = \"%s\"\n", glGetString(GL_EXTENSIONS));
    glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &temp);
    fprintf(stderr, "    GL_MAX_TEXTURE_UNITS_ARB = %d\n", temp);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &temp);
    fprintf(stderr, "    GL_MAX_TEXTURE_SIZE = %d\n", temp);

    // stuff related to texture byte order and alignment.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_LSB_FIRST, 0);
    glPixelStorei(GL_UNPACK_LSB_FIRST, 0);

    // initialize the singleton helper texture(s)
    {
        Texture *white = Texture::Create(ScreenCoordVector2(1, 1), true);
        white->GetData()[0] = 255;
        white->GetData()[1] = 255;
        white->GetData()[2] = 255;
        white->GetData()[3] = 255;
        g_gltexture_white = GLTexture::Create(white);
    }

    // general initialization and mode setup
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glClearColor(0, 0, 0, 0);
    glShadeModel(GL_SMOOTH);

    // set up the blending function for correct alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);

    // GL_COMBINE_ARB texture env default values
    //
    // GL_COMBINE_RGB_ARB = GL_MODULATE
    // GL_COMBINE_ALPHA_ARB = GL_MODULATE
    //
    // GL_SOURCE0_RGB_ARB = GL_TEXTURE
    // GL_OPERAND0_RGB_ARB = GL_SRC_COLOR
    // GL_SOURCE0_ALPHA_ARB = GL_TEXTURE
    // GL_OPERAND0_ALPHA_ARB = GL_SRC_ALPHA
    //
    // GL_SOURCE1_RGB_ARB = GL_PREVIOUS_ARB
    // GL_OPERAND1_RGB_ARB = GL_SRC_COLOR
    // GL_SOURCE1_ALPHA_ARB = GL_PREVIOUS_ARB
    // GL_OPERAND1_ALPHA_ARB = GL_SRC_ALPHA
    //
    // GL_SOURCE2_RGB_ARB = GL_CONSTANT_ARB
    // GL_OPERAND2_RGB_ARB = GL_SRC_ALPHA
    // GL_SOURCE2_ALPHA_ARB = GL_CONSTANT_ARB
    // GL_OPERAND2_ALPHA_ARB = GL_SRC_ALPHA
    //
    // GL_TEXTURE_ENV_COLOR = (0.000000, 0.000000, 0.000000, 0.000000)

    // set up texture unit 0 -- texturing and color bias
    {
        glActiveTextureARB(GL_TEXTURE0_ARB);

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);

        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_INTERPOLATE_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_REPLACE);

        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_TEXTURE);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA);

        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_CONSTANT_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB, GL_PREVIOUS_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_ARB, GL_SRC_ALPHA);

        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, GL_CONSTANT_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, GL_ONE_MINUS_SRC_ALPHA);
    }

    // set up texture unit 1 -- color masking
    {
        glActiveTextureARB(GL_TEXTURE1_ARB);

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);

        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_MODULATE);
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_MODULATE);

        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_PREVIOUS_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA);

        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_CONSTANT_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB, GL_CONSTANT_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_ARB, GL_SRC_ALPHA);

        // might as well bind the all-white texture to texture unit 1
        // right now since that's all it will ever use.
        glBindTexture(GL_TEXTURE_2D, g_gltexture_white->GetHandle());
    }

    glActiveTextureARB(GL_TEXTURE0_ARB);
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
