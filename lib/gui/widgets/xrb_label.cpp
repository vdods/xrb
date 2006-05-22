// ///////////////////////////////////////////////////////////////////////////
// xrb_label.cpp by Victor Dods, created 2004/09/12
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_label.h"

#include <stdarg.h>

#include "xrb_gl.h"
#include "xrb_render.h"
#include "xrb_screen.h"

namespace Xrb
{

Label::Label (
    std::string const &text,
    Widget *const parent,
    std::string const &name)
    :
    Widget(parent, name),
    m_receiver_set_text(&Label::SetText, this)
{
    // do the common initialization
    Initialize();

    // ctor-specific initialization
    m_text = text;
    Label::UpdateRenderTextColor();
    Label::UpdateRenderFont();

    ASSERT1(!GetPicture().GetIsValid())
    ASSERT1(!GetRenderPicture().GetIsValid())
}

Label::Label (
    Resource<GLTexture> const &picture,
    Widget *const parent,
    std::string const &name)
    :
    Widget(parent, name),
    m_receiver_set_text(&Label::SetText, this)
{
    // do the common initialization
    Initialize();

    // m_picture may be invalid (for CheckBox)
    m_picture = picture;
    Label::UpdateRenderPicture();

    ASSERT1(!GetRenderFont().GetIsValid())
}

Label::~Label ()
{
}

void Label::SetIsMinWidthFixedToTextWidth (
    bool const is_min_width_fixed_to_text_width)
{
    if (m_is_min_width_fixed_to_text_width != is_min_width_fixed_to_text_width)
    {
        if (m_is_min_width_fixed_to_text_width)
            ASSERT1(!GetWordWrap())
        m_is_min_width_fixed_to_text_width = is_min_width_fixed_to_text_width;
        UpdateMinWidth();
    }
}

void Label::SetIsMaxWidthFixedToTextWidth (
    bool const is_max_width_fixed_to_text_width)
{
    if (m_is_max_width_fixed_to_text_width != is_max_width_fixed_to_text_width)
    {
        if (m_is_max_width_fixed_to_text_width)
            ASSERT1(!GetWordWrap())
        m_is_max_width_fixed_to_text_width = is_max_width_fixed_to_text_width;
        UpdateMaxWidth();
    }
}

void Label::SetIsWidthFixedToTextWidth (
    bool const is_width_fixed_to_text_width)
{
    SetIsMinWidthFixedToTextWidth(is_width_fixed_to_text_width);
    SetIsMaxWidthFixedToTextWidth(is_width_fixed_to_text_width);
}

void Label::SetIsMinHeightFixedToTextHeight (
    bool const is_min_height_fixed_to_text_height)
{
    if (m_is_min_height_fixed_to_text_height != is_min_height_fixed_to_text_height)
    {
        if (m_is_min_height_fixed_to_text_height)
            ASSERT1(!GetWordWrap())
        m_is_min_height_fixed_to_text_height = is_min_height_fixed_to_text_height;
        UpdateMinHeight();
    }
}

void Label::SetIsMaxHeightFixedToTextHeight (
    bool const is_max_height_fixed_to_text_height)
{
    if (m_is_max_height_fixed_to_text_height != is_max_height_fixed_to_text_height)
    {
        if (m_is_max_height_fixed_to_text_height)
            ASSERT1(!GetWordWrap())
        m_is_max_height_fixed_to_text_height = is_max_height_fixed_to_text_height;
        UpdateMaxHeight();
    }
}

void Label::SetIsHeightFixedToTextHeight (
    bool const is_height_fixed_to_text_height)
{
    SetIsMinHeightFixedToTextHeight(is_height_fixed_to_text_height);
    SetIsMaxHeightFixedToTextHeight(is_height_fixed_to_text_height);
}

void Label::SetIsSizeFixedToTextSize (
    bool const is_size_fixed_to_text_size)
{
    SetIsWidthFixedToTextWidth(is_size_fixed_to_text_size);
    SetIsHeightFixedToTextHeight(is_size_fixed_to_text_size);
}

void Label::SetText (std::string const &text)
{
    m_text = text;
    DirtyTextFormatting();
    UpdateMinWidth();
    UpdateMaxWidth();
    UpdateMinHeight();
    UpdateMaxHeight();
}

void Label::SetTextColor (Color const &color)
{
    m_text_color = color;
    UpdateRenderTextColor();
}

void Label::SetAlignment (Alignment2 const &alignment)
{
    m_alignment = alignment;
}

void Label::SetAlignment (Uint32 const component, Alignment const alignment)
{
    ASSERT1(component <= 1)
    m_alignment[component] = alignment;
}

void Label::SetWordWrap (bool const word_wrap)
{
    if (m_word_wrap != word_wrap)
    {
        if (word_wrap)
        {
            SetIsMinWidthFixedToTextWidth(false);
            SetIsMaxWidthFixedToTextWidth(false);
            SetIsMinHeightFixedToTextHeight(false);
            SetIsMaxHeightFixedToTextHeight(false);
        }
        m_word_wrap = word_wrap;
        DirtyTextFormatting();
    }
}

void Label::SetIndent (bool const indent)
{
    if (m_indent != indent)
    {
        m_indent = indent;
        DirtyTextFormatting();
    }
}

void Label::SetTabSize (Uint32 const tab_size)
{
    if (m_tab_size != tab_size)
    {
        m_tab_size = tab_size;
        DirtyTextFormatting();
    }
}

void Label::SetFont (Resource<Font> const &font)
{
    ASSERT1(font.GetIsValid())
    ASSERT1(!GetRenderPicture().GetIsValid())

    m_font = font;

    HandleChangedLabelFont();
}

void Label::SetFontFaceFilename (std::string const &font_face_filename)
{
    ASSERT1(!font_face_filename.empty())
    ASSERT1(!GetRenderPicture().GetIsValid())

    m_font =
        Singletons::ResourceLibrary()->LoadFilename<Font>(
            Font::Create,
            font_face_filename,
            m_font->GetPixelHeight());
    ASSERT1(m_font.GetIsValid())

    HandleChangedLabelFont();
}

void Label::SetFontHeightRatio (Float const font_height_ratio)
{
    ASSERT1(font_height_ratio > 0.0f)
    ASSERT1(!GetRenderPicture().GetIsValid())

    ScreenCoord font_height =
        GetWidgetSkin()->GetScreenCoordFromRatio(font_height_ratio);
    m_font =
        Singletons::ResourceLibrary()->LoadFilename<Font>(
            Font::Create,
            m_font.GetFilename(),
            font_height);
    ASSERT1(m_font.GetIsValid())

    HandleChangedLabelFont();
}

void Label::SetFontHeight (ScreenCoord const font_height)
{
    ASSERT1(font_height > 0)
    ASSERT1(!GetRenderPicture().GetIsValid())

    m_font =
        Singletons::ResourceLibrary()->LoadFilename<Font>(
            Font::Create,
            m_font.GetFilename(),
            font_height);
    ASSERT1(m_font.GetIsValid())

    HandleChangedLabelFont();
}

void Label::SetPicture (std::string const &picture_name)
{
    ASSERT1(!GetRenderFont().GetIsValid())

    ASSERT1(!picture_name.empty())

    m_picture =
        Singletons::ResourceLibrary()->
            LoadFilename<GLTexture>(GLTexture::Create, picture_name);
    ASSERT1(m_picture.GetIsValid())

    UpdateRenderPicture();
}

void Label::SetPicture (Resource<GLTexture> const &picture)
{
    ASSERT1(!GetRenderFont().GetIsValid())

    m_picture = picture;

    UpdateRenderPicture();
}

void Label::SetContentMargins (ScreenCoordVector2 const &content_margins)
{
    m_content_margins.SetComponents(
        Max(content_margins[Dim::X], -GetFrameMargins()[Dim::X]),
        Max(content_margins[Dim::Y], -GetFrameMargins()[Dim::Y]));
    HandleChangedContentMargins();
}

void Label::SetContentMarginRatios (FloatVector2 const &content_margin_ratios)
{
    ScreenCoord size_ratio_basis = GetTopLevelParent()->GetSizeRatioBasis();
    ScreenCoordVector2 calculated_content_margins =
        (static_cast<Float>(size_ratio_basis) *
         content_margin_ratios).StaticCast<ScreenCoord>();
    SetContentMargins(calculated_content_margins);
}

void Label::Draw (RenderContext const &render_context) const
{
    // this handles drawing of the background
    Widget::Draw(render_context);

    // if a picture is specified, draw the picture, otherwise it must
    // be a text label, so draw text.
    if (GetRenderPicture().GetIsValid())
        DrawPicture(render_context);
    else
        DrawText(render_context);
}

ScreenCoordVector2 Label::Resize (ScreenCoordVector2 const &size)
{
    if (GetSize()[Dim::X] != size[Dim::X])
        DirtyTextFormatting();
    return Widget::Resize(size);
}

void Label::HandleChangedLabelFont ()
{
    UpdateRenderFont();
    DirtyTextFormatting();
    UpdateMinWidth();
    UpdateMaxWidth();
    UpdateMinHeight();
    UpdateMaxHeight();
}

void Label::HandleChangedFrameMargins ()
{
    Widget::HandleChangedFrameMargins();
    if (!GetRenderPicture().GetIsValid())
    {
        DirtyTextFormatting();
        UpdateMinWidth();
        UpdateMaxWidth();
        UpdateMinHeight();
        UpdateMaxHeight();
    }
}

void Label::HandleChangedContentMargins ()
{
    if (!GetRenderPicture().GetIsValid())
    {
        DirtyTextFormatting();
        UpdateMinWidth();
        UpdateMaxWidth();
        UpdateMinHeight();
        UpdateMaxHeight();
    }
}

void Label::DrawText (RenderContext const &render_context) const
{
    if (!GetRenderFont().GetIsValid())
        return;

    ScreenCoordRect string_rect(GetContentsRect());
    if (string_rect.GetIsValid())
    {
        // give it a chance to update the formatted text if
        // m_text_formatting_update_required is set
        UpdateCachedFormattedText();
        // generate a render context for the string drawing function
        RenderContext string_render_context(render_context);
        // calculate the clip rect
        string_render_context.ApplyClipRect(string_rect);
        // calculate the color mask
        string_render_context.ApplyColorMask(GetRenderTextColor());
        // set up the GL clip rect
        string_render_context.SetupGLClipRect();
        // draw using the appropriate text buffer
        if (m_word_wrap)
            DrawTextInternal(string_render_context, m_cached_formatted_text.c_str());
        else
            DrawTextInternal(string_render_context, m_text.c_str());
    }
}

void Label::DrawPicture (RenderContext const &render_context) const
{
    if (!GetRenderPicture().GetIsValid())
        return;

    // this is the rectangle which will be used to render the picture.
    ScreenCoordRect picture_rect;
    ScreenCoordRect contents_rect(GetContentsRect());

    if (m_picture_keeps_aspect_ratio)
    {
        ASSERT1(m_picture->GetWidth() > 0)
        ASSERT1(m_picture->GetHeight() > 0)
        // determine if the picture should be fit to the width
        // of the label or the height of the label.
        if (contents_rect.GetHeight() * m_picture->GetWidth() /
            m_picture->GetHeight()
            <=
            contents_rect.GetWidth())
        {
            // the picture must fit to the label's height
            ScreenCoordVector2 picture_size(
                contents_rect.GetHeight() * m_picture->GetWidth() /
                m_picture->GetHeight(),
                contents_rect.GetHeight());
            picture_rect = picture_size;
            picture_rect +=
                ScreenCoordVector2(
                    (contents_rect.GetWidth() - picture_rect.GetWidth()) / 2,
                    0);
        }
        else
        {
            // the picture must fit to the label's width
            ScreenCoordVector2 picture_size(
                contents_rect.GetWidth(),
                contents_rect.GetWidth() * m_picture->GetHeight() /
                m_picture->GetWidth());
            picture_rect = picture_size;
            picture_rect +=
                ScreenCoordVector2(
                    0,
                    (contents_rect.GetHeight() - picture_rect.GetHeight()) / 2);
        }
        picture_rect += contents_rect.GetBottomLeft();
    }
    else
    {
        picture_rect = contents_rect;
    }

    // do the actual rendering
    Render::DrawScreenRectTexture(
        render_context,
        *GetRenderPicture(),
        picture_rect);
}

void Label::SetRenderFont (Resource<Font> const &render_font)
{
    ASSERT1(!GetPicture().GetIsValid())
    if (m_render_font != render_font)
    {
        m_render_font = render_font;
        DirtyTextFormatting();
        UpdateMinWidth();
        UpdateMaxWidth();
        UpdateMinHeight();
        UpdateMaxHeight();
    }
}

void Label::UpdateRenderTextColor ()
{
    SetRenderTextColor(GetTextColor());
}

void Label::UpdateRenderFont ()
{
    SetRenderFont(GetFont());
}

void Label::UpdateRenderPicture ()
{
    SetRenderPicture(GetPicture());
}

void Label::UpdateMinWidth ()
{
    ScreenCoord width = GetRenderFont()->GetStringWidth(GetText().c_str());

    if (GetRenderFont().GetIsValid())
        width += 2 * (GetFrameMargins()[Dim::X] + GetContentMargins()[Dim::X]);

    if (GetIsMinWidthFixedToTextWidth())
        SetSizeProperty(SizeProperties::MIN, Dim::X, width);
    SetSizePropertyEnabled(
        SizeProperties::MIN,
        Dim::X,
        GetIsMinWidthFixedToTextWidth());
}

void Label::UpdateMaxWidth ()
{
    ScreenCoord width = GetRenderFont()->GetStringWidth(GetText().c_str());

    if (GetRenderFont().GetIsValid())
        width += 2 * (GetFrameMargins()[Dim::X] + GetContentMargins()[Dim::X]);

    if (GetIsMaxWidthFixedToTextWidth())
        SetSizeProperty(SizeProperties::MAX, Dim::X, width);
    SetSizePropertyEnabled(
        SizeProperties::MAX,
        Dim::X,
        GetIsMaxWidthFixedToTextWidth());
}

void Label::UpdateMinHeight ()
{
    ScreenCoord height = GetRenderFont()->GetStringHeight(GetText().c_str());

    if (GetRenderFont().GetIsValid())
        height += 2 * (GetFrameMargins()[Dim::Y] + GetContentMargins()[Dim::Y]);

    if (GetIsMinHeightFixedToTextHeight())
        SetSizeProperty(SizeProperties::MIN, Dim::Y, height);
    SetSizePropertyEnabled(
        SizeProperties::MIN,
        Dim::Y,
        GetIsMinHeightFixedToTextHeight());
}

void Label::UpdateMaxHeight ()
{
    ScreenCoord height = GetRenderFont()->GetStringHeight(GetText().c_str());

    if (GetRenderFont().GetIsValid())
        height += 2 * (GetFrameMargins()[Dim::Y] + GetContentMargins()[Dim::Y]);

    if (GetIsMaxHeightFixedToTextHeight())
        SetSizeProperty(SizeProperties::MAX, Dim::Y, height);
    SetSizePropertyEnabled(
        SizeProperties::MAX,
        Dim::Y,
        GetIsMaxHeightFixedToTextHeight());
}

void Label::DrawTextInternal (
    RenderContext const &render_context,
    char const *string) const
{
    // set up the GL texture transform so we can use the integer
    // texture coordinates of each glyph directly.
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, GetRenderFont()->GetTextureHandle());

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(
        1.0f / GetRenderFont()->GetTextureSize()[Dim::X],
        1.0f / GetRenderFont()->GetTextureSize()[Dim::Y],
        1.0f);
    // set up the rendering color
    glColor4fv(render_context.GetColorMask().m);

