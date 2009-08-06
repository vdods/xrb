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

    Weapon (Uint8 const upgrade_level, ItemType const item_type)
        :
        PoweredDevice(upgrade_level, item_type),
        m_reticle_coordinates(FloatVector2::ms_zero)
    {
        ASSERT1((item_type >= IT_WEAPON_LOWEST && item_type <= IT_WEAPON_HIGHEST)
                ||
                (item_type >= IT_ENEMY_WEAPON_LOWEST && item_type <= IT_ENEMY_WEAPON_HIGHEST));
    }
    virtual ~Weapon () { }

    inline void SetInputs (
        Float const primary_input,
        Float const secondary_input,
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

    inline Float PrimaryInput () const { return m_primary_input; }
    inline Float GetSecondaryInput () const { return m_secondary_input; }
    inline FloatVector2 const &MuzzleLocation () const { return m_muzzle_location; }
    inline FloatVector2 const &MuzzleDirection () const { return m_muzzle_direction; }
    inline FloatVector2 const &ReticleCoordinates () const { return m_reticle_coordinates; }

private:

    // inputs
    Float m_primary_input;
    Float m_secondary_input;
    FloatVector2 m_muzzle_location;
    FloatVector2 m_muzzle_direction;
    FloatVector2 m_reticle_coordinates;
}; // end of class Weapon

// - pea shooter (player starts off with this)
//   * upgrades will be to the projectile speed and damage (the projectiles will
//     not harm you by default)
class PeaShooter : public Weapon      //- primary: shoots Ballistic / secondary: charge-up weapon
{
public:

    PeaShooter (Uint32 const upgrade_level)
        :
        Weapon(upgrade_level, IT_WEAPON_PEA_SHOOTER)
    {
        ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f);
        m_time_last_fired = -1.0f / ms_fire_rate[GetUpgradeLevel()];
        m_charge_up_ratio = 0.0f;
    }
    virtual ~PeaShooter () { }

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float ReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_fire_rate[GetUpgradeLevel()];
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

    virtual Float PowerToBeUsedBasedOnInputs (
        Float time,
        Float frame_dt) const;

    virtual bool Activate (
        Float power,
        Float time,
        Float frame_dt);

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

// - lasers
//   * upgrades will be to the power and range (the lasers will not (can't)
//     harm the player by default)
class Laser : public Weapon // - primary: trace/LaserBeam / secondary: proximity laser
{
public:

    Laser (Uint32 const upgrade_level)
        :
        Weapon(upgrade_level, IT_WEAPON_LASER)
    {
        ASSERT1(ms_secondary_fire_rate[GetUpgradeLevel()] > 0.0f);
        m_time_last_fired = -1.0f / ms_secondary_fire_rate[GetUpgradeLevel()];
        m_laser_beam = NULL;
    }
    virtual ~Laser () { }

    inline void SetLaserBeam (LaserBeam *const laser_beam)
    {
        ASSERT1(laser_beam != NULL);
        m_laser_beam = laser_beam;
    }

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float ReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_secondary_fire_rate[GetUpgradeLevel()];
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

    static Float const ms_primary_range[UPGRADE_LEVEL_COUNT];
    static Float const ms_secondary_range[UPGRADE_LEVEL_COUNT];
    static Float const ms_secondary_fire_rate[UPGRADE_LEVEL_COUNT];
    static Float const ms_max_primary_power_output_rate[UPGRADE_LEVEL_COUNT];
    static Float const ms_damage_rate[UPGRADE_LEVEL_COUNT];
    static Float const ms_secondary_impact_damage[UPGRADE_LEVEL_COUNT];
    static Float const ms_beam_radius[UPGRADE_LEVEL_COUNT];

    Float m_time_last_fired;
    LaserBeam *m_laser_beam;
}; // end of class Laser

// - flame thrower (will not harm the player by default)
//   * upgrades will be to the power and range
class FlameThrower : public Weapon // - primary: shoots Fireball / secondary: none (so far)
{
public:

