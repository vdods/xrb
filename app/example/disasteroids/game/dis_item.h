// ///////////////////////////////////////////////////////////////////////////
// dis_item.h by Victor Dods, created 2005/12/02
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_ITEM_H_)
#define _DIS_ITEM_H_

#include "xrb.h"

#include <string>

#include "dis_enums.h"

using namespace Xrb;

namespace Dis
{

// baseclass for all items that can be stored in a ship's inventory
class Item
{
public:

    Item (Uint8 const upgrade_level, ItemType const type)
        :
        m_upgrade_level(upgrade_level),
        m_type(type)
    {
        ASSERT1(m_upgrade_level < UPGRADE_LEVEL_COUNT)
    }
    virtual ~Item () { }

    static Item *Create (ItemType type, Uint8 upgrade_level);
    
    static std::string const &GetMineralSpriteFilename (Uint8 mineral_index);
    static Uint32 GetItemPrice (
        ItemType item_type,
        Uint8 upgrade_level,
        Uint8 mineral_index);
    
    inline Uint8 GetUpgradeLevel () const { return m_upgrade_level; }
    inline ItemType GetType () const { return m_type; }
    
private:

    Uint8 const m_upgrade_level;
    ItemType const m_type;
}; // end of class Item

} // end of namespace Dis

#endif // !defined(_DIS_ITEM_H_)

