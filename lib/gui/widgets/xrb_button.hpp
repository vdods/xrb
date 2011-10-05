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

    Button (std::string const &text, WidgetContext &context, std::string const &name = "Button");
    Button (Resource<GlTexture> const &picture, WidgetContext &context, std::string const &name = "Button");
    virtual ~Button () { }

    bool IsPressed () const { return m_is_pressed; }
    Uint32 ButtonId () const { return m_button_id; }
    std::string const &IdleBackgroundStyle () const { return m_idle_background_style; }
    std::string const &PressedBackgroundStyle () const { return m_pressed_background_style; }
    std::string const &MouseoverBackgroundStyle () const { return m_mouseover_background_style; }

    void SetButtonId (Uint32 button_id) { m_button_id = button_id; }
    void SetIdleBackgroundStyle (std::string const &style);
    void SetPressedBackgroundStyle (std::string const &style);
    void SetMouseoverBackgroundStyle (std::string const &style);

    virtual void SetIsEnabled (bool is_enabled);

    //////////////////////////////////////////////////////////////////////////
    // SignalSender accessors
    SignalSender1<bool> const *SenderPressedStateChanged () { return &m_sender_pressed_state_changed; }
    SignalSender0 const *SenderPressed () { return &m_sender_pressed; }
    SignalSender0 const *SenderReleased () { return &m_sender_released; }

    SignalSender2<bool,Uint32> const *SenderPressedStateChangedWithButtonId () { return &m_sender_pressed_state_changed_with_button_id; }
    SignalSender1<Uint32> const *SenderPressedWithButtonId () { return &m_sender_pressed_with_button_id; }
    SignalSender1<Uint32> const *SenderReleasedWithButtonId () { return &m_sender_released_with_button_id; }
    // end of SignalSender accessors
    //////////////////////////////////////////////////////////////////////////

protected:

    virtual bool ProcessMouseButtonEvent (EventMouseButton const &e);
    virtual void HandleMouseoverOn ();
    virtual void HandleMouseoverOff ();
    virtual void HandlePressed ();
    virtual void HandleReleased ();
    virtual void UpdateRenderBackground ();

private:

    void Initialize ();

    // indicates if a mouse button is currently pressed on this widget
    bool m_is_pressed;
    // optionally specifiable identifier that can be sent when the button state changes
    Uint32 m_button_id;

    // style sheet stuff
    std::string m_idle_background_style;
    std::string m_pressed_background_style;
    std::string m_mouseover_background_style;
    
    //////////////////////////////////////////////////////////////////////////
    // SignalSenders
    SignalSender1<bool> m_sender_pressed_state_changed;
    SignalSender0 m_sender_pressed;
    SignalSender0 m_sender_released;

    SignalSender2<bool,Uint32> m_sender_pressed_state_changed_with_button_id;
    SignalSender1<Uint32> m_sender_pressed_with_button_id;
    SignalSender1<Uint32> m_sender_released_with_button_id;
    // end of SignalSenders
    //////////////////////////////////////////////////////////////////////////
}; // end of class Button

} // end of namespace Xrb

#endif // !defined(_XRB_BUTTON_HPP_)
