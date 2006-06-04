// ///////////////////////////////////////////////////////////////////////////
// xrb_widgetskin.cpp by Victor Dods, created 2005/04/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_widgetskin.h"

#include "xrb_fontface.h"
#include "xrb_screen.h"
#include "xrb_widgetbackground.h"

namespace Xrb
{

WidgetSkin::WidgetSkin (Screen const *screen)
{
    ASSERT1(screen != NULL)
    m_screen = screen;

    // ///////////////////////////////////////////////////////////////////////
    // widget backgrounds
    // ///////////////////////////////////////////////////////////////////////

    m_widget_background[MODAL_WIDGET_BACKGROUND] =
        new WidgetBackgroundColored(Color(0.0f, 0.0f, 0.0f, 1.0f));
    m_widget_background[BUTTON_BACKGROUND] =
        new WidgetBackgroundColored(Color(0.0f, 0.0f, 0.6f, 1.0f));
    m_widget_background[BUTTON_MOUSEOVER_BACKGROUND] =
        new WidgetBackgroundColored(Color(0.2f, 0.2f, 0.7f, 1.0f));
    m_widget_background[BUTTON_PRESSED_BACKGROUND] =
        new WidgetBackgroundColored(Color(0.4f, 0.4f, 0.8f, 1.0f));
    m_widget_background[LINE_EDIT_BACKGROUND] =
        new WidgetBackgroundColored(Color(0.2f, 0.2f, 0.2f, 1.0f));
    m_widget_background[CHECK_BOX_BACKGROUND] =
        new WidgetBackgroundColored(Color(0.9f, 0.8f, 0.0f, 1.0f));
    m_widget_background[RADIO_BUTTON_BACKGROUND] =
        new WidgetBackgroundColored(Color(0.9f, 0.1f, 0.0f, 1.0f));
    m_widget_background[TOOLBAR_BUTTON_BACKGROUND] =
        new WidgetBackgroundColored(Color(0.0f, 0.4f, 0.0f, 1.0f));
    m_widget_background[TOOLBAR_BUTTON_MOUSEOVER_BACKGROUND] =
        new WidgetBackgroundColored(Color(0.4f, 0.8f, 0.4f, 1.0f));
    m_widget_background[TOOLBAR_BUTTON_CHECKED_BACKGROUND] =
        new WidgetBackgroundColored(Color(0.2f, 0.6f, 0.2f, 1.0f));
    m_widget_background[TOOLBAR_BUTTON_PRESSED_BACKGROUND] =
        new WidgetBackgroundColored(Color(0.6f, 1.0f, 0.6f, 1.0f));

    // ///////////////////////////////////////////////////////////////////////
    // fonts
    // ///////////////////////////////////////////////////////////////////////

    // 0.03 looks like about the smallest font that appears decently
    // on a 640x480 screen.
    m_font_specification[DEFAULT_FONT].m_font_height_ratio = 0.023f;
    UpdateFontHeight(DEFAULT_FONT);
    SetFontFaceFilename(
        DEFAULT_FONT,
        "resources/FreeSansBoldCustom.ttf");

    // ///////////////////////////////////////////////////////////////////////
    // gl textures
    // ///////////////////////////////////////////////////////////////////////

    SetTextureFilename(CHECK_BOX_CHECK_TEXTURE, "resources/ui/black_checkmark.png");
    SetTextureFilename(RADIO_BUTTON_CHECK_TEXTURE, "resources/ui/radiobutton_dot.png");

    // ///////////////////////////////////////////////////////////////////////
    // margins
    // ///////////////////////////////////////////////////////////////////////

    SetMarginRatios(DEFAULT_FRAME_MARGINS, FloatVector2(0.006667f, 0.006667f));
    SetMarginRatios(DEFAULT_CONTENT_MARGINS, FloatVector2::ms_zero);
    SetMarginRatios(LAYOUT_FRAME_MARGINS, FloatVector2(0.006667f, 0.006667f));
    SetMarginRatios(LAYOUT_SPACING_MARGINS, FloatVector2(0.006667f, 0.006667f));
}

WidgetSkin::~WidgetSkin ()
{
    for (Uint32 i = 0; i < WIDGET_BACKGROUND_TYPE_COUNT; ++i)
        Delete(m_widget_background[i]);
}

WidgetSkin *WidgetSkin::CreateClone () const
{
    WidgetSkin *retval = new WidgetSkin;

    ASSERT1(m_screen != NULL)
    retval->m_screen = m_screen;

    for (Uint32 i = 0; i < WIDGET_BACKGROUND_TYPE_COUNT; ++i)
        retval->m_widget_background[i] =
            m_widget_background[i] != NULL ?
            m_widget_background[i]->CreateClone() :
            NULL;

    for (Uint32 i = 0; i < FONT_TYPE_COUNT; ++i)
        retval->m_font_specification[i] = m_font_specification[i];

    for (Uint32 i = 0; i < TEXTURE_TYPE_COUNT; ++i)
        retval->m_texture[i] = m_texture[i];

    for (Uint32 i = 0; i < MARGINS_TYPE_COUNT; ++i)
        retval->m_margins_specification[i] = m_margins_specification[i];

    return retval;
}

Float WidgetSkin::GetRatioFromScreenCoord (ScreenCoord const screen_coord) const
{
    ASSERT1(m_screen != NULL)
    ASSERT1(m_screen->GetSizeRatioBasis() > 0)
    return static_cast<Float>(screen_coord) / m_screen->GetSizeRatioBasis();
}

ScreenCoord WidgetSkin::GetScreenCoordFromRatio (Float const ratio) const
{
    ASSERT1(m_screen != NULL)
    ASSERT1(m_screen->GetSizeRatioBasis() > 0)
    return static_cast<ScreenCoord>(ratio * m_screen->GetSizeRatioBasis());
}

FloatVector2 WidgetSkin::GetRatiosFromScreenCoords (
    ScreenCoordVector2 const &screen_coords) const
{
    ASSERT1(m_screen != NULL)
    ASSERT1(m_screen->GetSizeRatioBasis() > 0)
    return screen_coords.StaticCast<Float>() /
           static_cast<Float>(m_screen->GetSizeRatioBasis());
}

ScreenCoordVector2 WidgetSkin::GetScreenCoordsFromRatios (
    FloatVector2 const &ratios) const
{
    ASSERT1(m_screen != NULL)
    ASSERT1(m_screen->GetSizeRatioBasis() > 0)
    return (ratios *
            static_cast<Float>(m_screen->GetSizeRatioBasis())
           ).StaticCast<ScreenCoord>();
}

void WidgetSkin::SetWidgetBackground (
    WidgetBackgroundType const widget_background_type,
    WidgetBackground const *const widget_background)
{
    ASSERT1(widget_background_type < WIDGET_BACKGROUND_TYPE_COUNT)
    delete m_widget_background[widget_background_type];
    m_widget_background[widget_background_type] = widget_background;
}

void WidgetSkin::SetFont (
    FontType const font_type,
    Resource<Font> const &font)
{
    ASSERT1(font_type < FONT_TYPE_COUNT)
    ASSERT1(font.GetIsValid())
    m_font_specification[font_type].m_font = font;
    m_font_specification[font_type].m_font_height = font->GetPixelHeight();
    m_font_specification[font_type].m_font_height_ratio =
        GetRatioFromScreenCoord(font->GetPixelHeight());
}

void WidgetSkin::SetFontFaceFilename (
    FontType const font_type,
    std::string const &font_face_filename)
{
    ASSERT1(font_type < FONT_TYPE_COUNT)
    m_font_specification[font_type].m_font_height =
        GetScreenCoordFromRatio(
            m_font_specification[font_type].m_font_height_ratio);
    m_font_specification[font_type].m_font =
        Singletons::ResourceLibrary()->LoadFilename<Font>(
            AsciiFont::Create,
            font_face_filename,
            m_font_specification[font_type].m_font_height);
    ASSERT1(m_font_specification[font_type].m_font.GetIsValid())
}


void WidgetSkin::SetFontHeightRatio (
    FontType const font_type,
    Float const font_height_ratio)
{
    ASSERT1(font_type < FONT_TYPE_COUNT)
    ASSERT1(font_height_ratio > 0.0f)
    m_font_specification[font_type].m_font_height_ratio = font_height_ratio;
    m_font_specification[font_type].m_font_height =
        GetScreenCoordFromRatio(font_height_ratio);
    m_font_specification[font_type].m_font =
        Singletons::ResourceLibrary()->LoadFilename<Font>(
            AsciiFont::Create,
            m_font_specification[font_type].m_font.GetFilename(),
            m_font_specification[font_type].m_font_height);
    ASSERT1(m_font_specification[font_type].m_font.GetIsValid())
}

void WidgetSkin::SetFontHeight (
    FontType const font_type,
    ScreenCoord const font_height)
{
    ASSERT1(font_type < FONT_TYPE_COUNT)
    ASSERT1(font_height > 0)
    m_font_specification[font_type].m_font_height = font_height;
    m_font_specification[font_type].m_font_height_ratio =
        GetRatioFromScreenCoord(font_height);
    m_font_specification[font_type].m_font =
        Singletons::ResourceLibrary()->LoadFilename<Font>(
            AsciiFont::Create,
            m_font_specification[font_type].m_font.GetFilename(),
            m_font_specification[font_type].m_font_height);
    ASSERT1(m_font_specification[font_type].m_font.GetIsValid())
}

void WidgetSkin::SetTexture (
    TextureType const texture_type,
    Resource<GLTexture> const &texture)
{
    ASSERT1(texture_type < TEXTURE_TYPE_COUNT)
    m_texture[texture_type] = texture;
}

void WidgetSkin::SetTextureFilename (
    TextureType const texture_type,
    std::string const &texture_filename)
{
    ASSERT1(texture_type < TEXTURE_TYPE_COUNT)
    m_texture[texture_type] =
        Singletons::ResourceLibrary()->LoadFilename<GLTexture>(
            GLTexture::Create,
            texture_filename);
}

void WidgetSkin::SetMarginRatios (
    MarginsType const margins_type,
    FloatVector2 const &margin_ratios)
{
    ASSERT1(margins_type < MARGINS_TYPE_COUNT)
    ASSERT1(margin_ratios[Dim::X] >= static_cast<Float>(0))
    ASSERT1(margin_ratios[Dim::Y] >= static_cast<Float>(0))
    m_margins_specification[margins_type].m_margin_ratios = margin_ratios;
    m_margins_specification[margins_type].m_margins =
        GetScreenCoordsFromRatios(margin_ratios);
}

void WidgetSkin::SetMargins (
    MarginsType const margins_type,
    ScreenCoordVector2 const &margins)
{
    ASSERT1(margins_type < MARGINS_TYPE_COUNT)
    ASSERT1(margins[Dim::X] >= 0)
    ASSERT1(margins[Dim::Y] >= 0)
    m_margins_specification[margins_type].m_margins = margins;
    m_margins_specification[margins_type].m_margin_ratios =
        GetRatiosFromScreenCoords(margins);
}

void WidgetSkin::UpdateFontHeight (FontType const font_type)
{
    ASSERT1(font_type < FONT_TYPE_COUNT)
    m_font_specification[font_type].m_font_height =
        GetScreenCoordFromRatio(
            m_font_specification[font_type].m_font_height_ratio);
}

void WidgetSkin::UpdateMargins (MarginsType const margins_type)
{
    ASSERT1(margins_type < MARGINS_TYPE_COUNT)
    m_margins_specification[margins_type].m_margins =
        GetScreenCoordsFromRatios(
            m_margins_specification[margins_type].m_margin_ratios);
}

} // end of namespace Xrb
