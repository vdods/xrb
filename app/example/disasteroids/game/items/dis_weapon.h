// ///////////////////////////////////////////////////////////////////////////
// dis_weapon.h by Victor Dods, created 2005/11/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_WEAPON_H_)
#define _DIS_WEAPON_H_

#include <set>

#include "dis_powereddevice.h"
#include "xrb_vector.h"

using namespace Xrb;

namespace Dis
{

class Ship;

/*
Weapon design:

Weapon
+---PeaShooter      - primary: shoots Pea               secondary: none
+---Laser           - primary: trace/LaserBeam          secondary: proximity laser
+---FlameThrower    - primary: shoots Fireball          secondary: shoots Napalm
+---GaussGun        - primary: trace/GaussGunTrail      secondary: none
+---GrenadeLauncher - primary: shoots Grenade           secondary: detonates Grenade
+---MineLayer       - primary: shoots Mine              secondary: guides Mine
+---MissileLauncher - primary: shoots Missile           secondary: shoots seeking Missile
+---EMPBombLayer    - primary: shoots EMPBomb           secondary: detonates EMPBomb
+---AutoDestruct    - primary: blows shit up            secondary: none
+---Tractor         - primary: pulls powerups           secondary: pulls anything
*/

class Weapon : public PoweredDevice
{
public:

    Weapon (Uint8 const upgrade_level, ItemType const type)
        :
        PoweredDevice(upgrade_level, type)
    {
        ASSERT1(type >= IT_WEAPON_LOWEST && type <= IT_WEAPON_HIGHEST ||
                type >= IT_ENEMY_WEAPON_LOWEST && type <= IT_ENEMY_WEAPON_HIGHEST)
        m_current_ammo = 0;
        m_max_ammo = 0;
    }
    virtual ~Weapon () { }

    inline void SetInputs (
        Float const primary_input,
        Float const secondary_input,
        FloatVector2 const &muzzle_location,
        FloatVector2 const &muzzle_direction,
        FloatVector2 const &reticle_coordinates)
    {
        ASSERT1(primary_input >= 0.0f && primary_input <= 1.0f)
        ASSERT1(secondary_input >= 0.0f && secondary_input <= 1.0f)
        m_primary_input = primary_input;
        m_secondary_input = secondary_input;
        m_muzzle_location = muzzle_location;
        m_muzzle_direction = muzzle_direction;
        m_reticle_coordinates = reticle_coordinates;
    }
    
    // ///////////////////////////////////////////////////////////////////////
    // public interface methods
    // ///////////////////////////////////////////////////////////////////////

    // returns true iff this weapon requires ammo in addition to power.
    // false effectively means infinite ammo (though it might require extra
    // power to fire the weapon)
    virtual bool GetRequiresAmmo () const = 0;
    virtual Float GetReadinessStatus (Float time) const = 0;
    
protected:

    inline Float GetPrimaryInput () const { return m_primary_input; }
    inline Float GetSecondaryInput () const { return m_secondary_input; }
    inline FloatVector2 const &GetMuzzleLocation () const { return m_muzzle_location; }
    inline FloatVector2 const &GetMuzzleDirection () const { return m_muzzle_direction; }
    inline FloatVector2 const &GetReticleCoordinates () const { return m_reticle_coordinates; }

    inline Uint32 GetCurrentAmmo () const
    {
        return m_current_ammo;
    }
    inline Uint32 GetMaxAmmo () const
    {
        return m_max_ammo;
    }
    
    inline void SetCurrentAmmo (Uint32 const current_ammo) 
    {
        m_current_ammo = Min(current_ammo, m_max_ammo);
    }
    
    inline void AddAmmo (Uint32 const ammo_to_add)
    {
        // protect against the ammo amount wrapping around
        Uint32 new_ammo = m_current_ammo + ammo_to_add;
        if (new_ammo < m_current_ammo || new_ammo > m_max_ammo)
            m_current_ammo = m_max_ammo;
        else
            m_current_ammo = new_ammo;
    }
    void RemoveAmmo (Uint32 const ammo_to_remove)
    {
        Uint32 new_ammo = m_current_ammo -= ammo_to_remove;
        // protect against the ammo amount wrapping around
        ASSERT1(new_ammo < m_current_ammo)
        m_current_ammo = new_ammo;
    }
        
private:

