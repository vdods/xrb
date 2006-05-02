// ///////////////////////////////////////////////////////////////////////////
// dis_weapon.cpp by Victor Dods, created 2005/11/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_weapon.h"

#include "dis_effect.h"
#include "dis_enemyship.h"
#include "dis_explosive.h"
#include "dis_entity.h"
#include "dis_linetracebinding.h"
#include "dis_physicshandler.h"
#include "dis_ship.h"
#include "dis_spawn.h"
#include "dis_world.h"
#include "xrb_engine2_objectlayer.h"
#include "xrb_engine2_world.h"

using namespace Xrb;

namespace Dis
{

// PeaShooter properties
Float const PeaShooter::ms_primary_impact_damage[UPGRADE_LEVEL_COUNT] = { 1.5f, 3.0f, 6.0f, 10.0f };
Float const PeaShooter::ms_max_secondary_impact_damage[UPGRADE_LEVEL_COUNT] = { 40.0f, 60.0f, 90.0f, 150.0f };
Float const PeaShooter::ms_muzzle_speed[UPGRADE_LEVEL_COUNT] = { 400.0f, 500.0f, 650.0f, 800.0f };
Float const PeaShooter::ms_ballistic_size[UPGRADE_LEVEL_COUNT] = { 10.0f, 12.0f, 14.0f, 16.0f };
Float const PeaShooter::ms_range[UPGRADE_LEVEL_COUNT] = { 150.0f, 200.0f, 300.0f, 450.0f };
Float const PeaShooter::ms_required_primary_power[UPGRADE_LEVEL_COUNT] = { 7.0f, 8.0f, 9.0f, 10.0f };
Float const PeaShooter::ms_max_secondary_power_rate[UPGRADE_LEVEL_COUNT] = { 100.0f, 150.0f, 225.0f, 300.0f };
Float const PeaShooter::ms_fire_rate[UPGRADE_LEVEL_COUNT] = { 10.0f, 10.0f, 10.0f, 10.0f };
Float const PeaShooter::ms_charge_up_time[UPGRADE_LEVEL_COUNT] = { 1.0f, 1.0f, 1.0f, 1.0f };

// Laser properties
Float const Laser::ms_primary_range[UPGRADE_LEVEL_COUNT] = { 125.0f, 150.0f, 185.0f, 250.0f };
Float const Laser::ms_secondary_range[UPGRADE_LEVEL_COUNT] = { 75.0f, 100.0f, 125.0f, 150.0f };
Float const Laser::ms_secondary_fire_rate[UPGRADE_LEVEL_COUNT] = { 2.0f, 2.0f, 2.0f, 2.0f };
Float const Laser::ms_max_primary_power_output_rate[UPGRADE_LEVEL_COUNT] = { 30.0f, 45.0f, 65.0f, 100.0f };
Float const Laser::ms_damage_rate[UPGRADE_LEVEL_COUNT] = { 60.0f, 90.0f, 120.0f, 200.0f };
Float const Laser::ms_secondary_impact_damage[UPGRADE_LEVEL_COUNT] = { 6.0f, 12.0f, 24.0f, 48.0f };
Float const Laser::ms_beam_radius[UPGRADE_LEVEL_COUNT] = { 0.0f, 0.0f, 0.0f, 0.0f };

// FlameThrower properties
Float const FlameThrower::ms_muzzle_speed[UPGRADE_LEVEL_COUNT] = { 200.0f, 250.0f, 325.0f, 400.0f };
Float const FlameThrower::ms_min_required_primary_power[UPGRADE_LEVEL_COUNT] = { 1.0f, 1.0f, 1.0f, 1.0f };
Float const FlameThrower::ms_max_required_primary_power[UPGRADE_LEVEL_COUNT] = { 10.0f, 15.0f, 22.0f, 35.0f };
Float const FlameThrower::ms_max_damage_per_fireball[UPGRADE_LEVEL_COUNT] = { 15.0f, 30.0f, 50.0f, 80.0f };
Float const FlameThrower::ms_final_fireball_size[UPGRADE_LEVEL_COUNT] = { 40.0f, 50.0f, 70.0f, 100.0f };
Float const FlameThrower::ms_fire_rate[UPGRADE_LEVEL_COUNT] = { 10.0f, 11.0f, 12.0f, 15.0f };

// GaussGun properties
Float const GaussGun::ms_impact_damage[UPGRADE_LEVEL_COUNT] = { 50.0f, 100.0f, 200.0f, 400.0f };
Float const GaussGun::ms_range[UPGRADE_LEVEL_COUNT] = { 350.0f, 400.0f, 450.0f, 500.0f };
Float const GaussGun::ms_required_primary_power[UPGRADE_LEVEL_COUNT] = { 50.0f, 65.0f, 80.0f, 100.0f };
Float const GaussGun::ms_fire_rate[UPGRADE_LEVEL_COUNT] = { 1.333f, 1.8333f, 2.333f, 2.8333f };

// GrenadeLauncher properties
Float const GrenadeLauncher::ms_muzzle_speed[UPGRADE_LEVEL_COUNT] = { 300.0f, 300.0f, 300.0f, 300.0f };
Float const GrenadeLauncher::ms_required_primary_power[UPGRADE_LEVEL_COUNT] = { 30.0f, 30.0f, 30.0f, 30.0f };
Float const GrenadeLauncher::ms_grenade_damage_to_inflict[UPGRADE_LEVEL_COUNT] = { 30.0f, 60.0f, 90.0f, 120.0f };
Float const GrenadeLauncher::ms_grenade_damage_radius[UPGRADE_LEVEL_COUNT] = { 40.0f, 45.0f, 50.0f, 60.0f };
Float const GrenadeLauncher::ms_grenade_health[UPGRADE_LEVEL_COUNT] = { 15.0f, 15.0f, 15.0f, 15.0f };
Float const GrenadeLauncher::ms_fire_rate[UPGRADE_LEVEL_COUNT] = { 4.0f, 5.0f, 7.0f, 10.0f };
Uint32 const GrenadeLauncher::ms_max_active_grenade_count[UPGRADE_LEVEL_COUNT] = { 6, 7, 8, 10 };

// MineLayer properties
Float const MineLayer::ms_muzzle_speed[UPGRADE_LEVEL_COUNT] = { 30.0f, 30.0f, 30.0f, 30.0f };
Float const MineLayer::ms_required_primary_power[UPGRADE_LEVEL_COUNT] = { 50.0f, 50.0f, 50.0f, 50.0f };
Float const MineLayer::ms_mine_damage_to_inflict[UPGRADE_LEVEL_COUNT] = { 200.0f, 200.0f, 200.0f, 200.0f };
Float const MineLayer::ms_mine_damage_radius[UPGRADE_LEVEL_COUNT] = { 100.0f, 100.0f, 100.0f, 100.0f };
Float const MineLayer::ms_mine_health[UPGRADE_LEVEL_COUNT] = { 100.0f, 100.0f, 100.0f, 100.0f };
Float const MineLayer::ms_fire_rate[UPGRADE_LEVEL_COUNT] = { 0.5f, 0.45f, 0.425f, 0.4f };
Uint32 const MineLayer::ms_max_active_mine_count[UPGRADE_LEVEL_COUNT] = { 2, 3, 4, 6 };

// MissileLauncher properties
Float const MissileLauncher::ms_muzzle_speed[UPGRADE_LEVEL_COUNT] = { 200.0f, 250.0f, 300.0f, 350.0f };
Float const MissileLauncher::ms_required_primary_power[UPGRADE_LEVEL_COUNT] = { 50.0f, 60.0f, 70.0f, 80.0f };
Float const MissileLauncher::ms_required_secondary_power[UPGRADE_LEVEL_COUNT] = { 70.0f, 80.0f, 90.0f, 100.0f };
Float const MissileLauncher::ms_missile_time_to_live[UPGRADE_LEVEL_COUNT] = { 2.0f, 1.9f, 1.8f, 1.5f };
Float const MissileLauncher::ms_missile_damage_amount[UPGRADE_LEVEL_COUNT] = { 40.0f, 50.0f, 60.0f, 70.0f };
Float const MissileLauncher::ms_missile_damage_radius[UPGRADE_LEVEL_COUNT] = { 70.0f, 75.0f, 80.0f, 85.0f };
Float const MissileLauncher::ms_missile_health[UPGRADE_LEVEL_COUNT] = { 30.0f, 30.0f, 30.0f, 30.0f };
Float const MissileLauncher::ms_fire_rate[UPGRADE_LEVEL_COUNT] = { 4.0f, 5.0f, 6.5f, 8.0f };

// EMPCore properties
Float const EMPCore::ms_required_primary_power[UPGRADE_LEVEL_COUNT] = { 60.0f, 80.0f, 130.0f, 200.0f };
Float const EMPCore::ms_emp_bomb_disable_time_factor[UPGRADE_LEVEL_COUNT] = { 30.0f, 30.0f, 30.0f, 30.0f };
Float const EMPCore::ms_emp_bomb_blast_radius[UPGRADE_LEVEL_COUNT] = { 225.0f, 300.0f, 400.0f, 550.0f };
Float const EMPCore::ms_fire_rate[UPGRADE_LEVEL_COUNT] = { 0.5f, 0.333f, 0.2f, 0.1666f };
/*
// EMPBombLayer properties
Float const EMPBombLayer::ms_muzzle_speed[UPGRADE_LEVEL_COUNT] = { 300.0f, 300.0f, 300.0f, 300.0f };
Float const EMPBombLayer::ms_required_primary_power[UPGRADE_LEVEL_COUNT] = { 60.0f, 70.0f, 75.0f, 80.0f };
Float const EMPBombLayer::ms_emp_bomb_disable_time_factor[UPGRADE_LEVEL_COUNT] = { 6.0f, 5.0f, 4.0f, 3.0f };
Float const EMPBombLayer::ms_emp_bomb_blast_radius[UPGRADE_LEVEL_COUNT] = { 100.0f, 175.0f, 250.0f, 300.0f };
Float const EMPBombLayer::ms_emp_bomb_health[UPGRADE_LEVEL_COUNT] = { 60.0f, 60.0f, 60.0f, 60.0f };
Float const EMPBombLayer::ms_fire_rate[UPGRADE_LEVEL_COUNT] = { 0.5f, 0.6f, 0.7f, 0.8f };
Uint32 const EMPBombLayer::ms_max_active_emp_bomb_count[UPGRADE_LEVEL_COUNT] = { 1, 1, 2, 4 };
*/

// AutoDestruct properties
Float const AutoDestruct::ms_trigger_countdown_time[UPGRADE_LEVEL_COUNT] = { 5.0f, 4.0f, 3.25f, 2.5f };
Float const AutoDestruct::ms_damage_amount[UPGRADE_LEVEL_COUNT] = { 80.0f, 150.0f, 300.0f, 500.0f };
Float const AutoDestruct::ms_damage_radius[UPGRADE_LEVEL_COUNT] = { 50.0f, 80.0f, 120.0f, 200.0f };

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
Float const SlowBulletGun::ms_impact_damage[UPGRADE_LEVEL_COUNT] = { 6.0f, 12.0f, 24.0f, 48.0f };
Float const SlowBulletGun::ms_muzzle_speed[UPGRADE_LEVEL_COUNT] = { 120.0f, 140.0f, 160.0f, 180.0f };
Float const SlowBulletGun::ms_range[UPGRADE_LEVEL_COUNT] = { 200.0f, 250.0f, 300.0f, 300.0f };
Float const SlowBulletGun::ms_required_primary_power[UPGRADE_LEVEL_COUNT] = { 7.0f, 8.0f, 9.0f, 10.0f };
Float const SlowBulletGun::ms_fire_rate[UPGRADE_LEVEL_COUNT] = { 0.333f, 0.35f, 0.4f, 0.5f };

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

Float PeaShooter::GetPowerToBeUsedBasedOnInputs (
    Float const time,
    Float const frame_dt) const
{
    // can't fire faster that the weapon's cycle time
    ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f)
    if (time < m_time_last_fired + 1.0f / ms_fire_rate[GetUpgradeLevel()])
        return 0.0f;

