// ///////////////////////////////////////////////////////////////////////////
// xrb_radiobutton.hpp by Victor Dods, created 2005/02/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_RADIOBUTTON_HPP_)
#define _XRB_RADIOBUTTON_HPP_

#include "xrb.hpp"

#include <map>

#include "xrb_button.hpp"
#include "xrb_input_events.hpp"
#include "xrb_key.hpp"
#include "xrb_widgetcontext.hpp"

namespace Xrb {
    
template <typename T, T sentinel> class RadioButtonGroup;

template <typename T, T sentinel>
class RadioButton : public Button
{
public:

    RadioButton (T radio_button_id, RadioButtonGroup<T, sentinel> *group, WidgetContext &context, std::string const &name = "RadioButton");
    // this constructor is for making toolbar-type buttons
    RadioButton (Resource<GlTexture> const &picture, T radio_button_id, RadioButtonGroup<T, sentinel> *group, WidgetContext &context, std::string const &name = "RadioButton");
    virtual ~RadioButton ();

    bool IsChecked () const { return m_is_checked; }
    T RadioButtonId () const { return m_radio_button_id; }
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
    SignalReceiver0 const *ReceiverCheck () { return &m_receiver_check; }
    // end SignalReceiver accessors
    //////////////////////////////////////////////////////////////////////////

    void Check ();

protected:

    virtual void HandleChangedStyleSheet ();
    virtual void UpdateRenderBackground ();

    virtual void HandleReleased ();
    
private:

    // called only by the owning RadioButtonGroup
    void Uncheck ();
    // called only by the owning RadioButtonGroup
    void AttachToGroup (RadioButtonGroup<T, sentinel> *group);
    // called only by the owning RadioButtonGroup
    void DetachFromGroup (RadioButtonGroup<T, sentinel> *group);

    void Initialize (T radio_button_id, RadioButtonGroup<T, sentinel> *group);

    // indicates if this radio button is checked
    bool m_is_checked;
    // this radio button's identifier
    T m_radio_button_id;
    // the radio button group this radio button belongs to
    RadioButtonGroup<T, sentinel> *m_group;

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
    SignalReceiver0 m_receiver_check;
    // end SignalReceivers
    //////////////////////////////////////////////////////////////////////////

    friend class RadioButtonGroup<T, sentinel>;
}; // end of class RadioButton

template <typename T, T sentinel>
class RadioButtonGroup : public SignalHandler
{
public:

    RadioButtonGroup ();
    ~RadioButtonGroup ();

    // returns a pointer to the currently checked button
    RadioButton<T, sentinel> *CheckedButton () const { return m_checked_button; }
    // returns the ID of the currently checked button
    T CheckedRadioButtonId () const { return m_radio_button_id; }

    //////////////////////////////////////////////////////////////////////////
    // SignalSender accessors
    SignalSender1<T> const *SenderCheckedRadioButtonIdChanged () { return &m_sender_checked_radio_button_id_changed; }
    // end SignalSender accessors
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // SignalReceiver accessors
    SignalReceiver1<T> const *ReceiverSetCheckedRadioButtonId () { return &m_receiver_set_checked_radio_button_id; }
    // end SignalReceiver accessors
    //////////////////////////////////////////////////////////////////////////

    void SetCheckedRadioButtonId (T radio_button_id);

    void AddButton (RadioButton<T, sentinel> *button);
    void RemoveButton (RadioButton<T, sentinel> *button);

private:

    typedef std::map<T, RadioButton<T, sentinel> *> RadioButtonMap;

    // the map of RadioButtons belonging to this RadioButtonGroup
    RadioButtonMap m_button_map;
    // a pointer to the child RadioButton which is currently checked
    RadioButton<T, sentinel> *m_checked_button;
    // current checked ID for this group (not derived from the checked
    // button, because there needs to be a value for when there are no
    // buttons added to the group)
    T m_radio_button_id;

