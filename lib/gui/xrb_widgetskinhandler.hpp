// ///////////////////////////////////////////////////////////////////////////
// xrb_widgetskinhandler.hpp by Victor Dods, created 2005/06/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_WIDGETSKINHANDLER_HPP_)
#define _XRB_WIDGETSKINHANDLER_HPP_

#include "xrb.hpp"

#include <vector>

#include "xrb_widgetskin.hpp"

namespace Xrb {

class Widget;

/** This class is a baseclass for Widget.  It was made separate to keep
  * Widget from getting too bulky, and to separate the code out into nice,
  * logical sections.
  *
  * See @ref section_widget_skinning "Widget Skinning" for detailed info.
  *
  * @brief Manages changes to the contained WidgetSkin.
  */
class WidgetSkinHandler
{
public:

    /** The default behavior is to default m_widget_skin to NULL, and let it
      * be set properly when attaching the Widget to its parent.  The
      * WidgetSkin is, by default, not deleted when this object is destroyed.
      * @brief Default constructor.
      */
    WidgetSkinHandler ();
    /** Iff m_delete_widget_skin is true, m_widget_skin is deleted.
      * @brief Destructor.
      */
    virtual ~WidgetSkinHandler ();

    // ///////////////////////////////////////////////////////////////////////
    // accessors
    // ///////////////////////////////////////////////////////////////////////

    /// Returns a pointer to the const WidgetSkin managed by this.
    WidgetSkin const *GetWidgetSkin () const { return m_widget_skin; }
    /// Frontend for @ref Xrb::WidgetSkin::GetWidgetBackground .
    WidgetBackground const *WidgetSkinWidgetBackground (WidgetSkin::WidgetBackgroundType widget_background_type) const;
    /// Frontend for @ref Xrb::WidgetSkin::FontPath .
    std::string const &WidgetSkinFontPath (WidgetSkin::FontType font_type) const;
    /// Frontend for @ref Xrb::WidgetSkin::FontHeightRatio .
    Float WidgetSkinFontHeightRatio (WidgetSkin::FontType font_type) const;
    /// Frontend for @ref Xrb::WidgetSkin::LoadFont .
    Resource<Font> WidgetSkinLoadFont (WidgetSkin::FontType font_type) const;
    /// Frontend for @ref Xrb::WidgetSkin::GetTexture .
    Resource<GlTexture> WidgetSkinTexture (WidgetSkin::TextureType texture_type) const;
    /// Frontend for @ref Xrb::WidgetSkin::Margins .
    ScreenCoordMargins WidgetSkinMargins (WidgetSkin::MarginsType margins_type) const;

    // ///////////////////////////////////////////////////////////////////////
    // modifiers
    // ///////////////////////////////////////////////////////////////////////

    void SetWidgetSkin (WidgetSkin *widget_skin);
    
    /** @brief Frontend for @ref Xrb::WidgetSkin::SetWidgetBackground .
      */
//     void SetWidgetSkinWidgetBackground (WidgetSkin::WidgetBackgroundType widget_background_type, WidgetBackground const *widget_background);
    /** @brief Frontend for @ref Xrb::WidgetSkin::SetFont .
      */
//     void SetWidgetSkinFont (WidgetSkin::FontType font_type, Resource<Font> const &font);
    /** @brief Frontend for @ref Xrb::WidgetSkin::SetFontFacePath .
      */
//     void SetWidgetSkinFontFacePath (WidgetSkin::FontType font_type, std::string const &font_face_path);
    /** @brief Frontend for @ref Xrb::WidgetSkin::SetFontHeightRatio .
      */
//     void SetWidgetSkinFontHeightRatio (WidgetSkin::FontType font_type, Float font_height_ratio);
    /** @brief Frontend for @ref Xrb::WidgetSkin::SetFontHeight .
      */
//     void SetWidgetSkinFontHeight (WidgetSkin::FontType font_type, ScreenCoord font_height);
    /** @brief Frontend for @ref Xrb::WidgetSkin::SetTexture .
      */
//     void SetWidgetSkinTexture (WidgetSkin::TextureType texture_type, Resource<GlTexture> const &texture);
    /** @brief Frontend for @ref Xrb::WidgetSkin::SetTexturePath .
      */
//     void SetWidgetSkinTexturePath (WidgetSkin::TextureType texture_type, std::string const &texture_path);
    /** @brief Frontend for @ref Xrb::WidgetSkin::SetMarginRatios .
      */
//     void SetWidgetSkinMarginRatios (WidgetSkin::MarginsType margin_type, FloatMargins const &margin_ratios);
    /** @brief Frontend for @ref Xrb::WidgetSkin::SetMargins .
      */
//     void SetWidgetSkinMargins (WidgetSkin::MarginsType margin_type, ScreenCoordMargins const &margins);

protected:

    /// This is called any time m_widget_skin changes.
    virtual void HandleChangedWidgetSkin () { }
    
    // ///////////////////////////////////////////////////////////////////////
    // abstract functions
    // ///////////////////////////////////////////////////////////////////////

    /// Should return the size ratio basis for use in Font loading and Margins determination.
    virtual ScreenCoord WidgetSkinHandlerSizeRatioBasis () const = 0;
    /// Should return the number of child WidgetSkinHandler objects this object owns.
    virtual Uint32 WidgetSkinHandlerChildCount () const = 0;
    /// Should return the specified indexed child WidgetSkinHandler.
    virtual WidgetSkinHandler *WidgetSkinHandlerChild (Uint32 index) = 0;
    /// Returns the parent WidgetSkinHandler of this object.
    virtual WidgetSkinHandler *WidgetSkinHandlerParent () = 0;

    /// Used (e.g. by Screen) to unload everything (e.g. before the Gl singleton dies)
    void ReleaseAllWidgetSkinResources ();

    /** This object will only be deleted iff m_delete_widget_skin is true.
      * @brief A pointer to the contained WidgetSkin object.
      */
    WidgetSkin *m_widget_skin;
    /** The default value is false.  Widget/Screen sets this according to
      * wether or not it is a top-level widget.
      * @brief Indicates if m_widget_skin should be deleted upon destruction.
      */
    bool m_delete_widget_skin;

private:
/*
    // this function is used to generalize the redundant code in all the SetBlahBlah functions
    template <typename PropertyType, typename DataType>
    void SetProperty (PropertyType property, DataType data, void (WidgetSkin::*SetWidgetSkinProperty)(PropertyType, DataType))
    {
        // make sure we go up to the top level parent before setting
        // the property and propagating the changes.
        if (WidgetSkinHandlerParent() != NULL)
        {
            WidgetSkinHandlerParent()->SetProperty<PropertyType, DataType>(property, data, SetWidgetSkinProperty);
        }
        else
        {
            ASSERT1(m_widget_skin != NULL);
            // set the property on the widget skin
            (m_widget_skin->*SetWidgetSkinProperty)(property, data);
            // send property change notification down the widget hierarchy.
            HandleChangedWidgetSkin();
        }
    }
    */
}; // end of class WidgetSkinHandler

} // end of namespace Xrb

#endif // !defined(_XRB_WIDGETSKINHANDLER_HPP_)

