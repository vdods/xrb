// ///////////////////////////////////////////////////////////////////////////
// dis_inventorybutton.hpp by Victor Dods, created 2005/12/17
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_INVENTORYBUTTON_HPP_)
#define _DIS_INVENTORYBUTTON_HPP_

#include "xrb_button.hpp"

#include "dis_item.hpp"

using namespace Xrb;

/*
design:
InventoryButton will work as it used to, except that it will have attached to
it two Labels that serve the following function:
- upon mouseover, Label 1 will display either '+' or nothing depending on the
  affordability of the item.  if the max level item is already owned, it will
  display 'MAX'
- Label 2 will display the upgrade level.  the numbering will start at 1.
  if the item is not owned, the label will either show no text, or be invisible.
the labels will be attached not directly, but through signal senders which
convey the pertinent data.
*/

namespace Dis {

class InventoryButton : public Button
{
public:

    InventoryButton (ItemType item_type, WidgetContext &context, std::string const &name = "InventoryButton");
    virtual ~InventoryButton () { }

    bool IsEquipped () const { return m_is_equipped; }
    Uint8 OwnedUpgradeLevel () const { return m_owned_upgrade_level; }
    bool IsAffordable () const { return m_is_affordable; }
    void SetProperties (bool is_equipped, Uint8 owned_upgrade_level, bool is_affordable);

    std::string UpgradeIndication () const;
    Uint32 UpgradeLevelNumeral () const { return Uint8(m_owned_upgrade_level+1); }

    SignalSender2<ItemType, Uint8> const *SenderAttemptToUpgradeItem () { return &m_sender_attempt_to_upgrade_item; }
    SignalSender2<ItemType, Uint8> const *SenderEquipItem () { return &m_sender_equip_item; }
    SignalSender2<ItemType, Uint8> const *SenderShowPrice () { return &m_sender_show_price; }
    SignalSender1<ItemType> const *SenderHidePrice () { return &m_sender_hide_price; }

    // empty, "BUY", "UPGR", or "MAX" depending on state
    SignalSender1<std::string> const *SenderUpgradeIndication () { return &m_sender_upgrade_indication; }
    // upgrade level starting at 1.  0 indicates not owned (this corresponds with upgrade level being Uint8(-1), i.e. 255)
    SignalSender1<Uint32> const *SenderUpgradeLevelNumeral () { return &m_sender_upgrade_level_numeral; }

protected:

    virtual void HandleFrame ();

    virtual void HandleMouseoverOn ();
    virtual void HandleMouseoverOff ();
    virtual void HandleReleased ();

    virtual void UpdateRenderBackground ();

private:

    void UpdateRenderParameters ();
    void SetCurrentSizeParameter (Float current_size_parameter);

    static Resource<GlTexture> ButtonTexture (ItemType item_type);

    static Float const ms_unequipped_size_ratio;
    static Float const ms_size_parameter_change_rate;

    ItemType const m_item_type;
    bool m_is_equipped;
    Uint8 m_owned_upgrade_level;
    bool m_is_affordable;

    Float m_current_size_parameter;
    Float m_target_size_parameter;

    SignalSender2<ItemType, Uint8> m_sender_attempt_to_upgrade_item;
    SignalSender2<ItemType, Uint8> m_sender_equip_item;
    SignalSender2<ItemType, Uint8> m_sender_show_price;
    SignalSender1<ItemType> m_sender_hide_price;

    std::string m_upgrade_indication;
    Uint32 m_upgrade_level_numeral;

    SignalSender1<std::string> m_sender_upgrade_indication;
    SignalSender1<Uint32> m_sender_upgrade_level_numeral;
}; // end of class InventoryButton

} // end of namespace Dis

#endif // !defined(_DIS_INVENTORYBUTTON_HPP_)

