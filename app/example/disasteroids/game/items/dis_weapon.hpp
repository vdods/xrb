// ///////////////////////////////////////////////////////////////////////////
// dis_weapon.hpp by Victor Dods, created 2005/11/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_WEAPON_HPP_)
#define _DIS_WEAPON_HPP_

#include "dis_powereddevice.hpp"

#include "dis_enemyship.hpp"

using namespace Xrb;

namespace Dis
{

class Ship;

class Weapon : public PoweredDevice
{
public:

    Weapon (Uint8 upgrade_level, ItemType item_type)
        :
        PoweredDevice(upgrade_level, item_type),
        m_reticle_coordinates(FloatVector2::ms_zero)
    {
        ASSERT1((item_type >= IT_WEAPON_LOWEST && item_type <= IT_WEAPON_HIGHEST)
                ||
                (item_type >= IT_ENEMY_WEAPON_LOWEST && item_type <= IT_ENEMY_WEAPON_HIGHEST));
    }
    virtual ~Weapon () { }

    void SetInputs (
        Float primary_input,
        Float secondary_input,
        FloatVector2 const &muzzle_location,
        FloatVector2 const &muzzle_direction,
        FloatVector2 const &reticle_coordinates)
    {
        ASSERT1(primary_input >= 0.0f && primary_input <= 1.0f);
        ASSERT1(secondary_input >= 0.0f && secondary_input <= 1.0f);
        m_primary_input = primary_input;
        m_secondary_input = secondary_input;
        m_muzzle_location = muzzle_location;
        m_muzzle_direction = muzzle_direction;
        m_reticle_coordinates = reticle_coordinates;
    }

    // ///////////////////////////////////////////////////////////////////////
    // public interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float ReadinessStatus (Float time) const = 0;

protected:

    Float PrimaryInput () const { return m_primary_input; }
    Float SecondaryInput () const { return m_secondary_input; }
    FloatVector2 const &MuzzleLocation () const { return m_muzzle_location; }
    FloatVector2 const &MuzzleDirection () const { return m_muzzle_direction; }
    FloatVector2 const &ReticleCoordinates () const { return m_reticle_coordinates; }

private:

    // inputs
    Float m_primary_input;
    Float m_secondary_input;
    FloatVector2 m_muzzle_location;
    FloatVector2 m_muzzle_direction;
    FloatVector2 m_reticle_coordinates;
}; // end of class Weapon

// primary: shoots Ballistic -- secondary: charge-up weapon
class PeaShooter : public Weapon
{
public:

    PeaShooter (Uint32 upgrade_level)
        :
        Weapon(upgrade_level, IT_WEAPON_PEA_SHOOTER)
    {
        ASSERT1(ms_fire_rate[UpgradeLevel()] > 0.0f);
        m_time_last_fired = -1.0f / ms_fire_rate[UpgradeLevel()];
        m_charge_up_ratio = 0.0f;
    }
    virtual ~PeaShooter () { }

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float ReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_fire_rate[UpgradeLevel()];
        Float const time_since_last_fire = time - m_time_last_fired;
        ASSERT1(cycle_time > 0.0f);
        ASSERT1(time_since_last_fire >= 0.0f);
        if (time_since_last_fire > cycle_time)
        {
            // if a charge-up is in progress, return its status
            if (m_charge_up_ratio > 0.0f)
                return m_charge_up_ratio;
            else
                return 1.0f;
        }
        else
            return time_since_last_fire / cycle_time;
    }

    // ///////////////////////////////////////////////////////////////////////
    // PoweredDevice interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float PowerToBeUsedBasedOnInputs (Float time, Float frame_dt) const;

    virtual bool Activate (Float power, Float time, Float frame_dt);

    // ///////////////////////////////////////////////////////////////////////
    // Item interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual void Unequip (Ship *owner_ship)
    {
        m_charge_up_ratio = 0.0f;
        Weapon::Unequip(owner_ship);
    }

private:

    static Float const ms_primary_impact_damage[UPGRADE_LEVEL_COUNT];
    static Float const ms_max_secondary_impact_damage[UPGRADE_LEVEL_COUNT];
    static Float const ms_muzzle_speed[UPGRADE_LEVEL_COUNT];
    static Float const ms_ballistic_size[UPGRADE_LEVEL_COUNT];
    static Float const ms_range[UPGRADE_LEVEL_COUNT];
    static Float const ms_required_primary_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_max_secondary_power_rate[UPGRADE_LEVEL_COUNT];
    static Float const ms_fire_rate[UPGRADE_LEVEL_COUNT];
    static Float const ms_charge_up_time[UPGRADE_LEVEL_COUNT];
    Float m_time_last_fired;
    Float m_charge_up_ratio;
}; // end of class PeaShooter

