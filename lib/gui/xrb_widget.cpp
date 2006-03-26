// ///////////////////////////////////////////////////////////////////////////
// xrb_widget.cpp by Victor Dods, created 2004/09/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_widget.h"

#include "xrb_gui_events.h"
#include "xrb_input_events.h"
#include "xrb_key.h"
#include "xrb_screen.h"
#include "xrb_widgetbackground.h"

namespace Xrb
{

// ///////////////////////////////////////////////////////////////////////////
// constructor and destructor
// ///////////////////////////////////////////////////////////////////////////

Widget::Widget (
    Widget *const parent,
    std::string const &name)
    :
    WidgetSkinHandler(),
    FrameHandler(),
    EventHandler(NULL),
    SignalHandler(),
    m_receiver_set_is_enabled(&Widget::SetIsEnabled, this),
    m_receiver_enable(&Widget::Enable, this),
    m_receiver_disable(&Widget::Disable, this),
    m_receiver_set_is_hidden(&Widget::SetIsHidden, this),
    m_receiver_hide(&Widget::Hide, this),
    m_receiver_show(&Widget::Show, this)
{
//     fprintf(stderr, "Widget::Widget(%s);\n", name.c_str());

    m_focus = NULL;
    m_focus_has_mouse_grab = false;
    m_accepts_focus = false;
    m_accepts_mouseover = true;
    m_mouseover_focus = NULL;
    m_render_background = NULL;
    m_name = name;
    m_is_enabled = true;
    m_is_hidden = false;
    m_screen_rect = ScreenCoordRect(0, 0, 500, 500); // arbitrary
    m_last_mouse_position = ScreenCoordVector2::ms_zero; // arbitrary
    m_color_mask = Color(1.0, 1.0, 1.0, 1.0);
    m_is_modal = false;
    m_main_widget = NULL;
    m_stack_priority = SP_NEUTRAL;
    m_background = NULL;
    m_render_background = NULL;
    m_frame_margins = ScreenCoordVector2::ms_zero;
    m_parent = NULL;

    // add this to the given parent
    if (parent != NULL)
    {
        SetOwnerEventQueue(parent->GetOwnerEventQueue());
        // m_parent will be set in this function call.
        parent->AttachChild(this);

        // this Widget has a WidgetSkin now, so do the proper initialization
        InitializeFromWidgetSkinProperties();
    }
}

Widget::~Widget ()
{
//     fprintf(stderr, "Widget::~Widget(%s);\n", m_name.c_str());

    // if this is a modal widget, make sure it gets taken off the
    // top-level widget's modal widget stack
    SetIsModal(false);
    // make sure that mouseover is off
    MouseoverOff();
    
    // DESTROY all the poor little child widgets (in reverse order).
    while (m_child_vector.size() > 0)
    {
        Widget *child = m_child_vector.back();
        ASSERT1(child != NULL)
        // this will detach the child from the parent and remove its
        // element from m_child_vector -- hence the weird traversal.
        Delete(child);
    }
    
    // clear the modal widget stack
    m_modal_widget_stack.clear();

    // detach this widget from its parent (if it has one), so that
    // "just deleting" a widget is acceptable.  detaching the widget
    // will clean up pointers that may otherwise be left dangling.
    if (m_parent != NULL)
        DetachFromParent();
    
    // nullify the pointers
    m_parent = NULL;
    m_focus = NULL;
    m_mouseover_focus = NULL;
    m_main_widget = NULL;
}

// ///////////////////////////////////////////////////////////////////////////
// accessors
// ///////////////////////////////////////////////////////////////////////////

Widget const *Widget::GetEffectiveParent () const
{
    return GetIsModal() ?
           static_cast<Widget const *>(GetTopLevelParent()) :
           m_parent;
}

Widget *Widget::GetEffectiveParent ()
{
    return GetIsModal() ?
           static_cast<Widget *>(GetTopLevelParent()) :
           m_parent;
}

Screen *Widget::GetTopLevelParent ()
{
    if (m_parent != NULL)
        return m_parent->GetTopLevelParent();
    else
        return static_cast<Screen *>(this);
}

Screen const *Widget::GetTopLevelParent () const
{
    if (m_parent != NULL)
        return m_parent->GetTopLevelParent();
    else
        return static_cast<Screen const *>(this);
}

bool Widget::GetIsMouseover () const
{
    Widget const *parent = GetEffectiveParent();
    return parent != NULL ?
           (parent->m_mouseover_focus == this) :
           true;
}

ScreenCoordVector2 Widget::GetAdjustedSize (ScreenCoordVector2 const &size) const
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

void Widget::SetSizePropertyEnabled (
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
        m_main_widget->SetSizePropertyEnabled(
            property,
            component,
            value,
            defer_parent_update);
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

void Widget::SetSizePropertyEnabled (
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
        m_main_widget->SetSizePropertyEnabled(
            property,
            value,
            defer_parent_update);
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

void Widget::SetSizeProperty (
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
        m_main_widget->SetSizeProperty(
            property,
            component,
            value,
            defer_parent_update);
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

void Widget::SetSizeProperty (
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
        m_main_widget->SetSizeProperty(
            property,
            value,
            defer_parent_update);
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

void Widget::SetSizePropertyRatio (
    SizeProperties::Property const property,
    Uint32 const component,
    Float const ratio,
    bool const defer_parent_update)
{
    ScreenCoord size_ratio_basis = GetTopLevelParent()->GetSizeRatioBasis();
    ScreenCoord calculated_value =
        static_cast<ScreenCoord>(size_ratio_basis * ratio);
    SetSizeProperty(property, component, calculated_value, defer_parent_update);
}

void Widget::SetSizePropertyRatios (
    SizeProperties::Property const property,
    FloatVector2 const &ratios,
    bool const defer_parent_update)
{
    ScreenCoord size_ratio_basis = GetTopLevelParent()->GetSizeRatioBasis();
    ScreenCoordVector2 calculated_value =
        (static_cast<Float>(size_ratio_basis) * ratios).StaticCast<ScreenCoord>();
    SetSizeProperty(property, calculated_value, defer_parent_update);
}

void Widget::SetIsModal (bool const is_modal)
{
    if (is_modal)
    {
        ASSERT0(!GetIsTopLevelParent() && "You can't make the top level widget modal!")
        ASSERT0(GetIsEnabled() && "You can't make a disabled widget modal!")
    }

    // only do stuff if the value is changing
    if (m_is_modal != is_modal)
    {
        // make sure that m_is_modal is still true during the call to
        // RemoveModalWidget, or before the call to AddModalWidget
        if (m_is_modal)
        {
            // make sure to remove mouseover when the modal state changes
            MouseoverOff();
            ASSERT1(!GetIsMouseover())
            // remove this widget from the modal stack of the top level parent
            RemoveModalWidget(this);
            m_is_modal = false;
            // send a mouseover event to the top level parent so that
            // mouseover is correctly set after the change in the
            // modal widget stack
            EventMouseover mouseover_event(
                GetTopLevelParent()->GetLastMousePosition(),
                GetTopLevelParent()->GetMostRecentEventTime());
            GetTopLevelParent()->ProcessEvent(&mouseover_event);
        }
        else
        {
            m_is_modal = true;
            AddModalWidget(this);
        }

        ParentChildSizePropertiesUpdate(false);        
    }
}

void Widget::SetMainWidget (Widget *const main_widget)
{
    m_main_widget = main_widget;
    if (m_main_widget != NULL)
    {
        ASSERT0(!m_main_widget->GetIsModal() &&
                "You can't use a modal widget as a main widget")
        ASSERT1(m_main_widget->GetParent() == this)
        m_main_widget->Resize(GetSize());
        m_main_widget->MoveTo(GetPosition());
    }
}

void Widget::SetStackPriority (StackPriority const stack_priority)
{
    if (m_stack_priority != stack_priority)
    {
        StackPriority previous_stack_priority = m_stack_priority;
        m_stack_priority = stack_priority;
        if (GetParent())
            GetParent()->ChildStackPriorityChanged(this, previous_stack_priority);
    }
}

void Widget::SetBackground (WidgetBackground *const background)
{
    Delete(m_background);
    m_background = background;
    HandleChangedBackground();
}

void Widget::SetFrameMargins (ScreenCoordVector2 const &frame_margins)
{
    if (m_frame_margins != frame_margins)
    {
        m_frame_margins = frame_margins;
        HandleChangedFrameMargins();
    }
}

void Widget::SetFrameMarginRatios (FloatVector2 const &frame_margin_ratios)
{
    ScreenCoord size_ratio_basis = GetTopLevelParent()->GetSizeRatioBasis();
    ScreenCoordVector2 calculated_frame_margins =
        (static_cast<Float>(size_ratio_basis) *
         frame_margin_ratios).StaticCast<ScreenCoord>();
    SetFrameMargins(calculated_frame_margins);
}

// ///////////////////////////////////////////////////////////////////////////
// procedures
// ///////////////////////////////////////////////////////////////////////////

void Widget::FixSize (ScreenCoordVector2 const &size)
{
    SetSizeProperty(SizeProperties::MIN, size);
    SetSizeProperty(SizeProperties::MAX, size);
    SetSizePropertyEnabled(SizeProperties::MIN, Bool2(true, true));
    SetSizePropertyEnabled(SizeProperties::MAX, Bool2(true, true));
}

void Widget::FixSizeRatios (FloatVector2 const &size_ratios)
{
    SetSizePropertyRatios(SizeProperties::MIN, size_ratios);
    SetSizePropertyRatios(SizeProperties::MAX, size_ratios);
    SetSizePropertyEnabled(SizeProperties::MIN, Bool2(true, true));
    SetSizePropertyEnabled(SizeProperties::MAX, Bool2(true, true));
}

void Widget::UnfixSize ()
{
    SetSizePropertyEnabled(SizeProperties::MIN, Bool2(false, false));
    SetSizePropertyEnabled(SizeProperties::MAX, Bool2(false, false));
}

void Widget::FixWidth (ScreenCoord const width)
{
    SetSizeProperty(SizeProperties::MIN, Dim::X, width);
    SetSizeProperty(SizeProperties::MAX, Dim::X, width);
    SetSizePropertyEnabled(SizeProperties::MIN, Dim::X, true);
    SetSizePropertyEnabled(SizeProperties::MAX, Dim::X, true);
}

void Widget::FixWidthRatio (Float const width_ratio)
{
    SetSizePropertyRatio(SizeProperties::MIN, Dim::X, width_ratio);
    SetSizePropertyRatio(SizeProperties::MAX, Dim::X, width_ratio);
    SetSizePropertyEnabled(SizeProperties::MIN, Dim::X, true);
    SetSizePropertyEnabled(SizeProperties::MAX, Dim::X, true);
}

void Widget::UnfixWidth ()
{
    SetSizePropertyEnabled(SizeProperties::MIN, Dim::X, false);
    SetSizePropertyEnabled(SizeProperties::MAX, Dim::X, false);
}

void Widget::FixHeight (ScreenCoord const height)
{
    SetSizeProperty(SizeProperties::MIN, Dim::Y, height);
    SetSizeProperty(SizeProperties::MAX, Dim::Y, height);
    SetSizePropertyEnabled(SizeProperties::MIN, Dim::Y, true);
    SetSizePropertyEnabled(SizeProperties::MAX, Dim::Y, true);
}

void Widget::FixHeightRatio (Float const height_ratio)
{
    SetSizePropertyRatio(SizeProperties::MIN, Dim::Y, height_ratio);
    SetSizePropertyRatio(SizeProperties::MAX, Dim::Y, height_ratio);
    SetSizePropertyEnabled(SizeProperties::MIN, Dim::Y, true);
    SetSizePropertyEnabled(SizeProperties::MAX, Dim::Y, true);
}

void Widget::UnfixHeight ()
{
    SetSizePropertyEnabled(SizeProperties::MIN, Dim::Y, false);
    SetSizePropertyEnabled(SizeProperties::MAX, Dim::Y, false);
}

void Widget::Draw (RenderContext const &render_context) const
{
    Float const disabled_widget_alpha_mask = 0.5f;

    // if the background exists, draw it
    if (GetRenderBackground())
        GetRenderBackground()->Draw(render_context,
                                    GetScreenRect(),
                                    GetFrameMargins());

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
}

void Widget::MoveTo (ScreenCoordVector2 const &position)
{
    MoveBy(position - GetPosition());
}

void Widget::MoveBy (ScreenCoordVector2 const &delta)
{
    // move this widget by the given delta
    m_screen_rect += delta;

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

ScreenCoordVector2 Widget::Resize (ScreenCoordVector2 const &size)
{
    ScreenCoordVector2 adjusted_size(m_size_properties.GetAdjustedSize(size));

    if (m_screen_rect.GetSize() != adjusted_size)
    {
        m_screen_rect.SetSize(adjusted_size);

        // if there is a main widget, resize it to match this one
        if (m_main_widget != NULL)
            m_main_widget->Resize(m_screen_rect.GetSize());

        // range checking
        SizeRangeAdjustment(&m_screen_rect);
        // indicate to the parent that a child has changed size properties
        ParentChildSizePropertiesUpdate(false);
    }

    // return what the actual size is now
    return m_screen_rect.GetSize();
}

ScreenCoordVector2 Widget::ResizeByRatios (FloatVector2 const &ratios)
{
    return Resize(
        (static_cast<Float>(GetTopLevelParent()->GetSizeRatioBasis()) *
         ratios).StaticCast<ScreenCoord>());
}

ScreenCoordVector2 Widget::MoveToAndResize (ScreenCoordRect const &screen_rect)
{
    MoveTo(screen_rect.GetBottomLeft());
    return Resize(screen_rect.GetSize());
}

void Widget::CenterOnWidget (Widget const *const widget)
{
    MoveTo(widget->GetPosition() + (widget->GetSize() - GetSize()) / 2);
}

bool Widget::Focus ()
{
    // you may not focus a hidden widget.
    ASSERT1(!GetIsHidden())

    // if this widget doesn't accept focus then return
    if (!m_accepts_focus)
        return false;

    // if already focused, then we don't need to do anything
    if (GetIsFocused())
        return true;

    // if this is not a top level widget, proceed normally
    if (m_parent != NULL)
    {
        // find the first ancestor of this widget that is focused
        Widget *first_focused_ancestor = m_parent;
        while (first_focused_ancestor->m_parent &&
               !first_focused_ancestor->GetIsFocused())
        {
            first_focused_ancestor = first_focused_ancestor->m_parent;
        }

        // unfocus all widgets from the focused child of the first focused
        // ancestor down
        if (first_focused_ancestor->m_focus)
            first_focused_ancestor->m_focus->UnfocusWidgetLine();
    }

    // focus all widgets from this widget on up to the first focused ancestor
    FocusWidgetLine();

    // focus was taken, return true
    return true;
}

void Widget::Unfocus ()
{
    // if not focused, then we don't need to do anything
    if (!GetIsFocused())
        return;

    // unfocus this widget and all its children (from bottom up)
    UnfocusWidgetLine();
}

void Widget::GrabMouse ()
{
    // if this widget already has the mouse grabbed, don't do anything
    if (GetIsMouseGrabbed())
        return;

    // gotta make sure that this widget has focus
    Focus();

    // set the mouse grab on this and up through the parent widgets
    if (m_parent != NULL)
    {
        // recurse to the parent widget
        m_parent->GrabMouse();
        // set this widget to mouse grabbing
        m_parent->m_focus_has_mouse_grab = true;

        // call the mouse grab handler and emit the mouse grab on signals
        HandleMouseGrabOn();
    }
}

void Widget::UnGrabMouse ()
{
    // if this widget already doesn't have the mouse grabbed, don't do anything
    if (!GetIsMouseGrabbed())
        return;

    // unset the mouse grab on this and up through the parent widgets
    if (m_parent != NULL)
    {
        // set this widget to not mouse grabbing
        m_parent->m_focus_has_mouse_grab = false;

        // call the mouse grab handler and emit the mouse grab on signals
        HandleMouseGrabOff();

        // recurse to the parent widget
        m_parent->UnGrabMouse();
    }
}

void Widget::AttachChild (Widget *const child)
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

void Widget::DetachChild (Widget *const child)
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

void Widget::DetachFromParent ()
{
    // i put an assert in here to make sure no one behaves badly and tries
    // to detach a top-level widget
    ASSERT1(m_parent != NULL)
    m_parent->DetachChild(this);
}

void Widget::MoveChildDown (Widget *const child)
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

void Widget::MoveChildUp (Widget *const child)
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

void Widget::MoveChildToBottom (Widget *const child)
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

void Widget::MoveChildToTop (Widget *const child)
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

void Widget::SetIsEnabled (bool const is_enabled)
{
    if (!is_enabled)
    {
        ASSERT0(!GetIsModal() && "You can't disable a modal widget!")
        ASSERT0(!GetIsTopLevelParent() && "You can't disable a top level widget!")
    }

    if (m_is_enabled != is_enabled)
    {
        m_is_enabled = is_enabled;
        // if the widget is disabled, unfocus it and disable outgoing signalsenders
        if (!m_is_enabled)
        {
            Unfocus();
            m_enabled_sender_blocking_state = GetIsBlockingSenders();
            SetIsBlockingSenders(true);
        }
        // otherwise restore the saved sender blocking state
        else
        {
            SetIsBlockingSenders(m_enabled_sender_blocking_state);
        }

        // stick signalsenders in here
    }
}

void Widget::ToggleIsHidden ()
{
    ASSERT0(m_parent != NULL && "You can't show or hide a top-level widget!")

    // if the widget is being hidden, then it should be
    // unfocused and mouseover-off'ed
    if (!m_is_hidden)
    {
        Unfocus();
        MouseoverOff();
    }
    
    m_is_hidden = !m_is_hidden;


    if (!GetIsModal())
        ParentChildSizePropertiesUpdate(false);
}

void Widget::SetIsHidden (bool const is_hidden)
{
    if (m_is_hidden != is_hidden)
        ToggleIsHidden();
}

// ///////////////////////////////////////////////////////////////////////////
// protected functions
// ///////////////////////////////////////////////////////////////////////////

Uint32 Widget::GetWidgetSkinHandlerChildCount () const
{
    return m_child_vector.size();
}

WidgetSkinHandler *Widget::GetWidgetSkinHandlerChild (Uint32 const index)
{
    ASSERT1(index < m_child_vector.size())
    Widget *child = m_child_vector[index];
    ASSERT1(child != NULL)
    return static_cast<WidgetSkinHandler *>(child);
}

WidgetSkinHandler *Widget::GetWidgetSkinHandlerParent ()
{
    return static_cast<WidgetSkinHandler *>(m_parent);
}

Bool2 Widget::GetContentsMinSizeEnabled () const
{
    ASSERT1(m_main_widget != NULL)
    return m_main_widget->GetMinSizeEnabled();
}

ScreenCoordVector2 Widget::GetContentsMinSize () const
{
    ASSERT1(m_main_widget != NULL)
    return m_main_widget->GetMinSize();
}

Bool2 Widget::GetContentsMaxSizeEnabled () const
{
    ASSERT1(m_main_widget != NULL)
    return m_main_widget->GetMaxSizeEnabled();
}

ScreenCoordVector2 Widget::GetContentsMaxSize () const
{
    ASSERT1(m_main_widget != NULL)
    return m_main_widget->GetMaxSize();
}

void Widget::InitializeFromWidgetSkinProperties ()
{
    UpdateRenderBackground();
    SetFrameMargins(GetWidgetSkinMargins(WidgetSkin::DEFAULT_FRAME_MARGINS));
}

void Widget::ProcessFrameOverride ()
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

bool Widget::ProcessEventOverride (Event const *const e)
{
    ASSERT1(e != NULL)

    // if this widget is disabled, it rejects all events
    if (!GetIsEnabled())
        return false;

    // mouse-position-related preprocessing
    if (e->GetType() == Event::MOUSEOVER)
    {
        EventMouseover const *mouseover_event =
            static_cast<EventMouseover const *const>(e);
        m_last_mouse_position = mouseover_event->GetPosition();
    }

    // the top-level widget gets an opportunity to pre-process events
    // (e.g. for generating mouseover events, or sending them to
    // modal widgets)
    if (GetIsTopLevelParent())
    {
        if (e->GetIsMouseMotionEvent())
        {
            EventMouseMotion const *mouse_motion_event =
                static_cast<EventMouseMotion const *const>(e);

            // generate a mouseover event from the mouse motion event
            EventMouseover mouseover_event(
                mouse_motion_event->GetPosition(),
                mouse_motion_event->GetTime());
            ProcessEvent(&mouseover_event);
        }

        // get the top of the modal widget stack
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

        // if there's a non-hidden modal widget, send the event to it.         
        if (modal_widget != NULL)
        {
            // check if this is a mouse event and it doesn't fall inside the
            // top modal widget.  if so, throw the event out
            if (e->GetIsMouseEvent())
            {
                EventMouse const *mouse_event =
                    static_cast<EventMouse const *>(e);
                if (!modal_widget->GetScreenRect().GetIsPointInside(
                        mouse_event->GetPosition()))
                {
                    return false;
                }
            }

            return modal_widget->ProcessEvent(e);
        }
    }
    else
    {
        ASSERT1(m_modal_widget_stack.empty())
    }

    switch (e->GetType())
    {
        case Event::KEYDOWN:
        case Event::KEYUP:
        case Event::KEYREPEAT:
            if (ProcessKeyEvent(static_cast<EventKey const *const>(e)))
                return true;
            else if (m_focus != NULL)
                return m_focus->ProcessEvent(e);
            else
                return false;

        case Event::MOUSEBUTTONDOWN:
        case Event::MOUSEBUTTONUP:
        case Event::MOUSEMOTION:
            if (PreprocessMouseEvent(static_cast<EventMouse const *const>(e)))
                return true;
            else
                return SendMouseEventToChild(
                    static_cast<EventMouse const *const>(e));

        case Event::MOUSEWHEEL:
            return PreprocessMouseWheelEvent(
                static_cast<EventMouseWheel const *const>(e));

        case Event::JOYAXIS:
        case Event::JOYBALL:
        case Event::JOYBUTTONDOWN:
        case Event::JOYBUTTONUP:
        case Event::JOYHAT:
            if (ProcessJoyEvent(static_cast<EventJoy const *const>(e)))
                return true;
            else if (m_focus != NULL)
                return m_focus->ProcessEvent(e);
            else
                return false;

        case Event::FOCUS:
            return PreprocessFocusEvent(
                static_cast<EventFocus const *const>(e));

        case Event::MOUSEOVER:
            return PreprocessMouseoverEvent(
                static_cast<EventMouseover const *const>(e));

        case Event::DELETE_CHILD_WIDGET:
            return ProcessDeleteChildWidgetEvent(
                static_cast<EventDeleteChildWidget const *const>(e));

        case Event::ACTIVE:
        case Event::RESIZE:
        case Event::EXPOSE:
        case Event::QUIT:
        case Event::SYSWM:
            return false;

        case Event::CUSTOM:
            return ProcessCustomEvent(e);

        default:
            ASSERT0(false && "Unknown event type")
            return false;
    }
}

bool Widget::ProcessDeleteChildWidgetEvent (
    EventDeleteChildWidget const *const e)
{
    ASSERT1(e->GetChildToDelete() != NULL)
    ASSERT1(e->GetChildToDelete()->GetParent() == this)
    // detach the child before deleting it
    DetachChild(e->GetChildToDelete());
    e->DeleteChildWidget();
    return true;
}

void Widget::HandleChangedBackground ()
{
    UpdateRenderBackground();
}

void Widget::AddModalWidget (Widget *const modal_widget)
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

void Widget::RemoveModalWidget (Widget *const modal_widget)
{
    ASSERT1(modal_widget != NULL)
    ASSERT1(modal_widget->GetIsModal())
    ASSERT1(modal_widget->GetIsEnabled())

    // if this is a top level widget, then remove the modal widget from the
    // modal widget stack.  otherwise, pass it up to the parent
    if (GetIsTopLevelParent())
    {
        ASSERT1(modal_widget == *m_modal_widget_stack.rbegin())
        modal_widget->Unfocus();
        m_modal_widget_stack.pop_back();
    }
    else
        GetParent()->RemoveModalWidget(modal_widget);
}

void Widget::CalculateMinAndMaxSizePropertiesFromContents ()
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

void Widget::ChildSizePropertiesChanged (Widget *const child)
{
    ASSERT1(child != NULL)
    ASSERT1(child->GetParent() == this)
    if (child == m_main_widget)
    {
        // adjust the size properties based on the contents (the main widget)
        CalculateMinAndMaxSizePropertiesFromContents();
        // attempt to resize the widget to the current size
        Resize(m_main_widget->GetSize());
    }
}

void Widget::ChildStackPriorityChanged (
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

void Widget::UpdateRenderBackground ()
{
    SetRenderBackground(GetBackground());
}

void Widget::ParentChildSizePropertiesUpdate (bool const defer_parent_update)
{
    if (!defer_parent_update && GetParent())
        GetParent()->ChildSizePropertiesChanged(this);
}

bool Widget::AdjustFromMinSize (ScreenCoordRect *const screen_rect) const
{
    // to keep track of if we need to call Resize()
    bool adjusted = false;
    // check it against the minimum size and if the size needs to be
    // adjusted, do so and flag adjusted
    for (Uint32 i = Dim::X; i <= Dim::Y; ++i)
    {
        if (m_size_properties.m_min_size_enabled[i] &&
            screen_rect->GetSize()[i] < m_size_properties.m_min_size[i])
        {
            screen_rect->SetSize(i, m_size_properties.m_min_size[i]);
            adjusted = true;
        }
    }
    return adjusted;
}

bool Widget::AdjustFromMaxSize (ScreenCoordRect *const screen_rect) const
{
    // to keep track of if we need to call Resize()
    bool adjusted = false;
    // check it against the maximum size and if the X size needs to be
    // adjusted, do so and flag adjusted
    for (Uint32 i = Dim::X; i <= Dim::Y; ++i)
    {
        if (m_size_properties.m_max_size_enabled[i] &&
            screen_rect->GetSize()[i] > m_size_properties.m_max_size[i])
        {
            screen_rect->SetSize(i, m_size_properties.m_max_size[i]);
            adjusted = true;
        }
    }
    return adjusted;
}

void Widget::MinSizeUpdated ()
{
    ScreenCoordRect temp;
    // make sure the size is non-negative
    temp = m_size_properties.m_min_size;
    SizeRangeAdjustment(&temp);
    m_size_properties.m_min_size = temp.GetSize();
    // make sure that min/max have the correct relationship
    temp = m_size_properties.m_max_size;
    AdjustFromMinSize(&temp);
    m_size_properties.m_max_size = temp.GetSize();
}

void Widget::MaxSizeUpdated ()
{
    ScreenCoordRect temp;
    // make sure the size is non-negative
    temp = m_size_properties.m_max_size;
    SizeRangeAdjustment(&temp);
    m_size_properties.m_max_size = temp.GetSize();
    // make sure that min/max have the correct relationship
    temp = m_size_properties.m_min_size;
    AdjustFromMaxSize(&temp);
    m_size_properties.m_min_size = temp.GetSize();
}

void Widget::SizeRangeAdjustment (ScreenCoordRect *const rect) const
{
    // make sure the size is non-negative
    if (rect->GetSize()[Dim::X] < 0)
        rect->SetSize(Dim::X, 0);
    if (rect->GetSize()[Dim::Y] < 0)
        rect->SetSize(Dim::Y, 0);
}

// ///////////////////////////////////////////////////////////////////////////
// private functions
// ///////////////////////////////////////////////////////////////////////////

Widget::WidgetVectorIterator Widget::FindChildWidget (Widget const *const child)
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

void Widget::FocusWidgetLine ()
{
    ASSERT1(!GetIsFocused())
    ASSERT1(m_focus == NULL)

    // make sure to focus parent widgets first, so that the focusing
    // happens from top down
    if (m_parent != NULL && !m_parent->GetIsFocused())
        m_parent->FocusWidgetLine();

    // make this widget focused
    if (m_parent != NULL)
        m_parent->m_focus = this;

    // call the focus handler and emit the focus signals
    HandleFocus();
}

void Widget::UnfocusWidgetLine ()
{
    ASSERT1(GetIsFocused())

    // make sure to unfocus child widgets first, so that the unfocusing
    // happens from bottom up
    if (m_focus != NULL)
        m_focus->UnfocusWidgetLine();

    // set the focus state
    if (m_parent != NULL)
        m_parent->m_focus = 0;

    // call the unfocus handler and emit the focus signals
    HandleUnfocus();
}

bool Widget::MouseoverOn ()
{
    // if this widget doesn't even accept mouseover-focus, then return
    if (!m_accepts_mouseover)
        return false;

    // if this widget is hidden, then it can't be moused-over
    if (GetIsHidden())
        return false;

    // if already mouseover-focused, then make sure there are no child
    // widgets that have mouseover, and then return true.
    if (GetIsMouseover())
    {
        if (m_mouseover_focus != NULL)
            m_mouseover_focus->MouseoverOffWidgetLine();
        return true;
    }

    Widget *parent = GetEffectiveParent();

    // if this is not a top level widget, proceed normally
    if (parent != NULL)
    {
        // find the first ancestor of this widget that is mouseover-focused
        Widget *first_mouseover_ancestor = parent;
        while (first_mouseover_ancestor->GetEffectiveParent() != NULL&&
               !first_mouseover_ancestor->GetIsMouseover())
        {
            first_mouseover_ancestor =
                first_mouseover_ancestor->GetEffectiveParent();
        }

        // unfocus all widgets from the mouseover-focused child of the first
        // mouseover-focused ancestor down
        if (first_mouseover_ancestor->m_mouseover_focus != NULL)
            first_mouseover_ancestor->m_mouseover_focus->MouseoverOffWidgetLine();
    }

    // mouseover-focus all widgets from this widget on up to the
    // first mouseover-focused ancestor
    MouseoverOnWidgetLine();

    // mouseover-focus was taken, return true
    return true;
}

void Widget::MouseoverOff ()
{
    if (GetIsTopLevelParent())
    {
        MouseoverOffWidgetLine();
        return;
    }

    if (!m_accepts_mouseover)
    {
        ASSERT1(!GetIsMouseover())
        return;
    }

    if (GetIsHidden())
    {
        ASSERT1(!GetIsMouseover())
        return;
    }

    if (GetIsMouseover())
        MouseoverOffWidgetLine();
}

void Widget::MouseoverOnWidgetLine ()
{
    ASSERT1(!GetIsMouseover())
    ASSERT1(m_mouseover_focus == NULL)

    Widget *parent = GetEffectiveParent();

    // make sure to mouseover-focus parent widgets first, so that
    // the mouseover-focusing happens from top down
    if (parent != NULL && !parent->GetIsMouseover())
        parent->MouseoverOnWidgetLine();

    // make this widget mouseover-focus state
    if (parent != NULL)
        parent->m_mouseover_focus = this;

    // call the mouseover-unfocus handler and emit the mouseover-focus signals
    HandleMouseoverOn();
}

void Widget::MouseoverOffWidgetLine ()
{
    ASSERT1(GetIsMouseover())

    Widget *parent = GetEffectiveParent();

    // make sure to mouseover-unfocus child widgets first, so that the
    // mouseover-unfocusing happens from bottom up
    if (m_mouseover_focus != NULL)
        m_mouseover_focus->MouseoverOffWidgetLine();

    // set the mouseover-focus state
    if (parent != NULL)
        parent->m_mouseover_focus = NULL;

    // call the mouseover-unfocus handler and emit the mouseover-focus signals
    HandleMouseoverOff();
}

bool Widget::PreprocessMouseEvent (EventMouse const *const e)
{
    ASSERT1(e != NULL)

    // if there is a widget in focus and it has mouse grab
    // on, send the mouse event there
    if (m_focus != NULL && m_focus_has_mouse_grab)
        return m_focus->ProcessEvent(e);

    // now give this widget the chance to process the event
    switch (e->GetType())
    {
        case Event::MOUSEBUTTONUP:
        case Event::MOUSEBUTTONDOWN:
            // let this widget have an opportunity at the event
            if (ProcessMouseButtonEvent((EventMouseButton const *const)e))
                return true;
            // if the event was not used, send a focus event down the line
            else
            {
                // create a focus event
                EventFocus focus_event(
                    static_cast<EventMouseButton const *const>(e)->GetPosition(),
                    e->GetTime());
                // send it to the event processor
                ProcessEvent(&focus_event);
            }
            break;

        case Event::MOUSEMOTION:
        {
            EventMouseMotion const *mouse_motion_event =
                static_cast<EventMouseMotion const *const>(e);
            // let this widget have an opportunity at the event
            bool retval = ProcessMouseMotionEvent(mouse_motion_event);

            // save the event's position as the last known
            // mouse position
            m_last_mouse_position = e->GetPosition();

            return retval;
        }

        default:
            ASSERT1(false && "Invalid/unknown event type")
            break;
    }

    // no widget accepted it, return false
    return false;
}

bool Widget::PreprocessMouseWheelEvent (EventMouseWheel const *const e)
{
    // if there is a widget in focus and it has mouse grab
    // on, send the mouse event there
    if (m_focus != NULL && m_focus_has_mouse_grab)
        return m_focus->ProcessEvent(e);

    // otherwise let this widget have a chance at the event
    if (ProcessMouseWheelEvent(e))
        return true;

    // otherwise attempt to send it to the widget that is below
    // the mouse cursor
    return SendMouseEventToChild(e);
}

bool Widget::PreprocessFocusEvent (EventFocus const *const e)
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
                break;
        }

        // if none of the widgets accepted focus, then this is the bottom-most
        // widget which should be focused.
        if (it == it_end && m_accepts_focus)
        {
            bool retval = Focus();
            ASSERT1(retval)
            return retval;
        }
        else
            return false;
    }
}

bool Widget::PreprocessMouseoverEvent (EventMouseover const *const e)
{
    // hidden widgets can't be moused over
    if (GetIsHidden())
        return false;

    // widgets that don't accept mouseover-focus return false
    if (!m_accepts_mouseover)
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
                break;
    }

    // if none of the widgets accepted mouseover-focus, then this is the
    // bottom-most widget which should be focused.
    if (it == it_end)
        MouseoverOn();

    return true;
}

bool Widget::SendMouseEventToChild (EventMouse const *const e)
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

} // end of namespace Xrb
