// ///////////////////////////////////////////////////////////////////////////
// xrb_widgetcontext.cpp by Victor Dods, created 2011/09/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_widgetcontext.hpp"

#include "xrb_eventqueue.hpp"
#include "xrb_math.hpp"
#include "xrb_screen.hpp"

namespace Xrb {

ScreenCoord WidgetContext::SizeRatioBasis () const
{
    return m_screen.SizeRatioBasis();
}

ScreenCoord WidgetContext::PixelSize (Float size_ratio) const
{
    ASSERT1(m_screen.SizeRatioBasis() > 0);
    return ScreenCoord(Math::Round(size_ratio * m_screen.SizeRatioBasis()));
}

Float WidgetContext::SizeRatio (ScreenCoord pixel_size) const
{
    ASSERT1(m_screen.SizeRatioBasis() > 0);
    Float size_ratio = Float(pixel_size) / m_screen.SizeRatioBasis();
    ASSERT1(PixelSize(size_ratio) == pixel_size);
    return size_ratio;
}

WidgetBackground const *WidgetContext::WidgetSkin_Background (std::string const &type) const
{
    ASSERT1(WidgetSkin::ms_fallback_background == NULL && "allocation worries are necessary, since Widget deletes backgrounds");
    if (m_widget_skin == NULL)
        return WidgetSkin::ms_fallback_background;
    else
        return m_widget_skin->Background(type);
}

std::string const &WidgetContext::WidgetSkin_FontPath (std::string const &type) const
{
    if (m_widget_skin == NULL)
        return WidgetSkin::ms_fallback_font_path;
    else
        return m_widget_skin->FontPath(type);
}

Float WidgetContext::WidgetSkin_FontHeightRatio (std::string const &type) const
{
    if (m_widget_skin == NULL)
        return WidgetSkin::ms_fallback_font_height_ratio;
    else
        return m_widget_skin->FontHeightRatio(type);
}

ScreenCoord WidgetContext::WidgetSkin_FontPixelHeight (std::string const &type) const
{
    return PixelSize(WidgetSkin_FontHeightRatio(type));
}

Resource<Font> WidgetContext::WidgetSkin_LoadFont (std::string const &type) const
{
    return Font::Load(WidgetSkin_FontPath(type), WidgetSkin_FontPixelHeight(type));
}

Resource<GlTexture> const &WidgetContext::WidgetSkin_Texture (std::string const &type) const
{
    if (m_widget_skin == NULL)
        return WidgetSkin::ms_fallback_texture;
    else
        return m_widget_skin->GetTexture(type);
}

FloatMargins WidgetContext::WidgetSkin_MarginsRatios (std::string const &type) const
{
    if (m_widget_skin == NULL)
        return WidgetSkin::ms_fallback_margins_ratios;
    else
        return m_widget_skin->MarginsRatios(type);
}

ScreenCoordMargins WidgetContext::WidgetSkin_Margins (std::string const &type) const
{
    FloatMargins margins_ratios(WidgetSkin_MarginsRatios(type));
    return ScreenCoordMargins(
        ScreenCoordVector2(
            PixelSize(margins_ratios.m_bottom_left[Dim::X]),
            PixelSize(margins_ratios.m_bottom_left[Dim::Y])),
        ScreenCoordVector2(
            PixelSize(margins_ratios.m_top_right[Dim::X]),
            PixelSize(margins_ratios.m_top_right[Dim::Y])));
}

void WidgetContext::SetWidgetSkin (WidgetSkin *widget_skin)
{
    // delete the old widget skin.
    delete m_widget_skin;
    m_widget_skin = widget_skin;
    WidgetSkinWasChanged();
}

void WidgetContext::WidgetSkinWasChanged ()
{
    // update m_screen and all the widgets in this context.
    m_screen.HandleChangedWidgetSkin();
    for (WidgetSet::iterator it = m_widget_set.begin(), it_end = m_widget_set.end(); it != it_end; ++it)
        (*it)->HandleChangedWidgetSkin();
}

WidgetContext::WidgetContext (Screen &screen)
    :
    m_screen(screen),
    m_widget_skin(NULL)
{
    m_event_queue = new EventQueue();
}

WidgetContext::~WidgetContext ()
{
    if (!m_widget_set.empty())
        fprintf(stderr, "WidgetContext::~WidgetContext(); DANGLING WIDGETS (m_screen \"%s\"):\n", m_screen.Name().c_str());
    for (WidgetSet::iterator it = m_widget_set.begin(), it_end = m_widget_set.end(); it != it_end; ++it)
        fprintf(stderr, "\t\"%s\"\n", (*it)->Name().c_str());
    ASSERT1(m_widget_set.empty() && "there are dangling widgets");

    DeleteAndNullify(m_widget_skin);
    DeleteAndNullify(m_event_queue);
}

void WidgetContext::AddWidget (Widget &widget)
{
    if (&widget == &m_screen)
        return; // don't bother adding the Screen.
    ASSERT1(m_widget_set.find(&widget) == m_widget_set.end() && "this widget is already added");
    m_widget_set.insert(&widget);
}

void WidgetContext::RemoveWidget (Widget &widget)
{
    ASSERT1(&widget != &m_screen); // this should be prevented by Widget::m_remove_from_widget_context_upon_destruction.
    ASSERT1(m_widget_set.find(&widget) != m_widget_set.end() && "this widget was never added");
    m_widget_set.erase(&widget);
}

} // end of namespace Xrb
