// ///////////////////////////////////////////////////////////////////////////
// dis_powergenerator.cpp by Victor Dods, created 2005/12/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_powergenerator.hpp"

#include "dis_powereddevice.hpp"
#include "dis_ship.hpp"

using namespace Xrb;

namespace Dis {

Float const PowerGenerator::ms_max_power_output_rate[UPGRADE_LEVEL_COUNT] = { 100.0f, 150.0f, 250.0f, 600.0f };
Float const PowerGenerator::ms_max_power_storage_capacity[UPGRADE_LEVEL_COUNT] = { 150.0f, 300.0f, 500.0f, 1000.0f };

void PowerGenerator::PowerDevices (
    Ship &owner_ship,
    PoweredDevice *const *powered_device,
    Float *power_allocator,
    Uint32 powered_device_count,
    bool attack_boost_is_enabled,
    bool defense_boost_is_enabled,
    Time time,
    Time::Delta frame_dt)
{
    ASSERT1(powered_device != NULL);
    ASSERT1(power_allocator != NULL);

    Float power_boost_factor = defense_boost_is_enabled ? owner_ship.DefenseBoostPowerFactor() : 1.0f;
    Float total_required_power = 0.0f;
    for (Uint32 i = 0; i < powered_device_count; ++i)
    {
        if (powered_device[i] != NULL)
        {
            power_allocator[i] = powered_device[i]->PowerToBeUsedBasedOnInputs(attack_boost_is_enabled, defense_boost_is_enabled, time, frame_dt);
            total_required_power += power_allocator[i];
        }
        else
        {
            power_allocator[i] = 0.0f;
        }
    }

    // account for defense boost power
    total_required_power /= power_boost_factor;

    if (total_required_power > 0.0f)
    {
        Float available_power_ratio = Min(m_stored_power, total_required_power) / total_required_power;
        // we never want to supply too much power to devices (or many many
        // asserts will be triggered)
        ASSERT1(available_power_ratio <= 1.0f);
        for (Uint32 i = 0; i < powered_device_count; ++i)
            power_allocator[i] *= available_power_ratio;
    }

    for (Uint32 i = 0; i < powered_device_count; ++i)
    {
        if (powered_device[i] != NULL &&
            powered_device[i]->Activate(power_allocator[i], attack_boost_is_enabled, defense_boost_is_enabled, time, frame_dt))
        {
            m_stored_power -= power_allocator[i] / power_boost_factor;
        }
    }

    // the slightly negative value is because floating point arithmetic
    // isn't exact and the assert was sometimes failing.  even though we
    // limit the stored power to >= 0, the assert is to check that the
    // amounts of power actually used were valid.
    ASSERT1(m_stored_power >= -0.001f);
    if (m_stored_power < 0.0f)
        m_stored_power = 0.0f;
}

void PowerGenerator::Think (Time time, Time::Delta frame_dt)
{
    m_stored_power += frame_dt * ms_max_power_output_rate[UpgradeLevel()];
    if (m_stored_power > ms_max_power_storage_capacity[UpgradeLevel()])
        m_stored_power = ms_max_power_storage_capacity[UpgradeLevel()];
}

} // end of namespace Dis