    ScreenCoord string_height =
        m_line_format_vector.size() * GetRenderFont()->GetPixelHeight();
    ScreenCoordRect string_rect = GetContentsRect();
    ScreenCoordVector2 total_spacing = ScreenCoordVector2::ms_zero;
    // set up the initial position of the character drawing
    ScreenCoordVector2 initial_position(string_rect.GetTopLeft());
    initial_position[Dim::Y] -= GetRenderFont()->GetPixelHeight();
    initial_position += m_text_offset;
    ScreenCoordVector2 current_position(initial_position);

    switch (m_alignment[Dim::Y])
    {
        case TOP:
            // do nothing because current_position and spacing are
            // already set up correctly
            break;

        case CENTER:
            current_position[Dim::Y] =
                initial_position[Dim::Y] -
                (string_rect.GetHeight() - string_height) / 2;
            // total spacing is already set up correctly
            break;

        case BOTTOM:
            current_position[Dim::Y] =
                initial_position[Dim::Y] -
                (string_rect.GetHeight() - string_height);
            // total spacing is already set up correctly
            break;

        case SPACED:
            if (m_line_format_vector.size() == 1)
            {
                current_position[Dim::Y] =
                    initial_position[Dim::Y] -
                    (string_rect.GetHeight() - string_height) / 2;
                // total spacing is set up correctly
            }
            else if (m_word_wrap)
            {
                // current position is set up correctly
                total_spacing[Dim::Y] = string_rect.GetHeight() - string_height;
            }
            // range checking
            if (total_spacing[Dim::Y] < 0)
                total_spacing[Dim::Y] = 0;
            break;

        default:
            ASSERT0(false && "Invalid Alignment")
            break;
    }

