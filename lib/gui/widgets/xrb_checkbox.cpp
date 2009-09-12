// ///////////////////////////////////////////////////////////////////////////
// xrb_checkbox.cpp by Victor Dods, created 2005/02/04
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_checkbox.hpp"

#include "xrb_input_events.hpp"

namespace Xrb
{

CheckBox::CheckBox (
    ContainerWidget *const parent,
    std::string const &name)
    :
    Button(Resource<GlTexture>(), parent, name),
    m_sender_checked_state_changed(this),
    m_sender_checked(this),
    m_sender_unchecked(this),
    m_receiver_set_is_checked(&CheckBox::SetIsChecked, this),
    m_receiver_check(&CheckBox::Check, this),
    m_receiver_uncheck(&CheckBox::Uncheck, this),
    m_receiver_toggle_is_checked(&CheckBox::ToggleIsChecked, this)
{
    m_is_checked = false;

    FixSize(
        ScreenCoordVector2(
            WidgetSkinFont(WidgetSkin::DEFAULT_FONT)->PixelHeight(),
            WidgetSkinFont(WidgetSkin::DEFAULT_FONT)->PixelHeight()));

    CheckBox::UpdateRenderBackground();
    CheckBox::UpdateRenderPicture();
}

void CheckBox::SetIsEnabled (bool const is_enabled)
{
    Button::SetIsEnabled(is_enabled);
}

void CheckBox::SetIsChecked (bool const is_checked)
{
    if (m_is_checked != is_checked)
        ToggleIsChecked();
}

void CheckBox::ToggleIsChecked ()
{
    m_is_checked = !m_is_checked;
    HandleIsCheckedChanged();
    UpdateRenderPicture();

    m_sender_checked_state_changed.Signal(m_is_checked);
    if (m_is_checked)
        m_sender_checked.Signal();
    else
        m_sender_unchecked.Signal();
}

bool CheckBox::ProcessMouseButtonEvent (EventMouseButton const *const e)
{
    // first call Button's handler
    Button::ProcessMouseButtonEvent(e);

    // if the left mouse button was clicked and released, toggle m_is_checked
    if (e->ButtonCode() == Key::LEFTMOUSE && e->IsMouseButtonUpEvent())
        ToggleIsChecked();

    return true;
}

void CheckBox::UpdateRenderBackground ()
{
    SetRenderBackground(
        WidgetSkinWidgetBackground(WidgetSkin::CHECK_BOX_BACKGROUND));
}

void CheckBox::UpdateRenderPicture ()
{
    if (IsChecked())
        SetRenderPicture(WidgetSkinTexture(WidgetSkin::CHECK_BOX_CHECK_TEXTURE));
    else
        SetRenderPicture(Resource<GlTexture>());
}

void CheckBox::HandleChangedWidgetSkinWidgetBackground (
    WidgetSkin::WidgetBackgroundType const widget_background_type)
{
    if (widget_background_type == WidgetSkin::CHECK_BOX_BACKGROUND)
        UpdateRenderBackground();
}

void CheckBox::HandleChangedWidgetSkinTexture (
    WidgetSkin::TextureType const texture_type)
{
    if (texture_type == WidgetSkin::CHECK_BOX_CHECK_TEXTURE)
        UpdateRenderPicture();
}

} // end of namespace Xrb
