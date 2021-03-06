// ///////////////////////////////////////////////////////////////////////////
// dis_weapon.cpp by Victor Dods, created 2005/11/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_weapon.hpp"

#include "dis_effect.hpp"
#include "dis_enemyship.hpp"
#include "dis_explosive.hpp"
#include "dis_entity.hpp"
#include "dis_interloper.hpp"
#include "dis_revulsion.hpp"
#include "dis_shade.hpp"
#include "dis_ship.hpp"
#include "dis_spawn.hpp"
#include "dis_world.hpp"
#include "xrb_engine2_circle_physicshandler.hpp"
#include "xrb_engine2_objectlayer.hpp"
#include "xrb_engine2_world.hpp"

using namespace Xrb;

namespace Dis {

// PeaShooter properties
Float const PeaShooter::ms_primary_impact_damage[UPGRADE_LEVEL_COUNT] = { 1.5f, 4.0f, 20.0f, 50.0f };
Float const PeaShooter::ms_max_secondary_impact_damage[UPGRADE_LEVEL_COUNT] = { 80.0f, 160.0f, 320.0f, 640.0f };
Float const PeaShooter::ms_muzzle_speed[UPGRADE_LEVEL_COUNT] = { 400.0f, 500.0f, 650.0f, 800.0f };
Float const PeaShooter::ms_ballistic_size[UPGRADE_LEVEL_COUNT] = { 10.0f, 15.0f, 20.0f, 25.0f };
Float const PeaShooter::ms_range[UPGRADE_LEVEL_COUNT] = { 150.0f, 200.0f, 300.0f, 450.0f };
Float const PeaShooter::ms_required_primary_power[UPGRADE_LEVEL_COUNT] = { 7.0f, 8.0f, 9.0f, 10.0f };
Float const PeaShooter::ms_max_secondary_power_rate[UPGRADE_LEVEL_COUNT] = { 100.0f, 150.0f, 225.0f, 300.0f };
Float const PeaShooter::ms_fire_rate[UPGRADE_LEVEL_COUNT] = { 10.0f, 10.0f, 10.0f, 10.0f };
Time::Delta  const PeaShooter::ms_charge_up_time[UPGRADE_LEVEL_COUNT] = { 1.0f, 1.0f, 1.0f, 1.0f };

// Laser properties
Float const Laser::ms_primary_range[UPGRADE_LEVEL_COUNT] = { 125.0f, 150.0f, 175.0f, 200.0f };
Float const Laser::ms_secondary_range[UPGRADE_LEVEL_COUNT] = { 75.0f, 80.0f, 85.0f, 90.0f };
Float const Laser::ms_secondary_fire_rate[UPGRADE_LEVEL_COUNT] = { 3.0f, 3.0f, 3.0f, 3.0f };
Float const Laser::ms_max_primary_power_output_rate[UPGRADE_LEVEL_COUNT] = { 30.0f, 45.0f, 65.0f, 100.0f };
Float const Laser::ms_damage_rate[UPGRADE_LEVEL_COUNT] = { 70.0f, 130.0f, 190.0f, 250.0f };
Float const Laser::ms_secondary_impact_damage[UPGRADE_LEVEL_COUNT] = { 7.0f, 14.0f, 21.0f, 28.0f };
Float const Laser::ms_beam_radius[UPGRADE_LEVEL_COUNT] = { 0.0f, 0.0f, 0.0f, 0.0f };
Float const Laser::ms_beam_visible_width[UPGRADE_LEVEL_COUNT] = { 4.0f, 6.0f, 9.0f, 13.0f };

// FlameThrower properties
Float const FlameThrower::ms_muzzle_speed[UPGRADE_LEVEL_COUNT] = { 200.0f, 250.0f, 325.0f, 400.0f };
Float const FlameThrower::ms_min_required_power[UPGRADE_LEVEL_COUNT] = { 1.0f, 1.0f, 1.0f, 1.0f };
Float const FlameThrower::ms_max_required_power[UPGRADE_LEVEL_COUNT] = { 10.0f, 20.0f, 40.0f, 80.0f };
Float const FlameThrower::ms_max_damage_per_fireball[UPGRADE_LEVEL_COUNT] = { 20.0f, 35.0f, 65.0f, 130.0f };
Float const FlameThrower::ms_final_fireball_size[UPGRADE_LEVEL_COUNT] = { 40.0f, 50.0f, 70.0f, 100.0f };
Float const FlameThrower::ms_fire_rate[UPGRADE_LEVEL_COUNT] = { 10.0f, 11.0f, 12.0f, 15.0f };
Float const FlameThrower::ms_blast_mode_power_factor = 6.0f;
Float const FlameThrower::ms_blast_mode_damage_factor = 3.0f;

// GaussGun properties
Float const GaussGun::ms_impact_damage[UPGRADE_LEVEL_COUNT] = { 50.0f, 100.0f, 200.0f, 400.0f };
Float const GaussGun::ms_knockback_momentum[UPGRADE_LEVEL_COUNT] = { 5000.0f, 10000.0f, 20000.0f, 40000.0f };
Float const GaussGun::ms_range[UPGRADE_LEVEL_COUNT] = { 300.0f, 350.0f, 410.0f, 490.0f };
Float const GaussGun::ms_required_primary_power[UPGRADE_LEVEL_COUNT] = { 50.0f, 65.0f, 80.0f, 100.0f };
Float const GaussGun::ms_fire_rate[UPGRADE_LEVEL_COUNT] = { 1.5f, 1.35f, 1.15f, 1.0f };
Float const GaussGun::ms_trail_visible_width[UPGRADE_LEVEL_COUNT] = { 20.0f, 30.0f, 45.0f, 60.0f };
Time::Delta const GaussGun::ms_effect_time_to_live[UPGRADE_LEVEL_COUNT] = { 0.2f, 0.3f, 0.4f, 0.5f };
Float const GaussGun::ms_impact_particle_speed_proportion[UPGRADE_LEVEL_COUNT] = { 4.0f, 3.0f, 2.75f, 2.5f };

// GrenadeLauncher properties
Float const GrenadeLauncher::ms_muzzle_speed[UPGRADE_LEVEL_COUNT] = { 200.0f, 200.0f, 200.0f, 200.0f };
Float const GrenadeLauncher::ms_required_primary_power[UPGRADE_LEVEL_COUNT] = { 30.0f, 30.0f, 30.0f, 30.0f };
Float const GrenadeLauncher::ms_grenade_damage_to_inflict[UPGRADE_LEVEL_COUNT] = { 30.0f, 50.0f, 70.0f, 100.0f };
Float const GrenadeLauncher::ms_grenade_damage_radius[UPGRADE_LEVEL_COUNT] = { 30.0f, 30.0f, 30.0f, 30.0f };
Float const GrenadeLauncher::ms_grenade_health[UPGRADE_LEVEL_COUNT] = { 15.0f, 15.0f, 15.0f, 15.0f };
Float const GrenadeLauncher::ms_fire_rate[UPGRADE_LEVEL_COUNT] = { 7.0f, 8.0f, 9.0f, 10.0f };
Uint32 const GrenadeLauncher::ms_max_active_grenade_count[UPGRADE_LEVEL_COUNT] = { 6, 7, 8, 10 };

// MissileLauncher properties
Float const MissileLauncher::ms_primary_muzzle_speed[UPGRADE_LEVEL_COUNT] = { 200.0f, 200.0f, 200.0f, 200.0f };
Float const MissileLauncher::ms_maximum_thrust_force[UPGRADE_LEVEL_COUNT] = { 600.0f, 800.0f, 1000.0f, 1200.0f };
Float const MissileLauncher::ms_required_primary_power[UPGRADE_LEVEL_COUNT] = { 60.0f, 120.0f, 240.0f, 480.0f };
Float const MissileLauncher::ms_primary_missile_time_to_live[UPGRADE_LEVEL_COUNT] = { 30.0f, 30.0f, 30.0f, 30.0f };
Float const MissileLauncher::ms_missile_damage_amount[UPGRADE_LEVEL_COUNT] = { 150.0f, 225.0f, 400.0f, 800.0f };
Float const MissileLauncher::ms_missile_damage_radius[UPGRADE_LEVEL_COUNT] = { 20.0f, 30.0f, 40.0f, 50.0f };
Float const MissileLauncher::ms_missile_health[UPGRADE_LEVEL_COUNT] = { 15.0f, 17.0f, 20.0f, 25.0f };
Float const MissileLauncher::ms_fire_rate[UPGRADE_LEVEL_COUNT] = { 2.0f, 2.5f, 3.0f, 4.0f };
Uint32 const MissileLauncher::ms_max_active_missile_count[UPGRADE_LEVEL_COUNT] = { 3, 4, 6, 8 };

// Tractor properties
Float const Tractor::ms_range[UPGRADE_LEVEL_COUNT] = { 250.0f, 250.0f, 250.0f, 250.0f };
Float const Tractor::ms_max_power_output_rate[UPGRADE_LEVEL_COUNT] = { 30.0f, 45.0f, 65.0f, 100.0f };
Float const Tractor::ms_strength[UPGRADE_LEVEL_COUNT] = { 500.0f, 750.0f, 1000.0f, 1500.0f };
Float const Tractor::ms_max_force[UPGRADE_LEVEL_COUNT] = { 2500.0f, 3750.0f, 5000.0f, 7500.0f };
Float const Tractor::ms_beam_radius[UPGRADE_LEVEL_COUNT] = { 30.0f, 35.0f, 45.0f, 60.0f };

// ///////////////////////////////////////////////////////////////////////////
// enemy weapon properties
// ///////////////////////////////////////////////////////////////////////////

// SlowBulletGun properties
Float const SlowBulletGun::ms_impact_damage[UPGRADE_LEVEL_COUNT] = { 5.0f, 10.0f, 17.0f, 32.0f };
Float const SlowBulletGun::ms_muzzle_speed[UPGRADE_LEVEL_COUNT] = { 120.0f, 140.0f, 160.0f, 180.0f };
Float const SlowBulletGun::ms_range[UPGRADE_LEVEL_COUNT] = { 200.0f, 250.0f, 300.0f, 300.0f };
Float const SlowBulletGun::ms_required_primary_power[UPGRADE_LEVEL_COUNT] = { 7.0f, 8.0f, 9.0f, 10.0f };
Float const SlowBulletGun::ms_fire_rate[UPGRADE_LEVEL_COUNT] = { 0.333f, 0.35f, 0.4f, 0.5f };
Float const SlowBulletGun::ms_bullet_size[UPGRADE_LEVEL_COUNT] = { 3.0f, 4.0f, 5.0f, 6.0f };

// EnemySpawner properties
Float const EnemySpawner::ms_muzzle_speed[UPGRADE_LEVEL_COUNT] = { 250.0f, 250.0f, 250.0f, 250.0f };
Float const EnemySpawner::ms_required_primary_power[UPGRADE_LEVEL_COUNT] = { 10.0f, 10.0f, 10.0f, 10.0f };
Float const EnemySpawner::ms_fire_rate[UPGRADE_LEVEL_COUNT] = { 15.0f, 15.0f, 15.0f, 15.0f };

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

Float PeaShooter::PowerToBeUsedBasedOnInputs (bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt) const
{
    // can't fire faster that the weapon's cycle time
    Float fire_rate_factor = attack_boost_is_enabled ? OwnerShip()->AttackBoostFireRateFactor() : 1.0f;
    Float fire_rate = ms_fire_rate[UpgradeLevel()] * fire_rate_factor;
    ASSERT1(fire_rate > 0.0f);
    if (time < m_time_last_fired + 1.0f / fire_rate)
        return 0.0f;

    // secondary fire (charge-up) overrides primary fire
    if (SecondaryInput() > 0.0f)
    {
        ASSERT1(ms_charge_up_time[UpgradeLevel()] > 0.0f);
        return SecondaryInput() * ms_max_secondary_power_rate[UpgradeLevel()] * frame_dt / ms_charge_up_time[UpgradeLevel()] / fire_rate_factor;
    }
    // otherwise if primary fire is on at all, return full power
    else if (PrimaryInput() > 0.0f)
        return ms_required_primary_power[UpgradeLevel()] / fire_rate_factor;
    // otherwise return 0.
    else
        return 0.0f;
}

bool PeaShooter::Activate (Float power, bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt)
{
    ASSERT1(OwnerShip()->GetWorld() != NULL);
    ASSERT1(OwnerShip()->GetObjectLayer() != NULL);

    Float fire_rate_factor = attack_boost_is_enabled ? OwnerShip()->AttackBoostFireRateFactor() : 1.0f;
    Float damage_factor = attack_boost_is_enabled ? OwnerShip()->AttackBoostDamageFactor() : 1.0f;

    // determine if the secondary fire was released, indicating the
    // charge-up weapon should fire.
    if (SecondaryInput() == 0.0f && m_charge_up_ratio > 0.0f)
    {
        ASSERT1(m_charge_up_ratio >= 0.0f);
        ASSERT1(m_charge_up_ratio <= 1.0f);

        // calculate the ballistic size and impact damage
        Float ballistic_size =
            ms_ballistic_size[UpgradeLevel()] * (m_charge_up_ratio + 1.0f);
        Float parameter = m_charge_up_ratio * m_charge_up_ratio;
        Float impact_damage = Math::LinearlyInterpolate(ms_primary_impact_damage[UpgradeLevel()], ms_max_secondary_impact_damage[UpgradeLevel()], 0.0f, 1.0f, parameter);
        // spawn it
        SpawnSmartBallistic(
            OwnerShip()->GetObjectLayer(),
            time,
            MuzzleLocation() + ballistic_size * MuzzleDirection(),
            ballistic_size,
            1.0f,
            1.5f * ms_muzzle_speed[UpgradeLevel()] * MuzzleDirection() + OwnerShip()->Velocity(),
            impact_damage * damage_factor,
            ms_range[UpgradeLevel()] / ms_muzzle_speed[UpgradeLevel()],
            time,
            UpgradeLevel(),
            OwnerShip()->GetReference());

        // reset the charge-up ratio
        m_charge_up_ratio = 0.0f;
        // update the last time fired
        m_time_last_fired = time;
        // we didn't actually use any power here, because the weapon
        // charges up beforehand.  therefore, return false.
        return false;
    }
    // secondary fire (charge-up) overrides primary fire
    else if (SecondaryInput() > 0.0f && power > 0.0f)
    {
        ASSERT1(power <=
                SecondaryInput() * ms_max_secondary_power_rate[UpgradeLevel()] * frame_dt /
                ms_charge_up_time[UpgradeLevel()] / fire_rate_factor);

        // if completely charged up, don't use up the power
        if (m_charge_up_ratio == 1.0f)
            return false;

        // increment the charge up ratio at the given rate
        m_charge_up_ratio += power * fire_rate_factor / ms_max_secondary_power_rate[UpgradeLevel()];
        if (m_charge_up_ratio > 1.0f)
            m_charge_up_ratio = 1.0f;

        // power was used up, so return true.
        return true;
    }
    // primary fire
    else
    {
        ASSERT1(power <= ms_required_primary_power[UpgradeLevel()] / fire_rate_factor);

        // can't fire if not enough power was supplied
        if (power < ms_required_primary_power[UpgradeLevel()] / fire_rate_factor)
            return false;

        ASSERT1(PrimaryInput() > 0.0f);

        // fire the weapon -- create a Pea and set its position and velocity
        ASSERT1(ms_muzzle_speed[UpgradeLevel()] > 0.0f);
        SpawnSmartBallistic(
            OwnerShip()->GetObjectLayer(),
            time,
            MuzzleLocation() + ms_ballistic_size[UpgradeLevel()] * MuzzleDirection(),
            ms_ballistic_size[UpgradeLevel()],
            1.0f,
            ms_muzzle_speed[UpgradeLevel()] * MuzzleDirection() + OwnerShip()->Velocity(),
            ms_primary_impact_damage[UpgradeLevel()] * damage_factor,
            ms_range[UpgradeLevel()] / ms_muzzle_speed[UpgradeLevel()],
            time,
            UpgradeLevel(),
            OwnerShip()->GetReference());

        // update the last time fired
        m_time_last_fired = time;

        return true;
    }
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

Float Laser::PowerToBeUsedBasedOnInputs (bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt) const
{
    ASSERT1(PrimaryInput() <= 1.0f);
    ASSERT1(SecondaryInput() <= 1.0f);

    if (PrimaryInput() > 0.0f)
        return PrimaryInput() * frame_dt * ms_max_primary_power_output_rate[UpgradeLevel()];
    else
        return 0.0f;
}

bool Laser::Activate (Float power, bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt)
{
    ASSERT1(m_laser_beam != NULL);
    ASSERT1(m_laser_beam->IsInWorld());
    ASSERT1(m_laser_impact_effect != NULL);
    ASSERT1(m_laser_impact_effect->IsInWorld());

    Float damage_factor = attack_boost_is_enabled ? OwnerShip()->AttackBoostDamageFactor() : 1.0f;

    // secondary fire can happen in parallel with primary
    Float fire_rate_factor = attack_boost_is_enabled ? OwnerShip()->AttackBoostFireRateFactor() : 1.0f;
    Float secondary_fire_rate = ms_secondary_fire_rate[UpgradeLevel()] * fire_rate_factor;
    ASSERT1(secondary_fire_rate > 0.0f);
    if (SecondaryInput() > 0.0f &&
        time >= m_time_last_fired + 1.0f / secondary_fire_rate)
    {
        Engine2::Circle::AreaTraceList area_trace_list;
        ASSERT1(OwnerShip()->GetObjectLayer() != NULL);
        OwnerShip()->GetPhysicsHandler()->AreaTrace(
            *OwnerShip()->GetObjectLayer(),
            OwnerShip()->Translation(),
            ms_secondary_range[UpgradeLevel()] + OwnerShip()->PhysicalRadius(),
            false,
            area_trace_list);

        Mortal *best_target = NULL;
        Sint32 best_target_priority = 0;
        for (Engine2::Circle::AreaTraceList::iterator it = area_trace_list.begin(),
                                                      it_end = area_trace_list.end();
             it != it_end;
             ++it)
        {
            Entity *entity = DStaticCast<Entity *>(*it);
            ASSERT1(entity != NULL);

            // we don't want to shoot ourselves
            if (entity == OwnerShip())
                continue;

            // only target enemy ships or explosives
            if (entity->IsEnemyShip() || entity->IsExplosive())
            {
                Mortal *potential_target = DStaticCast<Mortal *>(entity);
                Sint32 potential_target_priority = potential_target->TargetPriority();
                if (best_target == NULL ||
                    potential_target_priority > best_target_priority)
                {
                    best_target = potential_target;
                    best_target_priority = potential_target_priority;
                }
            }
        }

        // only attempt to fire if we found a target
        if (best_target != NULL)
        {
            FloatVector2 fire_vector(OwnerShip()->GetObjectLayer()->AdjustedDifference(best_target->Translation(), OwnerShip()->Translation()));
            FloatVector2 fire_location(
                OwnerShip()->Translation() +
                    OwnerShip()->PhysicalRadius() *
                    fire_vector.Normalization());

            if (fire_vector.Length() > ms_secondary_range[UpgradeLevel()] + OwnerShip()->PhysicalRadius())
            {
                fire_vector.Normalize();
                fire_vector *= ms_secondary_range[UpgradeLevel()] + OwnerShip()->PhysicalRadius();
            }

            // do a line trace
            Engine2::Circle::LineTraceBindingSet line_trace_binding_set;
            ASSERT1(OwnerShip()->GetObjectLayer() != NULL);
            OwnerShip()->GetPhysicsHandler()->LineTrace(
                *OwnerShip()->GetObjectLayer(),
                fire_location,
                fire_vector,
                0.0f,
                false,
                line_trace_binding_set);

            Engine2::Circle::LineTraceBindingSet::iterator it = line_trace_binding_set.begin();
            Engine2::Circle::LineTraceBindingSet::iterator it_end = line_trace_binding_set.end();
            // don't damage the owner of this weapon or powerups
            while (it != it_end &&
                   (DStaticCast<Entity *>(&it->m_entity)->IsPowerup() ||
                    DStaticCast<Entity *>(&it->m_entity)->IsBallistic() ||
                    DStaticCast<Entity *>(&it->m_entity) == OwnerShip()))
            {
                ++it;
            }

            // only fire at ships and explosives
            if (it != it_end && (DStaticCast<Entity *>(&it->m_entity)->IsShip() || DStaticCast<Entity *>(&it->m_entity)->IsExplosive()))
            {
                DStaticCast<Mortal *>(&it->m_entity)->Damage(
                    OwnerShip(),
                    NULL, // laser does not have an Entity medium
                    ms_secondary_impact_damage[UpgradeLevel()] * damage_factor,
                    NULL,
                    fire_location + it->m_clamped_trace_hit_parameter * fire_vector,
                    MuzzleDirection(),
                    0.0f,
                    Mortal::D_COMBAT_LASER,
                    time,
                    frame_dt);
                // spawn the "gauss gun trail"
                SpawnGaussGunTrail(
                    OwnerShip()->GetObjectLayer(),
                    "fs://laser_beam.png",
                    time,
                    fire_location,
                    fire_vector,
                    OwnerShip()->Velocity(),
                    ms_beam_visible_width[UpgradeLevel()],
                    it->m_clamped_trace_hit_parameter * fire_vector.Length(),
                    1,
                    0.5f,
                    time);
                // update the last time fired
                m_time_last_fired = time;
            }
        }
    }

    // primary constant beam firing mode
    if (PrimaryInput() > 0.0f && power > 0.0f)
    {
        ASSERT1(power <= PrimaryInput() * frame_dt * ms_max_primary_power_output_rate[UpgradeLevel()]);

        Engine2::Circle::LineTraceBindingSet line_trace_binding_set;
        ASSERT1(OwnerShip()->GetObjectLayer() != NULL);
        OwnerShip()->GetPhysicsHandler()->LineTrace(
            *OwnerShip()->GetObjectLayer(),
            MuzzleLocation(),
            ms_primary_range[UpgradeLevel()] * MuzzleDirection(),
            ms_beam_radius[UpgradeLevel()],
            false,
            line_trace_binding_set);

        Engine2::Circle::LineTraceBindingSet::iterator it = line_trace_binding_set.begin();
        Engine2::Circle::LineTraceBindingSet::iterator it_end = line_trace_binding_set.end();

        // we don't want to hit the owner of the weapon or non-mortals, so just skip them.
        while (it != it_end && (&it->m_entity == OwnerShip() || !DStaticCast<Entity *>(&it->m_entity)->IsMortal()))
            ++it;

        FloatVector2 laser_beam_hit_location(
            MuzzleLocation() + ms_primary_range[UpgradeLevel()] * MuzzleDirection());
        // damage the next thing if it exists
        if (it != it_end)
        {
            Float ratio_of_max_power_output =
                power / (frame_dt * ms_max_primary_power_output_rate[UpgradeLevel()]);
            laser_beam_hit_location =
                MuzzleLocation() + it->m_clamped_trace_hit_parameter * ms_primary_range[UpgradeLevel()] * MuzzleDirection();
            if (DStaticCast<Entity *>(&it->m_entity)->IsMortal())
            {
                DStaticCast<Mortal *>(&it->m_entity)->Damage(
                    OwnerShip(),
                    NULL, // laser does not have a Entity medium
                    ms_damage_rate[UpgradeLevel()] * damage_factor * ratio_of_max_power_output * frame_dt,
                    NULL, // we don't care how much damage was taken
                    laser_beam_hit_location,
                    MuzzleDirection(),
                    0.0f,
                    Mortal::D_MINING_LASER,
                    time,
                    frame_dt);
            }
        }

        // place the laser beam effect
        Float intensity = power / (frame_dt * ms_max_primary_power_output_rate[UpgradeLevel()]);
        ASSERT1(intensity >= 0.0f && intensity <= 1.0f);
        m_laser_beam->SetIntensity(intensity);
        m_laser_beam->SnapToShip(
            MuzzleLocation() + frame_dt * OwnerShip()->Velocity(),
            laser_beam_hit_location + frame_dt * OwnerShip()->Velocity(),
            ms_beam_visible_width[UpgradeLevel()]);

        // place the laser impact effect
        m_laser_impact_effect->SnapToLocationAndSetScaleFactor(laser_beam_hit_location, 3.0f*ms_beam_visible_width[UpgradeLevel()]);
        // if there was an impact, set the laser impact effect to opaque.  otherwise transparent.
        m_laser_impact_effect->OwnerObject()->ColorMask() = (it != it_end) ? Color::ms_opaque_white : Color::ms_transparent_white;

        // the weapon fired successfully
        return true;
    }
    else
    {
        // turn off the constant fire laser
        m_laser_beam->SetIntensity(0.0f);
        // turn off the laser impact effect
        m_laser_impact_effect->OwnerObject()->ColorMask() = Color::ms_transparent_white;
        // the weapon was not fired
        return false;
    }
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

Float FlameThrower::PowerToBeUsedBasedOnInputs (bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt) const
{
    // can't fire faster that the weapon's cycle time
    Float fire_rate_factor = attack_boost_is_enabled ? OwnerShip()->AttackBoostFireRateFactor() : 1.0f;
    Float fire_rate = ms_fire_rate[UpgradeLevel()] * fire_rate_factor;
    ASSERT1(fire_rate > 0.0f);
    if (time < m_time_last_fired + 1.0f / fire_rate)
        return 0.0f;

    // return a power proportional to the primary input required power,
    // or 0 if its less than the minimum required power.

    // secondary fire (super-blast) overrides primary fire
    Float input = PrimaryInput();
    Float factor = 1.0f;
    if (SecondaryInput() > 0.0f)
    {
        input = SecondaryInput();
        factor = ms_blast_mode_power_factor;
    }

    Float scaled_power = input * factor * ms_max_required_power[UpgradeLevel()];
    if (scaled_power < factor * ms_min_required_power[UpgradeLevel()])
        return 0.0f;
    else
        return scaled_power / fire_rate_factor;
}

bool FlameThrower::Activate (Float power, bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt)
{
    // secondary fire (super-blast) overrides primary fire
    Float input = PrimaryInput();
    Float power_factor = 1.0f;
    Float damage_factor = 1.0f;

    if (SecondaryInput() > 0.0f)
    {
        input = SecondaryInput();
        power_factor = ms_blast_mode_power_factor;
        damage_factor = ms_blast_mode_damage_factor;
    }

    Float boost_damage_factor = attack_boost_is_enabled ? OwnerShip()->AttackBoostDamageFactor() : 1.0f;

    Float fire_rate_factor = attack_boost_is_enabled ? OwnerShip()->AttackBoostFireRateFactor() : 1.0f;
    Float min_required_power = power_factor * ms_min_required_power[UpgradeLevel()] / fire_rate_factor;
    Float max_required_power = power_factor * ms_max_required_power[UpgradeLevel()] / fire_rate_factor;

    ASSERT1(power <= max_required_power);

    // can't fire faster that the weapon's cycle time
    Float fire_rate = ms_fire_rate[UpgradeLevel()] * fire_rate_factor;
    ASSERT1(fire_rate > 0.0f);
    if (time < m_time_last_fired + 1.0f / fire_rate)
        return false;

    // don't fire if not enough power was supplied
    if (power < min_required_power)
        return false;

    ASSERT1(power >= 0.0f);
    ASSERT1(input > 0.0f);

    Float max_damage_per_fireball =
        damage_factor * boost_damage_factor *
            (IsMaxDamagePerFireballOverridden() ?
             m_max_damage_per_fireball_override :
             ms_max_damage_per_fireball[UpgradeLevel()]);
    ASSERT1(max_damage_per_fireball > 0.0f);

    Float final_fireball_size =
        damage_factor *
            (IsFinalFireballSizeOverridden() ?
            m_final_fireball_size_override :
            ms_final_fireball_size[UpgradeLevel()]);
    ASSERT1(final_fireball_size > 0.0f);

    // fire the weapon
    SpawnFireball(
        OwnerShip()->GetObjectLayer(),
        time,
        MuzzleLocation() + 2.0f * MuzzleDirection(), // the extra is just so we don't fry ourselves
        ms_muzzle_speed[UpgradeLevel()] * MuzzleDirection() + OwnerShip()->Velocity(),
        power / max_required_power * max_damage_per_fireball,
        max_damage_per_fireball,
        0.0f, // initial_size
        final_fireball_size,
        1.0f,
        time,
        OwnerShip()->GetReference());

    // update the last time fired
    m_time_last_fired = time;

    // the weapon fired successfully
    return true;
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

// void GaussGun::EnsureReticleEffectIsCleared ()
// {
//     if (m_reticle_effect.IsValid() && m_reticle_effect->IsInWorld())
//         m_reticle_effect->RemoveFromWorld();
// }

Float GaussGun::PowerToBeUsedBasedOnInputs (bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt) const
{
    // can't fire faster that the weapon's cycle time
    Float fire_rate_factor = attack_boost_is_enabled ? OwnerShip()->AttackBoostFireRateFactor() : 1.0f;
    Float fire_rate = ms_fire_rate[UpgradeLevel()] * fire_rate_factor;
    ASSERT1(fire_rate > 0.0f);
    if (time < m_time_last_fired + 1.0f / fire_rate)
        return 0.0f;

    // if the primary input is on at all, return the full primary power
    return (PrimaryInput() > 0.0f) ? ms_required_primary_power[UpgradeLevel()] / fire_rate_factor : 0.0f;
}

bool GaussGun::Activate (Float power, bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt)
{
    Float fire_rate_factor = attack_boost_is_enabled ? OwnerShip()->AttackBoostFireRateFactor() : 1.0f;
    ASSERT1(power <= ms_required_primary_power[UpgradeLevel()] / fire_rate_factor);

    // can't fire if not enough power was supplied
    if (power < ms_required_primary_power[UpgradeLevel()] / fire_rate_factor)
        return false;

    ASSERT1(PrimaryInput() > 0.0f);

    // fire the weapon -- do a trace and spawn the GaussGunTrail
    ASSERT1(OwnerShip()->GetWorld() != NULL);
    ASSERT1(OwnerShip()->GetObjectLayer() != NULL);

    // do a line trace
    Engine2::Circle::LineTraceBindingSet line_trace_binding_set;
    ASSERT1(OwnerShip()->GetObjectLayer() != NULL);
    OwnerShip()->GetPhysicsHandler()->LineTrace(
        *OwnerShip()->GetObjectLayer(),
        MuzzleLocation(),
        ms_range[UpgradeLevel()] * MuzzleDirection(),
        0.0f,
        false,
        line_trace_binding_set);

    Engine2::Circle::LineTraceBindingSet::iterator it = line_trace_binding_set.begin();
    Engine2::Circle::LineTraceBindingSet::iterator it_end = line_trace_binding_set.end();

    static Float const s_impact_particle_spread_angle = 70.0f;
    static Uint32 const s_impact_particle_count = 8;
    // calculate the seed radius for splash impact effects
    Float seed_radius = 0.1f * ms_trail_visible_width[UpgradeLevel()];

    Float damage_factor = attack_boost_is_enabled ? OwnerShip()->AttackBoostDamageFactor() : 1.0f;

    // decide how much damage to inflict total
    Float total_damage_to_inflict =
        IsImpactDamageOverridden() ?
        ImpactDamageOverride() :
        damage_factor * ms_impact_damage[UpgradeLevel()];
    Float damage_left_to_inflict = total_damage_to_inflict;

    // damage the next thing if it exists
    Float furthest_hit_parameter = 1.0f;
    Float damage_amount_used;
    bool first_hit_registered = false;
    while ((!first_hit_registered || damage_left_to_inflict > 0.0f) && it != it_end)
    {
        // we don't want to hit the owner of this weapon or powerups
        // (continue without updating the furthest hit time)
        if (&it->m_entity == OwnerShip() || DStaticCast<Entity *>(&it->m_entity)->IsPowerup())
        {
            ++it;
            continue;
        }

        first_hit_registered = true;

        furthest_hit_parameter = it->m_clamped_trace_hit_parameter;
        if (DStaticCast<Entity *>(&it->m_entity)->IsMortal())
        {
            FloatVector2 impact_location(MuzzleLocation() + it->m_clamped_trace_hit_parameter * ms_range[UpgradeLevel()] * MuzzleDirection());
            FloatVector2 impact_normal(OwnerShip()->GetObjectLayer()->AdjustedDifference(impact_location, it->m_entity.Translation()));
            if (impact_normal.LengthSquared() > 0.0001f)
                impact_normal.Normalize();
            else
                impact_normal = FloatVector2(1.0f, 0.0f); // arbitrary unit vector

            // add knockback momentum to the target (BEFORE damaging it)
            // TODO: possibly add a cap on the added speed (hitting a small asteroid
            // with the best gauss gun is hilarious otherwise)
            it->m_entity.AccumulateMomentum(-damage_factor * ms_knockback_momentum[UpgradeLevel()] * impact_normal);

            DStaticCast<Mortal *>(&it->m_entity)->Damage(
                OwnerShip(),
                NULL, // gauss gun does not have a Entity medium
                damage_left_to_inflict,
                &damage_amount_used,
                impact_location,
                MuzzleDirection(),
                0.0f,
                Mortal::D_BALLISTIC,
                time,
                frame_dt);

            // spawn an impact effect at the impact site
            SpawnSplashImpactEffect(
                OwnerShip()->GetObjectLayer(),
                "fs://plasma_ball_green.png",
                time,
                impact_location,
                impact_normal,
                it->m_entity.Velocity(),
                Math::RandomFloat(-10.0f, 10.0f),           // seed angle
                damage_amount_used / total_damage_to_inflict * seed_radius,
                s_impact_particle_count,
                s_impact_particle_spread_angle,
                ms_effect_time_to_live[UpgradeLevel()],
                ms_impact_particle_speed_proportion[UpgradeLevel()]);

            ASSERT1(damage_amount_used <= damage_left_to_inflict + 0.0001f);
            damage_left_to_inflict -= damage_amount_used;
        }

        furthest_hit_parameter = it->m_clamped_trace_hit_parameter;

        ++it;
    }

    if (damage_left_to_inflict > 0.0001f) // to within some tolerance
        furthest_hit_parameter = 1.0f;

    // figure out how long each gauss gun trail segment should be
    ASSERT1(furthest_hit_parameter >= 0.0f);
    Float segment_width = ms_trail_visible_width[UpgradeLevel()];
    Float continuum_segment_count = Math::Ceiling((furthest_hit_parameter * ms_range[UpgradeLevel()]) / segment_width);
    ASSERT1(continuum_segment_count >= 0.0f);
    Uint32 segment_count = Uint32(continuum_segment_count);
    Float segment_length = segment_count > 0 ?
                           furthest_hit_parameter * ms_range[UpgradeLevel()] / segment_count :
                           0.0f;

    // spawn an impact effect at the weapon muzzle
    SpawnSplashImpactEffect(
        OwnerShip()->GetObjectLayer(),
        "fs://plasma_ball_green.png",
        time,
        MuzzleLocation(),
        MuzzleDirection(),
        OwnerShip()->Velocity(),
        Math::RandomFloat(-10.0f, 10.0f),           // seed angle
        seed_radius,
        s_impact_particle_count,
        s_impact_particle_spread_angle,
        ms_effect_time_to_live[UpgradeLevel()],
        ms_impact_particle_speed_proportion[UpgradeLevel()]);

    // spawn the gauss gun trail
    SpawnGaussGunTrail(
        OwnerShip()->GetObjectLayer(),
        "fs://gauss_gun_trail.png",
        time,
        MuzzleLocation(),
        MuzzleDirection(),
        OwnerShip()->Velocity(),
        segment_width,
        segment_length,
        segment_count,
        ms_effect_time_to_live[UpgradeLevel()],
        time);

    // add knockback to the firer (AFTER spawning the gauss gun trail, so the
    // trail is not affected by this impulse)
    OwnerShip()->AccumulateMomentum(-ms_knockback_momentum[UpgradeLevel()] * damage_factor * MuzzleDirection());

    // update the last time fired
    m_time_last_fired = time;

    // the weapon fired successfully
    return true;
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

GrenadeLauncher::~GrenadeLauncher ()
{
    for (ActiveGrenadeSet::iterator it = m_active_grenade_set.begin(),
                                    it_end = m_active_grenade_set.end();
         it != it_end;
         ++it)
    {
        Grenade *active_grenade = *it;
        active_grenade->SetOwnerGrenadeLauncher(NULL);
    }
    m_active_grenade_set.clear();
}

void GrenadeLauncher::ActiveGrenadeDestroyed (Grenade &active_grenade)
{
    ASSERT1(active_grenade.OwnerGrenadeLauncher() == this);
    ASSERT1(ActiveGrenadeCount() > 0);

    // delete the active grenade from the active grenade set
    ActiveGrenadeSet::iterator it = m_active_grenade_set.find(&active_grenade);
    ASSERT1(it != m_active_grenade_set.end());
    m_active_grenade_set.erase(it);

    active_grenade.SetOwnerGrenadeLauncher(NULL);
}

Float GrenadeLauncher::PowerToBeUsedBasedOnInputs (bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt) const
{
    // can't fire if the maximum number of active grenades has been reached
    if (ActiveGrenadeCount() >= ms_max_active_grenade_count[UpgradeLevel()])
        return 0.0f;

    // can't fire faster that the weapon's cycle time
    Float fire_rate_factor = attack_boost_is_enabled ? OwnerShip()->AttackBoostFireRateFactor() : 1.0f;
    Float fire_rate = ms_fire_rate[UpgradeLevel()] * fire_rate_factor;
    ASSERT1(fire_rate > 0.0f);
    if (time < m_time_last_fired + 1.0f / fire_rate)
        return 0.0f;

    // if the primary input is on at all, return the full primary power
    return (PrimaryInput() > 0.0f) ? ms_required_primary_power[UpgradeLevel()] / fire_rate_factor : 0.0f;
}

bool GrenadeLauncher::Activate (Float power, bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt)
{
    Float fire_rate_factor = attack_boost_is_enabled ? OwnerShip()->AttackBoostFireRateFactor() : 1.0f;
    ASSERT1(power <= ms_required_primary_power[UpgradeLevel()] / fire_rate_factor);

    // since the secondary fire takes no power, we have to check the inputs
    if (PrimaryInput() == 0.0f && SecondaryInput() == 0.0f)
        return false;

    // you can fire secondary at any time, it doesn't use any power
    // and doesn't inhibit the primary fire at all.
    if (SecondaryInput() > 0.0f && ActiveGrenadeCount() > 0)
    {
        // detonate all the grenades (this isn't a for loop because detonating
        // the grenades will erase iterators from this set).
        while (!m_active_grenade_set.empty())
        {
            Grenade *active_grenade = *m_active_grenade_set.begin();
            ASSERT1(active_grenade != NULL);
            DEBUG1_CODE(Uint32 active_grenade_set_size = m_active_grenade_set.size());
            active_grenade->Detonate(time, frame_dt);
            ASSERT1(m_active_grenade_set.size() == active_grenade_set_size - 1);
        }
    }

    // can't fire primary if not enough power was supplied
    if (power < ms_required_primary_power[UpgradeLevel()] / fire_rate_factor)
        return false;

    ASSERT1(PrimaryInput() > 0.0f);
    ASSERT1(ActiveGrenadeCount() < ms_max_active_grenade_count[UpgradeLevel()]);

    // fire the weapon -- spawn a Grenade
    ASSERT1(OwnerShip()->GetWorld() != NULL);
    ASSERT1(OwnerShip()->GetObjectLayer() != NULL);

    Float damage_factor = attack_boost_is_enabled ? OwnerShip()->AttackBoostDamageFactor() : 1.0f;

    Float const grenade_scale_factor = 4.0f;
    Grenade *grenade = SpawnGrenade(
        OwnerShip()->GetObjectLayer(),
        time,
        MuzzleLocation() + grenade_scale_factor * MuzzleDirection(),
        grenade_scale_factor,
        ms_muzzle_speed[UpgradeLevel()] * MuzzleDirection() + OwnerShip()->Velocity(),
        this,
        ms_grenade_damage_to_inflict[UpgradeLevel()] * damage_factor,
        ms_grenade_damage_radius[UpgradeLevel()],
        2.0f * ms_grenade_damage_radius[UpgradeLevel()],
        UpgradeLevel(),
        OwnerShip()->GetReference(),
        ms_grenade_health[UpgradeLevel()]);

    // add the grenade to the active grenade set
    ASSERT1(grenade != NULL);
    m_active_grenade_set.insert(grenade);

    // update the last time fired
    m_time_last_fired = time;

    // the weapon fired successfully
    return true;
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

MissileLauncher::~MissileLauncher ()
{
    for (ActiveMissileSet::iterator it = m_active_missile_set.begin(),
                                    it_end = m_active_missile_set.end();
         it != it_end;
         ++it)
    {
        Missile *active_missile = *it;
        active_missile->SetOwnerMissileLauncher(NULL);
    }
    m_active_missile_set.clear();
}

void MissileLauncher::ActiveMissileDestroyed (Missile *active_missile)
{
    ASSERT1(active_missile != NULL);
    ASSERT1(active_missile->OwnerMissileLauncher() == this);
    ASSERT1(ActiveMissileCount() > 0);

    // delete the active missile from the active missile set
    ActiveMissileSet::iterator it = m_active_missile_set.find(active_missile);
    ASSERT1(it != m_active_missile_set.end());
    m_active_missile_set.erase(it);

    active_missile->SetOwnerMissileLauncher(NULL);
}

Float MissileLauncher::PowerToBeUsedBasedOnInputs (bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt) const
{
    // can't fire if the maximum number of active missiles has been reached
    if (ActiveMissileCount() >= ms_max_active_missile_count[UpgradeLevel()])
        return 0.0f;

    // can't fire faster that the weapon's cycle time
    Float fire_rate_factor = attack_boost_is_enabled ? OwnerShip()->AttackBoostFireRateFactor() : 1.0f;
    Float fire_rate = ms_fire_rate[UpgradeLevel()] * fire_rate_factor;
    ASSERT1(fire_rate > 0.0f);
    if (time < m_time_last_fired + 1.0f / fire_rate)
        return 0.0f;

    // secondary fire takes no power, so primary fire is all that matters
    if (PrimaryInput() > 0.0f)
        return ms_required_primary_power[UpgradeLevel()] / fire_rate_factor;
    else
        return 0.0f;
}

bool MissileLauncher::Activate (Float power, bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt)
{
    DEBUG1_CODE(Float fire_rate_factor = attack_boost_is_enabled ? OwnerShip()->AttackBoostFireRateFactor() : 1.0f);
    ASSERT1(power <= ms_required_primary_power[UpgradeLevel()] / fire_rate_factor);

    // it's possible to fire primary and secondary simultaneously
    if (SecondaryInput() > 0.0f)
    {
        // set each active missile on an intercept course with the player's
        // mouse cursor (poor mouse cursor!)
        for (ActiveMissileSet::iterator it = m_active_missile_set.begin(),
                                        it_end = m_active_missile_set.end();
             it != it_end;
             ++it)
        {
            Missile *active_missile = *it;
            ASSERT1(active_missile != NULL);
            ASSERT1(active_missile->OwnerMissileLauncher() == this);
            // maybe we could detect what enemy the ship is aiming at, and then
            // use that entity as a target (see Entity::ApplyInterceptCourseAcceleration)
            active_missile->ApplyInterceptCourseAcceleration(
                ReticleCoordinates(),
                FloatVector2::ms_zero, // velocity
                FloatVector2::ms_zero, // accel
                ms_maximum_thrust_force[UpgradeLevel()],
                false,                 // apply_force_on_target_also
                false);                // reverse_thrust
        }
    }

    if (PrimaryInput() > 0.0f &&
        power >= ms_required_primary_power[UpgradeLevel()])
    {
        // fire the weapon -- spawn a Missile
        ASSERT1(ActiveMissileCount() < ms_max_active_missile_count[UpgradeLevel()]);
        ASSERT1(OwnerShip()->GetWorld() != NULL);
        ASSERT1(OwnerShip()->GetObjectLayer() != NULL);

        Float damage_factor = attack_boost_is_enabled ? OwnerShip()->AttackBoostDamageFactor() : 1.0f;
        Float const missile_scale_factor = 7.0f;
        Missile *missile = SpawnMissile(
            OwnerShip()->GetObjectLayer(),
            time,
            MuzzleLocation() + missile_scale_factor * MuzzleDirection(),
            missile_scale_factor,
            Math::Arg(MuzzleDirection()),
            ms_primary_muzzle_speed[UpgradeLevel()] * MuzzleDirection() + OwnerShip()->Velocity(),
            this,
            ms_primary_missile_time_to_live[UpgradeLevel()],
            time,
            ms_missile_damage_amount[UpgradeLevel()] * damage_factor,
            ms_missile_damage_radius[UpgradeLevel()],
            2.0f * ms_missile_damage_radius[UpgradeLevel()],
            UpgradeLevel(),
            OwnerShip()->GetReference(),
            ms_missile_health[UpgradeLevel()],
            m_spawn_enemy_missiles);

        // add the missile to the active missile set
        ASSERT1(missile != NULL);
        m_active_missile_set.insert(missile);

        // update the last time fired
        m_time_last_fired = time;
        // the weapon fired successfully
        return true;
    }

    // the weapon did not fire
    return false;
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

Float Tractor::PowerToBeUsedBasedOnInputs (bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt) const
{
    if (SecondaryInput() > 0.0f)
        return SecondaryInput() * frame_dt * ms_max_power_output_rate[UpgradeLevel()];
    else
        return PrimaryInput() * frame_dt * ms_max_power_output_rate[UpgradeLevel()];
}

bool Tractor::Activate (Float power, bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt)
{
    // the epsilon is because floating point arithmetic isn't exact
    // and the first condition was sometimes failing.
    ASSERT1(power <= PowerToBeUsedBasedOnInputs(attack_boost_is_enabled, defense_boost_is_enabled, time, frame_dt) + 0.001f);
    ASSERT1(m_tractor_beam != NULL);
    ASSERT1(m_tractor_beam->IsInWorld());

    // don't do anything if no power was supplied
    if (power == 0.0f)
    {
        m_tractor_beam->SetParameters(Color::ms_transparent_black, 0.0f);
        return false;
    }

    ASSERT1(PrimaryInput() > 0.0f || SecondaryInput() > 0.0f);
    // the primary tractor beam pulls everything
    Float push_instead_of_pull = SecondaryInput() > 0.0f;
    Float input = push_instead_of_pull ? SecondaryInput() : PrimaryInput();

    Float range =
        IsRangeOverridden() ?
        RangeOverride() :
        ms_range[UpgradeLevel()];
    Float strength =
        IsStrengthOverridden() ?
        StrengthOverride() :
        ms_strength[UpgradeLevel()];
    Float max_force =
        IsMaxForceOverridden() ?
        MaxForceOverride() :
        ms_max_force[UpgradeLevel()];
    Float beam_radius =
        IsBeamRadiusOverridden() ?
        BeamRadiusOverride() :
        ms_beam_radius[UpgradeLevel()];

    // use the reticle coordinates for the area the tractor pulls/pushes,
    // but make sure it's inside the maximum range.
    FloatVector2 reticle_coordinates(ReticleCoordinates());
    Float reticle_distance = (reticle_coordinates - OwnerShip()->Translation()).Length();
    if (reticle_distance > range)
    {
        ASSERT1(reticle_distance > 0.0f);
        reticle_coordinates =
            range / reticle_distance *
            (reticle_coordinates - MuzzleLocation()) +
            MuzzleLocation();
    }

    Engine2::Circle::AreaTraceList area_trace_list;
    ASSERT1(OwnerShip()->GetObjectLayer() != NULL);
    OwnerShip()->GetPhysicsHandler()->AreaTrace(
        *OwnerShip()->GetObjectLayer(),
        reticle_coordinates,
        beam_radius,
        false,
        area_trace_list);

    Polynomial::SolutionSet solution_set;
    for (Engine2::Circle::AreaTraceList::iterator it = area_trace_list.begin(),
                                                  it_end = area_trace_list.end();
         it != it_end;
         ++it)
    {
        Entity *entity = DStaticCast<Entity *>(*it);
        ASSERT1(entity != NULL);

        // we don't want to pull ourselves (it would cancel out anyway)
        if (entity == OwnerShip())
            continue;

        Float force = Min(input * strength * entity->PhysicalRadius(), max_force);
        entity->ApplyInterceptCourseAcceleration(
            OwnerShip(),
            force,
            true,
            push_instead_of_pull,
            &solution_set);
    }

    // place the tractor beam effect
    m_tractor_beam->SetScaleFactor(beam_radius);
    m_tractor_beam->SetTranslation(reticle_coordinates + frame_dt * OwnerShip()->Velocity());
    // set its pulling input and intensity
    Float intensity = power / (frame_dt * ms_max_power_output_rate[UpgradeLevel()]);
    ASSERT1(intensity >= 0.0f && intensity <= 1.0f);
    m_tractor_beam->SetParameters(push_instead_of_pull ? Color(1.0f, 0.0f, 0.0f) : Color(0.0f, 1.0f, 0.0f), intensity);

    return true;
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

bool AdvancedTractor::Activate (Float power, bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt)
{
    if (PrimaryInput() <= 0.0f || SecondaryInput() <= 0.0f)
        return Tractor::Activate(power, attack_boost_is_enabled, defense_boost_is_enabled, time, frame_dt);

    // if both primary and secondary inputs are on, engage advanced mode.
    // the epsilon is because floating point arithmetic isn't exact
    // and the first condition was sometimes failing.
    ASSERT1(power <= PowerToBeUsedBasedOnInputs(attack_boost_is_enabled, defense_boost_is_enabled, time, frame_dt) + 0.001f);

    // don't do anything if no power was supplied
    if (power == 0.0f)
    {
        m_tractor_beam->SetParameters(Color::ms_transparent_black, 0.0f);
        return false;
    }

    // the primary tractor beam pulls everything
    Float input = Max(SecondaryInput(), PrimaryInput());

    Float range =
        IsRangeOverridden() ?
        RangeOverride() :
        ms_range[UpgradeLevel()];
    Float strength =
        IsStrengthOverridden() ?
        StrengthOverride() :
        ms_strength[UpgradeLevel()];
    Float max_force =
        IsMaxForceOverridden() ?
        MaxForceOverride() :
        ms_max_force[UpgradeLevel()];
    Float beam_radius =
        IsBeamRadiusOverridden() ?
        BeamRadiusOverride() :
        ms_beam_radius[UpgradeLevel()];

    // use the reticle coordinates for the area the tractor pulls/pushes,
    // but make sure it's inside the maximum range.
    FloatVector2 reticle_coordinates(ReticleCoordinates());
    Float reticle_distance = (reticle_coordinates - OwnerShip()->Translation()).Length();
    if (reticle_distance > range)
    {
        ASSERT1(reticle_distance > 0.0f);
        reticle_coordinates =
            range / reticle_distance *
            (reticle_coordinates - MuzzleLocation()) +
            MuzzleLocation();
    }

    Engine2::Circle::AreaTraceList area_trace_list;
    ASSERT1(OwnerShip()->GetObjectLayer() != NULL);
    OwnerShip()->GetPhysicsHandler()->AreaTrace(
        *OwnerShip()->GetObjectLayer(),
        reticle_coordinates,
        beam_radius,
        false,
        area_trace_list);

    Polynomial::SolutionSet solution_set;
    for (Engine2::Circle::AreaTraceList::iterator it = area_trace_list.begin(),
                                                  it_end = area_trace_list.end();
         it != it_end;
         ++it)
    {
        Entity *entity = DStaticCast<Entity *>(*it);
        ASSERT1(entity != NULL);

        // we don't want to push ourselves (it would cancel out anyway)
        // we don't want to push the target entity
        if (entity == OwnerShip() || entity == m_target)
            continue;

        // apply the force on the entity and the owner ship
        Float force = Min(input * strength * entity->PhysicalRadius(), max_force);
        entity->ApplyInterceptCourseAcceleration(
            m_target,
            force,
            false, // apply_force_on_target_also
            false, // reverse_thrust
            &solution_set);
        OwnerShip()->ApplyInterceptCourseAcceleration(
            m_target,
            force,
            false, // apply_force_on_target_also
            true, // reverse_thrust
            &solution_set);
    }

    // place the tractor beam effect
    m_tractor_beam->SetScaleFactor(beam_radius);
    m_tractor_beam->SetTranslation(reticle_coordinates + frame_dt * OwnerShip()->Velocity());
    // set its pulling input and intensity
    Float intensity = power / (frame_dt * ms_max_power_output_rate[UpgradeLevel()]);
    ASSERT1(intensity >= 0.0f && intensity <= 1.0f);
    m_tractor_beam->SetParameters(Color(1.0f, 1.0f, 0.0f), intensity);

    return true;
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

Float SlowBulletGun::PowerToBeUsedBasedOnInputs (bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt) const
{
    // can't fire faster that the weapon's cycle time
    Float fire_rate_factor = attack_boost_is_enabled ? OwnerShip()->AttackBoostFireRateFactor() : 1.0f;
    Float fire_rate = FireRate() * fire_rate_factor;
    ASSERT1(fire_rate > 0.0f);
    if (time < m_time_last_fired + 1.0f / fire_rate)
        return 0.0f;

    // if the primary input is on at all, return the full primary power
    return (PrimaryInput() > 0.0f) ? ms_required_primary_power[UpgradeLevel()] / fire_rate_factor : 0.0f;
}

bool SlowBulletGun::Activate (Float power, bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt)
{
    Float fire_rate_factor = attack_boost_is_enabled ? OwnerShip()->AttackBoostFireRateFactor() : 1.0f;
    ASSERT1(power <= ms_required_primary_power[UpgradeLevel()] / fire_rate_factor);

    // can't fire if not enough power was supplied
    if (power < ms_required_primary_power[UpgradeLevel()] / fire_rate_factor)
        return false;

    ASSERT1(PrimaryInput() > 0.0f);

    Float damage_factor = attack_boost_is_enabled ? OwnerShip()->AttackBoostDamageFactor() : 1.0f;

    Float range = IsRangeOverridden() ? m_range_override : ms_range[UpgradeLevel()];
    
    // fire the weapon -- create a Pea and set its position and velocity
    ASSERT1(OwnerShip()->GetWorld() != NULL);
    ASSERT1(OwnerShip()->GetObjectLayer() != NULL);
    ASSERT1(ms_muzzle_speed[UpgradeLevel()] > 0.0f);
    SpawnDumbBallistic(
        OwnerShip()->GetObjectLayer(),
        time,
        MuzzleLocation() + ms_bullet_size[UpgradeLevel()] * MuzzleDirection(),
        ms_bullet_size[UpgradeLevel()],
        3.0f,
        ms_muzzle_speed[UpgradeLevel()] * MuzzleDirection() + OwnerShip()->Velocity(),
        ms_impact_damage[UpgradeLevel()] * damage_factor,
        range / ms_muzzle_speed[UpgradeLevel()],
        time,
        UpgradeLevel(),
        OwnerShip()->GetReference());

    // update the last time fired
    m_time_last_fired = time;

    return true;
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

Float EnemySpawner::PowerToBeUsedBasedOnInputs (bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt) const
{
    Float fire_rate_factor = attack_boost_is_enabled ? OwnerShip()->AttackBoostFireRateFactor() : 1.0f;
    Float const fire_rate =
        IsFireRateOverridden() ?
        FireRateOverride() :
        ms_fire_rate[UpgradeLevel()] * fire_rate_factor;

    // can't fire faster that the weapon's cycle time
    ASSERT1(fire_rate > 0.0f);
    if (time < m_time_last_fired + 1.0f / fire_rate)
        return 0.0f;

    // if the primary input is on at all, return the full primary power
    return (PrimaryInput() > 0.0f) ? ms_required_primary_power[UpgradeLevel()] / fire_rate_factor : 0.0f;
}

bool EnemySpawner::Activate (Float power, bool attack_boost_is_enabled, bool defense_boost_is_enabled, Time time, Time::Delta frame_dt)
{
    Float fire_rate_factor = attack_boost_is_enabled ? OwnerShip()->AttackBoostFireRateFactor() : 1.0f;
    ASSERT1(power <= ms_required_primary_power[UpgradeLevel()] / fire_rate_factor);

    // can't fire if not enough power was supplied
    if (power < ms_required_primary_power[UpgradeLevel()] / fire_rate_factor)
        return false;

    ASSERT1(PrimaryInput() > 0.0f);

    // fire the weapon -- spawn the specified type/level of enemy
    ASSERT1(ms_muzzle_speed[UpgradeLevel()] > 0.0f);
    FloatVector2 muzzle_velocity(
        ms_muzzle_speed[UpgradeLevel()] * MuzzleDirection() +
        OwnerShip()->Velocity());

    Uint8 spawn_enemy_level = Math::RandomUint16(0, UpgradeLevel());
    EnemyShip *enemy_ship =
        SpawnEnemyShip(
            OwnerShip()->GetObjectLayer(),
            time,
            FloatVector2::ms_zero,
            muzzle_velocity,
            EnemySpawnType(),
            spawn_enemy_level);
    enemy_ship->SetTranslation(MuzzleLocation() + enemy_ship->PhysicalRadius() * MuzzleDirection());

    // update the last time fired
    m_time_last_fired = time;

    return true;
}

} // end of namespace Dis