    Uint32 line = 0;
    bool start_of_line = true;
    char previous_char = '\0';
    ScreenCoordRect glyph_texture_coordinates;
    ScreenCoordRect glyph_vertex_coordinates;
    Uint32 lines_left = m_line_format_vector.size() - 1;
    Uint32 chars_left;

    glBegin(GL_QUADS);
    while (*string != '\0')
    {
        if (start_of_line)
        {
            chars_left = m_line_format_vector[line].m_char_count - 1;
            // horizontal justification
            switch (m_alignment[Dim::X])
            {
                case LEFT:
                    current_position[Dim::X] = initial_position[Dim::X];
                    break;

                case CENTER:
                    current_position[Dim::X] =
                        initial_position[Dim::X] +
                        (string_rect.GetWidth() - m_line_format_vector[line].m_width) /
                        2;
                    break;

                case RIGHT:
                    current_position[Dim::X] =
                        initial_position[Dim::X] +
                        string_rect.GetWidth() -
                        m_line_format_vector[line].m_width;
                    break;

                case SPACED:
                    current_position[Dim::X] = initial_position[Dim::X];
                    if (m_word_wrap)
                        total_spacing[Dim::X] =
                            string_rect.GetWidth() -
                            m_line_format_vector[line].m_width;
                    // if the line isn't long enough, don't space it out
                    if (3 * total_spacing[Dim::X] > string_rect.GetWidth())
                        total_spacing[Dim::X] = 0;
                    // range checking
                    else if (total_spacing[Dim::X] < 0)
                        total_spacing[Dim::X] = 0;
                    break;

                default:
                    ASSERT0(false && "Invalid Alignment")
                    break;
            }
        }

        if (*string == '\n')
        {
            start_of_line = true;
            ++line;
            current_position[Dim::Y] -= GetRenderFont()->GetPixelHeight();
            if (lines_left > 0)
            {
                current_position[Dim::Y] -= total_spacing[Dim::Y] / lines_left;
                total_spacing[Dim::Y] -= total_spacing[Dim::Y] / lines_left;
            }
            --lines_left;
        }
        else if (*string == '\t')
        {
            current_position[Dim::X] +=
                GetTabSize() * GetRenderFont()->GetGlyphPixelAdvance(' ');
            if (chars_left > 0)
            {
                current_position[Dim::X] += total_spacing[Dim::X] / chars_left;
                total_spacing[Dim::X] -= total_spacing[Dim::X] / chars_left;
            }
        }
        else
        {
            // apply kerning offset (if not at the start of the line)
            if (!start_of_line)
                current_position[Dim::X] +=
                    GetRenderFont()->GetKerningPixelAdvance(previous_char, *string);

            // draw the current char
            glyph_texture_coordinates =
                GetRenderFont()->GetGlyphPixelSize(*string);
            glyph_texture_coordinates +=
                GetRenderFont()->GetGlyphTextureCoordinates(*string);

            glyph_vertex_coordinates =
                GetRenderFont()->GetGlyphPixelSize(*string);
            glyph_vertex_coordinates +=
                current_position +
                ScreenCoordVector2(
                    0,
                    GetRenderFont()->GetBaselineHeight() +
                    GetRenderFont()->GetGlyphBearing(*string)[Dim::Y] -
                    GetRenderFont()->GetGlyphPixelSize(*string)[Dim::Y]);

            glTexCoord2iv(glyph_texture_coordinates.GetBottomLeft().m);
            glVertex2iv(glyph_vertex_coordinates.GetTopLeft().m);

            glTexCoord2iv(glyph_texture_coordinates.GetTopLeft().m);
            glVertex2iv(glyph_vertex_coordinates.GetBottomLeft().m);

            glTexCoord2iv(glyph_texture_coordinates.GetTopRight().m);
            glVertex2iv(glyph_vertex_coordinates.GetBottomRight().m);

            glTexCoord2iv(glyph_texture_coordinates.GetBottomRight().m);
            glVertex2iv(glyph_vertex_coordinates.GetTopRight().m);

            current_position[Dim::X] +=
                GetRenderFont()->GetGlyphPixelAdvance(*string);
            if (chars_left > 0)
            {
                current_position[Dim::X] += total_spacing[Dim::X] / chars_left;
                total_spacing[Dim::X] -= total_spacing[Dim::X] / chars_left;
            }
            --chars_left;
        }

        start_of_line = false;
        previous_char = *string;
        ++string;
    }
    glEnd();

