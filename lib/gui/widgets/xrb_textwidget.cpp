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

    m_text_color = Color(1.0f, 1.0f, 1.0f, 1.0f);
    m_render_text_color_needs_update = true;
    m_render_font_needs_update = true;
    m_font_style = StyleSheet::FontType::DEFAULT;
    
    m_is_min_width_fixed_to_text_width = false;
    m_is_max_width_fixed_to_text_width = false;
    m_is_min_height_fixed_to_text_height = false;
    m_is_max_height_fixed_to_text_height = false;
}

std::string const &TextWidget::FontPath () const
{
    if (FontStyleIsOverridden())
        return m_font_path;
    else
        return Context().StyleSheet_FontPath(m_font_style);
}

Float TextWidget::FontHeightRatio () const
{
    if (FontStyleIsOverridden())
        return m_font_height_ratio;
    else
        return Context().StyleSheet_FontHeightRatio(m_font_style);
}

ScreenCoord TextWidget::FontPixelHeight () const
{
    return Context().PixelSize(FontHeightRatio());
}

Resource<Font> const &TextWidget::GetFont () const
{
    EnsureFontIsCached();
    ASSERT1(m_font.IsValid());
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

void TextWidget::SetFontStyle (std::string const &style)
{
    ASSERT1(!style.empty() && "don't specify an empty style, use SetBackground to override the value explicitly");
    if (m_font_style != style)
    {
        m_font_style = style;
        EnsureFontIsCached();
        SetRenderFontNeedsUpdate();
        UpdateMinAndMaxSizesFromText();
    }
}

void TextWidget::SetFontPath (std::string const &font_path)
{
    SetFont(font_path, FontHeightRatio()); // use the existing height ratio, replace the font path.
}

void TextWidget::SetFontHeightRatio (Float font_height_ratio)
{
    SetFont(FontPath(), font_height_ratio); // use the existing font path, replace the font height ratio.
}

void TextWidget::SetFontPixelHeight (ScreenCoord font_pixel_height)
{
    SetFont(FontPath(), Context().SizeRatio(font_pixel_height)); // use the existing font path, replace the font height ratio
}

void TextWidget::SetFont (std::string const &font_path, Float font_height_ratio)
{
    ScreenCoord font_pixel_height = Context().PixelSize(font_height_ratio);
    SetFont(Font::Load(font_path, font_pixel_height));
    // store the "real" font height ratio, because there could be aliasing via the double conversion in the other SetFont.
    m_font_height_ratio = font_height_ratio;
}

void TextWidget::SetFont (Resource<Font> const &font)
{
    ASSERT1(font.IsValid());
    m_font_style.clear(); // override style sheet by setting the style string to empty
    ASSERT1(FontStyleIsOverridden());

    // extract the font path and height ratio
    m_font_path = font->FontFacePath();
    m_font_height_ratio = Context().SizeRatio(font->PixelHeight());

    // set the font and update stuff
    m_font = font;
    SetRenderFontNeedsUpdate();
    UpdateMinAndMaxSizesFromText();
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

void TextWidget::HandleChangedStyleSheet ()
{
    Widget::HandleChangedStyleSheet();
    EnsureFontIsCached();
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

void TextWidget::EnsureFontIsCached () const
{
    std::string const &font_path(FontStyleIsOverridden() ? m_font_path : Context().StyleSheet_FontPath(m_font_style));
    Float font_height_ratio(FontStyleIsOverridden() ? m_font_height_ratio : Context().StyleSheet_FontHeightRatio(m_font_style));
    ScreenCoord font_pixel_height = Context().PixelSize(font_height_ratio);
    // [re]cache the font if...
    if (!m_font.IsValid() ||                        // there is no font cached, or
        m_font->FontFacePath() != font_path ||      // the font path doesn't match, or
        m_font->PixelHeight() != font_pixel_height) // the font pixel height doesn't match.
    {
        m_font = Font::Load(font_path, font_pixel_height);
    }
    ASSERT1(m_font.IsValid());
}

} // end of namespace Xrb