    // secondary fire (charge-up) overrides primary fire
    if (GetSecondaryInput() > 0.0f)
    {
        ASSERT1(ms_charge_up_time[GetUpgradeLevel()] > 0.0f)
        return GetSecondaryInput() * ms_max_secondary_power_rate[GetUpgradeLevel()] * frame_dt /
               ms_charge_up_time[GetUpgradeLevel()];
    }
    // otherwise if primary fire is on at all, return full power
    else if (GetPrimaryInput() > 0.0f)
        return ms_required_primary_power[GetUpgradeLevel()];
    // otherwise return 0.
    else
        return 0.0f;
}

bool PeaShooter::Activate (
    Float const power,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(GetOwnerShip()->GetWorld() != NULL)
    ASSERT1(GetOwnerShip()->GetObjectLayer() != NULL)

    // determine if the secondary fire was released, indicating the
    // charge-up weapon should fire.
    if (GetSecondaryInput() == 0.0f && m_charge_up_ratio > 0.0f)
    {
        ASSERT1(m_charge_up_ratio >= 0.0f)
        ASSERT1(m_charge_up_ratio <= 1.0f)
    
        // calculate the ballistic size and impact damage
        Float ballistic_size =
            ms_ballistic_size[GetUpgradeLevel()] * (m_charge_up_ratio + 1.0f);
        Float parameter = m_charge_up_ratio * m_charge_up_ratio;
        Float impact_damage =
            ms_primary_impact_damage[GetUpgradeLevel()] * (1.0f - parameter) +
            ms_max_secondary_impact_damage[GetUpgradeLevel()] * parameter;
        // spawn it
        SpawnSmartBallistic(
            GetOwnerShip()->GetWorld(),
            GetOwnerShip()->GetObjectLayer(),
            GetMuzzleLocation() + ballistic_size * GetMuzzleDirection(),
            ballistic_size,
            1.0f,
            1.5f * ms_muzzle_speed[GetUpgradeLevel()] * GetMuzzleDirection() + GetOwnerShip()->GetVelocity(),
            impact_damage,
            ms_range[GetUpgradeLevel()] / ms_muzzle_speed[GetUpgradeLevel()],
            time,
            GetOwnerShip()->GetReference());

        // reset the charge-up ratio
        m_charge_up_ratio = 0.0f;
        // update the last time fired
        m_time_last_fired = time;
        // we didn't actually use any power here, because the weapon
        // charges up beforehand.  therefore, return false.
        return false;
    }
    // secondary fire (charge-up) overrides primary fire
    else if (GetSecondaryInput() > 0.0f && power > 0.0f)
    {
        Float max_power = GetSecondaryInput() * ms_max_secondary_power_rate[GetUpgradeLevel()] * frame_dt /
                          ms_charge_up_time[GetUpgradeLevel()];
        ASSERT1(power <= max_power)

        // if completely charged up, don't use up the power
        if (m_charge_up_ratio == 1.0f)
            return false;

        // increment the charge up ratio at the given rate
        m_charge_up_ratio += power / ms_max_secondary_power_rate[GetUpgradeLevel()];
        if (m_charge_up_ratio > 1.0f)
            m_charge_up_ratio = 1.0f;

        // power was used up, so return true.
        return true;
    }
    // primary fire
    else
    {
        ASSERT1(power <= ms_required_primary_power[GetUpgradeLevel()])
    
        // can't fire if not enough power was supplied
        if (power < ms_required_primary_power[GetUpgradeLevel()])
            return false;
    
        ASSERT1(GetPrimaryInput() > 0.0f)
    
        // fire the weapon -- create a Pea and set its position and velocity
        ASSERT1(ms_muzzle_speed[GetUpgradeLevel()] > 0.0f);
        SpawnSmartBallistic(
            GetOwnerShip()->GetWorld(),
            GetOwnerShip()->GetObjectLayer(),
            GetMuzzleLocation() + ms_ballistic_size[GetUpgradeLevel()] * GetMuzzleDirection(),
            ms_ballistic_size[GetUpgradeLevel()],
            1.0f,
            ms_muzzle_speed[GetUpgradeLevel()] * GetMuzzleDirection() + GetOwnerShip()->GetVelocity(),
            ms_primary_impact_damage[GetUpgradeLevel()],
            ms_range[GetUpgradeLevel()] / ms_muzzle_speed[GetUpgradeLevel()],
            time,
            GetOwnerShip()->GetReference());
    
        // update the last time fired
        m_time_last_fired = time;
    
        return true;
    }
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

Float Laser::GetPowerToBeUsedBasedOnInputs (
    Float const time,
    Float const frame_dt) const
{
    ASSERT1(GetPrimaryInput() <= 1.0f)
    ASSERT1(GetSecondaryInput() <= 1.0f)

    if (GetPrimaryInput() > 0.0f)
        return GetPrimaryInput() * frame_dt * ms_max_primary_power_output_rate[GetUpgradeLevel()];
    else
        return 0.0f;
}

bool Laser::Activate (
    Float power,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(m_laser_beam != NULL)
    ASSERT1(m_laser_beam->GetIsInWorld())

    // secondary fire can happen in parallel with primary
    ASSERT1(ms_secondary_fire_rate[GetUpgradeLevel()] > 0.0f)
    if (GetSecondaryInput() > 0.0f &&
        time >= m_time_last_fired + 1.0f / ms_secondary_fire_rate[GetUpgradeLevel()])
    {
        AreaTraceList area_trace_list;
        GetOwnerShip()->GetPhysicsHandler()->AreaTrace(
            GetOwnerShip()->GetObjectLayer(),
            GetOwnerShip()->GetTranslation(),
            ms_secondary_range[GetUpgradeLevel()] + GetOwnerShip()->GetScaleFactor(),
            false,
            &area_trace_list);

        EnemyShip *best_target = NULL;
        for (AreaTraceListIterator it = area_trace_list.begin(),
                                   it_end = area_trace_list.end();
             it != it_end;
             ++it)
        {
            Entity *entity = *it;
            ASSERT1(entity != NULL)
            
            // we don't want to shoot ourselves
            if (entity == GetOwnerShip())
                continue;

            // only target enemy ships
            if (entity->GetIsEnemyShip())
            {
                EnemyShip *enemy_ship = DStaticCast<EnemyShip *>(entity);
                // prefer more powerful enemies over weaker ones
                if (best_target == NULL ||
                    enemy_ship->GetEnemyLevel() > best_target->GetEnemyLevel() ||
                    enemy_ship->GetTargetPriority() > best_target->GetTargetPriority())
                {
                    best_target = enemy_ship;
                }
            }
        }

        // only fire if we found a target
        if (best_target != NULL)
        {
            FloatVector2 fire_vector(
                GetOwnerShip()->GetObjectLayer()->GetAdjustedCoordinates(
                    best_target->GetTranslation(),
                    GetOwnerShip()->GetTranslation())
                -
                GetOwnerShip()->GetTranslation());

            // TODO: do a real line trace for the damage (there might be something blocking the beam)

            if (fire_vector.GetLength() > ms_secondary_range[GetUpgradeLevel()] + GetOwnerShip()->GetScaleFactor())
            {
                fire_vector.Normalize();
                fire_vector *= ms_secondary_range[GetUpgradeLevel()] + GetOwnerShip()->GetScaleFactor();
            }
        
            // spawn the "gauss gun trail"
            SpawnGaussGunTrail(
                GetOwnerShip()->GetWorld(),
                GetOwnerShip()->GetObjectLayer(),
                GetOwnerShip()->GetTranslation() + GetOwnerShip()->GetScaleFactor() * fire_vector.GetNormalization(),
                fire_vector,
                GetOwnerShip()->GetVelocity(),
                2.0f,
                0.5f,
                time);
            
            // damage the target (the gauss gun trail is just a visual effect)
            best_target->Damage(
                GetOwnerShip(),
                NULL, // laser does not have a Entity medium
                ms_secondary_impact_damage[GetUpgradeLevel()],
                NULL,
                best_target->GetTranslation(), // TEMP for now, do real calculation later
                GetMuzzleDirection(),
                0.0f,
                Mortal::D_COMBAT_LASER,
                time,
                frame_dt);
            
            // update the last time fired
            m_time_last_fired = time;
        }
    }

    // primary constant beam firing mode
    if (GetPrimaryInput() > 0.0f && power > 0.0f)
    {
        ASSERT1(power <= GetPrimaryInput() * frame_dt * ms_max_primary_power_output_rate[GetUpgradeLevel()])
    
        LineTraceBindingSet line_trace_binding_set;
        GetOwnerShip()->GetPhysicsHandler()->LineTrace(
            GetOwnerShip()->GetObjectLayer(),
            GetMuzzleLocation(),
            ms_primary_range[GetUpgradeLevel()] * GetMuzzleDirection(),
            ms_beam_radius[GetUpgradeLevel()],
            false,
            &line_trace_binding_set);
    
        LineTraceBindingSetIterator it = line_trace_binding_set.begin();
        LineTraceBindingSetIterator it_end = line_trace_binding_set.end();
        // don't damage the owner of this weapon
        if (it->m_entity == GetOwnerShip())
            ++it;
    
        FloatVector2 laser_beam_hit_location(
            GetMuzzleLocation() + ms_primary_range[GetUpgradeLevel()] * GetMuzzleDirection());

        // we don't want to hit powerups or ballistics, just skip them.
        while (it != it_end && (it->m_entity->GetIsPowerup() || it->m_entity->GetIsBallistic()))
            ++it;
            
        // damage the next thing if it exists
        if (it != it_end && it->m_entity != NULL)
        {
            Float ratio_of_max_power_output =
                power / (frame_dt * ms_max_primary_power_output_rate[GetUpgradeLevel()]);
            laser_beam_hit_location =
                GetMuzzleLocation() + it->m_time * ms_primary_range[GetUpgradeLevel()] * GetMuzzleDirection();
            if (it->m_entity->GetIsMortal())
                DStaticCast<Mortal *>(it->m_entity)->Damage(
                    GetOwnerShip(),
                    NULL, // laser does not have a Entity medium
                    ms_damage_rate[GetUpgradeLevel()] * ratio_of_max_power_output * frame_dt,
                    NULL, // we don't care how much damage was taken
                    laser_beam_hit_location,
                    GetMuzzleDirection(),
                    0.0f,
                    Mortal::D_MINING_LASER,
                    time,
                    frame_dt);
        }
    
        // place the laser beam effect
        static Float const s_laser_beam_width = 2.0f;
        Float intensity = power / (frame_dt * ms_max_primary_power_output_rate[GetUpgradeLevel()]);
        ASSERT1(intensity >= 0.0f && intensity <= 1.0f)
        m_laser_beam->SetIntensity(intensity);
        m_laser_beam->SnapToShip(
            GetMuzzleLocation() + frame_dt * GetOwnerShip()->GetVelocity(),
            laser_beam_hit_location + frame_dt * GetOwnerShip()->GetVelocity(),
            s_laser_beam_width);

        // the weapon fired successfully
        return true;
    }
    else
    {
        // turn off the constant fire laser
        m_laser_beam->SetIntensity(0.0f);
        // the weapon was not fired
        return false;
    }
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

Float FlameThrower::GetPowerToBeUsedBasedOnInputs (
    Float const time,
    Float const frame_dt) const
{
    // can't fire faster that the weapon's cycle time
    ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f)
    if (time < m_time_last_fired + 1.0f / ms_fire_rate[GetUpgradeLevel()])
        return 0.0f;

    // return a power proportional to the primary input required power,
    // or 0 if its less than the minimum required power
    Float scaled_power = GetPrimaryInput() * ms_max_required_primary_power[GetUpgradeLevel()];
    if (scaled_power < ms_min_required_primary_power[GetUpgradeLevel()])
        return 0.0f;
    else
        return scaled_power;
}

bool FlameThrower::Activate (
    Float const power,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(power <= ms_max_required_primary_power[GetUpgradeLevel()])

    // can't fire faster that the weapon's cycle time
    ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f)
    if (time < m_time_last_fired + 1.0f / ms_fire_rate[GetUpgradeLevel()])
        return false;
    
    // don't fire if not enough power was supplied
    if (power < ms_min_required_primary_power[GetUpgradeLevel()])
        return false;

    ASSERT1(power >= 0.0f)
    ASSERT1(GetPrimaryInput() > 0.0f)
        
    // fire the weapon
    SpawnFireball(
        GetOwnerShip()->GetWorld(),
        GetOwnerShip()->GetObjectLayer(),
        GetMuzzleLocation() + 2.0f * GetMuzzleDirection(), // the extra is just so we don't fry ourselves
        ms_muzzle_speed[GetUpgradeLevel()] * GetMuzzleDirection() + GetOwnerShip()->GetVelocity(),
        power / ms_max_required_primary_power[GetUpgradeLevel()] * ms_max_damage_per_fireball[GetUpgradeLevel()],
        ms_max_damage_per_fireball[GetUpgradeLevel()],
        ms_final_fireball_size[GetUpgradeLevel()],
        1.0f,
        time,
        GetOwnerShip()->GetReference());
            
    // update the last time fired
    m_time_last_fired = time;
        
    // the weapon fired successfully
    return true;
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

Float GaussGun::GetPowerToBeUsedBasedOnInputs (
    Float const time,
    Float const frame_dt) const
{
    // can't fire faster that the weapon's cycle time
    ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f)
    if (time < m_time_last_fired + 1.0f / ms_fire_rate[GetUpgradeLevel()])
        return 0.0f;

    // if the primary input is on at all, return the full primary power
    return (GetPrimaryInput() > 0.0f) ? ms_required_primary_power[GetUpgradeLevel()] : 0.0f;
}

