// ///////////////////////////////////////////////////////////////////////////
// xrb_layout.hpp by Victor Dods, created 2004/09/17
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_LAYOUT_HPP_)
#define _XRB_LAYOUT_HPP_

#include "xrb.hpp"

#include "xrb_containerwidget.hpp"
#include "xrb_enums.hpp"

namespace Xrb {

class Layout : public ContainerWidget
{
public:

    Layout (LineDirection major_direction, Uint32 major_count, WidgetContext &context, std::string const &name = "Layout");
    Layout (Orientation orientation, WidgetContext &context, std::string const &name = "Layout");
    virtual ~Layout ();

    LineDirection MajorDirection () const { return m_major_direction; }
    LineDirection MinorDirection () const { return (m_major_direction == ROW) ? COLUMN : ROW; }
    Uint32 MajorCount () const { return m_major_count; }
    Uint32 MinorCount () const { return (m_child_vector.size() + m_major_count - 1) / m_major_count; }

    bool IsUsingZeroedFrameMargins () const { return m_is_using_zeroed_frame_margins; }
    bool IsUsingZeroedLayoutSpacingMargins () const { return m_is_using_zeroed_layout_spacing_margins; }

    Widget *GridChild (Uint32 major_index, Uint32 minor_index) const;
    Widget *GridChildByColumnAndRow (Uint32 column_index, Uint32 row_index) const;

    ScreenCoordVector2 const &TotalSpacing () const;

    Uint32 ColumnCount () const;
    Bool2 const &ColumnMinSizeEnabled (Uint32 index) const;
    ScreenCoordVector2 const &ColumnMinSize (Uint32 index) const;
    Bool2 const &ColumnMaxSizeEnabled (Uint32 index) const;
    ScreenCoordVector2 const &ColumnMaxSize (Uint32 index) const;
    ScreenCoord ColumnWidth (Uint32 index) const;
    Uint32 HiddenColumnCount () const;

    Uint32 RowCount () const;
    Bool2 const &RowMinSizeEnabled (Uint32 index) const;
    ScreenCoordVector2 const &RowMinSize (Uint32 index) const;
    Bool2 const &RowMaxSizeEnabled (Uint32 index) const;
    ScreenCoordVector2 const &RowMaxSize (Uint32 index) const;
    ScreenCoord RowHeight (Uint32 index) const;
    Uint32 HiddenRowCount () const;

    virtual Bool2 ContentsMinSizeEnabled () const;
    virtual ScreenCoordVector2 ContentsMinSize () const;
    virtual Bool2 ContentsMaxSizeEnabled () const;
    virtual ScreenCoordVector2 ContentsMaxSize () const;

    void SetMajorDirection (LineDirection major_direction);
    void SetMajorCount (Uint32 major_count);

    void SetIsUsingZeroedFrameMargins (bool is_using_zeroed_frame_margins);
    void SetIsUsingZeroedLayoutSpacingMargins (bool is_using_zeroed_layout_spacing_margins);

    virtual void SetSizePropertyEnabled (SizeProperties::Property property, Uint32 component, bool value);
    virtual void SetSizePropertyEnabled (SizeProperties::Property property, Bool2 const &value);
    virtual void SetSizeProperty (SizeProperties::Property property, Uint32 component, ScreenCoord value);
    virtual void SetSizeProperty (SizeProperties::Property property, ScreenCoordVector2 const &value);
    
    virtual void SetMainWidget (Widget *main_widget)
    {
        // disallowed
        ASSERT0(false && "You can't set a main widget in a Layout");
    }

    virtual ScreenCoordVector2 Resize (ScreenCoordVector2 const &size);
    virtual void AttachChild (Widget *child);
    virtual void DetachChild (Widget *child);

    virtual void MoveChildDown (Widget *child);
    virtual void MoveChildUp (Widget *child);
    virtual void MoveChildToBottom (Widget *child);
    virtual void MoveChildToTop (Widget *child);

protected:

    ScreenCoordMargins CalculateLayoutFrameMargins () const;
    ScreenCoordMargins CalculateLayoutSpacingMargins () const;

