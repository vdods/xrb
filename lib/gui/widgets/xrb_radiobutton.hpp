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
        Resource<GLTexture> const &picture,
        T id,
        RadioButtonGroup<T, sentinel> *group,
        ContainerWidget *parent,
        std::string const &name = "RadioButton");
    virtual ~RadioButton ();

    inline bool IsChecked () const { return m_is_checked; }
    inline T GetID () const { return m_id; }

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
    virtual void HandleChangedWidgetSkinWidgetBackground (
        WidgetSkin::WidgetBackgroundType widget_background_type);
    virtual void HandleChangedWidgetSkinTexture (
        WidgetSkin::TextureType texture_type);

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
    inline RadioButton<T, sentinel> *GetCheckedButton () const
    {
        return m_checked_button;
    }
    // returns the ID of the currently checked button
    inline T GetID () const
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
    typedef typename RadioButtonMap::iterator RadioButtonMapIterator;

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

// function definitions for RadioButton and RadioButtonGroup
#include "xrb_radiobutton.tcpp"

} // end of namespace Xrb

#endif // !defined(_XRB_RADIOBUTTON_HPP_)

