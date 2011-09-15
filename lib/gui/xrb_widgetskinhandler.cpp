// ///////////////////////////////////////////////////////////////////////////
// xrb_widgetskinhandler.cpp by Victor Dods, created 2005/06/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_widgetskinhandler.hpp"

#include "xrb_emptystring.hpp"

namespace Xrb {

WidgetSkinHandler::WidgetSkinHandler ()
{
    m_widget_skin = NULL;
    m_delete_widget_skin = false;
}

WidgetSkinHandler::~WidgetSkinHandler ()
{
    if (m_delete_widget_skin)
        Delete(m_widget_skin);
    else
        m_widget_skin = NULL;
}

WidgetBackground const *WidgetSkinHandler::WidgetSkinWidgetBackground (WidgetSkin::WidgetBackgroundType widget_background_type) const
{
    if (m_widget_skin == NULL)
        return NULL;
    else
        return m_widget_skin->GetWidgetBackground(widget_background_type);
}

std::string const &WidgetSkinHandler::WidgetSkinFontPath (WidgetSkin::FontType font_type) const
{
    if (m_widget_skin == NULL)
        return g_empty_string;
    else
        return m_widget_skin->FontPath(font_type);
}

Float WidgetSkinHandler::WidgetSkinFontHeightRatio (WidgetSkin::FontType font_type) const
{
    if (m_widget_skin == NULL)
        return 0.03f; // 0.03 looks like about the smallest font that appears decently on a 640x480 screen.
    else
        return m_widget_skin->FontHeightRatio(font_type);
}

Resource<Font> WidgetSkinHandler::WidgetSkinLoadFont (WidgetSkin::FontType font_type) const
{
    if (m_widget_skin == NULL)
        return Font::Load(
            "resources/FreeSansBoldCustom.ttf",
            WidgetSkin::FontPixelHeight(
                WidgetSkinFontHeightRatio(font_type), // will return the default font height ratio
                WidgetSkinHandlerSizeRatioBasis()));
    else
        return m_widget_skin->LoadFont(font_type, WidgetSkinHandlerSizeRatioBasis());
}

Resource<GlTexture> WidgetSkinHandler::WidgetSkinTexture (WidgetSkin::TextureType texture_type) const
{
    if (m_widget_skin == NULL)
        return Resource<GlTexture>();
    else
        return m_widget_skin->GetTexture(texture_type);
}

ScreenCoordMargins WidgetSkinHandler::WidgetSkinMargins (WidgetSkin::MarginsType margins_type) const
{
    if (m_widget_skin == NULL)
        return ScreenCoordMargins::ms_zero;
    else
        return m_widget_skin->Margins(margins_type, WidgetSkinHandlerSizeRatioBasis());
}

// ///////////////////////////////////////////////////////////////////////////
// WidgetSkin frontend modifiers
// ///////////////////////////////////////////////////////////////////////////

void WidgetSkinHandler::SetWidgetSkin (WidgetSkin *widget_skin)
{
    if (m_delete_widget_skin)
        Delete(m_widget_skin);
    m_widget_skin = widget_skin;
    
    for (Uint32 i = 0, count = WidgetSkinHandlerChildCount(); i < count; ++i)
    {
        WidgetSkinHandler *child = WidgetSkinHandlerChild(i);
        ASSERT1(child != NULL);
        child->SetWidgetSkin(widget_skin);
        child->m_delete_widget_skin = false;
    }
    
    HandleChangedWidgetSkin();
}
/*
void WidgetSkinHandler::SetWidgetSkinWidgetBackground (WidgetSkin::WidgetBackgroundType widget_background_type, WidgetBackground const *widget_background)
{
    SetProperty<
        WidgetSkin::WidgetBackgroundType,
        WidgetBackground const *>(
            widget_background_type,
            widget_background,
            &WidgetSkin::SetWidgetBackground);
}

void WidgetSkinHandler::SetWidgetSkinFont (WidgetSkin::FontType font_type, Resource<Font> const &font)
{
    SetProperty<
        WidgetSkin::FontType,
        Resource<Font> const &>(
            font_type,
            font,
            &WidgetSkin::SetFont);
}

void WidgetSkinHandler::SetWidgetSkinFontFacePath (WidgetSkin::FontType font_type, std::string const &font_face_path)
{
    SetProperty<
        WidgetSkin::FontType,
        std::string const &>(
            font_type,
            font_face_path,
            &WidgetSkin::SetFontFacePath);
}

void WidgetSkinHandler::SetWidgetSkinFontHeightRatio (WidgetSkin::FontType font_type, Float font_height_ratio)
{
    SetProperty<
        WidgetSkin::FontType,
        Float>(
            font_type,
            font_height_ratio,
            &WidgetSkin::SetFontHeightRatio);
}

void WidgetSkinHandler::SetWidgetSkinFontHeight (WidgetSkin::FontType font_type, ScreenCoord font_height)
{
    SetProperty<
        WidgetSkin::FontType,
        ScreenCoord>(
            font_type,
            font_height,
            &WidgetSkin::SetFontHeight);
}

void WidgetSkinHandler::SetWidgetSkinTexture (WidgetSkin::TextureType texture_type, Resource<GlTexture> const &texture)
{
    SetProperty<
        WidgetSkin::TextureType,
        Resource<GlTexture> const &>(
            texture_type,
            texture,
            &WidgetSkin::SetTexture);
}

void WidgetSkinHandler::SetWidgetSkinTexturePath (WidgetSkin::TextureType texture_type, std::string const &texture_path)
{
    SetProperty<
        WidgetSkin::TextureType,
        std::string const &>(
            texture_type,
            texture_path,
            &WidgetSkin::SetTexturePath);
}

void WidgetSkinHandler::SetWidgetSkinMarginRatios (WidgetSkin::MarginsType margin_type, FloatMargins const &margin_ratios)
{
    SetProperty<
        WidgetSkin::MarginsType,
        FloatMargins const &>(
            margin_type,
            margin_ratios,
            &WidgetSkin::SetMarginRatios);
}

void WidgetSkinHandler::SetWidgetSkinMargins (WidgetSkin::MarginsType margin_type, ScreenCoordMargins const &margins)
{
    SetProperty<
        WidgetSkin::MarginsType,
        ScreenCoordMargins const &>(
            margin_type,
            margins,
            &WidgetSkin::SetMargins);
}
*/
// ///////////////////////////////////////////////////////////////////////////

void WidgetSkinHandler::ReleaseAllWidgetSkinResources ()
{
    ASSERT1(m_widget_skin != NULL);
    m_widget_skin->ReleaseAllResources();
}

} // end of namespace Xrb
