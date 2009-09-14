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

#include "xrb_color.hpp"
#include "xrb_gltexture.hpp"
#include "xrb_gltextureatlas.hpp"
#include "xrb_texture.hpp"

namespace Xrb
{

namespace
{

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

Gl::Gl ()
    :
    m_gltexture_opaque_white(NULL),
    m_atlas_bound_to_unit_0(NULL)
{
    ResetBindTextureCallCounts();

    // print some useful info
    fprintf(stderr, "OpenGL initialization\n");
    fprintf(stderr, "    GL_VENDOR = \"%s\"\n", glGetString(GL_VENDOR));
    fprintf(stderr, "    GL_RENDERER = \"%s\"\n", glGetString(GL_RENDERER));
    fprintf(stderr, "    GL_VERSION = \"%s\"\n", glGetString(GL_VERSION));
    fprintf(stderr, "    GL_MAX_TEXTURE_SIZE = %d\n", Integer(GL_MAX_TEXTURE_SIZE));

    // check for certain extensions and values
    {
        GLint max_texture_units;
        glGetIntegerv(GL_MAX_TEXTURE_UNITS, &max_texture_units);
        fprintf(stderr, "    Checking GL_MAX_TEXTURE_UNITS (must be at least 2): %d\n", max_texture_units);
        ASSERT0(max_texture_units >= 2 && "GL_MAX_TEXTURE_UNITS must be at least 2");
    }

    // stuff related to texture byte order and alignment.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // general initialization and mode setup
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glDepthFunc(GL_LEQUAL);

    // set up the blending function for correct alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // only bother drawing pixels with a nonzero alpha value.
    // this was moved here from Engine2::VisibilityQuadTree::Draw*
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    // don't bother writing to the alpha channel
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
}

Gl::~Gl ()
{
    fprintf(stderr, "OpenGL shutdown\n");

    // delete the opaque white texture (if it exists)
    DeleteAndNullify(m_gltexture_opaque_white);

    // shutdown both texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    m_atlas_bound_to_unit_0 = NULL;
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    // delete all texture atlases
    for (AtlasVector::iterator it = m_atlas.begin(), it_end = m_atlas.end();
         it != it_end;
         ++it)
    {
        GlTextureAtlas *atlas = *it;
        ASSERT1(atlas != NULL);
        delete atlas;
    }
    m_atlas.clear();
}

void Gl::FinishInitialization ()
{
    ASSERT1(m_gltexture_opaque_white == NULL);

    // create the 1x1 opaque white texture (which is used for color biasing)
    Texture *opaque_white = Texture::Create(ScreenCoordVector2(1, 1), true);
    opaque_white->Data()[0] = 255;
    opaque_white->Data()[1] = 255;
    opaque_white->Data()[2] = 255;
    opaque_white->Data()[3] = 255;
    GlTextureLoadParameters load_parameters(GlTextureLoadParameters::USES_SEPARATE_ATLAS);
    m_gltexture_opaque_white = CreateGlTexture(*opaque_white, load_parameters);
    ASSERT1(m_gltexture_opaque_white != NULL);

    // GL_COMBINE texture env default values
    //
    // GL_COMBINE_RGB = GL_MODULATE
    // GL_SRC0_RGB = GL_TEXTURE
    // GL_OPERAND0_RGB = GL_SRC_COLOR
    // GL_SRC1_RGB = GL_PREVIOUS
    // GL_OPERAND1_RGB = GL_SRC_COLOR
    // GL_SRC2_RGB = GL_CONSTANT
    // GL_OPERAND2_RGB = GL_SRC_ALPHA
    //
    // GL_COMBINE_ALPHA = GL_MODULATE
    // GL_SRC0_ALPHA = GL_TEXTURE
    // GL_OPERAND0_ALPHA = GL_SRC_ALPHA
    // GL_SRC1_ALPHA = GL_PREVIOUS
    // GL_OPERAND1_ALPHA = GL_SRC_ALPHA
    // GL_SRC2_ALPHA = GL_CONSTANT
    // GL_OPERAND2_ALPHA = GL_SRC_ALPHA
    //
    // GL_TEXTURE_ENV_COLOR = (0, 0, 0, 0)

    // set up texture unit 0 -- texturing and color masking -- we use
    // the glColor value instead of the GL_TEXTURE_ENV_COLOR value
    // for the color mask.
    {
        glActiveTexture(GL_TEXTURE0);

        // enable GL_COMBINE (this allows multitexturing operations)
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

        // NOTE: these all happen to be the defaults (see above)

        // the RGB operation is GL_MODULATE (dest = op0*op1)
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
        glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PREVIOUS);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
        // SRC2_RGB and OPERAND2_RGB are not used for GL_MODULATE

        // the ALPHA operation is GL_MODULATE (dest = op0*op1)
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);
        glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_TEXTURE);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
        glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_PREVIOUS);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
        // SRC2_ALPHA and OPERAND2_ALPHA are not used for GL_MODULATE
    }

    // set up texture unit 1 -- color biasing
    {
        glActiveTexture(GL_TEXTURE1);

        // enable GL_COMBINE (this allows multitexturing operations)
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

        // the RGB operation is GL_INTERPOLATE (dest = op0*op2 + op1*(1-op2))
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
        glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_CONSTANT);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PREVIOUS);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_SRC2_RGB, GL_CONSTANT);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_ALPHA);

        // the ALPHA operation is GL_REPLACE (dest = op0)
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
        glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_PREVIOUS);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
        // SRC1_ALPHA and OPERAND1_ALPHA are not used for GL_REPLACE
        // SRC2_ALPHA and OPERAND2_ALPHA are not used for GL_REPLACE

        // bind the all-white texture to texture unit 1 right now
        // since that's all it will ever use.  this is required
        // for the texture unit to activate and hold its settings.
        glBindTexture(GL_TEXTURE_2D, m_gltexture_opaque_white->Atlas().Handle());
    }

    glActiveTexture(GL_TEXTURE0);
}

