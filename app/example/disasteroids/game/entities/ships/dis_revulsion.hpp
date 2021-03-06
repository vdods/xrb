// ///////////////////////////////////////////////////////////////////////////
// dis_revulsion.hpp by Victor Dods, created 2006/01/24
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_REVULSION_HPP_)
#define _DIS_REVULSION_HPP_

#include "dis_enemyship.hpp"

using namespace Xrb;

namespace Dis {

class GaussGun;
class ReticleEffect;

class Revulsion : public EnemyShip
{
public:

    static Float const ms_max_health[ENEMY_LEVEL_COUNT];
    static Float const ms_engine_thrust[ENEMY_LEVEL_COUNT];
    static Float const ms_max_angular_velocity[ENEMY_LEVEL_COUNT];
    static Float const ms_ship_radius[ENEMY_LEVEL_COUNT];
    static Float const ms_baseline_mass[ENEMY_LEVEL_COUNT];
    static Float const ms_damage_dissipation_rate[ENEMY_LEVEL_COUNT];
    static Float const ms_weapon_impact_damage[ENEMY_LEVEL_COUNT];
    static Float const ms_target_aim_angle_flee_limit[ENEMY_LEVEL_COUNT];
    static Float const ms_target_aim_angle_trail_limit[ENEMY_LEVEL_COUNT];
    static Float const ms_preferred_location_distance_tolerance[ENEMY_LEVEL_COUNT];
    static Time::Delta const ms_aim_duration[ENEMY_LEVEL_COUNT];
    static Float const ms_aim_error_radius[ENEMY_LEVEL_COUNT];
    static Float const ms_flee_speed[ENEMY_LEVEL_COUNT];
    static Float const ms_wander_speed[ENEMY_LEVEL_COUNT];

    Revulsion (Uint8 enemy_level);
    virtual ~Revulsion ();

    virtual void Think (Time time, Time::Delta frame_dt);
    virtual void Die (
        Entity *killer,
        Entity *kill_medium,
        FloatVector2 const &kill_location,
        FloatVector2 const &kill_normal,
        Float kill_force,
        DamageType kill_type,
        Time time,
        Time::Delta frame_dt);

    // ///////////////////////////////////////////////////////////////////////
    // Ship interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float MaxAngularVelocity () const
    {
        return ms_max_angular_velocity[EnemyLevel()];
    }
    virtual Float ShipRadius () const
    {
        return ms_ship_radius[EnemyLevel()];
    }
    virtual Float ShipBaselineMass () const
    {
        return ms_baseline_mass[EnemyLevel()];
    }

    // ///////////////////////////////////////////////////////////////////////
    // EnemyShip interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual void SetTarget (Mortal *target);

private:

    void PickWanderDirection (Time time, Time::Delta frame_dt);
    void Wander (Time time, Time::Delta frame_dt);
    void TrailTarget (Time time, Time::Delta frame_dt);
    void StartAimAtTarget (Time time, Time::Delta frame_dt);
    void ContinueAimAtTarget (Time time, Time::Delta frame_dt);
    void FireAtTarget (Time time, Time::Delta frame_dt);
    void FleeTarget (Time time, Time::Delta frame_dt);

    Float TargetAimAngle () const;

    void MatchVelocity (FloatVector2 const &velocity, Time::Delta frame_dt);

    typedef void (Revulsion::*ThinkState)(Time time, Time::Delta frame_dt);

    ThinkState m_think_state;
    Time m_next_wander_time;
    Float m_wander_angle;
    Float m_wander_angle_derivative;
    EntityReference<Ship> m_target;
    EntityReference<ReticleEffect> m_reticle_effect;
    FloatVector2 m_aim_delta;
    Time m_aim_start_time;
    GaussGun *m_weapon;
}; // end of class Revulsion

} // end of namespace Dis

#endif // !defined(_DIS_REVULSION_HPP_)

