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

    InventoryPanel (
        ContainerWidget *parent,
        std::string const &name = "InventoryPanel");
    virtual ~InventoryPanel () { }

    inline SignalSender0 const *SenderDeactivate () { return &m_sender_deactivate; }
    SignalSender0 const *SenderEndGame ();
    SignalSender0 const *SenderQuitGame ();

    inline void SetInventoryOwnerShip (PlayerShip *const inventory_owner_ship)
    {
        m_inventory_owner_ship = inventory_owner_ship;
        UpdatePanelState();
    }
    void SetItemStatus (
        ItemType item_type,
        Uint8 upgrade_level,
        InventoryButton::Status status);

    void UpdatePanelState ();

protected:

    virtual bool ProcessKeyEvent (EventKey const *e);

private:

    void CreateInventoryButtonColumn (ItemType item_type, Layout *parent);
    void ConnectInventoryButton (InventoryButton *inventory_button);

    void AttemptToBuyItem (ItemType item_type, Uint8 upgrade_level);
    void EquipItem (ItemType item_type, Uint8 upgrade_level);
    void ShowPrice (ItemType item_type, Uint8 upgrade_level);
    void HidePrice (ItemType item_type, Uint8 upgrade_level);

    void Deactivate ();
    void ActivateControlsDialog ();
    void ControlsDialogReturned (Dialog::ButtonID button_id);

    PlayerShip *m_inventory_owner_ship;

    InventoryButton *m_inventory_button[IT_COUNT][UPGRADE_LEVEL_COUNT];
    // TODO: misc upgrade buttons

    ValueLabel<Uint32> *m_mineral_cost_label[MINERAL_COUNT];
    Label *m_mineral_icon_label[MINERAL_COUNT];

    Button *m_return_button;
    Button *m_controls_button;
    Button *m_end_button;
    Button *m_quit_button;

    bool m_prices_are_shown;
    ItemType m_currently_shown_price_item_type;
    Uint8 m_currently_shown_price_upgrade_level;

    ControlsPanel *m_controls_panel;

    static Color const ms_affordable_mineral_color_mask;
    static Color const ms_not_affordable_mineral_color_mask;

    SignalSender0 m_sender_deactivate;

    SignalReceiver2<ItemType, Uint8> m_receiver_attempt_to_buy_item;
    SignalReceiver2<ItemType, Uint8> m_receiver_equip_item;
    SignalReceiver2<ItemType, Uint8> m_receiver_show_price;
    SignalReceiver2<ItemType, Uint8> m_receiver_hide_price;

    SignalReceiver0 m_internal_receiver_deactivate;
    SignalReceiver0 m_internal_receiver_activate_controls_dialog;
    SignalReceiver1<Dialog::ButtonID> m_internal_receiver_controls_dialog_returned;
}; // end of class InventoryPanel

} // end of namespace Dis

#endif // !defined(_DIS_INVENTORYPANEL_HPP_)