bool GaussGun::Activate (
    Float const power,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(power <= ms_required_primary_power[GetUpgradeLevel()])

    // can't fire if not enough power was supplied
    if (power < ms_required_primary_power[GetUpgradeLevel()])
        return false;

    ASSERT1(GetPrimaryInput() > 0.0f)
        
    // fire the weapon -- do a trace and spawn the GaussGunTrail
    ASSERT1(GetOwnerShip()->GetWorld() != NULL)
    ASSERT1(GetOwnerShip()->GetObjectLayer() != NULL)

    // do a line trace
    LineTraceBindingSet line_trace_binding_set;
    GetOwnerShip()->GetPhysicsHandler()->LineTrace(
        GetOwnerShip()->GetObjectLayer(),
        GetMuzzleLocation(),
        ms_range[GetUpgradeLevel()] * GetMuzzleDirection(),
        0.0f,
        false,
        &line_trace_binding_set);

    LineTraceBindingSetIterator it = line_trace_binding_set.begin();
    LineTraceBindingSetIterator it_end = line_trace_binding_set.end();
    // don't damage the owner of this weapon
    if (it->m_entity == GetOwnerShip())
        ++it;

    // decide how much damage to inflict total
    Float damage_left_to_inflict =
        GetIsImpactDamageOverridden() ?
        GetImpactDamageOverride() :
        ms_impact_damage[GetUpgradeLevel()];
        
    // damage the next thing if it exists
    Float furthest_hit_time = 1.0f;
    Float damage_amount_used;
    while (damage_left_to_inflict > 0.0f &&
           it != it_end)
    {
        // we don't want to hit powerups (continue without incrementing
        // the hit count or updating the furthest hit time)
        if (it->m_entity->GetIsPowerup())
        {
            ++it;
            continue;
        }
    
        furthest_hit_time = it->m_time;
        if (it->m_entity->GetIsMortal())
            DStaticCast<Mortal *>(it->m_entity)->Damage(
                GetOwnerShip(),
                NULL, // gauss gun does not have a Entity medium
                damage_left_to_inflict,
                &damage_amount_used,
                GetMuzzleLocation() + it->m_time * ms_range[GetUpgradeLevel()] * GetMuzzleDirection(),
                GetMuzzleDirection(),
                0.0f,
                Mortal::D_BALLISTIC,
                time,
                frame_dt);

        ASSERT1(damage_amount_used <= damage_left_to_inflict)
        damage_left_to_inflict -= damage_amount_used;

        furthest_hit_time = it->m_time;
        
        ++it;
    }

    if (damage_left_to_inflict > 0.0f)
        furthest_hit_time = 1.0f;

    // spawn the gauss gun trail
    SpawnGaussGunTrail(
        GetOwnerShip()->GetWorld(),
        GetOwnerShip()->GetObjectLayer(),
        GetMuzzleLocation(),
        furthest_hit_time * ms_range[GetUpgradeLevel()] * GetMuzzleDirection(),
        GetOwnerShip()->GetVelocity(),
        2.0f,
        1.0f,
        time);
    
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
    for (ActiveGrenadeSetIterator it = m_active_grenade_set.begin(),
                                  it_end = m_active_grenade_set.end();
         it != it_end;
         ++it)
    {
        Grenade *active_grenade = *it;
        active_grenade->SetOwnerGrenadeLauncher(NULL);
    }
    m_active_grenade_set.clear();
}

