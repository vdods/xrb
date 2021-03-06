// ///////////////////////////////////////////////////////////////////////////
// xrb_keyselectorbutton.cpp by Victor Dods, created 2006/08/19
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_keyselectorbutton.hpp"

#include "xrb_inputstate.hpp"
#include "xrb_input_events.hpp"
#include "xrb_label.hpp"
#include "xrb_layout.hpp"
#include "xrb_screen.hpp"
#include "xrb_widgetcontext.hpp"

namespace Xrb {

KeySelectorButton::KeySelectorButton (std::string const &input_action_name, Key::Code key_code, WidgetContext &context, std::string const &name)
    :
    Button(Singleton::InputState().KeyName(key_code), context, name),
    m_input_action_name(input_action_name),
    m_internal_receiver_dialog_returned(&KeySelectorButton::DialogReturned, this)
{
    m_key_code = key_code;
    m_key_selector_dialog = NULL;
}

void KeySelectorButton::SetKeyCode (Key::Code key_code)
{
    if (Singleton::InputState().IsValidKeyCode(key_code))
        m_key_code = key_code;
    else
        m_key_code = Key::INVALID;
    SetText(Singleton::InputState().KeyName(m_key_code));
}

void KeySelectorButton::HandleReleased ()
{
    ASSERT1(m_key_selector_dialog == NULL);
    m_key_selector_dialog = new KeySelectorDialog("Press new key/button for \"" + m_input_action_name + "\"", Context());
    m_key_selector_dialog->CenterOnWidget(Context().GetScreen());
    Context().GetScreen().AttachAsModalChildWidget(*m_key_selector_dialog);
    
    SignalHandler::Connect1(
        m_key_selector_dialog->SenderDialogReturned(),
        &m_internal_receiver_dialog_returned);
}

void KeySelectorButton::DialogReturned (Dialog::ButtonID button_id)
{
    ASSERT1(m_key_selector_dialog != NULL);
    if (button_id == Dialog::ID_OK)
        SetKeyCode(m_key_selector_dialog->KeyCode());
    m_key_selector_dialog = NULL;
}

// ///////////////////////////////////////////////////////////////////////////
// KeySelectorButton::KeySelectorDialog
// ///////////////////////////////////////////////////////////////////////////

KeySelectorButton::KeySelectorDialog::KeySelectorDialog (std::string const &message, WidgetContext &context, std::string const &name)
    :
    Dialog(DT_CANCEL, context, name)
{
    // this is so clicking on the CANCEL button eats the mouse event and
    // ProcessMouseButtonEvent is not called
    m_children_get_input_events_first = true;

    m_key_code = Key::INVALID;

    Label *l;
    l = new Label(message, Context());
    l->SetIsHeightFixedToTextHeight(true);
    DialogLayout()->AttachChild(l);
}

void KeySelectorButton::KeySelectorDialog::HandleActivate ()
{
    Dialog::HandleActivate();
    // this is so if you want to set a mouse button/wheel as the key code,
    // the mouse cursor doesn't have to be over this dialog to register it.
    GrabMouse();
}

bool KeySelectorButton::KeySelectorDialog::ProcessKeyEvent (EventKey const &e)
{
    ASSERT1(Singleton::InputState().IsValidKeyCode(e.KeyCode()));
    m_key_code = e.KeyCode();
    OKButtonActivated();
    return true;
}

bool KeySelectorButton::KeySelectorDialog::ProcessMouseButtonEvent (EventMouseButton const &e)
{
    ASSERT1(Singleton::InputState().IsValidKeyCode(e.ButtonCode()));
    m_key_code = e.ButtonCode();
    OKButtonActivated();
    return true;
}

bool KeySelectorButton::KeySelectorDialog::ProcessMouseWheelEvent (EventMouseWheel const &e)
{
    ASSERT1(Singleton::InputState().IsValidKeyCode(e.ButtonCode()));
    m_key_code = e.ButtonCode();
    OKButtonActivated();
    return true;
}

} // end of namespace Xrb

