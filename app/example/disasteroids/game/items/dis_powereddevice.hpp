// ///////////////////////////////////////////////////////////////////////////
// dis_powereddevice.hpp by Victor Dods, created 2005/11/30
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_POWEREDDEVICE_HPP_)
#define _DIS_POWEREDDEVICE_HPP_

#include "xrb.hpp"

#include "dis_item.hpp"
#include "xrb_time.hpp"

using namespace Xrb;

namespace Dis {

class Ship;

// interface class for devices that require the ship to expend power
class PoweredDevice : public Item
{
public:

    PoweredDevice (Uint8 upgrade_level, ItemType item_type)
        :
        Item(upgrade_level, item_type)
    {
        m_owner_ship = NULL;
    }
    virtual ~PoweredDevice () { }

    // Item interface method
    virtual bool IsPoweredDevice () const { return true; }

    Ship *OwnerShip () { return m_owner_ship; }
    Ship const *OwnerShip () const { return m_owner_ship; }

    // ///////////////////////////////////////////////////////////////////////
    // public Item interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual void Equip (Ship *owner_ship)
    {
        ASSERT1(owner_ship != NULL);
        ASSERT1(m_owner_ship == NULL);
        m_owner_ship = owner_ship;
    }
    virtual void Unequip (Ship *owner_ship)
    {
        ASSERT1(owner_ship == m_owner_ship);
        ASSERT1(m_owner_ship != NULL);
        m_owner_ship = NULL;
    }

    // ///////////////////////////////////////////////////////////////////////
    // public interface methods
    // ///////////////////////////////////////////////////////////////////////

    // returns the amount of power that would be used based on the set inputs
    // (inputs set separately), and the current time and frame_dt.  this
    // method is to be used for a ship to decide how much power to apply
    // to each device it has equipped, based on how much each would draw.
    virtual Float PowerToBeUsedBasedOnInputs (bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt) const = 0;

    // activates this device using the power supplied, and returns true iff
    // the device was activated and the power used.
    virtual bool Activate (Float power, bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt) = 0;

private:

    Ship *m_owner_ship;
}; // end of class PoweredDevice

} // end of namespace Dis

#endif // !defined(_DIS_POWEREDDEVICE_HPP_)

