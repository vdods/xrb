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

#include "xrb_containerwidget.h"
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

Widget::Widget (ContainerWidget *const parent, std::string const &name)
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

    m_accepts_focus = false;
    m_accepts_mouseover = true;
    m_render_background = NULL;
    m_name = name;
    m_is_enabled = true;
    m_is_hidden = false;
    m_screen_rect = ScreenCoordRect(0, 0, 500, 500); // arbitrary
    m_last_mouse_position = ScreenCoordVector2::ms_zero; // arbitrary
    m_color_mask = Color(1.0, 1.0, 1.0, 1.0);
    m_is_modal = false;
    m_stack_priority = SP_NEUTRAL;
    m_background = NULL;
    m_render_background = NULL;
    m_frame_margins = ScreenCoordVector2::ms_zero;
    m_content_margins = ScreenCoordVector2::ms_zero;
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

    // detach this widget from its parent (if it has one), so that
    // "just deleting" a widget is acceptable.  detaching the widget
    // will clean up pointers that may otherwise be left dangling.
    if (m_parent != NULL)
        DetachFromParent();
    ASSERT1(m_parent == NULL)
}

// ///////////////////////////////////////////////////////////////////////////
// accessors
// ///////////////////////////////////////////////////////////////////////////

ContainerWidget const *Widget::GetEffectiveParent () const
{
    return GetIsModal() ?
           DStaticCast<ContainerWidget const *>(GetTopLevelParent()) :
           m_parent;
}

ContainerWidget *Widget::GetEffectiveParent ()
{
    return GetIsModal() ?
           DStaticCast<ContainerWidget *>(GetTopLevelParent()) :
           m_parent;
}

Screen const *Widget::GetTopLevelParent () const
{
    if (m_parent != NULL)
        return m_parent->GetTopLevelParent();
    else
        return DStaticCast<Screen const *>(this);
}

Screen *Widget::GetTopLevelParent ()
{
    if (m_parent != NULL)
        return m_parent->GetTopLevelParent();
    else
        return DStaticCast<Screen *>(this);
}

bool Widget::GetIsFocused () const
{
    ContainerWidget const *parent = GetEffectiveParent();
    return parent != NULL ? parent->m_focus == this : true;
}

bool Widget::GetIsMouseover () const
{
    ContainerWidget const *parent = GetEffectiveParent();
    return parent != NULL ? parent->m_mouseover_focus == this : true;
}

bool Widget::GetIsMouseGrabbed () const
{
    ContainerWidget const *parent = GetEffectiveParent();
    return parent != NULL ? parent->m_focus == this && parent->m_focus_has_mouse_grab : true;
}

