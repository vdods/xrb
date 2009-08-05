// ///////////////////////////////////////////////////////////////////////////
// xrb_button.cpp by Victor Dods, created 2004/11/07
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_button.hpp"

#include "xrb_input_events.hpp"

namespace Xrb
{

Button::Button (
    std::string const &text,
    ContainerWidget *const parent,
    std::string const &name)
    :
    Label(text, parent, name),
    m_sender_pressed_state_changed(this),
    m_sender_pressed(this),
    m_sender_released(this)
{
    SetAlignment(Alignment2(CENTER, CENTER));
    Initialize();
}

Button::Button (
    Resource<GLTexture> const &picture,
    ContainerWidget *const parent,
    std::string const &name)
    :
    Label(picture, parent, name),
    m_sender_pressed_state_changed(this),
    m_sender_pressed(this),
    m_sender_released(this)
{
    Initialize();
}

void Button::SetIsEnabled (bool const is_enabled)
{
    Widget::SetIsEnabled(is_enabled);
    if (!IsEnabled())
        m_is_pressed = false;
    UpdateRenderBackground();
}

bool Button::ProcessMouseButtonEvent (EventMouseButton const *const e)
{
    if (e->ButtonCode() == Key::LEFTMOUSE)
    {
        if (e->IsMouseButtonDownEvent())
        {
            m_is_pressed = true;
            HandlePressed();
        }
        else if (e->IsMouseButtonUpEvent())
        {
            if (m_is_pressed)
            {
                m_is_pressed = false;
                HandleReleased();
            }
        }
        else
        {
            ASSERT1(false &&
                    "IsMouseButtonDownEvent() and IsMouseButtonUpEvent() "
                    "both failed.  Something's wacky.");
        }

        // update the render properties
        UpdateRenderBackground();
    }

    return true;
}

void Button::HandleMouseoverOn ()
{
    ASSERT1(IsMouseover());
    UpdateRenderBackground();
}

void Button::HandleMouseoverOff ()
{
    ASSERT1(!IsMouseover());
    m_is_pressed = false;
    UpdateRenderBackground();
}

void Button::HandlePressed ()
{
    m_sender_pressed_state_changed.Signal(m_is_pressed);
    m_sender_pressed.Signal();
}

void Button::HandleReleased ()
{
    m_sender_pressed_state_changed.Signal(m_is_pressed);
    m_sender_released.Signal();
}

void Button::UpdateRenderBackground ()
{
    // state priority: disabled, pressed, mouseover, default
    if (!IsEnabled())
        SetRenderBackground(
            GetWidgetSkinWidgetBackground(WidgetSkin::BUTTON_BACKGROUND));
    else if (IsPressed())
        SetRenderBackground(
            GetWidgetSkinWidgetBackground(WidgetSkin::BUTTON_PRESSED_BACKGROUND));
    else if (IsMouseover() && AcceptsMouseover())
        SetRenderBackground(
            GetWidgetSkinWidgetBackground(WidgetSkin::BUTTON_MOUSEOVER_BACKGROUND));
    else
        SetRenderBackground(
            GetWidgetSkinWidgetBackground(WidgetSkin::BUTTON_BACKGROUND));
}

void Button::HandleChangedWidgetSkinWidgetBackground (
    WidgetSkin::WidgetBackgroundType const widget_background_type)
{
    // don't bother checking what background changed, just update the shit.
    UpdateRenderBackground();
}

void Button::Initialize ()
{
    m_accepts_focus = true;
    m_accepts_mouseover = true;
    m_is_pressed = false;

    Button::UpdateRenderBackground();
}

} // end of namespace Xrb