    FlameThrower (Uint32 const upgrade_level)
        :
        Weapon(upgrade_level, IT_WEAPON_FLAME_THROWER)
    {
        ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f);
        m_time_last_fired = -1.0f / ms_fire_rate[GetUpgradeLevel()];
        m_max_damage_per_fireball_override = -1.0f;
        m_final_fireball_size_override = -1.0f;
    }
    virtual ~FlameThrower () { }

    inline bool IsMaxDamagePerFireballOverridden () const { return m_max_damage_per_fireball_override >= 0.0f; }
    inline bool IsFinalFireballSizeOverridden () const { return m_final_fireball_size_override >= 0.0f; }
    inline Float MaxDamagePerFireballOverride () const { return m_max_damage_per_fireball_override; }
    inline Float FinalFireballSizeOverride () const { return m_final_fireball_size_override; }

    inline void SetMaxDamagePerFireballOverride (Float max_damage_per_fireball_override)
    {
        ASSERT1(max_damage_per_fireball_override > 0.0f);
        m_max_damage_per_fireball_override = max_damage_per_fireball_override;
    }
    inline void SetFinalFireballSizeOverride (Float final_fireball_size_override)
    {
        ASSERT1(final_fireball_size_override > 0.0f);
        m_final_fireball_size_override = final_fireball_size_override;
    }

    inline void ClearMaxDamagePerFireballOverride () { m_max_damage_per_fireball_override = -1.0f; }
    inline void ClearFinalFireballSizeOverride () { m_final_fireball_size_override = -1.0f; }

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float ReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_fire_rate[GetUpgradeLevel()];
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

    static Float const ms_muzzle_speed[UPGRADE_LEVEL_COUNT];
    static Float const ms_min_required_primary_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_max_required_primary_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_max_damage_per_fireball[UPGRADE_LEVEL_COUNT];
    static Float const ms_final_fireball_size[UPGRADE_LEVEL_COUNT];
    static Float const ms_fire_rate[UPGRADE_LEVEL_COUNT];
    Float m_time_last_fired;
    Float m_max_damage_per_fireball_override;
    Float m_final_fireball_size_override;
}; // end of class FlameThrower

// - gauss gun (high power instant-hit type weapon)
//   * upgrades will be to the damage of the projectile and the recharge time
//     (by default the projectile will not harm you)
class GaussGun : public Weapon
{
public:

    static Float const ms_impact_damage[UPGRADE_LEVEL_COUNT];
    static Float const ms_range[UPGRADE_LEVEL_COUNT];
    static Float const ms_required_primary_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_fire_rate[UPGRADE_LEVEL_COUNT];

    GaussGun (Uint32 const upgrade_level)
        :
        Weapon(upgrade_level, IT_WEAPON_GAUSS_GUN)
    {
        ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f);
        m_time_last_fired = -1.0f / ms_fire_rate[GetUpgradeLevel()];
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

    inline bool IsImpactDamageOverridden () const { return m_impact_damage_override >= 0.0f; }
    inline Float ImpactDamageOverride () const { return m_impact_damage_override; }

    inline void SetImpactDamageOverride (Float impact_damage_override)
    {
        ASSERT1(impact_damage_override >= 0.0f);
        m_impact_damage_override = impact_damage_override;
    }

    inline void ClearImpactDamageOverride () { m_impact_damage_override = -1.0f; }
//     void EnsureReticleEffectIsCleared ();

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float ReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_fire_rate[GetUpgradeLevel()];
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

    Float m_time_last_fired;
    Float m_impact_damage_override;
//     EntityReference<ReticleEffect> m_reticle_effect;
}; // end of class GaussGun

class Grenade;

// - grenade launcher
//   * upgrades will be to the power of the grenade explosion and to make the
//     explosions not harm you
class GrenadeLauncher : public Weapon // primary: shoots Grenade / secondary: detonates Grenade
{
public:

