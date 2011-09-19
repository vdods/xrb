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

    enum WidgetBackgroundType
    {
        MODAL_WIDGET_BACKGROUND = 0,
        BUTTON_BACKGROUND,
        BUTTON_MOUSEOVER_BACKGROUND,
        BUTTON_PRESSED_BACKGROUND,
        LINE_EDIT_BACKGROUND,
        CHECK_BOX_BACKGROUND,
        RADIO_BUTTON_BACKGROUND,
        TOOLBAR_BUTTON_BACKGROUND,
        TOOLBAR_BUTTON_MOUSEOVER_BACKGROUND,
        TOOLBAR_BUTTON_CHECKED_BACKGROUND,
        TOOLBAR_BUTTON_PRESSED_BACKGROUND,

        WIDGET_BACKGROUND_TYPE_COUNT
    }; // end of enum WidgetSkin::WidgetBackgroundType

    enum FontType
    {
        DEFAULT_FONT = 0,

        FONT_TYPE_COUNT
    }; // end of enum WidgetSkin::FontType

    enum TextureType
    {
        CHECK_BOX_CHECK_TEXTURE = 0,
        RADIO_BUTTON_CHECK_TEXTURE,

        TEXTURE_TYPE_COUNT
    }; // end of enum WidgetSkin::TextureType

    enum MarginsType
    {
        DEFAULT_FRAME_MARGINS = 0,
        DEFAULT_CONTENT_MARGINS,
        LAYOUT_FRAME_MARGINS,
        LAYOUT_SPACING_MARGINS, // this only uses m_bottom_left
        CHECK_BOX_FRAME_MARGINS,
        RADIO_BUTTON_FRAME_MARGINS,

        MARGINS_TYPE_COUNT
    }; // end of enum WidgetSkin::MarginsType

    /** This constructor needs to be replaced by a static Create() function
      * which specifies some path of a widgetskin property descriptor file
      * and the constructor should be made protected.
      * @brief Constructs a WidgetSkin with a bunch of default values for now.
      */
    WidgetSkin ();
    /** Deletes all the widget backgrounds.  The resourced properties will
      * go out of scope and be automatically deleted here.
      * @brief Destructor.
      */
    ~WidgetSkin ();

    /// Populates this WidgetSkin with sensible default values.
    void PopulateUsingDefaults ();
    /// Used by whatever owns the WidgetSkin (e.g. Screen) to unload everything (e.g. before the Gl singleton dies)
    void ReleaseAllResources ();

    /// This is the way to derive font [pixel] heights from height ratios.
    static ScreenCoord FontPixelHeight (Float font_height_ratio, ScreenCoord size_ratio_basis);
    /// This is the way to derive height ratios from font [pixel] heights.
    static Float FontHeightRatio (ScreenCoord font_pixel_height, ScreenCoord size_ratio_basis);

    /// Returns a pointer to the const WidgetBackground object of the requested type.
    WidgetBackground const *GetWidgetBackground (WidgetBackgroundType widget_background_type) const
    {
        ASSERT1(widget_background_type < WIDGET_BACKGROUND_TYPE_COUNT);
        return m_widget_background[widget_background_type];
    }
    /// Returns the font path for the given font type.
    std::string const &FontPath (FontType font_type) const
    {
        ASSERT1(font_type < FONT_TYPE_COUNT);
        return m_font_specification[font_type].m_path;
    }
    /// Returns the font height ratio for the given font type.
    Float FontHeightRatio (FontType font_type) const
    {
        ASSERT1(font_type < FONT_TYPE_COUNT);
        return m_font_specification[font_type].m_height_ratio;
    }
    /// Returns the const resourced GlTexture object of the requested type.
    Resource<GlTexture> const &GetTexture (TextureType texture_type) const
    {
        ASSERT1(texture_type < TEXTURE_TYPE_COUNT);
        return m_texture[texture_type];
    }
    /// Returns a screen coordinate vector containing the requested margins type.
    FloatMargins MarginsRatios (MarginsType margins_type) const
    {
        ASSERT1(margins_type < MARGINS_TYPE_COUNT);
        return m_margins_ratios[margins_type];
    }

    /// Sets the given type of widget background.  Specifying NULL will cause the WidgetBackground
    /// to be entirely transparent.  This WidgetSkin takes ownership of the given pointer.  The
    /// background being replaced is deleted, if it exists.
    void SetWidgetBackground (WidgetBackgroundType widget_background_type, WidgetBackground const *widget_background);
    /// Sets the given type of font using the given font face.
    void SetFontPath (FontType font_type, std::string const &font_path);
    /// Sets the screen size-ratio-basis height ratio of the specified font type.
    void SetFontHeightRatio (FontType font_type, Float font_height_ratio);
    /// Sets the given type of texture.
    void SetTexture (TextureType texture_type, Resource<GlTexture> const &texture);
    /// Sets the screen size-ratio-basis margins ratios of the specified type.
    void SetMarginsRatios (MarginsType margins_type, FloatMargins const &margins_ratios);

private:

    struct FontSpecification
    {
        std::string m_path;
        Float m_height_ratio;

        FontSpecification () : m_height_ratio(0.0f) { }
    }; // end of struct WidgetSkin::FontSpecification

    WidgetBackground const *m_widget_background[WIDGET_BACKGROUND_TYPE_COUNT];
    FontSpecification m_font_specification[FONT_TYPE_COUNT];
    Resource<GlTexture> m_texture[TEXTURE_TYPE_COUNT];
    FloatMargins m_margins_ratios[MARGINS_TYPE_COUNT];
}; // end of class WidgetSkin

} // end of namespace Xrb

#endif // !defined(_XRB_WIDGETSKIN_HPP_)
