// ///////////////////////////////////////////////////////////////////////////
// xrb_stylesheet.cpp by Victor Dods, created 2005/04/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_stylesheet.hpp"

#include "xrb_math.hpp"
#include "xrb_screen.hpp"
#include "xrb_util.hpp"
#include "xrb_widgetbackground.hpp"

namespace Xrb {

std::string const StyleSheet::BackgroundType::TRANSPARENT("transparent");
std::string const StyleSheet::BackgroundType::MODAL_WIDGET("ModalWidget");
std::string const StyleSheet::BackgroundType::BUTTON_IDLE("Button:idle");
std::string const StyleSheet::BackgroundType::BUTTON_MOUSEOVER("Button:mouseover");
std::string const StyleSheet::BackgroundType::BUTTON_PRESSED("Button:pressed");
std::string const StyleSheet::BackgroundType::LINE_EDIT("LineEdit");
std::string const StyleSheet::BackgroundType::CHECK_BOX_UNCHECKED("CheckBox:unchecked");
std::string const StyleSheet::BackgroundType::CHECK_BOX_MOUSEOVER("CheckBox:mouseover");
std::string const StyleSheet::BackgroundType::CHECK_BOX_PRESSED("CheckBox:pressed");
std::string const StyleSheet::BackgroundType::CHECK_BOX_CHECKED("CheckBox:checked");
std::string const StyleSheet::BackgroundType::RADIO_BUTTON_UNCHECKED("RadioButton:unchecked");
std::string const StyleSheet::BackgroundType::RADIO_BUTTON_MOUSEOVER("RadioButton:mouseover");
std::string const StyleSheet::BackgroundType::RADIO_BUTTON_PRESSED("RadioButton:pressed");
std::string const StyleSheet::BackgroundType::RADIO_BUTTON_CHECKED("RadioButton:checked");

std::string const StyleSheet::FontType::DEFAULT("default");

std::string const StyleSheet::MarginsType::ZERO("zero");
std::string const StyleSheet::MarginsType::DEFAULT_FRAME("default:frame");
std::string const StyleSheet::MarginsType::DEFAULT_CONTENT("default:content");
std::string const StyleSheet::MarginsType::LAYOUT_FRAME("Layout:frame");
std::string const StyleSheet::MarginsType::LAYOUT_SPACING("Layout:spacing");
std::string const StyleSheet::MarginsType::CHECK_BOX_FRAME("CheckBox:frame");
std::string const StyleSheet::MarginsType::RADIO_BUTTON_FRAME("RadioButton:frame");

WidgetBackground const *const StyleSheet::ms_fallback_background = NULL; // no background (transparent)
std::string const StyleSheet::ms_fallback_font_path("resources/FreeSansBoldCustom.ttf"); // TODO: built-in font
// 0.03 looks like about the smallest font that appears decently on a 640x480 screen.
Float const StyleSheet::ms_fallback_font_height_ratio = 0.03f;
Resource<GlTexture> const &StyleSheet::ms_fallback_texture(Resource<GlTexture>::ms_invalid); // invalid/blank texture
FloatMargins const &StyleSheet::ms_fallback_margins_ratios(FloatMargins::ms_zero); // zeroed margins

StyleSheet::StyleSheet ()
{
    // empty by default
}

StyleSheet::~StyleSheet ()
{
    // in case it didn't already happen.
    ReleaseAllResources();
}

void StyleSheet::PopulateUsingDefaults (std::string const &font_path_override)
{
    // in case there was stuff in here already
    ReleaseAllResources();
    
    // ///////////////////////////////////////////////////////////////////////
    // backgrounds
    // ///////////////////////////////////////////////////////////////////////

    SetBackground(BackgroundType::TRANSPARENT,              NULL); // NULL indicates no background (transparent)
    SetBackground(BackgroundType::MODAL_WIDGET,             new WidgetBackgroundColored(Color(0.0f, 0.0f, 0.0f, 1.0f)));
    SetBackground(BackgroundType::BUTTON_IDLE,              new WidgetBackgroundColored(Color(0.0f, 0.0f, 0.6f, 1.0f)));
    SetBackground(BackgroundType::BUTTON_MOUSEOVER,         new WidgetBackgroundColored(Color(0.2f, 0.2f, 0.7f, 1.0f)));
    SetBackground(BackgroundType::BUTTON_PRESSED,           new WidgetBackgroundColored(Color(0.4f, 0.4f, 0.8f, 1.0f)));
    SetBackground(BackgroundType::LINE_EDIT,                new WidgetBackgroundColored(Color(0.2f, 0.2f, 0.2f, 1.0f)));
    SetBackground(BackgroundType::CHECK_BOX_UNCHECKED,      new WidgetBackgroundColored(Color(0.0f, 0.0f, 0.4f, 1.0f)));
    SetBackground(BackgroundType::CHECK_BOX_MOUSEOVER,      new WidgetBackgroundColored(Color(0.4f, 0.4f, 0.8f, 1.0f)));
    SetBackground(BackgroundType::CHECK_BOX_PRESSED,        new WidgetBackgroundColored(Color(0.6f, 0.6f, 1.0f, 1.0f)));
    SetBackground(BackgroundType::CHECK_BOX_CHECKED,        new WidgetBackgroundColored(Color(0.2f, 0.2f, 0.6f, 1.0f)));
    SetBackground(BackgroundType::RADIO_BUTTON_UNCHECKED,   new WidgetBackgroundColored(Color(0.0f, 0.4f, 0.0f, 1.0f)));
    SetBackground(BackgroundType::RADIO_BUTTON_MOUSEOVER,   new WidgetBackgroundColored(Color(0.4f, 0.8f, 0.4f, 1.0f)));
    SetBackground(BackgroundType::RADIO_BUTTON_PRESSED,     new WidgetBackgroundColored(Color(0.6f, 1.0f, 0.6f, 1.0f)));
    SetBackground(BackgroundType::RADIO_BUTTON_CHECKED,     new WidgetBackgroundColored(Color(0.2f, 0.6f, 0.2f, 1.0f)));

    // ///////////////////////////////////////////////////////////////////////
    // fonts
    // ///////////////////////////////////////////////////////////////////////

    SetFont(FontType::DEFAULT, font_path_override, 0.023f);

    // ///////////////////////////////////////////////////////////////////////
    // textures
    // ///////////////////////////////////////////////////////////////////////


    // ///////////////////////////////////////////////////////////////////////
    // margins
    // ///////////////////////////////////////////////////////////////////////

    SetMarginsRatios(MarginsType::ZERO, FloatMargins::ms_zero);
    SetMarginsRatios(MarginsType::DEFAULT_FRAME, FloatMargins(FloatVector2(0.006667f, 0.006667f)));
    SetMarginsRatios(MarginsType::DEFAULT_CONTENT, FloatMargins::ms_zero);
    SetMarginsRatios(MarginsType::LAYOUT_FRAME, FloatMargins(FloatVector2(0.006667f, 0.006667f)));
    SetMarginsRatios(MarginsType::LAYOUT_SPACING, FloatMargins(FloatVector2(0.006667f, 0.006667f)));
    SetMarginsRatios(MarginsType::CHECK_BOX_FRAME, FloatMargins::ms_zero);
    SetMarginsRatios(MarginsType::RADIO_BUTTON_FRAME, FloatMargins::ms_zero);
}

void StyleSheet::ReleaseAllResources ()
{
    for (BackgroundMap::iterator it = m_background.begin(), it_end = m_background.end(); it != it_end; ++it)
        delete it->second;

    m_background.clear();
    m_font_spec.clear();
    m_texture.clear();
    m_margins_ratios.clear();
}

WidgetBackground const *StyleSheet::Background (std::string const &type) const
{
    BackgroundMap::const_iterator it = m_background.find(Util::Lowercase(type));
    if (it == m_background.end())
    {
        fprintf(stderr, "StyleSheet::Background(); WARNING: type \"%s\" not found.  using austere default.\n", type.c_str());
        return ms_fallback_background;
    }
    else
        return it->second;
}

std::string const &StyleSheet::FontPath (std::string const &type) const
{
    FontSpecificationMap::const_iterator it = m_font_spec.find(Util::Lowercase(type));
    if (it == m_font_spec.end())
    {
        fprintf(stderr, "StyleSheet::FontPath(); WARNING: type \"%s\" not found.  using austere default.\n", type.c_str());
        return ms_fallback_font_path;
    }
    else
        return it->second.m_path;
}

Float StyleSheet::FontHeightRatio (std::string const &type) const
{
    FontSpecificationMap::const_iterator it = m_font_spec.find(Util::Lowercase(type));
    if (it == m_font_spec.end())
    {
        fprintf(stderr, "StyleSheet::FontHeightRatio(); WARNING: type \"%s\" not found.  using austere default.\n", type.c_str());
        return ms_fallback_font_height_ratio;
    }
    else
        return it->second.m_height_ratio;
}

Resource<GlTexture> const &StyleSheet::GetTexture (std::string const &type) const
{
    TextureMap::const_iterator it = m_texture.find(Util::Lowercase(type));
    if (it == m_texture.end())
    {
        fprintf(stderr, "StyleSheet::GetTexture(); WARNING: type \"%s\" not found.  using austere default.\n", type.c_str());
        return ms_fallback_texture;
    }
    else
        return it->second;
}

FloatMargins const &StyleSheet::MarginsRatios (std::string const &type) const
{
    MarginsRatiosMap::const_iterator it = m_margins_ratios.find(Util::Lowercase(type));
    if (it == m_margins_ratios.end())
    {
        fprintf(stderr, "StyleSheet::MarginsRatios(); WARNING: type \"%s\" not found.  using austere default.\n", type.c_str());
        return ms_fallback_margins_ratios;
    }
    else
        return it->second;
}

void StyleSheet::SetBackground (std::string const &type, WidgetBackground const *background)
{
    std::string lowercase_type(Util::Lowercase(type));
    BackgroundMap::iterator it = m_background.find(lowercase_type);
    if (it != m_background.end())
        delete it->second;
    m_background[lowercase_type] = background;
}

void StyleSheet::SetFont (std::string const &type, std::string const &font_path, Float font_height_ratio)
{
    ASSERT1(font_height_ratio > 0.0f);
    m_font_spec[Util::Lowercase(type)] = FontSpec(font_path, font_height_ratio);
}

void StyleSheet::SetTexture (std::string const &type, Resource<GlTexture> const &texture)
{
    m_texture[Util::Lowercase(type)] = texture;
}

void StyleSheet::SetMarginsRatios (std::string const &type, FloatMargins const &margins_ratios)
{
    m_margins_ratios[Util::Lowercase(type)] = margins_ratios;
}

} // end of namespace Xrb