    GrenadeLauncher (Uint32 const upgrade_level)
        :
        Weapon(upgrade_level, IT_WEAPON_GRENADE_LAUNCHER)
    {
        ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f);
        m_time_last_fired = -1.0f / ms_fire_rate[GetUpgradeLevel()];
    }
    virtual ~GrenadeLauncher ();

    inline Uint32 ActiveGrenadeCount () const
    {
        return m_active_grenade_set.size();
    }

    void ActiveGrenadeDestroyed (Grenade *active_grenade);

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float ReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_fire_rate[GetUpgradeLevel()];
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

    typedef std::set<Grenade *> ActiveGrenadeSet;
    typedef ActiveGrenadeSet::iterator ActiveGrenadeSetIterator;

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

// - missile launcher
//   * upgrades will be to the projectile speed and explosion power,
//     dumb-seeking, smart-seeking, to make the explosions not hurt you
class MissileLauncher : public Weapon // - primary: shoots Missile / secondary: shoots seeking Missile
{
public:

    MissileLauncher (Uint32 const upgrade_level)
        :
        Weapon(upgrade_level, IT_WEAPON_MISSILE_LAUNCHER)
    {
        ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f);
        m_time_last_fired = -1.0f / ms_fire_rate[GetUpgradeLevel()];
        m_spawn_enemy_missiles = false;
    }
    virtual ~MissileLauncher () { }

    inline bool GetSpawnEnemyMissiles () const { return m_spawn_enemy_missiles; }

    inline void SetSpawnEnemyMissiles (bool spawn_enemy_missiles)
    {
        m_spawn_enemy_missiles = spawn_enemy_missiles;
    }

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float ReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_fire_rate[GetUpgradeLevel()];
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

    static Float const ms_primary_muzzle_speed[UPGRADE_LEVEL_COUNT];
    static Float const ms_secondary_muzzle_speed[UPGRADE_LEVEL_COUNT];
    static Float const ms_required_primary_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_required_secondary_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_primary_missile_time_to_live[UPGRADE_LEVEL_COUNT];
    static Float const ms_secondary_missile_time_to_live[UPGRADE_LEVEL_COUNT];
    static Float const ms_missile_damage_amount[UPGRADE_LEVEL_COUNT];
    static Float const ms_missile_damage_radius[UPGRADE_LEVEL_COUNT];
    static Float const ms_missile_health[UPGRADE_LEVEL_COUNT];
    static Float const ms_fire_rate[UPGRADE_LEVEL_COUNT];

    Float m_time_last_fired;
    bool m_spawn_enemy_missiles;
}; // end of class MissileLauncher

// - EMP blast - disables enemies (maybe with certain exceptions) for a short
//   time.  fire will cause an EMP explosion out from the player's ship,
//   disabling enemies which touch it but not the player.  if any ship gets
//   trapped in the blast radius, it is disabled for a short period of time
//   (can't move or fire).
class EMPCore : public Weapon // - primary: EMP explosion / secondary: ?
{
public:

    EMPCore (Uint32 const upgrade_level)
        :
        Weapon(upgrade_level, IT_WEAPON_EMP_CORE)
    {
        ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f);
        m_time_last_fired = -1.0f / ms_fire_rate[GetUpgradeLevel()];
    }
    virtual ~EMPCore () { }

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float ReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_fire_rate[GetUpgradeLevel()];
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

    static Float const ms_required_primary_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_emp_bomb_disable_time_factor[UPGRADE_LEVEL_COUNT];
    static Float const ms_emp_bomb_blast_radius[UPGRADE_LEVEL_COUNT];
    static Float const ms_fire_rate[UPGRADE_LEVEL_COUNT];

    Float m_time_last_fired;
}; // end of class EMPCore