void GrenadeLauncher::ActiveGrenadeDestroyed (Grenade *const active_grenade)
{
    ASSERT1(active_grenade != NULL)
    ASSERT1(active_grenade->GetOwnerGrenadeLauncher() == this)
    ASSERT1(GetActiveGrenadeCount() > 0)

    // delete the active grenade from the active grenade set
    ActiveGrenadeSetIterator it = m_active_grenade_set.find(active_grenade);
    ActiveGrenadeSetIterator it_end = m_active_grenade_set.end();
    ASSERT1(it != it_end)
    m_active_grenade_set.erase(it);

    active_grenade->SetOwnerGrenadeLauncher(NULL);
}

Float GrenadeLauncher::GetPowerToBeUsedBasedOnInputs (
    Float const time,
    Float const frame_dt) const
{
    // can't fire if the maximum number of active grenades has been reached
    if (GetActiveGrenadeCount() >= ms_max_active_grenade_count[GetUpgradeLevel()])
        return false;

    // can't fire faster that the weapon's cycle time
    ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f)
    if (time < m_time_last_fired + 1.0f / ms_fire_rate[GetUpgradeLevel()])
        return 0.0f;

    // if the primary input is on at all, return the full primary power
    return (GetPrimaryInput() > 0.0f) ? ms_required_primary_power[GetUpgradeLevel()] : 0.0f;
}

