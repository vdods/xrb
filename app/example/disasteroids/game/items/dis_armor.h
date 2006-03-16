// ///////////////////////////////////////////////////////////////////////////
// dis_armor.h by Victor Dods, created 2005/12/03
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_ARMOR_H_)
#define _DIS_ARMOR_H_

#include "xrb.h"

#include "dis_item.h"

using namespace Xrb;

namespace Dis
{

// this would be a PoweredDevice, but it doesn't use power.
class Armor : public Item
{
public:

    Armor (Uint8 const upgrade_level)
        :
        Item(upgrade_level, IT_ARMOR)
    { }
    virtual ~Armor () { }

    inline Float GetDamageDissipationRate () const
    {
        ASSERT1(GetUpgradeLevel() < UPGRADE_LEVEL_COUNT)
        return ms_damage_dissipation_rate[GetUpgradeLevel()];
    }
    inline Float GetFirstMoment () const
    {
        ASSERT1(GetUpgradeLevel() < UPGRADE_LEVEL_COUNT)
        return ms_first_moment[GetUpgradeLevel()];        
    }
    
private:

    static Float const ms_damage_dissipation_rate[UPGRADE_LEVEL_COUNT];
    static Float const ms_first_moment[UPGRADE_LEVEL_COUNT];
}; // end of class Armor

} // end of namespace Dis

#endif // !defined(_DIS_ARMOR_H_)