/*
class EMPBomb;

// - EMP bomb - disables enemies (maybe with certain exceptions) for a short
//   time.  the player presses and holds the fire button to launch it, and
//   releases the fire button to detonate it.  if any ship gets trapped
//   in the blast radius, it is disabled for a short period of time (can't
//   move or fire).  each EMP bomb must be produced one at a time --
//   there is no infinite ammo with this one.
class EMPBombLayer : public Weapon // - primary: shoots EMPBomb / secondary: detonates EMPBomb
{
public:

    EMPBombLayer (Uint32 const upgrade_level)
        :
        Weapon(upgrade_level, IT_WEAPON_EMP_BOMB_LAYER)
    {
        ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f);
        m_time_last_fired = -1.0f / ms_fire_rate[GetUpgradeLevel()];
    }
    virtual ~EMPBombLayer ();

    inline Uint32 ActiveEMPBombCount () const
    {
        return m_active_emp_bomb_set.size();
    }

    void ActiveEMPBombDestroyed (EMPBomb *active_emp_bomb);

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float ReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_fire_rate[GetUpgradeLevel()];
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

    typedef std::set<EMPBomb *> ActiveEMPBombSet;
    typedef ActiveEMPBombSet::iterator ActiveEMPBombSetIterator;

    static Float const ms_muzzle_speed[UPGRADE_LEVEL_COUNT];
    static Float const ms_required_primary_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_emp_bomb_disable_time_factor[UPGRADE_LEVEL_COUNT];
    static Float const ms_emp_bomb_blast_radius[UPGRADE_LEVEL_COUNT];
    static Float const ms_emp_bomb_health[UPGRADE_LEVEL_COUNT];
    static Float const ms_fire_rate[UPGRADE_LEVEL_COUNT];
    static Uint32 const ms_max_active_emp_bomb_count[UPGRADE_LEVEL_COUNT];

    ActiveEMPBombSet m_active_emp_bomb_set;
    Float m_time_last_fired;
}; // end of class EMPBombLayer
*/

class TractorBeam;

// - Tractor - pushes or pulls stuff to/from the ship
//   - upgrades are to the power, range and size
class Tractor : public Weapon // - primary: trace/TractorBeam / secondary: proximity tractor
{
public:

    Tractor (Uint32 const upgrade_level)
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

    inline bool IsRangeOverridden () const { return m_range_override >= 0.0f; }
    inline Float RangeOverride () const { return m_range_override; }

    inline bool IsStrengthOverridden () const { return m_strength_override >= 0.0f; }
    inline Float GetStrengthOverride () const { return m_strength_override; }

    inline bool IsMaxForceOverridden () const { return m_max_force_override >= 0.0f; }
    inline Float MaxForceOverride () const { return m_max_force_override; }

    inline bool IsBeamRadiusOverridden () const { return m_beam_radius_override >= 0.0f; }
    inline Float BeamRadiusOverride () const { return m_beam_radius_override; }

    inline void SetRangeOverride (Float range_override)
    {
        ASSERT1(range_override >= 0.0f);
        m_range_override = range_override;
    }
    inline void SetStrengthOverride (Float strength_override)
    {
        ASSERT1(strength_override >= 0.0f);
        m_strength_override = strength_override;
    }
    inline void SetMaxForceOverride (Float max_force_override)
    {
        ASSERT1(max_force_override >= 0.0f);
        m_max_force_override = max_force_override;
    }
    inline void SetBeamRadiusOverride (Float beam_radius_override)
    {
        ASSERT1(beam_radius_override >= 0.0f);
        m_beam_radius_override = beam_radius_override;
    }

    inline void ClearRangeOverride () { m_range_override = -1.0f; }
    inline void ClearStrengthOverride () { m_strength_override = -1.0f; }
    inline void ClearMaxForceOverride () { m_max_force_override = -1.0f; }
    inline void ClearBeamRadiusOverride () { m_beam_radius_override = -1.0f; }

    inline void SetTractorBeam (TractorBeam *const tractor_beam)
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

    virtual Float PowerToBeUsedBasedOnInputs (
        Float time,
        Float frame_dt) const;