    Uint32 m_current_ammo;
    Uint32 m_max_ammo;

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
class PeaShooter : public Weapon      //- primary: shoots Pea / secondary: none
{
public:

    PeaShooter (Uint32 const upgrade_level)
        :
        Weapon(upgrade_level, IT_WEAPON_PEA_SHOOTER)
    {
        ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f)
        m_time_last_fired = -1.0f / ms_fire_rate[GetUpgradeLevel()];
    }
    virtual ~PeaShooter () { }

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual bool GetRequiresAmmo () const
    {
        return false;
    }
    virtual Float GetReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_fire_rate[GetUpgradeLevel()];
        Float const time_since_last_fire = time - m_time_last_fired;
        ASSERT1(cycle_time > 0.0f)
        ASSERT1(time_since_last_fire >= 0.0f)
        if (time_since_last_fire > cycle_time)
            return 1.0f;
        else
            return time_since_last_fire / cycle_time;
    }
        
    // ///////////////////////////////////////////////////////////////////////
    // PoweredDevice interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual Float GetPowerToBeUsedBasedOnInputs (
        Float time,
        Float frame_dt) const;

    virtual bool Activate (
        Float power,
        Float time,
        Float frame_dt);

private:

    static Float const ms_impact_damage[UPGRADE_LEVEL_COUNT];
    static Float const ms_muzzle_speed[UPGRADE_LEVEL_COUNT];
    static Float const ms_range[UPGRADE_LEVEL_COUNT];
    static Float const ms_required_primary_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_fire_rate[UPGRADE_LEVEL_COUNT];
    Float m_time_last_fired;
}; // end of class PeaShooter

class LaserBeam;

// - lasers
//   * upgrades will be to the power and range (the lasers will not harm the
//     player by default)
class Laser : public Weapon // - primary: trace/LaserBeam / secondary: proximity laser
{
public:

    Laser (Uint32 const upgrade_level)
        :
        Weapon(upgrade_level, IT_WEAPON_LASER)
    {
        m_laser_beam = NULL;
    }
    virtual ~Laser () { }

    inline void SetLaserBeam (LaserBeam *const laser_beam)
    {
        ASSERT1(laser_beam != NULL)
        m_laser_beam = laser_beam;
    }
    
    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual bool GetRequiresAmmo () const
    {
        return false;
    }
    virtual Float GetReadinessStatus (Float time) const
    {
        return 1.0f;
    }
    
    // ///////////////////////////////////////////////////////////////////////
    // PoweredDevice interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual Float GetPowerToBeUsedBasedOnInputs (
        Float time,
        Float frame_dt) const;

    virtual bool Activate (
        Float power,
        Float time,
        Float frame_dt);
        
private:

    static Float const ms_range[UPGRADE_LEVEL_COUNT];
    static Float const ms_max_primary_power_output_rate[UPGRADE_LEVEL_COUNT];
    static Float const ms_damage_rate[UPGRADE_LEVEL_COUNT];
    static Float const ms_required_secondary_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_beam_radius[UPGRADE_LEVEL_COUNT];

    LaserBeam *m_laser_beam;
}; // end of class Laser

// - flame thrower (will harm the player by default)
//   * upgrades will be to the power and range to make the fire
//     not harm you
class FlameThrower : public Weapon // - primary: shoots Fireball / secondary: shoots Napalm
{
public:

