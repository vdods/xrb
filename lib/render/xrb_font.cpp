// ///////////////////////////////////////////////////////////////////////////
// xrb_font.cpp by Victor Dods, created 2005/06/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_font.hpp"

#include "ft2build.h"
#include FT_FREETYPE_H
#include "xrb_gl.hpp"
#include "xrb_math.hpp"
#include "xrb_render.hpp"
#include "xrb_rendercontext.hpp"
#include "xrb_texture.hpp"
#include "xrb_utf8.hpp"

namespace Xrb
{

// ///////////////////////////////////////////////////////////////////////////
// Font
// ///////////////////////////////////////////////////////////////////////////

ScreenCoordRect Font::StringRect (char const *const string) const
{
    ASSERT1(string != NULL);

    ScreenCoordVector2 pen_position_26_6(ScreenCoordVector2::ms_zero);
    ScreenCoordVector2 pen_position_span_26_6(pen_position_26_6);

    char const *current_glyph = string;
    char const *next_glyph;
    // the end of the string is indicated when current_glyph == next_glyph
    while (*current_glyph != '\0')
    {
        // get the glyph after the current one
        next_glyph = UTF8::NextCharacter(current_glyph);
        // move through all the normal glyphs (do no spacing)
        MoveThroughGlyph(
            &pen_position_26_6,
            ScreenCoordVector2::ms_zero,
            current_glyph,
            next_glyph,
            NULL,
            NULL);
        // advance one glyph
        current_glyph = next_glyph;
        // keep track of the bounding box
        TrackBoundingBox(&pen_position_span_26_6, pen_position_26_6);
    }

    // tack on a newline at the end so the last line is counted (do no spacing)
    MoveThroughGlyph(
        &pen_position_26_6,
        ScreenCoordVector2::ms_zero,
        "\n",
        NULL,
        NULL,
        NULL);
    // update the bounding box one more time
    TrackBoundingBox(&pen_position_span_26_6, pen_position_26_6);

    // round pen_position_span
    pen_position_span_26_6[Dim::X] = Math::FixedPointRound<6>(pen_position_span_26_6[Dim::X]);
    pen_position_span_26_6[Dim::Y] = Math::FixedPointRound<6>(pen_position_span_26_6[Dim::Y]);

    return ScreenCoordRect(
        Min(pen_position_span_26_6[Dim::X]>>6, 0),
        Min(pen_position_span_26_6[Dim::Y]>>6, 0),
        Max(pen_position_span_26_6[Dim::X]>>6, 0),
        Max(pen_position_span_26_6[Dim::Y]>>6, 0));
}

ScreenCoordRect Font::StringRect (LineFormatVector const &line_format_vector) const
{
    ASSERT1(!line_format_vector.empty());

    ScreenCoord width = 0;
    for (Font::LineFormatVectorConstIterator it = line_format_vector.begin(),
                                             it_end = line_format_vector.end();
         it != it_end;
         ++it)
    {
        width = Max(width, it->m_width);
    }
    return ScreenCoordRect(ScreenCoordVector2(width, line_format_vector.size() * PixelHeight()));
}

void Font::DrawString (
    RenderContext const &render_context,
    ScreenCoordVector2 const &initial_pen_position,
    char const *const string) const
{
    // this is for any pre-rendering setup DrawGlyph needs to do
    DrawGlyphSetup(render_context);
    // this does the actual rendering
    DrawStringPrivate(render_context, initial_pen_position, string);
    // this is for any post-rendering shutdown DrawGlyph needs to do
    DrawGlyphShutdown(render_context);
}

void Font::GenerateLineFormatVector (
    char const *const source_string,
    LineFormatVector *const dest_line_format_vector) const
{
    ASSERT1(source_string != NULL);
    ASSERT1(dest_line_format_vector != NULL);

    dest_line_format_vector->clear();

    ScreenCoordVector2 pen_position_26_6(ScreenCoordVector2::ms_zero);
    LineFormat line_format;
    line_format.m_ptr = source_string;
    line_format.m_glyph_count = 0;
    bool line_start = true;
    // iterate over the whole string
    char const *current_glyph = source_string;
    char const *next_glyph;
    while (*current_glyph != '\0')
    {
        // get the glyph after the current one
        next_glyph = UTF8::NextCharacter(current_glyph);

        // if this is a new line, re-init the format values and line_start
        if (line_start)
        {
            pen_position_26_6 = ScreenCoordVector2::ms_zero;
            line_format.m_ptr = current_glyph;
            line_format.m_glyph_count = 0;
            line_start = false;
        }

        // if it's a newline, signal line_start and push the current
        // line_format struct
        if (*current_glyph == '\n')
        {
            line_start = true;
            line_format.m_width = pen_position_26_6[Dim::X] >> 6;
            dest_line_format_vector->push_back(line_format);
        }
        // otherwise increment the glyph count
        else
            ++line_format.m_glyph_count;

        // now move through the glyph (which, if a newline,
        // will advance the pen downward).
        MoveThroughGlyph(
            &pen_position_26_6,
            ScreenCoordVector2::ms_zero,
            current_glyph,
            next_glyph);

        // advance one glyph
        current_glyph = next_glyph;
    }
    // make sure to push the last one
    line_format.m_width = Abs(pen_position_26_6[Dim::X]) >> 6;
    dest_line_format_vector->push_back(line_format);
}

void Font::DrawLineFormattedText (
    RenderContext const &render_context,
    ScreenCoordRect const &draw_rect,
    char const *const source_string,
    LineFormatVector const &line_format_vector,
    Alignment2 const &alignment) const
{
    ASSERT1(line_format_vector.size() > 0);

    // early out if alignment is (LEFT, TOP)
    if (alignment[Dim::X] == LEFT && alignment[Dim::Y] == TOP)
    {
        DrawString(render_context, draw_rect.TopLeft(), source_string);
        return;
    }

    ScreenCoord text_height =
        line_format_vector.size() * PixelHeight();
    ScreenCoordVector2 total_spacing = ScreenCoordVector2::ms_zero;
    ScreenCoordVector2 initial_pen_position(draw_rect.TopLeft());
    ScreenCoordVector2 pen_position(initial_pen_position);

    switch (alignment[Dim::Y])
    {
        case TOP: break; // pen_position and total_spacing are correct already

        case CENTER:
            pen_position[Dim::Y] = initial_pen_position[Dim::Y] - (draw_rect.Height() - text_height) / 2;
            // total spacing is already set up correctly
            break;

        case BOTTOM:
            pen_position[Dim::Y] = initial_pen_position[Dim::Y] - (draw_rect.Height() - text_height);
            // total spacing is already set up correctly
            break;

        case SPACED:
            if (line_format_vector.size() == 1)
                pen_position[Dim::Y] = initial_pen_position[Dim::Y] - (draw_rect.Height() - text_height) / 2;
                // total spacing is set up correctly
            else
                // current position is set up correctly
                total_spacing[Dim::Y] = draw_rect.Height() - text_height;
            // range checking
            if (total_spacing[Dim::Y] < 0)
                total_spacing[Dim::Y] = 0;
            break;

        default: ASSERT0(false && "Invalid Alignment"); break;
    }

    // this is for any pre-rendering setup DrawGlyph needs to do
    DrawGlyphSetup(render_context);

    Uint32 spacing_lines_left = line_format_vector.size() - 1;
    for (Uint32 line = 0; line < line_format_vector.size(); ++line)
    {
        // horizontal justification
        switch (alignment[Dim::X])
        {
            case LEFT:
                pen_position[Dim::X] = initial_pen_position[Dim::X];
                break;

            case CENTER:
                pen_position[Dim::X] = initial_pen_position[Dim::X] + (draw_rect.GetWidth() - line_format_vector[line].m_width) / 2;
                break;

            case RIGHT:
                pen_position[Dim::X] = initial_pen_position[Dim::X] + draw_rect.GetWidth() - line_format_vector[line].m_width;
                break;

            case SPACED:
                pen_position[Dim::X] = initial_pen_position[Dim::X];
                total_spacing[Dim::X] = draw_rect.GetWidth() - line_format_vector[line].m_width;
                // if the line isn't long enough, don't space it out
                if (total_spacing[Dim::X] > 4 * m_pixel_height)
                    total_spacing[Dim::X] = 0;
                // range checking
                else if (total_spacing[Dim::X] < 0)
                    total_spacing[Dim::X] = 0;
                break;

            default:
                ASSERT0(false && "Invalid Alignment");
                break;
        }

        DrawStringPrivate(
            render_context,
            pen_position,
            line_format_vector[line].m_ptr,
            "\n",
            line_format_vector[line].m_glyph_count,
            total_spacing[Dim::X]);
        // move the pen position down a line
        pen_position[Dim::Y] -= PixelHeight();

        if (spacing_lines_left > 0)
        {
            ScreenCoord spacing_to_use = total_spacing[Dim::Y] / spacing_lines_left--;
            pen_position[Dim::Y] -= spacing_to_use;
            total_spacing[Dim::Y] -= spacing_to_use;
        }
    }

    // this is for any post-rendering shutdown DrawGlyph needs to do
    DrawGlyphShutdown(render_context);
}

void Font::DrawStringPrivate (
    RenderContext const &render_context,
    ScreenCoordVector2 const &initial_pen_position,
    char const *const string,
    char const *const string_terminator,
    Uint32 remaining_glyph_count,
    ScreenCoord const remaining_space) const
{
    ASSERT1(string != NULL);

    ScreenCoordVector2 pen_position_26_6(
        initial_pen_position[Dim::X] << 6,
        initial_pen_position[Dim::Y] << 6);
    ScreenCoord major_space_26_6 = remaining_space << 6;

    char const *current_glyph = string;
    char const *next_glyph;
    // loop until a NULL char (or the string_terminator, if specified) is hit.
    while (*current_glyph != '\0' &&
           !UTF8::AreCharactersEqual(current_glyph, string_terminator))
    {
        // get the glyph after the current one
        next_glyph = UTF8::NextCharacter(current_glyph);
        // draw the glyph
        DrawGlyph(render_context, current_glyph, pen_position_26_6);
        // advance the pen
        MoveThroughGlyph(
            &pen_position_26_6,
            initial_pen_position,
            current_glyph,
            next_glyph,
            &remaining_glyph_count,
            &major_space_26_6);
        // advance one glyph
        current_glyph = next_glyph;
    }
}

void Font::TrackBoundingBox (
    ScreenCoordVector2 *const pen_position_span_26_6,
    ScreenCoordVector2 const &pen_position_26_6) const
{
    ASSERT1(pen_position_span_26_6 != NULL);

    (*pen_position_span_26_6)[Dim::X] = Max((*pen_position_span_26_6)[Dim::X], pen_position_26_6[Dim::X]);
    (*pen_position_span_26_6)[Dim::Y] = Min((*pen_position_span_26_6)[Dim::Y], pen_position_26_6[Dim::Y]);
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

Font *AsciiFont::Create (
    std::string const &font_face_filename,
    ScreenCoord const pixel_height)
{
    Resource<FontFace> font_face =
        Singletons::ResourceLibrary().LoadFilename<FontFace>(
            FontFace::Create,
            font_face_filename);
    return Create(font_face, pixel_height);
}

Font *AsciiFont::Create (
    Resource<FontFace> const &font_face,
    ScreenCoord const pixel_height)
{
    AsciiFont *retval = NULL;

    if (!font_face.IsValid())
        return retval;

    FT_FaceRec_ *ft_face = font_face->FTFace();
    FT_Error error;

    ASSERT1(ft_face != NULL);
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
    ASSERT1(smallest_fitting_texture_size != ScreenCoordVector2::ms_zero);

    // generate the texture using the calculated size
    retval->GenerateTexture(smallest_fitting_texture_size);
    ASSERT1(retval->m_gl_texture != NULL);

    return retval;
}

void AsciiFont::MoveThroughGlyph (
    ScreenCoordVector2 *const pen_position_26_6,
    ScreenCoordVector2 const &initial_pen_position,
    char const *const current_glyph,
    char const *const next_glyph,
    Uint32 *remaining_glyph_count,
    ScreenCoord *major_space_26_6) const
{
    ASSERT1(current_glyph != NULL);
    ASSERT1(*current_glyph != '\0');

    if (*current_glyph == '\n')
    {
        (*pen_position_26_6)[Dim::X] = initial_pen_position[Dim::X] << 6;
        (*pen_position_26_6)[Dim::Y] -= PixelHeight() << 6;
    }
    else
    {
        if (*current_glyph == '\t')
        {
            (*pen_position_26_6)[Dim::X] += TAB_SIZE * m_glyph_specification[GlyphIndex(' ')].m_advance_26_6;
        }
        else
        {
            (*pen_position_26_6)[Dim::X] += m_glyph_specification[GlyphIndex(*current_glyph)].m_advance_26_6;
            if (next_glyph != NULL)
                (*pen_position_26_6)[Dim::X] += KerningPixelAdvance_26_6(*current_glyph, *next_glyph);
        }

        if (remaining_glyph_count != NULL && *remaining_glyph_count > 1)
        {
            ASSERT1(major_space_26_6 != NULL);
            *remaining_glyph_count -= 1;
            ScreenCoord space_to_use_26_6 = *major_space_26_6 / *remaining_glyph_count;
            ASSERT1(space_to_use_26_6 <= *major_space_26_6);
            *major_space_26_6 -= space_to_use_26_6;
            (*pen_position_26_6)[Dim::X] += space_to_use_26_6;
        }
    }
}

void AsciiFont::GenerateWordWrappedString (
    std::string const &source_string,
    std::string *const dest_string,
    ScreenCoordVector2 const &text_area_size) const
{
    ASSERT1(dest_string != NULL);

    // clear the destination string
    dest_string->clear();

    // the width of the string rect
    ScreenCoord wrap_width_26_6 = text_area_size[Dim::X] << 6;

    bool forced_newline = false;
    bool line_start = true;
    char const *current_token;
    char const *next_token;
    ScreenCoord current_pos_26_6 = 0;
    ScreenCoord token_width_26_6;
    TokenClass next_token_class;

    current_token = source_string.c_str();
    while (GetTokenClass(*current_token) != NULLCHAR)
    {
        if (line_start)
        {
            current_pos_26_6 = 0;
            // if indents are to be put back in, they should go right here
            if (forced_newline)
                *dest_string += '\n';
            while (GetTokenClass(*current_token) == WHITESPACE)
                current_token = StartOfNextToken(current_token);
            line_start = false;
            forced_newline = false;
        }

        switch (GetTokenClass(*current_token))
        {
            case WHITESPACE:
                // check if the token after this is a newline, a nullchar
                // or a word that won't fit:
                next_token = StartOfNextToken(current_token);
                next_token_class = GetTokenClass(*next_token);
                token_width_26_6 = TokenWidth_26_6(" ") + TokenWidth_26_6(next_token);
                if (next_token_class == NULLCHAR)
                {
                }
                else if (next_token_class == NEWLINE)
                {
                }
                else if (next_token_class == WORD &&
                         current_pos_26_6 + token_width_26_6 > wrap_width_26_6)
                {
                    *dest_string += '\n';
                    line_start = true;
                }
                else
                {
                    // only if all of the above criteria fail we add in a space
                    *dest_string += ' ';
                    current_pos_26_6 += TokenWidth_26_6(" ");
                }
                current_token = next_token;
                break;

            case NEWLINE:
                *dest_string += '\n';
                forced_newline = true;
                line_start = true;
                current_token = StartOfNextToken(current_token);
                break;

            case NULLCHAR:
                break;

            case WORD:
                token_width_26_6 = TokenWidth_26_6(current_token);
                next_token = StartOfNextToken(current_token);
                forced_newline = false;
                if (token_width_26_6 > wrap_width_26_6)
                {
                    for (Sint32 i = 0; i < next_token - current_token; ++i)
                        *dest_string += *(current_token + i);
                    *dest_string += '\n';
                    current_token = next_token;
                    line_start = true;
                }
                else if (current_pos_26_6 + token_width_26_6 > wrap_width_26_6)
                {
                    *dest_string += '\n';
                    line_start = true;
                }
                else
                {
                    for (Sint32 i = 0; i < next_token - current_token; ++i)
                        *dest_string += *(current_token + i);
                    current_pos_26_6 += token_width_26_6;
                    current_token = next_token;
                }
                break;
        }
    }
}

void AsciiFont::DrawGlyphSetup (RenderContext const &render_context) const
{
    ASSERT1(m_gl_texture != NULL);

    if (render_context.MaskAndBiasWouldResultInNoOp())
        return;
        
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    Render::SetupTextureUnits(
        m_gl_texture->Handle(), 
        render_context.ColorMask(), 
        render_context.BiasColor());

    // make sure to reactivate texture unit 0 so that the calls to glTexCoord2iv
    // in DrawGlyph (and the matrix operations below) operate on the correct texture unit.
    glActiveTextureARB(GL_TEXTURE0_ARB);

    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();
    glScalef(
        1.0f / m_gl_texture->GetWidth(),
        1.0f / m_gl_texture->Height(),
        1.0f);

    // start rendering one quad for each glyph
    glBegin(GL_QUADS);
}

void AsciiFont::DrawGlyphShutdown (RenderContext const &render_context) const
{
    // stop rendering glyph quads
    glEnd();

    // pop the texture matrix
    ASSERT1(GL::MatrixMode() == GL_TEXTURE);
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
    ASSERT1(glyph != NULL);
    ASSERT1(*glyph != '\0');

    if (*glyph == '\n' || *glyph == '\t')
        return;

    Uint32 glyph_index = GlyphIndex(*glyph);

    ScreenCoordRect glyph_texture_coordinates(m_glyph_specification[glyph_index].m_size);
    glyph_texture_coordinates += m_glyph_specification[glyph_index].m_texture_coordinates;

    // add the horizontal bearing and round the 26.6 fixed
    // point pen position to the nearest int
    ScreenCoordVector2 pen_position(pen_position_26_6);
    pen_position[Dim::X] += m_glyph_specification[glyph_index].m_bearing_26_6[Dim::X];
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

    glTexCoord2iv(glyph_texture_coordinates.BottomLeft().m);
    glVertex2iv(glyph_vertex_coordinates.TopLeft().m);

    glTexCoord2iv(glyph_texture_coordinates.TopLeft().m);
    glVertex2iv(glyph_vertex_coordinates.BottomLeft().m);

    glTexCoord2iv(glyph_texture_coordinates.TopRight().m);
    glVertex2iv(glyph_vertex_coordinates.BottomRight().m);

    glTexCoord2iv(glyph_texture_coordinates.BottomRight().m);
    glVertex2iv(glyph_vertex_coordinates.TopRight().m);
}

ScreenCoord AsciiFont::KerningPixelAdvance_26_6 (char const left, char const right) const
{
    if (!m_has_kerning)
        return 0;

    ASSERT1(m_font_face.IsValid());

    // TODO: somehow cache the kern pairs, so we don't have to call
    // FT_Set_Pixel_Sizes and FT_Get_Kerning here (thus making this
    // method non-reentrant)

    FT_Error error = FT_Set_Pixel_Sizes(m_font_face->FTFace(), 0, m_pixel_height);
    if (error != 0)
        return 0;

    FT_Vector delta;
    FT_Get_Kerning(
        m_font_face->FTFace(),
        FT_Get_Char_Index(m_font_face->FTFace(), left),
        FT_Get_Char_Index(m_font_face->FTFace(), right),
        FT_KERNING_UNFITTED,
        &delta);

    // delta is in 26.6 fixed point format
    return delta.x;
}

void AsciiFont::PopulateGlyphSpecification (Resource<FontFace> const &font_face)
{
    FT_Error error;
    FT_FaceRec_ *ft_face = font_face->FTFace();
    ScreenCoord tallest_glyph_height = -1;
    ScreenCoord tallest_glyph_bearing_y = -1;

    for (char ascii = RENDERED_GLYPH_LOWEST;
         ascii != RENDERED_GLYPH_HIGHEST+1;
         ++ascii)
    {
        error = FT_Load_Glyph(ft_face, FT_Get_Char_Index(ft_face, ascii), FT_LOAD_DEFAULT);
        ASSERT1(error == 0);

        GlyphSpecification &glyph = m_glyph_specification[GlyphIndex(ascii)];

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
        Uint32 used_area = UsedTextureArea(texture_size, sorted_glyph_specification);
        DEBUG1_CODE(Uint32 total_area = texture_size[Dim::X] * texture_size[Dim::Y]);
        ASSERT1(used_area <= total_area);
        if (used_area > 0)
            return texture_size;

        texture_size[increase] <<= 1;
        increase = (increase == Dim::X) ? Dim::Y : Dim::X;
    }
    // TODO: proper handling -- scale the face down and try again
    ASSERT0(false && "No texture big enough to hold all glyphs");
    return ScreenCoordVector2::ms_zero;
}

Uint32 AsciiFont::UsedTextureArea (
    ScreenCoordVector2 const &texture_size,
    GlyphSpecification *const *const sorted_glyph_specification)
{
    ASSERT1(texture_size[Dim::X] > 0);
    ASSERT1(texture_size[Dim::Y] > 0);
    // the left hand size of this cryptic expression evaluates to 0
    // for zero or any non-negative integer power of 2 (e.g. 0, 1, 2,
    // 4, 8, 16, 32, 64, etc).
    ASSERT1((texture_size[Dim::X] & (texture_size[Dim::X] - 1)) == 0);
    ASSERT1((texture_size[Dim::Y] & (texture_size[Dim::Y] - 1)) == 0);
    ASSERT1(sorted_glyph_specification != NULL);

    ScreenCoord packed_width = 0;
    ScreenCoordVector2 current_packing_area = ScreenCoordVector2::ms_zero;
    Uint32 index = 0;
    Uint32 total_area = 0;

    while (packed_width + current_packing_area[Dim::X] <= texture_size[Dim::X] &&
           index < RENDERED_GLYPH_COUNT)
    {
        GlyphSpecification *glyph = sorted_glyph_specification[index];
        ASSERT1(glyph != NULL);

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
        ASSERT1(current_packing_area[Dim::Y] <= texture_size[Dim::Y]);
    }

    // return with success iff all the glyphs were stored within
    // the bounds of the texture.
    if (packed_width <= texture_size[Dim::X] && index == RENDERED_GLYPH_COUNT)
    {
        ASSERT1(total_area > 0);
        return total_area;
    }
    else
        return 0;
}

void AsciiFont::GenerateTexture (ScreenCoordVector2 const &texture_size)
{
    Texture *texture = Texture::Create(texture_size, false);

    FT_Error error;
    FT_FaceRec_ *ft_face = m_font_face->FTFace();
    for (Uint32 i = 0; i < RENDERED_GLYPH_COUNT; ++i)
    {
        GlyphSpecification &glyph = m_glyph_specification[i];

        error = FT_Load_Glyph(ft_face, FT_Get_Char_Index(ft_face, glyph.m_ascii), FT_LOAD_RENDER);
        ASSERT1(error == 0);
        ASSERT1(ft_face->glyph->format == FT_GLYPH_FORMAT_BITMAP);

        // copy the bitmap data over
        // TODO: real method which does proper pitch detection
        ASSERT1(ft_face->glyph->bitmap.pitch == glyph.m_size[Dim::X]);
        Uint8 const *source_pixel_data = ft_face->glyph->bitmap.buffer;
        for (Sint32 y = 0; y < glyph.m_size[Dim::Y]; ++y)
        {
            Uint8 *dest_pixel_data =
                texture->Data() +
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
    ASSERT1(m_gl_texture != NULL);
    DeleteAndNullify(texture);
}

AsciiFont::TokenClass AsciiFont::GetTokenClass (char const c)
{
    switch (c)
    {
        case ' ' :
        case '\t': return WHITESPACE;
        case '\n': return NEWLINE;
        case '\0': return NULLCHAR;
        default  : return WORD;
    }
}

char const *AsciiFont::StartOfNextToken (char const *string)
{
    ASSERT1(string != NULL);

    Sint32 token_class = GetTokenClass(*string);

    if (token_class == NEWLINE)
        return ++string;
    else if (token_class == NULLCHAR)
        return string;

    while (*string != '\0' && GetTokenClass(*string) == token_class)
        ++string;

    return string;
}

ScreenCoord AsciiFont::TokenWidth_26_6 (char const *const string) const
{
    ASSERT1(string != NULL);

    char const *current_glyph = string;
    char const *next_glyph;
    char const *const end_glyph = StartOfNextToken(current_glyph);
    ScreenCoordVector2 pen_position_26_6(ScreenCoordVector2::ms_zero);
    while (current_glyph != end_glyph)
    {
        next_glyph = current_glyph + 1;
        if (*string != '\n')
            MoveThroughGlyph(
                &pen_position_26_6,
                ScreenCoordVector2::ms_zero,
                current_glyph,
                next_glyph);
        current_glyph = next_glyph;
    }

    return pen_position_26_6[Dim::X];
}

} // end of namespace Xrb

