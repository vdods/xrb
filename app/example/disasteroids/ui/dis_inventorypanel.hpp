// ///////////////////////////////////////////////////////////////////////////
// dis_inventorypanel.hpp by Victor Dods, created 2005/12/17
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_INVENTORYPANEL_HPP_)
#define _DIS_INVENTORYPANEL_HPP_

#include "xrb_modalwidget.hpp"

#include "dis_inventorybutton.hpp"
#include "dis_item.hpp"
#include "xrb_button.hpp"
#include "xrb_dialog.hpp"
#include "xrb_parse_datafile_value.hpp"
#include "xrb_resourcelibrary.hpp"

using namespace Xrb;

namespace Xrb {

class Label;
class Layout;
template <typename T> class ValueLabel;

} // end of namespace Xrb

namespace Dis {

class InventoryButton;
class ControlsPanel;
class PlayerShip;

class InventoryPanel : public ModalWidget
{
public:

    InventoryPanel (ContainerWidget *parent, std::string const &name = "InventoryPanel");
    virtual ~InventoryPanel () { }

    SignalSender0 const *SenderDeactivate () { return &m_sender_deactivate; }
    SignalSender0 const *SenderEndGame ();
    SignalSender0 const *SenderQuitGame ();

    void SetInventoryOwnerShip (PlayerShip *inventory_owner_ship)
    {
        m_inventory_owner_ship = inventory_owner_ship;
        UpdatePanelState();
    }

    void UpdatePanelState ();

protected:

    virtual bool ProcessKeyEvent (EventKey const *e);

private:

    void CreateInventoryButtonAndFriends (ItemType item_type, ContainerWidget *parent);
    void ConnectInventoryButton (InventoryButton &inventory_button, Label &upgrade_indication_label, ValueLabel<Uint32> &upgrade_level_label);

    void AttemptToUpgradeItem (ItemType item_type, Uint8 upgrade_level);
    void EquipItem (ItemType item_type, Uint8 upgrade_level);
    void ShowPrice (ItemType item_type, Uint8 upgrade_level);
    void HidePrice (ItemType item_type);

    void Deactivate ();
    void ActivateControlsDialog ();
    void ControlsDialogReturned (Dialog::ButtonID button_id);

    PlayerShip *m_inventory_owner_ship;

    InventoryButton *m_inventory_button[IT_COUNT];
    ValueLabel<Uint32> *m_upgrade_level_label[IT_COUNT];
    Label *m_upgrade_indication_label[IT_COUNT];
    ValueLabel<Uint32> *m_game_tip_header_label;
    Label *m_game_tip_text_label;
    Label *m_game_tip_picture_label;
    Button *m_game_tip_previous_button;
    Button *m_game_tip_next_button;

    ValueLabel<Uint32> *m_mineral_cost_label[MINERAL_COUNT];
    Label *m_mineral_icon_label[MINERAL_COUNT];

    Button *m_return_button;
    Button *m_controls_button;
    Button *m_end_button;
    Button *m_quit_button;

    bool m_prices_are_shown;
    ItemType m_currently_shown_price_item_type;
    Uint8 m_currently_shown_price_upgrade_level;

    Resource<Parse::DataFile::Structure> m_game_tip_data;

    ControlsPanel *m_controls_panel;

    static Color const ms_affordable_mineral_color_mask;
    static Color const ms_not_affordable_mineral_color_mask;

    SignalSender0 m_sender_deactivate;

    SignalReceiver2<ItemType, Uint8> m_receiver_attempt_to_upgrade_item;
    SignalReceiver2<ItemType, Uint8> m_receiver_equip_item;
    SignalReceiver2<ItemType, Uint8> m_receiver_show_price;
    SignalReceiver1<ItemType> m_receiver_hide_price;

    SignalReceiver0 m_internal_receiver_deactivate;
    SignalReceiver0 m_internal_receiver_activate_controls_dialog;
    SignalReceiver1<Dialog::ButtonID> m_internal_receiver_controls_dialog_returned;
}; // end of class InventoryPanel

} // end of namespace Dis

#endif // !defined(_DIS_INVENTORYPANEL_HPP_)

