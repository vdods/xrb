// ///////////////////////////////////////////////////////////////////////////
// dis_shade.h by Victor Dods, created 2005/12/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_SHADE_H_)
#define _DIS_SHADE_H_

#include "dis_enemyship.h"

using namespace Xrb;

namespace Dis
{

/*
state machine:

PickWanderDirection
    pick a direction/speed to wander in
    goto Wander

Wander
    incrementally accelerate up to the wander direction/speed
    scan area for targets
    if (found a target)
        goto MoveToAttackRange
    else if (collision is imminent)
        pick a direction/speed to avoid the collision
        
    // slowly change the wander direction        

*/

class Shade : public EnemyShip
{
public:

    Shade (Uint8 enemy_level);
    virtual ~Shade ();

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

    // TEMP - move into Ship (or Entity)
    // TEMP - move into Ship (or Entity)
    // TEMP - move into Ship (or Entity)
    Float GetCollisionTime (Entity *entity, Float lookahead_time) const;
    // TEMP - move into Ship (or Entity)
    // TEMP - move into Ship (or Entity)
    // TEMP - move into Ship (or Entity)

    void PickWanderDirection (Float time, Float frame_dt);
    void Wander (Float time, Float frame_dt);
//     void Seek (Float time, Float frame_dt);
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

    typedef void (Shade::*ThinkState)(Float time, Float frame_dt);

    ThinkState m_think_state;
    Float m_next_wander_time;
    Float m_wander_angle;
    Float m_slow_angle;
    EntityReference<Ship> m_target;
    Weapon *m_weapon;
}; // end of class Shade

} // end of namespace Dis

#endif // !defined(_DIS_SHADE_H_)

