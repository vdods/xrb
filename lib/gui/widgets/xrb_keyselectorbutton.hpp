// ///////////////////////////////////////////////////////////////////////////
// xrb_keyselectorbutton.hpp by Victor Dods, created 2006/08/19
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_KEYSELECTORBUTTON_HPP_)
#define _XRB_KEYSELECTORBUTTON_HPP_

#include "xrb.hpp"

#include "xrb_button.hpp"
#include "xrb_dialog.hpp"
#include "xrb_key.hpp"

namespace Xrb {

class KeySelectorButton : public Button
{
public:

    KeySelectorButton (std::string const &input_action_name, Key::Code key_code, WidgetContext &context, std::string const &name = "KeySelectorButton");

    Key::Code KeyCode () const { return m_key_code; }
    void SetKeyCode (Key::Code key_code);

protected:

    virtual void HandleReleased ();

private:

    void DialogReturned (Dialog::ButtonID button_id);

    class KeySelectorDialog : public Dialog
    {
    public:

        KeySelectorDialog (std::string const &message, WidgetContext &context, std::string const &name = "KeySelectorDialog");

        Key::Code KeyCode () const { return m_key_code; }

    protected:

        // overridden so we can grab the mouse at the earliest possible moment.
        virtual void HandleActivate ();

        virtual bool ProcessKeyEvent (EventKey const &e);
        virtual bool ProcessMouseButtonEvent (EventMouseButton const &e);
        virtual bool ProcessMouseWheelEvent (EventMouseWheel const &e);

    private:

        Key::Code m_key_code;
    }; // end of class KeySelectorButton::KeySelectorDialog

    std::string const m_input_action_name;
    Key::Code m_key_code;
    KeySelectorDialog *m_key_selector_dialog;
    SignalReceiver1<Dialog::ButtonID> m_internal_receiver_dialog_returned;
}; // end of class KeySelectorButton

} // end of namespace Xrb

#endif // !defined(_XRB_KEYSELECTORBUTTON_HPP_)

