// ///////////////////////////////////////////////////////////////////////////
// dis_inventorypanel.h by Victor Dods, created 2005/12/17
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_INVENTORYPANEL_H_)
#define _DIS_INVENTORYPANEL_H_

#include "xrb_modalwidget.h"

#include "dis_inventorybutton.h"
#include "dis_item.h"
#include "xrb_button.h"

using namespace Xrb;

namespace Xrb
{
class Label;
template <typename T> class ValueLabel;
} // end of namespace Xrb

namespace Dis
{

class InventoryButton;
class PlayerShip;

class InventoryPanel : public ModalWidget
{
public:

    InventoryPanel (
        Widget *parent,
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

    void ConnectInventoryButton (InventoryButton *inventory_button);

    void AttemptToBuyItem (ItemType item_type, Uint8 upgrade_level);
    void EquipItem (ItemType item_type, Uint8 upgrade_level);
    void ShowPrice (ItemType item_type, Uint8 upgrade_level);
    void HidePrice (ItemType item_type, Uint8 upgrade_level);

    void Deactivate ();
    
    PlayerShip *m_inventory_owner_ship;

    InventoryButton *m_inventory_button[IT_COUNT][UPGRADE_LEVEL_COUNT];
    // TODO: misc upgrade buttons

    ValueLabel<Uint32> *m_mineral_cost_label[MINERAL_COUNT];
    Label *m_mineral_icon_label[MINERAL_COUNT];

    Button *m_return_button;
    Button *m_end_button;
    Button *m_quit_button;
    
    bool m_prices_are_shown;
    ItemType m_currently_shown_price_item_type;
    Uint8 m_currently_shown_price_upgrade_level;

    static Color const ms_affordable_mineral_color_mask;
    static Color const ms_not_affordable_mineral_color_mask;
    
    SignalSender0 m_sender_deactivate;
        
    SignalReceiver2<ItemType, Uint8> m_receiver_attempt_to_buy_item;
    SignalReceiver2<ItemType, Uint8> m_receiver_equip_item;
    SignalReceiver2<ItemType, Uint8> m_receiver_show_price;
    SignalReceiver2<ItemType, Uint8> m_receiver_hide_price;

    SignalReceiver0 m_internal_receiver_deactivate;
}; // end of class InventoryPanel

} // end of namespace Dis

#endif // !defined(_DIS_INVENTORYPANEL_H_)