class LaserBeam;
class LaserImpactEffect;

// primary: asteroid-cutting laser -- secondary: proximity defense laser
class Laser : public Weapon
{
public:

    Laser (Uint32 upgrade_level)
        :
        Weapon(upgrade_level, IT_WEAPON_LASER)
    {
        ASSERT1(ms_secondary_fire_rate[UpgradeLevel()] > 0.0f);
        m_time_last_fired = -1.0f / ms_secondary_fire_rate[UpgradeLevel()];
        m_laser_beam = NULL;
        m_laser_impact_effect = NULL;
    }
    virtual ~Laser () { }

    void SetLaserBeam (LaserBeam *laser_beam)
    {
        ASSERT1(laser_beam != NULL);
        m_laser_beam = laser_beam;
    }
    void SetLaserImpactEffect (LaserImpactEffect *laser_impact_effect)
    {
        ASSERT1(laser_impact_effect != NULL);
        m_laser_impact_effect = laser_impact_effect;
    }

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float ReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_secondary_fire_rate[UpgradeLevel()];
        Float const time_since_last_fire = time - m_time_last_fired;
        ASSERT1(cycle_time > 0.0f);
        ASSERT1(time_since_last_fire >= 0.0f);
        if (time_since_last_fire > cycle_time)
            return 1.0f;
        else
            return time_since_last_fire / cycle_time;
    }

    // ///////////////////////////////////////////////////////////////////////
    // PoweredDevice interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float PowerToBeUsedBasedOnInputs (Float time, Float frame_dt) const;

    virtual bool Activate (Float power, Float time, Float frame_dt);

private:

    static Float const ms_primary_range[UPGRADE_LEVEL_COUNT];
    static Float const ms_secondary_range[UPGRADE_LEVEL_COUNT];
    static Float const ms_secondary_fire_rate[UPGRADE_LEVEL_COUNT];
    static Float const ms_max_primary_power_output_rate[UPGRADE_LEVEL_COUNT];
    static Float const ms_damage_rate[UPGRADE_LEVEL_COUNT];
    static Float const ms_secondary_impact_damage[UPGRADE_LEVEL_COUNT];
    static Float const ms_beam_radius[UPGRADE_LEVEL_COUNT];

    Float m_time_last_fired;
    LaserBeam *m_laser_beam;
    LaserImpactEffect *m_laser_impact_effect;
}; // end of class Laser

// primary: shoots Fireball -- secondary: quick/powerful blast mode
class FlameThrower : public Weapon
{
public:

    FlameThrower (Uint32 upgrade_level)
        :
        Weapon(upgrade_level, IT_WEAPON_FLAME_THROWER)
    {
        ASSERT1(ms_fire_rate[UpgradeLevel()] > 0.0f);
        m_time_last_fired = -1.0f / ms_fire_rate[UpgradeLevel()];
        m_max_damage_per_fireball_override = -1.0f;
        m_final_fireball_size_override = -1.0f;
    }
    virtual ~FlameThrower () { }

    bool IsMaxDamagePerFireballOverridden () const { return m_max_damage_per_fireball_override >= 0.0f; }
    bool IsFinalFireballSizeOverridden () const { return m_final_fireball_size_override >= 0.0f; }
    Float MaxDamagePerFireballOverride () const { return m_max_damage_per_fireball_override; }
    Float FinalFireballSizeOverride () const { return m_final_fireball_size_override; }

    void SetMaxDamagePerFireballOverride (Float max_damage_per_fireball_override)
    {
        ASSERT1(max_damage_per_fireball_override > 0.0f);
        m_max_damage_per_fireball_override = max_damage_per_fireball_override;
    }
    void SetFinalFireballSizeOverride (Float final_fireball_size_override)
    {
        ASSERT1(final_fireball_size_override > 0.0f);
        m_final_fireball_size_override = final_fireball_size_override;
    }

