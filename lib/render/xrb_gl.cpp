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

#include <iomanip>

#include "xrb_color.hpp"
#include "xrb_gltexture.hpp"
#include "xrb_gltextureatlas.hpp"
#include "xrb_math.hpp"
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
    // NOTE: we can't call Gl singleton state methods here because the singleton
    // is being constructed right now, so it is unavailable.
    {
        // init all the necessary state vars
        {
            m_active_texture = Integer(GL_ACTIVE_TEXTURE);
            m_client_active_texture = Integer(GL_CLIENT_ACTIVE_TEXTURE);
            // this (0, 0, 0, 0) is the default value as specified in the openGL docs
            m_texture_unit_1_env_color = Color::ms_transparent_black; // no tinting
        }

        glDisable(GL_DEPTH_TEST); // we sort the objects back-to-front and then render
        glDepthFunc(GL_LEQUAL);   // them, making the depth buffer unnecessary.
        glDepthMask(GL_FALSE);

        glDisable(GL_LIGHTING);
        glDisable(GL_FOG);
        glShadeModel(GL_FLAT);

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
}

Gl::~Gl ()
{
    fprintf(stderr, "Gl::~Gl(); OpenGL shutdown\n");

    // delete the utility textures (if necessary)
    DeleteAndNullify(m_gltexture_opaque_white);

    // shutdown both texture units
    Gl::ActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    m_atlas_bound_to_unit_0 = NULL;
    Gl::ActiveTexture(GL_TEXTURE1);
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

    // NOTE: it's ok to call Gl singleton state methods here, because the
    // Gl singleton has been constructed.  this method is just for 'finishing
    // initialization'.

    // create the 1x1 opaque white texture (which is used for color biasing)
    Texture *opaque_white = Texture::Create(ScreenCoordVector2(1, 1), Texture::CLEAR);
    opaque_white->Data()[0] = opaque_white->Data()[1] = opaque_white->Data()[2] = opaque_white->Data()[3] = 255;
    m_gltexture_opaque_white = CreateGlTexture(*opaque_white, GlTexture::NONE|GlTexture::USES_SEPARATE_ATLAS);
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

    // NOTE: the blending done below must be mimicked exactly by the
    // color-only version of SetupTextureUnits.

    // enable vertex arrays
    Gl::EnableClientState(GL_VERTEX_ARRAY);

    // set up texture unit 0 -- texturing and color masking -- we use
    // the glColor value instead of the GL_TEXTURE_ENV_COLOR value
    // for the color mask.
    {
        Gl::ActiveTexture(GL_TEXTURE0);

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
        Gl::ActiveTexture(GL_TEXTURE1);

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

        // enable the tex coord arrays for client texture unit 1
        Gl::ClientActiveTexture(GL_TEXTURE1);
        Gl::EnableClientState(GL_TEXTURE_COORD_ARRAY);
        // set up the texture array for the opaque white texture.
        // this should be set once and stay for the entire execution.
        glTexCoordPointer(2, GL_SHORT, 0, m_gltexture_opaque_white->TextureCoordinateArray());
        // leave the client active texture as GL_TEXTURE0.  it should
        // stay as this value for the entire execution.
        Gl::ClientActiveTexture(GL_TEXTURE0);
    }
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

#if !USE_GL_STATE_WRAPPERS && !INLINE_GL_CALLS
bool Gl::IsEnabled (GLenum cap) { return glIsEnabled(cap) == GL_TRUE; }
void Gl::Enable (GLenum cap) { glEnable(cap); }
void Gl::Disable (GLenum cap) { glDisable(cap); }
void Gl::EnableClientState (GLenum cap) { glEnableClientState(cap); }
void Gl::DisableClientState (GLenum cap) { glDisableClientState(cap); }
GLenum Gl::ActiveTexture () { return Integer(GL_ACTIVE_TEXTURE); }
void Gl::ActiveTexture (GLenum texture) { glActiveTexture(texture); }
GLenum Gl::ClientActiveTexture () { return Integer(GL_CLIENT_ACTIVE_TEXTURE); }
void Gl::ClientActiveTexture (GLenum texture) { glClientActiveTexture(texture); }
#endif

void Gl::UnregisterGlTexture (GlTexture &gltexture)
{
    // deallocate the space in the appropriate atlas
    gltexture.Atlas().UnplaceTexture(gltexture);
    // if the texture had USES_SEPARATE_ATLAS, then delete the atlas, because
    // we don't want some dumb little dinky atlas clogging shit up.
    // or
    // if the atlas is empty, delete it.
    if (gltexture.UsesSeparateAtlas() || gltexture.Atlas().GlTextureCount() == 0)
    {
        AtlasVector::iterator it = m_atlas.begin();
        AtlasVector::iterator it_end = m_atlas.end();
        while (it != it_end && *it != &gltexture.Atlas())
            ++it;
        ASSERT1(it != it_end && "atlas not found");
        GlTextureAtlas *atlas = *it;
        m_atlas.erase(it);
        delete atlas;

        Uint32 allocated_texture_byte_count = AllocatedTextureByteCount();
        Uint32 efficiency = (allocated_texture_byte_count > 0) ?
                            100 * UsedTextureByteCount() / allocated_texture_byte_count :
                            100;
        fprintf(stderr, "GlTextureAtlas count = %u, packing efficiency = %u%%\n", Uint32(m_atlas.size()), efficiency);
    }
}

void Gl::SetupTextureUnit0 (GlTexture const &gltexture, Color const &color_mask, Color const &color_bias)
{
    ASSERT1(Gl::ActiveTexture() == GL_TEXTURE0);
    BindAtlas(gltexture.Atlas());
    // due to limitations in the PowerVR MBX platform, (see
    // http://developer.apple.com/iphone/library/documentation/3DDrawing/Conceptual/OpenGLES_ProgrammingGuide/OpenGLESPlatforms/OpenGLESPlatforms.html ),
    // the value of GL_TEXTURE_ENV_COLOR must be the same for both texture
    // units.  but in texture unit 0, we don't actually use the value of
    // GL_TEXTURE_ENV_COLOR, we use the glColor value instead.
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, color_bias.m);
    glColor4f(color_mask[Dim::R], color_mask[Dim::G], color_mask[Dim::B], color_mask[Dim::A]);
}

