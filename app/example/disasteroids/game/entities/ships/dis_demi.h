// ///////////////////////////////////////////////////////////////////////////
// dis_demi.h by Victor Dods, created 2006/05/07
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_DEMI_H_)
#define _DIS_DEMI_H_

#include "dis_enemyship.h"

using namespace Xrb;

namespace Dis
{

/*
notes:

front port:
    gauss gun
    flame thrower
    missile launcher
    EMP bomb layer

side ports:
    tractor

back port:
    enemy spawner
    mine layer

actions:
    gauss gun fire: if the player trespasses in a certain area in front of us,
                    pause, slowly aim, then fire dramatically
    flame thrower: if the player gets close enough in front, spray fire
                   left/right for a few seconds
    missile launcher: intermittent barrages of non-guided missiles (directed
                      at the player)
    missile launcher: intermittent barrages of guided missiles (which seek
                      only the player, not enemy ships)
    tractor: when there are missiles/grenades/ballistics in front, they
             are deflected away with the tractor
    tractor: when player is close enough, tractor some nearby asteroids into
             a collision course with the player
    tractor: tractor the player closer so we can throw some flames on it
    tractor: tractor the player towards a Devourment
    enemy spawner: intermittent barrages of certain types of enemies
    ---maybe---
    EMP bomb layer: occasionally shoot out an EMB bomb and detonate it
    mine layer: occasionally lay a mine

------------------------------------------------------------------------------

state machine:

PickWanderDirection -> Wander
Wander - no collision avoidance -> Stalk

Stalk - has a target.  moving around for a better position

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

class FlameThrower;
class GaussGun;
class MissileLauncher;
class ReticleEffect;
class Tractor;

class Demi : public EnemyShip
{
public:

    static Float const ms_max_health[ENEMY_LEVEL_COUNT];
    static Float const ms_engine_thrust[ENEMY_LEVEL_COUNT];
    static Float const ms_scale_factor[ENEMY_LEVEL_COUNT];
    static Float const ms_baseline_first_moment[ENEMY_LEVEL_COUNT];
    static Float const ms_damage_dissipation_rate[ENEMY_LEVEL_COUNT];
    static Float const ms_wander_speed[ENEMY_LEVEL_COUNT];
    static Float const ms_gauss_gun_impact_damage[ENEMY_LEVEL_COUNT];
    static Float const ms_gauss_gun_aim_error_radius[ENEMY_LEVEL_COUNT];
    static Float const ms_gauss_gun_aim_max_speed[ENEMY_LEVEL_COUNT];
    static Float const ms_gauss_gun_reticle_scale_factor[ENEMY_LEVEL_COUNT];

    Demi (Uint8 enemy_level);
    virtual ~Demi ();

    virtual void Think (Float time, Float frame_dt);

    // ///////////////////////////////////////////////////////////////////////
    // Ship interface methods
    // ///////////////////////////////////////////////////////////////////////

//     virtual FloatVector2 GetMuzzleLocation (Weapon const *weapon) const;
//     virtual FloatVector2 GetMuzzleDirection (Weapon const *weapon) const;
    virtual Float GetShipScaleFactor () const
    {
        return ms_scale_factor[GetEnemyLevel()];
    }
    virtual Float GetShipBaselineFirstMoment () const
    {
        return ms_baseline_first_moment[GetEnemyLevel()];
    }

private:

    void PickWanderDirection (Float time, Float frame_dt);
    void Wander (Float time, Float frame_dt);
    void GaussGunStartAim (Float time, Float frame_dt);
    void GaussGunContinueAim (Float time, Float frame_dt);
    void GaussGunFire (Float time, Float frame_dt);

    void MatchVelocity (FloatVector2 const &velocity, Float frame_dt);

    typedef void (Demi::*ThinkState)(Float time, Float frame_dt);

    ThinkState m_think_state;
    Float m_next_wander_time;
    Float m_wander_angle;
    Float m_wander_angle_low_pass;
    EntityReference<Ship> m_target;
    Float m_attack_start_time;

    // currently equipped (main) weapon -- TODO: i don't really like
    // this main weapon stuff.  the think states should handle firing of the weapons
    // instead of Think().
    Weapon *m_main_weapon;
    // choices for main weapon (gauss gun, flame thrower, missile launcher)
    GaussGun *m_gauss_gun;
    EntityReference<ReticleEffect> m_reticle_effect;
    FlameThrower *m_flame_thrower;
    MissileLauncher *m_missile_launcher;
}; // end of class Demi

} // end of namespace Dis

#endif // !defined(_DIS_DEMI_H_)

