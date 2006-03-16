// ///////////////////////////////////////////////////////////////////////////
// xrb_widgetskinhandler.cpp by Victor Dods, created 2005/06/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_widgetskinhandler.h"

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
        WidgetBackground const *,
        &WidgetSkin::SetWidgetBackground,
        &WidgetSkinHandler::PropagateChangedWidgetBackground>(
            widget_background_type,
            widget_background);
}

void WidgetSkinHandler::SetWidgetSkinFont (
    WidgetSkin::FontType const font_type,
    Resource<Font> const &font)
{
    SetProperty<
        WidgetSkin::FontType,
        Resource<Font> const &,
        &WidgetSkin::SetFont,
        &WidgetSkinHandler::PropagateChangedFont>(
            font_type,
            font);
}

void WidgetSkinHandler::SetWidgetSkinFontFaceFilename (
    WidgetSkin::FontType const font_type,
    std::string const &font_face_filename)
{
    SetProperty<
        WidgetSkin::FontType,
        std::string const &,
        &WidgetSkin::SetFontFaceFilename,
        &WidgetSkinHandler::PropagateChangedFont>(
            font_type,
            font_face_filename);
}

void WidgetSkinHandler::SetWidgetSkinFontHeightRatio (
    WidgetSkin::FontType const font_type,
    Float const font_height_ratio)
{
    SetProperty<
        WidgetSkin::FontType,
        Float,
        &WidgetSkin::SetFontHeightRatio,
        &WidgetSkinHandler::PropagateChangedFont>(
            font_type,
            font_height_ratio);
}

void WidgetSkinHandler::SetWidgetSkinFontHeight (
    WidgetSkin::FontType const font_type,
    ScreenCoord const font_height)
{
    SetProperty<
        WidgetSkin::FontType,
        ScreenCoord,
        &WidgetSkin::SetFontHeight,
        &WidgetSkinHandler::PropagateChangedFont>(
            font_type,
            font_height);
}

void WidgetSkinHandler::SetWidgetSkinTexture (
    WidgetSkin::TextureType const texture_type,
    Resource<GLTexture> const &texture)
{
    SetProperty<
        WidgetSkin::TextureType,
        Resource<GLTexture> const &,
        &WidgetSkin::SetTexture,
        &WidgetSkinHandler::PropagateChangedTexture>(
            texture_type,
            texture);
}

void WidgetSkinHandler::SetWidgetSkinTextureFilename (
    WidgetSkin::TextureType const texture_type,
    std::string const &texture_filename)
{
    SetProperty<
        WidgetSkin::TextureType,
        std::string const &,
        &WidgetSkin::SetTextureFilename,
        &WidgetSkinHandler::PropagateChangedTexture>(
            texture_type,
            texture_filename);
}

void WidgetSkinHandler::SetWidgetSkinMarginRatios (
    WidgetSkin::MarginsType const margin_type,
    FloatVector2 const &margin_ratios)
{
    SetProperty<
        WidgetSkin::MarginsType,
        FloatVector2 const &,
        &WidgetSkin::SetMarginRatios,
        &WidgetSkinHandler::PropagateChangedMargins>(
            margin_type,
            margin_ratios);
}

void WidgetSkinHandler::SetWidgetSkinMargins (
    WidgetSkin::MarginsType const margin_type,
    ScreenCoordVector2 const &margins)
{
    SetProperty<
        WidgetSkin::MarginsType,
        ScreenCoordVector2 const &,
        &WidgetSkin::SetMargins,
        &WidgetSkinHandler::PropagateChangedMargins>(
            margin_type,
            margins);
}

// ///////////////////////////////////////////////////////////////////////////
// these functions propagate property changes to child widgets
// ///////////////////////////////////////////////////////////////////////////

void WidgetSkinHandler::PropagateChangedWidgetBackground (
    WidgetSkin::WidgetBackgroundType const widget_background_type)
{
    PropagateChangedProperty<
        WidgetSkin::WidgetBackgroundType,
        &WidgetSkinHandler::HandleChangedWidgetSkinWidgetBackground>(
            widget_background_type);
}

void WidgetSkinHandler::PropagateChangedFont (
    WidgetSkin::FontType const font_type)
{
    PropagateChangedProperty<
        WidgetSkin::FontType,
        &WidgetSkinHandler::HandleChangedWidgetSkinFont>(
            font_type);
}

void WidgetSkinHandler::PropagateChangedTexture (
    WidgetSkin::TextureType const texture_type)
{
    PropagateChangedProperty<
        WidgetSkin::TextureType,
        &WidgetSkinHandler::HandleChangedWidgetSkinTexture>(
            texture_type);
}

void WidgetSkinHandler::PropagateChangedMargins (
    WidgetSkin::MarginsType const margins_type)
{
    PropagateChangedProperty<
        WidgetSkin::MarginsType,
        &WidgetSkinHandler::HandleChangedWidgetSkinMargins>(
            margins_type);
}

} // end of namespace Xrb
