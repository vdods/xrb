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
#include "xrb_resourcelibrary.hpp"
#include "xrb_screencoord.hpp"

namespace Xrb
{

class Screen;
class WidgetBackground;

/** See @ref section_widget_skinning "Widget Skinning" for more details.
  * @brief Stores widget skin properties for use in consistent and convenient
  *        GUI skinning.
  */
class WidgetSkin
{
public:

    /** @brief Enumerates all the different specific backgrounds a WidgetSkin
      *        stores.
      */
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

    /** @brief Enumerates all the different specific fonts a WidgetSkin
      *        stores.
      */
    enum FontType
    {
        DEFAULT_FONT = 0,

        FONT_TYPE_COUNT
    }; // end of enum WidgetSkin::FontType

    /** @brief Enumerates all the different specific textures a WidgetSkin
      *        stores.
      */
    enum TextureType
    {
        CHECK_BOX_CHECK_TEXTURE = 0,
        RADIO_BUTTON_CHECK_TEXTURE,

        TEXTURE_TYPE_COUNT
    }; // end of enum WidgetSkin::TextureType

    /** @brief Enumerates all the different specific margin types a WidgetSkin
      *        stores.
      */
    enum MarginsType
    {
        DEFAULT_FRAME_MARGINS = 0,
        DEFAULT_CONTENT_MARGINS,
        LAYOUT_FRAME_MARGINS,
        LAYOUT_SPACING_MARGINS,

        MARGINS_TYPE_COUNT
    }; // end of enum WidgetSkin::MarginsType

    /** This constructor needs to be replaced by a static Create() function
      * which specifies some filename of a widgetskin property descriptor file
      * and the constructor should be made protected.
      * @brief Constructs a WidgetSkin with a bunch of default values for now.
      */
    WidgetSkin (Screen const *screen);
    /** Deletes all the widget backgrounds.  The resourced properties will
      * go out of scope and be automatically deleted here.
      * @brief Destructor.
      */
    ~WidgetSkin ();

    /** The contents of this object are cloned as well.
      * @brief Creates a newly instanced clone of this WidgetSkin object.
      */
    WidgetSkin *CreateClone () const;

    /** @brief Converts a screen coordinate size into a size-ratio-basis
      *        ratio, using the Screen object associated with this WidgetSkin.
      * @param screen_coord The screen coordinate to convert.
      */
    Float GetRatioFromScreenCoord (ScreenCoord screen_coord) const;
    /** @brief Converts a size-ratio-basis ratio into a screen coordinate
      *        size, using the Screen object associated with this WidgetSkin.
      * @param ratio The ratio to convert.
      */
    ScreenCoord GetScreenCoordFromRatio (Float ratio) const;
    /** @brief Converts a vector of screen coordinate sizes into a vector of
      *        size-ratio-basis ratios, using the Screen object associated
      *        with this WidgetSkin.
      * @param screen_coords The screen coordinate vector to convert.
      */
    FloatVector2 GetRatiosFromScreenCoords (
        ScreenCoordVector2 const &screen_coords) const;
    /** @brief Converts a vector of size-ratio-basis ratios into a vector of
      *        screen coordinate sizes, using the Screen object associated
      *        with this WidgetSkin.
      * @param ratios The ratio vector to convert.
      */
    ScreenCoordVector2 GetScreenCoordsFromRatios (
        FloatVector2 const &ratios) const;

    /** @brief Returns a pointer to the const WidgetBackground object of the
      *        requested type.
      * @param widget_background_type The specific type of background to return.
      */
    inline WidgetBackground const *GetWidgetBackground (
        WidgetBackgroundType const widget_background_type) const
    {
        ASSERT1(widget_background_type < WIDGET_BACKGROUND_TYPE_COUNT);
        return m_widget_background[widget_background_type];
    }
    /** @brief Returns the const resourced Font object of the requested type.
      * @param font_type The specific type of font to return.
      */
    inline Resource<Font> const &GetFont (FontType const font_type) const
    {
        ASSERT1(font_type < FONT_TYPE_COUNT);
        return m_font_specification[font_type].m_font;
    }
    /** @brief Returns the const resourced GLTexture object of the requested
      *        type.
      * @param texture_type The specific type of texture to return.
      */
    inline Resource<GLTexture> const &GetTexture (
        TextureType const texture_type) const
    {
        ASSERT1(texture_type < TEXTURE_TYPE_COUNT);
        return m_texture[texture_type];
    }
    /** @brief Returns a screen coordinate vector containing the requested
      *        margins type.
      * @param margins_type The specific type of margins to return.
      */
    inline ScreenCoordVector2 const &Margins (MarginsType const margins_type) const
    {
        ASSERT1(margins_type < MARGINS_TYPE_COUNT);
        return m_margins_specification[margins_type].m_margins;
    }