bool Gl::Boolean (GLenum name)
{
    return Integer(name) != 0;
}

GLint Gl::Integer (GLenum name)
{
    // this may not be a comprehensive check against all larger-than-4-return-value
    // integers -- it was based on the openGL 2.1 man page for glGet.  these
    // names are prohibited because they return more than 4 values (and we don't
    // want to bother with a buffer that large in this function that only returns 1).
    ASSERT1(name != GL_COMPRESSED_TEXTURE_FORMATS   && "unsupported name");
#if !defined(__IPHONEOS__)
    ASSERT1(name != GL_TRANSPOSE_COLOR_MATRIX       && "unsupported name");
    ASSERT1(name != GL_TRANSPOSE_MODELVIEW_MATRIX   && "unsupported name");
    ASSERT1(name != GL_TRANSPOSE_PROJECTION_MATRIX  && "unsupported name");
    ASSERT1(name != GL_TRANSPOSE_TEXTURE_MATRIX     && "unsupported name");
    ASSERT1(name != GL_COLOR_MATRIX                 && "unsupported name");
#endif
    ASSERT1(name != GL_MODELVIEW_MATRIX             && "unsupported name");
    ASSERT1(name != GL_PROJECTION_MATRIX            && "unsupported name");
    ASSERT1(name != GL_TEXTURE_MATRIX               && "unsupported name");

    // the requested integer may fill up to 4 entries
    GLint integer[4];
    glGetIntegerv(name, integer);
    return integer[0];
}

void Gl::SetClipRect (ScreenCoordRect const &clip_rect)
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

