// ///////////////////////////////////////////////////////////////////////////
// xrb_containerwidget.cpp by Victor Dods, created 2006/08/16
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_containerwidget.hpp"

#include "xrb_gui_events.hpp"
#include "xrb_input_events.hpp"
#include "xrb_screen.hpp"
#include "xrb_widgetcontext.hpp"

namespace Xrb {

// ///////////////////////////////////////////////////////////////////////////
// constructor and destructor
// ///////////////////////////////////////////////////////////////////////////

ContainerWidget::ContainerWidget (WidgetContext &context, std::string const &name)
    :
    Widget(context, name)
{
//     std::cerr << "ContainerWidget::ContainerWidget(); name = \"" << name << '"' << std::endl;

    m_accepts_focus = false;
    m_children_get_input_events_first = false;
    m_focus = NULL;
    m_focus_has_mouse_grab = false;
    m_mouseover_focus = NULL;
    m_main_widget = NULL;
    m_child_resize_blocker_count = 0;
    m_child_resize_was_blocked = false;
}

ContainerWidget::~ContainerWidget ()
{
//     std::cerr << "ContainerWidget::~ContainerWidget(); name = \"" << name << '"' << std::endl;

    ASSERT1(m_child_resize_blocker_count == 0 && "you must not delete a ChildResizeBlocker'ed ContainerWidget");

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

Bool2 const &ContainerWidget::MinSizeEnabled () const
{
    return m_main_widget ? m_main_widget->MinSizeEnabled() : m_size_properties.m_min_size_enabled;
}

ScreenCoordVector2 const &ContainerWidget::MinSize () const
{
    return m_main_widget ? m_main_widget->MinSize() : m_size_properties.m_min_size;
}

Bool2 const &ContainerWidget::MaxSizeEnabled () const
{
    return m_main_widget ? m_main_widget->MaxSizeEnabled() : m_size_properties.m_max_size_enabled;
}

ScreenCoordVector2 const &ContainerWidget::MaxSize () const
{
    return m_main_widget ? m_main_widget->MaxSize() : m_size_properties.m_max_size;
}

ScreenCoordVector2 ContainerWidget::AdjustedSize (ScreenCoordVector2 const &size) const
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
    return rect.Size();
}

// ///////////////////////////////////////////////////////////////////////////
// modifiers
// ///////////////////////////////////////////////////////////////////////////

void ContainerWidget::SetSizePropertyEnabled (
    SizeProperties::Property property,
    Uint32 component,
    bool value,
    bool defer_parent_update)
{
    // update the preferred size properties
    ASSERT1(component <= 1);
    if (property == SizeProperties::MIN)
        m_preferred_size_properties.m_min_size_enabled[component] = value;
    else
        m_preferred_size_properties.m_max_size_enabled[component] = value;

    // if there is a main widget, pass this call down to it
    if (m_main_widget != NULL)
    {
        if (ChildResizeBlockerCount() == 0)
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
                    Resize(m_screen_rect.Size());
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
                    Resize(m_screen_rect.Size());
                ParentChildSizePropertiesUpdate(defer_parent_update);
            }
        }
    }
}

void ContainerWidget::SetSizePropertyEnabled (
    SizeProperties::Property property,
    Bool2 const &value,
    bool defer_parent_update)
{
    // update the preferred size properties
    if (property == SizeProperties::MIN)
        m_preferred_size_properties.m_min_size_enabled = value;
    else
        m_preferred_size_properties.m_max_size_enabled = value;

    // if there is a main widget, pass this call down to it
    if (m_main_widget != NULL)
    {
        if (ChildResizeBlockerCount() == 0)
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
                    Resize(m_screen_rect.Size());
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
                    Resize(m_screen_rect.Size());
                ParentChildSizePropertiesUpdate(defer_parent_update);
            }
        }
    }
}

void ContainerWidget::SetSizeProperty (
    SizeProperties::Property property,
    Uint32 component,
    ScreenCoord value,
    bool defer_parent_update)
{
    // update the preferred size properties
    ASSERT1(component <= 1);
    ASSERT1(value >= 0);
    if (property == SizeProperties::MIN)
        m_preferred_size_properties.m_min_size[component] = value;
    else
        m_preferred_size_properties.m_max_size[component] = value;

    // if there is a main widget, pass this call down to it
    if (m_main_widget != NULL)
    {
        if (ChildResizeBlockerCount() == 0)
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
        ASSERT1(component <= 1);
        if (property == SizeProperties::MIN)
        {
            if (m_size_properties.m_min_size[component] != value)
            {
                m_size_properties.m_min_size[component] = value;
                MinSizeUpdated();
                if (AdjustFromMinSize(&m_screen_rect))
                    Resize(m_screen_rect.Size());
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
                    Resize(m_screen_rect.Size());
                ParentChildSizePropertiesUpdate(defer_parent_update);
            }
        }
    }
}

