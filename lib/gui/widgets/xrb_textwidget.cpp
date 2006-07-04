// ///////////////////////////////////////////////////////////////////////////
// xrb_textwidget.cpp by Victor Dods, created 2006/06/10
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_textwidget.h"

#include <stdarg.h>

#include "xrb_gl.h"
#include "xrb_render.h"
#include "xrb_screen.h"

namespace Xrb
{

TextWidget::TextWidget (
    std::string const &text,
    Widget *const parent,
    std::string const &name)
    :
    Widget(parent, name),
    m_text(text),
    m_receiver_set_text(&TextWidget::SetText, this)
{
    m_accepts_focus = false;
    m_accepts_mouseover = false;

    m_text_color = Color(1.0, 1.0, 1.0, 1.0);
    SetRenderTextColor(m_text_color);
    m_font = GetWidgetSkinFont(WidgetSkin::DEFAULT_FONT);
    m_is_min_width_fixed_to_text_width = false;
    m_is_max_width_fixed_to_text_width = false;
    m_is_min_height_fixed_to_text_height = false;
    m_is_max_height_fixed_to_text_height = false;

    TextWidget::UpdateRenderBackground();
    TextWidget::UpdateRenderTextColor();
    TextWidget::UpdateRenderFont();
}

void TextWidget::SetText (std::string const &text)
{
    m_text = text;
    UpdateMinAndMaxSizesFromText();
}

void TextWidget::SetTextColor (Color const &color)
{
    m_text_color = color;
    UpdateRenderTextColor();
}

void TextWidget::SetFont (Resource<Font> const &font)
{
    if (m_font != font)
    {
        m_font = font;
        HandleChangedFont();
    }
}

void TextWidget::SetFontFaceFilename (std::string const &font_face_filename)
{
    ASSERT1(!font_face_filename.empty())
    ASSERT1(m_font.GetIsValid())

    Resource<Font> font =
        Singletons::ResourceLibrary().LoadFilename<Font>(
            AsciiFont::Create,
            font_face_filename,
            m_font->GetPixelHeight());
    ASSERT1(font.GetIsValid())

    if (m_font != font)
    {
        m_font = font;
        HandleChangedFont();
    }
}

void TextWidget::SetFontHeightRatio (Float const font_height_ratio)
{
    ASSERT1(font_height_ratio > 0.0f)
    ASSERT1(m_font.GetIsValid())

    ScreenCoord font_height =
        GetWidgetSkin()->GetScreenCoordFromRatio(font_height_ratio);

    if (m_font->GetPixelHeight() != font_height)
    {
        m_font =
            Singletons::ResourceLibrary().LoadFilename<Font>(
                AsciiFont::Create,
                m_font.GetFilename(),
                font_height);
        ASSERT1(m_font.GetIsValid())
        HandleChangedFont();
    }
}

void TextWidget::SetFontHeight (ScreenCoord const font_height)
{
    ASSERT1(font_height > 0)
    ASSERT1(m_font.GetIsValid())

    if (m_font->GetPixelHeight() != font_height)
    {
        m_font =
            Singletons::ResourceLibrary().LoadFilename<Font>(
                AsciiFont::Create,
                m_font.GetFilename(),
                font_height);
        ASSERT1(m_font.GetIsValid())
        HandleChangedFont();
    }
}

void TextWidget::SetIsMinWidthFixedToTextWidth (
    bool const is_min_width_fixed_to_text_width)
{
    if (m_is_min_width_fixed_to_text_width != is_min_width_fixed_to_text_width)
    {
        m_is_min_width_fixed_to_text_width = is_min_width_fixed_to_text_width;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetIsMaxWidthFixedToTextWidth (
    bool const is_max_width_fixed_to_text_width)
{
    if (m_is_max_width_fixed_to_text_width != is_max_width_fixed_to_text_width)
    {
        m_is_max_width_fixed_to_text_width = is_max_width_fixed_to_text_width;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetIsWidthFixedToTextWidth (
    bool const is_width_fixed_to_text_width)
{
    if (m_is_min_width_fixed_to_text_width != is_width_fixed_to_text_width ||
        m_is_max_width_fixed_to_text_width != is_width_fixed_to_text_width)
    {
        m_is_min_width_fixed_to_text_width = is_width_fixed_to_text_width;
        m_is_max_width_fixed_to_text_width = is_width_fixed_to_text_width;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetIsMinHeightFixedToTextHeight (
    bool const is_min_height_fixed_to_text_height)
{
    if (m_is_min_height_fixed_to_text_height != is_min_height_fixed_to_text_height)
    {
        m_is_min_height_fixed_to_text_height = is_min_height_fixed_to_text_height;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetIsMaxHeightFixedToTextHeight (
    bool const is_max_height_fixed_to_text_height)
{
    if (m_is_max_height_fixed_to_text_height != is_max_height_fixed_to_text_height)
    {
        m_is_max_height_fixed_to_text_height = is_max_height_fixed_to_text_height;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetIsHeightFixedToTextHeight (
    bool const is_height_fixed_to_text_height)
{
    if (m_is_min_height_fixed_to_text_height != is_height_fixed_to_text_height ||
        m_is_max_height_fixed_to_text_height != is_height_fixed_to_text_height)
    {
        m_is_min_height_fixed_to_text_height = is_height_fixed_to_text_height;
        m_is_max_height_fixed_to_text_height = is_height_fixed_to_text_height;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetIsSizeFixedToTextSize (
    bool const is_size_fixed_to_text_size)
{
    if (m_is_min_width_fixed_to_text_width != is_size_fixed_to_text_size ||
        m_is_max_width_fixed_to_text_width != is_size_fixed_to_text_size ||
        m_is_min_height_fixed_to_text_height != is_size_fixed_to_text_size ||
        m_is_max_height_fixed_to_text_height != is_size_fixed_to_text_size)
    {
        m_is_min_width_fixed_to_text_width = is_size_fixed_to_text_size;
        m_is_max_width_fixed_to_text_width = is_size_fixed_to_text_size;
        m_is_min_height_fixed_to_text_height = is_size_fixed_to_text_size;
        m_is_max_height_fixed_to_text_height = is_size_fixed_to_text_size;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::HandleChangedFont ()
{
    UpdateRenderFont();
}

void TextWidget::HandleChangedFrameMargins ()
{
    Widget::HandleChangedFrameMargins();
    UpdateMinAndMaxSizesFromText();
}

void TextWidget::HandleChangedContentMargins ()
{
    Widget::HandleChangedContentMargins();
    UpdateMinAndMaxSizesFromText();
}

void TextWidget::SetRenderFont (Resource<Font> const &render_font)
{
    if (m_render_font != render_font)
    {
        m_render_font = render_font;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::UpdateRenderTextColor ()
{
    SetRenderTextColor(GetTextColor());
}

void TextWidget::UpdateRenderFont ()
{
    SetRenderFont(GetFont());
}

void TextWidget::UpdateMinAndMaxSizesFromText ()
{
    if (!GetRenderFont().GetIsValid())
        return;

    ScreenCoordRect string_rect(GetRenderFont()->GetStringRect(GetText().c_str()));
    ScreenCoord width =
        string_rect.GetWidth() +
        2 * (GetFrameMargins()[Dim::X] + GetContentMargins()[Dim::X]);
    ScreenCoord height =
        string_rect.GetHeight() +
        2 * (GetFrameMargins()[Dim::Y] + GetContentMargins()[Dim::Y]);

    // TODO: keep "preferred" min/max sizes, so that they can be reverted to
    // if/when the text-based min/max sizes no longer apply

    if (m_is_min_width_fixed_to_text_width)
        SetSizeProperty(SizeProperties::MIN, Dim::X, width);
    if (m_is_max_width_fixed_to_text_width)
        SetSizeProperty(SizeProperties::MAX, Dim::X, width);
    if (m_is_min_height_fixed_to_text_height)
        SetSizeProperty(SizeProperties::MIN, Dim::Y, height);
    if (m_is_max_height_fixed_to_text_height)
        SetSizeProperty(SizeProperties::MAX, Dim::Y, height);
    SetSizePropertyEnabled(
        SizeProperties::MIN,
        Bool2(m_is_min_width_fixed_to_text_width, m_is_min_height_fixed_to_text_height));
    SetSizePropertyEnabled(
        SizeProperties::MAX,
        Bool2(m_is_max_width_fixed_to_text_width, m_is_max_height_fixed_to_text_height));
}

} // end of namespace Xrb