// ///////////////////////////////////////////////////////////////////////////
// xrb_textwidget.cpp by Victor Dods, created 2006/06/10
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_textwidget.hpp"

#include <stdarg.h>

#include "xrb_gl.hpp"
#include "xrb_render.hpp"
#include "xrb_screen.hpp"

namespace Xrb
{

TextWidget::TextWidget (
    std::string const &text,
    ContainerWidget *const parent,
    std::string const &name)
    :
    Widget(parent, name),
    m_text(text),
    m_receiver_set_text(&TextWidget::SetText, this),
    m_receiver_set_text_v(&TextWidget::SetTextV, this)
{
    m_accepts_focus = false;
    m_accepts_mouseover = false;

    m_text_color = Color(1.0, 1.0, 1.0, 1.0);
    SetRenderTextColor(m_text_color);
    m_font = WidgetSkinFont(WidgetSkin::DEFAULT_FONT);
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
    ASSERT1(!font_face_filename.empty());
    ASSERT1(m_font.IsValid());

    Resource<Font> font =
        Singleton::ResourceLibrary().LoadFilename<Font>(
            AsciiFont::Create,
            font_face_filename,
            m_font->PixelHeight());
    ASSERT1(font.IsValid());

    if (m_font != font)
    {
        m_font = font;
        HandleChangedFont();
    }
}

void TextWidget::SetFontHeightRatio (Float const font_height_ratio)
{
    ASSERT1(font_height_ratio > 0.0f);
    ASSERT1(m_font.IsValid());

    ScreenCoord font_height =
        GetWidgetSkin()->ScreenCoordFromRatio(font_height_ratio);

    if (m_font->PixelHeight() != font_height)
    {
        m_font =
            Singleton::ResourceLibrary().LoadFilename<Font>(
                AsciiFont::Create,
                m_font.Filename(),
                font_height);
        ASSERT1(m_font.IsValid());
        HandleChangedFont();
    }
}

void TextWidget::SetFontHeight (ScreenCoord const font_height)
{
    ASSERT1(font_height > 0);
    ASSERT1(m_font.IsValid());

    if (m_font->PixelHeight() != font_height)
    {
        m_font =
            Singleton::ResourceLibrary().LoadFilename<Font>(
                AsciiFont::Create,
                m_font.Filename(),
                font_height);
        ASSERT1(m_font.IsValid());
        HandleChangedFont();
    }
}

void TextWidget::SetIsMinWidthFixedToTextWidth (bool const is_min_width_fixed_to_text_width)
{
    if (m_is_min_width_fixed_to_text_width != is_min_width_fixed_to_text_width)
    {
        m_is_min_width_fixed_to_text_width = is_min_width_fixed_to_text_width;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetIsMaxWidthFixedToTextWidth (bool const is_max_width_fixed_to_text_width)
{
    if (m_is_max_width_fixed_to_text_width != is_max_width_fixed_to_text_width)
    {
        m_is_max_width_fixed_to_text_width = is_max_width_fixed_to_text_width;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetIsWidthFixedToTextWidth (bool const is_width_fixed_to_text_width)
{
    if (m_is_min_width_fixed_to_text_width != is_width_fixed_to_text_width ||
        m_is_max_width_fixed_to_text_width != is_width_fixed_to_text_width)
    {
        m_is_min_width_fixed_to_text_width = is_width_fixed_to_text_width;
        m_is_max_width_fixed_to_text_width = is_width_fixed_to_text_width;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetIsMinHeightFixedToTextHeight (bool const is_min_height_fixed_to_text_height)
{
    if (m_is_min_height_fixed_to_text_height != is_min_height_fixed_to_text_height)
    {
        m_is_min_height_fixed_to_text_height = is_min_height_fixed_to_text_height;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetIsMaxHeightFixedToTextHeight (bool const is_max_height_fixed_to_text_height)
{
    if (m_is_max_height_fixed_to_text_height != is_max_height_fixed_to_text_height)
    {
        m_is_max_height_fixed_to_text_height = is_max_height_fixed_to_text_height;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetIsHeightFixedToTextHeight (bool const is_height_fixed_to_text_height)
{
    if (m_is_min_height_fixed_to_text_height != is_height_fixed_to_text_height ||
        m_is_max_height_fixed_to_text_height != is_height_fixed_to_text_height)
    {
        m_is_min_height_fixed_to_text_height = is_height_fixed_to_text_height;
        m_is_max_height_fixed_to_text_height = is_height_fixed_to_text_height;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetIsMinSizeFixedToTextSize (bool const is_min_size_fixed_to_text_size)
{
    if (m_is_min_width_fixed_to_text_width != is_min_size_fixed_to_text_size ||
        m_is_min_height_fixed_to_text_height != is_min_size_fixed_to_text_size)
    {
        m_is_min_width_fixed_to_text_width = is_min_size_fixed_to_text_size;
        m_is_min_height_fixed_to_text_height = is_min_size_fixed_to_text_size;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetIsMaxSizeFixedToTextSize (bool const is_max_size_fixed_to_text_size)
{
    if (m_is_max_width_fixed_to_text_width != is_max_size_fixed_to_text_size ||
        m_is_max_height_fixed_to_text_height != is_max_size_fixed_to_text_size)
    {
        m_is_max_width_fixed_to_text_width = is_max_size_fixed_to_text_size;
        m_is_max_height_fixed_to_text_height = is_max_size_fixed_to_text_size;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetIsSizeFixedToTextSize (bool const is_size_fixed_to_text_size)
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

void TextWidget::SetRenderFont (Resource<Font> const &render_font)
{
    if (m_render_font != render_font)
    {
        m_render_font = render_font;
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

void TextWidget::UpdateRenderTextColor ()
{
    SetRenderTextColor(TextColor());
}

void TextWidget::UpdateRenderFont ()
{
    SetRenderFont(GetFont());
}

ScreenCoordRect TextWidget::TextRect () const
{
    return RenderFont()->StringRect(Text().c_str());
}

void TextWidget::UpdateMinAndMaxSizesFromText ()
{
    if (!RenderFont().IsValid())
        return;

    ScreenCoordVector2 size(TextRect().Size() + 2 * (FrameMargins() + ContentMargins()));

    if (m_is_min_width_fixed_to_text_width)
        SetSizeProperty(SizeProperties::MIN, Dim::X, size[Dim::X]);
    if (m_is_max_width_fixed_to_text_width)
        SetSizeProperty(SizeProperties::MAX, Dim::X, size[Dim::X]);
    if (m_is_min_height_fixed_to_text_height)
        SetSizeProperty(SizeProperties::MIN, Dim::Y, size[Dim::Y]);
    if (m_is_max_height_fixed_to_text_height)
        SetSizeProperty(SizeProperties::MAX, Dim::Y, size[Dim::Y]);
    SetSizePropertyEnabled(
        SizeProperties::MIN,
        Bool2(m_is_min_width_fixed_to_text_width, m_is_min_height_fixed_to_text_height));
    SetSizePropertyEnabled(
        SizeProperties::MAX,
        Bool2(m_is_max_width_fixed_to_text_width, m_is_max_height_fixed_to_text_height));
}

} // end of namespace Xrb