    FlameThrower (Uint32 const upgrade_level)
        :
        Weapon(upgrade_level, IT_WEAPON_FLAME_THROWER)
    {
        ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f)
        m_time_last_fired = -1.0f / ms_fire_rate[GetUpgradeLevel()];
    }
    virtual ~FlameThrower () { }

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual bool GetRequiresAmmo () const
    {
        return false;
    }
    virtual Float GetReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_fire_rate[GetUpgradeLevel()];
        Float const time_since_last_fire = time - m_time_last_fired;
        ASSERT1(cycle_time > 0.0f)
        ASSERT1(time_since_last_fire >= 0.0f)
        if (time_since_last_fire > cycle_time)
            return 1.0f;
        else
            return time_since_last_fire / cycle_time;
    }
    
    // ///////////////////////////////////////////////////////////////////////
    // PoweredDevice interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual Float GetPowerToBeUsedBasedOnInputs (
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
        ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f)
        m_time_last_fired = -1.0f / ms_fire_rate[GetUpgradeLevel()];
        ClearImpactDamageOverride();
    }
    virtual ~GaussGun () { }

    inline bool GetIsImpactDamageOverridden () const { return m_impact_damage_override >= 0.0f; }
    inline Float GetImpactDamageOverride () const { return m_impact_damage_override; }

    inline void SetImpactDamageOverride (Float impact_damage_override)
    {
        ASSERT1(impact_damage_override >= 0.0f)
        m_impact_damage_override = impact_damage_override;
    }

    inline void ClearImpactDamageOverride () { m_impact_damage_override = -1.0f; }
        
    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual bool GetRequiresAmmo () const
    {
        return false;
    }
    virtual Float GetReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_fire_rate[GetUpgradeLevel()];
        Float const time_since_last_fire = time - m_time_last_fired;
        ASSERT1(cycle_time > 0.0f)
        ASSERT1(time_since_last_fire >= 0.0f)
        if (time_since_last_fire > cycle_time)
            return 1.0f;
        else
            return time_since_last_fire / cycle_time;
    }
    
    // ///////////////////////////////////////////////////////////////////////
    // PoweredDevice interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual Float GetPowerToBeUsedBasedOnInputs (
        Float time,
        Float frame_dt) const;

    virtual bool Activate (
        Float power,
        Float time,
        Float frame_dt);
        
private:

    Float m_time_last_fired;
    Float m_impact_damage_override;
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
        ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f)
        m_time_last_fired = -1.0f / ms_fire_rate[GetUpgradeLevel()];
    }
    virtual ~GrenadeLauncher ();

    inline Uint32 GetActiveGrenadeCount () const
    {
        return m_active_grenade_set.size();
    }
    
    void ActiveGrenadeDestroyed (Grenade *active_grenade);
    
    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual bool GetRequiresAmmo () const
    {
        return false;
    }
    virtual Float GetReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_fire_rate[GetUpgradeLevel()];
        Float const time_since_last_fire = time - m_time_last_fired;
        ASSERT1(cycle_time > 0.0f)
        ASSERT1(time_since_last_fire >= 0.0f)
        if (time_since_last_fire > cycle_time)
            return 1.0f;
        else
            return time_since_last_fire / cycle_time;
    }
    
    // ///////////////////////////////////////////////////////////////////////
    // PoweredDevice interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual Float GetPowerToBeUsedBasedOnInputs (
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

class Mine;

// - mine layer
//   you can only have up to X mines active in the world at any given time.
//   - level 0 : no seeking, dumb triggering, no ambient velocity matching (2 mines)
//   - level 1 : no seeking, dumb triggering, uses ambient velocity matching (3 mines)
//   - level 2 : dumb seeking, smart triggering, uses ambient velocity matching (4 mines)
//   - level 3 : smart seeking, smart triggering, uses ambient velocity matching (6 mines)
class MineLayer : public Weapon // - primary: shoots Mine / secondary: guides Mine
{
public:

    MineLayer (Uint32 const upgrade_level)
        :
        Weapon(upgrade_level, IT_WEAPON_MINE_LAYER)
    {
        ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f)
        m_time_last_fired = -1.0f / ms_fire_rate[GetUpgradeLevel()];
    }
    virtual ~MineLayer ();

    inline Uint32 GetActiveMineCount () const
    {
        return m_active_mine_set.size();
    }
    
    void ActiveMineDestroyed (Mine *active_mine);
    
    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual bool GetRequiresAmmo () const
    {
        return false;
    }
    virtual Float GetReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_fire_rate[GetUpgradeLevel()];
        Float const time_since_last_fire = time - m_time_last_fired;
        ASSERT1(cycle_time > 0.0f)
        ASSERT1(time_since_last_fire >= 0.0f)
        if (time_since_last_fire > cycle_time)
            return 1.0f;
        else
            return time_since_last_fire / cycle_time;
    }
    
    // ///////////////////////////////////////////////////////////////////////
    // PoweredDevice interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual Float GetPowerToBeUsedBasedOnInputs (
        Float time,
        Float frame_dt) const;

    virtual bool Activate (
        Float power,
        Float time,
        Float frame_dt);
        
