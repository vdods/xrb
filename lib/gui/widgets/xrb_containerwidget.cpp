// ///////////////////////////////////////////////////////////////////////////
// xrb_containerwidget.cpp by Victor Dods, created 2006/08/16
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_containerwidget.h"

#include "xrb_gui_events.h"
#include "xrb_input_events.h"
#include "xrb_screen.h"

namespace Xrb
{

// ///////////////////////////////////////////////////////////////////////////
// constructor and destructor
// ///////////////////////////////////////////////////////////////////////////

ContainerWidget::ContainerWidget (ContainerWidget *const parent, std::string const &name)
    :
    Widget(parent, name)
{
//     fprintf(stderr, "ContainerWidget::ContainerWidget(%s);\n", name.c_str());

    m_accepts_focus = false;
    m_focus = NULL;
    m_focus_has_mouse_grab = false;
    m_mouseover_focus = NULL;
    m_main_widget = NULL;
    m_child_resize_blocker_count = 0;
    m_child_resize_was_blocked = false;
}

ContainerWidget::~ContainerWidget ()
{
//     fprintf(stderr, "ContainerWidget::~ContainerWidget(%s);\n", m_name.c_str());

    ASSERT1(m_child_resize_blocker_count == 0 && "you must not delete a ChildResizeBlocker'ed ContainerWidget")

    // delete all child widgets
    DeleteAllChildren();

    // nullify the pointers
    m_focus = NULL;
    m_mouseover_focus = NULL;
    m_main_widget = NULL;
}

// ///////////////////////////////////////////////////////////////////////////
// accessors
// ///////////////////////////////////////////////////////////////////////////

ScreenCoordVector2 ContainerWidget::GetAdjustedSize (ScreenCoordVector2 const &size) const
{
    ScreenCoordRect rect(size);
    if (m_main_widget != NULL)
    {
        m_main_widget->AdjustFromMinSize(&rect);
        m_main_widget->AdjustFromMaxSize(&rect);
    }
    else
    {
        AdjustFromMinSize(&rect);
        AdjustFromMaxSize(&rect);
    }
    return rect.GetSize();
}

// ///////////////////////////////////////////////////////////////////////////
// modifiers
// ///////////////////////////////////////////////////////////////////////////

void ContainerWidget::SetSizePropertyEnabled (
    SizeProperties::Property const property,
    Uint32 const component,
    bool const value,
    bool const defer_parent_update)
{
    // update the preferred size properties
    ASSERT1(component <= 1)
    if (property == SizeProperties::MIN)
        m_preferred_size_properties.m_min_size_enabled[component] = value;
    else
        m_preferred_size_properties.m_max_size_enabled[component] = value;

    // if there is a main widget, pass this call down to it
    if (m_main_widget != NULL)
    {
        if (GetChildResizeBlockerCount() == 0)
            m_main_widget->SetSizePropertyEnabled(
                property,
                component,
                value,
                defer_parent_update);
        else
            IndicateChildResizeWasBlocked();
    }
    else
    {
        if (property == SizeProperties::MIN)
        {
            if (m_size_properties.m_min_size_enabled[component] != value)
            {
                m_size_properties.m_min_size_enabled[component] = value;
                MinSizeUpdated();
                if (AdjustFromMinSize(&m_screen_rect))
                    Resize(m_screen_rect.GetSize());
                ParentChildSizePropertiesUpdate(defer_parent_update);
            }
        }
        else
        {
            if (m_size_properties.m_max_size_enabled[component] != value)
            {
                m_size_properties.m_max_size_enabled[component] = value;
                MaxSizeUpdated();
                if (AdjustFromMaxSize(&m_screen_rect))
                    Resize(m_screen_rect.GetSize());
                ParentChildSizePropertiesUpdate(defer_parent_update);
            }
        }
    }
}

void ContainerWidget::SetSizePropertyEnabled (
    SizeProperties::Property const property,
    Bool2 const &value,
    bool const defer_parent_update)
{
    // update the preferred size properties
    if (property == SizeProperties::MIN)
        m_preferred_size_properties.m_min_size_enabled = value;
    else
        m_preferred_size_properties.m_max_size_enabled = value;

    // if there is a main widget, pass this call down to it
    if (m_main_widget != NULL)
    {
        if (GetChildResizeBlockerCount() == 0)
            m_main_widget->SetSizePropertyEnabled(
                property,
                value,
                defer_parent_update);
        else
            IndicateChildResizeWasBlocked();
    }
    else
    {
        if (property == SizeProperties::MIN)
        {
            if (m_size_properties.m_min_size_enabled != value)
            {
                m_size_properties.m_min_size_enabled = value;
                MinSizeUpdated();
                if (AdjustFromMinSize(&m_screen_rect))
                    Resize(m_screen_rect.GetSize());
                ParentChildSizePropertiesUpdate(defer_parent_update);
            }
        }
        else
        {
            if (m_size_properties.m_max_size_enabled != value)
            {
                m_size_properties.m_max_size_enabled = value;
                MaxSizeUpdated();
                if (AdjustFromMaxSize(&m_screen_rect))
                    Resize(m_screen_rect.GetSize());
                ParentChildSizePropertiesUpdate(defer_parent_update);
            }
        }
    }
}

void ContainerWidget::SetSizeProperty (
    SizeProperties::Property const property,
    Uint32 const component,
    ScreenCoord const value,
    bool const defer_parent_update)
{
    // update the preferred size properties
    ASSERT1(component <= 1)
    ASSERT1(value >= 0)
    if (property == SizeProperties::MIN)
        m_preferred_size_properties.m_min_size[component] = value;
    else
        m_preferred_size_properties.m_max_size[component] = value;

    // if there is a main widget, pass this call down to it
    if (m_main_widget != NULL)
    {
        if (GetChildResizeBlockerCount() == 0)
            m_main_widget->SetSizeProperty(
                property,
                component,
                value,
                defer_parent_update);
        else
            IndicateChildResizeWasBlocked();
    }
    else
    {
        ASSERT1(component <= 1)
        if (property == SizeProperties::MIN)
        {
            if (m_size_properties.m_min_size[component] != value)
            {
                m_size_properties.m_min_size[component] = value;
                MinSizeUpdated();
                if (AdjustFromMinSize(&m_screen_rect))
                    Resize(m_screen_rect.GetSize());
                ParentChildSizePropertiesUpdate(defer_parent_update);
            }
        }
        else
        {
            if (m_size_properties.m_max_size[component] != value)
            {
                m_size_properties.m_max_size[component] = value;
                MaxSizeUpdated();
                if (AdjustFromMaxSize(&m_screen_rect))
                    Resize(m_screen_rect.GetSize());
                ParentChildSizePropertiesUpdate(defer_parent_update);
            }
        }
    }
}

void ContainerWidget::SetSizeProperty (
    SizeProperties::Property const property,
    ScreenCoordVector2 const &value,
    bool const defer_parent_update)
{
    // update the preferred size properties
    ASSERT1(value[Dim::X] >= 0)
    ASSERT1(value[Dim::Y] >= 0)
    if (property == SizeProperties::MIN)
        m_preferred_size_properties.m_min_size = value;
    else
        m_preferred_size_properties.m_max_size = value;

    // if there is a main widget, pass this call down to it
    if (m_main_widget != NULL)
    {
        if (GetChildResizeBlockerCount() == 0)
            m_main_widget->SetSizeProperty(
                property,
                value,
                defer_parent_update);
        else
            IndicateChildResizeWasBlocked();
    }
    else
    {
        if (property == SizeProperties::MIN)
        {
            if (m_size_properties.m_min_size != value)
            {
                m_size_properties.m_min_size = value;
                MinSizeUpdated();
                if (AdjustFromMinSize(&m_screen_rect))
                    Resize(m_screen_rect.GetSize());
                ParentChildSizePropertiesUpdate(defer_parent_update);
            }
        }
        else
        {
            if (m_size_properties.m_max_size != value)
            {
                m_size_properties.m_max_size = value;
                MaxSizeUpdated();
                if (AdjustFromMaxSize(&m_screen_rect))
                    Resize(m_screen_rect.GetSize());
                ParentChildSizePropertiesUpdate(defer_parent_update);
            }
        }
    }
}

void ContainerWidget::SetMainWidget (Widget *const main_widget)
{
    m_main_widget = main_widget;
    if (m_main_widget != NULL)
    {
        ASSERT0(!m_main_widget->GetIsModal() &&
                "You can't use a modal widget as a main widget")
        ASSERT1(m_main_widget->GetParent() == this)
        m_main_widget->Resize(GetSize());
        m_main_widget->MoveTo(GetPosition());
        ParentChildSizePropertiesUpdate(false);
    }
}

// ///////////////////////////////////////////////////////////////////////////
// procedures
// ///////////////////////////////////////////////////////////////////////////

void ContainerWidget::Draw (RenderContext const &render_context) const
{
    // call the superclass' Draw method
    Widget::Draw(render_context);

    Float const disabled_widget_alpha_mask = 0.5f;
    // create a render context for the child widgets
    RenderContext child_render_context(render_context);
    // call Draw on all the child widgets with the appropriate clipping
    for (WidgetVectorConstIterator it = m_child_vector.begin(),
                                   it_end = m_child_vector.end();
         it != it_end;
         ++it)
    {
        Widget *child = *it;
        ASSERT1(child != NULL)

        // skip hidden and modal children (modal widgets are drawn
        // by the top-level widget.
        if (!child->GetIsHidden() && !child->GetIsModal())
        {
            // calculate the drawing clip rect from this widget's clip rect
            // and the child widget's virtual rect.
            child_render_context.SetClipRect(
                render_context.GetClippedRect(child->GetScreenRect()));
            // don't even bother drawing a child widget if this resulting
            // clip rect is invalid (0 area)
            if (child_render_context.GetClipRect().GetIsValid())
            {
                // set the color mask
                child_render_context.SetColorMask(
                    render_context.GetMaskedColor(child->GetColorMask()));
                // if the child widget is disabled (but this widget is enabled),
                // apply a transparent color mask as a visual indicator
                if (!child->GetIsEnabled() && GetIsEnabled())
                    child_render_context.ApplyAlphaMaskToColorMask(
                        disabled_widget_alpha_mask);
                // set up the GL clip rect for the child
                child_render_context.SetupGLClipRect();
                // do the actual draw call
                child->Draw(child_render_context);
            }
        }
    }

    // if there are modal widgets, draw them
    if (!m_modal_widget_stack.empty())
    {
        ASSERT1(GetIsTopLevelParent())

        // draw all the modal widgets, from the bottom of the stack, up.
        for (WidgetListConstIterator it = m_modal_widget_stack.begin(),
                                     it_end = m_modal_widget_stack.end();
             it != it_end;
             ++it)
        {
            Widget *modal_widget = *it;
            ASSERT1(modal_widget != NULL)

            // skip hidden modal widgets
            if (modal_widget->GetIsHidden())
                continue;

            // calculate the drawing clip rect from this widget's clip rect
            // and the child widget's virtual rect.
            child_render_context.SetClipRect(
                render_context.GetClippedRect(modal_widget->GetScreenRect()));
            // don't even bother drawing a modal widget if this resulting
            // clip rect is invalid (0 area)
            if (child_render_context.GetClipRect().GetIsValid())
            {
                // set the color mask
                child_render_context.SetColorMask(
                    render_context.GetMaskedColor(modal_widget->GetColorMask()));
                ASSERT1(modal_widget->GetIsEnabled())
                // set up the clip rect for the child
                child_render_context.SetupGLClipRect();
                // do the actual draw call
                modal_widget->Draw(child_render_context);
            }
        }
    }

    // restore the GL clip rect for this widget (this may be unnecessary,
    // because any widget that has child widgets shouldn't really be drawing
    // anything explicitly -- all its drawing will be handled by Widget).
    render_context.SetupGLClipRect();
}

void ContainerWidget::MoveBy (ScreenCoordVector2 const &delta)
{
    // call the superclass' MoveBy method
    Widget::MoveBy(delta);

    // move all child widgets by the same delta
    for (WidgetVectorIterator it = m_child_vector.begin(),
                              it_end = m_child_vector.end();
         it != it_end;
         ++it)
    {
        Widget *child = *it;
        ASSERT1(child != NULL)
        child->MoveBy(delta);
    }
}

ScreenCoordVector2 ContainerWidget::Resize (ScreenCoordVector2 const &size)
{
    ScreenCoordVector2 adjusted_size(m_size_properties.GetAdjustedSize(size));

    if (m_screen_rect.GetSize() != adjusted_size || GetChildResizeBlockerCount() == 0 && GetChildResizeWasBlocked())
    {
        m_screen_rect.SetSize(adjusted_size);

        // only update size stuff if not blocked
        if (GetChildResizeBlockerCount() == 0)
        {
            m_child_resize_was_blocked = false;
            // if there is a main widget, resize it to match this one
            if (m_main_widget != NULL)
                m_main_widget->Resize(m_screen_rect.GetSize());
        }
        else
            IndicateChildResizeWasBlocked();

        // range checking
        SizeRangeAdjustment(&m_screen_rect);
        // indicate to the parent that a child has changed size properties
        ParentChildSizePropertiesUpdate(false);
    }

    // return what the actual size is now
    return m_screen_rect.GetSize();
}

void ContainerWidget::AttachChild (Widget *const child)
{
    ASSERT1(child != NULL)
    ASSERT1(child->m_parent == NULL)
    // add the child at the end of the section of the list of child widgets
    // which have the same stack priority.  first find the appropriate place
    // to put the child
    WidgetVectorIterator it;
    WidgetVectorIterator it_end;
    for (it = m_child_vector.begin(),
         it_end = m_child_vector.end();
         it != it_end;
         ++it)
    {
        Widget *widget = *it;
        ASSERT1(widget != NULL)
        if (widget->GetStackPriority() > child->GetStackPriority())
            break;
    }
    // insert the child at the appropriate place
    m_child_vector.insert(it, child);
    // set its parent
    child->m_parent = this;
    // attempt to make the child inherit this widget's WidgetSkin,
    // without forcing the inheritance
    child->m_widget_skin = m_widget_skin;
    // sanity check -- Screen should be the top-level parent widget
    ASSERT1(dynamic_cast<Screen *>(GetTopLevelParent()) != NULL)
}

void ContainerWidget::DetachChild (Widget *const child)
{
    ASSERT1(child != NULL)
    ASSERT1(child->GetParent() != NULL)
    ASSERT1(child->GetParent() == this)
    // check that its actually a child
    WidgetVectorIterator it = FindChildWidget(child);
    ASSERT1((*it)->GetParent() == this &&
            it != m_child_vector.end() &&
            "not a child of this widget")
    // make sure to unfocus it
    child->Unfocus();
    // make sure mouseover-off it
    child->MouseoverOff();
    // if it's the main widget, clear the main widget
    if (m_main_widget == child)
        m_main_widget = NULL;
    // remove it from this widget's child list
    m_child_vector.erase(it);
    // set its parent to null
    child->m_parent = NULL;
}

void ContainerWidget::MoveChildDown (Widget *const child)
{
    // check that its actually a child
    WidgetVectorIterator it = FindChildWidget(child);
    WidgetVectorIterator it_end = m_child_vector.end();
    ASSERT1((*it)->GetParent() == this && it != it_end && "not a child of this widget")
    if (it != m_child_vector.begin())
    {
        // get the widget below this one
        WidgetVectorIterator prev = it;
        --prev;
        // do a value swap of the 2 elements, if they're of the same
        // stack priority
        if (child->GetStackPriority() == (*prev)->GetStackPriority())
        {
            Widget *temp = *it;
            *it = *prev;
            *prev = temp;
        }
    }
}

void ContainerWidget::MoveChildUp (Widget *const child)
{
    // check that its actually a child
    WidgetVectorIterator it = FindChildWidget(child);
    WidgetVectorIterator it_end = m_child_vector.end();
    ASSERT1((*it)->GetParent() == this && it != it_end && "not a child of this widget")
    // get the widget above this one
    WidgetVectorIterator next = it;
    ++next;
    // do a value swap of the 2 elements, if they're of the same
    // stack priority
    if (next != it_end &&
        child->GetStackPriority() == (*next)->GetStackPriority())
    {
        Widget *temp = *it;
        *it = *next;
        *next = temp;
    }
}

void ContainerWidget::MoveChildToBottom (Widget *const child)
{
    // check that its actually a child
    WidgetVectorIterator it_begin = m_child_vector.begin();
    WidgetVectorIterator it = FindChildWidget(child);
    WidgetVectorIterator it_end = m_child_vector.end();
    ASSERT1((*it)->GetParent() == this && it != it_end && "not a child of this widget")
    // find the appropriate place to move the child to (within the section
    // of the child vector of the same stack priority)
    WidgetVectorIterator dest_it = it;
    while (dest_it != it_begin &&
           (*dest_it)->GetStackPriority() == child->GetStackPriority())
    {
        --dest_it;
    }
    // make sure we correct going too far back
    if ((*dest_it)->GetStackPriority() != child->GetStackPriority())
        ++dest_it;

    // only do stuff if the widget is actually going to move
    if (it != dest_it)
    {
        // remove it from this widget's child list
        m_child_vector.erase(it);
        // insert the widget at the calculated destination
        m_child_vector.insert(dest_it, child);
    }
}

void ContainerWidget::MoveChildToTop (Widget *const child)
{
    // check that its actually a child
    WidgetVectorIterator it = FindChildWidget(child);
    WidgetVectorIterator it_end = m_child_vector.end();
    ASSERT1((*it)->GetParent() == this && it != it_end && "not a child of this widget")
    // find the appropriate place to move the child to (within the section
    // of the child vector of the same stack priority)
    WidgetVectorIterator dest_it = it;
    while (dest_it != it_end &&
           (*dest_it)->GetStackPriority() == child->GetStackPriority())
    {
        ++dest_it;
    }

    // only do stuff if the widget is actually going to move
    if (it != dest_it)
    {
        // insert the widget at the calculated destination
        m_child_vector.insert(dest_it, child);
        // the 'it' iterator must be recalculated because insertions
        // into the child vector can cause all iterators to become invalid.
        // this depends on the fact that the insertion was later in
        // the child vector than its previous position.
        it = FindChildWidget(child);
        // remove it from this widget's child list and set its parent to null
        m_child_vector.erase(it);
    }
}

void ContainerWidget::DeleteAllChildren ()
{
    // DESTROY all the poor little child widgets (in reverse order).
    while (m_child_vector.size() > 0)
    {
        Widget *child = m_child_vector.back();
        ASSERT1(child != NULL)
        // this will detach the child from the parent and remove its
        // element from m_child_vector -- hence the backwards traversal.
        Delete(child);
    }

    // clear the modal widget stack
    m_modal_widget_stack.clear();
}

// ///////////////////////////////////////////////////////////////////////////
// protected functions
// ///////////////////////////////////////////////////////////////////////////

Uint32 ContainerWidget::GetWidgetSkinHandlerChildCount () const
{
    return m_child_vector.size();
}

WidgetSkinHandler *ContainerWidget::GetWidgetSkinHandlerChild (Uint32 const index)
{
    ASSERT1(index < m_child_vector.size())
    Widget *child = m_child_vector[index];
    ASSERT1(child != NULL)
    return static_cast<WidgetSkinHandler *>(child);
}

Bool2 ContainerWidget::GetContentsMinSizeEnabled () const
{
    ASSERT1(m_main_widget != NULL)
    return m_main_widget->GetMinSizeEnabled();
}

ScreenCoordVector2 ContainerWidget::GetContentsMinSize () const
{
    ASSERT1(m_main_widget != NULL)
    return m_main_widget->GetMinSize();
}

Bool2 ContainerWidget::GetContentsMaxSizeEnabled () const
{
    ASSERT1(m_main_widget != NULL)
    return m_main_widget->GetMaxSizeEnabled();
}

ScreenCoordVector2 ContainerWidget::GetContentsMaxSize () const
{
    ASSERT1(m_main_widget != NULL)
    return m_main_widget->GetMaxSize();
}

void ContainerWidget::HandleFrame ()
{
    // call ProcessFrame on all the child widgets
    for (WidgetVectorIterator it = m_child_vector.begin(),
                              it_end = m_child_vector.end();
         it != it_end;
         ++it)
    {
        Widget *child = *it;
        ASSERT1(child != NULL)
        child->ProcessFrame(GetFrameTime());
    }
}

bool ContainerWidget::ProcessDeleteChildWidgetEvent (EventDeleteChildWidget const *const e)
{
    ASSERT1(e->GetChildToDelete() != NULL)
    ASSERT1(e->GetChildToDelete()->GetParent() == this)
    // detach the child before deleting it
    DetachChild(e->GetChildToDelete());
    e->DeleteChildWidget();
    return true;
}

void ContainerWidget::AddModalWidget (Widget *const modal_widget)
{
    ASSERT1(modal_widget != NULL)
    ASSERT1(modal_widget->GetIsModal())
    ASSERT1(modal_widget->GetIsEnabled())
    ASSERT1(!modal_widget->GetIsTopLevelParent())

    // if this is a top level widget, then add the modal widget to the
    // modal widget stack.  otherwise, pass it up to the parent
    if (GetIsTopLevelParent())
    {
        // turn off mouseover on this top level widget and all subordinate
        // widgets that have mouseover focus
        MouseoverOffWidgetLine();
        // focus the modal widget
        modal_widget->Focus();
        // stick the modal widget on the modal widget stack
        m_modal_widget_stack.push_back(modal_widget);
    }
    else
    {
        // pass this call up to the parent
        GetParent()->AddModalWidget(modal_widget);
    }
}

void ContainerWidget::RemoveModalWidget (Widget *const modal_widget)
{
    ASSERT1(modal_widget != NULL)
    ASSERT1(modal_widget->GetIsModal())
    ASSERT1(modal_widget->GetIsEnabled())

    // if this is a top level widget, then remove the modal widget from the
    // modal widget stack.  otherwise, pass it up to the parent
    if (GetIsTopLevelParent())
    {
        modal_widget->Unfocus();
        WidgetListIterator it = std::find(m_modal_widget_stack.begin(), m_modal_widget_stack.end(), modal_widget);
        ASSERT1(it != m_modal_widget_stack.end())
        m_modal_widget_stack.erase(it);
    }
    else
        GetParent()->RemoveModalWidget(modal_widget);
}

void ContainerWidget::CalculateMinAndMaxSizePropertiesFromContents ()
{
    // iterate over X and Y dimensions
    for (Uint8 d = 0; d < 2; ++d)
    {
        {
            bool contents_min_size_enabled = GetContentsMinSizeEnabled()[d];
            // calculate the min size enabled property
            m_size_properties.m_min_size_enabled[d] =
                contents_min_size_enabled ||
                m_preferred_size_properties.m_min_size_enabled[d];
            // calculate the actual min size
            if (contents_min_size_enabled)
            {
                if (m_preferred_size_properties.m_min_size_enabled[d])
                    m_size_properties.m_min_size[d] =
                        Max(GetContentsMinSize()[d],
                            m_preferred_size_properties.m_min_size[d]);
                else
                    m_size_properties.m_min_size[d] = GetContentsMinSize()[d];
            }
            else
            {
                if (m_preferred_size_properties.m_min_size_enabled[d])
                    m_size_properties.m_min_size[d] =
                        m_preferred_size_properties.m_min_size[d];
                else
                    m_size_properties.m_min_size[d] =
                        SizeProperties::GetDefaultMinSizeComponent();
            }
        }

        {
            bool contents_max_size_enabled = GetContentsMaxSizeEnabled()[d];
            // calculate the max size enabled property
            m_size_properties.m_max_size_enabled[d] =
                contents_max_size_enabled ||
                m_preferred_size_properties.m_max_size_enabled[d];
            // calculate the actual max size
            if (contents_max_size_enabled)
            {
                if (m_preferred_size_properties.m_max_size_enabled[d])
                    m_size_properties.m_max_size[d] =
                        Min(GetContentsMaxSize()[d],
                            m_preferred_size_properties.m_max_size[d]);
                else
                    m_size_properties.m_max_size[d] = GetContentsMaxSize()[d];
            }
            else
            {
                if (m_preferred_size_properties.m_max_size_enabled[d])
                    m_size_properties.m_max_size[d] =
                        m_preferred_size_properties.m_max_size[d];
                else
                    m_size_properties.m_max_size[d] =
                        SizeProperties::GetDefaultMaxSizeComponent();
            }
        }
    }

    MinSizeUpdated();
    ParentChildSizePropertiesUpdate(false);
}

void ContainerWidget::ChildSizePropertiesChanged (Widget *const child)
{
    ASSERT1(child != NULL)
    ASSERT1(child->GetParent() == this)

    if (child == m_main_widget)
    {
        if (GetChildResizeBlockerCount() == 0)
        {
            // adjust the size properties based on the contents (the main widget)
            CalculateMinAndMaxSizePropertiesFromContents();
            // attempt to resize the widget to the current size
            Resize(m_main_widget->GetSize());
        }
        else
            IndicateChildResizeWasBlocked();
    }
}

void ContainerWidget::ChildStackPriorityChanged (
    Widget *const child,
    StackPriority const previous_stack_priority)
{
    ASSERT1(child != NULL)
    ASSERT1(child->GetParent() == this)
    ASSERT1(child->GetStackPriority() != previous_stack_priority)

    WidgetVectorIterator it_begin = m_child_vector.begin();
    WidgetVectorIterator it = FindChildWidget(child);
    WidgetVectorIterator it_end = m_child_vector.end();
    WidgetVectorIterator dest_it = it;

    // check if the widget should be moved up or down
    if (it == it_end)
        ASSERT1(false && "Given child is somehow not in the child vector")

    // we can derive the direction to move it from the previous stack priority
    if (child->GetStackPriority() < previous_stack_priority)
    {
        // move it down

        // calculate the place to move the widget to
        while (dest_it != it_begin &&
               (*dest_it)->GetStackPriority() > child->GetStackPriority())
        {
            --dest_it;
        }
        // make sure we correct going too far back
        if ((*dest_it)->GetStackPriority() <= child->GetStackPriority())
            ++dest_it;

        // only do stuff if the widget will move
        if (dest_it != it)
        {
            // remove it from this widget's child list
            m_child_vector.erase(it);
            // insert the widget at the calculated destination
            m_child_vector.insert(dest_it, child);
        }
    }
    else
    {
        // move it up

        // calculate the place to move the widget to
        while (dest_it != it_end &&
               (*dest_it)->GetStackPriority() < child->GetStackPriority())
        {
            ++dest_it;
        }

        // only do stuff if the widget will move
        if (dest_it != it)
        {
            // insert the widget at the calculated destination
            m_child_vector.insert(dest_it, child);
            // the 'it' iterator must be recalculated because insertions
            // into the child vector can cause all iterators to become invalid.
            // this depends on the fact that the insertion was later in
            // the child vector than its previous position.
            it = FindChildWidget(child);
            // remove it from this widget's child list and set its parent to null
            m_child_vector.erase(it);
        }
    }
}

// ///////////////////////////////////////////////////////////////////////////
// private functions
// ///////////////////////////////////////////////////////////////////////////

ContainerWidget::WidgetVectorIterator ContainerWidget::FindChildWidget (Widget const *const child)
{
    WidgetVectorIterator it;
    WidgetVectorIterator it_end;
    for (it = m_child_vector.begin(),
         it_end = m_child_vector.end();
         it != it_end;
         ++it)
    {
        if (*it == child)
            break;
    }
    return it;
}

bool ContainerWidget::PreprocessMouseEvent (EventMouse const *const e)
{
    ASSERT1(e != NULL)

    // if there is a widget in focus and it has mouse grab
    // on, send the mouse event there
    if (m_focus != NULL && m_focus_has_mouse_grab)
        return m_focus->ProcessEvent(e);
    // otherwise, hand it to the superclass' method
    else
        return Widget::PreprocessMouseEvent(e);
}

bool ContainerWidget::PreprocessMouseWheelEvent (EventMouseWheel const *const e)
{
    // if there is a widget in focus and it has mouse grab
    // on, send the mouse event there
    if (m_focus != NULL && m_focus_has_mouse_grab)
        return m_focus->ProcessEvent(e);
    // otherwise, hand it to the superclass' method
    else
        return Widget::PreprocessMouseWheelEvent(e);
}

bool ContainerWidget::PreprocessFocusEvent (EventFocus const *const e)
{
    // hidden widgets can't be focused
    if (GetIsHidden())
        return false;

    // widgets that don't accept focus and have no children (which
    // may potentially accept focus) return false
    if (!m_accepts_focus && m_child_vector.size() == 0)
        return false;

    // if there are any modal widgets, then focus can only go the top
    // unhidden modal widget.
    Widget *modal_widget = NULL;
    for (WidgetListReverseIterator it = m_modal_widget_stack.rbegin(),
                                   it_end = m_modal_widget_stack.rend();
         it != it_end;
         ++it)
    {
        Widget *widget = *it;
        ASSERT1(widget != NULL)
        if (!widget->GetIsHidden())
        {
            modal_widget = widget;
            break;
        }
    }

    if (modal_widget != NULL)
    {
        if (modal_widget->GetScreenRect().GetIsPointInside(e->GetPosition()))
            return modal_widget->PreprocessFocusEvent(e);
        else
            return false;
    }
    // otherwise, loop through all the child widgets (from top to bottom)
    else
    {
        WidgetVectorReverseIterator it;
        WidgetVectorReverseIterator it_end;
        for (it = m_child_vector.rbegin(),
             it_end = m_child_vector.rend();
             it != it_end;
             ++it)
        {
            Widget *child = *it;
            ASSERT1(child != NULL)
            if (!child->GetIsHidden() &&
                child->GetScreenRect().GetIsPointInside(e->GetPosition()) &&
                child->PreprocessFocusEvent(e))
                return true;
        }

        // if none of the widgets accepted focus, hand it to the superclass' method
        return Widget::PreprocessFocusEvent(e);
    }
}

bool ContainerWidget::PreprocessMouseoverEvent (EventMouseover const *const e)
{
    // hidden widgets can't be moused over
    if (GetIsHidden())
        return false;

    // loop through all the child widgets (from top to bottom)
    WidgetVectorReverseIterator it;
    WidgetVectorReverseIterator it_end;
    for (it = m_child_vector.rbegin(),
         it_end = m_child_vector.rend();
         it != it_end;
         ++it)
    {
        Widget *child = *it;
        ASSERT1(child != NULL)
        if (child->GetScreenRect().GetIsPointInside(e->GetPosition()))
            if (child->PreprocessMouseoverEvent(e))
                return true;
    }

    // if none of the widgets accepted mouseover-focus, hand it to the superclass' method
    return Widget::PreprocessMouseoverEvent(e);
}

bool ContainerWidget::SendMouseEventToChild (EventMouse const *const e)
{
    // attempt to send the mouse event to the widget that is below
    // the mouse cursor, traversing the child vector from top to
    // bottom.
    for (WidgetVectorReverseIterator it = m_child_vector.rbegin(),
                                     it_end = m_child_vector.rend();
         it != it_end;
         ++it)
    {
        Widget *child = *it;
        ASSERT1(child != NULL)
        // only send the event to widgets that are not hidden
        // AND if the mouse event position is inside the widget's rect
        if (!child->GetIsHidden() &&
            child->GetScreenRect().GetIsPointInside(e->GetPosition()))
            if (child->ProcessEvent(e))
                return true;
    }
    // if no widget accepted it, return false
    return false;
}

void ContainerWidget::IncrementResizeBlockerCount ()
{
    ASSERT1(m_child_resize_blocker_count < UINT32_UPPER_BOUND)
    ++m_child_resize_blocker_count;
}

void ContainerWidget::DecrementResizeBlockerCount ()
{
    ASSERT1(m_child_resize_blocker_count > 0)
    --m_child_resize_blocker_count;
    if (m_child_resize_blocker_count == 0)
        Resize(GetSize());
    m_child_resize_was_blocked = false;
}

} // end of namespace Xrb