    virtual bool Activate (
        Float power,
        Float time,
        Float frame_dt);

private:

    static Float const ms_range[UPGRADE_LEVEL_COUNT];
    static Float const ms_max_power_output_rate[UPGRADE_LEVEL_COUNT];
    static Float const ms_strength[UPGRADE_LEVEL_COUNT];
    static Float const ms_max_force[UPGRADE_LEVEL_COUNT];
    static Float const ms_beam_radius[UPGRADE_LEVEL_COUNT];

    TractorBeam *m_tractor_beam;
    Float m_range_override;
    Float m_strength_override;
    Float m_max_force_override;
    Float m_beam_radius_override;
}; // end of class Tractor

// ///////////////////////////////////////////////////////////////////////////
// enemy weapons
// ///////////////////////////////////////////////////////////////////////////

class SlowBulletGun : public Weapon
{
public:

    static Float const ms_impact_damage[UPGRADE_LEVEL_COUNT];
    static Float const ms_muzzle_speed[UPGRADE_LEVEL_COUNT];
    static Float const ms_range[UPGRADE_LEVEL_COUNT];
    static Float const ms_required_primary_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_fire_rate[UPGRADE_LEVEL_COUNT];

    SlowBulletGun (Uint32 const upgrade_level)
        :
        Weapon(upgrade_level, IT_ENEMY_WEAPON_SLOW_BULLET_GUN)
    {
        ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f);
        m_time_last_fired = -1.0f / ms_fire_rate[GetUpgradeLevel()];
    }
    virtual ~SlowBulletGun () { }

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float ReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_fire_rate[GetUpgradeLevel()];
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

    Float m_time_last_fired;
}; // end of class SlowBulletGun

class EnemySpawner : public Weapon
{
public:

    static Float const ms_muzzle_speed[UPGRADE_LEVEL_COUNT];
    static Float const ms_required_primary_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_fire_rate[UPGRADE_LEVEL_COUNT];

    EnemySpawner (Uint32 const upgrade_level)
        :
        Weapon(upgrade_level, IT_ENEMY_WEAPON_ENEMY_SPAWNER)
    {
        ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f);
        m_time_last_fired = -1.0f / ms_fire_rate[GetUpgradeLevel()];
        SetEnemySpawnType(ET_INTERLOPER);
        ClearFireRateOverride();
    }
    virtual ~EnemySpawner () { }

    inline EntityType EnemySpawnType () const { return m_enemy_spawn_type; }
    inline bool IsFireRateOverridden () const { return m_fire_rate_override > 0.0f; }
    inline Float FireRateOverride () const { return m_fire_rate_override; }

    inline void SetEnemySpawnType (EntityType enemy_spawn_type)
    {
        ASSERT1(enemy_spawn_type >= ET_ENEMY_SHIP_LOWEST);
        ASSERT1(enemy_spawn_type <= ET_ENEMY_SHIP_HIGHEST);
        m_enemy_spawn_type = enemy_spawn_type;
    }
    inline void SetFireRateOverride (Float fire_rate_override)
    {
        ASSERT1(fire_rate_override > 0.0f);
        m_fire_rate_override = fire_rate_override;
    }

    inline void ClearFireRateOverride () { m_fire_rate_override = -1.0f; }

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float ReadinessStatus (Float time) const
    {
        Float const fire_rate =
            IsFireRateOverridden() ?
            FireRateOverride() :
            ms_fire_rate[GetUpgradeLevel()];
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

    virtual Float PowerToBeUsedBasedOnInputs (
        Float time,
        Float frame_dt) const;

    virtual bool Activate (
        Float power,
        Float time,
        Float frame_dt);

private:

    EntityType m_enemy_spawn_type;
    Float m_fire_rate_override;
    Float m_time_last_fired;
}; // end of class EnemySpawner

} // end of namespace Dis

#endif // !defined(_DIS_WEAPON_HPP_)

