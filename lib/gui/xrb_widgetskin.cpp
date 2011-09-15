// ///////////////////////////////////////////////////////////////////////////
// xrb_widgetskin.cpp by Victor Dods, created 2005/04/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_widgetskin.hpp"

#include "xrb_math.hpp"
#include "xrb_screen.hpp"
#include "xrb_widgetbackground.hpp"

namespace Xrb {

WidgetSkin::WidgetSkin ()
{
    // ///////////////////////////////////////////////////////////////////////
    // widget backgrounds
    // ///////////////////////////////////////////////////////////////////////

    m_widget_background[MODAL_WIDGET_BACKGROUND] = new WidgetBackgroundColored(Color(0.0f, 0.0f, 0.0f, 1.0f));
    m_widget_background[BUTTON_BACKGROUND] = new WidgetBackgroundColored(Color(0.0f, 0.0f, 0.6f, 1.0f));
    m_widget_background[BUTTON_MOUSEOVER_BACKGROUND] = new WidgetBackgroundColored(Color(0.2f, 0.2f, 0.7f, 1.0f));
    m_widget_background[BUTTON_PRESSED_BACKGROUND] = new WidgetBackgroundColored(Color(0.4f, 0.4f, 0.8f, 1.0f));
    m_widget_background[LINE_EDIT_BACKGROUND] = new WidgetBackgroundColored(Color(0.2f, 0.2f, 0.2f, 1.0f));
    m_widget_background[CHECK_BOX_BACKGROUND] = new WidgetBackgroundColored(Color(0.9f, 0.8f, 0.0f, 1.0f));
    m_widget_background[RADIO_BUTTON_BACKGROUND] = new WidgetBackgroundColored(Color(0.9f, 0.1f, 0.0f, 1.0f));
    m_widget_background[TOOLBAR_BUTTON_BACKGROUND] = new WidgetBackgroundColored(Color(0.0f, 0.4f, 0.0f, 1.0f));
    m_widget_background[TOOLBAR_BUTTON_MOUSEOVER_BACKGROUND] = new WidgetBackgroundColored(Color(0.4f, 0.8f, 0.4f, 1.0f));
    m_widget_background[TOOLBAR_BUTTON_CHECKED_BACKGROUND] = new WidgetBackgroundColored(Color(0.2f, 0.6f, 0.2f, 1.0f));
    m_widget_background[TOOLBAR_BUTTON_PRESSED_BACKGROUND] = new WidgetBackgroundColored(Color(0.6f, 1.0f, 0.6f, 1.0f));

    // ///////////////////////////////////////////////////////////////////////
    // fonts
    // ///////////////////////////////////////////////////////////////////////

    // 0.03 looks like about the smallest font that appears decently on a 640x480 screen.
    m_font_specification[DEFAULT_FONT].m_path = "resources/FreeSansBoldCustom.ttf";
    m_font_specification[DEFAULT_FONT].m_height_ratio = 0.023f;

    // ///////////////////////////////////////////////////////////////////////
    // gl textures
    // ///////////////////////////////////////////////////////////////////////

    m_texture[CHECK_BOX_CHECK_TEXTURE] = GlTexture::Load("resources/ui/black_checkmark.png"); // TODO: replace with procedurally generated texture
    m_texture[RADIO_BUTTON_CHECK_TEXTURE] = GlTexture::Load("resources/ui/radiobutton_dot.png"); // TODO: replace with procedurally generated texture

    // ///////////////////////////////////////////////////////////////////////
    // margins
    // ///////////////////////////////////////////////////////////////////////

    m_margins_ratios[DEFAULT_FRAME_MARGINS] = FloatMargins(FloatVector2(0.006667f, 0.006667f));
    m_margins_ratios[DEFAULT_CONTENT_MARGINS] = FloatMargins(FloatVector2::ms_zero);
    m_margins_ratios[LAYOUT_FRAME_MARGINS] = FloatMargins(FloatVector2(0.006667f, 0.006667f));
    m_margins_ratios[LAYOUT_SPACING_MARGINS] = FloatMargins(FloatVector2(0.006667f, 0.006667f));
    m_margins_ratios[CHECK_BOX_FRAME_MARGINS] = FloatMargins(FloatVector2::ms_zero);
    m_margins_ratios[RADIO_BUTTON_FRAME_MARGINS] = FloatMargins(FloatVector2::ms_zero);
}

WidgetSkin::~WidgetSkin ()
{
    // just in case it didn't already happen.
    ReleaseAllResources();
}

void WidgetSkin::ReleaseAllResources ()
{
    for (Uint32 i = 0; i < WIDGET_BACKGROUND_TYPE_COUNT; ++i)
        DeleteAndNullify(m_widget_background[i]);

    for (Uint32 i = 0; i < TEXTURE_TYPE_COUNT; ++i)
    {
        m_texture[i].Release();
        ASSERT1(!m_texture[i].IsValid());
    }
}

ScreenCoord WidgetSkin::FontPixelHeight (Float font_height_ratio, ScreenCoord size_ratio_basis)
{
    ASSERT1(size_ratio_basis > 0);
    return ScreenCoord(Math::Round(font_height_ratio * size_ratio_basis));
}

Float WidgetSkin::FontHeightRatio (ScreenCoord font_pixel_height, ScreenCoord size_ratio_basis)
{
    ASSERT1(size_ratio_basis > 0);
    Float font_height_ratio = Float(font_pixel_height) / size_ratio_basis;
    ASSERT1(FontPixelHeight(font_height_ratio, size_ratio_basis) == font_pixel_height);
    return font_height_ratio;
}

Resource<Font> WidgetSkin::LoadFont (FontType font_type, ScreenCoord size_ratio_basis) const
{
    ASSERT1(font_type < FONT_TYPE_COUNT);
    ASSERT1(size_ratio_basis > 0);
    return Font::Load(m_font_specification[font_type].m_path, FontPixelHeight(m_font_specification[font_type].m_height_ratio, size_ratio_basis));
}

ScreenCoordMargins WidgetSkin::Margins (MarginsType margins_type, ScreenCoord size_ratio_basis) const
{
    ASSERT1(margins_type < MARGINS_TYPE_COUNT);
    ASSERT1(size_ratio_basis > 0);
    return (m_margins_ratios[margins_type] * Float(size_ratio_basis)).StaticCast<ScreenCoord>();
}

void WidgetSkin::SetWidgetBackground (WidgetBackgroundType widget_background_type, WidgetBackground const *widget_background)
{
    ASSERT1(widget_background_type < WIDGET_BACKGROUND_TYPE_COUNT);
    delete m_widget_background[widget_background_type];
    m_widget_background[widget_background_type] = widget_background;
}

void WidgetSkin::SetFontPath (FontType font_type, std::string const &font_path)
{
    ASSERT1(font_type < FONT_TYPE_COUNT);
    m_font_specification[font_type].m_path = font_path;
}

void WidgetSkin::SetFontHeightRatio (FontType font_type, Float font_height_ratio)
{
    ASSERT1(font_type < FONT_TYPE_COUNT);
    m_font_specification[font_type].m_height_ratio = font_height_ratio;
}

void WidgetSkin::SetTexture (TextureType texture_type, Resource<GlTexture> const &texture)
{
    ASSERT1(texture_type < TEXTURE_TYPE_COUNT);
    m_texture[texture_type] = texture;
}

void WidgetSkin::SetMarginsRatios (MarginsType margins_type, FloatMargins const &margins_ratios)
{
    ASSERT1(margins_type < MARGINS_TYPE_COUNT);
    m_margins_ratios[margins_type] = margins_ratios;
}

} // end of namespace Xrb
