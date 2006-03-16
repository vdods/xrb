// ///////////////////////////////////////////////////////////////////////////
// xrb_cellpaddingwidget.h by Victor Dods, created 2005/04/11
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_CELLPADDINGWIDGET_H_)
#define _XRB_CELLPADDINGWIDGET_H_

#include "xrb.h"

#include "xrb_ntuple.h"
#include "xrb_widget.h"

namespace Xrb
{

class CellPaddingWidget : public Widget
{
public:

    CellPaddingWidget (
        Widget *const parent,
        std::string const &name = "CellPaddingWidget");
    virtual ~CellPaddingWidget () { }

    inline Alignment2 const &GetAlignment () const { return m_alignment; }
    Widget *GetSingleChildWidget ();
    Widget const *GetSingleChildWidget () const;

    void SetAlignmentComponent (
        Uint32 const index,
        Alignment const alignment);
    void SetAlignment (Alignment2 const &alignment);

    virtual ScreenCoordVector2 Resize (ScreenCoordVector2 const &size);
    virtual void AttachChild (Widget *child);
    virtual void ChildSizePropertiesChanged (Widget *child);

    virtual void SetMainWidget (Widget *main_widget);

protected:

    virtual void UpdateRenderBackground ();

private:

    void PositionSingleChildWidget ();

    // contains the alignment for the child widget
    Alignment2 m_alignment;
}; // end of class CellPaddingWidget

} // end of namespace Xrb

#endif // !defined(_XRB_CELLPADDINGWIDGET_H_)