void ContainerWidget::SetSizeProperty (
    SizeProperties::Property property,
    ScreenCoordVector2 const &value,
    bool defer_parent_update)
{
    // update the preferred size properties
    ASSERT1(value[Dim::X] >= 0);
    ASSERT1(value[Dim::Y] >= 0);
    if (property == SizeProperties::MIN)
        m_preferred_size_properties.m_min_size = value;
    else
        m_preferred_size_properties.m_max_size = value;

    // if there is a main widget, pass this call down to it
    if (m_main_widget != NULL)
    {
        if (ChildResizeBlockerCount() == 0)
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
                    Resize(m_screen_rect.Size());
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
                    Resize(m_screen_rect.Size());
                ParentChildSizePropertiesUpdate(defer_parent_update);
            }
        }
    }
}

void ContainerWidget::SetMainWidget (Widget *main_widget)
{
    if (main_widget != NULL && main_widget->Parent() != this)
    {
        ASSERT1(false && "you can't call SetMainWidget on a non-child");
        return;
    }
    
    m_main_widget = main_widget;
    if (m_main_widget != NULL)
    {
        ASSERT0(!m_main_widget->IsModal() && "You can't use a modal widget as a main widget");
        m_main_widget->Resize(Size());
        m_main_widget->MoveTo(Position());
        ParentChildSizePropertiesUpdate(false);
    }
}

// ///////////////////////////////////////////////////////////////////////////
// procedures
// ///////////////////////////////////////////////////////////////////////////

void ContainerWidget::PreDraw()
{
    Widget::PreDraw();
    // call Draw on all the child widgets with the appropriate clipping
    for (WidgetVector::const_iterator it = m_child_vector.begin(),
                                      it_end = m_child_vector.end();
         it != it_end;
         ++it)
    {
        ASSERT1(*it != NULL);
        Widget &child = **it;
        child.PreDraw();
    }
}

void ContainerWidget::Draw (RenderContext const &render_context) const
{
    // call the superclass' Draw method
    Widget::Draw(render_context);

    static Float const s_disabled_widget_alpha_mask = 0.5f;
    // create a render context for the child widgets
    RenderContext child_render_context(render_context);
    // call Draw on all the child widgets with the appropriate clipping
    for (WidgetVector::const_iterator it = m_child_vector.begin(),
                                      it_end = m_child_vector.end();
         it != it_end;
         ++it)
    {
        ASSERT1(*it != NULL);
        Widget &child = **it;

        // skip hidden and modal children (modal widgets are drawn
        // by the top-level widget.
        if (!child.IsHidden() && !child.IsModal())
        {
            // calculate the drawing clip rect from this widget's clip rect
            // and the child widget's virtual rect.
            child_render_context.SetClipRect(render_context.ClippedRect(child.ScreenRect()));
            // don't even bother drawing a child widget if this resulting
            // clip rect is invalid (0 area)
            if (child_render_context.ClipRect().IsValid())
            {
                // set the color bias and color mask
                child_render_context.ColorBias() = render_context.ColorBias();
                child_render_context.ApplyColorBias(child.ColorBias());
                child_render_context.ColorMask() = render_context.ColorMask();
                child_render_context.ApplyColorMask(child.ColorMask());
                // if the child widget is disabled (but this widget is enabled),
                // apply a transparent color mask as a visual indicator
                if (!child.IsEnabled() && IsEnabled())
                    child_render_context.ApplyAlphaMaskToColorMask(s_disabled_widget_alpha_mask);
                // set up the GL clip rect for the child
                Context().GetScreen().SetViewport(child_render_context.ClipRect());
                // do the actual draw call
                child.Draw(child_render_context);
            }
        }
    }

    // restore the GL clip rect for this widget (this may be unnecessary,
    // because any widget that has child widgets shouldn't really be drawing
    // anything explicitly -- all its drawing will be handled by Widget).
    Context().GetScreen().SetViewport(render_context.ClipRect());
}

