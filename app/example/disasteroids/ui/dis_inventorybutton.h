// ///////////////////////////////////////////////////////////////////////////
// dis_inventorybutton.h by Victor Dods, created 2005/12/17
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_INVENTORYBUTTON_H_)
#define _DIS_INVENTORYBUTTON_H_

#include "xrb_button.h"

#include "dis_item.h"

using namespace Xrb;

namespace Dis
{

class InventoryButton : public Button
{
public:

    enum Status
    {
        S_NOT_AFFORDABLE = 0,
        S_AFFORDABLE,
        S_OWNED,
        S_EQUIPPED,

        S_COUNT
    }; // end of enum Status

    InventoryButton (
        ItemType item_type,
        Uint8 upgrade_level,
        Widget *parent,
        std::string const &name = "InventoryButton");
    virtual ~InventoryButton () { }

    static Resource<GLTexture> GetButtonTexture (ItemType item_type, Uint8 upgrade_level);
    
    void SetStatus (Status status);

    inline SignalSender2<ItemType, Uint8> const *SenderAttemptToBuyItem () { return &m_sender_attempt_to_buy_item; }
    inline SignalSender2<ItemType, Uint8> const *SenderEquipItem () { return &m_sender_equip_item; }
    inline SignalSender2<ItemType, Uint8> const *SenderShowPrice () { return &m_sender_show_price; }
    inline SignalSender2<ItemType, Uint8> const *SenderHidePrice () { return &m_sender_hide_price; }
    
protected:

    virtual void ProcessFrameOverride ();

    virtual void HandleMouseoverOn ();
    virtual void HandleMouseoverOff ();
    virtual void HandleReleased ();

    virtual void UpdateRenderBackground ();
        
private:

    void SetCurrentSizeParameter (Float current_size_parameter);

    static Float const ms_unequipped_size_ratio;
    static Float const ms_size_parameter_change_rate;

    Status m_status;    
    Float m_current_size_parameter;
    Float m_target_size_parameter;
    
    ItemType const m_item_type;
    Uint8 const m_upgrade_level;

    SignalSender2<ItemType, Uint8> m_sender_attempt_to_buy_item;
    SignalSender2<ItemType, Uint8> m_sender_equip_item;
    SignalSender2<ItemType, Uint8> m_sender_show_price;
    SignalSender2<ItemType, Uint8> m_sender_hide_price;
}; // end of class InventoryButton

} // end of namespace Dis

#endif // !defined(_DIS_INVENTORYBUTTON_H_)

