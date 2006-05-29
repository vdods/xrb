// ///////////////////////////////////////////////////////////////////////////
// xrb_font.cpp by Victor Dods, created 2005/06/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_font.h"

#include "ft2build.h"
#include FT_FREETYPE_H
#include "xrb_gl.h"
#include "xrb_math.h"
#include "xrb_rendercontext.h"
#include "xrb_texture.h"

namespace Xrb
{

// ///////////////////////////////////////////////////////////////////////////
// Font::GlyphSpecification
// ///////////////////////////////////////////////////////////////////////////

int Font::GlyphSpecification::SortByWidthFirst (
    void const *left_operand,
    void const *right_operand)
{
    GlyphSpecification const *left_glyph =
        *static_cast<GlyphSpecification const *const *>(left_operand);
    GlyphSpecification const *right_glyph =
        *static_cast<GlyphSpecification const *const *>(right_operand);

    // sort by height first, then width.
    if (left_glyph->m_size[Dim::X] < right_glyph->m_size[Dim::X])
        return -1;
    else if (left_glyph->m_size[Dim::X] == right_glyph->m_size[Dim::X])
        return left_glyph->m_size[Dim::Y] - right_glyph->m_size[Dim::Y];
    else
        return 1;
}

int Font::GlyphSpecification::SortByHeightFirst (
    void const *left_operand,
    void const *right_operand)
{
    GlyphSpecification const *left_glyph =
        *static_cast<GlyphSpecification const *const *>(left_operand);
    GlyphSpecification const *right_glyph =
        *static_cast<GlyphSpecification const *const *>(right_operand);

    // sort by height first, then width.
    if (left_glyph->m_size[Dim::Y] < right_glyph->m_size[Dim::Y])
        return -1;
    else if (left_glyph->m_size[Dim::Y] == right_glyph->m_size[Dim::Y])
        return left_glyph->m_size[Dim::X] - right_glyph->m_size[Dim::X];
    else
        return 1;
}

// ///////////////////////////////////////////////////////////////////////////
// Font
// ///////////////////////////////////////////////////////////////////////////

Font *Font::Create (
    std::string const &font_face_filename,
    ScreenCoord const pixel_height)
{
    Resource<FontFace> font_face =
        Singletons::ResourceLibrary()->LoadFilename<FontFace>(
            FontFace::Create,
            font_face_filename);
    return Create(font_face, pixel_height);
}

Font *Font::Create (
    Resource<FontFace> const &font_face,
    ScreenCoord const pixel_height)
{
    Font *retval = NULL;

    if (!font_face.GetIsValid())
        return retval;

    FT_FaceRec_ *ft_face = font_face->GetFTFace();
    FT_Error error;

    ASSERT1(ft_face != NULL)
    error = FT_Set_Pixel_Sizes(ft_face, 0, pixel_height);
    if (error != 0)
        return retval;

    retval = new Font;
    retval->m_has_kerning = FT_HAS_KERNING(ft_face) != 0;
    retval->m_pixel_height = pixel_height;
    retval->m_font_face = font_face;

    retval->PopulateGlyphSpecification(font_face);

    // make a list of pointers to each glyph specification
    GlyphSpecification *sorted_glyph_specification[RENDERED_GLYPH_COUNT];
    for (Uint32 i = 0; i < RENDERED_GLYPH_COUNT; ++i)
        sorted_glyph_specification[i] = &retval->m_glyph_specification[i];

    // sort them by width then height
    qsort(
        sorted_glyph_specification,
        RENDERED_GLYPH_COUNT,
        sizeof(GlyphSpecification const *),
        GlyphSpecification::SortByWidthFirst);

    // find the smallest texture size that will fit all the glyphs
    ScreenCoordVector2 smallest_fitting_texture_size =
        retval->FindSmallestFittingTextureSize(sorted_glyph_specification);
    ASSERT1(smallest_fitting_texture_size != ScreenCoordVector2::ms_zero)

    // generate the texture using the calculated size
    retval->GenerateTexture(smallest_fitting_texture_size);
    ASSERT1(retval->m_gl_texture != NULL)

    return retval;
}

ScreenCoord Font::GetKerningPixelAdvance (Uint32 left, Uint32 right) const
{
    if (!m_has_kerning)
        return 0;

    ASSERT1(m_font_face.GetIsValid());
    FT_FaceRec_ *ft_face = m_font_face->GetFTFace();

    FT_Error error = FT_Set_Pixel_Sizes(ft_face, 0, m_pixel_height);
    if (error != 0)
        return 0;

    FT_Vector delta;
    FT_Get_Kerning(
        ft_face,
        FT_Get_Char_Index(ft_face, left),
        FT_Get_Char_Index(ft_face, right),
        FT_KERNING_DEFAULT,
        &delta);

    // delta is in 26.6 fixed point, so return only the integer portion.
    return delta.x >> 6;
}

ScreenCoordRect Font::GetStringRect (char const *string) const
{
    ASSERT1(string != NULL)
    ScreenCoord max_line_width = 0;
    ScreenCoord current_line_width = 0;
    ScreenCoord line_count = 1;
    while (*string != '\0')
    {
        if (*string == '\n')
        {
            if (max_line_width < current_line_width)
                max_line_width = current_line_width;
            current_line_width = 0;
            ++line_count;
        }
        else if (*string != '\t')
            current_line_width += GetGlyphPixelAdvance(*string);

        ++string;
    }
    if (max_line_width < current_line_width)
        max_line_width = current_line_width;

    return ScreenCoordRect(
        ScreenCoordVector2(
            max_line_width,
            line_count * GetPixelHeight()));
}

ScreenCoord Font::GetStringWidth (char const *string) const
{
    ASSERT1(string != NULL)
    ScreenCoord max_line_width = 0;
    ScreenCoord current_line_width = 0;
    while (*string != '\0')
    {
        if (*string == '\n')
        {
            if (max_line_width < current_line_width)
                max_line_width = current_line_width;
            current_line_width = 0;
        }
        else if (*string != '\t')
            current_line_width += GetGlyphPixelAdvance(*string);

        ++string;
    }
    if (max_line_width < current_line_width)
        max_line_width = current_line_width;

    return max_line_width;
}

ScreenCoord Font::GetStringHeight (char const *string) const
{
    ASSERT1(string != NULL)
    ScreenCoord line_count = 1;
    while (*string != '\0')
    {
        if (*string == '\n')
            ++line_count;

        ++string;
    }
    return line_count * GetPixelHeight();
}

void Font::PopulateGlyphSpecification (Resource<FontFace> const &font_face)
{
    FT_Error error;
    FT_FaceRec_ *ft_face = font_face->GetFTFace();
    ScreenCoord tallest_glyph_height = -1;
    ScreenCoord tallest_glyph_bearing_y = -1;

    for (Uint32 unicode = RENDERED_GLYPH_LOWEST;
         unicode <= RENDERED_GLYPH_HIGHEST;
         ++unicode)
    {
        error = FT_Load_Glyph(ft_face, FT_Get_Char_Index(ft_face, unicode), FT_LOAD_DEFAULT);
        ASSERT1(error == 0)

        GlyphSpecification &glyph =
            m_glyph_specification[GetGlyphIndex(unicode)];

        glyph.m_unicode = unicode;
        glyph.m_size.SetComponents(
            ft_face->glyph->metrics.width >> 6,
            ft_face->glyph->metrics.height >> 6);
        glyph.m_bearing.SetComponents(
            ft_face->glyph->metrics.horiBearingX >> 6,
            ft_face->glyph->metrics.horiBearingY >> 6);
        glyph.m_advance = ft_face->glyph->metrics.horiAdvance >> 6;

        if (tallest_glyph_height < glyph.m_size[Dim::Y])
        {
            tallest_glyph_height = glyph.m_size[Dim::Y];
            tallest_glyph_bearing_y = glyph.m_bearing[Dim::Y];
        }
    }

    m_baseline_height =
        (m_pixel_height - tallest_glyph_height) / 2 +
        (tallest_glyph_height - tallest_glyph_bearing_y);
}

ScreenCoordVector2 Font::FindSmallestFittingTextureSize (
    GlyphSpecification *const *const sorted_glyph_specification)
{
    GLint max_texture_size;
    // TODO: replace with an accessor to the current video options
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);

