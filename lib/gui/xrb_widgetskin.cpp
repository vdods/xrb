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
    for (Uint32 i = 0; i < WIDGET_BACKGROUND_TYPE_COUNT; ++i)
        m_widget_background[i] = NULL;

    for (Uint32 i = 0; i < FONT_TYPE_COUNT; ++i)
    {
        m_font_specification[i].m_path = "resources/FreeSansBoldCustom.ttf";
        // 0.03 looks like about the smallest font that appears decently on a 640x480 screen.
        m_font_specification[i].m_height_ratio = 0.03f;
    }

    // the textures are already constructed empty.
    
    for (Uint32 i = 0; i < MARGINS_TYPE_COUNT; ++i)
        m_margins_ratios[i] = FloatMargins::ms_zero;
}

WidgetSkin::~WidgetSkin ()
{
    // in case it didn't already happen.
    ReleaseAllResources();
}

void WidgetSkin::PopulateUsingDefaults ()
{
    // in case there was stuff in here already
    ReleaseAllResources();
    
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

void WidgetSkin::ReleaseAllResources ()
{
    for (Uint32 i = 0; i < WIDGET_BACKGROUND_TYPE_COUNT; ++i)
        DeleteAndNullify(m_widget_background[i]);

    for (Uint32 i = 0; i < FONT_TYPE_COUNT; ++i)
    {
        m_font_specification[i].m_path = "resources/FreeSansBoldCustom.ttf";
        // 0.03 looks like about the smallest font that appears decently on a 640x480 screen.
        m_font_specification[i].m_height_ratio = 0.03f;
    }
    
    for (Uint32 i = 0; i < TEXTURE_TYPE_COUNT; ++i)
    {
        m_texture[i].Release();
        ASSERT1(!m_texture[i].IsValid());
    }

    for (Uint32 i = 0; i < MARGINS_TYPE_COUNT; ++i)
        m_margins_ratios[i] = FloatMargins::ms_zero;
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
