// ///////////////////////////////////////////////////////////////////////////
// xrb_widget.cpp by Victor Dods, created 2004/09/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_widget.hpp"

#include "xrb_containerwidget.hpp"
#include "xrb_gui_events.hpp"
#include "xrb_input_events.hpp"
#include "xrb_key.hpp"
#include "xrb_screen.hpp"
#include "xrb_widgetbackground.hpp"
#include "xrb_widgetcontext.hpp"

namespace Xrb {

// ///////////////////////////////////////////////////////////////////////////
// constructor and destructor
// ///////////////////////////////////////////////////////////////////////////

Widget::Widget (WidgetContext &context, std::string const &name)
    :
    FrameHandler(),
    EventHandler(&context.GetEventQueue()),
    SignalHandler(),
    m_context(context),
    m_receiver_set_is_enabled(&Widget::SetIsEnabled, this),
    m_receiver_enable(&Widget::Enable, this),
    m_receiver_disable(&Widget::Disable, this),
    m_receiver_set_is_hidden(&Widget::SetIsHidden, this),
    m_receiver_hide(&Widget::Hide, this),
    m_receiver_show(&Widget::Show, this)
{
//     fprintf(stderr, "Widget::Widget(%s);\n", name.c_str());

    ASSERT1(context.GetScreen().OwnerEventQueue() != NULL);

    m_accepts_focus = false;
    m_accepts_mouseover = true;
    m_name = name;
    m_is_enabled = true;
    m_is_hidden = false;
    m_screen_rect = ScreenCoordRect(0, 0, 500, 500); // arbitrary
    m_last_mouse_position = ScreenCoordVector2::ms_zero; // arbitrary
    m_color_bias = Color::ms_identity_color_bias;
    m_color_mask = Color::ms_identity_color_mask;
    m_remove_from_widget_context_upon_destruction = true;
    m_stack_priority = SP_NEUTRAL;
    m_background_style = StyleSheet::BackgroundType::TRANSPARENT;
    m_background = NULL;
    m_render_background = NULL;
    m_render_background_needs_update = true;
    m_frame_margins_style = StyleSheet::MarginsType::DEFAULT_FRAME;
    m_frame_margins_ratios = StyleSheet::ms_fallback_margins_ratios;
    m_content_margins_style = StyleSheet::MarginsType::DEFAULT_CONTENT;
    m_content_margins_ratios = StyleSheet::ms_fallback_margins_ratios;
    m_parent = NULL;

    m_context.AddWidget(*this);
}

Widget::~Widget ()
{
//     fprintf(stderr, "Widget::~Widget(%s);\n", m_name.c_str());
    ASSERT0(m_parent == NULL && "you must DetachFromParent before deleting this widget");

    DeleteAndNullify(m_background);
    
    // this if-condition is used to avoid the following method call after
    // Screen's destructor (which destroys this context).
    if (m_remove_from_widget_context_upon_destruction)
        m_context.RemoveWidget(*this);
}

// ///////////////////////////////////////////////////////////////////////////
// accessors
// ///////////////////////////////////////////////////////////////////////////

bool Widget::IsActive () const
{
    Widget const *root = this;
    ASSERT1(root != NULL); // just in case
    // this will terminate if there are no cycles, a condition which is checked for in ContainerWidget::AttachChild().
    while (root->m_parent != NULL)
        root = root->m_parent;
    return root->IsScreen();
}

bool Widget::IsFocused () const
{
    return this == &Context().GetScreen() // the screen is always focused
           ||
           (m_parent != NULL && m_parent->m_focus == this);
}

bool Widget::IsMouseover () const
{
    return this == &Context().GetScreen() // the screen always has mouseover (TODO: check for mouse leaving window)
           ||
           (m_parent != NULL && m_parent->m_mouseover_focus == this);
}

bool Widget::IsMouseGrabbed () const
{
    return this == &Context().GetScreen() // the screen always has the mouse grabbed
           ||
           (m_parent != NULL && m_parent->m_focus == this && m_parent->m_focus_has_mouse_grab);
}

bool Widget::IsModal () const
{
    return Context().GetScreen().IsAttachedAsModalChildWidget(*this);
}

WidgetBackground const *Widget::Background () const
{
    if (BackgroundStyleIsOverridden())
        return m_background;
    else
        return Context().StyleSheet_Background(m_background_style);
}

ScreenCoordMargins Widget::FrameMargins () const
{
    return Context().MarginsFromRatios(FrameMarginsRatios());
}

FloatMargins const &Widget::FrameMarginsRatios () const
{
    if (FrameMarginsStyleIsOverridden())
        return m_frame_margins_ratios;
    else
        return Context().StyleSheet_MarginsRatios(m_frame_margins_style);
}

ScreenCoordMargins Widget::ContentMargins () const
{
    return Context().MarginsFromRatios(ContentMarginsRatios());
}

FloatMargins const &Widget::ContentMarginsRatios () const
{
    if (ContentMarginsStyleIsOverridden())
        return m_content_margins_ratios;
    else
        return Context().StyleSheet_MarginsRatios(m_content_margins_style);
}

ScreenCoordVector2 Widget::AdjustedSize (ScreenCoordVector2 const &size) const
{
    ScreenCoordRect rect(size);
    AdjustFromMinSize(&rect);
    AdjustFromMaxSize(&rect);
    return rect.Size();
}

ScreenCoordRect Widget::ContentsRect () const
{
    ScreenCoordMargins grow_by(-(FrameMargins() + ContentMargins()));
    return ScreenRect().Grown(grow_by.m_bottom_left, grow_by.m_top_right);
}

// ///////////////////////////////////////////////////////////////////////////
// modifiers
// ///////////////////////////////////////////////////////////////////////////

void Widget::SetSizePropertyEnabled (SizeProperties::Property property, Uint32 component, bool value, bool defer_parent_update)
{
    ASSERT1(component <= 1);
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

void Widget::SetSizePropertyEnabled (SizeProperties::Property property, Bool2 const &value, bool defer_parent_update)
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

void Widget::SetSizeProperty (SizeProperties::Property property, Uint32 component, ScreenCoord value, bool defer_parent_update)
{
    ASSERT1(component <= 1);
    ASSERT1(value >= 0);
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

void Widget::SetSizeProperty (SizeProperties::Property property, ScreenCoordVector2 const &value, bool defer_parent_update)
{
    ASSERT1(value[Dim::X] >= 0);
    ASSERT1(value[Dim::Y] >= 0);
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

void Widget::SetSizePropertyRatio (SizeProperties::Property property, Uint32 component, Float ratio, bool defer_parent_update)
{
    ScreenCoord size_ratio_basis = Context().SizeRatioBasis();
    ScreenCoord calculated_value = ScreenCoord(size_ratio_basis * ratio);
    SetSizeProperty(property, component, calculated_value, defer_parent_update);
}

void Widget::SetSizePropertyRatios (SizeProperties::Property property, FloatVector2 const &ratios, bool defer_parent_update)
{
    ScreenCoord size_ratio_basis = Context().SizeRatioBasis();
    ScreenCoordVector2 calculated_value = (Float(size_ratio_basis) * ratios).StaticCast<ScreenCoord>();
    SetSizeProperty(property, calculated_value, defer_parent_update);
}

void Widget::SetStackPriority (StackPriority stack_priority)
{
    if (m_stack_priority != stack_priority)
    {
        StackPriority previous_stack_priority = m_stack_priority;
        m_stack_priority = stack_priority;
        if (m_parent != NULL)
            m_parent->ChildStackPriorityChanged(this, previous_stack_priority);
    }
}

void Widget::SetBackgroundStyle (std::string const &style)
{
    ASSERT1(!style.empty() && "don't specify an empty style, use SetBackground to override the value explicitly");
    if (m_background_style != style)
    {
        m_background_style = style;
        SetRenderBackgroundNeedsUpdate();
    }
}

void Widget::SetBackground (WidgetBackground const *background)
{
    m_background_style.clear(); // override style sheet by setting the style string to empty
    ASSERT1(BackgroundStyleIsOverridden());
    Delete(m_background);
    m_background = background;
    SetRenderBackgroundNeedsUpdate();
}

void Widget::SetFrameMarginsStyle (std::string const &style)
{
    ASSERT1(!style.empty() && "don't specify an empty style, use SetFrameMargins[Ratios] to override the value explicitly");
    if (m_frame_margins_style != style)
    {
        m_frame_margins_style = style;
        HandleChangedFrameMargins();
    }
}

void Widget::SetFrameMargins (ScreenCoordMargins const &frame_margins)
{
    SetFrameMarginsRatios(Context().RatiosFromMargins(frame_margins));
}

void Widget::SetFrameMarginsRatios (FloatMargins const &frame_margins_ratios)
{
    if (!m_frame_margins_style.empty() || m_frame_margins_ratios != frame_margins_ratios)
    {
        m_frame_margins_style.clear(); // override style sheet by setting the style string to empty
        ASSERT1(FrameMarginsStyleIsOverridden());
        m_frame_margins_ratios = frame_margins_ratios;
        HandleChangedFrameMargins();
    }
}

void Widget::SetContentMarginsStyle (std::string const &style)
{
    ASSERT1(!style.empty() && "don't specify an empty style, use SetContentMargins[Ratios] to override the value explicitly");
    if (m_content_margins_style != style)
    {
        m_content_margins_style = style;
        HandleChangedContentMargins();
    }
}

void Widget::SetContentMargins (ScreenCoordMargins const &content_margins)
{
    SetContentMarginsRatios(Context().RatiosFromMargins(content_margins));
}

void Widget::SetContentMarginsRatios (FloatMargins const &content_margins_ratios)
{
    if (!m_content_margins_style.empty() || m_content_margins_ratios != content_margins_ratios)
    {
        m_content_margins_style.clear(); // override style sheet by setting the style string to empty
        ASSERT1(ContentMarginsStyleIsOverridden());
        m_content_margins_ratios = content_margins_ratios;
        HandleChangedContentMargins();
    }
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

void Widget::FixWidth (ScreenCoord width)
{
    // TODO: make into atomic operation
    SetSizeProperty(SizeProperties::MIN, Dim::X, width);
    SetSizeProperty(SizeProperties::MAX, Dim::X, width);
    SetSizePropertyEnabled(SizeProperties::MIN, Dim::X, true);
    SetSizePropertyEnabled(SizeProperties::MAX, Dim::X, true);
}

void Widget::FixWidthRatio (Float width_ratio)
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

void Widget::FixHeight (ScreenCoord height)
{
    // TODO: make into atomic operation
    SetSizeProperty(SizeProperties::MIN, Dim::Y, height);
    SetSizeProperty(SizeProperties::MAX, Dim::Y, height);
    SetSizePropertyEnabled(SizeProperties::MIN, Dim::Y, true);
    SetSizePropertyEnabled(SizeProperties::MAX, Dim::Y, true);
}

void Widget::FixHeightRatio (Float height_ratio)
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

void Widget::PreDraw ()
{
    if (RenderBackgroundNeedsUpdate())
        UpdateRenderBackground();
    ASSERT1(!RenderBackgroundNeedsUpdate());
}

void Widget::Draw (RenderContext const &render_context) const
{
    ASSERT1(!RenderBackgroundNeedsUpdate());
    if (RenderBackground() != NULL) // NULL indicates no background
        RenderBackground()->Draw(render_context, ScreenRect(), FrameMargins());
}

void Widget::PostDraw ()
{
    // nothing to do (so far)
}

void Widget::MoveTo (ScreenCoordVector2 const &position)
{
    MoveBy(position - Position());
}

void Widget::MoveBy (ScreenCoordVector2 const &delta)
{
    // move this widget by the given delta
    m_screen_rect += delta;
}

ScreenCoordVector2 Widget::Resize (ScreenCoordVector2 const &size)
{
    ScreenCoordVector2 adjusted_size(m_size_properties.AdjustedSize(size));

    if (m_screen_rect.Size() != adjusted_size)
    {
        m_screen_rect.SetSize(adjusted_size);
        // range checking
        SizeRangeAdjustment(&m_screen_rect);
        // indicate to the parent that a child has changed size properties
        ParentChildSizePropertiesUpdate(false);
    }

    // return what the actual size is now
    return m_screen_rect.Size();
}

ScreenCoordVector2 Widget::ResizeByRatios (FloatVector2 const &ratios)
{
    return Resize((Float(Context().SizeRatioBasis()) * ratios).StaticCast<ScreenCoord>());
}

ScreenCoordVector2 Widget::MoveToAndResize (ScreenCoordRect const &screen_rect)
{
    MoveTo(screen_rect.BottomLeft());
    return Resize(screen_rect.Size());
}

void Widget::CenterOnWidget (Widget const &widget)
{
    MoveTo(widget.Position() + (widget.Size() - Size()) / 2);
}

bool Widget::Focus ()
{
    ASSERT1(IsActive() && "can't focus an inactive widget");
    ASSERT1(!IsHidden() && "can't focus a hidden widget");

    // if already focused, then we don't need to do anything
    if (IsFocused())
        return true;

    // there's no point in focusing if there's no parent (Screen already has focus anyway).
    if (m_parent == NULL)
        return true;

    // find the first ancestor of this widget that is focused
    ContainerWidget *first_focused_ancestor = m_parent;
    while (first_focused_ancestor->m_parent != NULL &&
           !first_focused_ancestor->IsFocused())
    {
        first_focused_ancestor = first_focused_ancestor->m_parent;
    }

    // unfocus all widgets from the focused child of the first focused
    // ancestor down
    ASSERT1(first_focused_ancestor != NULL);
    if (first_focused_ancestor->m_focus != NULL)
        first_focused_ancestor->m_focus->UnfocusWidgetLine();

    // focus all widgets from this widget on up to the first focused ancestor
    FocusWidgetLine();
    // make sure the parent's m_focus actually points to this widget
    ASSERT1(m_parent == NULL || m_parent->m_focus == this);
    // focus was taken, return true
    return true;
}

void Widget::Unfocus ()
{
    // if not active, there's nothing to do.
    if (!IsActive())
    {
        ASSERT1(!IsFocused());
        return;
    }
    
    // if not focused, then we don't need to do anything
    if (!IsFocused())
        return;

    // there's no point in unfocusing if there's no parent (Screen can't be unfocused anyway).
    if (m_parent == NULL)
        return;
    
    // unfocus this widget and all its children (from bottom up)
    UnfocusWidgetLine();
}

void Widget::GrabMouse ()
{
    ASSERT1(IsActive() && "can't GrabMouse on an inactive widget");

    // if this widget already has the mouse grabbed, don't do anything
    if (IsMouseGrabbed())
        return;

    // there's no point in mouse-grabbing if there's no parent (Screen already has mouse-grab anyway)
    if (m_parent == NULL)
        return;

    // gotta make sure that this widget has focus
    DEBUG1_CODE(bool was_focused =)
    Focus();
    ASSERT1(was_focused);
    ASSERT1(IsFocused());

    // set the mouse grab on this and up through the parent widgets
    m_parent->GrabMouse();
    // set this widget to mouse grabbing
    m_parent->m_focus_has_mouse_grab = true;
    // make sure the parent's m_focus actually points to this widget
    ASSERT1(m_parent->m_focus == this);
    // call the mouse grab handler and emit the mouse grab on signals
    HandleMouseGrabOn();
}

void Widget::UnGrabMouse ()
{
    // if this widget already doesn't have the mouse grabbed, don't do anything
    if (!IsMouseGrabbed())
        return;

    // there's no point in un-mouse-grabbing if there's no parent (Screen can't un-mouse-grab anyway)
    if (m_parent == NULL)
        return;
    
    // unset the mouse grab on this and up through the parent widgets
    m_parent->m_focus_has_mouse_grab = false;
    // call the mouse grab handler and emit the mouse grab on signals
    HandleMouseGrabOff();
    // recurse to the parent widget
    m_parent->UnGrabMouse();
}

void Widget::DetachFromParent ()
{
    if (m_parent == NULL)
    {
        ASSERT1(false && "you can't detach a root widget");
        return;
    }
    m_parent->DetachChild(this);
}

void Widget::SetIsEnabled (bool is_enabled)
{
    if (!is_enabled)
    {
        ASSERT0(!IsModal() && "You can't disable a modal widget");
        ASSERT0(this != &Context().GetScreen() && "You can't disable the Screen");
    }

    if (m_is_enabled != is_enabled)
    {
        m_is_enabled = is_enabled;
        // if the widget is disabled, unfocus it and disable outgoing signalsenders
        if (!m_is_enabled)
        {
            Unfocus();
            m_enabled_sender_blocking_state = IsBlockingSenders();
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
//     ASSERT0(m_parent != NULL && "You can't show or hide a top-level widget!");

    // if the widget is being hidden, then it should be
    // unfocused and mouseover-off'ed
    if (!m_is_hidden)
    {
        Unfocus();
        MouseoverOff();
    }

    m_is_hidden = !m_is_hidden;

    if (!IsModal())
        ParentChildSizePropertiesUpdate(false);
}

void Widget::SetIsHidden (bool is_hidden)
{
    if (m_is_hidden != is_hidden)
        ToggleIsHidden();
}

// ///////////////////////////////////////////////////////////////////////////
// protected methods
// ///////////////////////////////////////////////////////////////////////////

bool Widget::IsScreen () const
{
    return this == &Context().GetScreen();
}

void Widget::HandleChangedStyleSheet ()
{
    SetRenderBackgroundNeedsUpdate();
    HandleChangedFrameMargins();
    HandleChangedContentMargins();
}

bool Widget::HandleEvent (Event const *e)
{
    ASSERT1(e != NULL);

    // if this widget is disabled, it rejects all events
    if (!IsEnabled())
        return false;

    switch (e->GetEventType())
    {
        case Event::KEYDOWN:
        case Event::KEYUP:
        case Event::KEYREPEAT:
            return InternalProcessKeyEvent(DStaticCast<EventKey const *>(e));

        case Event::MOUSEBUTTONDOWN:
        case Event::MOUSEBUTTONUP:
        case Event::MOUSEWHEEL:
        case Event::MOUSEMOTION:
            return InternalProcessMouseEvent(DStaticCast<EventMouse const *>(e));

        case Event::PINCHBEGIN:
        case Event::PINCHEND:
        case Event::PINCHMOTION:
            return InternalProcessPinchEvent(DStaticCast<EventPinch const *>(e));

        case Event::ROTATEBEGIN:
        case Event::ROTATEEND:
        case Event::ROTATEMOTION:
            return InternalProcessRotateEvent(DStaticCast<EventRotate const *>(e));

        case Event::JOYAXIS:
        case Event::JOYBALL:
        case Event::JOYBUTTONDOWN:
        case Event::JOYBUTTONUP:
        case Event::JOYHAT:
            return InternalProcessJoyEvent(DStaticCast<EventJoy const *>(e));

        case Event::FOCUS:
            return InternalProcessFocusEvent(DStaticCast<EventFocus const *>(e));

        case Event::MOUSEOVER:
            return InternalProcessMouseoverEvent(DStaticCast<EventMouseover const *>(e));

        case Event::DETACH_AND_DELETE_CHILD_WIDGET:
            return ProcessDetachAndDeleteChildWidgetEvent(DStaticCast<EventDetachAndDeleteChildWidget const *>(e));

        case Event::QUIT:
            return false;

        case Event::STATE_MACHINE_INPUT:
            return ProcessStateMachineInputEvent(DStaticCast<EventStateMachineInput const *>(e));

        case Event::CUSTOM:
            return ProcessCustomEvent(DStaticCast<EventCustom const *>(e));

        default:
            ASSERT0(false && "Invalid Event::EventType");
            return false;
    }
}

bool Widget::ProcessDetachAndDeleteChildWidgetEvent (EventDetachAndDeleteChildWidget const *e)
{
    ASSERT0(false && "this should never be called");
    return false;
}

void Widget::HandleAttachedToParent ()
{
    // the widget was necessarily inactive before being attached to its parent.
    // NOTE: this is not the only place a widget becomes active (if it has a parent
    // and the parent becomes active, then it also becomes active.  see
    // ContainerWidget::AttachChild).
    if (IsActive())
        HandleActivate();
}

void Widget::HandleAboutToDetachFromParent ()
{
    // the widget will necessarily become inactive after being detached from its parent.
    // NOTE: this is not the only place a widget becomes inactive (if it has a parent
    // and the parent becomes inactive, then it also becomes inactive.  see
    // ContainerWidget::DetachChild.
    if (IsActive())
        HandleDeactivate();
}

void Widget::UpdateRenderBackground ()
{
    m_render_background_needs_update = false;
    SetRenderBackground(Background());
}

void Widget::ParentChildSizePropertiesUpdate (bool defer_parent_update)
{
    if (!defer_parent_update && m_parent != NULL/* && !IsHidden() && !IsModal()*/) // TODO: enable and test this
        m_parent->ChildSizePropertiesChanged(this);
}

bool Widget::AdjustFromMinSize (ScreenCoordRect *screen_rect) const
{
    // to keep track of if we need to call Resize()
    bool adjusted = false;
    // check it against the minimum size and if the size needs to be
    // adjusted, do so and flag adjusted
    for (Uint32 i = Dim::X; i <= Dim::Y; ++i)
    {
        if (m_size_properties.m_min_size_enabled[i] &&
            screen_rect->Size()[i] < m_size_properties.m_min_size[i])
        {
            screen_rect->SetSize(i, m_size_properties.m_min_size[i]);
            adjusted = true;
        }
    }
    return adjusted;
}

bool Widget::AdjustFromMaxSize (ScreenCoordRect *screen_rect) const
{
    // to keep track of if we need to call Resize()
    bool adjusted = false;
    // check it against the maximum size and if the X size needs to be
    // adjusted, do so and flag adjusted
    for (Uint32 i = Dim::X; i <= Dim::Y; ++i)
    {
        if (m_size_properties.m_max_size_enabled[i] &&
            screen_rect->Size()[i] > m_size_properties.m_max_size[i])
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
    m_size_properties.m_min_size = temp.Size();
    // make sure that min/max have the correct relationship
    temp = m_size_properties.m_max_size;
    AdjustFromMinSize(&temp);
    m_size_properties.m_max_size = temp.Size();
}

void Widget::MaxSizeUpdated ()
{
    ScreenCoordRect temp;
    // make sure the size is non-negative
    temp = m_size_properties.m_max_size;
    SizeRangeAdjustment(&temp);
    m_size_properties.m_max_size = temp.Size();
    // make sure that min/max have the correct relationship
    temp = m_size_properties.m_min_size;
    AdjustFromMaxSize(&temp);
    m_size_properties.m_min_size = temp.Size();
}

void Widget::SizeRangeAdjustment (ScreenCoordRect *rect) const
{
    // make sure the size is non-negative
    if (rect->Size()[Dim::X] < 0)
        rect->SetSize(Dim::X, 0);
    if (rect->Size()[Dim::Y] < 0)
        rect->SetSize(Dim::Y, 0);
}

// ///////////////////////////////////////////////////////////////////////////
// private methods
// ///////////////////////////////////////////////////////////////////////////

void Widget::FocusWidgetLine ()
{
    ASSERT1(!IsFocused());
    DEBUG1_CODE(ContainerWidget *this_container_widget = dynamic_cast<ContainerWidget *>(this));
    ASSERT1(this_container_widget == NULL || this_container_widget->m_focus == NULL);

    // make sure to focus parent widgets first, so that the focusing
    // happens from top down
    if (m_parent != NULL && !m_parent->IsFocused())
        m_parent->FocusWidgetLine();

    // make this widget focused
    if (m_parent != NULL)
        m_parent->m_focus = this;

    // call the focus handler and emit the focus signals
    HandleFocus();
}

void Widget::UnfocusWidgetLine ()
{
    ASSERT1(IsFocused());

    ContainerWidget *this_container_widget = dynamic_cast<ContainerWidget *>(this);

    // make sure to unfocus child widgets first, so that the unfocusing
    // happens from bottom up
    if (this_container_widget != NULL && this_container_widget->m_focus != NULL)
        this_container_widget->m_focus->UnfocusWidgetLine();

    // set the focus state
    if (m_parent != NULL)
        m_parent->m_focus = NULL;

    // call the unfocus handler and emit the focus signals
    HandleUnfocus();
}

bool Widget::MouseoverOn ()
{
    // an inactive widget can't accept mouseover-focus yet.
    if (!IsActive())
        return false;
    
    // if this widget doesn't even accept mouseover-focus, then return
    if (!m_accepts_mouseover)
        return false;

    // if this widget is hidden, then it can't be moused-over
    if (IsHidden())
        return false;

    // if already mouseover-focused, then make sure there are no child
    // widgets that have mouseover, and then return true.
    if (IsMouseover())
    {
        ContainerWidget *this_container_widget = dynamic_cast<ContainerWidget *>(this);

        if (this_container_widget != NULL && this_container_widget->m_mouseover_focus != NULL)
            this_container_widget->m_mouseover_focus->MouseoverOffWidgetLine();
        return true;
    }

    // if this is not a top level widget, proceed normally
    if (m_parent != NULL)
    {
        // find the first ancestor of this widget that is mouseover-focused
        ContainerWidget *first_mouseover_ancestor = m_parent;
        while (first_mouseover_ancestor->m_parent != NULL && !first_mouseover_ancestor->IsMouseover())
            first_mouseover_ancestor = first_mouseover_ancestor->m_parent;

        // unfocus all widgets from the mouseover-focused child of the first mouseover-focused ancestor down
        ASSERT1(first_mouseover_ancestor != NULL);
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
    if (IsScreen())
    {
        MouseoverOffWidgetLine();
        return;
    }

    if (!m_accepts_mouseover)
        ASSERT1(!IsMouseover());

    if (IsHidden())
        ASSERT1(!IsMouseover());

    if (IsMouseover())
        MouseoverOffWidgetLine();
}

void Widget::MouseoverOnWidgetLine ()
{
    ASSERT1(!IsMouseover());
    DEBUG1_CODE(ContainerWidget *this_container_widget = dynamic_cast<ContainerWidget *>(this));
    ASSERT1(this_container_widget == NULL || this_container_widget->m_mouseover_focus == NULL);

    // make sure to mouseover-focus parent widgets first, so that
    // the mouseover-focusing happens from top down
    if (m_parent != NULL && !m_parent->IsMouseover())
        m_parent->MouseoverOnWidgetLine();

    // make this widget mouseover-focus state
    if (m_parent != NULL)
        m_parent->m_mouseover_focus = this;

    // call the mouseover-unfocus handler and emit the mouseover-focus signals
    HandleMouseoverOn();
}

void Widget::MouseoverOffWidgetLine ()
{
    ASSERT1(IsMouseover());

    // make sure to mouseover-unfocus child widgets first, so that the
    // mouseover-unfocusing happens from bottom up
    ContainerWidget *this_container_widget = dynamic_cast<ContainerWidget *>(this);
    if (this_container_widget != NULL && this_container_widget->m_mouseover_focus != NULL)
        this_container_widget->m_mouseover_focus->MouseoverOffWidgetLine();

    // set the mouseover-focus state
    if (m_parent != NULL)
        m_parent->m_mouseover_focus = NULL;

    // call the mouseover-unfocus handler and emit the mouseover-focus signals
    HandleMouseoverOff();
}

bool Widget::InternalProcessKeyEvent (EventKey const *e)
{
    ASSERT1(e != NULL);
    return ProcessKeyEvent(DStaticCast<EventKey const *>(e));
}

bool Widget::InternalProcessMouseEvent (EventMouse const *e)
{
    ASSERT1(e != NULL);

    // if it was a mouse event, record the mouse position over this widget
    if (e->IsMouseEvent())
        m_last_mouse_position = DStaticCast<EventMouse const *>(e)->Position();

    // give this widget the chance to process the event
    switch (e->GetEventType())
    {
        case Event::MOUSEBUTTONUP:
        case Event::MOUSEBUTTONDOWN:
            // let this widget have an opportunity at the event
            if (ProcessMouseButtonEvent(DStaticCast<EventMouseButton const *>(e)))
                return true;
            break;

        case Event::MOUSEWHEEL:
            if (ProcessMouseWheelEvent(DStaticCast<EventMouseWheel const *>(e)))
                return true;
            break;

        case Event::MOUSEMOTION:
            // let this widget have an opportunity at the event
            if (ProcessMouseMotionEvent(DStaticCast<EventMouseMotion const *>(e)))
                return true;
            break;

        default:
            ASSERT1(false && "Invalid Event::EventType");
            break;
    }

    return false;
}

bool Widget::InternalProcessPinchEvent (EventPinch const *e)
{
    ASSERT1(e != NULL);

    // give this widget the chance to process the event
    switch (e->GetEventType())
    {
        case Event::PINCHBEGIN:
            if (ProcessPinchBeginEvent(DStaticCast<EventPinchBegin const *>(e)))
                return true;
            break;

        case Event::PINCHEND:
            if (ProcessPinchEndEvent(DStaticCast<EventPinchEnd const *>(e)))
                return true;
            break;

        case Event::PINCHMOTION:
            if (ProcessPinchMotionEvent(DStaticCast<EventPinchMotion const *>(e)))
                return true;
            break;

        default:
            ASSERT1(false && "Invalid Event::EventType");
            break;
    }

    return false;
}

bool Widget::InternalProcessRotateEvent (EventRotate const *e)
{
    ASSERT1(e != NULL);

    // give this widget the chance to process the event
    switch (e->GetEventType())
    {
        case Event::ROTATEBEGIN:
            if (ProcessRotateBeginEvent(DStaticCast<EventRotateBegin const *>(e)))
                return true;
            break;

        case Event::ROTATEEND:
            if (ProcessRotateEndEvent(DStaticCast<EventRotateEnd const *>(e)))
                return true;
            break;

        case Event::ROTATEMOTION:
            if (ProcessRotateMotionEvent(DStaticCast<EventRotateMotion const *>(e)))
                return true;
            break;

        default:
            ASSERT1(false && "Invalid Event::EventType");
            break;
    }

    return false;
}

bool Widget::InternalProcessJoyEvent (EventJoy const *e)
{
    ASSERT1(e != NULL);
    return ProcessJoyEvent(DStaticCast<EventJoy const *>(e));
}

bool Widget::InternalProcessFocusEvent (EventFocus const *e)
{
    // hidden widgets can't be focused
    if (IsHidden())
        return false;

    // can't accept focus if we don't accept focus
    if (!m_accepts_focus)
        return false;

    // attempt to focus this widget (it better work)
    bool retval = Focus();
    ASSERT1(retval);
    return retval;
}

bool Widget::InternalProcessMouseoverEvent (EventMouseover const *e)
{
    // hidden widgets can't be moused over
    if (IsHidden())
        return false;

    // widgets that don't accept mouseover-focus return false
    if (!m_accepts_mouseover)
        return false;

    // this is the bottom-most widget which should be mouseovered
    return MouseoverOn();
}

} // end of namespace Xrb