bool GrenadeLauncher::Activate (
    Float const power,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(power <= ms_required_primary_power[GetUpgradeLevel()])

    // since the secondary fire takes no power, we have to check the inputs
    if (GetPrimaryInput() == 0.0f && GetSecondaryInput() == 0.0f)
        return false;

    // you can fire secondary at any time, it doesn't use any power
    // and doesn't inhibit the primary fire at all.
    if (GetSecondaryInput() > 0.0f && GetActiveGrenadeCount() > 0)
    {
        // detonate all the grenades (this isn't a for loop because detonating
        // the grenades will erase iterators from this set).
        while (!m_active_grenade_set.empty())
        {
            Grenade *active_grenade = *m_active_grenade_set.begin();
            ASSERT1(active_grenade != NULL)
            DEBUG1_CODE(Uint32 active_grenade_set_size = m_active_grenade_set.size());
            active_grenade->Detonate(time, frame_dt);
            ASSERT1(m_active_grenade_set.size() == active_grenade_set_size - 1)
        }
    }
    
    // can't fire primary if not enough power was supplied
    if (power < ms_required_primary_power[GetUpgradeLevel()])
        return false;

    ASSERT1(GetPrimaryInput() > 0.0f)
        
    // fire the weapon -- spawn a Grenade
    ASSERT1(GetOwnerShip()->GetWorld() != NULL)
    ASSERT1(GetOwnerShip()->GetObjectLayer() != NULL)

    Float const grenade_scale_factor = 4.0f;
    Grenade *grenade = SpawnGrenade(
        GetOwnerShip()->GetWorld(),
        GetOwnerShip()->GetObjectLayer(),
        GetMuzzleLocation() + grenade_scale_factor * GetMuzzleDirection(),
        grenade_scale_factor,
        ms_muzzle_speed[GetUpgradeLevel()] * GetMuzzleDirection() + GetOwnerShip()->GetVelocity(),
        this,
        ms_grenade_damage_to_inflict[GetUpgradeLevel()],
        ms_grenade_damage_radius[GetUpgradeLevel()],
        2.0f * ms_grenade_damage_radius[GetUpgradeLevel()],
        GetUpgradeLevel(),
        GetOwnerShip()->GetReference(),
        ms_grenade_health[GetUpgradeLevel()]);

    // add the grenade to the active grenade set
    ASSERT1(grenade != NULL)
    m_active_grenade_set.insert(grenade);

    // update the last time fired
    m_time_last_fired = time;
        
    // the weapon fired successfully
    return true;
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

MineLayer::~MineLayer ()
{
    for (ActiveMineSetIterator it = m_active_mine_set.begin(),
                               it_end = m_active_mine_set.end();
         it != it_end;
         ++it)
    {
        Mine *active_mine = *it;
        active_mine->SetOwnerMineLayer(NULL);
    }
    m_active_mine_set.clear();
}

void MineLayer::ActiveMineDestroyed (Mine *const active_mine)
{
    ASSERT1(active_mine != NULL)
    ASSERT1(active_mine->GetOwnerMineLayer() == this)
    ASSERT1(GetActiveMineCount() > 0)

    // delete the active mine from the active mine set
    ActiveMineSetIterator it = m_active_mine_set.find(active_mine);
    ActiveMineSetIterator it_end = m_active_mine_set.end();
    ASSERT1(it != it_end)
    m_active_mine_set.erase(it);

    active_mine->SetOwnerMineLayer(NULL);
}

Float MineLayer::GetPowerToBeUsedBasedOnInputs (
    Float const time,
    Float const frame_dt) const
{
    // can't apply power if the maximum number of active mines has been reached
    if (GetActiveMineCount() >= ms_max_active_mine_count[GetUpgradeLevel()])
        return 0.0f;

    // can't apply power if the weapon isn't ready to fire
    ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f)
    if (time < m_time_last_fired + 1.0f / ms_fire_rate[GetUpgradeLevel()])
        return 0.0f;

    // return the sum of the primary and secondary required powers
    // if each is on respectively.
    return (GetPrimaryInput() > 0.0f) ? ms_required_primary_power[GetUpgradeLevel()] : 0.0f;
}