ScreenCoordVector2 Widget::GetAdjustedSize (ScreenCoordVector2 const &size) const
{
    ScreenCoordRect rect(size);
    AdjustFromMinSize(&rect);
    AdjustFromMaxSize(&rect);
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
    ASSERT1(component <= 1)
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

void Widget::SetSizePropertyEnabled (
    SizeProperties::Property const property,
    Bool2 const &value,
    bool const defer_parent_update)
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

void Widget::SetSizeProperty (
    SizeProperties::Property const property,
    Uint32 const component,
    ScreenCoord const value,
    bool const defer_parent_update)
{
    ASSERT1(component <= 1)
    ASSERT1(value >= 0)
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

void Widget::SetSizeProperty (
    SizeProperties::Property const property,
    ScreenCoordVector2 const &value,
    bool const defer_parent_update)
{
    ASSERT1(value[Dim::X] >= 0)
    ASSERT1(value[Dim::Y] >= 0)
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
        // make sure to remove mouseover when the modal state changes
        MouseoverOff();

        // make sure that m_is_modal is still true during the call to
        // RemoveModalWidget, or before the call to AddModalWidget
        if (m_is_modal)
        {
            ASSERT1(!GetIsMouseover())
            // remove this widget from the modal stack of the top level parent
            GetTopLevelParent()->RemoveModalWidget(this);
            m_is_modal = false;
        }
        else
        {
            m_is_modal = true;
            GetTopLevelParent()->AddModalWidget(this);
        }

        ParentChildSizePropertiesUpdate(false);
    }
}

void Widget::SetStackPriority (StackPriority const stack_priority)
{
    if (m_stack_priority != stack_priority)
    {
        StackPriority previous_stack_priority = m_stack_priority;
        m_stack_priority = stack_priority;
        if (m_parent != NULL)
            m_parent->ChildStackPriorityChanged(this, previous_stack_priority);
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

void Widget::SetContentMargins (ScreenCoordVector2 const &content_margins)
{
    ScreenCoordVector2 adjusted_content_margins(
        Max(content_margins[Dim::X], -m_frame_margins[Dim::X]),
        Max(content_margins[Dim::Y], -m_frame_margins[Dim::Y]));
    if (m_content_margins != adjusted_content_margins)
    {
        m_content_margins = adjusted_content_margins;
        HandleChangedContentMargins();
    }
}

void Widget::SetContentMarginRatios (FloatVector2 const &content_margin_ratios)
{
    ScreenCoord size_ratio_basis = GetTopLevelParent()->GetSizeRatioBasis();
    ScreenCoordVector2 calculated_content_margins =
        (static_cast<Float>(size_ratio_basis) *
         content_margin_ratios).StaticCast<ScreenCoord>();
    SetContentMargins(calculated_content_margins);
}

// ///////////////////////////////////////////////////////////////////////////
// procedures
// ///////////////////////////////////////////////////////////////////////////

void Widget::FixSize (ScreenCoordVector2 const &size)
{
    // TODO: make into atomic operation
    SetSizeProperty(SizeProperties::MIN, size);
    SetSizeProperty(SizeProperties::MAX, size);
    SetSizePropertyEnabled(SizeProperties::MIN, Bool2(true, true));
    SetSizePropertyEnabled(SizeProperties::MAX, Bool2(true, true));
}

void Widget::FixSizeRatios (FloatVector2 const &size_ratios)
{
    // TODO: make into atomic operation
    SetSizePropertyRatios(SizeProperties::MIN, size_ratios);
    SetSizePropertyRatios(SizeProperties::MAX, size_ratios);
    SetSizePropertyEnabled(SizeProperties::MIN, Bool2(true, true));
    SetSizePropertyEnabled(SizeProperties::MAX, Bool2(true, true));
}

void Widget::UnfixSize ()
{
    // TODO: make into atomic operation
    SetSizePropertyEnabled(SizeProperties::MIN, Bool2(false, false));
    SetSizePropertyEnabled(SizeProperties::MAX, Bool2(false, false));
}

void Widget::FixWidth (ScreenCoord const width)
{
    // TODO: make into atomic operation
    SetSizeProperty(SizeProperties::MIN, Dim::X, width);
    SetSizeProperty(SizeProperties::MAX, Dim::X, width);
    SetSizePropertyEnabled(SizeProperties::MIN, Dim::X, true);
    SetSizePropertyEnabled(SizeProperties::MAX, Dim::X, true);
}

void Widget::FixWidthRatio (Float const width_ratio)
{
    // TODO: make into atomic operation
    SetSizePropertyRatio(SizeProperties::MIN, Dim::X, width_ratio);
    SetSizePropertyRatio(SizeProperties::MAX, Dim::X, width_ratio);
    SetSizePropertyEnabled(SizeProperties::MIN, Dim::X, true);
    SetSizePropertyEnabled(SizeProperties::MAX, Dim::X, true);
}

void Widget::UnfixWidth ()
{
    // TODO: make into atomic operation
    SetSizePropertyEnabled(SizeProperties::MIN, Dim::X, false);
    SetSizePropertyEnabled(SizeProperties::MAX, Dim::X, false);
}

void Widget::FixHeight (ScreenCoord const height)
{
    // TODO: make into atomic operation
    SetSizeProperty(SizeProperties::MIN, Dim::Y, height);
    SetSizeProperty(SizeProperties::MAX, Dim::Y, height);
    SetSizePropertyEnabled(SizeProperties::MIN, Dim::Y, true);
    SetSizePropertyEnabled(SizeProperties::MAX, Dim::Y, true);
}

void Widget::FixHeightRatio (Float const height_ratio)
{
    // TODO: make into atomic operation
    SetSizePropertyRatio(SizeProperties::MIN, Dim::Y, height_ratio);
    SetSizePropertyRatio(SizeProperties::MAX, Dim::Y, height_ratio);
    SetSizePropertyEnabled(SizeProperties::MIN, Dim::Y, true);
    SetSizePropertyEnabled(SizeProperties::MAX, Dim::Y, true);
}

void Widget::UnfixHeight ()
{
    // TODO: make into atomic operation
    SetSizePropertyEnabled(SizeProperties::MIN, Dim::Y, false);
    SetSizePropertyEnabled(SizeProperties::MAX, Dim::Y, false);
}

void Widget::Draw (RenderContext const &render_context) const
{
    // if the background exists, draw it
    if (GetRenderBackground())
        GetRenderBackground()->Draw(render_context, GetScreenRect(), GetFrameMargins());
}

void Widget::MoveTo (ScreenCoordVector2 const &position)
{
    MoveBy(position - GetPosition());
}

void Widget::MoveBy (ScreenCoordVector2 const &delta)
{
    // move this widget by the given delta
    m_screen_rect += delta;
}

ScreenCoordVector2 Widget::Resize (ScreenCoordVector2 const &size)
{
    ScreenCoordVector2 adjusted_size(m_size_properties.GetAdjustedSize(size));

    if (m_screen_rect.GetSize() != adjusted_size)
    {
        m_screen_rect.SetSize(adjusted_size);
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
        ContainerWidget *first_focused_ancestor = GetEffectiveParent();
        while (first_focused_ancestor->GetEffectiveParent() != NULL &&
               !first_focused_ancestor->GetIsFocused())
        {
            first_focused_ancestor = first_focused_ancestor->GetEffectiveParent();
        }

        // unfocus all widgets from the focused child of the first focused
        // ancestor down
        ASSERT1(first_focused_ancestor != NULL)
        if (first_focused_ancestor->m_focus != NULL)
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
    ContainerWidget *parent = GetEffectiveParent();
    if (parent != NULL)
    {
        // recurse to the parent widget
        parent->GrabMouse();
        // set this widget to mouse grabbing
        parent->m_focus_has_mouse_grab = true;
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
    ContainerWidget *parent = GetEffectiveParent();
    if (parent != NULL)
    {
        // set this widget to not mouse grabbing
        parent->m_focus_has_mouse_grab = false;
        // call the mouse grab handler and emit the mouse grab on signals
        HandleMouseGrabOff();
        // recurse to the parent widget
        parent->UnGrabMouse();
    }
}

void Widget::DetachFromParent ()
{
    // i put an assert in here to make sure no one behaves badly and tries
    // to detach a top-level widget
    ASSERT1(m_parent != NULL)
    m_parent->DetachChild(this);
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

WidgetSkinHandler *Widget::GetWidgetSkinHandlerParent ()
{
    return static_cast<WidgetSkinHandler *>(m_parent);
}

void Widget::InitializeFromWidgetSkinProperties ()
{
    UpdateRenderBackground();
    SetFrameMargins(GetWidgetSkinMargins(WidgetSkin::DEFAULT_FRAME_MARGINS));
    SetContentMargins(GetWidgetSkinMargins(WidgetSkin::DEFAULT_CONTENT_MARGINS));
}

bool Widget::HandleEvent (Event const *const e)
{
    ASSERT1(e != NULL)

    // if this widget is disabled, it rejects all events
    if (!GetIsEnabled())
        return false;

    // mouse-position-related preprocessing
    if (e->GetEventType() == Event::MOUSEOVER)
    {
        EventMouseover const *mouseover_event =
            DStaticCast<EventMouseover const *>(e);
        m_last_mouse_position = mouseover_event->GetPosition();
    }

    ContainerWidget *this_container_widget = dynamic_cast<ContainerWidget *>(this);

    // the top-level widget gets an opportunity to pre-process events
    // (e.g. for generating mouseover events, or sending them to
    // modal widgets)
    if (GetIsTopLevelParent())
    {
        ASSERT1(this_container_widget != NULL)

        if (e->GetIsMouseMotionEvent())
        {
            EventMouseMotion const *mouse_motion_event =
                DStaticCast<EventMouseMotion const *>(e);

            // generate a mouseover event from the mouse motion event
            EventMouseover mouseover_event(
                mouse_motion_event->GetPosition(),
                mouse_motion_event->GetTime());
            ProcessEvent(&mouseover_event);
        }

        // get the top of the modal widget stack
        Widget *modal_widget = NULL;
        for (ContainerWidget::WidgetListReverseIterator
                 it = this_container_widget->m_modal_widget_stack.rbegin(),
                 it_end = this_container_widget->m_modal_widget_stack.rend();
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
                    DStaticCast<EventMouse const *>(e);
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
        if (this_container_widget != NULL)
            ASSERT1(this_container_widget->m_modal_widget_stack.empty())
    }

    switch (e->GetEventType())
    {
        case Event::KEYDOWN:
        case Event::KEYUP:
        case Event::KEYREPEAT:
            if (ProcessKeyEvent(DStaticCast<EventKey const *>(e)))
                return true;
            else if (this_container_widget != NULL && this_container_widget->m_focus != NULL)
                return this_container_widget->m_focus->ProcessEvent(e);
            else
                return false;

        case Event::MOUSEBUTTONDOWN:
        case Event::MOUSEBUTTONUP:
        case Event::MOUSEMOTION:
            if (PreprocessMouseEvent(DStaticCast<EventMouse const *>(e)))
                return true;
            else
                return SendMouseEventToChild(DStaticCast<EventMouse const *>(e));

        case Event::MOUSEWHEEL:
            return PreprocessMouseWheelEvent(DStaticCast<EventMouseWheel const *>(e));

        case Event::JOYAXIS:
        case Event::JOYBALL:
        case Event::JOYBUTTONDOWN:
        case Event::JOYBUTTONUP:
        case Event::JOYHAT:
            if (ProcessJoyEvent(DStaticCast<EventJoy const *>(e)))
                return true;
            else if (this_container_widget != NULL && this_container_widget->m_focus != NULL)
                return this_container_widget->m_focus->ProcessEvent(e);
            else
                return false;

        case Event::FOCUS:
            return PreprocessFocusEvent(DStaticCast<EventFocus const *>(e));

        case Event::MOUSEOVER:
            return PreprocessMouseoverEvent(DStaticCast<EventMouseover const *>(e));

        case Event::DELETE_CHILD_WIDGET:
            return ProcessDeleteChildWidgetEvent(DStaticCast<EventDeleteChildWidget const *>(e));

        case Event::ACTIVE:
        case Event::RESIZE:
        case Event::EXPOSE:
        case Event::QUIT:
        case Event::SYSWM:
            return false;

        case Event::STATE_MACHINE_INPUT:
            return ProcessStateMachineInputEvent(DStaticCast<EventStateMachineInput const *>(e));

        case Event::CUSTOM:
            return ProcessCustomEvent(DStaticCast<EventCustom const *>(e));

        default:
            ASSERT0(false && "Invalid Event::EventType")
            return false;
    }
}

bool Widget::ProcessDeleteChildWidgetEvent (EventDeleteChildWidget const *const e)
{
    ASSERT0(false && "this should never be called")
    return false;
}

void Widget::HandleChangedBackground ()
{
    UpdateRenderBackground();
}

void Widget::UpdateRenderBackground ()
{
    SetRenderBackground(GetBackground());
}

void Widget::ParentChildSizePropertiesUpdate (bool const defer_parent_update)
{
    if (!defer_parent_update && m_parent != NULL)
        m_parent->ChildSizePropertiesChanged(this);
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
// private methods
// ///////////////////////////////////////////////////////////////////////////

void Widget::FocusWidgetLine ()
{
    ASSERT1(!GetIsFocused())
    DEBUG1_CODE(ContainerWidget *this_container_widget = dynamic_cast<ContainerWidget *>(this));
    ASSERT1(this_container_widget == NULL || this_container_widget->m_focus == NULL)

    ContainerWidget *parent = GetEffectiveParent();

    // make sure to focus parent widgets first, so that the focusing
    // happens from top down
    if (parent != NULL && !parent->GetIsFocused())
        parent->FocusWidgetLine();

    // make this widget focused
    if (parent != NULL)
        parent->m_focus = this;

    // call the focus handler and emit the focus signals
    HandleFocus();
}

void Widget::UnfocusWidgetLine ()
{
    ASSERT1(GetIsFocused())

    ContainerWidget *this_container_widget = dynamic_cast<ContainerWidget *>(this);

    // make sure to unfocus child widgets first, so that the unfocusing
    // happens from bottom up
    if (this_container_widget != NULL && this_container_widget->m_focus != NULL)
        this_container_widget->m_focus->UnfocusWidgetLine();

    ContainerWidget *parent = GetEffectiveParent();
    // set the focus state
    if (parent != NULL)
        parent->m_focus = NULL;

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
        ContainerWidget *this_container_widget = dynamic_cast<ContainerWidget *>(this);

        if (this_container_widget != NULL && this_container_widget->m_mouseover_focus != NULL)
            this_container_widget->m_mouseover_focus->MouseoverOffWidgetLine();
        return true;
    }

    ContainerWidget *parent = GetEffectiveParent();

    // if this is not a top level widget, proceed normally
    if (parent != NULL)
    {
        // find the first ancestor of this widget that is mouseover-focused
        ContainerWidget *first_mouseover_ancestor = parent;
        while (first_mouseover_ancestor->GetEffectiveParent() != NULL &&
               !first_mouseover_ancestor->GetIsMouseover())
        {
            first_mouseover_ancestor = first_mouseover_ancestor->GetEffectiveParent();
        }

        // unfocus all widgets from the mouseover-focused child of the first
        // mouseover-focused ancestor down
        ASSERT1(first_mouseover_ancestor != NULL)
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
    DEBUG1_CODE(ContainerWidget *this_container_widget = dynamic_cast<ContainerWidget *>(this));
    ASSERT1(this_container_widget == NULL || this_container_widget->m_mouseover_focus == NULL)

    ContainerWidget *parent = GetEffectiveParent();

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

    // make sure to mouseover-unfocus child widgets first, so that the
    // mouseover-unfocusing happens from bottom up
    ContainerWidget *this_container_widget = dynamic_cast<ContainerWidget *>(this);
    if (this_container_widget != NULL && this_container_widget->m_mouseover_focus != NULL)
        this_container_widget->m_mouseover_focus->MouseoverOffWidgetLine();

    // set the mouseover-focus state
    ContainerWidget *parent = GetEffectiveParent();
    if (parent != NULL)
        parent->m_mouseover_focus = NULL;

    // call the mouseover-unfocus handler and emit the mouseover-focus signals
    HandleMouseoverOff();
}

bool Widget::PreprocessMouseEvent (EventMouse const *const e)
{
    ASSERT1(e != NULL)

    // now give this widget the chance to process the event
    switch (e->GetEventType())
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
            // save the event's position as the last known mouse position
            m_last_mouse_position = e->GetPosition();
            return retval;
        }

        default:
            ASSERT1(false && "Invalid Event::EventType")
            break;
    }

    // no widget accepted it, return false
    return false;
}

bool Widget::PreprocessMouseWheelEvent (EventMouseWheel const *const e)
{
    // let this widget have a chance at the event
    return ProcessMouseWheelEvent(e);
}

bool Widget::PreprocessFocusEvent (EventFocus const *const e)
{
    // hidden widgets can't be focused
    if (GetIsHidden())
        return false;

    // can't accept focus if we don't accept focus
    if (!m_accepts_focus)
        return false;

    // attempt to focus this widget (it better work)
    bool retval = Focus();
    ASSERT1(retval)
    return retval;
}

bool Widget::PreprocessMouseoverEvent (EventMouseover const *const e)
{
    // hidden widgets can't be moused over
    if (GetIsHidden())
        return false;

    // widgets that don't accept mouseover-focus return false
    if (!m_accepts_mouseover)
        return false;

    // this is the bottom-most widget which should be focused.
    MouseoverOn();
    return true;
}

bool Widget::SendMouseEventToChild (EventMouse const *const e)
{
    return false;
}

} // end of namespace Xrb
