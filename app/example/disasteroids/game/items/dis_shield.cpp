// ///////////////////////////////////////////////////////////////////////////
// dis_shield.cpp by Victor Dods, created 2005/12/03
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_shield.h"

using namespace Xrb;

namespace Dis
{

Float const Shield::ms_power_consumption_rate[UPGRADE_LEVEL_COUNT] = { 10.0f, 15.0f, 22.0f, 30.0f };
Float const Shield::ms_max_charged_power[UPGRADE_LEVEL_COUNT] = { 40.0f, 45.0f, 60.0f, 80.0f };
Float const Shield::ms_max_damage_dissipation[UPGRADE_LEVEL_COUNT] = { 8.0f, 12.0f, 20.0f, 30.0f };
Float const Shield::ms_recharge_interval[UPGRADE_LEVEL_COUNT] = { 3.0f, 2.5f, 2.0f, 1.5f };

Float Shield::Damage (
    Entity *const damager,
    Entity *const damage_medium,
    Float const damage_amount,
    Float *const damage_amount_used,
    FloatVector2 const &damage_location,
    FloatVector2 const &damage_normal,
    Float const damage_force,
    Mortal::DamageType const damage_type,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(damage_amount >= 0.0f)
    ASSERT1(m_charged_power >= 0.0f)
    ASSERT1(m_charged_power <= ms_max_charged_power[GetUpgradeLevel()])

    Float available_damage_dissipation = GetAvailableDamageDissipation();
    // if there is no dissipation available, just return the full amount
    // of damage
    if (available_damage_dissipation == 0.0f || damage_amount == 0.0f)
    {
        if (damage_amount_used != NULL)
            *damage_amount_used = 0.0f;
        return damage_amount;
    }
    // calculate how much power to use to block this amount of damage
    Float power_required =
        ms_max_charged_power[GetUpgradeLevel()] *
        Min(damage_amount, available_damage_dissipation) /
        ms_max_damage_dissipation[GetUpgradeLevel()];
    // check against the available power
    Float power_to_use = Min(power_required, m_charged_power);
    // calculate the amount of damage that was actually blocked given the
    // amount of power to use
    Float damage_actually_blocked =
        Min(damage_amount, available_damage_dissipation) * power_to_use / power_required;
    ASSERT1(damage_actually_blocked >= 0.0f)
    ASSERT1(damage_actually_blocked <= damage_amount)
    // calculate the amount of damage the ship should take
    Float adjusted_damage_amount = damage_amount - damage_actually_blocked;
    // subtract the power to use from the charged power
    m_charged_power -= power_to_use;
    ASSERT1(m_charged_power >= 0.0f)

    if (damage_amount_used != NULL)
        *damage_amount_used = damage_actually_blocked;   
    // return the adjusted damage amount (this is the amount the ship should take)
    return adjusted_damage_amount;
}

Float Shield::GetPowerToBeUsedBasedOnInputs (
    Float time,
    Float frame_dt) const
{
    Float max_power_that_can_be_recharged =
        frame_dt * ms_max_charged_power[GetUpgradeLevel()] /
        ms_recharge_interval[GetUpgradeLevel()];
    Float power_remaining_to_recharge =
        ms_max_charged_power[GetUpgradeLevel()] - m_charged_power;
    return
        Min(max_power_that_can_be_recharged, power_remaining_to_recharge) +
        frame_dt * ms_power_consumption_rate[GetUpgradeLevel()];
}

bool Shield::Activate (
    Float power,
    Float time,
    Float frame_dt)
{
    ASSERT1(power >= 0.0f)
    ASSERT1(power <= GetPowerToBeUsedBasedOnInputs(time, frame_dt) + 0.001f)
    ASSERT1(power <= ms_max_charged_power[GetUpgradeLevel()] /
                     ms_recharge_interval[GetUpgradeLevel()] +
                     frame_dt * ms_power_consumption_rate[GetUpgradeLevel()])

    power -= frame_dt * ms_power_consumption_rate[GetUpgradeLevel()];
    m_charged_power += power;
    // because the shield takes power to keep it on, m_charged_power can
    // decrease by not receiving enough power.  make sure that m_charged_power
    // doesn't drop below 0.
    if (m_charged_power < 0.0f)
        m_charged_power = 0.0;
    ASSERT1(m_charged_power <= ms_max_charged_power[GetUpgradeLevel()])

    return true;
}

} // end of namespace Dis
