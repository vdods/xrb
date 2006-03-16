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
    ScreenCoord tallest_glyph_bearing_y;

    for (Uint32 unicode = RENDERED_GLYPH_LOWEST;
         unicode <= RENDERED_GLYPH_HIGHEST;
         ++unicode)
    {
        error = FT_Load_Char(ft_face, unicode, FT_LOAD_DEFAULT);
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
        Uint32 total_area = texture_size[Dim::X] * texture_size[Dim::Y];
        Uint32 used_area = GetUsedTextureArea(texture_size, sorted_glyph_specification);
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

        error = FT_Load_Char(ft_face, glyph.m_unicode, FT_LOAD_RENDER);
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

