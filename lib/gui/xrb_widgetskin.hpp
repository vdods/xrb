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
/// @brief (soon-to-be) Resource-geared container class for widget skinning data.
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

    /** @brief Used by whatever owns the WidgetSkin (e.g. Screen) to
      *        unload everything (e.g. before the Gl singleton dies)
      */
    void ReleaseAllResources ();

    /// This is the way to derive font [pixel] heights from height ratios.
    static ScreenCoord FontPixelHeight (Float font_height_ratio, ScreenCoord size_ratio_basis);
    /// This is the way to derive height ratios from font [pixel] heights.
    static Float FontHeightRatio (ScreenCoord font_pixel_height, ScreenCoord size_ratio_basis);

    /** @brief Returns a pointer to the const WidgetBackground object of the
      *        requested type.
      * @param widget_background_type The specific type of background to return.
      */
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
    /// Loads the requested font type based on the given size_ratio_basis.
    Resource<Font> LoadFont (FontType font_type, ScreenCoord size_ratio_basis) const;
    /** @brief Returns the const resourced GlTexture object of the requested type.
      * @param texture_type The specific type of texture to return.
      */
    Resource<GlTexture> const &GetTexture (TextureType texture_type) const
    {
        ASSERT1(texture_type < TEXTURE_TYPE_COUNT);
        return m_texture[texture_type];
    }
    /** @brief Returns a screen coordinate vector containing the requested margins type.
      * @param margins_type The specific type of margins to return.
      */
    ScreenCoordMargins Margins (MarginsType margins_type, ScreenCoord size_ratio_basis) const;

    /** The background being replaced is deleted, if it exists.
      * @brief Sets the given type of widget background.
      * @param widget_background_type The background type to be set.
      * @param widget_background A pointer to the WidgetBackground to use.
      */
    void SetWidgetBackground (WidgetBackgroundType widget_background_type, WidgetBackground const *widget_background);
    /** @brief Sets the given type of font using the given font face.
      * @param font_type The font type to change.
      * @param font_path The path to the font face to use.
      */
    void SetFontPath (FontType font_type, std::string const &font_path);
    /** @brief Sets the screen size-ratio-basis height ratio of the specified font type.
      * @param font_type The font type to be changed.
      * @param font_height_ratio The height ratio to set.
      */
    void SetFontHeightRatio (FontType font_type, Float font_height_ratio);
    /** @brief Sets the given type of texture.
      * @param texture_type The texture type to be set.
      * @param texture The resourced GlTexture object to use.
      */
    void SetTexture (TextureType texture_type, Resource<GlTexture> const &texture);
    /** @brief Sets the screen size-ratio-basis margins ratios of the specified type.
      * @param margins_type The type of margins to change.
      * @param margins_ratios The margin ratios to be set.
      */
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