    Dim::Component increase = Dim::Y;
    ScreenCoordVector2 texture_size(32, 32);
    while(texture_size[Dim::X] <= static_cast<ScreenCoord>(max_texture_size) &&
          texture_size[Dim::Y] <= static_cast<ScreenCoord>(max_texture_size))
    {
        Uint32 used_area = GetUsedTextureArea(texture_size, sorted_glyph_specification);
        DEBUG1_CODE(Uint32 total_area = texture_size[Dim::X] * texture_size[Dim::Y]);
        ASSERT1(used_area <= total_area)
        if (used_area > 0)
            return texture_size;

        texture_size[increase] <<= 1;
        increase = increase == Dim::X ? Dim::Y : Dim::X;
    }
    // TODO: proper handling -- scale the face down and try again
    ASSERT1(false && "No texture big enough to hold all glyphs")
    return ScreenCoordVector2::ms_zero;
}

Uint32 Font::GetUsedTextureArea (
    ScreenCoordVector2 const &texture_size,
    GlyphSpecification *const *const sorted_glyph_specification)
{
    ASSERT1(texture_size[Dim::X] > 0)
    ASSERT1(texture_size[Dim::Y] > 0)
    // the left hand size of this cryptic expression evaluates to 0
    // for zero or any non-negative integer power of 2 (e.g. 0, 1, 2,
    // 4, 8, 16, 32, 64, etc).
    ASSERT1((texture_size[Dim::X] & (texture_size[Dim::X] - 1)) == 0)
    ASSERT1((texture_size[Dim::Y] & (texture_size[Dim::Y] - 1)) == 0)
    ASSERT1(sorted_glyph_specification != NULL)

    ScreenCoord packed_width = 0;
    ScreenCoordVector2 current_packing_area = ScreenCoordVector2::ms_zero;
    Uint32 index = 0;
    Uint32 total_area = 0;

    while (packed_width + current_packing_area[Dim::X] <= texture_size[Dim::X] &&
           index < RENDERED_GLYPH_COUNT)
    {
        GlyphSpecification *glyph = sorted_glyph_specification[index];
        ASSERT1(glyph != NULL)

        // return failure if this single glyph doesn't even fit
        // inside the entire texture.
        if (glyph->m_size[Dim::X] > texture_size[Dim::X] ||
            glyph->m_size[Dim::Y] > texture_size[Dim::Y])
            return 0;

        // if the glyph would stick off the bottom of this column,
        // go to the top of the next column.
        if (current_packing_area[Dim::Y] + glyph->m_size[Dim::Y] > texture_size[Dim::Y])
        {
            packed_width += current_packing_area[Dim::X];
            current_packing_area[Dim::Y] = 0;
        }
        // otherwise stick the glyph at the bottom of the current column.
        else
        {
            total_area += glyph->m_size[Dim::X] * glyph->m_size[Dim::Y];
            glyph->m_texture_coordinates.SetComponents(
                packed_width,
                current_packing_area[Dim::Y]);
            if (current_packing_area[Dim::X] < glyph->m_size[Dim::X])
                current_packing_area[Dim::X] = glyph->m_size[Dim::X];
            current_packing_area[Dim::Y] += glyph->m_size[Dim::Y];
            ++index;
        }
        ASSERT1(current_packing_area[Dim::Y] <= texture_size[Dim::Y])
    }

    // return with success iff all the glyphs were stored within
    // the bounds of the texture.
    if (packed_width <= texture_size[Dim::X] && index == RENDERED_GLYPH_COUNT)
    {
        ASSERT1(total_area > 0)
        return total_area;
    }
    else
        return 0;
}

