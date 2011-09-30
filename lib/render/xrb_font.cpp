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

#include "xrb_asciifont.hpp"
#include "xrb_filesystem.hpp"
#include "xrb_math.hpp"
#include "xrb_pal.hpp"
#include "xrb_rendercontext.hpp"
#include "xrb_utf8.hpp"

namespace Xrb {

FontCoordVector2 ScreenToFontCoordVector2 (ScreenCoordVector2 const &v)
{
    return FontCoordVector2(ScreenToFontCoord(v[Dim::X]), ScreenToFontCoord(v[Dim::Y]));
}

ScreenCoordVector2 FontToScreenCoordVector2 (FontCoordVector2 const &v)
{
    return ScreenCoordVector2(FontToScreenCoord(v[Dim::X]), FontToScreenCoord(v[Dim::Y]));
}

// ///////////////////////////////////////////////////////////////////////////
// Font::LoadParameters
// ///////////////////////////////////////////////////////////////////////////

std::string Font::LoadParameters::ResourceName () const
{
    return "Xrb::Font";
}

bool Font::LoadParameters::IsLessThan (ResourceLoadParameters const &p) const
{
    LoadParameters const &rhs = p.As<LoadParameters>();
    int comparison = m_path.compare(rhs.m_path);
    if (comparison < 0)
        return true;
    else if (comparison > 0)
        return false;
    else
        return m_pixel_height < rhs.m_pixel_height;
}

bool Font::LoadParameters::IsFallback () const
{
    return m_path.empty();
}

void Font::LoadParameters::Fallback ()
{
    // get rid of the path; a blank path will indicate the "missing" texture
    m_path.clear();
    m_pixel_height = 10; // arbitrary for now
}

void Font::LoadParameters::Print (std::ostream &stream) const
{
    stream << "path = \"" << m_path << "\", pixel height = " << m_pixel_height;
}

// ///////////////////////////////////////////////////////////////////////////
// Font
// ///////////////////////////////////////////////////////////////////////////

Font *Font::Create (ResourceLoadParameters const &p)
{
    LoadParameters const &load_parameters = p.As<LoadParameters>();

    // an empty path indicates to load the "missing" font.
    if (load_parameters.Path().empty())
    {
        ASSERT0(false && "TODO");
        return NULL;
    }

    // attempt to load the font from cached data.
    // NOTE: technically Font should not know about AsciiFont, but this will do until it becomes a problem
    {
        AsciiFont *cached_font = NULL;
        // first check if this font and size are cached on disk
        cached_font = AsciiFont::CreateFromCache(load_parameters.Path(), load_parameters.PixelHeight());
        if (cached_font != NULL)
            return cached_font;
    }

    return Singleton::Pal().LoadFont(load_parameters.Path().c_str(), load_parameters.PixelHeight());
}

ScreenCoordRect Font::StringRect (char const *string) const
{
    ASSERT1(string != NULL);

    FontCoordVector2 pen_position_26_6(FontCoordVector2::ms_zero);
    FontCoordVector2 pen_position_span_26_6(pen_position_26_6);

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
        Min(FontToScreenCoord(pen_position_span_26_6[Dim::X]), ScreenCoord(0)),
        Min(FontToScreenCoord(pen_position_span_26_6[Dim::Y]), ScreenCoord(0)),
        Max(FontToScreenCoord(pen_position_span_26_6[Dim::X]), ScreenCoord(0)),
        Max(FontToScreenCoord(pen_position_span_26_6[Dim::Y]), ScreenCoord(0)));
}

ScreenCoordRect Font::StringRect (LineFormatVector const &line_format_vector) const
{
    ASSERT1(!line_format_vector.empty());

    ScreenCoord width = 0;
    for (Font::LineFormatVector::const_iterator it = line_format_vector.begin(),
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
    char const *string) const
{
    // this is for any pre-rendering setup DrawGlyph needs to do
    DrawGlyphSetup(render_context);
    // this does the actual rendering
    DrawStringPrivate(render_context, initial_pen_position, string);
    // this is for any post-rendering shutdown DrawGlyph needs to do
    DrawGlyphShutdown(render_context);
}

void Font::GenerateLineFormatVector (
    char const *source_string,
    LineFormatVector *dest_line_format_vector) const
{
    ASSERT1(source_string != NULL);
    ASSERT1(dest_line_format_vector != NULL);

    dest_line_format_vector->clear();

    FontCoordVector2 pen_position_26_6(FontCoordVector2::ms_zero);
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
            pen_position_26_6 = FontCoordVector2::ms_zero;
            line_format.m_ptr = current_glyph;
            line_format.m_glyph_count = 0;
            line_start = false;
        }

        // if it's a newline, signal line_start and push the current
        // line_format struct
        if (*current_glyph == '\n')
        {
            line_start = true;
            line_format.m_width = FontToScreenCoord(pen_position_26_6[Dim::X]);
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
    line_format.m_width = FontToScreenCoord(Abs(pen_position_26_6[Dim::X]));
    dest_line_format_vector->push_back(line_format);
}

void Font::DrawLineFormattedText (
    RenderContext const &render_context,
    ScreenCoordRect const &draw_rect,
    char const *source_string,
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
                pen_position[Dim::X] = initial_pen_position[Dim::X] + (draw_rect.Width() - line_format_vector[line].m_width) / 2;
                break;

            case RIGHT:
                pen_position[Dim::X] = initial_pen_position[Dim::X] + draw_rect.Width() - line_format_vector[line].m_width;
                break;

            case SPACED:
                pen_position[Dim::X] = initial_pen_position[Dim::X];
                total_spacing[Dim::X] = draw_rect.Width() - line_format_vector[line].m_width;
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
    char const *string,
    char const *string_terminator,
    Uint32 remaining_glyph_count,
    ScreenCoord remaining_space) const
{
    ASSERT1(string != NULL);

    FontCoordVector2 pen_position_26_6(ScreenToFontCoordVector2(initial_pen_position));
    FontCoord major_space_26_6 = ScreenToFontCoord(remaining_space);

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
    FontCoordVector2 *pen_position_span_26_6,
    FontCoordVector2 const &pen_position_26_6) const
{
    ASSERT1(pen_position_span_26_6 != NULL);

    (*pen_position_span_26_6)[Dim::X] = Max((*pen_position_span_26_6)[Dim::X], pen_position_26_6[Dim::X]);
    (*pen_position_span_26_6)[Dim::Y] = Min((*pen_position_span_26_6)[Dim::Y], pen_position_26_6[Dim::Y]);
}

} // end of namespace Xrb

