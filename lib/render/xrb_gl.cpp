// ///////////////////////////////////////////////////////////////////////////
// xrb_gl.cpp by Victor Dods, created 2005/06/04
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_gl.hpp"

#include "xrb_texture.hpp"
#include "xrb_gltexture.hpp"

namespace Xrb
{

namespace
{

GLTexture *g_gltexture_opaque_white = NULL;

void CheckForExtension (char const *extension_name)
{
    fprintf(stderr, "        %s: ", extension_name);
    if (strstr(reinterpret_cast<char const *>(glGetString(GL_EXTENSIONS)), extension_name) == NULL)
    {
        fprintf(stderr, "NOT SUPPORTED -- aborting.\n");
        ASSERT0(false);
    }
    else
        fprintf(stderr, "supported.\n");
}

} // end of anonymous namespace

GLTexture const &GL::GLTexture_OpaqueWhite ()
{
    ASSERT1(g_gltexture_opaque_white != NULL && "You must call GL::Initialize before this function");
    return *g_gltexture_opaque_white;
}

void GL::Initialize ()
{
    // print some useful info
    fprintf(stderr, "GL::Initialize();\n");
    fprintf(stderr, "    GL_VENDOR = \"%s\"\n", glGetString(GL_VENDOR));
    fprintf(stderr, "    GL_RENDERER = \"%s\"\n", glGetString(GL_RENDERER));
    fprintf(stderr, "    GL_VERSION = \"%s\"\n", glGetString(GL_VERSION));

    // check for certain extensions and values
    {
        fprintf(stderr, "    Checking for OpenGL extensions.\n");
        // these ARB checks might be unnecessary now (maybe replace with some version check)
        CheckForExtension("GL_ARB_multitexture");
        CheckForExtension("GL_ARB_texture_env_combine");
        CheckForExtension("GL_ARB_vertex_buffer_object");

        GLint max_texture_units;
        glGetIntegerv(GL_MAX_TEXTURE_UNITS, &max_texture_units);
        fprintf(stderr, "    Checking GL_MAX_TEXTURE_UNITS (must be at least 2): %d\n", max_texture_units);
        ASSERT0(max_texture_units >= 2 && "GL_MAX_TEXTURE_UNITS must be at least 2");
    }

    // stuff related to texture byte order and alignment.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_LSB_FIRST, 0);
    glPixelStorei(GL_UNPACK_LSB_FIRST, 0);

    // initialize the singleton helper texture(s)
    {
        Texture *opaque_white = Texture::Create(ScreenCoordVector2(1, 1), true);
        opaque_white->Data()[0] = 255;
        opaque_white->Data()[1] = 255;
        opaque_white->Data()[2] = 255;
        opaque_white->Data()[3] = 255;
        g_gltexture_opaque_white = GLTexture::Create(opaque_white);
    }

    // general initialization and mode setup
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);

    // set up the blending function for correct alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);

    // GL_COMBINE texture env default values
    //
    // GL_COMBINE_RGB = GL_MODULATE
    // GL_COMBINE_ALPHA = GL_MODULATE
    //
    // GL_SRC0_RGB = GL_TEXTURE
    // GL_OPERAND0_RGB = GL_SRC_COLOR
    // GL_SRC0_ALPHA = GL_TEXTURE
    // GL_OPERAND0_ALPHA = GL_SRC_ALPHA
    //
    // GL_SRC1_RGB = GL_PREVIOUS
    // GL_OPERAND1_RGB = GL_SRC_COLOR
    // GL_SRC1_ALPHA = GL_PREVIOUS
    // GL_OPERAND1_ALPHA = GL_SRC_ALPHA
    //
    // GL_SRC2_RGB = GL_CONSTANT
    // GL_OPERAND2_RGB = GL_SRC_ALPHA
    // GL_SRC2_ALPHA = GL_CONSTANT
    // GL_OPERAND2_ALPHA = GL_SRC_ALPHA
    //
    // GL_TEXTURE_ENV_COLOR = (0.000000, 0.000000, 0.000000, 0.000000)

    // set up texture unit 0 -- texturing and color masking
    {
        glActiveTexture(GL_TEXTURE0);

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);

        glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_TEXTURE);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);

        glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_CONSTANT);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_CONSTANT);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);

        // SRC2_RGB and OPERAND2_RGB are not used for GL_MODULATE
        // SRC2_ALPHA and OPERAND2_ALPHA are not used for GL_MODULATE
    }

    // set up texture unit 1 -- color biasing
    {
        glActiveTexture(GL_TEXTURE1);

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);

        glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PREVIOUS);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_PREVIOUS);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);

        glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_CONSTANT);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
        // SRC1_ALPHA and OPERAND1_ALPHA are not used for GL_REPLACE

        glTexEnvi(GL_TEXTURE_ENV, GL_SRC2_RGB, GL_CONSTANT);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_ONE_MINUS_SRC_ALPHA);
        // SRC2_ALPHA and OPERAND2_ALPHA are not used for GL_REPLACE

        // might as well bind the all-white texture to texture unit 1
        // right now since that's all it will ever use.  this will
        // have no effect on the above texture unit operation, but is
        // required for the texture unit to activate.
        glBindTexture(GL_TEXTURE_2D, GLTexture_OpaqueWhite().Handle());
    }

    glActiveTexture(GL_TEXTURE0);
}

GLint GL::MatrixMode ()
{
    GLint matrix_mode;
    glGetIntegerv(GL_MATRIX_MODE, &matrix_mode);
    return matrix_mode;
}

GLint GL::MatrixStackDepth (GLenum const matrix_mode)
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

GLint GL::MaxMatrixStackDepth (GLenum const matrix_mode)
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

bool GL::IsTexture2dOn ()
{
    GLboolean is_texture_2d_on;
    glGetBooleanv(GL_TEXTURE_2D, &is_texture_2d_on);
    return is_texture_2d_on == GL_TRUE;
}

void GL::SetClipRect (ScreenCoordRect const &clip_rect)
{
    ASSERT1(clip_rect.IsValid());

    // set up the GL projection matrix here.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(
        clip_rect.Left(), clip_rect.Right(),
        clip_rect.Bottom(), clip_rect.Top(),
        -1.0, 1.0); // these values (-1, 1) are arbitrary

    // set up the viewport which is the rectangle on screen which
    // will be rendered to.  this also properly sets up the clipping
    // planes.
    glViewport(
        clip_rect.Left(),
        clip_rect.Bottom(),
        clip_rect.Width(),
        clip_rect.Height());
}

} // end of namespace Xrb
