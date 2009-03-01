// ///////////////////////////////////////////////////////////////////////////
// xrb_cellpaddingwidget.hpp by Victor Dods, created 2005/04/11
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_CELLPADDINGWIDGET_HPP_)
#define _XRB_CELLPADDINGWIDGET_HPP_

#include "xrb.hpp"

#include "xrb_containerwidget.hpp"
#include "xrb_ntuple.hpp"

namespace Xrb
{

class CellPaddingWidget : public ContainerWidget
{
public:

    CellPaddingWidget (
        ContainerWidget *const parent,
        std::string const &name = "CellPaddingWidget");
    virtual ~CellPaddingWidget () { }

    inline Alignment2 const &GetAlignment () const { return m_alignment; }
    Widget *GetSingleChildWidget ();
    Widget const *GetSingleChildWidget () const;

    virtual Bool2 GetContentsMinSizeEnabled () const;
    virtual ScreenCoordVector2 GetContentsMinSize () const;
    virtual Bool2 GetContentsMaxSizeEnabled () const;
    virtual ScreenCoordVector2 GetContentsMaxSize () const;

    void SetAlignment (Uint32 const index, Alignment const alignment);
    void SetAlignment (Alignment2 const &alignment);

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

    virtual void SetMainWidget (Widget *main_widget)
    {
        ASSERT0(false && "Setting a main widget in a CellPaddingWidget is pointless");
    }

    virtual ScreenCoordVector2 Resize (ScreenCoordVector2 const &size);
    virtual void AttachChild (Widget *child);
    virtual void DetachChild (Widget *child);

protected:

    virtual void ChildSizePropertiesChanged (Widget *child);

private:

    void PositionSingleChildWidget ();

    // the functions which 'dirty' the caches
    void DirtyContentsSizeProperties ();

    // the functions which 'undirty' the caches
    void UpdateContentsSizeProperties () const;

    // contains the alignment for the child widget
    Alignment2 m_alignment;
    // indicates if the cached size properties values must be recalculated
    mutable bool m_contents_size_properties_need_update;
    mutable SizeProperties m_contents_size_properties;
}; // end of class CellPaddingWidget

} // end of namespace Xrb

#endif // !defined(_XRB_CELLPADDINGWIDGET_HPP_)
