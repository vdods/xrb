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
#include "xrb_util.hpp"
#include "xrb_widgetbackground.hpp"

namespace Xrb {

std::string const WidgetSkin::BackgroundType::MODAL_WIDGET("ModalWidget");
std::string const WidgetSkin::BackgroundType::BUTTON_IDLE("Button:idle");
std::string const WidgetSkin::BackgroundType::BUTTON_MOUSEOVER("Button:mouseover");
std::string const WidgetSkin::BackgroundType::BUTTON_PRESSED("Button:pressed");
std::string const WidgetSkin::BackgroundType::LINE_EDIT("LineEdit");
std::string const WidgetSkin::BackgroundType::CHECK_BOX("CheckBox");
std::string const WidgetSkin::BackgroundType::RADIO_BUTTON("RadioButton");
std::string const WidgetSkin::BackgroundType::TOOLBAR_BUTTON_IDLE("ToolbarButton:idle");
std::string const WidgetSkin::BackgroundType::TOOLBAR_BUTTON_MOUSEOVER("ToolbarButton:mouseover");
std::string const WidgetSkin::BackgroundType::TOOLBAR_BUTTON_CHECKED("ToolbarButton:checked");
std::string const WidgetSkin::BackgroundType::TOOLBAR_BUTTON_PRESSED("ToolbarButton:pressed");

std::string const WidgetSkin::FontType::DEFAULT("default");

std::string const WidgetSkin::TextureType::CHECK_BOX("CheckBox");
std::string const WidgetSkin::TextureType::RADIO_BUTTON("RadioButton");

std::string const WidgetSkin::MarginsType::DEFAULT_FRAME("default:frame");
std::string const WidgetSkin::MarginsType::DEFAULT_CONTENT("default:content");
std::string const WidgetSkin::MarginsType::LAYOUT_FRAME("Layout:frame");
std::string const WidgetSkin::MarginsType::LAYOUT_SPACING("Layout:spacing");
std::string const WidgetSkin::MarginsType::CHECK_BOX_FRAME("CheckBox:frame");
std::string const WidgetSkin::MarginsType::RADIO_BUTTON_FRAME("RadioButton:frame");

WidgetBackground const *const WidgetSkin::ms_fallback_background = NULL; // no background (transparent)
std::string const WidgetSkin::ms_fallback_font_path("resources/FreeSansBoldCustom.ttf"); // TODO: built-in font
// 0.03 looks like about the smallest font that appears decently on a 640x480 screen.
Float const WidgetSkin::ms_fallback_font_height_ratio = 0.03f;
Resource<GlTexture> const &WidgetSkin::ms_fallback_texture(Resource<GlTexture>::ms_invalid); // invalid/blank texture
FloatMargins const &WidgetSkin::ms_fallback_margins_ratios(FloatMargins::ms_zero); // zeroed margins

WidgetSkin::WidgetSkin ()
{
    // empty by default
}

WidgetSkin::~WidgetSkin ()
{
    // in case it didn't already happen.
    ReleaseAllResources();
}

void WidgetSkin::PopulateUsingDefaults (std::string const &font_path_override)
{
    // in case there was stuff in here already
    ReleaseAllResources();
    
    // ///////////////////////////////////////////////////////////////////////
    // widget backgrounds
    // ///////////////////////////////////////////////////////////////////////

    SetBackground(BackgroundType::MODAL_WIDGET,             new WidgetBackgroundColored(Color(0.0f, 0.0f, 0.0f, 1.0f)));
    SetBackground(BackgroundType::BUTTON_IDLE,              new WidgetBackgroundColored(Color(0.0f, 0.0f, 0.6f, 1.0f)));
    SetBackground(BackgroundType::BUTTON_MOUSEOVER,         new WidgetBackgroundColored(Color(0.2f, 0.2f, 0.7f, 1.0f)));
    SetBackground(BackgroundType::BUTTON_PRESSED,           new WidgetBackgroundColored(Color(0.4f, 0.4f, 0.8f, 1.0f)));
    SetBackground(BackgroundType::LINE_EDIT,                new WidgetBackgroundColored(Color(0.2f, 0.2f, 0.2f, 1.0f)));
    SetBackground(BackgroundType::CHECK_BOX,                new WidgetBackgroundColored(Color(0.9f, 0.8f, 0.0f, 1.0f)));
    SetBackground(BackgroundType::RADIO_BUTTON,             new WidgetBackgroundColored(Color(0.9f, 0.1f, 0.0f, 1.0f)));
    SetBackground(BackgroundType::TOOLBAR_BUTTON_IDLE,      new WidgetBackgroundColored(Color(0.0f, 0.4f, 0.0f, 1.0f)));
    SetBackground(BackgroundType::TOOLBAR_BUTTON_MOUSEOVER, new WidgetBackgroundColored(Color(0.4f, 0.8f, 0.4f, 1.0f)));
    SetBackground(BackgroundType::TOOLBAR_BUTTON_CHECKED,   new WidgetBackgroundColored(Color(0.2f, 0.6f, 0.2f, 1.0f)));
    SetBackground(BackgroundType::TOOLBAR_BUTTON_PRESSED,   new WidgetBackgroundColored(Color(0.6f, 1.0f, 0.6f, 1.0f)));

    // ///////////////////////////////////////////////////////////////////////
    // fonts
    // ///////////////////////////////////////////////////////////////////////

    SetFont(FontType::DEFAULT, font_path_override, 0.023f);

    // ///////////////////////////////////////////////////////////////////////
    // gl textures
    // ///////////////////////////////////////////////////////////////////////

    SetTexture(TextureType::CHECK_BOX,    GlTexture::Load("resources/ui/black_checkmark.png")); // TODO: replace with procedurally generated texture
    SetTexture(TextureType::RADIO_BUTTON, GlTexture::Load("resources/ui/radiobutton_dot.png")); // TODO: replace with procedurally generated texture

    // ///////////////////////////////////////////////////////////////////////
    // margins
    // ///////////////////////////////////////////////////////////////////////

    SetMarginsRatios(MarginsType::DEFAULT_FRAME, FloatMargins(FloatVector2(0.006667f, 0.006667f)));
    SetMarginsRatios(MarginsType::DEFAULT_CONTENT, FloatMargins::ms_zero);
    SetMarginsRatios(MarginsType::LAYOUT_FRAME, FloatMargins(FloatVector2(0.006667f, 0.006667f)));
    SetMarginsRatios(MarginsType::LAYOUT_SPACING, FloatMargins(FloatVector2(0.006667f, 0.006667f)));
    SetMarginsRatios(MarginsType::CHECK_BOX_FRAME, FloatMargins::ms_zero);
    SetMarginsRatios(MarginsType::RADIO_BUTTON_FRAME, FloatMargins::ms_zero);
}

void WidgetSkin::ReleaseAllResources ()
{
    for (BackgroundMap::iterator it = m_background.begin(), it_end = m_background.end(); it != it_end; ++it)
        delete it->second;

    m_background.clear();
    m_font_spec.clear();
    m_texture.clear();
    m_margins_ratios.clear();
}

WidgetBackground const *WidgetSkin::Background (std::string const &type) const
{
    BackgroundMap::const_iterator it = m_background.find(Util::Lowercase(type));
    if (it == m_background.end())
    {
        fprintf(stderr, "WidgetSkin::Background(); WARNING: type \"%s\" not found.  using austere default.\n", type.c_str());
        return ms_fallback_background;
    }
    else
        return it->second;
}

std::string const &WidgetSkin::FontPath (std::string const &type) const
{
    FontSpecificationMap::const_iterator it = m_font_spec.find(Util::Lowercase(type));
    if (it == m_font_spec.end())
    {
        fprintf(stderr, "WidgetSkin::FontPath(); WARNING: type \"%s\" not found.  using austere default.\n", type.c_str());
        return ms_fallback_font_path;
    }
    else
        return it->second.m_path;
}

Float WidgetSkin::FontHeightRatio (std::string const &type) const
{
    FontSpecificationMap::const_iterator it = m_font_spec.find(Util::Lowercase(type));
    if (it == m_font_spec.end())
    {
        fprintf(stderr, "WidgetSkin::FontHeightRatio(); WARNING: type \"%s\" not found.  using austere default.\n", type.c_str());
        return ms_fallback_font_height_ratio;
    }
    else
        return it->second.m_height_ratio;
}

Resource<GlTexture> const &WidgetSkin::GetTexture (std::string const &type) const
{
    TextureMap::const_iterator it = m_texture.find(Util::Lowercase(type));
    if (it == m_texture.end())
    {
        fprintf(stderr, "WidgetSkin::GetTexture(); WARNING: type \"%s\" not found.  using austere default.\n", type.c_str());
        return ms_fallback_texture;
    }
    else
        return it->second;
}

FloatMargins WidgetSkin::MarginsRatios (std::string const &type) const
{
    MarginsRatiosMap::const_iterator it = m_margins_ratios.find(Util::Lowercase(type));
    if (it == m_margins_ratios.end())
    {
        fprintf(stderr, "WidgetSkin::MarginsRatios(); WARNING: type \"%s\" not found.  using austere default.\n", type.c_str());
        return ms_fallback_margins_ratios;
    }
    else
        return it->second;
}

void WidgetSkin::SetBackground (std::string const &type, WidgetBackground const *background)
{
    std::string lowercase_type(Util::Lowercase(type));
    BackgroundMap::iterator it = m_background.find(lowercase_type);
    if (it != m_background.end())
        delete it->second;
    m_background[lowercase_type] = background;
}

void WidgetSkin::SetFont (std::string const &type, std::string const &font_path, Float font_height_ratio)
{
    ASSERT1(font_height_ratio > 0.0f);
    m_font_spec[Util::Lowercase(type)] = FontSpec(font_path, font_height_ratio);
}

void WidgetSkin::SetTexture (std::string const &type, Resource<GlTexture> const &texture)
{
    m_texture[Util::Lowercase(type)] = texture;
}

void WidgetSkin::SetMarginsRatios (std::string const &type, FloatMargins const &margins_ratios)
{
    m_margins_ratios[Util::Lowercase(type)] = margins_ratios;
}

} // end of namespace Xrb