void Gl::SetupTextureUnit1 (Color const &color_bias)
{
    ASSERT1(Gl::ActiveTexture() == GL_TEXTURE1);
    Gl::Enable(GL_TEXTURE_2D);
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, color_bias.m);
    m_texture_unit_1_env_color = color_bias;
}

void Gl::SetupTextureUnits (
    GlTexture const &gltexture,
    Color const &color_mask,
    Color const &color_bias)
{
    // this fanciness is to first setup whatever texture unit is currently active,
    // so that only one call to glActiveTexture is necessary per call to this method.
    bool active_texture_0_first = (Gl::ActiveTexture() == GL_TEXTURE0);

    if (active_texture_0_first)
    {
        // texture unit 0 is active, so set it up first.
        SetupTextureUnit0(gltexture, color_mask, color_bias);
        // only setup texture unit 1 if texturing is disabled or it would actually change the color
        if (!IsTexture2dEnabled(GL_TEXTURE1) || m_texture_unit_1_env_color != color_bias)
        {
            Gl::ActiveTexture(GL_TEXTURE1);
            SetupTextureUnit1(color_bias);
        }
    }
    else
    {
        // only setup texture unit 1 if texturing is disabled or it would actually change the color
        if (!IsTexture2dEnabled(GL_TEXTURE1) || m_texture_unit_1_env_color != color_bias)
            SetupTextureUnit1(color_bias);
        // texture unit 0 is active, so set it up first.
        Gl::ActiveTexture(GL_TEXTURE0);
        SetupTextureUnit0(gltexture, color_mask, color_bias);
    }
}

void Gl::SetupTextureUnits (
    Color const &color_mask,
    Color const &color_bias)
{
    // set things up for no texture mapping

    bool active_texture_1_first = (Gl::ActiveTexture() == GL_TEXTURE1);

    if (active_texture_1_first) // should be identical to below
    {
        // set up (disable) texture unit 1
        Gl::Disable(GL_TEXTURE_2D);
    }

    // set up (disable) texture unit 0
    if (active_texture_1_first)
        Gl::ActiveTexture(GL_TEXTURE0);
    Gl::Disable(GL_TEXTURE_2D);

    // NOTE: the blending done below must mimic exactly what is done by
    // the GL_TEXTURE_ENV_MODE GL_COMBINE stuff in FinishInitialization.

    // calculate masked and biased color manually -- linearly interpolate
    // between color_mask and color_bias, using color_bias[Dim::A] as the
    // interpolation parameter.
    Color color(color_mask * (1.0f - color_bias[Dim::A]) + color_bias * color_bias[Dim::A]);
    // reset the calculated alpha channel to color_mask's
    color[Dim::A] = color_mask[Dim::A];
    // set the opengl color
    glColor4f(color[Dim::R], color[Dim::G], color[Dim::B], color[Dim::A]);

    if (!active_texture_1_first) // should be identical to above
    {
        // set up (disable) texture unit 1
        Gl::ActiveTexture(GL_TEXTURE1);
        Gl::Disable(GL_TEXTURE_2D);
    }
}