    /** The background being replaced is deleted, if it exists.
      * @brief Sets the given type of widget background.
      * @param widget_background_type The background type to be set.
      * @param widget_background A pointer to the WidgetBackground to use.
      */
    void SetWidgetBackground (
        WidgetBackgroundType widget_background_type,
        WidgetBackground const *widget_background);
    /** @brief Sets the given type of font.
      * @param font_type The font type to be set.
      * @param font The resourced Font object to use.
      */
    void SetFont (
        FontType font_type,
        Resource<Font> const &font);
    /** Uses the font pixel height from the previous font.
      * @brief Sets the given type of font using the given font face.
      * @param font_type The font type to change.
      * @param font_face_filename The filename of the font face to use.
      */
    void SetFontFaceFilename (
        FontType font_type,
        std::string const &font_face_filename);
    /** Does not change the given type's existing font face.
      * @brief Sets the screen size-ratio-basis height ratio of the
      *        specified font type.
      * @param font_type The font type to be changed.
      * @param font_height_ratio The height ratio to set.
      */
    void SetFontHeightRatio (
        FontType font_type,
        Float font_height_ratio);
    /** Does not change the given type's existing font face.
      * @brief Sets the pixel height of the specified font type.
      * @param font_type The font type to be changed.
      * @param font_height The pixel height to be set.
      */
    void SetFontHeight (
        FontType font_type,
        ScreenCoord font_height);
    /** @brief Sets the given type of texture.
      * @param texture_type The texture type to be set.
      * @param texture The resourced GLTexture object to use.
      */
    void SetTexture (
        TextureType texture_type,
        Resource<GLTexture> const &texture);
    /** @brief Sets the given type of texture.
      * @param texture_type The texture type to be set.
      * @param texture The filename of the texture to use.
      */
    void SetTextureFilename (
        TextureType texture_type,
        std::string const &texture_filename);
    /** @brief Sets the screen size-ratio-basis margin ratios of the
      *        specified type.
      * @param margins_type The type of margins to change.
      * @param margin_ratios The margin ratios to be set.
      */
    void SetMarginRatios (
        MarginsType margins_type,
        FloatVector2 const &margin_ratios);
    /** @brief Sets the pixel-based margins of the specified type.
      * @param margins_type The type of margins to change.
      * @param margin The margin sizes to be set.
      */
    void SetMargins (
        MarginsType margins_type,
        ScreenCoordVector2 const &margins);

    // TODO: some function that updates stuff when the screen res changes

protected:

    /** This should be deprecated once Create is made.
      * @brief This is used by CreateClone().
      */
    inline WidgetSkin () { }

private:

    void UpdateFontHeight (FontType font_type);
    void UpdateMargins (MarginsType margins_type);

    struct FontSpecification
    {
        Resource<Font> m_font;
        Float m_font_height_ratio;
        ScreenCoord m_font_height;

        inline FontSpecification ()
        {
            m_font_height_ratio = 0.0f;
            m_font_height = 0;
        }
    }; // end of struct WidgetSkin::FontSpecification

    struct MarginsSpecification
    {
        FloatVector2 m_margin_ratios;
        ScreenCoordVector2 m_margins;

        inline MarginsSpecification ()
        {
            m_margin_ratios = FloatVector2::ms_zero;
            m_margins = ScreenCoordVector2::ms_zero;
        }
    }; // end of struct WidgetSkin::MarginsSpecification

    // the screen which this WidgetSkin is adapting size ratios to
    Screen const *m_screen;

    WidgetBackground const *m_widget_background[WIDGET_BACKGROUND_TYPE_COUNT];
    FontSpecification m_font_specification[FONT_TYPE_COUNT];
    Resource<GLTexture> m_texture[TEXTURE_TYPE_COUNT];
    MarginsSpecification m_margins_specification[MARGINS_TYPE_COUNT];
}; // end of class WidgetSkin

} // end of namespace Xrb

#endif // !defined(_XRB_WIDGETSKIN_HPP_)
