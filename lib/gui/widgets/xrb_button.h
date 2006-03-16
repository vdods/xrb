// ///////////////////////////////////////////////////////////////////////////
// xrb_button.h by Victor Dods, created 2004/11/07
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_BUTTON_H_)
#define _XRB_BUTTON_H_

#include "xrb.h"

#include "xrb_label.h"

namespace Xrb
{

class Button : public Label
{
public:

    Button (
        std::string const &text,
        Widget *const parent,
        std::string const &name = "Button");
    Button (
        Resource<GLTexture> const &picture,
        Widget *const parent,
        std::string const &name = "Button");
    virtual ~Button () { }

    inline bool GetIsPressed () const
    {
        return m_is_pressed;
    }

    virtual void SetIsEnabled (bool is_enabled);

    //////////////////////////////////////////////////////////////////////////
    // SignalSender accessors
    SignalSender1<bool> const *SenderPressedStateChanged () { return &m_sender_pressed_state_changed; }
    SignalSender0 const *SenderPressed () { return &m_sender_pressed; }
    SignalSender0 const *SenderReleased () { return &m_sender_released; }
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
    virtual void HandleChangedWidgetSkinWidgetBackground (
        WidgetSkin::WidgetBackgroundType widget_background_type);

private:

    void Initialize ();

    // indicates if a mouse button is currently pressed on this widget
    bool m_is_pressed;

    //////////////////////////////////////////////////////////////////////////
    // SignalSenders
    SignalSender1<bool> m_sender_pressed_state_changed;
    SignalSender0 m_sender_pressed;
    SignalSender0 m_sender_released;
    // end of SignalSenders
    //////////////////////////////////////////////////////////////////////////
}; // end of class Button

} // end of namespace Xrb

#endif // !defined(_XRB_BUTTON_H_)