bool MineLayer::Activate (
    Float power,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(power <= ms_required_primary_power[GetUpgradeLevel()])

    // since the primary and secondary fires can be simultaneously
    // activated we must check both
    if (GetPrimaryInput() == 0.0f && GetSecondaryInput() == 0.0f)
        return false;

    // primary takes precedence over secondary fire
    if (GetPrimaryInput() > 0.0f &&
        power >= ms_required_primary_power[GetUpgradeLevel()])
    {
        // fire the weapon -- spawn a Mine
        ASSERT1(GetOwnerShip()->GetWorld() != NULL)
        ASSERT1(GetOwnerShip()->GetObjectLayer() != NULL)
    
        Float const mine_scale_factor = 8.0f;
        Mine *mine = SpawnMine(
            GetOwnerShip()->GetWorld(),
            GetOwnerShip()->GetObjectLayer(),
            GetMuzzleLocation() + mine_scale_factor * GetMuzzleDirection(),
            mine_scale_factor,
            ms_muzzle_speed[GetUpgradeLevel()] * GetMuzzleDirection() + GetOwnerShip()->GetVelocity(),
            this,
            ms_mine_damage_to_inflict[GetUpgradeLevel()],
            ms_mine_damage_radius[GetUpgradeLevel()],
            2.0f * ms_mine_damage_radius[GetUpgradeLevel()],
            GetUpgradeLevel(),
            GetOwnerShip()->GetReference(),
            ms_mine_health[GetUpgradeLevel()]);
    
        // add the mine to the active mine set
        ASSERT1(mine != NULL)
        m_active_mine_set.insert(mine);
    
        // update the last time fired
        m_time_last_fired = time;
        
        power -= ms_required_primary_power[GetUpgradeLevel()];
    }

    if (GetSecondaryInput() > 0.0f)
    {
        // guide all the mines to the aim reticle
        for (ActiveMineSetIterator it = m_active_mine_set.begin(),
                                   it_end = m_active_mine_set.end();
             it != it_end;
             ++it)
        {
            Mine *active_mine = *it;
            ASSERT1(active_mine != NULL)
            active_mine->GiveSeekCoordinates(GetReticleCoordinates(), time);
        }
    }
    
    // the weapon fired successfully
    return true;
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

Float MissileLauncher::GetPowerToBeUsedBasedOnInputs (
    Float const time,
    Float const frame_dt) const
{
    // can't fire faster that the weapon's cycle time
    ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f)
    if (time < m_time_last_fired + 1.0f / ms_fire_rate[GetUpgradeLevel()])
        return 0.0f;

    // primary fire overrides secondary fire.
    if (GetPrimaryInput() > 0.0f)
        return ms_required_primary_power[GetUpgradeLevel()];
    else if (GetSecondaryInput() > 0.0f)
        return ms_required_secondary_power[GetUpgradeLevel()];
    else
        return 0.0f;
}

bool MissileLauncher::Activate (
    Float const power,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(
        power 
        <=
        Max(ms_required_primary_power[GetUpgradeLevel()],
            ms_required_secondary_power[GetUpgradeLevel()]))

    // primary takes precedence over secondary fire
    if (GetPrimaryInput() > 0.0f &&
        power >= ms_required_primary_power[GetUpgradeLevel()])
    {
        // fire the weapon -- spawn a Missile
        ASSERT1(GetOwnerShip()->GetWorld() != NULL)
        ASSERT1(GetOwnerShip()->GetObjectLayer() != NULL)
    
        Float const missile_scale_factor = 10.0f;
        SpawnMissile(
            GetOwnerShip()->GetWorld(),
            GetOwnerShip()->GetObjectLayer(),
            GetMuzzleLocation() + missile_scale_factor * GetMuzzleDirection(),
            missile_scale_factor,
            Math::Atan(GetMuzzleDirection()),
            ms_muzzle_speed[GetUpgradeLevel()] * GetMuzzleDirection() + GetOwnerShip()->GetVelocity(),
            ms_missile_time_to_live[GetUpgradeLevel()],
            time,
            ms_missile_damage_amount[GetUpgradeLevel()],
            ms_missile_damage_radius[GetUpgradeLevel()],
            2.0f * ms_missile_damage_radius[GetUpgradeLevel()],
            GetUpgradeLevel(),
            GetOwnerShip()->GetReference(),
            ms_missile_health[GetUpgradeLevel()]);
    
        // update the last time fired
        m_time_last_fired = time;
        // the weapon fired successfully
        return true;
    }
    else if (GetSecondaryInput() > 0.0f &&
             power == ms_required_secondary_power[GetUpgradeLevel()])
    {
        // TODO : real guided missile
        return false;
    }
    
    // the weapon did not fire
    return false;
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

Float EMPCore::GetPowerToBeUsedBasedOnInputs (
    Float const time,
    Float const frame_dt) const
{
    // can't fire faster that the weapon's cycle time
    ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f)
    if (time < m_time_last_fired + 1.0f / ms_fire_rate[GetUpgradeLevel()])
        return 0.0f;

    // if the primary input is on at all, return the full primary power
    return (GetPrimaryInput() > 0.0f) ? ms_required_primary_power[GetUpgradeLevel()] : 0.0f;
}