void Font::GenerateTexture (ScreenCoordVector2 const &texture_size)
{
    Texture *texture = Texture::Create(texture_size, false);

    FT_Error error;
    FT_FaceRec_ *ft_face = m_font_face->GetFTFace();
    for (Uint32 i = 0; i < RENDERED_GLYPH_COUNT; ++i)
    {
        GlyphSpecification &glyph = m_glyph_specification[i];

        error = FT_Load_Glyph(ft_face, FT_Get_Char_Index(ft_face, glyph.m_unicode), FT_LOAD_RENDER);
        ASSERT1(error == 0)
        ASSERT1(ft_face->glyph->format == FT_GLYPH_FORMAT_BITMAP)

        // copy the bitmap data over
        // TODO: real method which does proper pitch detection
        ASSERT1(ft_face->glyph->bitmap.pitch == glyph.m_size[Dim::X])
        Uint8 const *source_pixel_data = ft_face->glyph->bitmap.buffer;
        for (Sint32 y = 0; y < glyph.m_size[Dim::Y]; ++y)
        {
            Uint8 *dest_pixel_data =
                texture->GetData() +
                (glyph.m_texture_coordinates[Dim::Y] + y) * texture_size[Dim::X] * 4 +
                glyph.m_texture_coordinates[Dim::X] * 4;

            for (Sint32 x = 0; x < glyph.m_size[Dim::X]; ++x)
            {
                // assumed to be 32 bit RGBA for now
                *(dest_pixel_data++) = 0xFF;
                *(dest_pixel_data++) = 0xFF;
                *(dest_pixel_data++) = 0xFF;
                *(dest_pixel_data++) = *(source_pixel_data++);
            }
        }
    }

//     texture->WritePNG("font_rendering.png");

    m_gl_texture = GLTexture::Create(texture);
    ASSERT1(m_gl_texture != NULL)
    DeleteAndNullify(texture);
}