    // clear the texture matrix
    glLoadIdentity();
}

Label::TokenClass Label::GetTokenClass (char const c)
{
    switch (c)
    {
        case ' ':
        case '\t':
            return WHITESPACE;

        case '\n':
            return NEWLINE;

        case '\0':
            return NULLCHAR;

        default:
            return WORD;
    }
}

char const *Label::GetStartOfNextToken (char const *string)
{
    ASSERT1(string != NULL)

    Sint32 token_class = GetTokenClass(*string);

    if (token_class == NEWLINE)
        return ++string;
    else if (token_class == NULLCHAR)
        return string;

    while (*string != '\0' && GetTokenClass(*string) == token_class)
        ++string;

    return string;
}

ScreenCoord Label::GetTokenWidth (char const *string) const
{
    ASSERT1(string != NULL)

    char const *end = GetStartOfNextToken(string);
    ScreenCoord retval = 0;
    while (string < end)
    {
        if (*string == '\n')
            retval += 0;
        if (*string == '\t')
            retval += GetRenderFont()->GetGlyphPixelAdvance(' ') * GetTabSize();
        else
            retval += GetRenderFont()->GetGlyphPixelAdvance(*string);
        ++string;
    }

    return retval;
}

void Label::UpdateCachedFormattedText () const
{
    // if no render font is set, early out
    if (!GetRenderFont().GetIsValid())
        return;

    // if no update was required, early-out
    if (!m_text_formatting_update_required)
        return;

    // text formatting update no longer required
    m_text_formatting_update_required = false;

    // non-word-wrapped text uses m_text directly
    if (!m_word_wrap)
    {
        UpdateLineFormatVector(m_text.c_str());
        return;
    }

    // the width of the string rect
    ScreenCoord wrap_width = GetContentsRect().GetWidth();

    // parse the string into the buffer
    // clear the buffer
    m_cached_formatted_text.clear();

    bool forced_newline = true;
    bool line_start = true;
    char const *current_token;
    char const *next_token;
    ScreenCoord current_pos = 0;
    ScreenCoord token_width;
    TokenClass next_token_class;

    current_token = m_text.c_str();
    while (GetTokenClass(*current_token) != NULLCHAR)
    {
        if (line_start)
        {
            current_pos = 0;
            if (forced_newline && m_indent)
            {
                for (Uint32 i = 0; i < GetTabSize(); ++i)
                {
                    m_cached_formatted_text += ' ';
                    current_pos += GetTokenWidth(" ");
                }
            }
            while (GetTokenClass(*current_token) == WHITESPACE)
                current_token = GetStartOfNextToken(current_token);
            line_start = false;
        }

        switch (GetTokenClass(*current_token))
        {
            case WHITESPACE:
                // check if the token after this is a newline, a nullchar
                // or a word that won't fit:
                next_token = GetStartOfNextToken(current_token);
                next_token_class = GetTokenClass(*next_token);
                token_width = GetTokenWidth(" ") + GetTokenWidth(next_token);
                if (next_token_class == NULLCHAR)
                {
                }
                else if (next_token_class == NEWLINE)
                {
                }
                else if (next_token_class == WORD &&
                         current_pos + token_width > wrap_width)
                {
                    m_cached_formatted_text += '\n';
                    line_start = true;
                }
                else
                {
                    // only if all of the above criteria fail may
                    // we add in a space
                    m_cached_formatted_text += ' ';
                    current_pos += GetTokenWidth(" ");
                }
                current_token = next_token;
                break;

            case NEWLINE:
                m_cached_formatted_text += '\n';
                forced_newline = true;
                line_start = true;
                current_token = GetStartOfNextToken(current_token);
                break;

            case NULLCHAR:
                break;

            case WORD:
                token_width = GetTokenWidth(current_token);
                next_token = GetStartOfNextToken(current_token);
                forced_newline = false;
                if (token_width > wrap_width)
                {
                    for (Sint32 i = 0; i < next_token - current_token; ++i)
                        m_cached_formatted_text += *(current_token + i);
                    m_cached_formatted_text += '\n';
                    current_token = next_token;
                    line_start = true;
                }
                else if (current_pos + token_width > wrap_width)
                {
                    m_cached_formatted_text += '\n';
                    line_start = true;
                }
                else
                {
                    for (Sint32 i = 0; i < next_token - current_token; ++i)
                        m_cached_formatted_text += *(current_token + i);
                    current_pos += token_width;
                    current_token = next_token;
                }
                break;
        }
    }

    // create the line index
    UpdateLineFormatVector(m_cached_formatted_text.c_str());
}