bool EMPCore::Activate (
    Float const power,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(power <= ms_required_primary_power[GetUpgradeLevel()])

    // if not firing, return false
    if (GetPrimaryInput() == 0.0f)
    {
        ASSERT1(power == 0.0f)
        return false;
    }

    // can't fire primary if not enough power was supplied
    if (power < ms_required_primary_power[GetUpgradeLevel()])
        return false;

    // fire the weapon -- spawn an EMPExplosion
    ASSERT1(GetOwnerShip()->GetWorld() != NULL)
    ASSERT1(GetOwnerShip()->GetObjectLayer() != NULL)

    SpawnEMPExplosion(
        GetOwnerShip()->GetWorld(),
        GetOwnerShip()->GetObjectLayer(),
        GetOwnerShip()->GetTranslation(),
        GetOwnerShip()->GetVelocity(),
        ms_emp_bomb_disable_time_factor[GetUpgradeLevel()],
        ms_emp_bomb_blast_radius[GetUpgradeLevel()],
        1.0f,
        time,
        GetOwnerShip()->GetReference());
    
    // update the last time fired
    m_time_last_fired = time;
        
    // the weapon fired successfully
    return true;
}

/*
EMPBombLayer::~EMPBombLayer ()
{
    for (ActiveEMPBombSetIterator it = m_active_emp_bomb_set.begin(),
                                  it_end = m_active_emp_bomb_set.end();
         it != it_end;
         ++it)
    {
        EMPBomb *active_emp_bomb = *it;
        active_emp_bomb->SetOwnerEMPBombLayer(NULL);
    }
    m_active_emp_bomb_set.clear();
}

void EMPBombLayer::ActiveEMPBombDestroyed (EMPBomb *const active_emp_bomb)
{
    ASSERT1(active_emp_bomb != NULL)
    ASSERT1(active_emp_bomb->GetOwnerEMPBombLayer() == this)
    ASSERT1(GetActiveEMPBombCount() > 0)

    // delete the active emp_bomb from the active emp_bomb set
    ActiveEMPBombSetIterator it = m_active_emp_bomb_set.find(active_emp_bomb);
    ActiveEMPBombSetIterator it_end = m_active_emp_bomb_set.end();
    ASSERT1(it != it_end)
    m_active_emp_bomb_set.erase(it);

    active_emp_bomb->SetOwnerEMPBombLayer(NULL);
}

Float EMPBombLayer::GetPowerToBeUsedBasedOnInputs (
    Float const time,
    Float const frame_dt) const
{
    // can't fire if the maximum number of active emp_bombs has been reached
    if (GetActiveEMPBombCount() >= ms_max_active_emp_bomb_count[GetUpgradeLevel()])
        return false;

    // can't fire faster that the weapon's cycle time
    ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f)
    if (time < m_time_last_fired + 1.0f / ms_fire_rate[GetUpgradeLevel()])
        return 0.0f;

    // if the primary input is on at all, return the full primary power
    return (GetPrimaryInput() > 0.0f) ? ms_required_primary_power[GetUpgradeLevel()] : 0.0f;
}

bool EMPBombLayer::Activate (
    Float const power,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(power <= ms_required_primary_power[GetUpgradeLevel()])

    // since the secondary fire takes no power, we have to check the inputs
    if (GetPrimaryInput() == 0.0f && GetSecondaryInput() == 0.0f)
        return false;

    // you can fire secondary at any time, it doesn't use any power
    // and doesn't inhibit the primary fire at all.
    if (GetSecondaryInput() > 0.0f && GetActiveEMPBombCount() > 0)
    {
        // detonate all the emp_bombs (this isn't a for loop because detonating
        // the emp_bombs will erase iterators from this set).
        while (!m_active_emp_bomb_set.empty())
        {
            EMPBomb *active_emp_bomb = *m_active_emp_bomb_set.begin();
            ASSERT1(active_emp_bomb != NULL)
            DEBUG1_CODE(Uint32 active_emp_bomb_set_size = m_active_emp_bomb_set.size());
            active_emp_bomb->Detonate(time, frame_dt);
            ASSERT1(m_active_emp_bomb_set.size() == active_emp_bomb_set_size - 1)
        }
    }
    
    // can't fire primary if not enough power was supplied
    if (power < ms_required_primary_power[GetUpgradeLevel()])
        return false;

    ASSERT1(GetPrimaryInput() > 0.0f)
        
    // fire the weapon -- spawn a EMPBomb
    ASSERT1(GetOwnerShip()->GetWorld() != NULL)
    ASSERT1(GetOwnerShip()->GetObjectLayer() != NULL)

    Float const emp_bomb_scale_factor = 4.0f;
    EMPBomb *emp_bomb = SpawnEMPBomb(
        GetOwnerShip()->GetWorld(),
        GetOwnerShip()->GetObjectLayer(),
        GetMuzzleLocation() + emp_bomb_scale_factor * GetMuzzleDirection(),
        emp_bomb_scale_factor,
        ms_muzzle_speed[GetUpgradeLevel()] * GetMuzzleDirection() + GetOwnerShip()->GetVelocity(),
        this,
        ms_emp_bomb_disable_time_factor[GetUpgradeLevel()],
        ms_emp_bomb_blast_radius[GetUpgradeLevel()],
        GetUpgradeLevel(),
        GetOwnerShip()->GetReference(),
        ms_emp_bomb_health[GetUpgradeLevel()]);

    // add the emp_bomb to the active emp_bomb set
    ASSERT1(emp_bomb != NULL)
    m_active_emp_bomb_set.insert(emp_bomb);

    // update the last time fired
    m_time_last_fired = time;
        
    // the weapon fired successfully
    return true;
}
*/

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

Float AutoDestruct::GetPowerToBeUsedBasedOnInputs (
    Float const time,
    Float const frame_dt) const
{
    // the amount of power to trigger it is small
    return (GetPrimaryInput() > 0.0f) ? 1.0f : 0.0f;
}

