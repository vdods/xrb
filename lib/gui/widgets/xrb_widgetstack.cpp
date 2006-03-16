// ///////////////////////////////////////////////////////////////////////////
// xrb_widgetstack.cpp by Victor Dods, created 2005/12/16
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_widgetstack.h"

#include "xrb_screen.h"

namespace Xrb
{

Bool2 WidgetStack::GetContentsMinSizeEnabled () const
{
    UpdateContentsSizeProperties();
    return m_contents_size_properties.m_min_size_enabled;
}

ScreenCoordVector2 WidgetStack::GetContentsMinSize () const
{
    UpdateContentsSizeProperties();
    return m_contents_size_properties.m_min_size;
}

Bool2 WidgetStack::GetContentsMaxSizeEnabled () const
{
    UpdateContentsSizeProperties();
    return m_contents_size_properties.m_max_size_enabled;
}

ScreenCoordVector2 WidgetStack::GetContentsMaxSize () const
{
    UpdateContentsSizeProperties();
    return m_contents_size_properties.m_max_size;
}

void WidgetStack::SetSizePropertyEnabled (
    SizeProperties::Property const property,
    Uint32 const component,
    bool const value)
{
    ASSERT1(component <= 1)
    if (property == SizeProperties::MIN)
        m_preferred_size_properties.m_min_size_enabled[component] = value;
    else
        m_preferred_size_properties.m_max_size_enabled[component] = value;
    Widget::SetSizePropertyEnabled(property, component, value, true);
    CalculateMinAndMaxSizePropertiesFromContents();
    ParentChildSizePropertiesUpdate(false);
}

void WidgetStack::SetSizePropertyEnabled (
    SizeProperties::Property const property,
    Bool2 const &value)
{
    if (property == SizeProperties::MIN)
        m_preferred_size_properties.m_min_size_enabled = value;
    else
        m_preferred_size_properties.m_max_size_enabled = value;
    Widget::SetSizePropertyEnabled(property, value, true);
    CalculateMinAndMaxSizePropertiesFromContents();
    ParentChildSizePropertiesUpdate(false);
}

void WidgetStack::SetSizeProperty (
    SizeProperties::Property const property,
    Uint32 const component,
    ScreenCoord const value)
{
    ASSERT1(component <= 1)
    ASSERT1(value >= 0)
    if (property == SizeProperties::MIN)
        m_preferred_size_properties.m_min_size[component] = value;
    else
        m_preferred_size_properties.m_max_size[component] = value;
    Widget::SetSizeProperty(property, component, value, true);
    CalculateMinAndMaxSizePropertiesFromContents();
    ParentChildSizePropertiesUpdate(false);
}

void WidgetStack::SetSizeProperty (
    SizeProperties::Property const property,
    ScreenCoordVector2 const &value)
{
    ASSERT1(value[Dim::X] >= 0)
    ASSERT1(value[Dim::Y] >= 0)
    if (property == SizeProperties::MIN)
        m_preferred_size_properties.m_min_size = value;
    else
        m_preferred_size_properties.m_max_size = value;
    Widget::SetSizeProperty(property, value, true);
    CalculateMinAndMaxSizePropertiesFromContents();
    ParentChildSizePropertiesUpdate(false);
}

void WidgetStack::SetSizePropertyRatio (
    SizeProperties::Property const property,
    Uint32 const component,
    Float const ratio)
{
    ScreenCoord size_ratio_basis = GetTopLevelParent()->GetSizeRatioBasis();
    ScreenCoord calculated_value =
        static_cast<ScreenCoord>(size_ratio_basis * ratio);
    WidgetStack::SetSizeProperty(property, component, calculated_value);
}

void WidgetStack::SetSizePropertyRatios (
    SizeProperties::Property const property,
    FloatVector2 const &ratios)
{
    ScreenCoord size_ratio_basis = GetTopLevelParent()->GetSizeRatioBasis();
    ScreenCoordVector2 calculated_value =
        (static_cast<Float>(size_ratio_basis) * ratios).StaticCast<ScreenCoord>();
    WidgetStack::SetSizeProperty(property, calculated_value);
}

ScreenCoordVector2 WidgetStack::Resize (ScreenCoordVector2 const &size)
{
    Widget::Resize(size);

    ResizeAndRepositionChildWidgets();

    return GetSize();
}

void WidgetStack::AttachChild (Widget *const child)
{
    // call the superclass to actually attach the child
    Widget::AttachChild(child);
    // size a child's size properties have potentially changed, the
    // contents size properties need to be recalculated.
    DirtyContentsSizeProperties();
    // make sure that the min/max sizes are consistent with the contents
    CalculateMinAndMaxSizePropertiesFromContents();
    // attempt to resize the widget to the current size
    Resize(GetSize());
}

void WidgetStack::DetachChild (Widget *const child)
{
    // call the superclass to actually detach the child
    Widget::DetachChild(child);
    // size a child's size properties have potentially changed, the
    // contents size properties need to be recalculated.
    DirtyContentsSizeProperties();
    // make sure that the min/max sizes are consistent with the contents
    CalculateMinAndMaxSizePropertiesFromContents();
    // attempt to resize the widget to the current size
    Resize(GetSize());
}

void WidgetStack::ChildSizePropertiesChanged (Widget *const child)
{
    // size a child's size properties have changed, the
    // contents size properties need to be recalculated.
    DirtyContentsSizeProperties();
    // make sure that the min/max sizes are consistent with the contents
    CalculateMinAndMaxSizePropertiesFromContents();
    // attempt to resize the widget to the current size
    Resize(GetSize());
    // propogate the call up to this widget's parent
    ParentChildSizePropertiesUpdate(false);
}

void WidgetStack::ResizeAndRepositionChildWidgets ()
{
    ScreenCoordVector2 positional_offset;

    for (Uint32 i = 0; i < m_child_vector.size(); ++i)
    {
        Widget *child = m_child_vector[i];
        ASSERT1(child != NULL)

        // skip hidden children
        if (child->GetIsHidden())
            continue;

        // skip modal children
        if (child->GetIsModal())
            continue;

        // resize the child using the corresponding column and row sizes
        child->Resize(GetSize());
        // if the child didn't use up all the space, then calculate
        // how much extra space there is so that the child can be
        // properly centered on the grid slot.
        ScreenCoordVector2 extra_space(GetSize() - child->GetSize());
        ASSERT1(extra_space[Dim::X] >= 0 &&
                extra_space[Dim::Y] >= 0)
        // move the child to the tracked current positional offset,
        // plus half of the extra space, so the child is centered
        // on the grid slot.
        child->MoveTo(GetPosition() + extra_space / 2);
    }
}

void WidgetStack::DirtyContentsSizeProperties ()
{
    m_contents_size_properties_need_update = true;
}

void WidgetStack::UpdateContentsSizeProperties () const
{
    // early out if no update necessary
    if (!m_contents_size_properties_need_update)
        return;

    m_contents_size_properties_need_update = false;

    // initialize the min size properties
    m_contents_size_properties.m_min_size_enabled = Bool2(false, false);
    m_contents_size_properties.m_min_size = ScreenCoordVector2::ms_zero;

    // initialize the max size properties enabled
    m_contents_size_properties.m_max_size_enabled = Bool2(false, false);
    m_contents_size_properties.m_max_size =
        ScreenCoordVector2(
            SizeProperties::GetDefaultMaxSizeComponent(),
            SizeProperties::GetDefaultMaxSizeComponent());

    // iterate over the child widgets
    for (Uint32 i = 0; i < m_child_vector.size(); ++i)
    {
        Widget const *child = m_child_vector[i];
        ASSERT1(child != NULL)

        // skip hidden children
        if (child->GetIsHidden())
            continue;

        // skip modal children
        if (child->GetIsModal())
            continue;

        // iterate over X and Y dimensions
        for (Uint8 d = 0; d < 2; ++d)
        {   
            // if the child has a min size, then the contents as a whole
            // have a min size
            if (child->GetMinSizeEnabled()[d])
            {
                m_contents_size_properties.m_min_size_enabled[d] = true;
                m_contents_size_properties.m_min_size[d] =
                    Max(child->GetMinSize()[d],
                        m_contents_size_properties.m_min_size[d]);
            }
    
            // if the child has a max size, then the contents as a whole
            // have a max size
            if (child->GetMaxSizeEnabled()[d])
            {
                m_contents_size_properties.m_max_size_enabled[d] = true;
                m_contents_size_properties.m_max_size[d] =
                    Min(child->GetMaxSize()[d],
                        m_contents_size_properties.m_max_size[d]);
            }
        }
    }
}

} // end of namespace Xrb