void ContainerWidget::PostDraw()
{
    Widget::PostDraw();
    // call Draw on all the child widgets with the appropriate clipping
    for (WidgetVector::const_iterator it = m_child_vector.begin(),
                                      it_end = m_child_vector.end();
         it != it_end;
         ++it)
    {
        ASSERT1(*it != NULL);
        Widget &child = **it;
        child.PostDraw();
    }
}

void ContainerWidget::MoveBy (ScreenCoordVector2 const &delta)
{
    // call the superclass' MoveBy method
    Widget::MoveBy(delta);

    // move all child widgets by the same delta
    for (WidgetVector::iterator it = m_child_vector.begin(),
                              it_end = m_child_vector.end();
         it != it_end;
         ++it)
    {
        Widget *child = *it;
        ASSERT1(child != NULL);
        child->MoveBy(delta);
    }
}

ScreenCoordVector2 ContainerWidget::Resize (ScreenCoordVector2 const &size)
{
    ScreenCoordVector2 adjusted_size(m_size_properties.AdjustedSize(size));

    if (m_screen_rect.Size() != adjusted_size || (ChildResizeBlockerCount() == 0 && ChildResizeWasBlocked()))
    {
        m_screen_rect.SetSize(adjusted_size);

        // only update size stuff if not blocked
        if (ChildResizeBlockerCount() == 0)
        {
            m_child_resize_was_blocked = false;
            // if there is a main widget, resize it to match this one
            if (m_main_widget != NULL)
                m_main_widget->Resize(m_screen_rect.Size());
        }
        else
            IndicateChildResizeWasBlocked();

        // range checking
        SizeRangeAdjustment(&m_screen_rect);
        // indicate to the parent that a child has changed size properties
        ParentChildSizePropertiesUpdate(false);
    }

    // return what the actual size is now
    return m_screen_rect.Size();
}

void ContainerWidget::AttachChild (Widget *child)
{
    ASSERT1(child != NULL);
    if (child->m_parent != NULL)
    {
        ASSERT1(false && "this child already has a parent");
        return;
    }

    // because AttachChild is no longer called during construction, it is
    // possible to accidentally call a ContainerWidget's AttachChild on itself
    // (or on an ancestor of a ContainerWidget).  This causes a cycle in
    // what should be the DAG of Widget ancestry.  so check for that.
    ContainerWidget *ancestor = this;
    while (ancestor != NULL)
    {
        if (child == ancestor)
        {
            ASSERT1(false && "you AttachChild'ed a widget to itself (possibly indirectly) -- you're a bad programmer.");
            return;
        }
        ancestor = ancestor->m_parent;
    }
    
    // add the child at the end of the section of the list of child widgets
    // which have the same stack priority.  first find the appropriate place
    // to put the child
    WidgetVector::iterator it;
    WidgetVector::iterator it_end;
    for (it = m_child_vector.begin(),
         it_end = m_child_vector.end();
         it != it_end;
         ++it)
    {
        Widget *widget = *it;
        ASSERT1(widget != NULL);
        if (widget->GetStackPriority() > child->GetStackPriority())
            break;
    }
    // insert the child at the appropriate place
    m_child_vector.insert(it, child);
    // set its parent
    child->m_parent = this;
    // allow the child to cope with having a new parent.
    child->HandleAttachedToParent();
}