bool AutoDestruct::Activate (
    Float const power,
    Float const time,
    Float const frame_dt)
{
    if (power == 0.0f)
    {
        m_triggered_time = -1.0f;
        return false;
    }

    if (m_triggered_time < 0.0f)
        m_triggered_time = time;

    if (time - m_triggered_time >= ms_trigger_countdown_time[GetUpgradeLevel()])
    {
        GetOwnerShip()->Kill(
            GetOwnerShip(),
            NULL, // auto destruct does not have a Entity kill medium
            GetOwnerShip()->GetTranslation(),
            Math::UnitVector(GetOwnerShip()->GetAngle()),
            0.0f,
            Mortal::D_EXPLOSION,
            time,
            frame_dt);
        SpawnDamageExplosion(
            GetOwnerShip()->GetWorld(),
            GetOwnerShip()->GetObjectLayer(),
            GetOwnerShip()->GetTranslation(),
            GetOwnerShip()->GetVelocity(),
            ms_damage_amount[GetUpgradeLevel()],
            ms_damage_radius[GetUpgradeLevel()],
            2.0f * ms_damage_radius[GetUpgradeLevel()],
            0.2f,
            time,
            GetOwnerShip()->GetReference());
    }

    return true;
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

Float Tractor::GetPowerToBeUsedBasedOnInputs (
    Float const time,
    Float const frame_dt) const
{
    if (GetSecondaryInput() > 0.0f)
        return GetSecondaryInput() * frame_dt * ms_max_power_output_rate[GetUpgradeLevel()];
    else
        return GetPrimaryInput() * frame_dt * ms_max_power_output_rate[GetUpgradeLevel()];
}

bool Tractor::Activate (
    Float const power,
    Float const time,
    Float const frame_dt)
{
    // the epsilon is because floating point arithmetic isn't exact
    // and the first condition was sometimes failing.
    ASSERT1(power <= GetPowerToBeUsedBasedOnInputs(time, frame_dt) + 0.001f)
    ASSERT1(m_tractor_beam != NULL)
    ASSERT1(m_tractor_beam->GetIsInWorld())

    // don't do anything if no power was supplied
    if (power == 0.0f)
    {
        m_tractor_beam->SetParameters(true, false, 0.0f, 0.0f);
        return false;
    }

    ASSERT1(GetPrimaryInput() > 0.0f || GetSecondaryInput() > 0.0f)
    // the secondary tractor mode pulls everything, not just powerups
    bool pull_everything = GetSecondaryInput() > 0.0f;
    bool push_instead_of_pull = GetPrimaryInput() == 0.0f;
    Float input = pull_everything ? GetSecondaryInput() : GetPrimaryInput();

    Float range =
        GetIsRangeOverridden() ?
        GetRangeOverride() :
        ms_range[GetUpgradeLevel()];
    Float strength =
        GetIsStrengthOverridden() ?
        GetStrengthOverride() :
        ms_strength[GetUpgradeLevel()];
    Float max_force =
        GetIsMaxForceOverridden() ?
        GetMaxForceOverride() :
        ms_max_force[GetUpgradeLevel()];
    Float beam_radius =
        GetIsBeamRadiusOverridden() ?
        GetBeamRadiusOverride() :
        ms_beam_radius[GetUpgradeLevel()];
            
    // use the reticle coordinates for the area the tractor pulls/pushes,
    // but make sure it's inside the maximum range.
    FloatVector2 reticle_coordinates(GetReticleCoordinates());
    Float reticle_distance =
        (reticle_coordinates - GetOwnerShip()->GetTranslation()).GetLength();
    if (reticle_distance > range)
        reticle_coordinates =
            range / reticle_distance *
            (reticle_coordinates - GetOwnerShip()->GetTranslation()) +
            GetOwnerShip()->GetTranslation();

    AreaTraceList area_trace_list;
    GetOwnerShip()->GetPhysicsHandler()->AreaTrace(
        GetOwnerShip()->GetObjectLayer(),
        reticle_coordinates,
        beam_radius,
        false,
        &area_trace_list);

    Polynomial::SolutionSet solution_set;
    for (AreaTraceListIterator it = area_trace_list.begin(),
                               it_end = area_trace_list.end();
         it != it_end;
         ++it)
    {
        Entity *entity = *it;
        ASSERT1(entity != NULL)
        
        // we don't want to pull ourselves (it would cancel out anyway)
        if (entity == GetOwnerShip())
            continue;

        // if we're not pulling everything (secondary tractor mode),
        // then break if this game object isn't a powerup.
        if (!pull_everything && entity->GetEntityType() != ET_POWERUP)
            continue;

        Float force = Min(input * strength * entity->GetScaleFactor(), max_force);
        entity->ApplyInterceptCourseAcceleration(
            GetOwnerShip(),
            force,
            true,
            push_instead_of_pull,
            &solution_set);
    }

    // place the tractor beam effect
    m_tractor_beam->SetScaleFactor(beam_radius);
    m_tractor_beam->SetTranslation(reticle_coordinates + frame_dt * GetOwnerShip()->GetVelocity());
    // set its pulling input and intensity
    Float intensity = power / (frame_dt * ms_max_power_output_rate[GetUpgradeLevel()]);
    ASSERT1(intensity >= 0.0f && intensity <= 1.0f)
    m_tractor_beam->SetParameters(pull_everything, push_instead_of_pull, GetPrimaryInput(), intensity);
    
    return true;
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

Float SlowBulletGun::GetPowerToBeUsedBasedOnInputs (
    Float const time,
    Float const frame_dt) const
{
    // can't fire faster that the weapon's cycle time
    ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f)
    if (time < m_time_last_fired + 1.0f / ms_fire_rate[GetUpgradeLevel()])
        return 0.0f;

    // if the primary input is on at all, return the full primary power
    return (GetPrimaryInput() > 0.0f) ? ms_required_primary_power[GetUpgradeLevel()] : 0.0f;
}

bool SlowBulletGun::Activate (
    Float const power,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(power <= ms_required_primary_power[GetUpgradeLevel()])

    // can't fire if not enough power was supplied
    if (power < ms_required_primary_power[GetUpgradeLevel()])
        return false;

    ASSERT1(GetPrimaryInput() > 0.0f)

    // fire the weapon -- create a Pea and set its position and velocity
    ASSERT1(GetOwnerShip()->GetWorld() != NULL)
    ASSERT1(GetOwnerShip()->GetObjectLayer() != NULL)
    ASSERT1(ms_muzzle_speed[GetUpgradeLevel()] > 0.0f);
    static Float const s_bullet_size = 3.0f;
    SpawnDumbBallistic(
        GetOwnerShip()->GetWorld(),
        GetOwnerShip()->GetObjectLayer(),
        GetMuzzleLocation() + s_bullet_size * GetMuzzleDirection(),
        s_bullet_size,
        3.0f,
        ms_muzzle_speed[GetUpgradeLevel()] * GetMuzzleDirection() + GetOwnerShip()->GetVelocity(),
        ms_impact_damage[GetUpgradeLevel()],
        ms_range[GetUpgradeLevel()] / ms_muzzle_speed[GetUpgradeLevel()],
        time,
        GetOwnerShip()->GetReference());

    // update the last time fired
    m_time_last_fired = time;

    return true;
}

} // end of namespace Dis