//////////////////////////////////
//////////////////////////////////
//////////////////////////////////
//////////////////////////////////







// ///////////////////////////////////////////////////////////////////////////
// FontBase
// ///////////////////////////////////////////////////////////////////////////

ScreenCoordRect FontBase::GetStringRect (
    char const *const string,
    ScreenCoordVector2 const &initial_pen_position) const
{
    ASSERT1(string != NULL)

    ScreenCoordVector2 pen_position_26_6(
        initial_pen_position[Dim::X] << 6,
        initial_pen_position[Dim::Y] << 6);
    ScreenCoordVector2 pen_position_span_26_6(pen_position_26_6);

    char const *current_glyph = string;
    char const *next_glyph = GetNextGlyph(current_glyph);
    // the end of the string is indicated when current_glyph == next_glyph
    while (current_glyph != next_glyph)
    {
        // move through all the normal glyphs
        MoveThroughGlyph(&pen_position_26_6, initial_pen_position, current_glyph, next_glyph);
        current_glyph = next_glyph;
        next_glyph = GetNextGlyph(next_glyph);

        // track the bounding box width
        if (GetInitialPenOrientation()[Dim::X] == LEFT)
            pen_position_span_26_6[Dim::X] = Max(pen_position_span_26_6[Dim::X], pen_position_26_6[Dim::X]);
        else
            pen_position_span_26_6[Dim::X] = Min(pen_position_span_26_6[Dim::X], pen_position_26_6[Dim::X]);

        // track the bounding box height
        if (GetInitialPenOrientation()[Dim::Y] == BOTTOM)
            pen_position_span_26_6[Dim::Y] = Max(pen_position_span_26_6[Dim::Y], pen_position_26_6[Dim::Y]);
        else
            pen_position_span_26_6[Dim::Y] = Min(pen_position_span_26_6[Dim::Y], pen_position_26_6[Dim::Y]);
    }

    // tack on a newline at the end so the last line is counted
    MoveThroughGlyph(&pen_position_26_6, initial_pen_position, "\n", NULL);

    // round pen_position_span away from inital_pen_position
    {
        pen_position_span_26_6[Dim::X] -= initial_pen_position[Dim::X]<<6;
        pen_position_span_26_6[Dim::X] = Math::FixedPointRound<6>(pen_position_span_26_6[Dim::X]);
        pen_position_span_26_6[Dim::X] += initial_pen_position[Dim::X]<<6;

        pen_position_span_26_6[Dim::Y] -= initial_pen_position[Dim::Y]<<6;
        pen_position_span_26_6[Dim::Y] = Math::FixedPointRound<6>(pen_position_span_26_6[Dim::Y]);
        pen_position_span_26_6[Dim::Y] += initial_pen_position[Dim::Y]<<6;
    }

    return ScreenCoordRect(
        Min(pen_position_span_26_6[Dim::X]>>6, initial_pen_position[Dim::X]),
        Min(pen_position_span_26_6[Dim::Y]>>6, initial_pen_position[Dim::Y]),
        Max(pen_position_span_26_6[Dim::X]>>6, initial_pen_position[Dim::X]),
        Max(pen_position_span_26_6[Dim::Y]>>6, initial_pen_position[Dim::Y]));
}