void Label::UpdateLineFormatVector (char const *string) const
{
    ASSERT1(string != NULL)
    ASSERT1(GetRenderFont().GetIsValid())

    // clear the format vector
    m_line_format_vector.clear();

    bool line_start = true;
    LineFormat line_format;
    // iterate over the whole string
    while (*string != '\0')
    {
        // if this is a new line, re-init the format values and line_start
        if (line_start)
        {
            line_format.m_width = 0;
            line_format.m_ptr = string;
            line_format.m_char_count = 0;
            line_start = false;
        }

        // if it's a newline, signal line_start and push the current
        // line_format struct
        if (*string == '\n')
        {
            line_start = true;
            m_line_format_vector.push_back(line_format);
        }
        // otherwise add a tab width
        else if (*string == '\t')
        {
            line_format.m_width +=
                GetRenderFont()->GetGlyphPixelAdvance(' ') * GetTabSize();
            ++line_format.m_char_count;
        }
        // otherwise add a normal char width
        else
        {
            line_format.m_width += GetRenderFont()->GetGlyphPixelAdvance(*string);
            ++line_format.m_char_count;
        }

        // increment the string's pointer
        ++string;
    }
    // make sure to push the last one
    m_line_format_vector.push_back(line_format);
}

void Label::Initialize ()
{
    m_accepts_focus = false;
    m_accepts_mouseover = false;

    DirtyTextFormatting();
    ASSERT1(m_text.empty())
    m_text_offset = ScreenCoordVector2::ms_zero;
    m_is_min_width_fixed_to_text_width = true;
    m_is_max_width_fixed_to_text_width = false;
    m_is_min_height_fixed_to_text_height = true;
    m_is_max_height_fixed_to_text_height = false;
    m_text_color = Color(1.0, 1.0, 1.0, 1.0);
    m_alignment = Alignment2(CENTER, CENTER);
    m_word_wrap = false;
    m_indent = false;
    m_tab_size = 4;
    m_font = GetWidgetSkinFont(WidgetSkin::DEFAULT_FONT);
    m_picture_keeps_aspect_ratio = false;
    m_content_margins = GetWidgetSkinMargins(WidgetSkin::DEFAULT_CONTENT_MARGINS);
    ASSERT1(!m_picture.GetIsValid())
    ASSERT1(!m_render_picture.GetIsValid())
    SetRenderTextColor(m_text_color);

    Label::UpdateRenderBackground();
}

} // end of namespace Xrb
