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

namespace Xrb
{

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

    /** @brief Returns a pointer to the const WidgetSkin managed by this.
      */
    inline WidgetSkin const *GetWidgetSkin () const
    {
        return m_widget_skin;
    }
    /** @brief Frontend for @ref Xrb::WidgetSkin::GetWidgetBackground .
      * @param widget_background_type The background type to retrieve.
      */
    inline WidgetBackground const *GetWidgetSkinWidgetBackground (
        WidgetSkin::WidgetBackgroundType const widget_background_type) const
    {
        return m_widget_skin->GetWidgetBackground(widget_background_type);
    }
    /** @brief Frontend for @ref Xrb::WidgetSkin::GetFont .
      * @param font_type The font type to retrieve.
      */
    inline Resource<Font> const &GetWidgetSkinFont (
        WidgetSkin::FontType const font_type) const
    {
        return m_widget_skin->GetFont(font_type);
    }
    /** @brief Frontend for @ref Xrb::WidgetSkin::GetTexture .
      * @param texture_type The texture type to retrieve.
      */
    inline Resource<GLTexture> const &GetWidgetSkinTexture (
        WidgetSkin::TextureType const texture_type) const
    {
        return m_widget_skin->GetTexture(texture_type);
    }
    /** @brief Frontend for @ref Xrb::WidgetSkin::GetMargins .
      * @param margins_type The margins type to retrieve.
      */
    inline ScreenCoordVector2 const &GetWidgetSkinMargins (
        WidgetSkin::MarginsType const margins_type) const
    {
        return m_widget_skin->GetMargins(margins_type);
    }

    // ///////////////////////////////////////////////////////////////////////
    // modifiers
    // ///////////////////////////////////////////////////////////////////////

    /** @brief Frontend for @ref Xrb::WidgetSkin::SetWidgetBackground .
      */
    void SetWidgetSkinWidgetBackground (
        WidgetSkin::WidgetBackgroundType widget_background_type,
        WidgetBackground const *widget_background);
    /** @brief Frontend for @ref Xrb::WidgetSkin::SetFont .
      */
    void SetWidgetSkinFont (
        WidgetSkin::FontType font_type,
        Resource<Font> const &font);
    /** @brief Frontend for @ref Xrb::WidgetSkin::SetFontFaceFilename .
      */
    void SetWidgetSkinFontFaceFilename (
        WidgetSkin::FontType font_type,
        std::string const &font_face_filename);
    /** @brief Frontend for @ref Xrb::WidgetSkin::SetFontFaceFilename .
      */
    void SetWidgetSkinFontHeightRatio (
        WidgetSkin::FontType font_type,
        Float font_height_ratio);
    /** @brief Frontend for @ref Xrb::WidgetSkin::SetFontHeightRatio .
      */
    void SetWidgetSkinFontHeight (
        WidgetSkin::FontType font_type,
        ScreenCoord font_height);
    /** @brief Frontend for @ref Xrb::WidgetSkin::SetTexture .
      */
    void SetWidgetSkinTexture (
        WidgetSkin::TextureType texture_type,
        Resource<GLTexture> const &texture);
    /** @brief Frontend for @ref Xrb::WidgetSkin::SetTextureFilename .
      */
    void SetWidgetSkinTextureFilename (
        WidgetSkin::TextureType texture_type,
        std::string const &texture_filename);
    /** @brief Frontend for @ref Xrb::WidgetSkin::SetMarginRatios .
      */
    void SetWidgetSkinMarginRatios (
        WidgetSkin::MarginsType margin_type,
        FloatVector2 const &margin_ratios);
    /** @brief Frontend for @ref Xrb::WidgetSkin::SetMargins .
      */
    void SetWidgetSkinMargins (
        WidgetSkin::MarginsType margin_type,
        ScreenCoordVector2 const &margins);

protected:

    // ///////////////////////////////////////////////////////////////////////
    // abstract functions
    // ///////////////////////////////////////////////////////////////////////

