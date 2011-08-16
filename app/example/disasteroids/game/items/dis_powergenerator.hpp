// ///////////////////////////////////////////////////////////////////////////
// dis_powergenerator.hpp by Victor Dods, created 2005/12/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_POWERGENERATOR_HPP_)
#define _DIS_POWERGENERATOR_HPP_

#include "xrb.hpp"

#include "dis_item.hpp"

using namespace Xrb;

namespace Dis {

class PoweredDevice;
class Ship;

class PowerGenerator : public Item
{
public:

    PowerGenerator (Uint8 const upgrade_level)
        :
        Item(upgrade_level, IT_POWER_GENERATOR)
    {
        m_stored_power = ms_max_power_storage_capacity[UpgradeLevel()];
    }
    virtual ~PowerGenerator () { }

    Float StoredPower () const { return m_stored_power; }
    Float MaxPower () const { return ms_max_power_storage_capacity[UpgradeLevel()]; }

    void Drain () { m_stored_power = 0.0f; }
    void PowerDevices (
        Ship &owner_ship,
        PoweredDevice *const *powered_device,
        Float *power_allocator,
        Uint32 powered_device_count,
        bool attack_boost_is_enabled,
        bool defense_boost_is_enabled,
        Float time,
        Float frame_dt);
    // NOTE: this method is not associated with Entity!
    void Think (Float time, Float frame_dt);

private:

    static Float const ms_max_power_output_rate[UPGRADE_LEVEL_COUNT];
    static Float const ms_max_power_storage_capacity[UPGRADE_LEVEL_COUNT];

    Float m_stored_power;
}; // end of class PowerGenerator

} // end of namespace Dis

#endif // !defined(_DIS_POWERGENERATOR_HPP_)

