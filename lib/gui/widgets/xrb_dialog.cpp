// ///////////////////////////////////////////////////////////////////////////
// xrb_dialog.cpp by Victor Dods, created 2005/02/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_dialog.hpp"

#include "xrb_button.hpp"
#include "xrb_input_events.hpp"
#include "xrb_layout.hpp"
#include "xrb_spacerwidget.hpp"

namespace Xrb
{

Dialog::Dialog (
    DialogType const dialog_type,
    ContainerWidget *const parent,
    std::string const &name)
    :
    ModalWidget(parent, name),
    m_sender_dialog_returned(this),
    m_sender_dialog_returned_ok(this),
    m_sender_dialog_returned_cancel(this),
    m_internal_receiver_ok_button_activated(&Dialog::OKButtonActivated, this),
    m_internal_receiver_cancel_button_activated(&Dialog::CancelButtonActivated, this)
{
    ASSERT1(parent != NULL && "The top-level widget must be a Screen, not a Dialog");

    Dialog::UpdateRenderBackground();

    // the main layout which contains the button layout and to which the user
    // can add more controls
    m_dialog_layout = new Layout(VERTICAL, this, "dialog layout");
    m_dialog_layout->SetIsUsingZeroedFrameMargins(false);

    // the horizontal layout for the OK and cancel, etc. buttons
    m_button_layout = new Layout(HORIZONTAL, m_dialog_layout, "button layout");
    m_button_layout->SetIsUsingZeroedFrameMargins(false);
    // make the buttons stay at the bottom of the dialog box
    m_button_layout->SetStackPriority(SP_STAY_ON_TOP);

    // add the OK, cancel, etc. buttons
    Uint32 added_button_count = 0;
    m_dialog_type = dialog_type;
    m_ok_button = 0;
    m_cancel_button = 0;
    if (GetHasOKButton())
    {
        new SpacerWidget(m_button_layout);
        m_ok_button = new Button("OK", m_button_layout, "OK button");
        m_ok_button->SetIsHeightFixedToTextHeight(true);
        ++added_button_count;

        SignalHandler::Connect0(
            m_ok_button->SenderReleased(),
            &m_internal_receiver_ok_button_activated);
    }
    if (GetHasCancelButton())
    {
        new SpacerWidget(m_button_layout);
        m_cancel_button = new Button("Cancel", m_button_layout, "Cancel button");
        m_cancel_button->SetIsHeightFixedToTextHeight(true);
        ++added_button_count;

        SignalHandler::Connect0(
            m_cancel_button->SenderReleased(),
            &m_internal_receiver_cancel_button_activated);
    }
    new SpacerWidget(m_button_layout);
    ASSERT1(added_button_count > 0 && "No buttons were added to the Dialog");

    SetMainWidget(m_dialog_layout);
}

bool Dialog::GetHasButton (ButtonID const button_id) const
{
    switch (button_id) {
        case ID_OK:
            return GetHasOKButton();

        case ID_CANCEL:
            return GetHasCancelButton();

        default:
            ASSERT1(false && "Invalid button ID");
            return false;
    }
}

bool Dialog::ProcessKeyEvent (EventKey const *const e)
{
    if (e->GetIsKeyDownEvent())
    {
        switch (e->GetKeyCode())
        {
            case Key::RETURN:
                if (GetHasOKButton() && e->GetIsEitherControlKeyPressed())
                {
                    OKButtonActivated();
                    return true;
                }
                else
                    return false;

            case Key::ESCAPE:
                if (GetHasCancelButton())
                    CancelButtonActivated();
                return true;

            default:
                return false;
        }
    }

    return false;
}

void Dialog::OKButtonActivated ()
{
    Shutdown();
    m_sender_dialog_returned.Signal(ID_OK);
    m_sender_dialog_returned_ok.Signal();
}

void Dialog::CancelButtonActivated ()
{
    Shutdown();
    m_sender_dialog_returned.Signal(ID_CANCEL);
    m_sender_dialog_returned_cancel.Signal();
}

} // end of namespace Xrb