void ContainerWidget::DetachChild (Widget *child)
{
    ASSERT1(child != NULL);
    ASSERT1(child->Parent() == this);
    // check that it's actually a child
    WidgetVector::iterator it = FindChildWidget(child);
    if (it == m_child_vector.end())
    {
        ASSERT1(false && "not a child of this widget");
        return;
    }
    // allow the child to come to terms with its parent's imminent disappearance.
    child->HandleAboutToDetachFromParent();
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

void ContainerWidget::MoveChildDown (Widget *child)
{
    // check that its actually a child
    WidgetVector::iterator it = FindChildWidget(child);
    ASSERT1((*it)->Parent() == this && it != m_child_vector.end() && "not a child of this widget");
    if (it != m_child_vector.begin())
    {
        // get the widget below this one
        WidgetVector::iterator prev = it;
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

void ContainerWidget::MoveChildUp (Widget *child)
{
    // check that its actually a child
    WidgetVector::iterator it = FindChildWidget(child);
    WidgetVector::iterator it_end = m_child_vector.end();
    ASSERT1((*it)->Parent() == this && it != it_end && "not a child of this widget");
    // get the widget above this one
    WidgetVector::iterator next = it;
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

void ContainerWidget::MoveChildToBottom (Widget *child)
{
    // check that its actually a child
    WidgetVector::iterator it_begin = m_child_vector.begin();
    WidgetVector::iterator it = FindChildWidget(child);
    ASSERT1((*it)->Parent() == this && it != m_child_vector.end() && "not a child of this widget");
    // find the appropriate place to move the child to (within the section
    // of the child vector of the same stack priority)
    WidgetVector::iterator dest_it = it;
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

void ContainerWidget::MoveChildToTop (Widget *child)
{
    // check that its actually a child
    WidgetVector::iterator it = FindChildWidget(child);
    WidgetVector::iterator it_end = m_child_vector.end();
    ASSERT1((*it)->Parent() == this && it != it_end && "not a child of this widget");
    // find the appropriate place to move the child to (within the section
    // of the child vector of the same stack priority)
    WidgetVector::iterator dest_it = it;
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
        ASSERT1(child != NULL);
        // detach the child before deleting.  this is critical, because the
        // child will call virtual methods in its detach handling.
        DetachChild(child);
        Delete(child);
    }
}

// ///////////////////////////////////////////////////////////////////////////
// protected functions
// ///////////////////////////////////////////////////////////////////////////

Bool2 ContainerWidget::ContentsMinSizeEnabled () const
{
    ASSERT1(m_main_widget != NULL);
    return m_main_widget->MinSizeEnabled();
}

ScreenCoordVector2 ContainerWidget::ContentsMinSize () const
{
    ASSERT1(m_main_widget != NULL);
    return m_main_widget->MinSize();
}

Bool2 ContainerWidget::ContentsMaxSizeEnabled () const
{
    ASSERT1(m_main_widget != NULL);
    return m_main_widget->MaxSizeEnabled();
}

ScreenCoordVector2 ContainerWidget::ContentsMaxSize () const
{
    ASSERT1(m_main_widget != NULL);
    return m_main_widget->MaxSize();
}

void ContainerWidget::HandleFrame ()
{
    // call ProcessFrame on all the child widgets
    for (WidgetVector::iterator it = m_child_vector.begin(),
                                it_end = m_child_vector.end();
         it != it_end;
         ++it)
    {
        ASSERT1(*it != NULL);
        Widget &child = **it;
        child.ProcessFrame(FrameTime());
    }
}

bool ContainerWidget::ProcessDetachAndDeleteChildWidgetEvent (EventDetachAndDeleteChildWidget const &e)
{
    ASSERT1(e.ChildToDetachAndDelete() != this && "a widget must not delete itself");
    e.DetachAndDeleteChildWidget();
    return true;
}

/// Calls HandleActivate on all child widgets as well.
void ContainerWidget::HandleActivate ()
{
    for (WidgetVector::iterator it = m_child_vector.begin(), it_end = m_child_vector.end(); it != it_end; ++it)
    {
        ASSERT1(*it != NULL);
        Widget &child = **it;
        child.HandleActivate();
    }
}

/// Calls HandleDeactivate on all child widgets as well.
void ContainerWidget::HandleDeactivate ()
{
    for (WidgetVector::iterator it = m_child_vector.begin(), it_end = m_child_vector.end(); it != it_end; ++it)
    {
        ASSERT1(*it != NULL);
        Widget &child = **it;
        child.HandleDeactivate();
    }
}

void ContainerWidget::CalculateMinAndMaxSizePropertiesFromContents ()
{
    // iterate over X and Y dimensions
    for (Uint8 d = 0; d < 2; ++d)
    {
        {
            bool contents_min_size_enabled = ContentsMinSizeEnabled()[d];
            // calculate the min size enabled property
            m_size_properties.m_min_size_enabled[d] =
                contents_min_size_enabled ||
                m_preferred_size_properties.m_min_size_enabled[d];
            // calculate the actual min size
            if (contents_min_size_enabled)
            {
                if (m_preferred_size_properties.m_min_size_enabled[d])
                    m_size_properties.m_min_size[d] =
                        Max(ContentsMinSize()[d],
                            m_preferred_size_properties.m_min_size[d]);
                else
                    m_size_properties.m_min_size[d] = ContentsMinSize()[d];
            }
            else
            {
                if (m_preferred_size_properties.m_min_size_enabled[d])
                    m_size_properties.m_min_size[d] =
                        m_preferred_size_properties.m_min_size[d];
                else
                    m_size_properties.m_min_size[d] =
                        SizeProperties::DefaultMinSizeComponent();
            }
        }

        {
            bool contents_max_size_enabled = ContentsMaxSizeEnabled()[d];
            // calculate the max size enabled property
            m_size_properties.m_max_size_enabled[d] =
                contents_max_size_enabled ||
                m_preferred_size_properties.m_max_size_enabled[d];
            // calculate the actual max size
            if (contents_max_size_enabled)
            {
                if (m_preferred_size_properties.m_max_size_enabled[d])
                    m_size_properties.m_max_size[d] =
                        Min(ContentsMaxSize()[d],
                            m_preferred_size_properties.m_max_size[d]);
                else
                    m_size_properties.m_max_size[d] = ContentsMaxSize()[d];
            }
            else
            {
                if (m_preferred_size_properties.m_max_size_enabled[d])
                    m_size_properties.m_max_size[d] =
                        m_preferred_size_properties.m_max_size[d];
                else
                    m_size_properties.m_max_size[d] =
                        SizeProperties::DefaultMaxSizeComponent();
            }
        }
    }

    MinSizeUpdated();
    ParentChildSizePropertiesUpdate(false);
}

void ContainerWidget::ChildSizePropertiesChanged (Widget *child)
{
    ASSERT1(child != NULL);
    ASSERT1(child->Parent() == this);

    if (child == m_main_widget)
    {
        if (ChildResizeBlockerCount() == 0)
        {
            // adjust the size properties based on the contents (the main widget)
            CalculateMinAndMaxSizePropertiesFromContents();
            // attempt to resize the widget to the current size
            Resize(m_main_widget->Size());
        }
        else
            IndicateChildResizeWasBlocked();
    }
}

void ContainerWidget::ChildStackPriorityChanged (Widget *child, StackPriority previous_stack_priority)
{
    ASSERT1(child != NULL);
    ASSERT1(child->Parent() == this);
    ASSERT1(child->GetStackPriority() != previous_stack_priority);

    WidgetVector::iterator it_begin = m_child_vector.begin();
    WidgetVector::iterator it = FindChildWidget(child);
    WidgetVector::iterator it_end = m_child_vector.end();
    WidgetVector::iterator dest_it = it;

    // check if the widget should be moved up or down
    if (it == it_end)
        ASSERT1(false && "Given child is somehow not in the child vector");

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

ContainerWidget::WidgetVector::iterator ContainerWidget::FindChildWidget (Widget const *child)
{
    WidgetVector::iterator it;
    WidgetVector::iterator it_end;
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

bool ContainerWidget::InternalProcessKeyEvent (EventKey const &e)
{
    if (m_children_get_input_events_first)
    {
        if (m_focus != NULL)
            return m_focus->ProcessEvent(e);
        else
            return ProcessKeyEvent(dynamic_cast<EventKey const &>(e));
    }
    else
    {
        if (ProcessKeyEvent(dynamic_cast<EventKey const &>(e)))
            return true;
        else if (m_focus != NULL)
            return m_focus->ProcessEvent(e);
        else
            return false;
    }
}

bool ContainerWidget::InternalProcessMouseEvent (EventMouse const &e)
{
    if (m_children_get_input_events_first)
    {
        if (m_focus != NULL && m_focus_has_mouse_grab)
            return m_focus->ProcessEvent(e);
        else if (SendMouseEventToChild(dynamic_cast<EventMouse const &>(e)))
            return true;
        else
            return Widget::InternalProcessMouseEvent(e);
    }
    else
    {
        if (m_focus != NULL && m_focus_has_mouse_grab)
            return m_focus->ProcessEvent(e);
        else if (Widget::InternalProcessMouseEvent(e))
            return true;
        else
            return SendMouseEventToChild(dynamic_cast<EventMouse const &>(e));
    }
}

bool ContainerWidget::InternalProcessPinchEvent (EventPinch const &e)
{
    // because a pinch event is not associated with a screen position, the only way it
    // makes sense to handle is like a keyboard event.  therefore send it down the focus line.

    if (m_children_get_input_events_first)
    {
        if (m_focus != NULL)
            return m_focus->ProcessEvent(e);
        else
            return Widget::InternalProcessPinchEvent(dynamic_cast<EventPinch const &>(e));
    }
    else
    {
        if (Widget::InternalProcessPinchEvent(dynamic_cast<EventPinch const &>(e)))
            return true;
        else if (m_focus != NULL)
            return m_focus->ProcessEvent(e);
        else
            return false;
    }
}

bool ContainerWidget::InternalProcessRotateEvent (EventRotate const &e)
{
    // because a rotate event is not associated with a screen position, the only way it
    // makes sense to handle is like a keyboard event.  therefore send it down the focus line.

    if (m_children_get_input_events_first)
    {
        if (m_focus != NULL)
            return m_focus->ProcessEvent(e);
        else
            return Widget::InternalProcessRotateEvent(dynamic_cast<EventRotate const &>(e));
    }
    else
    {
        if (Widget::InternalProcessRotateEvent(dynamic_cast<EventRotate const &>(e)))
            return true;
        else if (m_focus != NULL)
            return m_focus->ProcessEvent(e);
        else
            return false;
    }
}

bool ContainerWidget::InternalProcessJoyEvent (EventJoy const &e)
{
    if (m_children_get_input_events_first)
    {
        if (m_focus != NULL)
            return m_focus->ProcessEvent(e);
        else if (ProcessJoyEvent(dynamic_cast<EventJoy const &>(e)))
            return true;
    }
    else
    {
        if (ProcessJoyEvent(dynamic_cast<EventJoy const &>(e)))
            return true;
        else if (m_focus != NULL)
            return m_focus->ProcessEvent(e);
    }

    return false;
}

bool ContainerWidget::InternalProcessFocusEvent (EventFocus const &e)
{
    // hidden widgets can't be focused
    if (IsHidden())
        return false;

    // widgets that don't accept focus and have no children (which
    // may potentially accept focus) return false
    if (!m_accepts_focus && m_child_vector.size() == 0)
        return false;

    // loop through all the child widgets (from top to bottom)
    WidgetVector::reverse_iterator it;
    WidgetVector::reverse_iterator it_end;
    for (it = m_child_vector.rbegin(), it_end = m_child_vector.rend(); it != it_end; ++it)
    {
        ASSERT1(*it != NULL);
        Widget &child = **it;
        if (!child.IsHidden() && child.ScreenRect().IsPointInside(e.Position()) && child.InternalProcessFocusEvent(e))
            return true;
    }

    // if none of the widgets accepted focus, hand it to the superclass' method
    return Widget::InternalProcessFocusEvent(e);
}

bool ContainerWidget::InternalProcessMouseoverEvent (EventMouseover const &e)
{
    // hidden widgets can't be moused over
    if (IsHidden())
        return false;

    // loop through all the child widgets (from top to bottom)
    WidgetVector::reverse_iterator it;
    WidgetVector::reverse_iterator it_end;
    for (it = m_child_vector.rbegin(), it_end = m_child_vector.rend(); it != it_end; ++it)
    {
        ASSERT1(*it != NULL);
        Widget &child = **it;
        if (child.ScreenRect().IsPointInside(e.Position()))
            if (child.InternalProcessMouseoverEvent(e))
                return true;
    }

    // if none of the widgets accepted mouseover-focus, hand it to the superclass' method
    return Widget::InternalProcessMouseoverEvent(e);
}

bool ContainerWidget::SendMouseEventToChild (EventMouse const &e)
{
    // attempt to send the mouse event to the widget that is below
    // the mouse cursor, traversing the child vector from top to
    // bottom.
    for (WidgetVector::reverse_iterator it = m_child_vector.rbegin(), it_end = m_child_vector.rend(); it != it_end; ++it)
    {
        ASSERT1(*it != NULL);
        Widget &child = **it;
        // only send the event to widgets that are not hidden
        // AND if the mouse event position is inside the widget's rect
        if (!child.IsHidden() &&
            child.ScreenRect().IsPointInside(e.Position()))
            if (child.ProcessEvent(e))
                return true;
    }
    // if no widget accepted it, return false
    return false;
}

void ContainerWidget::IncrementResizeBlockerCount ()
{
    ASSERT1(m_child_resize_blocker_count < UINT32_UPPER_BOUND);
    ++m_child_resize_blocker_count;
}

void ContainerWidget::DecrementResizeBlockerCount ()
{
    ASSERT1(m_child_resize_blocker_count > 0);
    --m_child_resize_blocker_count;
    if (m_child_resize_blocker_count == 0)
        Resize(Size());
    m_child_resize_was_blocked = false;
}

} // end of namespace Xrb