GlTexture *Gl::CreateGlTexture (Texture const &texture, GlTextureLoadParameters const &load_parameters)
{
    // if the texture wants to use a separate atlas, make a new atlas just for it
    if (load_parameters.UsesSeparateAtlas())
    {
        GlTextureAtlas *atlas = new GlTextureAtlas(texture.Size());
        m_atlas.push_back(atlas);
        GlTexture *retval = atlas->PlaceTexture(texture, load_parameters);
        ASSERT1(retval != NULL);
        return retval;
    }
    // otherwise place it into an existing atlas, creating a new one if there's no room
    else
    {
        for (Uint32 i = 0; i < m_atlas.size(); ++i)
        {
            ASSERT1(m_atlas[i] != NULL);
            GlTextureAtlas &atlas = *m_atlas[i];
            GlTexture *retval = atlas.PlaceTexture(texture, load_parameters);
            if (retval != NULL)
                return retval;
        }

        // no fit so far, so add a new atlas to the end
        GlTextureAtlas *atlas = new GlTextureAtlas(ScreenCoordVector2(1024, 1024)); // HIPPO: do real size
        m_atlas.push_back(atlas);
        return atlas->PlaceTexture(texture, load_parameters);
    }
}

void Gl::UnregisterGlTexture (GlTexture &gltexture)
{
    // deallocate the space in the appropriate atlas
    gltexture.Atlas().UnplaceTexture(gltexture);
    // if the texture had USES_SEPARATE_ATLAS, then delete the atlas, because
    // we don't want some dumb little dinky atlas clogging shit up.
    if (gltexture.LoadParameters().UsesSeparateAtlas())
    {
        AtlasVector::iterator it = m_atlas.begin();
        AtlasVector::iterator it_end = m_atlas.end();
        while (it != it_end && *it != &gltexture.Atlas())
            ++it;
        ASSERT1(it != it_end && "atlas not found");
        delete *it;
        m_atlas.erase(it);
    }
}

void Gl::SetupTextureUnits (
    GlTexture const &gltexture,
    Color const &color_mask,
    Color const &color_bias)
{
    ASSERT1(m_gltexture_opaque_white != NULL);

    // set up texture unit 0
    BindAtlas(gltexture.Atlas());
    // due to limitations in the PowerVR MBX platform, (see
    // http://developer.apple.com/iphone/library/documentation/3DDrawing/Conceptual/
    //          OpenGLES_ProgrammingGuide/OpenGLESPlatforms/OpenGLESPlatforms.html ),
    // the value of GL_TEXTURE_ENV_COLOR must be the same for both texture
    // units.  but in texture unit 0, we don't actually use the value of
    // GL_TEXTURE_ENV_COLOR, we use the glColor value instead.
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, color_bias.m);
    glColor4f(color_mask[Dim::R], color_mask[Dim::G], color_mask[Dim::B], color_mask[Dim::A]);

    // set up texture unit 1
    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    // TODO: assert that the opaque white texture is still bound to unit 1
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, color_bias.m);
}

void Gl::BindAtlas (GlTextureAtlas const &atlas)
{
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    if (&atlas != m_atlas_bound_to_unit_0)
    {
        // bind the (atlas) texture
        glBindTexture(GL_TEXTURE_2D, atlas.Handle());
        // set up the texture matrix so we can use integer coordinates to
        // address the individual subtextures in the atlas.
        glMatrixMode(GL_TEXTURE);
        glLoadIdentity();
        glScalef(
            1.0f / atlas.Size()[Dim::X],
            1.0f / atlas.Size()[Dim::Y],
            1.0f);

        m_atlas_bound_to_unit_0 = &atlas;
        ++m_bind_texture_call_miss_count;
    }
    else
    {
        ++m_bind_texture_call_hit_count;
    }
}

void Gl::EnsureAtlasIsNotBound (GlTextureAtlas const &atlas)
{
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    if (m_atlas_bound_to_unit_0 == &atlas)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        m_atlas_bound_to_unit_0 = NULL;
    }
}

void Gl::ResetBindTextureCallCounts ()
{
    m_bind_texture_call_hit_count = 0;
    m_bind_texture_call_miss_count = 0;
}

} // end of namespace Xrb
