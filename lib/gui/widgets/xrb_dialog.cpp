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

namespace Xrb {

Dialog::Dialog (DialogType dialog_type, WidgetContext &context, std::string const &name)
    :
    ModalWidget(context, name),
    m_sender_dialog_returned(this),
    m_sender_dialog_returned_ok(this),
    m_sender_dialog_returned_cancel(this),
    m_internal_receiver_ok_button_activated(&Dialog::OKButtonActivated, this),
    m_internal_receiver_cancel_button_activated(&Dialog::CancelButtonActivated, this)
{
    // the main layout which contains the button layout and to which the user
    // can add more controls
    m_dialog_layout = new Layout(VERTICAL, Context(), "dialog layout");
    m_dialog_layout->SetIsUsingZeroedFrameMargins(false);
    this->AttachChild(m_dialog_layout);

    // the horizontal layout for the OK and cancel, etc. buttons
    m_button_layout = new Layout(HORIZONTAL, Context(), "button layout");
    m_button_layout->SetIsUsingZeroedFrameMargins(false);
    // make the buttons stay at the bottom of the dialog box
    m_button_layout->SetStackPriority(SP_STAY_ON_TOP);
    m_dialog_layout->AttachChild(m_button_layout);

    // add the OK, cancel, etc. buttons
    Uint32 added_button_count = 0;
    m_dialog_type = dialog_type;
    m_ok_button = 0;
    m_cancel_button = 0;
    if (HasOKButton())
    {
        m_button_layout->AttachChild(new SpacerWidget(context));
        m_ok_button = new Button("OK", Context(), "OK button");
        m_ok_button->SetIsHeightFixedToTextHeight(true);
        m_button_layout->AttachChild(m_ok_button);
        ++added_button_count;

        SignalHandler::Connect0(
            m_ok_button->SenderReleased(),
            &m_internal_receiver_ok_button_activated);
    }
    if (HasCancelButton())
    {
        m_button_layout->AttachChild(new SpacerWidget(context));
        m_cancel_button = new Button("Cancel", Context(), "Cancel button");
        m_cancel_button->SetIsHeightFixedToTextHeight(true);
        m_button_layout->AttachChild(m_cancel_button);
        ++added_button_count;

        SignalHandler::Connect0(
            m_cancel_button->SenderReleased(),
            &m_internal_receiver_cancel_button_activated);
    }
    m_button_layout->AttachChild(new SpacerWidget(context));
    ASSERT1(added_button_count > 0 && "No buttons were added to the Dialog");

    SetMainWidget(m_dialog_layout);
}

bool Dialog::HasButton (ButtonID button_id) const
{
    switch (button_id) {
        case ID_OK:
            return HasOKButton();

        case ID_CANCEL:
            return HasCancelButton();

        default:
            ASSERT1(false && "Invalid button ID");
            return false;
    }
}

bool Dialog::ProcessKeyEvent (EventKey const &e)
{
    if (e.IsKeyDownEvent())
    {
        switch (e.KeyCode())
        {
            case Key::RETURN:
                if (HasOKButton() && e.IsEitherControlKeyPressed())
                {
                    OKButtonActivated();
                    return true;
                }
                else
                    return false;

            case Key::ESCAPE:
                if (HasCancelButton())
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
