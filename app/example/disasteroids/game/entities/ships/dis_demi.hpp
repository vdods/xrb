// ///////////////////////////////////////////////////////////////////////////
// dis_demi.hpp by Victor Dods, created 2006/05/07
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_DEMI_HPP_)
#define _DIS_DEMI_HPP_

#include "dis_enemyship.hpp"

using namespace Xrb;

namespace Dis
{

/*
notes:

front port:
    gauss gun
    flame thrower
    missile launcher

side ports:
    tractor
    flame thrower
    missile launcher

aft port:
    enemy spawner
    flame thrower
    missile launcher

actions:
    gauss gun fire: if the player trespasses in a certain area in front of us,
                    pause, slowly aim, then fire dramatically
    gauss gun fire: if the player gets too far away, initiate the gauss gun
                    fire sequence (so the player will just see a reticle approaching it)
x   flame thrower: if the player gets close enough in front, spray fire
                   left/right for a few seconds
x   flame thrower: short blast of flames out of front/side/aft ports simultaneously
x   flame thrower: spinning flame throwing
x   missile launcher: intermittent barrages of non-guided missiles (directed
                      at the player) -- out of the front port only
x   missile launcher: spinning blasts of non-guided missiles out all ports
x   missile launcher: spinning blasts of guided missiles out of
                      all weapon ports (which seek only the player, not enemy ships)
x   tractor: when there are missiles/grenades/ballistics in front, they
             are deflected away with the tractor
x   tractor: tractor the player closer so we can throw some flames on it
    tractor: when player is close enough, tractor some nearby asteroids into
             a collision course with the player
    tractor: throw interlopers at the player
    tractor: tractor the player towards a Devourment
x   enemy spawner: intermittent barrages of certain types of enemies
    asteroid destruction: if we collide head-on with a large enough asteroid,
                          somehow destroy it
    blah: if player gets too far away, charge at it

------------------------------------------------------------------------------

state machine:

PickWanderDirection -> Wander
Wander - no collision avoidance -> Stalk

Stalk - has a target.  moving around for a better position
    if the player is close enough, goto
        FlameThrowSweepStart or
        MissileLaunchStart or
        SpinningInterloperSpawn
    if the player is a mid distance away, goto
        GaussGunStartAim or
        FlameThrowBlastStart or
        SpinningFlameThrow
        SpinningMissileLaunch
        SpinningGuidedMissileLaunch
        InterloperSpawnBlastStart
        TractorTargetCloserStart
    if the player is a long distance away, goto
        GaussGunStartAim
        TractorTargetCloserStart
        MissileLaunchStart

GaussGunPause -> GaussGunAim
GaussGunAim -> GaussGunFire
GaussGunFire -> Stalk

FlameThrowStart -> FlameThrowContinue
FlameThrowContinue -> Stalk

MissileLaunchStart -> MissileLaunchContinue
MissileLaunchContinue -> Stalk

GuidedMissileLaunchStart -> GuidedMissileLaunchContinue
GuidedMissileLaunchContinue -> Stalk

*/

class AdvancedTractor;
class EnemySpawner;
class FlameThrower;
class GaussGun;
class MissileLauncher;
class ReticleEffect;
class TractorBeam;

class Demi : public EnemyShip
{
public:

    static Float const ms_max_health[ENEMY_LEVEL_COUNT];
    static Float const ms_engine_thrust[ENEMY_LEVEL_COUNT];
    static Float const ms_max_angular_velocity[ENEMY_LEVEL_COUNT];
    static Float const ms_scale_factor[ENEMY_LEVEL_COUNT];
    static Float const ms_baseline_mass[ENEMY_LEVEL_COUNT];
    static Float const ms_damage_dissipation_rate[ENEMY_LEVEL_COUNT];
    static Float const ms_wander_speed[ENEMY_LEVEL_COUNT];
    static Float const ms_weapon_fov[ENEMY_LEVEL_COUNT];
    static Float const ms_spinning_attack_acceleration_duration[ENEMY_LEVEL_COUNT];
    static Float const ms_spinning_attack_duration[ENEMY_LEVEL_COUNT];
    static Float const ms_flame_thrower_max_damage_per_fireball[ENEMY_LEVEL_COUNT];
    static Float const ms_flame_thrower_final_fireball_size[ENEMY_LEVEL_COUNT];
    static Float const ms_gauss_gun_impact_damage[ENEMY_LEVEL_COUNT];
    static Float const ms_gauss_gun_aim_error_radius[ENEMY_LEVEL_COUNT];
    static Float const ms_gauss_gun_aim_max_speed[ENEMY_LEVEL_COUNT];
    static Float const ms_gauss_gun_reticle_scale_factor[ENEMY_LEVEL_COUNT];
    static Float const ms_gauss_gun_max_duration[ENEMY_LEVEL_COUNT];
    static Float const ms_flame_throw_sweep_duration[ENEMY_LEVEL_COUNT];
    static Float const ms_flame_throw_blast_duration[ENEMY_LEVEL_COUNT];
    static Float const ms_missile_launch_duration[ENEMY_LEVEL_COUNT];
    static Float const ms_enemy_spawn_blast_duration[ENEMY_LEVEL_COUNT];
    static Float const ms_tractor_target_closer_duration[ENEMY_LEVEL_COUNT];
    static Float const ms_tractor_range[ENEMY_LEVEL_COUNT];
    static Float const ms_tractor_strength[ENEMY_LEVEL_COUNT];
    static Float const ms_tractor_max_force[ENEMY_LEVEL_COUNT];
    static Float const ms_tractor_beam_radius[ENEMY_LEVEL_COUNT];
    static Float const ms_target_near_range_distance[ENEMY_LEVEL_COUNT];
    static Float const ms_target_mid_range_distance[ENEMY_LEVEL_COUNT];
    static Float const ms_pause_duration[ENEMY_LEVEL_COUNT];
    static Float const ms_health_powerup_amount_to_spawn[ENEMY_LEVEL_COUNT];
    static Float const ms_option_powerup_spawn_density_min[ENEMY_LEVEL_COUNT];
    static Float const ms_option_powerup_spawn_density_max[ENEMY_LEVEL_COUNT];

    Demi (Uint8 enemy_level);
    virtual ~Demi ();

    virtual void Think (Float time, Float frame_dt);
    virtual void Die (
        Entity *killer,
        Entity *kill_medium,
        FloatVector2 const &kill_location,
        FloatVector2 const &kill_normal,
        Float kill_force,
        DamageType kill_type,
        Float time,
        Float frame_dt);

    // ///////////////////////////////////////////////////////////////////////
    // Ship interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual FloatVector2 MuzzleLocation (Weapon const *weapon) const;
    virtual FloatVector2 MuzzleDirection (Weapon const *weapon) const;
    virtual Float MaxAngularVelocity () const
    {
        return ms_max_angular_velocity[EnemyLevel()];
    }
    virtual Float ShipScaleFactor () const
    {
        return ms_scale_factor[EnemyLevel()];
    }
    virtual Float ShipBaselineMass () const
    {
        return ms_baseline_mass[EnemyLevel()];
    }

    virtual bool TakePowerup (Powerup *powerup, Float time, Float frame_dt);

    // ///////////////////////////////////////////////////////////////////////
    // EnemyShip interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual void SetTarget (Mortal *target);

protected:

    virtual void ResetInputs ();

private:


    Float NormalizedPortWeaponPrimaryInput () const
    {
        return static_cast<Float>(m_port_weapon_primary_input) /
               static_cast<Float>(UINT8_UPPER_BOUND);
    }
    Float NormalizedPortWeaponSecondaryInput () const
    {
        return static_cast<Float>(m_port_weapon_secondary_input) /
               static_cast<Float>(UINT8_UPPER_BOUND);
    }
    Float NormalizedStarboardWeaponPrimaryInput () const
    {
        return static_cast<Float>(m_starboard_weapon_primary_input) /
               static_cast<Float>(UINT8_UPPER_BOUND);
    }
    Float NormalizedStarboardWeaponSecondaryInput () const
    {
        return static_cast<Float>(m_starboard_weapon_secondary_input) /
               static_cast<Float>(UINT8_UPPER_BOUND);
    }
    Float NormalizedAftWeaponPrimaryInput () const
    {
        return static_cast<Float>(m_aft_weapon_primary_input) /
               static_cast<Float>(UINT8_UPPER_BOUND);
    }
    Float NormalizedAftWeaponSecondaryInput () const
    {
        return static_cast<Float>(m_aft_weapon_secondary_input) /
               static_cast<Float>(UINT8_UPPER_BOUND);
    }

