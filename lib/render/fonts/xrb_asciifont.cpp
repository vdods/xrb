// ///////////////////////////////////////////////////////////////////////////
// xrb_asciifont.cpp by Victor Dods, created 2005/06/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_asciifont.hpp"

#include <sstream>

#include "xrb_binaryfileserializer.hpp"
#include "xrb_exception.hpp"
#include "xrb_gl.hpp"
#include "xrb_gltexture.hpp"
#include "xrb_pal.hpp"
#include "xrb_render.hpp"
#include "xrb_rendercontext.hpp"
#include "xrb_texture.hpp"
#include "xrb_util.hpp"

namespace Xrb
{

// ///////////////////////////////////////////////////////////////////////////
// AsciiFont::GlyphSpecification
// ///////////////////////////////////////////////////////////////////////////

void AsciiFont::GlyphSpecification::Read (Serializer &serializer)
{
    serializer.Read<char>(m_ascii);
    serializer.ReadAggregate<ScreenCoordVector2>(m_size);
    serializer.ReadAggregate<FontCoordVector2>(m_bearing_26_6);
    serializer.Read<FontCoord>(m_advance_26_6);
    serializer.ReadAggregate<ScreenCoordVector2>(m_texture_coordinates);
}

void AsciiFont::GlyphSpecification::Write (Serializer &serializer) const
{
    serializer.Write<char>(m_ascii);
    serializer.WriteAggregate<ScreenCoordVector2>(m_size);
    serializer.WriteAggregate<FontCoordVector2>(m_bearing_26_6);
    serializer.Write<FontCoord>(m_advance_26_6);
    serializer.WriteAggregate<ScreenCoordVector2>(m_texture_coordinates);
}

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

// ///////////////////////////////////////////////////////////////////////////
// AsciiFont
// ///////////////////////////////////////////////////////////////////////////

AsciiFont *AsciiFont::CreateFromCache (
    std::string const &font_face_path,
    ScreenCoord pixel_height)
{
    AsciiFont *retval = NULL;

    std::string font_metadata_path(FORMAT(font_face_path << '.' << pixel_height << ".data"));
    std::string font_bitmap_path(FORMAT(font_face_path << '.' << pixel_height << ".png"));

    // check for the appropriate font bitmap file
    Texture *texture = Singleton::Pal().LoadImage(font_bitmap_path.c_str());
    // if the file doesn't exist or can't be opened or loaded, get out of here
    if (texture == NULL)
        return retval;

    // check for the appropriate font metadata file
    try
    {
        BinaryFileSerializer serializer(font_metadata_path, IOD_READ);

        // now try to read the font metadata
        retval = new AsciiFont(font_face_path, pixel_height);

        Uint32 hash;
        Uint32 rendered_glyph_count;

        serializer.Read<Uint32>(hash);
        serializer.Read<bool>(retval->m_has_kerning);
        serializer.Read<ScreenCoord>(retval->m_baseline_height);
        serializer.Read<Uint32>(rendered_glyph_count);

        if (rendered_glyph_count != ms_rendered_glyph_count)
            throw Exception(FORMAT("AsciiFont::Create(\"" << font_face_path << "\", " << pixel_height << "); glyph count mismatch (got " << rendered_glyph_count << ", expected " << ms_rendered_glyph_count << ") -- delete the associated cache files"));

        for (Uint32 i = 0; i < ms_rendered_glyph_count; ++i)
            retval->m_glyph_specification[i].Read(serializer);
        serializer.ReadBuffer<FontCoord>(retval->m_kern_pair_26_6, LENGTHOF(retval->m_kern_pair_26_6));

        if (hash != retval->Hash())
            throw Exception(FORMAT("AsciiFont::Create(\"" << font_face_path << "\", " << pixel_height << "); invalid font metadata -- delete the associated cache files"));

        if (!serializer.IsAtEnd())
            throw Exception(FORMAT("AsciiFont::Create(\"" << font_face_path << "\", " << pixel_height << "); font metadata file is too long -- delete the associated cache files"));

        retval->m_gl_texture = GLTexture::Create(texture);
        ASSERT1(retval->m_gl_texture != NULL);

        fprintf(stderr, "AsciiFont::Create(\"%s\", %d); loaded cached font data\n", font_face_path.c_str(), pixel_height);
    }
    catch (Exception &e)
    {
        fprintf(stderr, "%s\n", e.what());
        DeleteAndNullify(retval);
    }

    Delete(texture);
    return retval;
}

AsciiFont *AsciiFont::Create (
    std::string const &font_face_path,
    ScreenCoord pixel_height,
    bool has_kerning,
    ScreenCoord baseline_height,
    GlyphSpecification sorted_glyph_specification[ms_rendered_glyph_count],
    FontCoord kern_pair_26_6[ms_rendered_glyph_count*ms_rendered_glyph_count],
    Texture *font_texture)
{
    ASSERT1(font_texture != NULL);

    AsciiFont *retval = new AsciiFont(font_face_path, pixel_height);

    retval->m_has_kerning = has_kerning;
    retval->m_baseline_height = baseline_height;
    memcpy(retval->m_glyph_specification, sorted_glyph_specification, sizeof(retval->m_glyph_specification));
    memcpy(retval->m_kern_pair_26_6, kern_pair_26_6, sizeof(retval->m_kern_pair_26_6));
    retval->m_gl_texture = GLTexture::Create(font_texture);
    ASSERT1(retval->m_gl_texture != NULL);

    return retval;
}

bool AsciiFont::CacheToDisk (Texture *font_texture) const
{
    ASSERT1(font_texture != NULL);

    std::string font_metadata_path(FORMAT(FontFacePath() << '.' << PixelHeight() << ".data"));
    std::string font_bitmap_path(FORMAT(FontFacePath() << '.' << PixelHeight() << ".png"));

    // write the font metadata out to disk
    try
    {
        fprintf(stderr, "AsciiFont::Create(\"%s\", %d); ", FontFacePath().c_str(), PixelHeight());

        BinaryFileSerializer serializer(font_metadata_path, IOD_WRITE);

        serializer.Write<Uint32>(Hash());
        serializer.Write<bool>(m_has_kerning);
        serializer.Write<ScreenCoord>(m_baseline_height);
        serializer.Write<Uint32>(ms_rendered_glyph_count);
        for (Uint32 i = 0; i < ms_rendered_glyph_count; ++i)
            m_glyph_specification[i].Write(serializer);
        serializer.WriteBuffer<FontCoord>(m_kern_pair_26_6, LENGTHOF(m_kern_pair_26_6));

        fprintf(stderr, "cached font data\n");

        // continue on to write the font bitmap

        fprintf(stderr, "AsciiFont::Create(\"%s\", %d); ", FontFacePath().c_str(), PixelHeight());
        bool success = Singleton::Pal().SaveImage(font_bitmap_path.c_str(), *font_texture) == Pal::SUCCESS;
        if (success)
            fprintf(stderr, "cached font bitmap\n");
        else
            fprintf(stderr, "error caching font bitmap file\n");
        return success;
    }
    catch (Exception &e)
    {
        fprintf(stderr, "error caching font data (%s)\n", e.what());
        return false;
    }
}

void AsciiFont::MoveThroughGlyph (
    FontCoordVector2 *const pen_position_26_6,
    ScreenCoordVector2 const &initial_pen_position,
    char const *const current_glyph,
    char const *const next_glyph,
    Uint32 *remaining_glyph_count,
    FontCoord *major_space_26_6) const
{
    ASSERT1(current_glyph != NULL);
    ASSERT1(*current_glyph != '\0');

    if (*current_glyph == '\n')
    {
        (*pen_position_26_6)[Dim::X] = ScreenToFontCoord(initial_pen_position[Dim::X]);
        (*pen_position_26_6)[Dim::Y] -= ScreenToFontCoord(PixelHeight());
    }
    else
    {
        if (*current_glyph == '\t')
        {
            (*pen_position_26_6)[Dim::X] += ms_tab_size * m_glyph_specification[GlyphIndex(' ')].m_advance_26_6;
        }
        else
        {
            (*pen_position_26_6)[Dim::X] += m_glyph_specification[GlyphIndex(*current_glyph)].m_advance_26_6;
            if (next_glyph != NULL)
                (*pen_position_26_6)[Dim::X] += KernPair_26_6(*current_glyph, *next_glyph);
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
    ScreenCoord wrap_width_26_6 = ScreenToFontCoord(text_area_size[Dim::X]);

    bool forced_newline = false;
    bool line_start = true;
    char const *current_token;
    char const *next_token;
    FontCoord current_pos_26_6 = 0;
    FontCoord token_width_26_6;
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
        render_context.ColorBias());

    // make sure to reactivate texture unit 0 so that the calls to glTexCoord2iv
    // in DrawGlyph (and the matrix operations below) operate on the correct texture unit.
    glActiveTexture(GL_TEXTURE0);

    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();
    glScalef(
        1.0f / m_gl_texture->Width(),
        1.0f / m_gl_texture->Height(),
        1.0f);

    // enable vertex and texture coord arrays so we can draw with glDrawArrays
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void AsciiFont::DrawGlyphShutdown (RenderContext const &render_context) const
{
    // disable vertex and texture coord arrays since we're done drawing with
    // glDrawArrays (this seems to be unnecessary, but there's probably a good
    // reason for it.)
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    // pop the texture matrix
    ASSERT1(GL::Integer(GL_MATRIX_MODE) == GL_TEXTURE);
    glPopMatrix();

    // pop the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void AsciiFont::DrawGlyph (
    RenderContext const &render_context,
    char const *const glyph,
    FontCoordVector2 const &pen_position_26_6) const
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
    ScreenCoordVector2 pen_position(
        FontToScreenCoordVector2(
            pen_position_26_6 + FontCoordVector2(m_glyph_specification[glyph_index].m_bearing_26_6[Dim::X], 0)));
    ScreenCoordRect glyph_vertex_coordinates(m_glyph_specification[glyph_index].m_size);
    glyph_vertex_coordinates += pen_position;
    glyph_vertex_coordinates +=
        ScreenCoordVector2(
            0,
            m_baseline_height +
            FontToScreenCoord(m_glyph_specification[glyph_index].m_bearing_26_6[Dim::Y]) -
            m_glyph_specification[glyph_index].m_size[Dim::Y] -
            PixelHeight());

    // the reason the texture coordinates look backwards (bottom on top)
    // is because the texture coordinates use a left-handed coordinate
    // system.
    {
/*
        ScreenCoordVector2 glyph_texture_coordinate_array[4] =
        {
            ScreenCoordVector2(glyph_texture_coordinates.TopLeft().m),
            ScreenCoordVector2(glyph_texture_coordinates.TopRight().m),
            ScreenCoordVector2(glyph_texture_coordinates.BottomLeft().m),
            ScreenCoordVector2(glyph_texture_coordinates.BottomRight().m)
        };
        ScreenCoordVector2 glyph_vertex_coordinate_array[4] =
        {
            ScreenCoordVector2(glyph_vertex_coordinates.BottomLeft().m),
            ScreenCoordVector2(glyph_vertex_coordinates.BottomRight().m),
            ScreenCoordVector2(glyph_vertex_coordinates.TopLeft().m),
            ScreenCoordVector2(glyph_vertex_coordinates.TopRight().m)
        };
*/
        Sint16 glyph_texture_coordinate_array[8] =
        {
            glyph_texture_coordinates.TopLeft()[Dim::X], glyph_texture_coordinates.TopLeft()[Dim::Y],
            glyph_texture_coordinates.TopRight()[Dim::X], glyph_texture_coordinates.TopRight()[Dim::Y],
            glyph_texture_coordinates.BottomLeft()[Dim::X], glyph_texture_coordinates.BottomLeft()[Dim::Y],
            glyph_texture_coordinates.BottomRight()[Dim::X], glyph_texture_coordinates.BottomRight()[Dim::Y]
        };
        Sint16 glyph_vertex_coordinate_array[8] =
        {
            glyph_vertex_coordinates.BottomLeft()[Dim::X], glyph_vertex_coordinates.BottomLeft()[Dim::Y],
            glyph_vertex_coordinates.BottomRight()[Dim::X], glyph_vertex_coordinates.BottomRight()[Dim::Y],
            glyph_vertex_coordinates.TopLeft()[Dim::X], glyph_vertex_coordinates.TopLeft()[Dim::Y],
            glyph_vertex_coordinates.TopRight()[Dim::X], glyph_vertex_coordinates.TopRight()[Dim::Y]
        };

        glVertexPointer(2, GL_SHORT, 0, glyph_vertex_coordinate_array);
//         glVertexPointer(2, GL_INT, 0, glyph_vertex_coordinate_array);

        glClientActiveTexture(GL_TEXTURE0);
        glTexCoordPointer(2, GL_SHORT, 0, glyph_texture_coordinate_array);
//         glTexCoordPointer(2, GL_INT, 0, glyph_texture_coordinate_array);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}

Uint32 AsciiFont::Hash () const
{
    // a 32-bit int is essentially Z/(2^32)Z (integer arithmetic modulo 2^32),
    // and since 2 and 11 are coprime, 2^32 and 11 are coprime, so 11 has
    // an inverse mod 2^32, which is given by 11^(phi(2^32)-1) mod 2^32 (phi
    // is the Euler totient function), which has a value of 3123612579.  thus
    // multiplying 11 by this value (using 32-bit unsigned ints) will give 1.
    //
    // this means that in 32-bit unsigned integer math, multiplication by
    // 11 (or actually any odd value) is an invertible operation.
    //
    // the reason for mentioning all this is that for a hashing function,
    // it is useful to have an operation which does not lose information (i.e.
    // an invertible operation).  multiplication by 11 and bitwise xor are
    // such operations.

    Uint32 retval = 0;
    retval ^= Uint32(PixelHeight());
    retval *= 11;
    retval ^= Uint32(m_has_kerning ? 1 : 0);
    retval *= 11;
    retval ^= Uint32(m_baseline_height);
    retval *= 11;
    for (Uint32 i = 0; i < ms_rendered_glyph_count; ++i)
    {
        retval ^= Uint32(m_glyph_specification[i].m_ascii);
        retval *= 11;
        retval ^= Uint32(m_glyph_specification[i].m_size[Dim::X]);
        retval *= 11;
        retval ^= Uint32(m_glyph_specification[i].m_size[Dim::Y]);
        retval *= 11;
        retval ^= Uint32(m_glyph_specification[i].m_bearing_26_6[Dim::X]);
        retval *= 11;
        retval ^= Uint32(m_glyph_specification[i].m_bearing_26_6[Dim::Y]);
        retval *= 11;
        retval ^= Uint32(m_glyph_specification[i].m_advance_26_6);
        retval *= 11;
        retval ^= Uint32(m_glyph_specification[i].m_texture_coordinates[Dim::X]);
        retval *= 11;
        retval ^= Uint32(m_glyph_specification[i].m_texture_coordinates[Dim::Y]);
        retval *= 11;
    }
    for (Uint32 i = 0; i < ms_rendered_glyph_count*ms_rendered_glyph_count; ++i)
    {
        retval ^= Uint32(m_kern_pair_26_6[i]);
        retval *= 11;
    }
    return retval;
}

FontCoord AsciiFont::KernPair_26_6 (char left, char right) const
{
    Uint32 glyph_index_left = GlyphIndex(left);
    Uint32 glyph_index_right = GlyphIndex(right);
    ASSERT1(glyph_index_left < ms_rendered_glyph_count);
    ASSERT1(glyph_index_right < ms_rendered_glyph_count);
    return m_kern_pair_26_6[glyph_index_left*ms_rendered_glyph_count + glyph_index_right];
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

FontCoord AsciiFont::TokenWidth_26_6 (char const *const string) const
{
    ASSERT1(string != NULL);

    char const *current_glyph = string;
    char const *next_glyph;
    char const *const end_glyph = StartOfNextToken(current_glyph);
    FontCoordVector2 pen_position_26_6(FontCoordVector2::ms_zero);
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