    virtual void HandleChangedStyleSheet ();

    virtual void HandleChangedLayoutFrameMargins ();
    virtual void HandleChangedLayoutSpacingMargins ();

    virtual void ChildSizePropertiesChanged (Widget *child);
    virtual void ChildStackPriorityChanged (Widget *child, StackPriority previous_stack_priority);

private:

    static int SizePropertiesSortingFunction (
        SizeProperties const *properties_a,
        SizeProperties const *properties_b,
        Uint32 index);

    static int ColumnCompareConstraints (void const *a, void const *b);
    static int RowCompareConstraints (void const *a, void const *b);

    // delegates widths to the columns in this layout (and then to the
    // child widgets in each column)
    void DelegateWidthsToColumns ();
    // delegates widths to the columns in this layout (and then to the
    // child widgets in each column)
    void DelegateHeightsToRows ();
    // given the delegated column widths and row heights, resize the
    // child widgets to the corresponding column/row height and reposition
    // them so they correctly lay out in the grid
    void ResizeAndRepositionChildWidgets ();

    // makes sure that the preferred size properties are >= the total spacing
    void ConstrainPreferredSizeProperties ();
    // calculates the size properties for a single row or column
    void CalculateLineSizeProperties (
        LineDirection line_direction,
        Uint32 line_index,
        SizeProperties *size_properties,
        bool *line_is_hidden) const;

    // the functions which 'dirty' the caches
    void DirtyTotalSpacing ();
    void DirtyColumnSizePropertyAllocations ();
    void DirtyColumnSizeProperties ();
    void DirtyRowSizePropertyAllocations ();
    void DirtyRowSizeProperties ();
    void DirtyContentsSizeProperties ();

    // the functions which 'undirty' the caches
    void UpdateTotalSpacing () const;
    void UpdateColumnSizePropertyAllocation () const;
    void UpdateColumnSizeProperties () const;
    void UpdateRowSizePropertyAllocation () const;
    void UpdateRowSizeProperties () const;
    void UpdateContentsSizeProperties () const;

    void DeleteColumnSizePropertyArrays () const;
    void DeleteRowSizePropertyArrays () const;

    void Initialize (LineDirection major_direction, Uint32 major_count);

    // indicates if this grid is row-major or column-major
    LineDirection m_major_direction;
    // the number of widgets across the major direction (i.e. this value
    // would store the number of columns in a row-major grid layout
    Uint32 m_major_count;

    // indicates if the zero vector (true) or StyleSheet frame margins
    // (false) is to be used for the frame margin size.
    bool m_is_using_zeroed_frame_margins;
    // indicates if the zero vector (true) or StyleSheet layout spacing
    // margins (false) is to be used for the layout spacing margin size.
    bool m_is_using_zeroed_layout_spacing_margins;

    mutable bool m_total_spacing_needs_update;
    // the total space due to frame and spacing margins
    mutable ScreenCoordVector2 m_total_spacing;

    mutable bool m_column_size_property_arrays_need_reallocation;
    mutable bool m_column_size_properties_need_update;
    // the size of the column size property arrays
    mutable Uint32 m_column_count;
    mutable SizeProperties *m_column_size_properties;
    mutable ScreenCoord *m_column_width;
    mutable bool *m_column_is_hidden;
    mutable Uint32 m_hidden_column_count;

    mutable bool m_row_size_property_arrays_need_reallocation;
    mutable bool m_row_size_properties_need_update;
    // the size of the row size property arrays
    mutable Uint32 m_row_count;
    mutable SizeProperties *m_row_size_properties;
    mutable ScreenCoord *m_row_height;
    mutable bool *m_row_is_hidden;
    mutable Uint32 m_hidden_row_count;

    // indicates if the cached size properties values must be recalculated
    mutable bool m_contents_size_properties_need_update;
    mutable SizeProperties m_contents_size_properties;

    // used in sorting the columns/rows while adjusting widget sizes
    ScreenCoordVector2 m_line_share_of_size;
}; // end of class Layout

} // end of namespace Xrb

#endif // !defined(_XRB_LAYOUT_HPP_)
