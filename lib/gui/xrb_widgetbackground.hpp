// ///////////////////////////////////////////////////////////////////////////
// xrb_widgetbackground.hpp by Victor Dods, created 2005/02/09
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_WIDGETBACKGROUND_HPP_)
#define _XRB_WIDGETBACKGROUND_HPP_

#include "xrb.hpp"

#include "xrb_color.hpp"
#include "xrb_emptystring.hpp"
#include "xrb_gltexture.hpp"
#include "xrb_margins.hpp"
#include "xrb_rect.hpp"
#include "xrb_rendercontext.hpp"
#include "xrb_resourcelibrary.hpp"
#include "xrb_vector.hpp"

namespace Xrb
{

/** A widget background implementation must indicate how it is to be
  * rendered to screen ( by overriding @ref Xrb::WidgetBackground::Draw ),
  * and how to clone itself ( by overriding
  * @ref Xrb::WidgetBackground::CreateClone ).
  * @brief Interface class for widget backgrounds.
  */
class WidgetBackground
{
public:

    WidgetBackground () { }
    virtual ~WidgetBackground () { }

    /** This function must be implemented by subclasses of WidgetBackground.
      * @brief Returns a pointer to a newly instantiated clone of this object.
      */
    virtual WidgetBackground *CreateClone () const = 0;

    /** This function must be implemented by subclasses of WidgetBackground.
      * @brief Draws this WidgetBackground object to screen.
      * @param render_context The RenderContext necessary for rendering.
      * @param widget_screen_rect The bounding rectangle of the widget this
      *                           background is being drawn for.
      * @param frame_margins The frame margins of the widget this background
      *                      is being drawn for.
      */
    virtual void Draw (
        RenderContext const &render_context,
        ScreenCoordRect const &widget_screen_rect,
        ScreenCoordMargins const &frame_margins) const = 0;
}; // end of class WidgetBackground

/** No external resources are necessary to use this background.
  * @brief Implements a solid-color widget background.
  */
class WidgetBackgroundColored : public WidgetBackground
{
public:

    /** The color may be anything, including any alpha value.
      * @brief Constructs the background using the specified color.
      */
    WidgetBackgroundColored (
        Color const &color)
        :
        WidgetBackground()
    {
        m_color = color;
    }
    /// Boring destructor.
    virtual ~WidgetBackgroundColored () { }

    /** See @ref Xrb::WidgetBackground::CreateClone for more details.
      * @brief WidgetBackgroundColored implementation of CreateClone.
      */
    virtual WidgetBackground *CreateClone () const;

    /** Simply fills the widget screen rect with the stored solid color.
      *
      * See @ref Xrb::WidgetBackground::Draw for more info.
      *
      * @brief Draws this widget background.
      */
    virtual void Draw (
        RenderContext const &render_context,
        ScreenCoordRect const &widget_screen_rect,
        ScreenCoordMargins const &frame_margins) const;

private:

    Color m_color;
}; // end of class WidgetBackgroundColored

/** A single texture resource is necessary for this background.
  * @brief Implements a single-texture widget background.
  */
class WidgetBackgroundTextured : public WidgetBackground
{
public:

    /** @brief Constructs the background using the specified texture.
      * @param texture_path Path of the desired texture.
      */
    WidgetBackgroundTextured (std::string const &texture_path);
    /** @brief Constructs the background using the specified texture.
      * @param texture Directly specifies the GlTexture object to use.
      */
    WidgetBackgroundTextured (Resource<GlTexture> const &texture);
    /** m_texture is automatically deleted because it goes out of scope here.
      * @brief Destructor.
      */
    virtual ~WidgetBackgroundTextured ();

    /** See @ref Xrb::WidgetBackground::CreateClone for more details.
      * @brief WidgetBackgroundTextured implementation of CreateClone.
      */
    virtual WidgetBackground *CreateClone () const;

    /** Simply draws the stored texture stretched to fill the
      * given widget screen rect.
      *
      * See @ref Xrb::WidgetBackground::Draw for more info.
      *
      * @brief Draws this widget background.
      */
    virtual void Draw (
        RenderContext const &render_context,
        ScreenCoordRect const &widget_screen_rect,
        ScreenCoordMargins const &frame_margins) const;

private:

    Resource<GlTexture> m_texture;
}; // end of class WidgetBackgroundTextured

/** Two texture resources are necessary for this background.
  *
  * A widget is divided up into 3 regions by its left and right margins (assuming
  * its frame margins have non-zero size).  The widget looks like <pre>
     +----+--------------------------+----+
     | 1  | 2                        | 3  |
     |    |                          |    |
     +    +                          +    +
     |    |                          |    |
     |    |                          |    |
     |    |                          |    |
     |    |                          |    |
     |    |                          |    |
     |    |                          |    |
     |    |                          |    |
     |    |                          |    |
     |    |                          |    |
     +    +                          +    +
     |    |                          |    |
     |    |                          |    |
     +----+--------------------------+----+
  * </pre>
  *
  * There are four textures specified; corner, top, left, and center.
  *     <ul>
  *     <li>Region 1 is rendered using the left texture, unflipped.</li>
  *     <li>Region 2 is rendered using the center texture, unflipped.</li>
  *     <li>Region 3 is rendered using the right texture, or the left texture, flipped
  *         horizontally, if the right texture is not specified.</li>
  *     </ul>
  *
  * @brief Implements a multiple-texture widget background.
  */
class WidgetBackgroundTextured3Way : public WidgetBackground
{
public:

    /** @brief Constructs the background using the specified textures.
      * @param left_texture_name The path of the left texture to use.
      * @param center_texture_name The path of the center texture to use.
      * @param right_texture_name The path of the center texture to use, or the
                                  empty string if the left texture is to be used
                                  (see above).
      */
    WidgetBackgroundTextured3Way (
        std::string const &left_texture_name,
        std::string const &center_texture_name,
        std::string const &right_texture_name = g_empty_string);
    /** @brief Constructs the background using the specified textures.
      * @param left_texture Directly specifies the GlTexture object to use
      *                     for the left texture.
      * @param center_texture Directly specifies the GlTexture object to use
      *                       for the center texture.
      * @param right_texture Directly specifies the GlTexture object to use
      *                      for the right texture.
      */
    WidgetBackgroundTextured3Way (
        Resource<GlTexture> const &left_texture,
        Resource<GlTexture> const &center_texture,
        Resource<GlTexture> const &right_texture = Resource<GlTexture>());
    /** m_corner_texture, m_top_texture, m_left_texture, and m_center_texture
      * are automatically deleted because they go out of scope here.
      * @brief Destructor.
      */
    virtual ~WidgetBackgroundTextured3Way ();

    /** See @ref Xrb::WidgetBackground::CreateClone for more details.
      * @brief WidgetBackgroundStylized implementation of CreateClone.
      */
    virtual WidgetBackground *CreateClone () const;

    /** @brief Draws this widget background.
      */
    virtual void Draw (
        RenderContext const &render_context,
        ScreenCoordRect const &widget_screen_rect,
        ScreenCoordMargins const &frame_margins) const;

private:

    Resource<GlTexture> m_left_texture;
    Resource<GlTexture> m_center_texture;
    Resource<GlTexture> m_right_texture;
}; // end of class WidgetBackgroundTextured3Way

/** Four texture resources are necessary for this background.
  *
  * A widget is divided up into 9 regions by its frame margins (assuming
  * its frame margins have non-zero size).  The widget looks like <pre>
     +----+--------------------------+----+
     | 1  | 2                        | 3  |
     |    |                          |    |
     +----+--------------------------+----+
     |    |                          |    |
     | 4  | 5                        | 6  |
     |    |                          |    |
     |    |                          |    |
     |    |                          |    |
     |    |                          |    |
     |    |                          |    |
     |    |                          |    |
     |    |                          |    |
     +----+--------------------------+----+
     | 7  | 8                        | 9  |
     |    |                          |    |
     +----+--------------------------+----+
  * </pre>
  *
  * There are four textures specified; corner, top, left, and center.
  *     <ul>
  *     <li>Region 1 is rendered using the corner texture, unflipped.</li>
  *     <li>Region 2 is rendered using the top texture, unflipped.</li>
  *     <li>Region 3 is rendered using the corner texture, flipped horizontally.</li>
  *     <li>Region 4 is rendered using the left texture, unflipped.</li>
  *     <li>Region 5 is rendered using the center texture, unflipped.</li>
  *     <li>Region 6 is rendered using the left texture, flipped horizontally.</li>
  *     <li>Region 7 is rendered using the corner texture, flipped vertically.</li>
  *     <li>Region 8 is rendered using the top texture, flipped vertically.</li>
  *     <li>Region 9 is rendered using the corner texture, flipper horizontally and vertically.</li>
  *     </ul>
  *
  * @brief Implements a multiple-texture widget background.
  */
class WidgetBackgroundStylized : public WidgetBackground
{
public:

    /** @brief Constructs the background using the specified textures.
      * @param corner_texture_name The path of the corner texture to use.
      * @param top_texture_name The path of the top texture to use.
      * @param left_texture_name The path of the left texture to use.
      * @param center_texture_name The path of the center texture to use.
      */
    WidgetBackgroundStylized (
        std::string const &corner_texture_name,
        std::string const &top_texture_name,
        std::string const &left_texture_name,
        std::string const &center_texture_name);
    /** @brief Constructs the background using the specified textures.
      * @param corner_texture Directly specifies the GlTexture object to use
      *                       for the corner texture.
      * @param top_texture Directly specifies the GlTexture object to use
      *                    for the top texture.
      * @param left_texture Directly specifies the GlTexture object to use
      *                     for the left texture.
      * @param center_texture Directly specifies the GlTexture object to use
      *                       for the center texture.
      */
    WidgetBackgroundStylized (
        Resource<GlTexture> const &corner_texture,
        Resource<GlTexture> const &top_texture,
        Resource<GlTexture> const &left_texture,
        Resource<GlTexture> const &center_texture);
    /** m_corner_texture, m_top_texture, m_left_texture, and m_center_texture
      * are automatically deleted because they go out of scope here.
      * @brief Destructor.
      */
    virtual ~WidgetBackgroundStylized ();

    /** See @ref Xrb::WidgetBackground::CreateClone for more details.
      * @brief WidgetBackgroundStylized implementation of CreateClone.
      */
    virtual WidgetBackground *CreateClone () const;

    /** Draws the corner texture on the frame margin corners of the widget,
      * the top and left textures on the top/bottom and left/right frame
      * margin sides of the widget, and the center texture on the remaining
      * area enclosed inside the frame margins.
      *
      * See @ref Xrb::WidgetBackground::Draw for more info.
      *
      * @brief Draws this widget background.
      */
    virtual void Draw (
        RenderContext const &render_context,
        ScreenCoordRect const &widget_screen_rect,
        ScreenCoordMargins const &frame_margins) const;

private:

    Resource<GlTexture> m_corner_texture;
    Resource<GlTexture> m_top_texture;
    Resource<GlTexture> m_left_texture;
    Resource<GlTexture> m_center_texture;
}; // end of class WidgetBackgroundStylized

/** Four texture resources are necessary for this background.
  *
  * A widget is divided up into 9 regions by its frame margins (assuming
  * its frame margins have non-zero size).  The widget looks like <pre>
     +----+--------------------------+----+
     | 1  | 2                        | 3  |
     |    |                          |    |
     +----+--------------------------+----+
     |    |                          |    |
     | 4  | 5                        | 6  |
     |    |                          |    |
     |    |                          |    |
     |    |                          |    |
     |    |                          |    |
     |    |                          |    |
     |    |                          |    |
     |    |                          |    |
     +----+--------------------------+----+
     | 7  | 8                        | 9  |
     |    |                          |    |
     +----+--------------------------+----+
  * </pre>
  *
  * Each region gets its own texture.
  *
  * @brief Implements a multiple-texture widget background.
  */
class WidgetBackgroundTextured9Way : public WidgetBackground
{
public:

    /** @brief Constructs the background using the specified textures.
      */
    WidgetBackgroundTextured9Way (
        Resource<GlTexture> const &top_left_texture,
        Resource<GlTexture> const &top_center_texture,
        Resource<GlTexture> const &top_right_texture,
        Resource<GlTexture> const &center_left_texture,
        Resource<GlTexture> const &center_center_texture,
        Resource<GlTexture> const &center_right_texture,
        Resource<GlTexture> const &bottom_left_texture,
        Resource<GlTexture> const &bottom_center_texture,
        Resource<GlTexture> const &bottom_right_texture);
    /** Each of the GlTexture Resources are automatically deleted because
      * they go out of scope here.
      * @brief Destructor.
      */
    virtual ~WidgetBackgroundTextured9Way ();

    /** See @ref Xrb::WidgetBackground::CreateClone for more details.
      * @brief WidgetBackgroundTextured9Way implementation of CreateClone.
      */
    virtual WidgetBackground *CreateClone () const;

    /** @brief Draws this widget background.
      */
    virtual void Draw (
        RenderContext const &render_context,
        ScreenCoordRect const &widget_screen_rect,
        ScreenCoordMargins const &frame_margins) const;

private:

    Resource<GlTexture> m_top_left_texture;
    Resource<GlTexture> m_top_center_texture;
    Resource<GlTexture> m_top_right_texture;
    Resource<GlTexture> m_center_left_texture;
    Resource<GlTexture> m_center_center_texture;
    Resource<GlTexture> m_center_right_texture;
    Resource<GlTexture> m_bottom_left_texture;
    Resource<GlTexture> m_bottom_center_texture;
    Resource<GlTexture> m_bottom_right_texture;
}; // end of class WidgetBackgroundTextured9Way

} // end of namespace Xrb

#endif // !defined(_XRB_WIDGETBACKGROUND_HPP_)