    void ClearMaxDamagePerFireballOverride () { m_max_damage_per_fireball_override = -1.0f; }
    void ClearFinalFireballSizeOverride () { m_final_fireball_size_override = -1.0f; }

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float ReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_fire_rate[UpgradeLevel()];
        Float const time_since_last_fire = time - m_time_last_fired;
        ASSERT1(cycle_time > 0.0f);
        ASSERT1(time_since_last_fire >= 0.0f);
        if (time_since_last_fire > cycle_time)
            return 1.0f;
        else
            return time_since_last_fire / cycle_time;
    }

    // ///////////////////////////////////////////////////////////////////////
    // PoweredDevice interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float PowerToBeUsedBasedOnInputs (Float time, Float frame_dt) const;

    virtual bool Activate (Float power, Float time, Float frame_dt);

private:

    static Float const ms_muzzle_speed[UPGRADE_LEVEL_COUNT];
    static Float const ms_min_required_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_max_required_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_max_damage_per_fireball[UPGRADE_LEVEL_COUNT];
    static Float const ms_final_fireball_size[UPGRADE_LEVEL_COUNT];
    static Float const ms_fire_rate[UPGRADE_LEVEL_COUNT];
    static Float const ms_blast_mode_power_factor;
    static Float const ms_blast_mode_damage_factor;
    Float m_time_last_fired;
    Float m_max_damage_per_fireball_override;
    Float m_final_fireball_size_override;
}; // end of class FlameThrower

// primary: fires instant-hit weapon -- secondary: none
class GaussGun : public Weapon
{
public:

    static Float const ms_impact_damage[UPGRADE_LEVEL_COUNT];
    static Float const ms_range[UPGRADE_LEVEL_COUNT];
    static Float const ms_required_primary_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_fire_rate[UPGRADE_LEVEL_COUNT];

    GaussGun (Uint32 upgrade_level)
        :
        Weapon(upgrade_level, IT_WEAPON_GAUSS_GUN)
    {
        ASSERT1(ms_fire_rate[UpgradeLevel()] > 0.0f);
        m_time_last_fired = -1.0f / ms_fire_rate[UpgradeLevel()];
        ClearImpactDamageOverride();
    }
    virtual ~GaussGun ()
    {
//         if (m_reticle_effect.IsValid())
//         {
//             if (m_reticle_effect->IsInWorld())
//                 m_reticle_effect->RemoveFromWorld();
//             delete m_reticle_effect->OwnerObject();
//         }
    }

    bool IsImpactDamageOverridden () const { return m_impact_damage_override >= 0.0f; }
    Float ImpactDamageOverride () const { return m_impact_damage_override; }

    void SetImpactDamageOverride (Float impact_damage_override)
    {
        ASSERT1(impact_damage_override >= 0.0f);
        m_impact_damage_override = impact_damage_override;
    }

    void ClearImpactDamageOverride () { m_impact_damage_override = -1.0f; }
//     void EnsureReticleEffectIsCleared ();

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float ReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_fire_rate[UpgradeLevel()];
        Float const time_since_last_fire = time - m_time_last_fired;
        ASSERT1(cycle_time > 0.0f);
        ASSERT1(time_since_last_fire >= 0.0f);
        if (time_since_last_fire > cycle_time)
            return 1.0f;
        else
            return time_since_last_fire / cycle_time;
    }

    // ///////////////////////////////////////////////////////////////////////
    // PoweredDevice interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float PowerToBeUsedBasedOnInputs (Float time, Float frame_dt) const;

    virtual bool Activate (Float power, Float time, Float frame_dt);

private:

    Float m_time_last_fired;
    Float m_impact_damage_override;
//     EntityReference<ReticleEffect> m_reticle_effect;
}; // end of class GaussGun

class Grenade;

// primary: shoots Grenade -- secondary: detonates Grenade
class GrenadeLauncher : public Weapon
{
public:

    GrenadeLauncher (Uint32 upgrade_level)
        :
        Weapon(upgrade_level, IT_WEAPON_GRENADE_LAUNCHER)
    {
        ASSERT1(ms_fire_rate[UpgradeLevel()] > 0.0f);
        m_time_last_fired = -1.0f / ms_fire_rate[UpgradeLevel()];
    }
    virtual ~GrenadeLauncher ();

    Uint32 ActiveGrenadeCount () const { return m_active_grenade_set.size(); }

    void ActiveGrenadeDestroyed (Grenade *active_grenade);

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float ReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_fire_rate[UpgradeLevel()];
        Float const time_since_last_fire = time - m_time_last_fired;
        ASSERT1(cycle_time > 0.0f);
        ASSERT1(time_since_last_fire >= 0.0f);
        if (time_since_last_fire > cycle_time)
            return 1.0f;
        else
            return time_since_last_fire / cycle_time;
    }

    // ///////////////////////////////////////////////////////////////////////
    // PoweredDevice interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float PowerToBeUsedBasedOnInputs (Float time, Float frame_dt) const;

    virtual bool Activate (Float power, Float time, Float frame_dt);

