// ///////////////////////////////////////////////////////////////////////////
// dis_powereddevice.h by Victor Dods, created 2005/11/30
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_POWEREDDEVICE_H_)
#define _DIS_POWEREDDEVICE_H_

#include "xrb.h"

#include "dis_item.h"

using namespace Xrb;

namespace Dis
{

class Ship;

// interface class for devices that require the ship to expend power
class PoweredDevice : public Item
{
public:

    PoweredDevice (
        Uint8 const upgrade_level,
        ItemType const item_type)
        :
        Item(upgrade_level, item_type)
    {
        m_owner_ship = NULL;
    }
    virtual ~PoweredDevice () { }

    // Item interface method
    virtual bool GetIsPoweredDevice () const { return true; }

    inline Ship *GetOwnerShip () { return m_owner_ship; }

    // ///////////////////////////////////////////////////////////////////////
    // public Item interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual void Equip (Ship *owner_ship)
    {
        ASSERT1(owner_ship != NULL)
        ASSERT1(m_owner_ship == NULL)
        m_owner_ship = owner_ship;
    }
    virtual void Unequip (Ship *owner_ship)
    {
        ASSERT1(owner_ship == m_owner_ship)
        ASSERT1(m_owner_ship != NULL)
        m_owner_ship = NULL;
    }
    
    // ///////////////////////////////////////////////////////////////////////
    // public interface methods
    // ///////////////////////////////////////////////////////////////////////
        
    // returns the amount of power that would be used based on the set inputs
    // (inputs set separately), and the current time and frame_dt.  this
    // method is to be used for a ship to decide how much power to apply
    // to each device it has equipped, based on how much each would draw.
    virtual Float GetPowerToBeUsedBasedOnInputs (
        Float time,
        Float frame_dt) const = 0;

    // activates this device using the power supplied, and returns true iff
    // the device was activated and the power used.
    virtual bool Activate (
        Float power,
        Float time,
        Float frame_dt) = 0;

private:

    Ship *m_owner_ship;
}; // end of class PoweredDevice

} // end of namespace Dis

#endif // !defined(_DIS_POWEREDDEVICE_H_)

