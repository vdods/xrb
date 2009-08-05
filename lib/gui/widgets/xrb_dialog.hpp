// ///////////////////////////////////////////////////////////////////////////
// xrb_dialog.hpp by Victor Dods, created 2005/02/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_DIALOG_HPP_)
#define _XRB_DIALOG_HPP_

#include "xrb.hpp"

#include "xrb_modalwidget.hpp"

namespace Xrb
{

class Layout;
class Button;

/** Child widgets are added to a Layout inside of the Dialog, not to the
  * Dialog itself.  The button controls are inside the Layout as well, but
  * they are set to use SP_STAY_ON_TOP so that they will remain at the
  * end of the Layout.
  * @brief A modal widget with a set of buttons at the bottom of a built-in
  *        vertical layout.
  */
class Dialog : public ModalWidget
{
private:

    enum
    {
        BUTTON_OK = 1,
        BUTTON_CANCEL = BUTTON_OK << 1
    };

public:

    /** These are used to specify which buttons should be added to the dialog.
      * @brief All the possible button combinations.
      */
    enum DialogType
    {
        DT_OK = BUTTON_OK,
        DT_OK_CANCEL = BUTTON_OK|BUTTON_CANCEL,
        DT_CANCEL = BUTTON_CANCEL
    }; // end of enum DialogType

    /** Used by m_sender_dialog_returned.
      * @brief Used to signal when a button has been pressed.
      */
    enum ButtonID
    {
        ID_OK = 0,
        ID_CANCEL,
    }; // end of enum ButtonID

    /**
      * @brief Constructs a Dialog using the given button combination.
      * @param dialog_type Indicates which buttons to add to the dialog.
      * @param parent The parent of this widget (see Widget::Widget).
      * @param name The name of this widget (see Widget::Widget).
      */
    Dialog (
        DialogType const dialog_type,
        ContainerWidget *const parent,
        std::string const &name = "Dialog");
    /** Doesn't do anything special.  The child widgets are deleted
      * by Widget's destructor.
      * @brief Destructor.
      */
    virtual ~Dialog () { }

    /** When adding widgets to a Dialog, this Layout should be used as the
      * parent widget, not the dialog itself.
      * @brief Returns the Layout for the dialog's controls.
      */
    inline Layout *DialogLayout () const { return m_dialog_layout; }
    /** @brief Returns true iff the specified button exists in this dialog.
      * @param button_id The button type to check for existence.
      */
    bool GetHasButton (ButtonID const button_id) const;
    /** @brief Explicitly checks for an OK button.
      */
    inline bool GetHasOKButton () const
    {
        return (m_dialog_type & BUTTON_OK) != 0;
    }
    /** @brief Explicitly checks for a Cancel button.
      */
    inline bool GetHasCancelButton () const
    {
        return (m_dialog_type & BUTTON_CANCEL) != 0;
    }

    /** @brief Accessor for the SignalSender which signals the ID of a button
      *        when a button has been pressed.
      */
    inline SignalSender1<ButtonID> const *SenderDialogReturned () { return &m_sender_dialog_returned; }
    /** @brief Accessor for the SignalSender which signals that the OK
      *        button has been pressed.
      */
    inline SignalSender0 const *SenderDialogReturnedOK ()
    {
        ASSERT1(GetHasOKButton());
        return &m_sender_dialog_returned_ok;
    }
    /** @brief Accessor for the SignalSender which signals that the Cancel
      *        button has been pressed.
      */
    inline SignalSender0 const *SenderDialogReturnedCancel ()
    {
        ASSERT1(GetHasCancelButton());
        return &m_sender_dialog_returned_cancel;
    }

protected:

    /** Ctrl+Enter is a shortcut for OK, escape is a shortcut for Cancel.
      * @brief Handles the default dialog keypresses.
      */
    virtual bool ProcessKeyEvent (EventKey const *e);

    /** Shuts down the dialog, and emits the appropriate signals.
      * This function can be called directly to simulate the OK button
      * being pressed.
      * @brief Called when the OK button is pressed.
      */
    virtual void OKButtonActivated ();
    /** Shuts down the dialog, and emits the appropriate signals.
      * This function can be called directly to simulate the Cancel button
      * being pressed.
      * @brief Called when the Cancel button is pressed.
      */
    virtual void CancelButtonActivated ();

private:

    DialogType m_dialog_type;

    Layout *m_dialog_layout;
    Layout *m_button_layout;
    Button *m_ok_button;
    Button *m_cancel_button;

    SignalSender1<ButtonID> m_sender_dialog_returned;
    SignalSender0 m_sender_dialog_returned_ok;
    SignalSender0 m_sender_dialog_returned_cancel;

    SignalReceiver0 m_internal_receiver_ok_button_activated;
    SignalReceiver0 m_internal_receiver_cancel_button_activated;
}; // end of class Dialog

} // end of namespace Xrb

#endif // !defined(_XRB_DIALOG_HPP_)

