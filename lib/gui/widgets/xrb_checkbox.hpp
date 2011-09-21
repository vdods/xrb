// ///////////////////////////////////////////////////////////////////////////
// xrb_checkbox.hpp by Victor Dods, created 2005/02/04
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_CHECKBOX_HPP_)
#define _XRB_CHECKBOX_HPP_

#include "xrb.hpp"

#include "xrb_button.hpp"

namespace Xrb {

class CheckBox : public Button
{
public:

    CheckBox (WidgetContext &context, std::string const &name = "CheckBox");
    virtual ~CheckBox () { }

    bool IsChecked () const { return m_is_checked; }
    std::string const &CheckedBackgroundStyle () const { return m_checked_background_style; }

    void SetCheckedBackgroundStyle (std::string const &style);

    //////////////////////////////////////////////////////////////////////////
    // SignalSender accessors
    SignalSender1<bool> const *SenderCheckedStateChanged () { return &m_sender_checked_state_changed; }
    SignalSender0 const *SenderChecked () { return &m_sender_checked; }
    SignalSender0 const *SenderUnchecked () { return &m_sender_unchecked; }
    // end SignalSender accessors
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // SignalReceiver accessors
    SignalReceiver1<bool> const *ReceiverSetIsChecked () { return &m_receiver_set_is_checked; }
    SignalReceiver0 const *ReceiverCheck () { return &m_receiver_check; }
    SignalReceiver0 const *ReceiverUncheck () { return &m_receiver_uncheck; }
    SignalReceiver0 const *ReceiverToggleIsChecked () { return &m_receiver_toggle_is_checked; }
    // end SignalReceiver accessors
    //////////////////////////////////////////////////////////////////////////

    virtual void SetIsEnabled (bool is_enabled);
    void SetIsChecked (bool is_checked);

    void Check () { SetIsChecked(true); }
    void Uncheck () { SetIsChecked(false); }
    void ToggleIsChecked ();

protected:

    virtual void HandleChangedStyleSheet ();
    virtual void UpdateRenderBackground ();

    virtual void HandleReleased ();

    virtual void HandleIsCheckedChanged () { }

private:

    bool m_is_checked;
    // style sheet stuff
    std::string m_checked_background_style;

    //////////////////////////////////////////////////////////////////////////
    // SignalSenders
    SignalSender1<bool> m_sender_checked_state_changed;
    SignalSender0 m_sender_checked;
    SignalSender0 m_sender_unchecked;
    // end SignalSenders
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // SignalReceivers
    SignalReceiver1<bool> m_receiver_set_is_checked;
    SignalReceiver0 m_receiver_check;
    SignalReceiver0 m_receiver_uncheck;
    SignalReceiver0 m_receiver_toggle_is_checked;
    // end SignalReceivers
    //////////////////////////////////////////////////////////////////////////
}; // end of class CheckBox

} // end of namespace Xrb

#endif // !defined(_XRB_CHECKBOX_HPP_)