    void SetPortReticleCoordinates (FloatVector2 const &reticle_coordinates)
    {
        ASSERT1(Math::IsFinite(reticle_coordinates[Dim::X]));
        ASSERT1(Math::IsFinite(reticle_coordinates[Dim::Y]));
        m_port_reticle_coordinates = reticle_coordinates;
    }
    void SetPortWeaponPrimaryInput (Uint8 port_weapon_primary_input)
    {
        m_port_weapon_primary_input = port_weapon_primary_input;
    }
    void SetPortWeaponSecondaryInput (Uint8 port_weapon_secondary_input)
    {
        m_port_weapon_secondary_input = port_weapon_secondary_input;
    }
    void SetStarboardReticleCoordinates (FloatVector2 const &reticle_coordinates)
    {
        ASSERT1(Math::IsFinite(reticle_coordinates[Dim::X]));
        ASSERT1(Math::IsFinite(reticle_coordinates[Dim::Y]));
        m_starboard_reticle_coordinates = reticle_coordinates;
    }
    void SetStarboardWeaponPrimaryInput (Uint8 starboard_weapon_primary_input)
    {
        m_starboard_weapon_primary_input = starboard_weapon_primary_input;
    }
    void SetStarboardWeaponSecondaryInput (Uint8 starboard_weapon_secondary_input)
    {
        m_starboard_weapon_secondary_input = starboard_weapon_secondary_input;
    }
    void SetAftWeaponPrimaryInput (Uint8 aft_weapon_primary_input)
    {
        m_aft_weapon_primary_input = aft_weapon_primary_input;
    }
    void SetAftWeaponSecondaryInput (Uint8 aft_weapon_secondary_input)
    {
        m_aft_weapon_secondary_input = aft_weapon_secondary_input;
    }

    // main think states
    void PickWanderDirection (Float time, Float frame_dt);
    void Wander (Float time, Float frame_dt);
    void Stalk (Float time, Float frame_dt);
    void PauseStart (Float time, Float frame_dt);
    void PauseContinue (Float time, Float frame_dt);
    void ChargeStart (Float time, Float frame_dt);
    void ChargeAccelerate (Float time, Float frame_dt);
    void ChargeCoast (Float time, Float frame_dt);
    void ChargeDecelerate (Float time, Float frame_dt);
    void GaussGunStartAim (Float time, Float frame_dt);
    void GaussGunContinueAim (Float time, Float frame_dt);
    void GaussGunFire (Float time, Float frame_dt);
    void FlameThrowSweepStart (Float time, Float frame_dt);
    void FlameThrowSweepContinue (Float time, Float frame_dt);
    void FlameThrowBlastStart (Float time, Float frame_dt);
    void FlameThrowBlastContinue (Float time, Float frame_dt);
    void MissileLaunchStart (Float time, Float frame_dt);
    void MissileLaunchContinue (Float time, Float frame_dt);
    void InterloperSpawnBlastStart (Float time, Float frame_dt);
    void InterloperSpawnBlastContinue (Float time, Float frame_dt);
    void ShadeSpawnBlastStart (Float time, Float frame_dt);
    void ShadeSpawnBlastContinue (Float time, Float frame_dt);
    void RevulsionSpawnBlastStart (Float time, Float frame_dt);
    void RevulsionSpawnBlastContinue (Float time, Float frame_dt);
    void SpinningFlameThrow (Float time, Float frame_dt);
    void SpinningMissileLaunch (Float time, Float frame_dt);
    void SpinningGuidedMissileLaunch (Float time, Float frame_dt);
    void SpinningInterloperSpawn (Float time, Float frame_dt);
    void SpinningShadeSpawn (Float time, Float frame_dt);
    void SpinningRevulsionSpawn (Float time, Float frame_dt);
    void SpinningAttackStart (Float time, Float frame_dt);
    void SpinningAttackAccelerate (Float time, Float frame_dt);
    void SpinningAttackFire (Float time, Float frame_dt);
    void SpinningAttackDecelerate (Float time, Float frame_dt);
    void TractorTargetCloserStart (Float time, Float frame_dt);
    void TractorTargetCloserContinue (Float time, Float frame_dt);