void FontBase::DrawString (
    RenderContext const &render_context,
    char const *const string,
    ScreenCoordVector2 const &initial_pen_position) const
{
    ASSERT1(string != NULL)

    // this is for any pre-rendering setup DrawGlyph needs to do
    DrawGlyphSetup(render_context);

    ScreenCoordVector2 pen_position_26_6(
        initial_pen_position[Dim::X] << 6,
        initial_pen_position[Dim::Y] << 6);

    // TODO: add left/right/spaced justification

    char const *current_glyph = string;
    char const *next_glyph = GetNextGlyph(current_glyph);
    // the end of the string is indicated when current_glyph == next_glyph
    while (current_glyph != next_glyph)
    {
        // draw the glyph
        DrawGlyph(render_context, current_glyph, pen_position_26_6);
        // advance the pen
        MoveThroughGlyph(&pen_position_26_6, initial_pen_position, current_glyph, next_glyph);
        current_glyph = next_glyph;
        next_glyph = GetNextGlyph(next_glyph);
    }

    // this is for any post-rendering setup DrawGlyph needs to do
    DrawGlyphShutdown(render_context);
}

// ///////////////////////////////////////////////////////////////////////////
// AsciiFont::GlyphSpecification
// ///////////////////////////////////////////////////////////////////////////

int AsciiFont::GlyphSpecification::SortByWidthFirst (
    void const *const left_operand,
    void const *const right_operand)
{
    GlyphSpecification const *left_glyph =
        *static_cast<GlyphSpecification const *const *>(left_operand);
    GlyphSpecification const *right_glyph =
        *static_cast<GlyphSpecification const *const *>(right_operand);

    // sort by height first, then width.
    if (left_glyph->m_size[Dim::X] < right_glyph->m_size[Dim::X])
        return -1;
    else if (left_glyph->m_size[Dim::X] == right_glyph->m_size[Dim::X])
        return left_glyph->m_size[Dim::Y] - right_glyph->m_size[Dim::Y];
    else
        return 1;
}

int AsciiFont::GlyphSpecification::SortByHeightFirst (
    void const *const left_operand,
    void const *const right_operand)
{
    GlyphSpecification const *left_glyph =
        *static_cast<GlyphSpecification const *const *>(left_operand);
    GlyphSpecification const *right_glyph =
        *static_cast<GlyphSpecification const *const *>(right_operand);

    // sort by height first, then width.
    if (left_glyph->m_size[Dim::Y] < right_glyph->m_size[Dim::Y])
        return -1;
    else if (left_glyph->m_size[Dim::Y] == right_glyph->m_size[Dim::Y])
        return left_glyph->m_size[Dim::X] - right_glyph->m_size[Dim::X];
    else
        return 1;
}

// ///////////////////////////////////////////////////////////////////////////
// AsciiFont
// ///////////////////////////////////////////////////////////////////////////

FontBase *AsciiFont::Create (
    std::string const &font_face_filename,
    ScreenCoord const pixel_height)
{
    Resource<FontFace> font_face =
        Singletons::ResourceLibrary()->LoadFilename<FontFace>(
            FontFace::Create,
            font_face_filename);
    return Create(font_face, pixel_height);
}

FontBase *AsciiFont::Create (
    Resource<FontFace> const &font_face,
    ScreenCoord const pixel_height)
{
    AsciiFont *retval = NULL;

    if (!font_face.GetIsValid())
        return retval;

    FT_FaceRec_ *ft_face = font_face->GetFTFace();
    FT_Error error;

    ASSERT1(ft_face != NULL)
    error = FT_Set_Pixel_Sizes(ft_face, 0, pixel_height);
    if (error != 0)
        return retval;

    retval = new AsciiFont(pixel_height);
    retval->m_has_kerning = FT_HAS_KERNING(ft_face) != 0;
    retval->m_pixel_height = pixel_height;
    retval->m_font_face = font_face;

    retval->PopulateGlyphSpecification(font_face);

    // make a list of pointers to each glyph specification
    GlyphSpecification *sorted_glyph_specification[RENDERED_GLYPH_COUNT];
    for (Uint32 i = 0; i < RENDERED_GLYPH_COUNT; ++i)
        sorted_glyph_specification[i] = &retval->m_glyph_specification[i];

    // sort them by width then height
    qsort(
        sorted_glyph_specification,
        RENDERED_GLYPH_COUNT,
        sizeof(GlyphSpecification const *),
        GlyphSpecification::SortByWidthFirst);

    // find the smallest texture size that will fit all the glyphs
    ScreenCoordVector2 smallest_fitting_texture_size =
        retval->FindSmallestFittingTextureSize(sorted_glyph_specification);
    ASSERT1(smallest_fitting_texture_size != ScreenCoordVector2::ms_zero)

    // generate the texture using the calculated size
    retval->GenerateTexture(smallest_fitting_texture_size);
    ASSERT1(retval->m_gl_texture != NULL)

    return retval;
}

