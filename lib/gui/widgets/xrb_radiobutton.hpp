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

namespace Xrb
{

template <typename T, T sentinel> class RadioButtonGroup;

template <typename T, T sentinel>
class RadioButton : public Button
{
public:

    RadioButton (
        T id,
        RadioButtonGroup<T, sentinel> *group,
        ContainerWidget *parent,
        std::string const &name = "RadioButton");
    RadioButton (
        Resource<GlTexture> const &picture,
        T id,
        RadioButtonGroup<T, sentinel> *group,
        ContainerWidget *parent,
        std::string const &name = "RadioButton");
    virtual ~RadioButton ();

    inline bool IsChecked () const { return m_is_checked; }
    inline T ID () const { return m_id; }

    //////////////////////////////////////////////////////////////////////////
    // SignalSender accessors
    inline SignalSender1<bool> const *SenderCheckedStateChanged () { return &m_sender_checked_state_changed; }
    inline SignalSender0 const *SenderChecked () { return &m_sender_checked; }
    inline SignalSender0 const *SenderUnchecked () { return &m_sender_unchecked; }
    // end SignalSender accessors
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // SignalReceiver accessors
    inline SignalReceiver0 const *ReceiverCheck () { return &m_receiver_check; }
    // end SignalReceiver accessors
    //////////////////////////////////////////////////////////////////////////

    void Check ();

protected:

    virtual bool ProcessMouseButtonEvent (EventMouseButton const *e);

    virtual void UpdateRenderBackground ();
    virtual void UpdateRenderPicture ();

    // WidgetSkinHandler overrides
    virtual void HandleChangedWidgetSkinWidgetBackground (WidgetSkin::WidgetBackgroundType widget_background_type);
    virtual void HandleChangedWidgetSkinTexture (WidgetSkin::TextureType texture_type);

private:

    // called only by the owning RadioButtonGroup
    void Uncheck ();
    // called only by the owning RadioButtonGroup
    void AttachToGroup (RadioButtonGroup<T, sentinel> *group);
    // called only by the owning RadioButtonGroup
    void DetachFromGroup (RadioButtonGroup<T, sentinel> *group);

    void Initialize (T id, RadioButtonGroup<T, sentinel> *group);

    // indicates if this radio button is checked
    bool m_is_checked;
    // this radio button's identifier
    T m_id;
    // the radio button group this radio button belongs to
    RadioButtonGroup<T, sentinel> *m_group;

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
    inline RadioButton<T, sentinel> *CheckedButton () const
    {
        return m_checked_button;
    }
    // returns the ID of the currently checked button
    inline T ID () const
    {
        return m_id;
    }

    //////////////////////////////////////////////////////////////////////////
    // SignalSender accessors
    inline SignalSender1<T> const *SenderIDChanged () { return &m_sender_id_changed; }
    // end SignalSender accessors
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // SignalReceiver accessors
    inline SignalReceiver1<T> const *ReceiverSetID () { return &m_receiver_set_id; }
    // end SignalReceiver accessors
    //////////////////////////////////////////////////////////////////////////

    void SetID (T id);

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
    T m_id;

