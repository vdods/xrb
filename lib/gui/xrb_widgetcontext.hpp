// ///////////////////////////////////////////////////////////////////////////
// xrb_widgetcontext.hpp by Victor Dods, created 2011/09/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_WIDGETCONTEXT_HPP_)
#define _XRB_WIDGETCONTEXT_HPP_

#include "xrb.hpp"

#include <set>

#include "xrb_font.hpp"
#include "xrb_gltexture.hpp"
#include "xrb_margins.hpp"
#include "xrb_resourcelibrary.hpp"
#include "xrb_screencoord.hpp"
#include "xrb_stylesheet.hpp"

namespace Xrb {

class EventQueue;
class Screen;
class Widget;
class WidgetBackground;

/// A WidgetContext is a bit of (relatively) "global" context data which is
/// used in the creation of GUI widgets.  There is exactly one WidgetContext
/// per instance of Screen, and each Widget subordinate to that Screen gets
/// a reference tho that WidgetContext during creation.  This reference is
/// stored by Widget, and is publicly accessible.
///
/// It is used, for example, for knowing the screen size for picking font sizes.
/// Another use is during modal widget creation -- a modal widget is attached
/// to the Screen directly, since it appears above all other widgets.
///
/// Additionally, the widgets using each WidgetContext are tracked automatically,
/// and if there is an active Widget instance at the time the WidgetContext is
/// destroyed, it is considered an error, and a debug message indicating which
/// Widget will be printed to the console, and the program will assert.
///
/// INTERNAL NOTE: Widget handles WidgetContext via reference, which means that it
/// can't be reassigned.  A consequence of this is that once a Widget is
/// created using a particular WidgetContext, it can't be moved to a different
/// WidgetContext.  To change this would just require changing the reference to
/// a pointer.  However switching widgets between contexts should be a rare or
/// nonexistent event.
class WidgetContext
{
public:

    Screen const &GetScreen () const { return m_screen; }
    Screen &GetScreen () { return m_screen; }

    // there is exactly one EventQueue per WidgetContext, which serves as the event
    // queue for all widgets in that context.
    EventQueue const &GetEventQueue () const { ASSERT1(m_event_queue != NULL); return *m_event_queue; }
    EventQueue &GetEventQueue () { ASSERT1(m_event_queue != NULL); return *m_event_queue; }

    /// Frontend for Screen::SizeRatioBasis().  This gives the effective size of the
    /// screen, which the size ratios are based upon.
    ScreenCoord SizeRatioBasis () const;

    /// Accessor for converting from screen proportion value to pixel size value.
    ScreenCoord PixelSize (Float size_ratio) const;
    /// Accessor for converting from pixel size value to screen proportion value.
    Float SizeRatio (ScreenCoord pixel_size) const;
    /// Accessor for converting from [screen-proportion-based] margins ratios to [pixel-based] margins.
    ScreenCoordMargins MarginsFromRatios (FloatMargins const &ratios) const;
    /// Accessor for converting from [pixel-based] margins to [screen-proportion-based] margins ratios.
    FloatMargins RatiosFromMargins (ScreenCoordMargins const &margins) const;

    /// Returns the current StyleSheet for this context.  NULL if none.  If there
    /// is no StyleSheet, default values will be returned by the StyleSheet* accessors.
    StyleSheet const *GetStyleSheet () const { return m_style_sheet; }
    WidgetBackground const *StyleSheet_Background (std::string const &type) const;
    std::string const &StyleSheet_FontPath (std::string const &type) const;
    Float StyleSheet_FontHeightRatio (std::string const &type) const;
    ScreenCoord StyleSheet_FontPixelHeight (std::string const &type) const;
    Resource<GlTexture> const &StyleSheet_Texture (std::string const &type) const;
    FloatMargins const &StyleSheet_MarginsRatios (std::string const &type) const;
    ScreenCoordMargins StyleSheet_Margins (std::string const &type) const;

    /// Setting the StyleSheet will affect all Widgets associated to this WidgetContext.
    /// If NULL is provided, default values will be returned by the StyleSheet* accessors.
    /// If the owned style sheet is changed, StyleSheetWasChanged() should be called,
    /// not SetStyleSheet again.
    void SetStyleSheet (StyleSheet *style_sheet);
    /// Call this method if you change the context-owned StyleSheet -- the widgets
    /// associated with this context will be informed of the change.
    void StyleSheetWasChanged ();

private:

    // used only by Screen's constructor.
    WidgetContext (Screen &screen);
    // this MUST be destroyed before the Gl and ResourceLibrary singletons are shut down.
    ~WidgetContext ();
    
    // this MUST happen in Widget's constructor.
    void AddWidget (Widget &widget);
    // this MUST happen in Widget's destructor.
    void RemoveWidget (Widget &widget);

    // allows Screen to set the EventQueue.
    void SetEventQueue (EventQueue &event_queue) { m_event_queue = &event_queue; }

    typedef std::set<Widget *> WidgetSet;

    EventQueue *m_event_queue;
    Screen &m_screen;
    StyleSheet *m_style_sheet;
    WidgetSet m_widget_set;
    
    friend class Screen; // only Screen can construct/destruct one of these
    friend class Widget; // only Widget can call Add/RemoveWidget
}; // end of class WidgetContext

} // end of namespace Xrb

#endif // !defined(_XRB_WIDGETCONTEXT_HPP_)
