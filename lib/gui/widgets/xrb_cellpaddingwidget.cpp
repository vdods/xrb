// ///////////////////////////////////////////////////////////////////////////
// xrb_cellpaddingwidget.cpp by Victor Dods, created 2005/04/14
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_cellpaddingwidget.hpp"

#include "xrb_screen.hpp"

namespace Xrb {

CellPaddingWidget::CellPaddingWidget (WidgetContext &context, std::string const &name)
    :
    ContainerWidget(context, name)
{
    m_accepts_focus = false;
    m_accepts_mouseover = true;
    m_alignment = Alignment2(CENTER, CENTER);

    // there should be no frame margins for CellPaddingWidget, since it's
    // more of a utility ContainerWidget.
    SetFrameMargins(ScreenCoordVector2::ms_zero);

    DirtyContentsSizeProperties();
}

Widget *CellPaddingWidget::SingleChildWidget ()
{
    ASSERT1(m_child_vector.size() <= 1);

    WidgetVector::iterator it = m_child_vector.begin();
    WidgetVector::iterator it_end = m_child_vector.end();
    if (it != it_end)
    {
        Widget *child = *it;
        ASSERT1(child != NULL);
        return child;
    }
    else
    {
        return NULL;
    }
}

Widget const *CellPaddingWidget::SingleChildWidget () const
{
    ASSERT1(m_child_vector.size() <= 1);

    WidgetVector::const_iterator it = m_child_vector.begin();
    WidgetVector::const_iterator it_end = m_child_vector.end();
    if (it != it_end)
    {
        Widget const *child = *it;
        ASSERT1(child != NULL);
        return child;
    }
    else
    {
        return NULL;
    }
}

Bool2 CellPaddingWidget::ContentsMinSizeEnabled () const
{
    UpdateContentsSizeProperties();
    return m_contents_size_properties.m_min_size_enabled;
}

ScreenCoordVector2 CellPaddingWidget::ContentsMinSize () const
{
    UpdateContentsSizeProperties();
    return m_contents_size_properties.m_min_size;
}

Bool2 CellPaddingWidget::ContentsMaxSizeEnabled () const
{
    UpdateContentsSizeProperties();
    return m_contents_size_properties.m_max_size_enabled;
}

ScreenCoordVector2 CellPaddingWidget::ContentsMaxSize () const
{
    UpdateContentsSizeProperties();
    return m_contents_size_properties.m_max_size;
}

void CellPaddingWidget::SetAlignment (Uint32 index, Alignment alignment)
{
    ASSERT1(index <= 1);
    ASSERT1(alignment != SPACED);
    if (m_alignment[index] != alignment)
    {
        m_alignment[index] = alignment;
        PositionSingleChildWidget();
    }
}

void CellPaddingWidget::SetAlignment (Alignment2 const &alignment)
{
    ASSERT1(alignment[Dim::X] != SPACED);
    ASSERT1(alignment[Dim::Y] != SPACED);
    if (m_alignment != alignment)
    {
        m_alignment = alignment;
        PositionSingleChildWidget();
    }
}

void CellPaddingWidget::SetSizePropertyEnabled (SizeProperties::Property property, Uint32 component, bool value)
{
    ASSERT1(component <= 1);
    if (property == SizeProperties::MIN)
        m_preferred_size_properties.m_min_size_enabled[component] = value;
    else
        m_preferred_size_properties.m_max_size_enabled[component] = value;
    ContainerWidget::SetSizePropertyEnabled(property, component, value, true);
    CalculateMinAndMaxSizePropertiesFromContents();
//     ParentChildSizePropertiesUpdate(false);
}

void CellPaddingWidget::SetSizePropertyEnabled (SizeProperties::Property property, Bool2 const &value)
{
    if (property == SizeProperties::MIN)
        m_preferred_size_properties.m_min_size_enabled = value;
    else
        m_preferred_size_properties.m_max_size_enabled = value;
    ContainerWidget::SetSizePropertyEnabled(property, value, true);
    CalculateMinAndMaxSizePropertiesFromContents();
//     ParentChildSizePropertiesUpdate(false);
}

void CellPaddingWidget::SetSizeProperty (SizeProperties::Property property, Uint32 component, ScreenCoord value)
{
    ASSERT1(component <= 1);
    ASSERT1(value >= 0);
    if (property == SizeProperties::MIN)
        m_preferred_size_properties.m_min_size[component] = value;
    else
        m_preferred_size_properties.m_max_size[component] = value;
    ContainerWidget::SetSizeProperty(property, component, value, true);
    CalculateMinAndMaxSizePropertiesFromContents();
//     ParentChildSizePropertiesUpdate(false);
}

void CellPaddingWidget::SetSizeProperty (SizeProperties::Property property, ScreenCoordVector2 const &value)
{
    ASSERT1(value[Dim::X] >= 0);
    ASSERT1(value[Dim::Y] >= 0);
    if (property == SizeProperties::MIN)
        m_preferred_size_properties.m_min_size = value;
    else
        m_preferred_size_properties.m_max_size = value;
    ContainerWidget::SetSizeProperty(property, value, true);
    CalculateMinAndMaxSizePropertiesFromContents();
//     ParentChildSizePropertiesUpdate(false);
}

ScreenCoordVector2 CellPaddingWidget::Resize (ScreenCoordVector2 const &size)
{
    ContainerWidget::Resize(size);

    // place the single child widget if it exists
    Widget *child = SingleChildWidget();
    if (child != NULL)
    {
        // only update size stuff if not blocked
        if (ChildResizeBlockerCount() == 0)
        {
            // attempt to resize the child to the current ContentsRect() size
            child->Resize(ContentsRect().Size());
            // put the child in its place!
            PositionSingleChildWidget();
        }
        else
            IndicateChildResizeWasBlocked();
    }

    return Size();
}

void CellPaddingWidget::AttachChild (Widget *child)
{
    // make sure there's not already a child widget, because
    // this widget can have only zero or one.
    ASSERT0(SingleChildWidget() == NULL);
    // call the superclass to actually attach the child
    ContainerWidget::AttachChild(child);
    // size a child's size properties have potentially changed, the
    // contents size properties need to be recalculated.
    DirtyContentsSizeProperties();
    // only update size stuff if not blocked
    if (ChildResizeBlockerCount() == 0)
    {
        // make sure that the min/max sizes are consistent with the contents
        CalculateMinAndMaxSizePropertiesFromContents();
        // attempt to resize the widget to the current size
        Resize(Size());
//         // propagate the changes up to the parent
//         ParentChildSizePropertiesUpdate(false);
    }
    else
        IndicateChildResizeWasBlocked();
}

void CellPaddingWidget::DetachChild (Widget *child)
{
    // make sure there is already a single child widget
    ASSERT0(SingleChildWidget() != NULL);
    // call the superclass to actually detach the child
    ContainerWidget::DetachChild(child);
    // size a child's size properties have potentially changed, the
    // contents size properties need to be recalculated.
    DirtyContentsSizeProperties();
    // only update size stuff if not blocked
    if (ChildResizeBlockerCount() == 0)
    {
        // make sure that the min/max sizes are consistent with the contents
        CalculateMinAndMaxSizePropertiesFromContents();
        // attempt to resize the widget to the current size
        Resize(Size());
//         // propagate the changes up to the parent
//         ParentChildSizePropertiesUpdate(false);
    }
    else
        IndicateChildResizeWasBlocked();
}

void CellPaddingWidget::ChildSizePropertiesChanged (Widget *child)
{
    ASSERT1(child == SingleChildWidget());
    // size a child's size properties have changed, the
    // contents size properties need to be recalculated.
    DirtyContentsSizeProperties();
    // only update size stuff if not blocked
    if (ChildResizeBlockerCount() == 0)
    {
        // make sure that the min/max sizes are consistent with the contents
        CalculateMinAndMaxSizePropertiesFromContents();
        // attempt to resize the widget to the current size
        Resize(Size());
//         // propagate the call up to this widget's parent
//         ParentChildSizePropertiesUpdate(false);
    }
    else
        IndicateChildResizeWasBlocked();
}

void CellPaddingWidget::PositionSingleChildWidget ()
{
    Widget *child = SingleChildWidget();
    if (child == NULL)
        return;

    // the contents rect will be used in the size calculations
    ScreenCoordRect contents_rect(ContentsRect());
    // get the amount of space left over
    ScreenCoordVector2 extra_space(contents_rect.Size() - child->Size());
    // calculate the child's offset from the parent, given its alignment
    ScreenCoordVector2 child_position_offset(contents_rect.BottomLeft() - ScreenRect().BottomLeft());
    for (Uint8 i = 0; i < 2; ++i)
    {
        switch (m_alignment.m[i])
        {
            case BOTTOM:
            case LEFT:
                child_position_offset.m[i] += 0;
                break;

            case CENTER:
                child_position_offset.m[i] += extra_space.m[i] / 2;
                break;

            case TOP:
            case RIGHT:
                child_position_offset.m[i] += extra_space.m[i];
                break;

            case SPACED:
                ASSERT1(false && "SPACED is invalid for use in CellPaddingWidget");
                break;

            default:
                ASSERT1(false && "Invalid Alignment");
                break;
        }
    }
    // move the child into position
    child->MoveTo(Position() + child_position_offset);
}

void CellPaddingWidget::DirtyContentsSizeProperties ()
{
    m_contents_size_properties_need_update = true;
}

void CellPaddingWidget::UpdateContentsSizeProperties () const
{
    // early out if no update necessary
    if (!m_contents_size_properties_need_update)
        return;

    m_contents_size_properties_need_update = false;

    // initialize the min size properties
    m_contents_size_properties.m_min_size_enabled = Bool2(false, false);
    m_contents_size_properties.m_min_size = ScreenCoordVector2::ms_zero;

    // initialize the max size properties
    m_contents_size_properties.m_max_size_enabled = Bool2(false, false);
    m_contents_size_properties.m_max_size =
        ScreenCoordVector2(
            SizeProperties::DefaultMaxSizeComponent(),
            SizeProperties::DefaultMaxSizeComponent());

    Widget const *child = SingleChildWidget();
    ASSERT1(child != NULL);

    // skip hidden and modal children
    if (child->IsHidden() || child->IsModal())
    {
        m_contents_size_properties = m_preferred_size_properties;
        return;
    }

    // iterate over X and Y dimensions
    for (Uint8 d = 0; d < 2; ++d)
    {
        m_contents_size_properties.m_min_size_enabled[d] =
            child->MinSizeEnabled()[d] ||
            m_preferred_size_properties.m_min_size_enabled[d];
        if (child->MinSizeEnabled()[d])
        {
            if (m_preferred_size_properties.m_min_size_enabled[d])
            {
                m_contents_size_properties.m_min_size[d] =
                    Max(child->MinSize()[d],
                        m_preferred_size_properties.m_min_size[d]);
            }
            else
            {
                m_contents_size_properties.m_min_size[d] = child->MinSize()[d];
            }
        }
        else
        {
            if (m_preferred_size_properties.m_min_size_enabled[d])
            {
                m_contents_size_properties.m_min_size[d] = m_preferred_size_properties.m_min_size[d];
            }
            else
            {
                // no min size enabled, so no min size
            }
        }

        m_contents_size_properties.m_max_size_enabled[d] =
            m_preferred_size_properties.m_max_size_enabled[d];
        if (m_preferred_size_properties.m_max_size_enabled[d])
        {
            if (child->MaxSizeEnabled()[d])
            {
                m_contents_size_properties.m_max_size[d] =
                    Max(child->MaxSize()[d],
                        m_preferred_size_properties.m_max_size[d]);
            }
            else
            {
                m_contents_size_properties.m_max_size[d] =
                    m_preferred_size_properties.m_max_size_enabled[d];
            }
        }
    }
}

} // end of namespace Xrb
