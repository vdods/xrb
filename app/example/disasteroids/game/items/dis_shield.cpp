// ///////////////////////////////////////////////////////////////////////////
// dis_shield.cpp by Victor Dods, created 2005/12/03
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_shield.hpp"

#include "dis_ship.hpp"

using namespace Xrb;

namespace Dis {

Float const Shield::ms_power_consumption_rate[UPGRADE_LEVEL_COUNT] = { 10.0f, 15.0f, 22.0f, 30.0f };
Float const Shield::ms_max_charged_power[UPGRADE_LEVEL_COUNT] = { 30.0f, 60.0f, 90.0f, 120.0f };
Float const Shield::ms_max_damage_dissipation[UPGRADE_LEVEL_COUNT] = { 4.0f, 8.0f, 16.0f, 32.0f };
Time::Delta const Shield::ms_recharge_interval[UPGRADE_LEVEL_COUNT] = { 3.0f, 2.5f, 2.0f, 1.5f };

Float Shield::Damage (
    Entity *damager,
    Entity *damage_medium,
    Float damage_amount,
    Float *damage_amount_used,
    FloatVector2 const &damage_location,
    FloatVector2 const &damage_normal,
    Float damage_force,
    Mortal::DamageType damage_type,
    Time time,
    Time::Delta frame_dt)
{
    ASSERT1(damage_amount >= 0.0f);
    ASSERT1(m_charged_power >= 0.0f);
    ASSERT1(m_charged_power <= ms_max_charged_power[UpgradeLevel()]);

    Float available_damage_dissipation = AvailableDamageDissipation();
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
        ms_max_charged_power[UpgradeLevel()] *
        Min(damage_amount, available_damage_dissipation) /
        ms_max_damage_dissipation[UpgradeLevel()];
    // check against the available power
    Float power_to_use = Min(power_required, m_charged_power);
    // calculate the amount of damage that was actually blocked given the
    // amount of power to use
    Float damage_actually_blocked =
        Min(damage_amount, available_damage_dissipation) * power_to_use / power_required;
    ASSERT1(damage_actually_blocked >= 0.0f);
    // +0.001f due to floating point inaccuracies (this assert was being triggered because of it)
    ASSERT1(damage_actually_blocked <= damage_amount + 0.001f);
    // calculate the amount of damage the ship should take
    Float adjusted_damage_amount = damage_amount - damage_actually_blocked;
    // subtract the power to use from the charged power
    m_charged_power -= power_to_use;
    ASSERT1(m_charged_power >= 0.0f);

    if (damage_amount_used != NULL)
        *damage_amount_used = damage_actually_blocked;
    // return the adjusted damage amount (this is the amount the ship should take)
    return adjusted_damage_amount;
}

Float Shield::PowerToBeUsedBasedOnInputs (bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt) const
{
    Float max_power_that_can_be_recharged =
        frame_dt * ms_max_charged_power[UpgradeLevel()] /
        ms_recharge_interval[UpgradeLevel()];
    Float power_remaining_to_recharge =
        ms_max_charged_power[UpgradeLevel()] - m_charged_power;
    return
        Min(max_power_that_can_be_recharged, power_remaining_to_recharge) +
        frame_dt * ms_power_consumption_rate[UpgradeLevel()];
}

bool Shield::Activate (Float power, bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt)
{
    ASSERT1(power >= 0.0f);
    ASSERT1(power <= PowerToBeUsedBasedOnInputs(attack_boost_is_enabled, defense_boost_is_enabled, time, frame_dt) + 0.001f);
    ASSERT1(power <= ms_max_charged_power[UpgradeLevel()] /
                     ms_recharge_interval[UpgradeLevel()] +
                     frame_dt * ms_power_consumption_rate[UpgradeLevel()]);

    power -= frame_dt * ms_power_consumption_rate[UpgradeLevel()];
    Float multiplier = defense_boost_is_enabled ? OwnerShip()->DefenseBoostShieldFactor() : 1.0f;
    m_charged_power += multiplier * power;
    // because the shield takes power to keep it on, m_charged_power can
    // decrease by not receiving enough power.  make sure that m_charged_power
    // doesn't drop below 0.  TODO: i think this is false, check this.
    if (m_charged_power < 0.0f)
        m_charged_power = 0.0;
    if (m_charged_power > ms_max_charged_power[UpgradeLevel()])
        m_charged_power = ms_max_charged_power[UpgradeLevel()];

    return true;
}

} // end of namespace Dis
