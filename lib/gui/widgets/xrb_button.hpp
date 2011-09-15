// ///////////////////////////////////////////////////////////////////////////
// xrb_button.hpp by Victor Dods, created 2004/11/07
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_BUTTON_HPP_)
#define _XRB_BUTTON_HPP_

#include "xrb.hpp"

#include "xrb_label.hpp"

namespace Xrb {

class Button : public Label
{
public:

    Button (std::string const &text, std::string const &name = "Button");
    Button (Resource<GlTexture> const &picture, std::string const &name = "Button");
    virtual ~Button () { }

    bool IsPressed () const { return m_is_pressed; }
    Uint32 Id () const { return m_id; }

    void SetId (Uint32 id) { m_id = id; }

    virtual void SetIsEnabled (bool is_enabled);

    //////////////////////////////////////////////////////////////////////////
    // SignalSender accessors
    SignalSender1<bool> const *SenderPressedStateChanged () { return &m_sender_pressed_state_changed; }
    SignalSender0 const *SenderPressed () { return &m_sender_pressed; }
    SignalSender0 const *SenderReleased () { return &m_sender_released; }

    SignalSender2<bool,Uint32> const *SenderPressedStateChangedWithId () { return &m_sender_pressed_state_changed_with_id; }
    SignalSender1<Uint32> const *SenderPressedWithId () { return &m_sender_pressed_with_id; }
    SignalSender1<Uint32> const *SenderReleasedWithId () { return &m_sender_released_with_id; }
    // end of SignalSender accessors
    //////////////////////////////////////////////////////////////////////////

protected:

    virtual bool ProcessMouseButtonEvent (EventMouseButton const *e);
    virtual void HandleMouseoverOn ();
    virtual void HandleMouseoverOff ();
    virtual void HandlePressed ();
    virtual void HandleReleased ();
    virtual void UpdateRenderBackground ();

    // WidgetSkinHandler overrides
    virtual void HandleChangedWidgetSkin ();

private:

    void Initialize ();

    // indicates if a mouse button is currently pressed on this widget
    bool m_is_pressed;
    // optionally specifiable identifier that can be sent when the button state changes
    Uint32 m_id;

    //////////////////////////////////////////////////////////////////////////
    // SignalSenders
    SignalSender1<bool> m_sender_pressed_state_changed;
    SignalSender0 m_sender_pressed;
    SignalSender0 m_sender_released;

    SignalSender2<bool,Uint32> m_sender_pressed_state_changed_with_id;
    SignalSender1<Uint32> m_sender_pressed_with_id;
    SignalSender1<Uint32> m_sender_released_with_id;
    // end of SignalSenders
    //////////////////////////////////////////////////////////////////////////
}; // end of class Button

} // end of namespace Xrb

#endif // !defined(_XRB_BUTTON_HPP_)
