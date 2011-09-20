// ///////////////////////////////////////////////////////////////////////////
// xrb_widgetskin.hpp by Victor Dods, created 2005/04/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_WIDGETSKIN_HPP_)
#define _XRB_WIDGETSKIN_HPP_

#include "xrb.hpp"

#include <map>

#include "xrb_font.hpp"
#include "xrb_gltexture.hpp"
#include "xrb_margins.hpp"
#include "xrb_resourcelibrary.hpp"
#include "xrb_screencoord.hpp"

namespace Xrb {

class Screen;
class WidgetBackground;

/// See @ref section_widget_skinning "Widget Skinning" for more details.
/// @brief Container class for widget skinning data.
class WidgetSkin
{
public:

    /// This struct provides a "namespace" for the base background type strings.
    struct BackgroundType
    {
        static std::string const MODAL_WIDGET;
        static std::string const BUTTON_IDLE;
        static std::string const BUTTON_MOUSEOVER;
        static std::string const BUTTON_PRESSED;
        static std::string const LINE_EDIT;
        static std::string const CHECK_BOX;
        static std::string const RADIO_BUTTON;
        static std::string const TOOLBAR_BUTTON_IDLE;
        static std::string const TOOLBAR_BUTTON_MOUSEOVER;
        static std::string const TOOLBAR_BUTTON_CHECKED;
        static std::string const TOOLBAR_BUTTON_PRESSED;
    }; // end of struct WidgetSkin::BackgroundType

    /// This struct provides a "namespace" for the base font type strings.
    struct FontType
    {
        static std::string const DEFAULT;
    }; // end of struct WidgetSkin::FontType

    /// This struct provides a "namespace" for the base texture type strings.
    struct TextureType
    {
        static std::string const CHECK_BOX;
        static std::string const RADIO_BUTTON;
    }; // end of struct WidgetSkin::TextureType

    /// This struct provides a "namespace" for the base margins type strings.
    struct MarginsType
    {
        static std::string const DEFAULT_FRAME;
        static std::string const DEFAULT_CONTENT;
        static std::string const LAYOUT_FRAME;
        static std::string const LAYOUT_SPACING; // this only uses m_bottom_left
        static std::string const CHECK_BOX_FRAME;
        static std::string const RADIO_BUTTON_FRAME;
    }; // end of struct WidgetSkin::MarginsType

    static WidgetBackground const *const ms_fallback_background;
    static std::string const ms_fallback_font_path;
    static Float const ms_fallback_font_height_ratio;
    static Resource<GlTexture> const &ms_fallback_texture;
    static FloatMargins const &ms_fallback_margins_ratios;

    /// Constructs a WidgetSkin with a bunch of really default/austere properties.
    WidgetSkin ();
    /// Deletes all the widget backgrounds.  The resourced properties will go out of scope and be automatically deleted here.
    ~WidgetSkin ();

    /// Populates this WidgetSkin with sensible default values.
    void PopulateUsingDefaults (std::string const &font_path_override = ms_fallback_font_path);
    /// Used by whatever owns the WidgetSkin (e.g. Screen) to unload everything (e.g. before the Gl singleton dies)
    void ReleaseAllResources ();

    /// This is the way to derive font [pixel] heights from height ratios.
    static ScreenCoord FontPixelHeight (Float font_height_ratio, ScreenCoord size_ratio_basis);
    /// This is the way to derive height ratios from font [pixel] heights.
    static Float FontHeightRatio (ScreenCoord font_pixel_height, ScreenCoord size_ratio_basis);

    /// Returns a pointer to the const WidgetBackground object of the requested type.
    /// @note The type string is case-insensitive.
    WidgetBackground const *Background (std::string const &type) const;
    /// Returns the font path for the given font type.
    /// @note The type string is case-insensitive.
    std::string const &FontPath (std::string const &type) const;
    /// Returns the font height ratio for the given font type.
    /// @note The type string is case-insensitive.
    Float FontHeightRatio (std::string const &type) const;
    /// Returns the const resourced GlTexture object of the requested type.
    /// @note The type string is case-insensitive.
    Resource<GlTexture> const &GetTexture (std::string const &type) const;
    /// Returns a screen coordinate vector containing the requested margins type.
    /// @note The type string is case-insensitive.
    FloatMargins MarginsRatios (std::string const &type) const;

    /// Sets the given type of widget background.  Specifying NULL will cause the WidgetBackground
    /// to be entirely transparent.  This WidgetSkin takes ownership of the given pointer.  The
    /// background being replaced is deleted, if it exists.
    /// @note The type string is case-insensitive.
    void SetBackground (std::string const &type, WidgetBackground const *background);
    /// Sets the given type of font using the given font face.
    /// @note The type string is case-insensitive.
    void SetFont (std::string const &type, std::string const &font_path, Float font_height_ratio);
    /// Sets the given type of texture.
    /// @note The type string is case-insensitive.
    void SetTexture (std::string const &type, Resource<GlTexture> const &texture);
    /// Sets the screen size-ratio-basis margins ratios of the specified type.
    /// @note The type string is case-insensitive.
    void SetMarginsRatios (std::string const &type, FloatMargins const &margins_ratios);

private:

    struct FontSpec
    {
        std::string m_path;
        Float m_height_ratio;

        FontSpec () : m_height_ratio(0.0f) { }
        FontSpec (std::string const &path, Float height_ratio) : m_path(path), m_height_ratio(height_ratio) { }
    }; // end of struct WidgetSkin::FontSpec

    typedef std::map<std::string, WidgetBackground const *> BackgroundMap;
    typedef std::map<std::string, FontSpec> FontSpecificationMap;
    typedef std::map<std::string, Resource<GlTexture> > TextureMap;
    typedef std::map<std::string, FloatMargins> MarginsRatiosMap;

    BackgroundMap m_background;
    FontSpecificationMap m_font_spec;
    TextureMap m_texture;
    MarginsRatiosMap m_margins_ratios;
}; // end of class WidgetSkin

} // end of namespace Xrb

#endif // !defined(_XRB_WIDGETSKIN_HPP_)