    //////////////////////////////////////////////////////////////////////////
    // SignalSenders
    SignalSender1<T> m_sender_id_changed;
    // end SignalSenders
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // SignalReceivers
    SignalReceiver1<T> m_receiver_set_id;
    // end SignalReceivers
    //////////////////////////////////////////////////////////////////////////
}; // end of class RadioButtonGroup

// ///////////////////////////////////////////////////////////////////////////
// RadioButton<T, sentinel>
// ///////////////////////////////////////////////////////////////////////////

template <typename T, T sentinel>
RadioButton<T, sentinel>::RadioButton (
    T id,
    RadioButtonGroup<T, sentinel> *group,
    ContainerWidget *parent,
    std::string const &name)
    :
    Button(Resource<GlTexture>(), parent, name),
    m_sender_checked_state_changed(this),
    m_sender_checked(this),
    m_sender_unchecked(this),
    m_receiver_check(&RadioButton<T, sentinel>::Check, this)
{
    Initialize(id, group);
}

template <typename T, T sentinel>
RadioButton<T, sentinel>::RadioButton (
    Resource<GlTexture> const &picture,
    T id,
    RadioButtonGroup<T, sentinel> *group,
    ContainerWidget *parent,
    std::string const &name)
    :
    Button(picture, parent, name),
    m_sender_checked_state_changed(this),
    m_sender_checked(this),
    m_sender_unchecked(this),
    m_receiver_check(&RadioButton<T, sentinel>::Check, this)
{
    Initialize(id, group);
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
void RadioButton<T, sentinel>::Check ()
{
    if (!m_is_checked)
    {
        m_is_checked = true;

        // if this button is part of a radio button group, inform the group
        // that this radio button has been checked
        if (m_group != NULL)
            m_group->SetID(ID());
        m_sender_checked_state_changed.Signal(m_is_checked);
        m_sender_checked.Signal();
    }
    UpdateRenderBackground();
    UpdateRenderPicture();
}

template <typename T, T sentinel>
bool RadioButton<T, sentinel>::ProcessMouseButtonEvent (EventMouseButton const *e)
{
    // first call Button's handler
    Button::ProcessMouseButtonEvent(e);

    // if the left mouse button was clicked and released, attempt to check it.
    if (e->ButtonCode() == Key::LEFTMOUSE && e->IsMouseButtonDownEvent())
        Check();

    return true;
}

template <typename T, T sentinel>
void RadioButton<T, sentinel>::UpdateRenderBackground ()
{
    SetRenderBackground(WidgetSkinWidgetBackground(WidgetSkin::RADIO_BUTTON_BACKGROUND));
}

template <typename T, T sentinel>
void RadioButton<T, sentinel>::UpdateRenderPicture ()
{
    if (IsChecked())
    {
        if (Picture().IsValid())
            SetRenderPicture(Picture());
        else
            SetRenderPicture(WidgetSkinTexture(WidgetSkin::RADIO_BUTTON_CHECK_TEXTURE));
    }
    else
        SetRenderPicture(Resource<GlTexture>());
}

template <typename T, T sentinel>
void RadioButton<T, sentinel>::HandleChangedWidgetSkinWidgetBackground (WidgetSkin::WidgetBackgroundType widget_background_type)
{
    if (widget_background_type == WidgetSkin::RADIO_BUTTON_BACKGROUND)
        UpdateRenderBackground();
}

template <typename T, T sentinel>
void RadioButton<T, sentinel>::HandleChangedWidgetSkinTexture (WidgetSkin::TextureType texture_type)
{
    if (texture_type == WidgetSkin::RADIO_BUTTON_CHECK_TEXTURE)
        UpdateRenderPicture();
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
    UpdateRenderBackground();
    UpdateRenderPicture();
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
void RadioButton<T, sentinel>::Initialize (T id, RadioButtonGroup<T, sentinel> *group)
{
    ASSERT1(group != NULL);

    m_is_checked = false;
    RadioButton<T, sentinel>::UpdateRenderBackground();
    RadioButton<T, sentinel>::UpdateRenderPicture();

    m_id = id;
    m_group = group;

    m_group->AddButton(this);

    FixSize(
        ScreenCoordVector2(
            WidgetSkinFont(WidgetSkin::DEFAULT_FONT)->PixelHeight(),
            WidgetSkinFont(WidgetSkin::DEFAULT_FONT)->PixelHeight()));
    SetFrameMargins(WidgetSkinMargins(WidgetSkin::RADIO_BUTTON_FRAME_MARGINS));
}

// ///////////////////////////////////////////////////////////////////////////
// RadioButtonGroup<T, sentinel>
// ///////////////////////////////////////////////////////////////////////////

template <typename T, T sentinel>
RadioButtonGroup<T, sentinel>::RadioButtonGroup ()
    :
    SignalHandler(),
    m_sender_id_changed(this),
    m_receiver_set_id(&RadioButtonGroup<T, sentinel>::SetID, this)
{
    m_id = sentinel;
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
void RadioButtonGroup<T, sentinel>::SetID (T id)
{
    // early out if the ID is the same
    if (m_id == id)
        return;

    RadioButton<T, sentinel> *new_checked_button;

    // special code for sentinel value (no buttons checked)
    if (id == sentinel)
    {
        new_checked_button = NULL;
    }
    else
    {
        // make sure the ID is valid, early out if not
        typename RadioButtonMap::iterator it = m_button_map.find(id);
        if (it == m_button_map.end())
            return;

        ASSERT1(it->second != NULL);
        new_checked_button = it->second;
    }

    // uncheck the current checked button (if applicable)
    if (m_checked_button != NULL)
        m_checked_button->Uncheck();

    m_id = id;
    m_checked_button = new_checked_button;

    // check the new current checked button (if applicable)
    if (m_checked_button != NULL)
        m_checked_button->Check();

    m_sender_id_changed.Signal(m_id);
}

template <typename T, T sentinel>
void RadioButtonGroup<T, sentinel>::AddButton (RadioButton<T, sentinel> *button)
{
    ASSERT1(button != NULL);
    // make sure the incoming button is not checked
    button->Uncheck();

    bool no_buttons_in_group = m_button_map.size() == 0;
    // make sure there's no button with the same ID already in there
    typename RadioButtonMap::iterator it = m_button_map.find(button->ID());
    // only if there was no match will we add the button
    ASSERT1(it == m_button_map.end() &&
            "You tried to add two RadioButtons with the same ID");
    m_button_map[button->ID()] = button;
    // if there were no buttons in this group, set the new one checked
    if (no_buttons_in_group)
        SetID(button->ID());
}

template <typename T, T sentinel>
void RadioButtonGroup<T, sentinel>::RemoveButton (RadioButton<T, sentinel> *button)
{
    ASSERT1(button != NULL);

    // make sure there's no button with the same ID already in there
    typename RadioButtonMap::iterator it = m_button_map.find(button->ID());
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
    SetID(sentinel);
}

} // end of namespace Xrb

#endif // !defined(_XRB_RADIOBUTTON_HPP_)