private:

    typedef std::set<Mine *> ActiveMineSet;
    typedef ActiveMineSet::iterator ActiveMineSetIterator;

    static Float const ms_muzzle_speed[UPGRADE_LEVEL_COUNT];
    static Float const ms_required_primary_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_mine_damage_to_inflict[UPGRADE_LEVEL_COUNT];
    static Float const ms_mine_damage_radius[UPGRADE_LEVEL_COUNT];
    static Float const ms_mine_health[UPGRADE_LEVEL_COUNT];
    static Float const ms_fire_rate[UPGRADE_LEVEL_COUNT];
    static Uint32 const ms_max_active_mine_count[UPGRADE_LEVEL_COUNT];

    ActiveMineSet m_active_mine_set;
    Float m_time_last_fired;
}; // end of class MineLayer

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
        ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f)
        m_time_last_fired = -1.0f / ms_fire_rate[GetUpgradeLevel()];
    }
    virtual ~MissileLauncher () { }

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual bool GetRequiresAmmo () const
    {
        return false;
    }
    virtual Float GetReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_fire_rate[GetUpgradeLevel()];
        Float const time_since_last_fire = time - m_time_last_fired;
        ASSERT1(cycle_time > 0.0f)
        ASSERT1(time_since_last_fire >= 0.0f)
        if (time_since_last_fire > cycle_time)
            return 1.0f;
        else
            return time_since_last_fire / cycle_time;
    }
    
    // ///////////////////////////////////////////////////////////////////////
    // PoweredDevice interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual Float GetPowerToBeUsedBasedOnInputs (
        Float time,
        Float frame_dt) const;

    virtual bool Activate (
        Float power,
        Float time,
        Float frame_dt);
        
private:

    static Float const ms_muzzle_speed[UPGRADE_LEVEL_COUNT];
    static Float const ms_required_primary_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_required_secondary_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_missile_time_to_live[UPGRADE_LEVEL_COUNT];
    static Float const ms_missile_power[UPGRADE_LEVEL_COUNT];
    static Float const ms_missile_health[UPGRADE_LEVEL_COUNT];
    static Float const ms_fire_rate[UPGRADE_LEVEL_COUNT];

    Float m_time_last_fired;
}; // end of class MissileLauncher

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
        ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f)
        m_time_last_fired = -1.0f / ms_fire_rate[GetUpgradeLevel()];
    }
    virtual ~EMPBombLayer ();

    inline Uint32 GetActiveEMPBombCount () const
    {
        return m_active_emp_bomb_set.size();
    }
    
    void ActiveEMPBombDestroyed (EMPBomb *active_emp_bomb);
    
    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual bool GetRequiresAmmo () const
    {
        // TODO: this weapon should use ammo
        return false;
    }
    virtual Float GetReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_fire_rate[GetUpgradeLevel()];
        Float const time_since_last_fire = time - m_time_last_fired;
        ASSERT1(cycle_time > 0.0f)
        ASSERT1(time_since_last_fire >= 0.0f)
        if (time_since_last_fire > cycle_time)
            return 1.0f;
        else
            return time_since_last_fire / cycle_time;
    }
    
    // ///////////////////////////////////////////////////////////////////////
    // PoweredDevice interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual Float GetPowerToBeUsedBasedOnInputs (
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

// - AutoDestruct - suicide attack which causes a huge explosion and does
//   lots and lots of damage
class AutoDestruct : public Weapon // - primary: blows shit up
{
public:

    AutoDestruct (Uint32 const upgrade_level)
        :
        Weapon(upgrade_level, IT_WEAPON_AUTO_DESTRUCT)
    {
        m_triggered_time = -1.0f;
    }
    virtual ~AutoDestruct () { }

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual bool GetRequiresAmmo () const
    {
        return false;
    }
    virtual Float GetReadinessStatus (Float time) const
    {
        ASSERT1(ms_trigger_countdown_time[GetUpgradeLevel()] > 0.0f)
        if (m_triggered_time >= 0.0f)
            return Max(0.0f, 1.0f - (time - m_triggered_time) / ms_trigger_countdown_time[GetUpgradeLevel()]);
        else
            return 1.0f;
    }
    