    /** This method must be implemented by baseclasses (Widget).
      * @brief Returns the number of child WidgetSkinHandler objects this
      *        object owns.
      */
    virtual Uint32 GetWidgetSkinHandlerChildCount () const = 0;
    /** This method must be implemented by baseclasses (Widget).
      * @brief Returns the indexed child WidgetSkinHandler.
      * @param index The index of the child to return.
      */
    virtual WidgetSkinHandler *GetWidgetSkinHandlerChild (Uint32 index) = 0;
    /** This method must be implemented by baseclasses (Widget).
      * @brief Returns the parent WidgetSkinHandler of this object.
      */
    virtual WidgetSkinHandler *GetWidgetSkinHandlerParent () = 0;

    // ///////////////////////////////////////////////////////////////////////
    // WidgetSkin property change handlers
    // ///////////////////////////////////////////////////////////////////////

    /** This method may be overridden by subclasses when it is necessary
      * to know when a widget background has been changed, for example,
      * to update the render background for a Button.
      * @brief Called when a widget background is changed.
      */
    virtual void HandleChangedWidgetSkinWidgetBackground (
        WidgetSkin::WidgetBackgroundType widget_background_type) { }
    /** This method may be overridden by subclasses when it is necessary
      * to know when a font has been changed, for example, to update the
      * render font for a Label.
      * @brief Called when a font is changed.
      */
    virtual void HandleChangedWidgetSkinFont (
        WidgetSkin::FontType font_type) { }
    /** This method may be overridden by subclasses when it is necessary
      * to know when a texture has been changed, for example, to update the
      * render picture for a CheckBox.
      * @brief Called when a texture is changed.
      */
    virtual void HandleChangedWidgetSkinTexture (
        WidgetSkin::TextureType texture_type) { }
    /** This method may be overridden by subclasses when it is necessary
      * to know when a margins vector has been changed, for example,
      * to update the properties of a Layout.
      * @brief Called when a margins vector is changed.
      */
    virtual void HandleChangedWidgetSkinMargins (
        WidgetSkin::MarginsType margins_type) { }

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

    // ///////////////////////////////////////////////////////////////////////
    // this function is used to generalize the redundant code in
    // all the SetBlahBlah functions
    // ///////////////////////////////////////////////////////////////////////

    template <typename PropertyType, typename DataType>
    void SetProperty (
        PropertyType const property,
        DataType const data,
        void (WidgetSkin::*SetWidgetSkinProperty)(PropertyType, DataType),
        void (WidgetSkinHandler::*PropagateChangedPropertyFunction)(PropertyType))
    {
        // make sure we go up to the top level parent before setting
        // the property and propagating the changes.
        if (GetWidgetSkinHandlerParent() != NULL)
        {
            GetWidgetSkinHandlerParent()->SetProperty<PropertyType, DataType>(
                property,
                data,
                SetWidgetSkinProperty,
                PropagateChangedPropertyFunction);
        }
        else
        {
            ASSERT1(m_widget_skin != NULL);
            // set the property on the widget skin
            (m_widget_skin->*SetWidgetSkinProperty)(property, data);
            // send property change notification down the widget hierarchy.
            (this->*PropagateChangedPropertyFunction)(property);
        }
    }

    // ///////////////////////////////////////////////////////////////////////
    // these functions propagate property changes to child widgets
    // ///////////////////////////////////////////////////////////////////////

    template <typename PropertyType>
    void PropagateChangedProperty (
        PropertyType const property,
        void (WidgetSkinHandler::*HandleChangedProperty)(PropertyType))
    {
        // call HandleChangedProperty on this WidgetSkinHandler
        (this->*HandleChangedProperty)(property);
        // call PropagateProperty on all child WidgetSkinHandlers
        for (Uint32 i = 0; i < GetWidgetSkinHandlerChildCount(); ++i)
        {
            WidgetSkinHandler *child = GetWidgetSkinHandlerChild(i);
            ASSERT1(child != NULL);
            child->PropagateChangedProperty<PropertyType>(
                property,
                HandleChangedProperty);
        }
    }

    void PropagateChangedWidgetBackground (
        WidgetSkin::WidgetBackgroundType widget_background_type);
    void PropagateChangedFont (
        WidgetSkin::FontType font_type);
    void PropagateChangedTexture (
        WidgetSkin::TextureType texture_type);
    void PropagateChangedMargins (
        WidgetSkin::MarginsType margins_type);
}; // end of class WidgetSkinHandler

} // end of namespace Xrb

#endif // !defined(_XRB_WIDGETSKINHANDLER_HPP_)

