// ///////////////////////////////////////////////////////////////////////////
// xrb_layout.h by Victor Dods, created 2004/09/17
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_LAYOUT_H_)
#define _XRB_LAYOUT_H_

#include "xrb.h"

#include "xrb_enums.h"
#include "xrb_widget.h"

namespace Xrb
{

class Layout : public Widget
{
public:

    Layout (
        LineDirection major_direction,
        Uint32 major_count,
        Widget *parent,
        std::string const &name = "Layout");
    Layout (
        Orientation orientation,
        Widget *parent,
        std::string const &name = "Layout");
    virtual ~Layout ();

    inline LineDirection GetMajorDirection () const
    {
        return m_major_direction;
    }
    inline LineDirection GetMinorDirection () const
    {
        return (m_major_direction == ROW) ? COLUMN : ROW;
    }
    inline Uint32 GetMajorCount () const
    {
        return m_major_count;
    }
    Uint32 GetMinorCount () const
    {
        return (m_child_vector.size() + m_major_count - 1) / m_major_count;
    }

    inline bool GetIsUsingZeroedFrameMargins () const
    {
        return m_is_using_zeroed_frame_margins;
    }
    inline bool GetIsUsingZeroedLayoutSpacingMargins () const
    {
        return m_is_using_zeroed_layout_spacing_margins;
    }

    Widget *GetGridChild (
        Uint32 major_index,
        Uint32 minor_index) const;
    Widget *GetGridChildByColumnAndRow (
        Uint32 column_index,
        Uint32 row_index) const;

    ScreenCoordVector2 const &GetTotalSpacing () const;

    Uint32 GetColumnCount () const;
    Bool2 const &GetColumnMinSizeEnabled (Uint32 index) const;
    ScreenCoordVector2 const &GetColumnMinSize (Uint32 index) const;
    Bool2 const &GetColumnMaxSizeEnabled (Uint32 index) const;
    ScreenCoordVector2 const &GetColumnMaxSize (Uint32 index) const;
    ScreenCoord GetColumnWidth (Uint32 index) const;
    Uint32 GetHiddenColumnCount () const;

    Uint32 GetRowCount () const;
    Bool2 const &GetRowMinSizeEnabled (Uint32 index) const;
    ScreenCoordVector2 const &GetRowMinSize (Uint32 index) const;
    Bool2 const &GetRowMaxSizeEnabled (Uint32 index) const;
    ScreenCoordVector2 const &GetRowMaxSize (Uint32 index) const;
    ScreenCoord GetRowHeight (Uint32 index) const;
    Uint32 GetHiddenRowCount () const;

    virtual Bool2 GetContentsMinSizeEnabled () const;
    virtual ScreenCoordVector2 GetContentsMinSize () const;
    virtual Bool2 GetContentsMaxSizeEnabled () const;
    virtual ScreenCoordVector2 GetContentsMaxSize () const;

    void SetMajorDirection (LineDirection major_direction);
    void SetMajorCount (Uint32 major_count);

    void SetIsUsingZeroedFrameMargins (
        bool const is_using_zeroed_frame_margins);
    void SetIsUsingZeroedLayoutSpacingMargins (
        bool const is_using_zeroed_layout_spacing_margins);

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
        ASSERT0(false && "You can't set a main widget in a Layout")
    }

    virtual ScreenCoordVector2 Resize (ScreenCoordVector2 const &size);
    virtual void AttachChild (Widget *child);
    virtual void DetachChild (Widget *child);

    virtual void MoveChildDown (Widget *child);
    virtual void MoveChildUp (Widget *child);
    virtual void MoveChildToBottom (Widget *child);
    virtual void MoveChildToTop (Widget *child);

protected:

    inline ScreenCoordVector2 const &CalculateLayoutFrameMargins () const
    {
        return GetIsUsingZeroedFrameMargins() ?
               ScreenCoordVector2::ms_zero :
               GetWidgetSkinMargins(WidgetSkin::LAYOUT_FRAME_MARGINS);
    }
    inline ScreenCoordVector2 const &CalculateLayoutSpacingMargins () const
    {
        return GetIsUsingZeroedLayoutSpacingMargins() ?
               ScreenCoordVector2::ms_zero :
               GetWidgetSkinMargins(WidgetSkin::LAYOUT_SPACING_MARGINS);
    }

    // WidgetSkinHandler overrides
    virtual void HandleChangedWidgetSkinMargins (
        WidgetSkin::MarginsType margins_type);

    // NOT part of WidgetSkinHandler
    virtual void HandleChangedLayoutFrameMargins ();
    virtual void HandleChangedLayoutSpacingMargins ();

    virtual void ChildSizePropertiesChanged (Widget *child);
    virtual void ChildStackPriorityChanged (
        Widget *child,
        StackPriority previous_stack_priority);

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

    void Initialize (
        LineDirection major_direction,
        Uint32 major_count);

    // indicates if this grid is row-major or column-major
    LineDirection m_major_direction;
    // the number of widgets across the major direction (i.e. this value
    // would store the number of columns in a row-major grid layout
    Uint32 m_major_count;

    // indicates if the zero vector (true) or WidgetSkin frame margins
    // (false) is to be used for the frame margin size.
    bool m_is_using_zeroed_frame_margins;
    // indicates if the zero vector (true) or WidgetSkin layout spacing
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

#endif // !defined(_XRB_LAYOUT_H_)
