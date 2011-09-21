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
#include "xrb_widgetcontext.hpp"

namespace Xrb {

CheckBox::CheckBox (WidgetContext &context, std::string const &name)
    :
    Button(Resource<GlTexture>(), context, name),
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
            Context().StyleSheet_FontPixelHeight(StyleSheet::FontType::DEFAULT),
            Context().StyleSheet_FontPixelHeight(StyleSheet::FontType::DEFAULT)));
    SetFrameMarginsStyle(StyleSheet::MarginsType::CHECK_BOX_FRAME);

    SetIdleBackgroundStyle(StyleSheet::BackgroundType::CHECK_BOX_UNCHECKED);
    SetMouseoverBackgroundStyle(StyleSheet::BackgroundType::CHECK_BOX_MOUSEOVER);
    SetPressedBackgroundStyle(StyleSheet::BackgroundType::CHECK_BOX_PRESSED);
    m_checked_background_style = StyleSheet::BackgroundType::CHECK_BOX_CHECKED;
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
    SetRenderBackgroundNeedsUpdate();

    m_sender_checked_state_changed.Signal(m_is_checked);
    if (m_is_checked)
        m_sender_checked.Signal();
    else
        m_sender_unchecked.Signal();
}

void CheckBox::HandleChangedStyleSheet ()
{
    Button::HandleChangedStyleSheet();
    FixSize(
        ScreenCoordVector2(
            Context().StyleSheet_FontPixelHeight(StyleSheet::FontType::DEFAULT),
            Context().StyleSheet_FontPixelHeight(StyleSheet::FontType::DEFAULT)));
}

void CheckBox::UpdateRenderBackground ()
{
    Button::UpdateRenderBackground();
    if (IsChecked() && !IsPressed())
        SetRenderBackground(Context().StyleSheet_Background(m_checked_background_style));
}

void CheckBox::HandleReleased ()
{
    Button::HandleReleased();
    ToggleIsChecked();
}

} // end of namespace Xrb