private:

    typedef std::set<Grenade *> ActiveGrenadeSet;

    static Float const ms_muzzle_speed[UPGRADE_LEVEL_COUNT];
    static Float const ms_required_primary_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_grenade_damage_to_inflict[UPGRADE_LEVEL_COUNT];
    static Float const ms_grenade_damage_radius[UPGRADE_LEVEL_COUNT];
    static Float const ms_grenade_health[UPGRADE_LEVEL_COUNT];
    static Float const ms_fire_rate[UPGRADE_LEVEL_COUNT];
    static Uint32 const ms_max_active_grenade_count[UPGRADE_LEVEL_COUNT];

    ActiveGrenadeSet m_active_grenade_set;
    Float m_time_last_fired;
}; // end of class GrenadeLauncher

class Missile;

// primary: shoots missile -- secondary: guides existing missiles
class MissileLauncher : public Weapon
{
public:

    MissileLauncher (Uint32 upgrade_level)
        :
        Weapon(upgrade_level, IT_WEAPON_MISSILE_LAUNCHER)
    {
        ASSERT1(ms_fire_rate[UpgradeLevel()] > 0.0f);
        m_time_last_fired = -1.0f / ms_fire_rate[UpgradeLevel()];
        m_spawn_enemy_missiles = false;
    }
    virtual ~MissileLauncher ();

    bool SpawnEnemyMissiles () const { return m_spawn_enemy_missiles; }

    void SetSpawnEnemyMissiles (bool spawn_enemy_missiles)
    {
        m_spawn_enemy_missiles = spawn_enemy_missiles;
    }

    Uint32 ActiveMissileCount () const { return m_active_missile_set.size(); }

    void ActiveMissileDestroyed (Missile *active_missile);

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float ReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_fire_rate[UpgradeLevel()];
        Float const time_since_last_fire = time - m_time_last_fired;
        ASSERT1(cycle_time > 0.0f);
        ASSERT1(time_since_last_fire >= 0.0f);
        if (time_since_last_fire > cycle_time)
            return 1.0f;
        else
            return time_since_last_fire / cycle_time;
    }

    // ///////////////////////////////////////////////////////////////////////
    // PoweredDevice interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float PowerToBeUsedBasedOnInputs (
        Float time,
        Float frame_dt) const;

    virtual bool Activate (
        Float power,
        Float time,
        Float frame_dt);

private:

    typedef std::set<Missile *> ActiveMissileSet;

    static Float const ms_primary_muzzle_speed[UPGRADE_LEVEL_COUNT];
    static Float const ms_maximum_thrust_force[UPGRADE_LEVEL_COUNT];
    static Float const ms_required_primary_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_primary_missile_time_to_live[UPGRADE_LEVEL_COUNT];
    static Float const ms_missile_damage_amount[UPGRADE_LEVEL_COUNT];
    static Float const ms_missile_damage_radius[UPGRADE_LEVEL_COUNT];
    static Float const ms_missile_health[UPGRADE_LEVEL_COUNT];
    static Float const ms_fire_rate[UPGRADE_LEVEL_COUNT];
    static Uint32 const ms_max_active_missile_count[UPGRADE_LEVEL_COUNT];

    ActiveMissileSet m_active_missile_set;
    Float m_time_last_fired;
    bool m_spawn_enemy_missiles;
}; // end of class MissileLauncher

// - primary: EMP explosion (disables enemies) -- secondary: none
class EMPCore : public Weapon
{
public:

    EMPCore (Uint32 upgrade_level)
        :
        Weapon(upgrade_level, IT_WEAPON_EMP_CORE)
    {
        ASSERT1(ms_fire_rate[UpgradeLevel()] > 0.0f);
        m_time_last_fired = -1.0f / ms_fire_rate[UpgradeLevel()];
    }
    virtual ~EMPCore () { }

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float ReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_fire_rate[UpgradeLevel()];
        Float const time_since_last_fire = time - m_time_last_fired;
        ASSERT1(cycle_time > 0.0f);
        ASSERT1(time_since_last_fire >= 0.0f);
        if (time_since_last_fire > cycle_time)
            return 1.0f;
        else
            return time_since_last_fire / cycle_time;
    }

    // ///////////////////////////////////////////////////////////////////////
    // PoweredDevice interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float PowerToBeUsedBasedOnInputs (Float time, Float frame_dt) const;

    virtual bool Activate (Float power, Float time, Float frame_dt);