char const *AsciiFont::GetNextGlyph (char const *current_glyph) const
{
    ASSERT1(current_glyph != NULL)
    // in ascii, each glyph is a single character, so simple pointer
    // incrementing works.  but if we're at a null character, return
    // the same value to indicate the end of the string.
    if (*current_glyph != '\0')
        ++current_glyph;
    return current_glyph;
}

void AsciiFont::MoveThroughGlyph (
    ScreenCoordVector2 *const pen_position_26_6,
    ScreenCoordVector2 const &initial_pen_position,
    char const *const current_glyph,
    char const *const next_glyph) const
{
    ASSERT1(current_glyph != NULL)
    ASSERT1(*current_glyph != '\0')

    if (*current_glyph == '\n')
    {
        (*pen_position_26_6)[Dim::X] = initial_pen_position[Dim::X] << 6;
        (*pen_position_26_6)[Dim::Y] -= GetPixelHeight() << 6;
    }
    else if (*current_glyph == '\t')
    {
        (*pen_position_26_6)[Dim::X] += 4 * m_glyph_specification[GetGlyphIndex(' ')].m_advance_26_6;
    }
    else
    {
        (*pen_position_26_6)[Dim::X] += m_glyph_specification[GetGlyphIndex(*current_glyph)].m_advance_26_6;
        if (next_glyph != NULL)
            (*pen_position_26_6)[Dim::X] += GetKerningPixelAdvance_26_6(*current_glyph, *next_glyph);
    }
}

void AsciiFont::DrawGlyphSetup (RenderContext const &render_context) const
{
    ASSERT1(m_gl_texture != NULL)

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_gl_texture->GetHandle());

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();
    glScalef(
        1.0f / m_gl_texture->GetWidth(),
        1.0f / m_gl_texture->GetHeight(),
        1.0f);

    // set up the rendering color
    glColor4fv(render_context.GetColorMask().m);

    // start rendering one quad for each glyph
    glBegin(GL_QUADS);
}

void AsciiFont::DrawGlyphShutdown (RenderContext const &render_context) const
{
    // stop rendering glyph quads
    glEnd();

    // pop the texture matrix
    ASSERT1(GL::GetMatrixMode() == GL_TEXTURE)
    glPopMatrix();

    // pop the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void AsciiFont::DrawGlyph (
    RenderContext const &render_context,
    char const *const glyph,
    ScreenCoordVector2 const &pen_position_26_6) const
{
    ASSERT1(glyph != NULL)
    ASSERT1(*glyph != '\0')

    Uint32 glyph_index = GetGlyphIndex(*glyph);

    ScreenCoordRect glyph_texture_coordinates(m_glyph_specification[glyph_index].m_size);
    glyph_texture_coordinates += m_glyph_specification[glyph_index].m_texture_coordinates;

    // add the horizontal bearing and round the 26.6 fixed
    // point pen position to the nearest int
    ScreenCoordVector2 pen_position(pen_position_26_6);
    pen_position[Dim::X] += m_glyph_specification[glyph_index].m_bearing_26_6[Dim::X];
//     pen_position[Dim::X] = Math::FixedPointRound<6>(pen_position[Dim::X]);
//     pen_position[Dim::Y] = Math::FixedPointRound<6>(pen_position[Dim::Y]);
    pen_position[Dim::X] >>= 6;
    pen_position[Dim::Y] >>= 6;
    ScreenCoordRect glyph_vertex_coordinates(m_glyph_specification[glyph_index].m_size);
    glyph_vertex_coordinates += pen_position;
    glyph_vertex_coordinates +=
        ScreenCoordVector2(
            0,
            m_baseline_height +
            (m_glyph_specification[glyph_index].m_bearing_26_6[Dim::Y]>>6) -
            m_glyph_specification[glyph_index].m_size[Dim::Y] -
            m_pixel_height);

    // the reason the texture coordinates look backwards (bottom on top)
    // is because the texture coordinates use a left-handed coordinate
    // system.

    glTexCoord2iv(glyph_texture_coordinates.GetBottomLeft().m);
    glVertex2iv(glyph_vertex_coordinates.GetTopLeft().m);

    glTexCoord2iv(glyph_texture_coordinates.GetTopLeft().m);
    glVertex2iv(glyph_vertex_coordinates.GetBottomLeft().m);

    glTexCoord2iv(glyph_texture_coordinates.GetTopRight().m);
    glVertex2iv(glyph_vertex_coordinates.GetBottomRight().m);

    glTexCoord2iv(glyph_texture_coordinates.GetBottomRight().m);
    glVertex2iv(glyph_vertex_coordinates.GetTopRight().m);
}

