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
#include "xrb_widgetcontext.hpp"

namespace Xrb {

TextWidget::TextWidget (std::string const &text, WidgetContext &context, std::string const &name)
    :
    Widget(context, name),
    m_text(text),
    m_receiver_set_text(&TextWidget::SetText, this),
    m_receiver_set_text_v(&TextWidget::SetTextV, this)
{
    m_accepts_focus = false;
    m_accepts_mouseover = false;

    m_text_color = Color(1.0, 1.0, 1.0, 1.0);
    m_render_text_color_needs_update = true;
    m_render_font_needs_update = true;
    m_font = Context().WidgetSkin_LoadFont(WidgetSkin::FontType::DEFAULT);
    
    m_is_min_width_fixed_to_text_width = false;
    m_is_max_width_fixed_to_text_width = false;
    m_is_min_height_fixed_to_text_height = false;
    m_is_max_height_fixed_to_text_height = false;
}

Resource<Font> const &TextWidget::GetFont () const
{
    return m_font;
}

void TextWidget::SetText (std::string const &text)
{
    if (m_text != text)
    {
        m_text = text;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetTextColor (Color const &color)
{
    m_text_color = color;
    SetRenderTextColorNeedsUpdate();
}

void TextWidget::SetIsMinWidthFixedToTextWidth (bool is_min_width_fixed_to_text_width)
{
    if (m_is_min_width_fixed_to_text_width != is_min_width_fixed_to_text_width)
    {
        m_is_min_width_fixed_to_text_width = is_min_width_fixed_to_text_width;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetIsMaxWidthFixedToTextWidth (bool is_max_width_fixed_to_text_width)
{
    if (m_is_max_width_fixed_to_text_width != is_max_width_fixed_to_text_width)
    {
        m_is_max_width_fixed_to_text_width = is_max_width_fixed_to_text_width;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetIsWidthFixedToTextWidth (bool is_width_fixed_to_text_width)
{
    if (m_is_min_width_fixed_to_text_width != is_width_fixed_to_text_width ||
        m_is_max_width_fixed_to_text_width != is_width_fixed_to_text_width)
    {
        m_is_min_width_fixed_to_text_width = is_width_fixed_to_text_width;
        m_is_max_width_fixed_to_text_width = is_width_fixed_to_text_width;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetIsMinHeightFixedToTextHeight (bool is_min_height_fixed_to_text_height)
{
    if (m_is_min_height_fixed_to_text_height != is_min_height_fixed_to_text_height)
    {
        m_is_min_height_fixed_to_text_height = is_min_height_fixed_to_text_height;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetIsMaxHeightFixedToTextHeight (bool is_max_height_fixed_to_text_height)
{
    if (m_is_max_height_fixed_to_text_height != is_max_height_fixed_to_text_height)
    {
        m_is_max_height_fixed_to_text_height = is_max_height_fixed_to_text_height;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetIsHeightFixedToTextHeight (bool is_height_fixed_to_text_height)
{
    if (m_is_min_height_fixed_to_text_height != is_height_fixed_to_text_height ||
        m_is_max_height_fixed_to_text_height != is_height_fixed_to_text_height)
    {
        m_is_min_height_fixed_to_text_height = is_height_fixed_to_text_height;
        m_is_max_height_fixed_to_text_height = is_height_fixed_to_text_height;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetIsMinSizeFixedToTextSize (bool is_min_size_fixed_to_text_size)
{
    if (m_is_min_width_fixed_to_text_width != is_min_size_fixed_to_text_size ||
        m_is_min_height_fixed_to_text_height != is_min_size_fixed_to_text_size)
    {
        m_is_min_width_fixed_to_text_width = is_min_size_fixed_to_text_size;
        m_is_min_height_fixed_to_text_height = is_min_size_fixed_to_text_size;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetIsMaxSizeFixedToTextSize (bool is_max_size_fixed_to_text_size)
{
    if (m_is_max_width_fixed_to_text_width != is_max_size_fixed_to_text_size ||
        m_is_max_height_fixed_to_text_height != is_max_size_fixed_to_text_size)
    {
        m_is_max_width_fixed_to_text_width = is_max_size_fixed_to_text_size;
        m_is_max_height_fixed_to_text_height = is_max_size_fixed_to_text_size;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetIsSizeFixedToTextSize (bool is_size_fixed_to_text_size)
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

void TextWidget::PreDraw ()
{
    Widget::PreDraw();
    
    if (RenderTextColorNeedsUpdate())
        UpdateRenderTextColor();
    ASSERT1(!RenderTextColorNeedsUpdate());
    
    if (RenderFontNeedsUpdate())
        UpdateRenderFont();
    ASSERT1(!RenderFontNeedsUpdate());
}

void TextWidget::SetRenderFont (Resource<Font> const &render_font)
{
    if (m_render_font != render_font)
    {
        m_render_font = render_font;
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::HandleChangedWidgetSkin ()
{
    Widget::HandleChangedWidgetSkin();
    m_font = Context().WidgetSkin_LoadFont(WidgetSkin::FontType::DEFAULT);
    SetRenderBackgroundNeedsUpdate();
    SetRenderTextColorNeedsUpdate();
    SetRenderFontNeedsUpdate();
}

void TextWidget::UpdateRenderTextColor ()
{
    m_render_text_color_needs_update = false;
    SetRenderTextColor(TextColor());
}

void TextWidget::UpdateRenderFont ()
{
    m_render_font_needs_update = false;
    SetRenderFont(GetFont());
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

ScreenCoordRect TextWidget::TextRect () const
{
    return RenderFont()->StringRect(Text().c_str());
}

void TextWidget::UpdateMinAndMaxSizesFromText ()
{
    if (!RenderFont().IsValid())
        return;

    ScreenCoordVector2 size(TextRect().Size() + (FrameMargins() + ContentMargins()).TotalMarginSize());

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
/*
void TextWidget::UpdateMinAndMaxSizesFromText ()
{
    if (!RenderFont().IsValid())
        return;

    ScreenCoordVector2 size(TextRect().Size() + (FrameMargins() + ContentMargins()).TotalMarginSize());

    if (m_is_min_width_fixed_to_text_width)
    {
        SetSizeProperty(SizeProperties::MIN, Dim::X, size[Dim::X]);
        SetSizePropertyEnabled(SizeProperties::MIN, Dim::X, m_is_min_width_fixed_to_text_width);
    }
    if (m_is_max_width_fixed_to_text_width)
    {
        SetSizeProperty(SizeProperties::MAX, Dim::X, size[Dim::X]);
        SetSizePropertyEnabled(SizeProperties::MAX, Dim::X, m_is_max_width_fixed_to_text_width);
    }
    if (m_is_min_height_fixed_to_text_height)
    {
        SetSizeProperty(SizeProperties::MIN, Dim::Y, size[Dim::Y]);
        SetSizePropertyEnabled(SizeProperties::MIN, Dim::Y, m_is_min_height_fixed_to_text_height);
    }
    if (m_is_max_height_fixed_to_text_height)
    {
        SetSizeProperty(SizeProperties::MAX, Dim::Y, size[Dim::Y]);
        SetSizePropertyEnabled(SizeProperties::MAX, Dim::Y, m_is_max_height_fixed_to_text_height);
    }
}
*/
} // end of namespace Xrb