private:

    static Float const ms_required_primary_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_emp_core_disable_time_factor[UPGRADE_LEVEL_COUNT];
    static Float const ms_emp_core_blast_radius[UPGRADE_LEVEL_COUNT];
    static Float const ms_fire_rate[UPGRADE_LEVEL_COUNT];

    Float m_time_last_fired;
}; // end of class EMPCore

class TractorBeam;

// primary: pulls -- secondary: pushes
class Tractor : public Weapon
{
public:

    Tractor (Uint32 upgrade_level)
        :
        Weapon(upgrade_level, IT_WEAPON_TRACTOR)
    {
        m_tractor_beam = NULL;
        m_range_override = -1.0f;
        m_strength_override = -1.0f;
        m_max_force_override = -1.0f;
        m_beam_radius_override = -1.0f;
    }
    virtual ~Tractor () { }

    bool IsRangeOverridden () const { return m_range_override >= 0.0f; }
    Float RangeOverride () const { return m_range_override; }

    bool IsStrengthOverridden () const { return m_strength_override >= 0.0f; }
    Float StrengthOverride () const { return m_strength_override; }

    bool IsMaxForceOverridden () const { return m_max_force_override >= 0.0f; }
    Float MaxForceOverride () const { return m_max_force_override; }

    bool IsBeamRadiusOverridden () const { return m_beam_radius_override >= 0.0f; }
    Float BeamRadiusOverride () const { return m_beam_radius_override; }

    void SetRangeOverride (Float range_override)
    {
        ASSERT1(range_override >= 0.0f);
        m_range_override = range_override;
    }
    void SetStrengthOverride (Float strength_override)
    {
        ASSERT1(strength_override >= 0.0f);
        m_strength_override = strength_override;
    }
    void SetMaxForceOverride (Float max_force_override)
    {
        ASSERT1(max_force_override >= 0.0f);
        m_max_force_override = max_force_override;
    }
    void SetBeamRadiusOverride (Float beam_radius_override)
    {
        ASSERT1(beam_radius_override >= 0.0f);
        m_beam_radius_override = beam_radius_override;
    }

    void ClearRangeOverride () { m_range_override = -1.0f; }
    void ClearStrengthOverride () { m_strength_override = -1.0f; }
    void ClearMaxForceOverride () { m_max_force_override = -1.0f; }
    void ClearBeamRadiusOverride () { m_beam_radius_override = -1.0f; }

    void SetTractorBeam (TractorBeam *tractor_beam)
    {
        ASSERT1(tractor_beam != NULL);
        m_tractor_beam = tractor_beam;
    }

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float ReadinessStatus (Float time) const
    {
        return 1.0f;
    }

    // ///////////////////////////////////////////////////////////////////////
    // PoweredDevice interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float PowerToBeUsedBasedOnInputs (Float time, Float frame_dt) const;

    virtual bool Activate (Float power, Float time, Float frame_dt);

protected:

    Tractor (Uint32 upgrade_level, ItemType item_type)
        :
        Weapon(upgrade_level, item_type)
    {
        m_tractor_beam = NULL;
        m_range_override = -1.0f;
        m_strength_override = -1.0f;
        m_max_force_override = -1.0f;
        m_beam_radius_override = -1.0f;
    }

protected:

    TractorBeam *m_tractor_beam;

    static Float const ms_range[UPGRADE_LEVEL_COUNT];
    static Float const ms_max_power_output_rate[UPGRADE_LEVEL_COUNT];
    static Float const ms_strength[UPGRADE_LEVEL_COUNT];
    static Float const ms_max_force[UPGRADE_LEVEL_COUNT];
    static Float const ms_beam_radius[UPGRADE_LEVEL_COUNT];

private:

    Float m_range_override;
    Float m_strength_override;
    Float m_max_force_override;
    Float m_beam_radius_override;
}; // end of class Tractor

// ///////////////////////////////////////////////////////////////////////////
// enemy weapons
// ///////////////////////////////////////////////////////////////////////////

// acts as a normal Tractor, but if primary and secondary input are both
// enabled, the tractor beam is yellow and acts in a grab-mode.
class AdvancedTractor : public Tractor
{
public:

    AdvancedTractor (Uint32 upgrade_level)
        :
        Tractor(upgrade_level, IT_ENEMY_WEAPON_ADVANCED_TRACTOR),
        m_target(NULL)
    { }
    virtual ~AdvancedTractor () { }