ScreenCoord AsciiFont::GetKerningPixelAdvance_26_6 (char const left, char const right) const
{
    if (!m_has_kerning)
        return 0;

    ASSERT1(m_font_face.GetIsValid());

    // TODO: somehow cache the kern pairs, so we don't have to call
    // FT_Set_Pixel_Sizes and FT_Get_Kerning here (thus making this
    // method non-reentrant)

    FT_Error error = FT_Set_Pixel_Sizes(m_font_face->GetFTFace(), 0, m_pixel_height);
    if (error != 0)
        return 0;

    FT_Vector delta;
    FT_Get_Kerning(
        m_font_face->GetFTFace(),
        FT_Get_Char_Index(m_font_face->GetFTFace(), left),
        FT_Get_Char_Index(m_font_face->GetFTFace(), right),
        FT_KERNING_UNFITTED,
        &delta);

    // delta is in 26.6 fixed point format
//     fprintf(stderr, "\"%c%c\"=%g\n", left, right, delta.x / 64.0f);
    return delta.x;
}

void AsciiFont::PopulateGlyphSpecification (Resource<FontFace> const &font_face)
{
    FT_Error error;
    FT_FaceRec_ *ft_face = font_face->GetFTFace();
    ScreenCoord tallest_glyph_height = -1;
    ScreenCoord tallest_glyph_bearing_y = -1;

    for (char ascii = RENDERED_GLYPH_LOWEST;
         ascii != RENDERED_GLYPH_HIGHEST+1;
         ++ascii)
    {
        error = FT_Load_Glyph(ft_face, FT_Get_Char_Index(ft_face, ascii), FT_LOAD_DEFAULT);
        ASSERT1(error == 0)

        GlyphSpecification &glyph = m_glyph_specification[GetGlyphIndex(ascii)];

        glyph.m_ascii = ascii;
        glyph.m_size.SetComponents(
            ft_face->glyph->metrics.width >> 6,
            ft_face->glyph->metrics.height >> 6);
        glyph.m_bearing_26_6.SetComponents(
            ft_face->glyph->metrics.horiBearingX,
            ft_face->glyph->metrics.horiBearingY);
        glyph.m_advance_26_6 = ft_face->glyph->metrics.horiAdvance;

        if (tallest_glyph_height < glyph.m_size[Dim::Y])
        {
            tallest_glyph_height = glyph.m_size[Dim::Y];
            tallest_glyph_bearing_y = glyph.m_bearing_26_6[Dim::Y]>>6;
        }
    }

    m_baseline_height =
        (m_pixel_height - tallest_glyph_height) / 2 +
        (tallest_glyph_height - tallest_glyph_bearing_y);
}

ScreenCoordVector2 AsciiFont::FindSmallestFittingTextureSize (
    GlyphSpecification *const *const sorted_glyph_specification)
{
    GLint max_texture_size;
    // TODO: replace with an accessor to the current video options
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);

    Dim::Component increase = Dim::Y;
    ScreenCoordVector2 texture_size(32, 32);
    while(texture_size[Dim::X] <= static_cast<ScreenCoord>(max_texture_size) &&
          texture_size[Dim::Y] <= static_cast<ScreenCoord>(max_texture_size))
    {
        Uint32 used_area = GetUsedTextureArea(texture_size, sorted_glyph_specification);
        DEBUG1_CODE(Uint32 total_area = texture_size[Dim::X] * texture_size[Dim::Y]);
        ASSERT1(used_area <= total_area)
        if (used_area > 0)
            return texture_size;

        texture_size[increase] <<= 1;
        increase = (increase == Dim::X) ? Dim::Y : Dim::X;
    }
    // TODO: proper handling -- scale the face down and try again
    ASSERT0(false && "No texture big enough to hold all glyphs")
    return ScreenCoordVector2::ms_zero;
}

