// ///////////////////////////////////////////////////////////////////////////
// dis_shield.hpp by Victor Dods, created 2005/12/03
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_SHIELD_HPP_)
#define _DIS_SHIELD_HPP_

#include "xrb.hpp"

#include "dis_entity.hpp"
#include "dis_mortal.hpp"
#include "dis_powereddevice.hpp"
#include "xrb_vector.hpp"

using namespace Xrb;

namespace Dis {

// level 0: plain old shields
// level 1: strong against collision damage
// level 2: damages enemies (also protects against collision damage)
// level 3: strong against explosion damage (also damages enemies and strong
//          against collision damage)
class Shield : public PoweredDevice
{
public:

    Shield (Uint8 upgrade_level)
        :
        PoweredDevice(upgrade_level, IT_SHIELD)
    {
        m_charged_power = 0.0f;
    }
    virtual ~Shield () { }

    inline Float Intensity () const
    {
        ASSERT1(m_charged_power >= 0.0f);
        ASSERT1(m_charged_power <= ms_max_charged_power[UpgradeLevel()]);
        ASSERT1(ms_max_charged_power[UpgradeLevel()] > 0.0f);
        return Math::Sqrt(m_charged_power / ms_max_charged_power[UpgradeLevel()]);
    }

    // takes damage for the ship, using power, and returns the amount of
    // unblocked damage that the ship should take.
    Float Damage (
        Entity *damager,
        Entity *damage_medium,
        Float damage_amount,
        Float *damage_amount_used,
        FloatVector2 const &damage_location,
        FloatVector2 const &damage_normal,
        Float damage_force,
        Mortal::DamageType damage_type,
        Time time,
        Time::Delta frame_dt);
    // this is used when the shield is hit by an EMP explosion
    void Drain ()
    {
        m_charged_power = 0.0f;
    }

    // returns the amount of power that would be used based on the set inputs
    // (inputs set separately), and the current time and frame_dt.  this
    // method is to be used for a ship to decide how much power to apply
    // to each device it has equipped, based on how much each would draw.
    virtual Float PowerToBeUsedBasedOnInputs (bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt) const;

    // activates this device using the power supplied, and returns true iff
    // the device was activated and the power used.
    virtual bool Activate (Float power, bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt);

private:

    Float AvailableDamageDissipation () const
    {
        return
            ms_max_damage_dissipation[UpgradeLevel()] *
            m_charged_power /
            ms_max_charged_power[UpgradeLevel()];
    }

    static Float const ms_power_consumption_rate[UPGRADE_LEVEL_COUNT];
    static Float const ms_max_charged_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_max_damage_dissipation[UPGRADE_LEVEL_COUNT];
    static Time::Delta const ms_recharge_interval[UPGRADE_LEVEL_COUNT];

    Float m_charged_power;
}; // end of class Shield

} // end of namespace Dis

#endif // !defined(_DIS_SHIELD_HPP_)