    void Target (Engine2::Circle::Entity *target) { m_target = target; }

    virtual bool Activate (Float power, Float time, Float frame_dt);

private:

    Engine2::Circle::Entity *m_target;
}; // end of class AdvancedTractor

class SlowBulletGun : public Weapon
{
public:

    static Float const ms_impact_damage[UPGRADE_LEVEL_COUNT];
    static Float const ms_muzzle_speed[UPGRADE_LEVEL_COUNT];
    static Float const ms_range[UPGRADE_LEVEL_COUNT];
    static Float const ms_required_primary_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_fire_rate[UPGRADE_LEVEL_COUNT];

    SlowBulletGun (Uint32 upgrade_level)
        :
        Weapon(upgrade_level, IT_ENEMY_WEAPON_SLOW_BULLET_GUN)
    {
        ASSERT1(ms_fire_rate[UpgradeLevel()] > 0.0f);
        m_time_last_fired = -1.0f / ms_fire_rate[UpgradeLevel()];
    }
    virtual ~SlowBulletGun () { }

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float ReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_fire_rate[UpgradeLevel()];
        Float const time_since_last_fire = time - m_time_last_fired;
        ASSERT1(cycle_time > 0.0f);
        ASSERT1(time_since_last_fire >= 0.0f);
        if (time_since_last_fire > cycle_time)
            return 1.0f;
        else
            return time_since_last_fire / cycle_time;
    }

    // ///////////////////////////////////////////////////////////////////////
    // PoweredDevice interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float PowerToBeUsedBasedOnInputs (Float time, Float frame_dt) const;

    virtual bool Activate (Float power, Float time, Float frame_dt);

private:

    Float m_time_last_fired;
}; // end of class SlowBulletGun

class EnemySpawner : public Weapon
{
public:

    static Float const ms_muzzle_speed[UPGRADE_LEVEL_COUNT];
    static Float const ms_required_primary_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_fire_rate[UPGRADE_LEVEL_COUNT];

    EnemySpawner (Uint32 upgrade_level)
        :
        Weapon(upgrade_level, IT_ENEMY_WEAPON_ENEMY_SPAWNER)
    {
        ASSERT1(ms_fire_rate[UpgradeLevel()] > 0.0f);
        m_time_last_fired = -1.0f / ms_fire_rate[UpgradeLevel()];
        SetEnemySpawnType(ET_INTERLOPER);
        ClearFireRateOverride();
    }
    virtual ~EnemySpawner () { }

    EntityType EnemySpawnType () const { return m_enemy_spawn_type; }
    bool IsFireRateOverridden () const { return m_fire_rate_override > 0.0f; }
    Float FireRateOverride () const { return m_fire_rate_override; }

    void SetEnemySpawnType (EntityType enemy_spawn_type)
    {
        ASSERT1(enemy_spawn_type >= ET_ENEMY_SHIP_LOWEST);
        ASSERT1(enemy_spawn_type <= ET_ENEMY_SHIP_HIGHEST);
        m_enemy_spawn_type = enemy_spawn_type;
    }
    void SetFireRateOverride (Float fire_rate_override)
    {
        ASSERT1(fire_rate_override > 0.0f);
        m_fire_rate_override = fire_rate_override;
    }

    void ClearFireRateOverride () { m_fire_rate_override = -1.0f; }

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float ReadinessStatus (Float time) const
    {
        Float const fire_rate =
            IsFireRateOverridden() ?
            FireRateOverride() :
            ms_fire_rate[UpgradeLevel()];
        Float const cycle_time = 1.0f / fire_rate;
        Float const time_since_last_fire = time - m_time_last_fired;
        ASSERT1(cycle_time > 0.0f);
        ASSERT1(time_since_last_fire >= 0.0f);
        if (time_since_last_fire > cycle_time)
            return 1.0f;
        else
            return time_since_last_fire / cycle_time;
    }

    // ///////////////////////////////////////////////////////////////////////
    // PoweredDevice interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float PowerToBeUsedBasedOnInputs (Float time, Float frame_dt) const;

    virtual bool Activate (Float power, Float time, Float frame_dt);

private:

    EntityType m_enemy_spawn_type;
    Float m_fire_rate_override;
    Float m_time_last_fired;
}; // end of class EnemySpawner

} // end of namespace Dis

#endif // !defined(_DIS_WEAPON_HPP_)

