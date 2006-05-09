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
    EMP bomb layer: occasionally shoot out an EMB bomb and detonate it
    tractor: when there are missiles/grenades/ballistics in front, they
             are deflected away with the tractor
    tractor: when player is close enough, tractor some nearby asteroids into
             a collision course with the player
    tractor: tractor the player closer so we can throw some flames on it
    tractor: tractor the player towards a Devourment
    enemy spawner: intermittent barrages of certain types of enemies
    mine layer: occasionally lay a mine

------------------------------------------------------------------------------

state machine:

PickWanderDirection
Wander -- no collision avoidance -- Demis don't care about pittling asteroids



GaussGunPause -> GaussGunAim
GaussGunAim -> GaussGunFire
GaussGunFire ->

FlameThrowStart
FlameThrowContinue

MissileLaunchStart
MissileLaunchContinue

GuidedMissileLaunchStart
GuidedMissileLaunchContinue



*/

class Demi : public EnemyShip
{
public:

    Demi (Uint8 enemy_level);
    virtual ~Demi ();

    virtual void Think (Float time, Float frame_dt);

    // ///////////////////////////////////////////////////////////////////////
    // Ship interface methods
    // ///////////////////////////////////////////////////////////////////////

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
    void Stalk (Float time, Float frame_dt);
    void MoveToAttackRange (Float time, Float frame_dt);
    void Teleport (Float time, Float frame_dt);

    void MatchVelocity (FloatVector2 const &velocity, Float frame_dt);
    void AimWeapon (FloatVector2 const &target_position);

    static Float const ms_max_health[ENEMY_LEVEL_COUNT];
    static Float const ms_engine_thrust[ENEMY_LEVEL_COUNT];
    static Float const ms_scale_factor[ENEMY_LEVEL_COUNT];
    static Float const ms_baseline_first_moment[ENEMY_LEVEL_COUNT];
    static Float const ms_damage_dissipation_rate[ENEMY_LEVEL_COUNT];
    static Float const ms_alarm_distance[ENEMY_LEVEL_COUNT];
    static Float const ms_stalk_minimum_distance[ENEMY_LEVEL_COUNT];
    static Float const ms_stalk_maximum_distance[ENEMY_LEVEL_COUNT];
    static Float const ms_move_relative_velocity[ENEMY_LEVEL_COUNT];
    static Float const ms_wander_speed[ENEMY_LEVEL_COUNT];

    typedef void (Demi::*ThinkState)(Float time, Float frame_dt);

    ThinkState m_think_state;
    Float m_next_wander_time;
    Float m_wander_angle;
    Float m_wander_angle_low_pass;
    EntityReference<Ship> m_target;
    Weapon *m_weapon;
}; // end of class Demi

} // end of namespace Dis

#endif // !defined(_DIS_DEMI_H_)