void Gl::BindAtlas (GlTextureAtlas const &atlas)
{
    Gl::ActiveTexture(GL_TEXTURE0);
    Gl::Enable(GL_TEXTURE_2D);
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
    Gl::ActiveTexture(GL_TEXTURE0);
    Gl::Enable(GL_TEXTURE_2D);
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

Uint32 Gl::AllocatedTextureByteCount () const
{
    Uint32 retval = 0;
    for (Uint32 i = 0; i < m_atlas.size(); ++i)
    {
        ASSERT1(m_atlas[i] != NULL);
        retval += m_atlas[i]->AllocatedTextureByteCount();
    }
    return retval;
}

Uint32 Gl::UsedTextureByteCount () const
{
    Uint32 retval = 0;
    for (Uint32 i = 0; i < m_atlas.size(); ++i)
    {
        ASSERT1(m_atlas[i] != NULL);
        retval += m_atlas[i]->UsedTextureByteCount();
    }
    return retval;
}

void Gl::DumpAtlases (std::string const &path_prefix) const
{
#if XRB_PLATFORM == XRB_PLATFORM_IPHONE
    fprintf(stderr, "Gl::DumpAtlases only supported on non-iphone builds\n");
#else // XRB_PLATFORM != XRB_PLATFORM_IPHONE
    fprintf(stderr, "Gl::DumpAtlases(); path_prefix = \"%s\"\n", path_prefix.c_str());
    for (Uint32 i = 0; i < m_atlas.size(); ++i)
    {
        ASSERT1(m_atlas[i] != NULL);
        Uint32 mipmap_level = 0;
        Texture *dump = NULL;
        while ((dump = m_atlas[i]->Dump(mipmap_level)) != NULL)
        {
            std::string dump_path(
                path_prefix +
                FORMAT('.' <<
                       std::setw(2) << std::setfill('0') << i << '.' <<
                       std::setw(2) << std::setfill('0') << mipmap_level << ".png"));
            Pal::Status status = dump->Save(dump_path);
            if (status == Pal::SUCCESS)
                fprintf(stderr, "    successfully dumped ");
            else
                fprintf(stderr, "    FAILURE while dumping ");
            fprintf(stderr, "\"%s\" (atlas %u, mipmap level %u)\n", dump_path.c_str(), i, mipmap_level);
            delete dump;
            ++mipmap_level;
        }
    }
#endif // XRB_PLATFORM == XRB_PLATFORM_IPHONE
}

bool Gl::IsEnabled_ (GLenum cap)
{
    return EnableMapValue(cap);
}

void Gl::Enable_ (GLenum cap)
{
    ASSERT1(!IsClientTextureUnitCapability(cap));
    bool &value = EnableMapValue(cap);
    if (!value)
    {
        glEnable(cap);
        value = true;
    }
}

void Gl::Disable_ (GLenum cap)
{
    ASSERT1(!IsClientTextureUnitCapability(cap));
    bool &value = EnableMapValue(cap);
    if (value)
    {
        glDisable(cap);
        value = false;
    }
}

void Gl::EnableClientState_ (GLenum cap)
{
    ASSERT1(IsClientTextureUnitCapability(cap));
    bool &value = EnableMapValue(cap);
    if (!value)
    {
        glEnableClientState(cap);
        value = true;
    }
}

void Gl::DisableClientState_ (GLenum cap)
{
    ASSERT1(IsClientTextureUnitCapability(cap));
    bool &value = EnableMapValue(cap);
    if (value)
    {
        glDisableClientState(cap);
        value = false;
    }
}

void Gl::ActiveTexture_ (GLenum texture)
{
    if (texture != m_active_texture)
    {
        glActiveTexture(texture);
        m_active_texture = texture;
    }
}

void Gl::ClientActiveTexture_ (GLenum texture)
{
    if (texture != m_client_active_texture)
    {
        glClientActiveTexture(texture);
        m_client_active_texture = texture;
    }
}

bool &Gl::EnableMapValue (GLenum cap)
{
    // hacky way to multiplex the enabled-value of GL_TEXTURE_2D and other
    // things based on which texture unit or client texture unit is currently
    // active (see ActiveTexture and ClientActiveTexture).
    // NOTE: this depends on all possible cap values being within the range [0,65536).
    // looking through GL.h, it looks like this is the case.
    GLenum cap_key = cap;
    if (IsClientTextureUnitCapability(cap_key))
        cap_key |= (m_client_active_texture-GL_TEXTURE0) << 16; // stick the client texture unit in the high bits
    else if (cap_key == GL_TEXTURE_2D)
        cap_key |= (m_active_texture-GL_TEXTURE0) << 16; // stick the texture unit index in the high bits

    EnableMap::iterator it = m_enable_map.find(cap_key);
    if (it != m_enable_map.end()) // already in the enable map
        return it->second;
    else // not in the enable map.  initialize the value from the gl state
        return m_enable_map[cap_key] = (glIsEnabled(cap) == GL_TRUE);
}

bool Gl::IsTexture2dEnabled (GLenum texture)
{
    GLenum cap_key = GL_TEXTURE_2D;
    cap_key |= (texture-GL_TEXTURE0) << 16;
    EnableMap::iterator it = m_enable_map.find(cap_key);
    if (it != m_enable_map.end()) // in the enable map
        return it->second;
    else
        return false; // if we haven't changed it, it's the default value (i.e. false)
}

bool Gl::IsClientTextureUnitCapability (GLenum cap)
{
    return cap == GL_COLOR_ARRAY ||
           cap == GL_NORMAL_ARRAY ||
#if defined(GL_POINT_SIZE_ARRAY_OES)
           cap == GL_POINT_SIZE_ARRAY_OES ||
#endif // defined(GL_POINT_SIZE_ARRAY_OES)
           cap == GL_TEXTURE_COORD_ARRAY ||
           cap == GL_VERTEX_ARRAY;
}

GlTexture *Gl::CreateGlTexture (Texture const &texture, Uint32 gltexture_flags)
{
    // check that the flags are acceptable
    if (gltexture_flags & GlTexture::USES_SEPARATE_ATLAS)
    {
        // any texture is fine if it has its own atlas
    }
    else
    {
        if (!Math::IsAPowerOf2(texture.Width()) ||
            !Math::IsAPowerOf2(texture.Height()) ||
            !texture.Width() == texture.Height())
        {
            fprintf(stderr, "Gl::CreateGlTexture(); ERROR: non-square, non-power-of-2-sized textures must use GlTexture::USES_SEPARATE_ATLAS\n");
            return NULL;
        }
    }

    ScreenCoordVector2 gltexture_atlas_size(Singleton::Pal().GlTextureAtlasSize());
    if (gltexture_atlas_size[Dim::X] > 0 &&
        gltexture_atlas_size[Dim::Y] > 0 &&
        (!Math::IsAPowerOf2(Uint32(gltexture_atlas_size[Dim::X])) || !Math::IsAPowerOf2(Uint32(gltexture_atlas_size[Dim::Y]))))
    {
        fprintf(stderr, "Gl::CreateGlTexture(); ERROR: Pal::GlTextureAtlasSize() coordinates must be power-of-2 or nonpositive values\n");
        return NULL;
    }

    // if the texture wants to use a separate atlas, make a new atlas just for it
    if ((gltexture_flags & GlTexture::USES_SEPARATE_ATLAS) || gltexture_atlas_size[Dim::X] <= 0 || gltexture_atlas_size[Dim::Y] <= 0)
    {
        fprintf(stderr, "Gl::CreateGlTexture(); creating (separate) GlTextureAtlas of size (%d, %d)\n", texture.Size()[Dim::X], texture.Size()[Dim::Y]);
        GlTextureAtlas *atlas = new GlTextureAtlas(texture.Size(), gltexture_flags);
        AddAtlas(atlas);
        GlTexture *retval = atlas->AttemptToPlaceTexture(texture, gltexture_flags);
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
            // only check atlases that match the requested gltexture_flags
            if (gltexture_flags == atlas.GlTextureFlags())
            {
                GlTexture *retval = atlas.AttemptToPlaceTexture(texture, gltexture_flags);
                // the atlas may not have had room, so check it
                if (retval != NULL)
                {
                    fprintf(stderr, "Gl atlas packing efficiency: %u%%\n", 100 * UsedTextureByteCount() / AllocatedTextureByteCount());
                    return retval;
                }
            }
        }

        // no fit so far, so add a new atlas to the end
        fprintf(stderr, "Gl::CreateGlTexture(); creating (shared) GlTextureAtlas of size (%d, %d)\n", gltexture_atlas_size[Dim::X], gltexture_atlas_size[Dim::Y]);
        GlTextureAtlas *atlas = new GlTextureAtlas(gltexture_atlas_size, gltexture_flags);
        AddAtlas(atlas);
        return atlas->AttemptToPlaceTexture(texture, gltexture_flags);
    }
}

void Gl::AddAtlas (GlTextureAtlas *atlas)
{
    ASSERT1(atlas != NULL);
    m_atlas.push_back(atlas);

    Uint32 allocated_texture_byte_count = AllocatedTextureByteCount();
    Uint32 efficiency = (allocated_texture_byte_count > 0) ?
                        100 * UsedTextureByteCount() / allocated_texture_byte_count :
                        100;
    fprintf(stderr, "GlTextureAtlas count = %u, packing efficiency = %u%%\n", Uint32(m_atlas.size()), efficiency);
}

} // end of namespace Xrb
