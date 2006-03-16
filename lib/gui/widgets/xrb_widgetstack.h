// ///////////////////////////////////////////////////////////////////////////
// xrb_widgetstack.h by Victor Dods, created 2005/12/16
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_WIDGETSTACK_H_)
#define _XRB_WIDGETSTACK_H_

#include "xrb.h"

#include "xrb_widget.h"

namespace Xrb
{

class WidgetStack : public Widget
{
public:

    WidgetStack (Widget *const parent, std::string const &name = "WidgetStack")
        :
        Widget(parent, name)
    {
        DirtyContentsSizeProperties();
    }
    virtual ~WidgetStack () { }

    virtual Bool2 GetContentsMinSizeEnabled () const;
    virtual ScreenCoordVector2 GetContentsMinSize () const;
    virtual Bool2 GetContentsMaxSizeEnabled () const;
    virtual ScreenCoordVector2 GetContentsMaxSize () const;
    
    virtual void SetSizePropertyEnabled (
        SizeProperties::Property property,
        Uint32 component,
        bool value);
    virtual void SetSizePropertyEnabled (
        SizeProperties::Property property,
        Bool2 const &value);

    virtual void SetSizeProperty (
        SizeProperties::Property property,
        Uint32 component,
        ScreenCoord value);
    virtual void SetSizeProperty (
        SizeProperties::Property property,
        ScreenCoordVector2 const &value);

    virtual void SetSizePropertyRatio (
        SizeProperties::Property property,
        Uint32 component,
        Float ratio);
    virtual void SetSizePropertyRatios (
        SizeProperties::Property property,
        FloatVector2 const &ratios);

    virtual void SetMainWidget (Widget *main_widget)
    {
        // disallowed
        ASSERT0(false && "You can't set a main widget in a WidgetStack")
    }

    virtual ScreenCoordVector2 Resize (ScreenCoordVector2 const &size);
    virtual void AttachChild (Widget *child);
    virtual void DetachChild (Widget *child);

protected:

    virtual void ChildSizePropertiesChanged (Widget *child);

private:

    // given the delegated column widths and row heights, resize the
    // child widgets to the corresponding column/row height and reposition
    // them so they correctly lay out in the grid
    void ResizeAndRepositionChildWidgets ();

    // the functions which 'dirty' the caches
    void DirtyContentsSizeProperties ();
        
    // the functions which 'undirty' the caches
    void UpdateContentsSizeProperties () const;


    // stores the 'preferred' size properties (i.e. size properties
    // given by calls to SetMinSizeEnabled, SetMinSize, etc, so that
    // when the size restrictions of the child widgets of this layout
    // permit, they can be resized to satisfy these properties.
    SizeProperties m_preferred_size_properties;

    // indicates if the cached size properties values must be recalculated
    mutable bool m_contents_size_properties_need_update;
    mutable SizeProperties m_contents_size_properties;                
}; // end of class WidgetStack

} // end of namespace Xrb

#endif // !defined(_XRB_WIDGETSTACK_H_)