    //////////////////////////////////////////////////////////////////////////
    // SignalSenders
    SignalSender1<T> m_sender_checked_radio_button_id_changed;
    // end SignalSenders
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // SignalReceivers
    SignalReceiver1<T> m_receiver_set_checked_radio_button_id;
    // end SignalReceivers
    //////////////////////////////////////////////////////////////////////////
}; // end of class RadioButtonGroup

// ///////////////////////////////////////////////////////////////////////////
// RadioButton<T, sentinel>
// ///////////////////////////////////////////////////////////////////////////

template <typename T, T sentinel>
RadioButton<T, sentinel>::RadioButton (T radio_button_id, RadioButtonGroup<T, sentinel> *group, WidgetContext &context, std::string const &name)
    :
    Button(Resource<GlTexture>(), context, name),
    m_sender_checked_state_changed(this),
    m_sender_checked(this),
    m_sender_unchecked(this),
    m_receiver_check(&RadioButton<T, sentinel>::Check, this)
{
    Initialize(radio_button_id, group);
}

template <typename T, T sentinel>
RadioButton<T, sentinel>::RadioButton (Resource<GlTexture> const &picture, T radio_button_id, RadioButtonGroup<T, sentinel> *group, WidgetContext &context, std::string const &name)
    :
    Button(picture, context, name),
    m_sender_checked_state_changed(this),
    m_sender_checked(this),
    m_sender_unchecked(this),
    m_receiver_check(&RadioButton<T, sentinel>::Check, this)
{
    Initialize(radio_button_id, group);
}

template <typename T, T sentinel>
RadioButton<T, sentinel>::~RadioButton ()
{
    if (m_group != NULL)
    {
        m_group->RemoveButton(this);
        m_group = NULL;
    }
}

template <typename T, T sentinel>
void RadioButton<T, sentinel>::SetCheckedBackgroundStyle (std::string const &style)
{
    ASSERT1(!style.empty());
    m_checked_background_style = style;
    SetRenderBackgroundNeedsUpdate();
}

template <typename T, T sentinel>
void RadioButton<T, sentinel>::Check ()
{
    if (!m_is_checked)
    {
        m_is_checked = true;

        // if this button is part of a radio button group, inform the group
        // that this radio button has been checked
        if (m_group != NULL)
            m_group->SetCheckedRadioButtonId(RadioButtonId());
        m_sender_checked_state_changed.Signal(m_is_checked);
        m_sender_checked.Signal();

        SetRenderBackgroundNeedsUpdate();
        SetRenderPictureNeedsUpdate();
    }
}

template <typename T, T sentinel>
void RadioButton<T, sentinel>::HandleChangedStyleSheet ()
{
    Button::HandleChangedStyleSheet();
    FixSize(
        ScreenCoordVector2(
            Context().StyleSheet_FontPixelHeight(StyleSheet::FontType::DEFAULT),
            Context().StyleSheet_FontPixelHeight(StyleSheet::FontType::DEFAULT)));
}

template <typename T, T sentinel>
void RadioButton<T, sentinel>::UpdateRenderBackground ()
{
    Button::UpdateRenderBackground();
    if (IsChecked() && !IsPressed())
        SetRenderBackground(Context().StyleSheet_Background(m_checked_background_style));
}

template <typename T, T sentinel>
void RadioButton<T, sentinel>::HandleReleased ()
{
    Button::HandleReleased();
    Check();
}

template <typename T, T sentinel>
void RadioButton<T, sentinel>::Uncheck ()
{
    if (m_is_checked)
    {
        m_is_checked = false;

        m_sender_checked_state_changed.Signal(m_is_checked);
        m_sender_unchecked.Signal();
    }
    SetRenderBackgroundNeedsUpdate();
    SetRenderPictureNeedsUpdate();
}

template <typename T, T sentinel>
void RadioButton<T, sentinel>::AttachToGroup (RadioButtonGroup<T, sentinel> *group)
{
    if (m_group != NULL)
        m_group->RemoveButton(this);

    ASSERT1(group != NULL);
    m_group = group;
}

template <typename T, T sentinel>
void RadioButton<T, sentinel>::DetachFromGroup (RadioButtonGroup<T, sentinel> *group)
{
    ASSERT1(m_group == group);
    m_group = NULL;
}

template <typename T, T sentinel>
void RadioButton<T, sentinel>::Initialize (T radio_button_id, RadioButtonGroup<T, sentinel> *group)
{
    ASSERT1(group != NULL);

    m_is_checked = false;
    m_radio_button_id = radio_button_id;
    m_group = group;
    m_group->AddButton(this);

    SetIdleBackgroundStyle(StyleSheet::BackgroundType::RADIO_BUTTON_UNCHECKED);
    SetMouseoverBackgroundStyle(StyleSheet::BackgroundType::RADIO_BUTTON_MOUSEOVER);
    SetPressedBackgroundStyle(StyleSheet::BackgroundType::RADIO_BUTTON_PRESSED);
    m_checked_background_style = StyleSheet::BackgroundType::RADIO_BUTTON_CHECKED;

    FixSize(
        ScreenCoordVector2(
            Context().StyleSheet_FontPixelHeight(StyleSheet::FontType::DEFAULT),
            Context().StyleSheet_FontPixelHeight(StyleSheet::FontType::DEFAULT)));
    SetFrameMarginsStyle(StyleSheet::MarginsType::RADIO_BUTTON_FRAME);
}

// ///////////////////////////////////////////////////////////////////////////
// RadioButtonGroup<T, sentinel>
// ///////////////////////////////////////////////////////////////////////////

template <typename T, T sentinel>
RadioButtonGroup<T, sentinel>::RadioButtonGroup ()
    :
    SignalHandler(),
    m_sender_checked_radio_button_id_changed(this),
    m_receiver_set_checked_radio_button_id(&RadioButtonGroup<T, sentinel>::SetCheckedRadioButtonId, this)
{
    m_radio_button_id = sentinel;
    m_checked_button = NULL;
}

template <typename T, T sentinel>
RadioButtonGroup<T, sentinel>::~RadioButtonGroup ()
{
    m_checked_button = NULL;

    // detach this group from all its children
    for (typename RadioButtonMap::iterator it = m_button_map.begin(), it_end = m_button_map.end();
         it != it_end;
         ++it)
    {
        RadioButton<T, sentinel> *radio_button = it->second;
        ASSERT1(radio_button != NULL);
        radio_button->DetachFromGroup(this);
    }

    m_button_map.clear();
}

template <typename T, T sentinel>
void RadioButtonGroup<T, sentinel>::SetCheckedRadioButtonId (T radio_button_id)
{
    // early out if the ID is the same
    if (m_radio_button_id == radio_button_id)
        return;

    RadioButton<T, sentinel> *new_checked_button;

    // special code for sentinel value (no buttons checked)
    if (radio_button_id == sentinel)
    {
        new_checked_button = NULL;
    }
    else
    {
        // make sure the ID is valid, early out if not
        typename RadioButtonMap::iterator it = m_button_map.find(radio_button_id);
        if (it == m_button_map.end())
            return;

        ASSERT1(it->second != NULL);
        new_checked_button = it->second;
    }

    // uncheck the current checked button (if applicable)
    if (m_checked_button != NULL)
        m_checked_button->Uncheck();

    m_radio_button_id = radio_button_id;
    m_checked_button = new_checked_button;

    // check the new current checked button (if applicable)
    if (m_checked_button != NULL)
        m_checked_button->Check();

    m_sender_checked_radio_button_id_changed.Signal(m_radio_button_id);
}

template <typename T, T sentinel>
void RadioButtonGroup<T, sentinel>::AddButton (RadioButton<T, sentinel> *button)
{
    ASSERT1(button != NULL);
    // make sure the incoming button is not checked
    button->Uncheck();

    bool no_buttons_in_group = m_button_map.size() == 0;
    // make sure there's no button with the same ID already in there
    ASSERT1(m_button_map.find(button->RadioButtonId()) == m_button_map.end() && "You tried to add two RadioButtons with the same ID");
    // only if there was no match will we add the button
    m_button_map[button->RadioButtonId()] = button;
    // if there were no buttons in this group, set the new one checked
    if (no_buttons_in_group)
        SetCheckedRadioButtonId(button->RadioButtonId());
}

template <typename T, T sentinel>
void RadioButtonGroup<T, sentinel>::RemoveButton (RadioButton<T, sentinel> *button)
{
    ASSERT1(button != NULL);

    // make sure there's no button with the same ID already in there
    typename RadioButtonMap::iterator it = m_button_map.find(button->RadioButtonId());
    // only if there was a match will we remove the button
    if (it != m_button_map.end())
    {
        ASSERT1(it->second != NULL);
        it->second->DetachFromGroup(this);
        m_button_map.erase(it);
    }

    // if a button is removed, then it will go back to the sentinel (no
    // button checked) value, because checking some arbitrary button would
    // probably be bad.
    SetCheckedRadioButtonId(sentinel);
}

} // end of namespace Xrb

#endif // !defined(_XRB_RADIOBUTTON_HPP_)

