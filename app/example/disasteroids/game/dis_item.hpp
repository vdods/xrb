// ///////////////////////////////////////////////////////////////////////////
// dis_item.hpp by Victor Dods, created 2005/12/02
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_ITEM_HPP_)
#define _DIS_ITEM_HPP_

#include "xrb.hpp"

#include <string>

#include "dis_enums.hpp"

using namespace Xrb;

namespace Dis
{

class Ship;

// baseclass for all items that can be stored in a ship's inventory
class Item
{
public:

    Item (Uint8 const upgrade_level, ItemType const item_type)
        :
        m_upgrade_level(upgrade_level),
        m_item_type(item_type)
    {
        ASSERT1(m_upgrade_level < UPGRADE_LEVEL_COUNT);
    }
    virtual ~Item () { }

    static Item *Create (ItemType item_type, Uint8 upgrade_level);
    
    static std::string const &GetMineralSpriteFilename (Uint8 mineral_index);
    static Uint32 GetItemPrice (
        ItemType item_type,
        Uint8 upgrade_level,
        Uint8 mineral_index);
    
    inline Uint8 GetUpgradeLevel () const { return m_upgrade_level; }
    inline ItemType GetItemType () const { return m_item_type; }

    virtual bool IsPoweredDevice () const { return false; }

    virtual void Equip (Ship *owner_ship) { }
    virtual void Unequip (Ship *owner_ship) { }
    
private:

    Uint8 const m_upgrade_level;
    ItemType const m_item_type;
}; // end of class Item

} // end of namespace Dis

#endif // !defined(_DIS_ITEM_HPP_)