Uint32 AsciiFont::GetUsedTextureArea (
    ScreenCoordVector2 const &texture_size,
    GlyphSpecification *const *const sorted_glyph_specification)
{
    ASSERT1(texture_size[Dim::X] > 0)
    ASSERT1(texture_size[Dim::Y] > 0)
    // the left hand size of this cryptic expression evaluates to 0
    // for zero or any non-negative integer power of 2 (e.g. 0, 1, 2,
    // 4, 8, 16, 32, 64, etc).
    ASSERT1((texture_size[Dim::X] & (texture_size[Dim::X] - 1)) == 0)
    ASSERT1((texture_size[Dim::Y] & (texture_size[Dim::Y] - 1)) == 0)
    ASSERT1(sorted_glyph_specification != NULL)

    ScreenCoord packed_width = 0;
    ScreenCoordVector2 current_packing_area = ScreenCoordVector2::ms_zero;
    Uint32 index = 0;
    Uint32 total_area = 0;

    while (packed_width + current_packing_area[Dim::X] <= texture_size[Dim::X] &&
           index < RENDERED_GLYPH_COUNT)
    {
        GlyphSpecification *glyph = sorted_glyph_specification[index];
        ASSERT1(glyph != NULL)

        // return failure if this single glyph doesn't even fit
        // inside the entire texture.
        if (glyph->m_size[Dim::X] > texture_size[Dim::X] ||
            glyph->m_size[Dim::Y] > texture_size[Dim::Y])
            return 0;

        // if the glyph would stick off the bottom of this column,
        // go to the top of the next column.
        if (current_packing_area[Dim::Y] + glyph->m_size[Dim::Y] > texture_size[Dim::Y])
        {
            packed_width += current_packing_area[Dim::X];
            current_packing_area[Dim::Y] = 0;
        }
        // otherwise stick the glyph at the bottom of the current column.
        else
        {
            total_area += glyph->m_size[Dim::X] * glyph->m_size[Dim::Y];
            glyph->m_texture_coordinates.SetComponents(
                packed_width,
                current_packing_area[Dim::Y]);
            if (current_packing_area[Dim::X] < glyph->m_size[Dim::X])
                current_packing_area[Dim::X] = glyph->m_size[Dim::X];
            current_packing_area[Dim::Y] += glyph->m_size[Dim::Y];
            ++index;
        }
        ASSERT1(current_packing_area[Dim::Y] <= texture_size[Dim::Y])
    }

    // return with success iff all the glyphs were stored within
    // the bounds of the texture.
    if (packed_width <= texture_size[Dim::X] && index == RENDERED_GLYPH_COUNT)
    {
        ASSERT1(total_area > 0)
        return total_area;
    }
    else
        return 0;
}

void AsciiFont::GenerateTexture (ScreenCoordVector2 const &texture_size)
{
    Texture *texture = Texture::Create(texture_size, false);

    FT_Error error;
    FT_FaceRec_ *ft_face = m_font_face->GetFTFace();
    for (Uint32 i = 0; i < RENDERED_GLYPH_COUNT; ++i)
    {
        GlyphSpecification &glyph = m_glyph_specification[i];

        error = FT_Load_Glyph(ft_face, FT_Get_Char_Index(ft_face, glyph.m_ascii), FT_LOAD_RENDER);
        ASSERT1(error == 0)
        ASSERT1(ft_face->glyph->format == FT_GLYPH_FORMAT_BITMAP)

        // copy the bitmap data over
        // TODO: real method which does proper pitch detection
        ASSERT1(ft_face->glyph->bitmap.pitch == glyph.m_size[Dim::X])
        Uint8 const *source_pixel_data = ft_face->glyph->bitmap.buffer;
        for (Sint32 y = 0; y < glyph.m_size[Dim::Y]; ++y)
        {
            Uint8 *dest_pixel_data =
                texture->GetData() +
                (glyph.m_texture_coordinates[Dim::Y] + y) * texture_size[Dim::X] * 4 +
                glyph.m_texture_coordinates[Dim::X] * 4;

            for (Sint32 x = 0; x < glyph.m_size[Dim::X]; ++x)
            {
                // assumed to be 32 bit RGBA for now
                *(dest_pixel_data++) = 0xFF;
                *(dest_pixel_data++) = 0xFF;
                *(dest_pixel_data++) = 0xFF;
                *(dest_pixel_data++) = *(source_pixel_data++);
            }
        }
    }

//     texture->WritePNG("font_rendering.png");

    m_gl_texture = GLTexture::Create(texture);
    ASSERT1(m_gl_texture != NULL)
    DeleteAndNullify(texture);
}

} // end of namespace Xrb

