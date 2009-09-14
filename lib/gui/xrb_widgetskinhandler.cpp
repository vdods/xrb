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

namespace Xrb
{

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

// ///////////////////////////////////////////////////////////////////////////
// WidgetSkin frontend modifiers
// ///////////////////////////////////////////////////////////////////////////

void WidgetSkinHandler::SetWidgetSkinWidgetBackground (
    WidgetSkin::WidgetBackgroundType const widget_background_type,
    WidgetBackground const *const widget_background)
{
    SetProperty<
        WidgetSkin::WidgetBackgroundType,
        WidgetBackground const *>(
            widget_background_type,
            widget_background,
            &WidgetSkin::SetWidgetBackground,
            &WidgetSkinHandler::PropagateChangedWidgetBackground);
}

void WidgetSkinHandler::SetWidgetSkinFont (
    WidgetSkin::FontType const font_type,
    Resource<Font> const &font)
{
    SetProperty<
        WidgetSkin::FontType,
        Resource<Font> const &>(
            font_type,
            font,
            &WidgetSkin::SetFont,
            &WidgetSkinHandler::PropagateChangedFont);
}

void WidgetSkinHandler::SetWidgetSkinFontFacePath (
    WidgetSkin::FontType const font_type,
    std::string const &font_face_path)
{
    SetProperty<
        WidgetSkin::FontType,
        std::string const &>(
            font_type,
            font_face_path,
            &WidgetSkin::SetFontFacePath,
            &WidgetSkinHandler::PropagateChangedFont);
}

void WidgetSkinHandler::SetWidgetSkinFontHeightRatio (
    WidgetSkin::FontType const font_type,
    Float const font_height_ratio)
{
    SetProperty<
        WidgetSkin::FontType,
        Float>(
            font_type,
            font_height_ratio,
            &WidgetSkin::SetFontHeightRatio,
            &WidgetSkinHandler::PropagateChangedFont);
}

void WidgetSkinHandler::SetWidgetSkinFontHeight (
    WidgetSkin::FontType const font_type,
    ScreenCoord const font_height)
{
    SetProperty<
        WidgetSkin::FontType,
        ScreenCoord>(
            font_type,
            font_height,
            &WidgetSkin::SetFontHeight,
            &WidgetSkinHandler::PropagateChangedFont);
}

void WidgetSkinHandler::SetWidgetSkinTexture (
    WidgetSkin::TextureType const texture_type,
    Resource<GlTexture> const &texture)
{
    SetProperty<
        WidgetSkin::TextureType,
        Resource<GlTexture> const &>(
            texture_type,
            texture,
            &WidgetSkin::SetTexture,
            &WidgetSkinHandler::PropagateChangedTexture);
}

void WidgetSkinHandler::SetWidgetSkinTexturePath (
    WidgetSkin::TextureType const texture_type,
    std::string const &texture_path)
{
    SetProperty<
        WidgetSkin::TextureType,
        std::string const &>(
            texture_type,
            texture_path,
            &WidgetSkin::SetTexturePath,
            &WidgetSkinHandler::PropagateChangedTexture);
}

void WidgetSkinHandler::SetWidgetSkinMarginRatios (
    WidgetSkin::MarginsType const margin_type,
    FloatVector2 const &margin_ratios)
{
    SetProperty<
        WidgetSkin::MarginsType,
        FloatVector2 const &>(
            margin_type,
            margin_ratios,
            &WidgetSkin::SetMarginRatios,
            &WidgetSkinHandler::PropagateChangedMargins);
}

void WidgetSkinHandler::SetWidgetSkinMargins (
    WidgetSkin::MarginsType const margin_type,
    ScreenCoordVector2 const &margins)
{
    SetProperty<
        WidgetSkin::MarginsType,
        ScreenCoordVector2 const &>(
            margin_type,
            margins,
            &WidgetSkin::SetMargins,
            &WidgetSkinHandler::PropagateChangedMargins);
}

// ///////////////////////////////////////////////////////////////////////////

void WidgetSkinHandler::ReleaseAllWidgetSkinResources ()
{
    ASSERT1(m_widget_skin != NULL);
    m_widget_skin->ReleaseAllResources();
}

// ///////////////////////////////////////////////////////////////////////////
// these functions propagate property changes to child widgets
// ///////////////////////////////////////////////////////////////////////////

void WidgetSkinHandler::PropagateChangedWidgetBackground (
    WidgetSkin::WidgetBackgroundType const widget_background_type)
{
    PropagateChangedProperty<WidgetSkin::WidgetBackgroundType>(
        widget_background_type,
        &WidgetSkinHandler::HandleChangedWidgetSkinWidgetBackground);
}

void WidgetSkinHandler::PropagateChangedFont (
    WidgetSkin::FontType const font_type)
{
    PropagateChangedProperty<WidgetSkin::FontType>(
        font_type,
        &WidgetSkinHandler::HandleChangedWidgetSkinFont);
}

void WidgetSkinHandler::PropagateChangedTexture (
    WidgetSkin::TextureType const texture_type)
{
    PropagateChangedProperty<WidgetSkin::TextureType>(
        texture_type,
        &WidgetSkinHandler::HandleChangedWidgetSkinTexture);
}

void WidgetSkinHandler::PropagateChangedMargins (
    WidgetSkin::MarginsType const margins_type)
{
    PropagateChangedProperty<WidgetSkin::MarginsType>(
        margins_type,
        &WidgetSkinHandler::HandleChangedWidgetSkinMargins);
}

} // end of namespace Xrb
