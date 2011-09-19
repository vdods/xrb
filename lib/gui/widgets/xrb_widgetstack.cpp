// ///////////////////////////////////////////////////////////////////////////
// xrb_widgetstack.cpp by Victor Dods, created 2005/12/16
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_widgetstack.hpp"

#include "xrb_screen.hpp"

namespace Xrb {

WidgetStack::WidgetStack (WidgetContext &context, std::string const &name)
    :
    ContainerWidget(context, name)
{
    DirtyContentsSizeProperties();
}

Bool2 WidgetStack::ContentsMinSizeEnabled () const
{
    UpdateContentsSizeProperties();
    return m_contents_size_properties.m_min_size_enabled;
}

ScreenCoordVector2 WidgetStack::ContentsMinSize () const
{
    UpdateContentsSizeProperties();
    return m_contents_size_properties.m_min_size;
}

Bool2 WidgetStack::ContentsMaxSizeEnabled () const
{
    UpdateContentsSizeProperties();
    return m_contents_size_properties.m_max_size_enabled;
}

ScreenCoordVector2 WidgetStack::ContentsMaxSize () const
{
    UpdateContentsSizeProperties();
    return m_contents_size_properties.m_max_size;
}

void WidgetStack::SetSizePropertyEnabled (SizeProperties::Property property, Uint32 component, bool value)
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

void WidgetStack::SetSizePropertyEnabled (SizeProperties::Property property, Bool2 const &value)
{
    if (property == SizeProperties::MIN)
        m_preferred_size_properties.m_min_size_enabled = value;
    else
        m_preferred_size_properties.m_max_size_enabled = value;
    ContainerWidget::SetSizePropertyEnabled(property, value, true);
    CalculateMinAndMaxSizePropertiesFromContents();
//     ParentChildSizePropertiesUpdate(false);
}

void WidgetStack::SetSizeProperty (SizeProperties::Property property, Uint32 component, ScreenCoord value)
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

void WidgetStack::SetSizeProperty (SizeProperties::Property property, ScreenCoordVector2 const &value)
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

ScreenCoordVector2 WidgetStack::Resize (ScreenCoordVector2 const &size)
{
    ContainerWidget::Resize(size);

    // only update size stuff if not blocked
    if (ChildResizeBlockerCount() == 0)
    {
        ResizeAndRepositionChildWidgets();
    }
    else
        IndicateChildResizeWasBlocked();

    return Size();
}

void WidgetStack::AttachChild (Widget *child)
{
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

void WidgetStack::DetachChild (Widget *child)
{
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

void WidgetStack::ChildSizePropertiesChanged (Widget *child)
{
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
//         // propagate the changes up to the parent
//         ParentChildSizePropertiesUpdate(false);
    }
    else
        IndicateChildResizeWasBlocked();
}

void WidgetStack::UpdateRenderBackground ()
{
    ContainerWidget::UpdateRenderBackground();
    SetRenderBackground(NULL);
}

void WidgetStack::ResizeAndRepositionChildWidgets ()
{
    ScreenCoordVector2 positional_offset;

    for (Uint32 i = 0; i < m_child_vector.size(); ++i)
    {
        Widget *child = m_child_vector[i];
        ASSERT1(child != NULL);

        // skip hidden children
        if (child->IsHidden())
            continue;

        // skip modal children
        if (child->IsModal())
            continue;

        // resize the child using the corresponding column and row sizes
        child->Resize(Size());
        // if the child didn't use up all the space, then calculate
        // how much extra space there is so that the child can be
        // properly centered on the grid slot.
        ScreenCoordVector2 extra_space(Size() - child->Size());
        ASSERT1(extra_space[Dim::X] >= 0 &&
                extra_space[Dim::Y] >= 0);
        // move the child to the tracked current positional offset,
        // plus half of the extra space, so the child is centered
        // on the grid slot.
        child->MoveTo(Position() + extra_space / 2);
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
            SizeProperties::DefaultMaxSizeComponent(),
            SizeProperties::DefaultMaxSizeComponent());

    // iterate over the child widgets
    for (Uint32 i = 0; i < m_child_vector.size(); ++i)
    {
        Widget const *child = m_child_vector[i];
        ASSERT1(child != NULL);

        // skip hidden children
        if (child->IsHidden())
            continue;

        // skip modal children
        if (child->IsModal())
            continue;

        // iterate over X and Y dimensions
        for (Uint8 d = 0; d < 2; ++d)
        {
            // if the child has a min size, then the contents as a whole
            // have a min size
            if (child->MinSizeEnabled()[d])
            {
                m_contents_size_properties.m_min_size_enabled[d] = true;
                m_contents_size_properties.m_min_size[d] =
                    Max(child->MinSize()[d],
                        m_contents_size_properties.m_min_size[d]);
            }

            // if the child has a max size, then the contents as a whole
            // have a max size
            if (child->MaxSizeEnabled()[d])
            {
                m_contents_size_properties.m_max_size_enabled[d] = true;
                m_contents_size_properties.m_max_size[d] =
                    Min(child->MaxSize()[d],
                        m_contents_size_properties.m_max_size[d]);
            }
        }
    }
}

} // end of namespace Xrb
