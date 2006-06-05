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
//         UpdateMinWidth();
        UpdateMinAndMaxSizes();
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
//         UpdateMaxWidth();
        UpdateMinAndMaxSizes();
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
//         UpdateMinHeight();
        UpdateMinAndMaxSizes();
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
//         UpdateMaxHeight();
        UpdateMinAndMaxSizes();
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
//     UpdateMinWidth();
//     UpdateMaxWidth();
//     UpdateMinHeight();
//     UpdateMaxHeight();
    UpdateMinAndMaxSizes();
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
            SetIsSizeFixedToTextSize(false);
//             SetIsMinWidthFixedToTextWidth(false);
//             SetIsMaxWidthFixedToTextWidth(false);
//             SetIsMinHeightFixedToTextHeight(false);
//             SetIsMaxHeightFixedToTextHeight(false);
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
        Singletons::ResourceLibrary().LoadFilename<Font>(
            AsciiFont::Create,
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
        Singletons::ResourceLibrary().LoadFilename<Font>(
            AsciiFont::Create,
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
        Singletons::ResourceLibrary().LoadFilename<Font>(
            AsciiFont::Create,
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
        Singletons::ResourceLibrary().
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
//     UpdateMinWidth();
//     UpdateMaxWidth();
//     UpdateMinHeight();
//     UpdateMaxHeight();
    UpdateMinAndMaxSizes();
}

void Label::HandleChangedFrameMargins ()
{
    Widget::HandleChangedFrameMargins();
    if (!GetRenderPicture().GetIsValid())
    {
        DirtyTextFormatting();
//         UpdateMinWidth();
//         UpdateMaxWidth();
//         UpdateMinHeight();
//         UpdateMaxHeight();
        UpdateMinAndMaxSizes();
    }
}

void Label::HandleChangedContentMargins ()
{
    if (!GetRenderPicture().GetIsValid())
    {
        DirtyTextFormatting();
//         UpdateMinWidth();
//         UpdateMaxWidth();
//         UpdateMinHeight();
//         UpdateMaxHeight();
        UpdateMinAndMaxSizes();
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
        ASSERT1(!m_text_formatting_update_required)
        // generate a render context for the string drawing function
        RenderContext string_render_context(render_context);
        // calculate the clip rect
        string_render_context.ApplyClipRect(string_rect);
        // calculate the color mask
        string_render_context.ApplyColorMask(GetRenderTextColor());
        // set up the GL clip rect
        string_render_context.SetupGLClipRect();
        // draw the text
        ASSERT1(m_line_format_vector_source != NULL)
        GetRenderFont()->DrawLineFormattedText(
            string_render_context,
            GetContentsRect() + m_text_offset,
            m_line_format_vector_source->c_str(),
            m_line_format_vector,
            m_alignment);
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
//         UpdateMinWidth();
//         UpdateMaxWidth();
//         UpdateMinHeight();
//         UpdateMaxHeight();
        UpdateMinAndMaxSizes();
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

void Label::UpdateMinAndMaxSizes ()
{
    ASSERT1(GetRenderFont().GetIsValid())

    ScreenCoordRect string_rect(
        GetRenderFont()->GetStringRect(
            GetText().c_str(),
            ScreenCoordVector2::ms_zero));
    ScreenCoord width =
        string_rect.GetWidth() +
        2 * (GetFrameMargins()[Dim::X] + GetContentMargins()[Dim::X]);
    ScreenCoord height =
        string_rect.GetHeight() +
        2 * (GetFrameMargins()[Dim::Y] + GetContentMargins()[Dim::Y]);

    if (GetIsMinWidthFixedToTextWidth())
        SetSizeProperty(SizeProperties::MIN, Dim::X, width);
    if (GetIsMaxWidthFixedToTextWidth())
        SetSizeProperty(SizeProperties::MAX, Dim::X, width);
    if (GetIsMinHeightFixedToTextHeight())
        SetSizeProperty(SizeProperties::MIN, Dim::Y, height);
    if (GetIsMaxHeightFixedToTextHeight())
        SetSizeProperty(SizeProperties::MAX, Dim::Y, height);
    SetSizePropertyEnabled(
        SizeProperties::MIN,
        Bool2(GetIsMinWidthFixedToTextWidth(), GetIsMinHeightFixedToTextHeight()));
    SetSizePropertyEnabled(
        SizeProperties::MAX,
        Bool2(GetIsMaxWidthFixedToTextWidth(), GetIsMaxHeightFixedToTextHeight()));
}
/*
void Label::UpdateMinWidth ()
{
    ASSERT1(GetRenderFont().GetIsValid())

    ScreenCoordRect string_rect(
        GetRenderFont()->GetStringRect(
            GetText().c_str(),
            ScreenCoordVector2::ms_zero));
    ScreenCoord width =
        string_rect.GetWidth() +
        2 * (GetFrameMargins()[Dim::X] + GetContentMargins()[Dim::X]);

    if (GetIsMinWidthFixedToTextWidth())
        SetSizeProperty(SizeProperties::MIN, Dim::X, width);
    SetSizePropertyEnabled(
        SizeProperties::MIN,
        Dim::X,
        GetIsMinWidthFixedToTextWidth());
}

void Label::UpdateMaxWidth ()
{
    ASSERT1(GetRenderFont().GetIsValid())

    ScreenCoordRect string_rect(
        GetRenderFont()->GetStringRect(
            GetText().c_str(),
            ScreenCoordVector2::ms_zero));
    ScreenCoord width =
        string_rect.GetWidth() +
        2 * (GetFrameMargins()[Dim::X] + GetContentMargins()[Dim::X]);

    if (GetIsMaxWidthFixedToTextWidth())
        SetSizeProperty(SizeProperties::MAX, Dim::X, width);
    SetSizePropertyEnabled(
        SizeProperties::MAX,
        Dim::X,
        GetIsMaxWidthFixedToTextWidth());
}

void Label::UpdateMinHeight ()
{
    ASSERT1(GetRenderFont().GetIsValid())

    ScreenCoordRect string_rect(
        GetRenderFont()->GetStringRect(
            GetText().c_str(),
            ScreenCoordVector2::ms_zero));
    ScreenCoord height =
        string_rect.GetHeight() +
        2 * (GetFrameMargins()[Dim::Y] + GetContentMargins()[Dim::Y]);

    if (GetIsMinHeightFixedToTextHeight())
        SetSizeProperty(SizeProperties::MIN, Dim::Y, height);
    SetSizePropertyEnabled(
        SizeProperties::MIN,
        Dim::Y,
        GetIsMinHeightFixedToTextHeight());
}

void Label::UpdateMaxHeight ()
{
    ASSERT1(GetRenderFont().GetIsValid())

    ScreenCoordRect string_rect(
        GetRenderFont()->GetStringRect(
            GetText().c_str(),
            ScreenCoordVector2::ms_zero));
    ScreenCoord height =
        string_rect.GetHeight() +
        2 * (GetFrameMargins()[Dim::Y] + GetContentMargins()[Dim::Y]);

    if (GetIsMaxHeightFixedToTextHeight())
        SetSizeProperty(SizeProperties::MAX, Dim::Y, height);
    SetSizePropertyEnabled(
        SizeProperties::MAX,
        Dim::Y,
        GetIsMaxHeightFixedToTextHeight());
}
*/
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

    // word-wrapped text munges m_text
    if (m_word_wrap)
    {
        // generate the word-wrapped string
        GetRenderFont()->GenerateWordWrappedString(
            m_text,
            &m_cached_formatted_text,
            GetContentsRect().GetSize());
        m_line_format_vector_source = &m_cached_formatted_text;
    }
    // otherwise use m_text directly
    else
    {
        m_line_format_vector_source = &m_text;
    }

    // create the line format vector from whatever text was chosen above
    GetRenderFont()->GenerateLineFormatVector(m_line_format_vector_source->c_str(), &m_line_format_vector);
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
    m_line_format_vector_source = NULL;
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