    // ///////////////////////////////////////////////////////////////////////
    // PoweredDevice interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual Float GetPowerToBeUsedBasedOnInputs (
        Float time,
        Float frame_dt) const;

    virtual bool Activate (
        Float power,
        Float time,
        Float frame_dt);
        
private:

    static Float const ms_trigger_countdown_time[UPGRADE_LEVEL_COUNT];
    static Float const ms_explosion_power[UPGRADE_LEVEL_COUNT];

    Float m_triggered_time;
}; // end of class AutoDestruct

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
        m_beam_radius_override = -1.0f;
        m_acceleration_override = -1.0f;
    }
    virtual ~Tractor () { }

    inline bool GetIsBeamRadiusOverridden () const { return m_beam_radius_override >= 0.0f; }
    inline Float GetBeamRadiusOverride () const { return m_beam_radius_override; }

    inline bool GetIsAccelerationOverridden () const { return m_acceleration_override >= 0.0f; }
    inline Float GetAccelerationOverride () const { return m_acceleration_override; }
    
    inline void SetBeamRadiusOverride (Float beam_radius_override)
    {
        ASSERT1(beam_radius_override >= 0.0f)
        m_beam_radius_override = beam_radius_override;
    }
    inline void SetAccelerationOverride (Float acceleration_override)
    {
        ASSERT1(acceleration_override >= 0.0f)
        m_acceleration_override = acceleration_override;
    }

    inline void ClearBeamRadiusOverride () { m_beam_radius_override = -1.0f; }
    inline void ClearAccelerationOverride () { m_acceleration_override = -1.0f; }
    
    inline void SetTractorBeam (TractorBeam *const tractor_beam)
    {
        ASSERT1(tractor_beam != NULL)
        m_tractor_beam = tractor_beam;
    }
    
    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual bool GetRequiresAmmo () const
    {
        return false;
    }
    virtual Float GetReadinessStatus (Float time) const
    {
        return 1.0f;
    }
    
    // ///////////////////////////////////////////////////////////////////////
    // PoweredDevice interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual Float GetPowerToBeUsedBasedOnInputs (
        Float time,
        Float frame_dt) const;

    virtual bool Activate (
        Float power,
        Float time,
        Float frame_dt);
        
private:

    static Float const ms_range[UPGRADE_LEVEL_COUNT];
    static Float const ms_max_power_output_rate[UPGRADE_LEVEL_COUNT];
    static Float const ms_acceleration[UPGRADE_LEVEL_COUNT];
    static Float const ms_beam_radius[UPGRADE_LEVEL_COUNT];

    TractorBeam *m_tractor_beam;
    Float m_beam_radius_override;
    Float m_acceleration_override;
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
        ASSERT1(ms_fire_rate[GetUpgradeLevel()] > 0.0f)
        m_time_last_fired = -1.0f / ms_fire_rate[GetUpgradeLevel()];
    }
    virtual ~SlowBulletGun () { }

    // ///////////////////////////////////////////////////////////////////////
    // Weapon interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual bool GetRequiresAmmo () const
    {
        return false;
    }
    virtual Float GetReadinessStatus (Float time) const
    {
        Float const cycle_time = 1.0f / ms_fire_rate[GetUpgradeLevel()];
        Float const time_since_last_fire = time - m_time_last_fired;
        ASSERT1(cycle_time > 0.0f)
        ASSERT1(time_since_last_fire >= 0.0f)
        if (time_since_last_fire > cycle_time)
            return 1.0f;
        else
            return time_since_last_fire / cycle_time;
    }
    
    // ///////////////////////////////////////////////////////////////////////
    // PoweredDevice interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual Float GetPowerToBeUsedBasedOnInputs (
        Float time,
        Float frame_dt) const;

    virtual bool Activate (
        Float power,
        Float time,
        Float frame_dt);

private:

    Float m_time_last_fired;
}; // end of class SlowBulletGun


} // end of namespace Dis

#endif // !defined(_DIS_WEAPON_H_)