    // tractor think states
    void PortTractorDeflectStuff (Float time, Float frame_dt);
    void StarboardTractorDeflectStuff (Float time, Float frame_dt);
    void PortTractorPullTargetCloser (Float time, Float frame_dt);
    void StarboardTractorPullTargetCloser (Float time, Float frame_dt);
    void PortTractorSuckUpPowerups (Float time, Float frame_dt);
    void StarboardTractorSuckUpPowerups (Float time, Float frame_dt);
    void PortTractorFlingStuffAtTarget (Float time, Float frame_dt);
    void StarboardTractorFlingStuffAtTarget (Float time, Float frame_dt);

    void PickTractorThinkStates ();

    void MatchVelocity (FloatVector2 const &velocity, Float frame_dt, Float max_thrust = -1.0f);

    enum TractorAction
    {
        TA_DEFLECT = 0,
        TA_SUCK_UP_POWERUPS,
        TA_FLING,

        TA_COUNT
    }; // end of enum Demi::TractorAction

    // if deflect is true, search for things that will hurt us.  if false,
    // search for things to fling at the target.
    Entity *FindTractorTarget (
        TractorAction tractor_action,
        FloatVector2 const &muzzle_location,
        FloatVector2 const &muzzle_direction,
        Float time,
        Float frame_dt);

    typedef void (Demi::*ThinkState)(Float time, Float frame_dt);

    struct WeightedThinkState
    {
        ThinkState m_think_state;
        Uint32 m_weight;
    }; // end of struct Demi::WeightedThinkState

    static Float const ms_side_port_angle;

    ThinkState m_think_state;
    ThinkState m_port_tractor_think_state;
    ThinkState m_starboard_tractor_think_state;
    Float m_next_wander_time;
    Float m_wander_angle;
    EntityReference<Ship> m_target;
    Float m_start_time;
    Float m_spin_direction;
    Float m_spin_accelerate_through_angle;
    Float m_spin_acceleration_duration;
    Float m_spin_duration;
    bool m_spinning_attack_uses_secondary_fire;
    FloatVector2 m_charge_velocity;

    FloatVector2 m_port_reticle_coordinates;
    Uint8 m_port_weapon_primary_input;
    Uint8 m_port_weapon_secondary_input;
    FloatVector2 m_starboard_reticle_coordinates;
    Uint8 m_starboard_weapon_primary_input;
    Uint8 m_starboard_weapon_secondary_input;
    Uint8 m_aft_weapon_primary_input;
    Uint8 m_aft_weapon_secondary_input;

    // currently equipped (main) weapon
    Weapon *m_main_weapon;
    // choices for main weapon
    GaussGun *m_gauss_gun;
    EntityReference<ReticleEffect> m_reticle_effect;
    FlameThrower *m_flame_thrower;
    MissileLauncher *m_missile_launcher;

    // currently equipped port-side weapon
    Weapon *m_port_weapon;
    // port-side weapons
    AdvancedTractor *m_port_tractor;
    EntityReference<TractorBeam> m_port_tractor_beam;
    FlameThrower *m_port_flame_thrower;
    MissileLauncher *m_port_missile_launcher;

    // currently equipped starboard-side weapon
    Weapon *m_starboard_weapon;
    // starboard-side weapons
    AdvancedTractor *m_starboard_tractor;
    EntityReference<TractorBeam> m_starboard_tractor_beam;
    FlameThrower *m_starboard_flame_thrower;
    MissileLauncher *m_starboard_missile_launcher;

    // currently equipped aft-port weapon
    Weapon *m_aft_weapon;
    // aft-port weapons
    EnemySpawner *m_aft_enemy_spawner;
    FlameThrower *m_aft_flame_thrower;
    MissileLauncher *m_aft_missile_launcher;
}; // end of class Demi

} // end of namespace Dis

#endif // !defined(_DIS_DEMI_HPP_)

