// ///////////////////////////////////////////////////////////////////////////
// xrb_layout.cpp by Victor Dods, created 2004/09/17
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_layout.hpp"

#include "xrb_screen.hpp"

namespace Xrb
{

Layout::Layout (
    LineDirection const major_direction,
    Uint32 const major_count,
    ContainerWidget *const parent,
    std::string const &name)
    :
    ContainerWidget(parent, name)
{
    Initialize(major_direction, major_count);
}

Layout::Layout (
    Orientation const orientation,
    ContainerWidget *const parent,
    std::string const &name)
    :
    ContainerWidget(parent, name)
{
    LineDirection major_direction;
    Uint32 major_count = 1;
    major_direction = (orientation == HORIZONTAL) ? COLUMN : ROW;

    Initialize(major_direction, major_count);
}

Layout::~Layout ()
{
    DeleteColumnSizePropertyArrays();
    DeleteRowSizePropertyArrays();
}

Widget *Layout::GetGridChild (
    Uint32 const major_index,
    Uint32 const minor_index) const
{
    ASSERT1(major_index < GetMinorCount());
    ASSERT1(minor_index < GetMajorCount());
    Uint32 linear_index = major_index * GetMajorCount() + minor_index;
    if (linear_index < m_child_vector.size())
        return m_child_vector[linear_index];
    else
        return 0;
}

Widget *Layout::GetGridChildByColumnAndRow (
    Uint32 const column_index,
    Uint32 const row_index) const
{
    ASSERT1(column_index < GetColumnCount());
    ASSERT1(row_index < GetRowCount());
    Uint32 linear_index = (m_major_direction == ROW) ?
                          row_index * GetColumnCount() + column_index :
                          column_index * GetRowCount() + row_index;
    if (linear_index < m_child_vector.size())
        return m_child_vector[linear_index];
    else
        return 0;
}

ScreenCoordVector2 const &Layout::GetTotalSpacing () const
{
    UpdateTotalSpacing();
    return m_total_spacing;
}

Uint32 Layout::GetColumnCount () const
{
    UpdateColumnSizeProperties();
    return m_column_count;
}

Bool2 const &Layout::GetColumnMinSizeEnabled (Uint32 const index) const
{
    UpdateColumnSizeProperties();
    ASSERT1(index < m_column_count);
    return m_column_size_properties[index].m_min_size_enabled;
}

ScreenCoordVector2 const &Layout::GetColumnMinSize (Uint32 const index) const
{
    UpdateColumnSizeProperties();
    ASSERT1(index < m_column_count);
    return m_column_size_properties[index].m_min_size;
}

Bool2 const &Layout::GetColumnMaxSizeEnabled (Uint32 const index) const
{
    UpdateColumnSizeProperties();
    ASSERT1(index < m_column_count);
    return m_column_size_properties[index].m_max_size_enabled;
}

ScreenCoordVector2 const &Layout::GetColumnMaxSize (Uint32 const index) const
{
    UpdateColumnSizeProperties();
    ASSERT1(index < m_column_count);
    return m_column_size_properties[index].m_max_size;
}

ScreenCoord Layout::GetColumnWidth (Uint32 const index) const
{
    UpdateColumnSizeProperties();
    ASSERT1(index < m_column_count);
    return m_column_width[index];
}

Uint32 Layout::GetHiddenColumnCount () const
{
    UpdateColumnSizeProperties();
    return m_hidden_column_count;
}

Uint32 Layout::GetRowCount () const
{
    UpdateRowSizeProperties();
    return m_row_count;
}

Bool2 const &Layout::GetRowMinSizeEnabled (Uint32 const index) const
{
    UpdateRowSizeProperties();
    ASSERT1(index < m_row_count);
    return m_row_size_properties[index].m_min_size_enabled;
}

ScreenCoordVector2 const &Layout::GetRowMinSize (Uint32 const index) const
{
    UpdateRowSizeProperties();
    ASSERT1(index < m_row_count);
    return m_row_size_properties[index].m_min_size;
}

Bool2 const &Layout::GetRowMaxSizeEnabled (Uint32 const index) const
{
    UpdateRowSizeProperties();
    ASSERT1(index < m_row_count);
    return m_row_size_properties[index].m_max_size_enabled;
}

ScreenCoordVector2 const &Layout::GetRowMaxSize (Uint32 const index) const
{
    UpdateRowSizeProperties();
    ASSERT1(index < m_row_count);
    return m_row_size_properties[index].m_max_size;
}

ScreenCoord Layout::GetRowHeight (Uint32 const index) const
{
    UpdateRowSizeProperties();
    ASSERT1(index < m_row_count);
    return m_row_height[index];
}

Uint32 Layout::GetHiddenRowCount () const
{
    UpdateRowSizeProperties();
    return m_hidden_row_count;
}

Bool2 Layout::GetContentsMinSizeEnabled () const
{
    UpdateContentsSizeProperties();
    return m_contents_size_properties.m_min_size_enabled;
}

ScreenCoordVector2 Layout::GetContentsMinSize () const
{
    UpdateContentsSizeProperties();
    return m_contents_size_properties.m_min_size;
}

Bool2 Layout::GetContentsMaxSizeEnabled () const
{
    UpdateContentsSizeProperties();
    return m_contents_size_properties.m_max_size_enabled;
}

ScreenCoordVector2 Layout::GetContentsMaxSize () const
{
    UpdateContentsSizeProperties();
    return m_contents_size_properties.m_max_size;
}

void Layout::SetMajorDirection (LineDirection const major_direction)
{
    if (m_major_direction != major_direction)
    {
        m_major_direction = major_direction;
        // the number of columns and rows is no longer valid, the
        // arrays must be reallocated
        DirtyColumnSizePropertyAllocations();
        DirtyRowSizePropertyAllocations();
    }
}

void Layout::SetMajorCount (Uint32 const major_count)
{
    ASSERT1(major_count > 0);

    if (m_major_count != major_count)
    {
        m_major_count = major_count;
        // the number of columns and rows is no longer valid, the
        // arrays must be reallocated
        DirtyColumnSizePropertyAllocations();
        DirtyRowSizePropertyAllocations();
    }
}

void Layout::SetIsUsingZeroedFrameMargins (
    bool const is_using_zeroed_frame_margins)
{
    if (m_is_using_zeroed_frame_margins != is_using_zeroed_frame_margins)
    {
        m_is_using_zeroed_frame_margins = is_using_zeroed_frame_margins;
        HandleChangedLayoutFrameMargins();
    }
}

void Layout::SetIsUsingZeroedLayoutSpacingMargins (
    bool const is_using_zeroed_layout_spacing_margins)
{
    if (m_is_using_zeroed_layout_spacing_margins !=
        is_using_zeroed_layout_spacing_margins)
    {
        m_is_using_zeroed_layout_spacing_margins =
            is_using_zeroed_layout_spacing_margins;
        HandleChangedLayoutSpacingMargins();
    }
}

void Layout::SetSizePropertyEnabled (
    SizeProperties::Property const property,
    Uint32 const component,
    bool const value)
{
    ASSERT1(component <= 1);
    if (property == SizeProperties::MIN)
        m_preferred_size_properties.m_min_size_enabled[component] = value;
    else
        m_preferred_size_properties.m_max_size_enabled[component] = value;
    ConstrainPreferredSizeProperties();
    ContainerWidget::SetSizePropertyEnabled(property, component, value, true);
    CalculateMinAndMaxSizePropertiesFromContents();
//     ParentChildSizePropertiesUpdate(false);
}

void Layout::SetSizePropertyEnabled (
    SizeProperties::Property const property,
    Bool2 const &value)
{
    if (property == SizeProperties::MIN)
        m_preferred_size_properties.m_min_size_enabled = value;
    else
        m_preferred_size_properties.m_max_size_enabled = value;
    ConstrainPreferredSizeProperties();
    ContainerWidget::SetSizePropertyEnabled(property, value, true);
    CalculateMinAndMaxSizePropertiesFromContents();
//     ParentChildSizePropertiesUpdate(false);
}

void Layout::SetSizeProperty (
    SizeProperties::Property const property,
    Uint32 const component,
    ScreenCoord const value)
{
    ASSERT1(component <= 1);
    ASSERT1(value >= 0);
    if (property == SizeProperties::MIN)
        m_preferred_size_properties.m_min_size[component] = value;
    else
        m_preferred_size_properties.m_max_size[component] = value;
    ConstrainPreferredSizeProperties();
    ContainerWidget::SetSizeProperty(property, component, value, true);
    CalculateMinAndMaxSizePropertiesFromContents();
//     ParentChildSizePropertiesUpdate(false);
}

void Layout::SetSizeProperty (
    SizeProperties::Property const property,
    ScreenCoordVector2 const &value)
{
    ASSERT1(value[Dim::X] >= 0);
    ASSERT1(value[Dim::Y] >= 0);
    if (property == SizeProperties::MIN)
        m_preferred_size_properties.m_min_size = value;
    else
        m_preferred_size_properties.m_max_size = value;
    ConstrainPreferredSizeProperties();
    ContainerWidget::SetSizeProperty(property, value, true);
    CalculateMinAndMaxSizePropertiesFromContents();
//     ParentChildSizePropertiesUpdate(false);
}

ScreenCoordVector2 Layout::Resize (ScreenCoordVector2 const &size)
{
    ContainerWidget::Resize(size);

    // only update size stuff if not blocked
    if (GetChildResizeBlockerCount() == 0)
    {
        DelegateWidthsToColumns();
        DelegateHeightsToRows();
        ResizeAndRepositionChildWidgets();
    }
    else
        IndicateChildResizeWasBlocked();

    return GetSize();
}

void Layout::AttachChild (Widget *const child)
{
    // call the superclass to actually attach the child
    ContainerWidget::AttachChild(child);
    // the number of widgets has changed, which means the cached number
    // of columns and rows isn't valid anymore, and must be reallocated
    DirtyTotalSpacing();
    DirtyColumnSizePropertyAllocations();
    DirtyRowSizePropertyAllocations();
    // only update size stuff if not blocked
    if (GetChildResizeBlockerCount() == 0)
    {
        // make sure that the min/max sizes are consistent with the contents
        CalculateMinAndMaxSizePropertiesFromContents();
        // attempt to resize the widget to the current size
        Resize(GetSize());
//         // propagate the changes up to the parent
//         ParentChildSizePropertiesUpdate(false);
    }
    else
        IndicateChildResizeWasBlocked();
}

void Layout::DetachChild (Widget *const child)
{
    // call the superclass to actually detach the child
    ContainerWidget::DetachChild(child);
    // the number of widgets has changed, which means the cached number
    // of columns and rows isn't valid anymore, and must be reallocated
    DirtyTotalSpacing();
    DirtyColumnSizePropertyAllocations();
    DirtyRowSizePropertyAllocations();
    // only update size stuff if not blocked
    if (GetChildResizeBlockerCount() == 0)
    {
        // make sure that the min/max sizes are consistent with the contents
        CalculateMinAndMaxSizePropertiesFromContents();
        // attempt to resize the widget to the current size
        Resize(GetSize());
//         // propagate the changes up to the parent
//         ParentChildSizePropertiesUpdate(false);
    }
    else
        IndicateChildResizeWasBlocked();
}

void Layout::MoveChildDown (Widget *const child)
{
    // call the superclass to actually move the child in the vector
    ContainerWidget::MoveChildDown(child);
    // moving existing children around in the layout could mean that
    // a column/row has become hidden/unhidden, so the contents size
    // properties are invalid
    DirtyTotalSpacing();
    DirtyColumnSizeProperties();
    DirtyRowSizeProperties();
    // only update size stuff if not blocked
    if (GetChildResizeBlockerCount() == 0)
    {
        // make sure that the min/max sizes are consistent with the contents
        CalculateMinAndMaxSizePropertiesFromContents();
        // attempt to resize the widget to the current size
        Resize(GetSize());
//         // propagate the changes up to the parent
//         ParentChildSizePropertiesUpdate(false);
    }
    else
        IndicateChildResizeWasBlocked();
}

void Layout::MoveChildUp (Widget *const child)
{
    // call the superclass to actually move the child in the vector
    ContainerWidget::MoveChildUp(child);
    // moving existing children around in the layout could mean that
    // a column/row has become hidden/unhidden, so the contents size
    // properties are invalid
    DirtyTotalSpacing();
    DirtyColumnSizeProperties();
    DirtyRowSizeProperties();
    // only update size stuff if not blocked
    if (GetChildResizeBlockerCount() == 0)
    {
        // make sure that the min/max sizes are consistent with the contents
        CalculateMinAndMaxSizePropertiesFromContents();
        // attempt to resize the widget to the current size
        Resize(GetSize());
//         // propagate the changes up to the parent
//         ParentChildSizePropertiesUpdate(false);
    }
    else
        IndicateChildResizeWasBlocked();
}

void Layout::MoveChildToBottom (Widget *const child)
{
    // call the superclass to actually move the child in the vector
    ContainerWidget::MoveChildToBottom(child);
    // moving existing children around in the layout could mean that
    // a column/row has become hidden/unhidden, so the contents size
    // properties are invalid
    DirtyTotalSpacing();
    DirtyColumnSizeProperties();
    DirtyRowSizeProperties();
    // only update size stuff if not blocked
    if (GetChildResizeBlockerCount() == 0)
    {
        // make sure that the min/max sizes are consistent with the contents
        CalculateMinAndMaxSizePropertiesFromContents();
        // attempt to resize the widget to the current size
        Resize(GetSize());
//         // propagate the changes up to the parent
//         ParentChildSizePropertiesUpdate(false);
    }
    else
        IndicateChildResizeWasBlocked();
}

void Layout::MoveChildToTop (Widget *const child)
{
    // call the superclass to actually move the child in the vector
    ContainerWidget::MoveChildToTop(child);
    // moving existing children around in the layout could mean that
    // a column/row has become hidden/unhidden, so the contents size
    // properties are invalid
    DirtyTotalSpacing();
    DirtyColumnSizeProperties();
    DirtyRowSizeProperties();
    // only update size stuff if not blocked
    if (GetChildResizeBlockerCount() == 0)
    {
        // make sure that the min/max sizes are consistent with the contents
        CalculateMinAndMaxSizePropertiesFromContents();
        // attempt to resize the widget to the current size
        Resize(GetSize());
//         // propagate the changes up to the parent
//         ParentChildSizePropertiesUpdate(false);
    }
    else
        IndicateChildResizeWasBlocked();
}

void Layout::HandleChangedWidgetSkinMargins (
    WidgetSkin::MarginsType const margins_type)
{
    if (margins_type == WidgetSkin::LAYOUT_FRAME_MARGINS)
        HandleChangedLayoutFrameMargins();
    else if (margins_type == WidgetSkin::LAYOUT_SPACING_MARGINS)
        HandleChangedLayoutSpacingMargins();
}

void Layout::HandleChangedLayoutFrameMargins ()
{
    DirtyTotalSpacing();
    DirtyContentsSizeProperties();
    ConstrainPreferredSizeProperties();
    // only update size stuff if not blocked
    if (GetChildResizeBlockerCount() == 0)
    {
        CalculateMinAndMaxSizePropertiesFromContents();
//         ParentChildSizePropertiesUpdate(false);
    }
    else
        IndicateChildResizeWasBlocked();
}

void Layout::HandleChangedLayoutSpacingMargins ()
{
    DirtyTotalSpacing();
    DirtyContentsSizeProperties();
    ConstrainPreferredSizeProperties();
    // only update size stuff if not blocked
    if (GetChildResizeBlockerCount() == 0)
    {
        CalculateMinAndMaxSizePropertiesFromContents();
//         ParentChildSizePropertiesUpdate(false);
    }
    else
        IndicateChildResizeWasBlocked();
}

void Layout::ChildSizePropertiesChanged (Widget *const child)
{
    // the child's size properties changed, so the column and row size
    // properties aren't valid anymore, and must be recalculated
    DirtyTotalSpacing();
    DirtyColumnSizeProperties();
    DirtyRowSizeProperties();
    // only update size stuff if not blocked
    if (GetChildResizeBlockerCount() == 0)
    {
        // make sure that the min/max sizes are consistent with the contents
        CalculateMinAndMaxSizePropertiesFromContents();
        // attempt to resize the widget to the current size
        Resize(GetSize());
//         // propagate the changes up to the parent
//         ParentChildSizePropertiesUpdate(false);
    }
    else
        IndicateChildResizeWasBlocked();
}

void Layout::ChildStackPriorityChanged (
    Widget *const child,
    StackPriority const previous_stack_priority)
{
    ContainerWidget::ChildStackPriorityChanged(child, previous_stack_priority);

    // the children have been reordered, so the column and row size
    // properties aren't valid anymore, and must be recalculated
    DirtyTotalSpacing();
    DirtyColumnSizeProperties();
    DirtyRowSizeProperties();
    // only update size stuff if not blocked
    if (GetChildResizeBlockerCount() == 0)
    {
        // make sure that the min/max sizes are consistent with the contents
        CalculateMinAndMaxSizePropertiesFromContents();
        // attempt to resize the widget to the current size
        Resize(GetSize());
//         // propagate the changes up to the parent
//         ParentChildSizePropertiesUpdate(false);
    }
    else
        IndicateChildResizeWasBlocked();
}

int Layout::SizePropertiesSortingFunction (
    SizeProperties const *const properties_a,
    SizeProperties const *const properties_b,
    Uint32 const index)
{
    ASSERT1(index <= 1);
    ASSERT1(properties_a != NULL);
    ASSERT1(properties_b != NULL);
    ASSERT1(properties_a->m_data != NULL);
    ASSERT1(properties_b->m_data != NULL);
    ASSERT1(properties_a->m_data == properties_b->m_data);
    ScreenCoord size_allotment = *properties_a->m_data;

    if (properties_a->m_max_size_enabled[index] && properties_a->m_max_size[index] < size_allotment)
    {
        if (properties_b->m_max_size_enabled[index] && properties_b->m_max_size[index] < size_allotment)
            // take the one with a smaller max size
            return properties_a->m_max_size[index] - properties_b->m_max_size[index];
        else
            // take a before b
            return -1;
    }
    else
    {
        if (properties_b->m_max_size_enabled[index] && properties_b->m_max_size[index] < size_allotment)
            // take b before a
            return 1;
    }

    if (properties_a->m_min_size_enabled[index] && properties_a->m_min_size[index] > size_allotment)
    {
        if (properties_b->m_min_size_enabled[index] && properties_b->m_min_size[index] > size_allotment)
            // take the one with a larger min size
            return properties_b->m_min_size[index] - properties_a->m_min_size[index];
        else
            // take a before b
            return -1;
    }
    else
    {
        if (properties_b->m_min_size_enabled[index] && properties_b->m_min_size[index] > size_allotment)
            // take b before a
            return 1;
    }

    // neither a nor b have priority.
    return 0;
}

int Layout::ColumnCompareConstraints (void const *a, void const *b)
{
    SizeProperties const *column_a =
        *(static_cast<SizeProperties *const *>(a));
    SizeProperties const *column_b =
        *(static_cast<SizeProperties *const *>(b));
    ASSERT1(dynamic_cast<SizeProperties const *>(column_a) != NULL);
    ASSERT1(dynamic_cast<SizeProperties const *>(column_b) != NULL);

    return SizePropertiesSortingFunction(column_a, column_b, Dim::X);
}

int Layout::RowCompareConstraints (void const *a, void const *b)
{
    SizeProperties const *row_a =
        *(static_cast<SizeProperties const *const *>(a));
    SizeProperties const *row_b =
        *(static_cast<SizeProperties const *const *>(b));
    ASSERT1(dynamic_cast<SizeProperties const *>(row_a) != NULL);
    ASSERT1(dynamic_cast<SizeProperties const *>(row_b) != NULL);

    return SizePropertiesSortingFunction(row_a, row_b, Dim::Y);
}

void Layout::DelegateWidthsToColumns ()
{
    // if there are no columns or no rows, we don't need to do anything
    if (GetColumnCount() == 0 || GetRowCount() == 0)
        return;

    // number of columns we're actually going to deal with
    Uint32 unhidden_column_count = 0;
    // make an array to point to the column size properties
#if defined(WIN32)
    SizeProperties const **column_order =
        new SizeProperties const *[m_column_count];
#else
    SizeProperties const *column_order[m_column_count];
#endif
    // fill the array
    for (Uint32 i = 0; i < m_column_count; ++i)
        if (!m_column_is_hidden[i])
            column_order[unhidden_column_count++] = &m_column_size_properties[i];

    // initialize the column widths to 0 (so that the hidden columns get a value)
    for (Uint32 i = 0; i < m_column_count; ++i)
        m_column_width[i] = 0;

    // must split up the total width among the columns
    ASSERT1(m_hidden_column_count == GetColumnCount() - unhidden_column_count);
    ScreenCoord total_width_left = GetWidth() - GetTotalSpacing()[Dim::X];
    ASSERT1(total_width_left >= 0);

    // sort the array unhidden_column_count times (to delegate the same number of widths)
    for (Uint32 i = 0; i < unhidden_column_count; ++i)
    {
        m_line_share_of_size[Dim::X] = total_width_left / (unhidden_column_count - i);
        // sort the columns starting at element i
        qsort(column_order + i,
              unhidden_column_count - i,
              sizeof(SizeProperties const *),
              Layout::ColumnCompareConstraints);

        // use the first element from the sorted list and delegate a width to it
        Uint32 column_index = column_order[i] - m_column_size_properties;
        m_column_width[column_index] =
            column_order[i]->AdjustedSizeComponent(Dim::X, m_line_share_of_size[Dim::X]);

        // update the remaining width left
        total_width_left -= m_column_width[column_index];
        ASSERT1(total_width_left >= 0);
    }

#if defined(WIN32)
    Delete(column_order);
#endif
}

void Layout::DelegateHeightsToRows ()
{
    // if there are no columns or no rows, we don't need to do anything
    if (GetColumnCount() == 0 || GetRowCount() == 0)
        return;

    // number of rows we're actually going to deal with
    Uint32 unhidden_row_count = 0;
    // make an array to point to the row size properties
#if defined(WIN32)
    SizeProperties const **row_order =
        new SizeProperties const *[m_row_count];
#else
    SizeProperties const *row_order[m_row_count];
#endif
    // fill the array
    for (Uint32 i = 0; i < m_row_count; ++i)
        if (!m_row_is_hidden[i])
            row_order[unhidden_row_count++] = &m_row_size_properties[i];

    // initialize the row heights to 0 (so that the hidden rows get a value)
    for (Uint32 i = 0; i < m_row_count; ++i)
        m_row_height[i] = 0;

    // must split up the total height among the rows
    ASSERT1(m_hidden_row_count == GetRowCount() - unhidden_row_count);
    ScreenCoord total_height_left = GetHeight() - GetTotalSpacing()[Dim::Y];
    ASSERT1(total_height_left >= 0);

    // sort the array unhidden_row_count times (to delegate the same number of heights)
    for (Uint32 i = 0; i < unhidden_row_count; ++i)
    {
        m_line_share_of_size[Dim::Y] = total_height_left / (unhidden_row_count - i);
        ASSERT1(m_line_share_of_size[Dim::Y] >= 0);
        // sort the rows starting at element i
        qsort(row_order + i,
              unhidden_row_count - i,
              sizeof(SizeProperties const *),
              Layout::RowCompareConstraints);

        // use the first element from the sorted list and delegate a height to it
        Uint32 row_index = row_order[i] - m_row_size_properties;
        m_row_height[row_index] =
            row_order[i]->AdjustedSizeComponent(Dim::Y, m_line_share_of_size[Dim::Y]);

        // update the remaining height left
        total_height_left -= m_row_height[row_index];
        ASSERT1(total_height_left >= 0);
    }

#if defined(WIN32)
    Delete(row_order);
#endif
}

void Layout::ResizeAndRepositionChildWidgets ()
{
    ScreenCoordVector2 positional_offset;
    ScreenCoordVector2 layout_frame_margins(CalculateLayoutFrameMargins());
    ScreenCoordVector2 layout_spacing_margins(CalculateLayoutSpacingMargins());

    positional_offset[Dim::Y] = GetPosition()[Dim::Y] + layout_frame_margins[Dim::Y];
    for (Uint32 row = m_row_count - 1; row < m_row_count; --row)
    {
        if (m_row_is_hidden[row])
            continue;

        positional_offset[Dim::X] = GetPosition()[Dim::X] + layout_frame_margins[Dim::X];
        for (Uint32 column = 0; column < m_column_count; ++column)
        {
            if (m_column_is_hidden[column])
                continue;

            Widget *child = GetGridChildByColumnAndRow(column, row);

            // only resize and reposition non-NULL, unhidden and non-modal widgets
            if (child != NULL &&
                !child->IsHidden() &&
                !child->IsModal())
            {
                ScreenCoordVector2 requested_size(
                    m_column_width[column],
                    m_row_height[row]);

                // resize the child using the corresponding column and row sizes
                child->Resize(requested_size);
                // if the child didn't use up all the space, then calculate
                // how much extra space there is so that the child can be
                // properly centered on the grid slot.
                ScreenCoordVector2 extra_space(requested_size - child->GetSize());
//                 // these asserts seem to fuck things up, so we'll see if they can be taken out
//                 ASSERT1(extra_space[Dim::X] >= 0);
//                 ASSERT1(extra_space[Dim::Y] >= 0);
                // move the child to the tracked current positional offset,
                // plus half of the extra space, so the child is centered
                // on the grid slot.
                child->MoveTo(positional_offset + extra_space / 2);
            }

            positional_offset[Dim::X] += m_column_width[column] + layout_spacing_margins[Dim::X];
        }

        positional_offset[Dim::Y] += m_row_height[row] + layout_spacing_margins[Dim::Y];
    }
}

void Layout::ConstrainPreferredSizeProperties ()
{
    for (Uint8 i = 0; i < 2; ++i)
    {
        if (m_preferred_size_properties.m_min_size[i] < GetTotalSpacing()[i])
            m_preferred_size_properties.m_min_size[i] = GetTotalSpacing()[i];

        if (m_preferred_size_properties.m_max_size[i] < GetTotalSpacing()[i])
            m_preferred_size_properties.m_max_size[i] = GetTotalSpacing()[i];
    }
}

void Layout::CalculateLineSizeProperties (
    LineDirection const line_direction,
    Uint32 const line_index,
    SizeProperties *const size_properties,
    bool *const line_is_hidden) const
{
    if (line_direction == GetMajorDirection())
        ASSERT1(line_index < GetMinorCount());
    else
        ASSERT1(line_index < GetMajorCount());
    ASSERT1(size_properties != NULL);
    ASSERT1(line_is_hidden != NULL);

    size_properties->m_min_size_enabled = Bool2(false, false);
    size_properties->m_min_size =
        ScreenCoordVector2(
            SizeProperties::GetDefaultMinSizeComponent(),
            SizeProperties::GetDefaultMinSizeComponent());

    Uint32 is_horizontal = (line_direction == ROW) ? 1 : 0;
    // figure out which dimension is along this line
    Uint32 along = 1 - is_horizontal;
    // get the 'across' dimension
    Uint32 across = 1 - along;

    // initialize the max size enabled
    size_properties->m_max_size_enabled[along] = m_child_vector.size() > 0;
    size_properties->m_max_size_enabled[across] = false;

    // initialize the max size
    size_properties->m_max_size[along] = 0;
    size_properties->m_max_size[across] =
        SizeProperties::GetDefaultMaxSizeComponent();

    // loop through all the child widgets
    Uint32 line_widget_count =
        (line_direction == GetMajorDirection()) ?
        GetMajorCount() :
        GetMinorCount();
    // the total number of hidden child widgets
    Uint32 hidden_child_widget_count = 0;
    for (Uint32 i = 0; i < line_widget_count; ++i)
    {
        Widget const *child;
        if (line_direction == GetMajorDirection())
            child = GetGridChild(line_index, i);
        else
            child = GetGridChild(i, line_index);

        // skip empty grid slots
        if (child == NULL || // empty slot counts as hidden
            child->IsHidden() || // actually hidden
            child->IsModal()) // modal widgets are reparented to the Screen
        {
            ++hidden_child_widget_count;
            continue;
        }

        // if at least one child has minsize enabled in the along direction,
        // then this layout has minsize enabled in the along direction.
        size_properties->m_min_size_enabled[along] =
            size_properties->m_min_size_enabled[along] ||
            child->GetMinSizeEnabled()[along];
        // if at least one child has minsize enabled in the across direction,
        // then this layout has minsize enabled in the across direction.
        size_properties->m_min_size_enabled[across] =
            size_properties->m_min_size_enabled[across] ||
            child->GetMinSizeEnabled()[across];

        // check if there's a min measurement for the 'along' dimension
        if (child->GetMinSizeEnabled()[along])
            // if there is, then add it to the total
            size_properties->m_min_size[along] += child->GetMinSize()[along];

        // check if there's a min measurement for the 'across' dimension
        if (child->GetMinSizeEnabled()[across])
            // if there is, then check it against the current across min
            if (size_properties->m_min_size[across] <
                child->GetMinSize()[across])
                // assign it only if it's the new largest min size
                size_properties->m_min_size[across] =
                    child->GetMinSize()[across];

        // if all child widgets have maxsize enabled in the along direction,
        // then this line has maxsize enabled in the along direction.
        size_properties->m_max_size_enabled[along] =
            size_properties->m_max_size_enabled[along] &&
            child->GetMaxSizeEnabled()[along];
        // if at least one child has maxsize enabled in the across direction,
        // then this line has maxsize enabled in the across direction.
        size_properties->m_max_size_enabled[across] =
            size_properties->m_max_size_enabled[across] ||
            child->GetMaxSizeEnabled()[across];

        // check if there's a max measurement for the 'along' dimension
        if (child->GetMaxSizeEnabled()[along])
            // if there is, add it to the total
            size_properties->m_max_size[along] += child->GetMaxSize()[along];

        // check if there's a max measurement for the 'across' dimension
        if (child->GetMaxSizeEnabled()[across])
            // if there is, then check it against the current across max
            if (size_properties->m_max_size[across] >
                child->GetMaxSize()[across])
                // assign it only if its the new smallest max size
                size_properties->m_max_size[across] =
                    child->GetMaxSize()[across];
    }

    // make sure that the max sizes are not smaller
    // than the min sizes (if enabled)
    size_properties->m_max_size =
        size_properties->AdjustedSize(size_properties->m_max_size);

    // if all child widgets are hidden, then this line is hidden
    *line_is_hidden = (hidden_child_widget_count == line_widget_count);
}

void Layout::DirtyTotalSpacing ()
{
    m_total_spacing_needs_update = true;
}

void Layout::DirtyColumnSizePropertyAllocations ()
{
    m_column_size_property_arrays_need_reallocation = true;
    m_total_spacing_needs_update = true;
    DirtyColumnSizeProperties();
}

void Layout::DirtyColumnSizeProperties ()
{
    m_column_size_properties_need_update = true;
    DirtyContentsSizeProperties();
}

void Layout::DirtyRowSizePropertyAllocations ()
{
    m_row_size_property_arrays_need_reallocation = true;
    m_total_spacing_needs_update = true;
    DirtyRowSizeProperties();
}

void Layout::DirtyRowSizeProperties ()
{
    m_row_size_properties_need_update = true;
    DirtyContentsSizeProperties();
}

void Layout::DirtyContentsSizeProperties ()
{
    m_contents_size_properties_need_update = true;
}

void Layout::UpdateTotalSpacing () const
{
    // early out if no update necessary
    if (!m_total_spacing_needs_update)
        return;

    m_total_spacing_needs_update = false;

    Uint32 column_spaces;
    Uint32 row_spaces;

    if (GetColumnCount() == 0 || GetColumnCount() == GetHiddenColumnCount())
        column_spaces = 0;
    else
    {
        ASSERT1(GetColumnCount() > GetHiddenColumnCount());
        column_spaces = GetColumnCount() - 1 - GetHiddenColumnCount();
    }

    if (GetRowCount() == 0 || GetRowCount() == GetHiddenRowCount())
        row_spaces = 0;
    else
    {
        ASSERT1(GetRowCount() > GetHiddenRowCount());
        row_spaces = GetRowCount() - 1 - GetHiddenRowCount();
    }

    m_total_spacing =
        CalculateLayoutFrameMargins() * 2 +
        ScreenCoordVector2(
            CalculateLayoutSpacingMargins()[Dim::X] * column_spaces,
            CalculateLayoutSpacingMargins()[Dim::Y] * row_spaces);
}

void Layout::UpdateColumnSizePropertyAllocation () const
{
    // early out if no update necessary
    if (!m_column_size_property_arrays_need_reallocation)
        return;

    m_column_size_property_arrays_need_reallocation = false;
    ASSERT1(m_column_size_properties_need_update);

    // figure out how many columns there should be
    Uint32 column_count =
        (GetMajorDirection() == COLUMN) ?
        GetMinorCount() :
        GetMajorCount();

    if (m_column_count > 0)
    {
        ASSERT1(m_column_size_properties != NULL);
        ASSERT1(m_column_width != NULL);
        ASSERT1(m_column_is_hidden != NULL);
    }

    // don't do anything if the number of columns is the same
    if (m_column_count == column_count)
        return;

    DeleteColumnSizePropertyArrays();

    ASSERT1(m_column_size_properties == NULL);
    ASSERT1(m_column_width == NULL);
    ASSERT1(m_column_is_hidden == NULL);

    m_column_count = column_count;
    if (m_column_count > 0)
    {
        m_column_size_properties = new SizeProperties[m_column_count];
        for (Uint32 i = 0; i < m_column_count; ++i)
            m_column_size_properties[i].m_data = &m_line_share_of_size[Dim::X];
        m_column_width = new ScreenCoord[m_column_count];
        m_column_is_hidden = new bool[m_column_count];
    }
}

void Layout::UpdateColumnSizeProperties () const
{
    UpdateColumnSizePropertyAllocation();

    // early out if no update necessary
    if (!m_column_size_properties_need_update)
        return;

    m_column_size_properties_need_update = false;

    m_hidden_column_count = 0;
    for (Uint32 i = 0; i < m_column_count; ++i)
    {
        CalculateLineSizeProperties(
            COLUMN,
            i,
            m_column_size_properties + i,
            m_column_is_hidden + i);
        if (m_column_is_hidden[i])
            ++m_hidden_column_count;
    }
}

void Layout::UpdateRowSizePropertyAllocation () const
{
    // early out if no update necessary
    if (!m_row_size_property_arrays_need_reallocation)
        return;

    m_row_size_property_arrays_need_reallocation = false;
    ASSERT1(m_row_size_properties_need_update);

    // figure out how many rows there should be
    Uint32 row_count = (GetMajorDirection() == ROW) ?
                       GetMinorCount() :
                       GetMajorCount();

    if (m_row_count > 0)
    {
        ASSERT1(m_row_size_properties != NULL);
        ASSERT1(m_row_height != NULL);
        ASSERT1(m_row_is_hidden != NULL);
    }

    // don't do anything if the number of rows is the same
    if (m_row_count == row_count)
        return;

    DeleteRowSizePropertyArrays();

    ASSERT1(m_row_size_properties == NULL);
    ASSERT1(m_row_height == NULL);
    ASSERT1(m_row_is_hidden == NULL);

    m_row_count = row_count;
    if (m_row_count > 0)
    {
        m_row_size_properties = new SizeProperties[m_row_count];
        for (Uint32 i = 0; i < m_row_count; ++i)
            m_row_size_properties[i].m_data = &m_line_share_of_size[Dim::Y];
        m_row_height = new ScreenCoord[m_row_count];
        m_row_is_hidden = new bool[m_row_count];
    }
}

void Layout::UpdateRowSizeProperties () const
{
    UpdateRowSizePropertyAllocation();

    // early out if no update necessary
    if (!m_row_size_properties_need_update)
        return;

    m_row_size_properties_need_update = false;

    m_hidden_row_count = 0;
    for (Uint32 i = 0; i < m_row_count; ++i)
    {
        CalculateLineSizeProperties(
            ROW,
            i,
            m_row_size_properties + i,
            m_row_is_hidden + i);
        if (m_row_is_hidden[i])
            ++m_hidden_row_count;
    }
}

void Layout::UpdateContentsSizeProperties () const
{
    // early out if no update necessary
    if (!m_contents_size_properties_need_update)
        return;

    m_contents_size_properties_need_update = false;

    // initialize the min size properties, considering the total spacing
    m_contents_size_properties.m_min_size_enabled =
        Bool2(GetTotalSpacing()[Dim::X] > 0,
              GetTotalSpacing()[Dim::Y] > 0);
    m_contents_size_properties.m_min_size = GetTotalSpacing();

    // initialize the max size properties, considering the total spacing
    m_contents_size_properties.m_max_size_enabled =
        Bool2(GetColumnCount() - m_hidden_column_count > 0,
              GetRowCount() - m_hidden_row_count > 0);
    m_contents_size_properties.m_max_size = GetTotalSpacing();

    // horizontal using columns
    for (Uint32 i = 0; i < GetColumnCount(); ++i)
    {
        // don't process hidden columns
        if (m_column_is_hidden[i])
            continue;

        // if one column has a min size, then the contents as a whole
        // have a min size (true value overcomes false value)
        m_contents_size_properties.m_min_size_enabled[Dim::X] =
            m_contents_size_properties.m_min_size_enabled[Dim::X] ||
            GetColumnMinSizeEnabled(i)[Dim::X];
        // add up the min sizes if the min size applies
        if (GetColumnMinSizeEnabled(i)[Dim::X])
            m_contents_size_properties.m_min_size[Dim::X] +=
                GetColumnMinSize(i)[Dim::X];

        // if one column doesn't have a max size, then the contents as a whole
        // don't have a max size (false value overcomes true value)
        m_contents_size_properties.m_max_size_enabled[Dim::X] =
            m_contents_size_properties.m_max_size_enabled[Dim::X] &&
            GetColumnMaxSizeEnabled(i)[Dim::X];
        // add up the max sizes if the max size applies
        if (GetColumnMaxSizeEnabled(i)[Dim::X])
            m_contents_size_properties.m_max_size[Dim::X] +=
                GetColumnMaxSize(i)[Dim::X];
    }

    // vertical using rows
    for (Uint32 i = 0; i < GetRowCount(); ++i)
    {
        // don't process hidden rows
        if (m_row_is_hidden[i])
            continue;

        // if one row has a min size, then the contents as a whole
        // have a min size (true value overcomes false value)
        m_contents_size_properties.m_min_size_enabled[Dim::Y] =
            m_contents_size_properties.m_min_size_enabled[Dim::Y] ||
            GetRowMinSizeEnabled(i)[Dim::Y];
        // add up the min sizes if the min size applies
        if (GetRowMinSizeEnabled(i)[Dim::Y])
            m_contents_size_properties.m_min_size[Dim::Y] +=
                GetRowMinSize(i)[Dim::Y];

        // if one row doesn't have a max size, then the contents as a whole
        // don't have a max size (false value overcomes true value)
        m_contents_size_properties.m_max_size_enabled[Dim::Y] =
            m_contents_size_properties.m_max_size_enabled[Dim::Y] &&
            GetRowMaxSizeEnabled(i)[Dim::Y];
        // add up the max sizes if the max size applies
        if (GetRowMaxSizeEnabled(i)[Dim::Y])
            m_contents_size_properties.m_max_size[Dim::Y] +=
                GetRowMaxSize(i)[Dim::Y];
    }
}

void Layout::DeleteColumnSizePropertyArrays () const
{
    if (m_column_size_properties != NULL)
    {
        DeleteArrayAndNullify(m_column_size_properties);
        DeleteArrayAndNullify(m_column_width);
        DeleteArrayAndNullify(m_column_is_hidden);
    }
    else
    {
        ASSERT1(m_column_width == NULL);
        ASSERT1(m_column_is_hidden == NULL);
    }
}

void Layout::DeleteRowSizePropertyArrays () const
{
    if (m_row_size_properties != NULL)
    {
        DeleteArrayAndNullify(m_row_size_properties);
        DeleteArrayAndNullify(m_row_height);
        DeleteArrayAndNullify(m_row_is_hidden);
    }
    else
    {
        ASSERT1(m_row_height == NULL);
        ASSERT1(m_row_is_hidden == NULL);
    }
}

void Layout::Initialize (
    LineDirection const major_direction,
    Uint32 const major_count)
{
    // doesn't independently accept focus
    m_accepts_focus = false;

    ASSERT1(major_count > 0);

    DirtyTotalSpacing();

    DirtyColumnSizePropertyAllocations();
    m_column_count = 0;
    m_column_size_properties = NULL;
    m_column_width = NULL;
    m_column_is_hidden = NULL;

    DirtyRowSizePropertyAllocations();
    m_row_count = 0;
    m_row_size_properties = NULL;
    m_row_height = NULL;
    m_row_is_hidden = NULL;

    m_major_count = major_count;
    m_major_direction = major_direction;

    m_is_using_zeroed_frame_margins = true;
    m_is_using_zeroed_layout_spacing_margins = false;

    SetBackground(NULL);
}

} // end of namespace Xrb
