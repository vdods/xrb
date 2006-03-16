// ///////////////////////////////////////////////////////////////////////////
// xrb_cellpaddingwidget.cpp by Victor Dods, created 2005/04/14
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_cellpaddingwidget.h"

namespace Xrb
{

CellPaddingWidget::CellPaddingWidget (
    Widget *const parent,
    std::string const &name)
    :
    Widget(parent, name)
{
    m_accepts_focus = false;
    m_accepts_mouseover = true;
    m_alignment = Alignment2(CENTER, CENTER);

    CellPaddingWidget::UpdateRenderBackground();
}

Widget *CellPaddingWidget::GetSingleChildWidget ()
{
    ASSERT1(m_child_vector.size() <= 1)

    WidgetVectorIterator it = m_child_vector.begin();
    WidgetVectorIterator it_end = m_child_vector.end();
    if (it != it_end)
    {
        Widget *child = *it;
        ASSERT1(child != NULL)
        return child;
    }
    else
    {
        return NULL;
    }
}

Widget const *CellPaddingWidget::GetSingleChildWidget () const
{
    ASSERT1(m_child_vector.size() <= 1)

    WidgetVectorConstIterator it = m_child_vector.begin();
    WidgetVectorConstIterator it_end = m_child_vector.end();
    if (it != it_end)
    {
        Widget const *child = *it;
        ASSERT1(child != NULL)
        return child;
    }
    else
    {
        return NULL;
    }
}

void CellPaddingWidget::SetAlignmentComponent (
    Uint32 const index,
    Alignment const alignment)
{
    ASSERT1(index <= 1)
    ASSERT1(alignment != SPACED)
    if (m_alignment[index] != alignment)
    {
        m_alignment[index] = alignment;
        PositionSingleChildWidget();
    }
}

void CellPaddingWidget::SetAlignment (
    Alignment2 const &alignment)
{
    ASSERT1(alignment[Dim::X] != SPACED)
    ASSERT1(alignment[Dim::Y] != SPACED)
    if (m_alignment != alignment)
    {
        m_alignment = alignment;
        PositionSingleChildWidget();
    }
}

ScreenCoordVector2 CellPaddingWidget::Resize (
    ScreenCoordVector2 const &size)
{
    Widget::Resize(size);

    // place the single child widget if it exists
    Widget *child = GetSingleChildWidget();
    if (child != NULL)
    {
        // attempt to resize the child to the current size
        child->Resize(size);
        // put the child in its place!
        PositionSingleChildWidget();
    }

    return GetSize();
}

void CellPaddingWidget::AttachChild (Widget *const child)
{
    // make sure there's not already a child widget, because
    // this widget can have only zero or one.
    ASSERT0(GetSingleChildWidget() == NULL)
    Widget::AttachChild(child);
}

void CellPaddingWidget::ChildSizePropertiesChanged (Widget *const child)
{
    ASSERT1(child != NULL)
    Widget::SetSizeProperty(SizeProperties::MIN, child->GetMinSize());
    Widget::SetSizePropertyEnabled(SizeProperties::MIN, child->GetMinSizeEnabled());
}

void CellPaddingWidget::SetMainWidget (Widget *const main_widget)
{
    ASSERT0(false && "Setting a main widget in a CellPaddingWidget is pointless")
}

void CellPaddingWidget::UpdateRenderBackground ()
{
    SetRenderBackground(NULL);
}

void CellPaddingWidget::PositionSingleChildWidget ()
{
    Widget *child = GetSingleChildWidget();
    if (child == NULL)
        return;

    // get the amount of space left over
    ScreenCoordVector2 extra_space(GetSize() - child->GetSize());
    // calculate the child's offset from the parent, given its alignment
    ScreenCoordVector2 child_position_offset;
    for (Uint8 i = 0; i < 2; ++i)
    {
        switch (m_alignment.m[i])
        {
            case TOP:
            case LEFT:
                child_position_offset.m[i] = 0;
                break;

            case CENTER:
                child_position_offset.m[i] = extra_space.m[i] / 2;
                break;

            case BOTTOM:
            case RIGHT:
                child_position_offset.m[i] = extra_space.m[i];
                break;

            case SPACED:
                ASSERT1(false && "SPACED is invalid for use in CellPaddingWidget")
                break;

            default:
                ASSERT1(false && "Invalid Alignment")
                break;
        }
    }
    // move the child into position
    child->MoveTo(GetPosition() + child_position_offset);
}

} // end of namespace Xrb
