// ///////////////////////////////////////////////////////////////////////////
// dis_interloper.hpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_INTERLOPER_HPP_)
#define _DIS_INTERLOPER_HPP_

#include "dis_enemyship.hpp"

using namespace Xrb;

namespace Dis {

class Interloper : public EnemyShip
{
public:

    static Float const ms_max_health[ENEMY_LEVEL_COUNT];
    static Float const ms_engine_thrust[ENEMY_LEVEL_COUNT];
    static Float const ms_max_angular_velocity[ENEMY_LEVEL_COUNT];
    static Float const ms_ship_radius[ENEMY_LEVEL_COUNT];
    static Float const ms_baseline_mass[ENEMY_LEVEL_COUNT];
    static Float const ms_damage_dissipation_rate[ENEMY_LEVEL_COUNT];
    static Float const ms_wander_speed[ENEMY_LEVEL_COUNT];

    Interloper (Uint8 enemy_level);
    virtual ~Interloper ();

    virtual void Think (Time time, Time::Delta frame_dt);
    virtual void Collide (
        Entity &collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
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

    bool IsFlockLeader () const { return m_flock_leader_weight > 0.25f; }

    void PickWanderDirection (Time time, Time::Delta frame_dt);
    void Wander (Time time, Time::Delta frame_dt);
    void Flock (Time time, Time::Delta frame_dt);
    void Charge (Time time, Time::Delta frame_dt);
    void Retreat (Time time, Time::Delta frame_dt);

    void MatchVelocity (FloatVector2 const &velocity, Time::Delta frame_dt);
    void AddFlockLeaderWeight (Float weight);

    typedef void (Interloper::*ThinkState)(Time time, Time::Delta frame_dt);

    ThinkState m_think_state;
    Time m_next_wander_time;
    Float m_wander_angle;
    Float m_wander_angle_derivative;
    EntityReference<Ship> m_target;
    Time m_time_at_retreat_start;
    Float m_flock_leader_weight;
    EntityReference<Interloper> m_closest_flock_member;
}; // end of class Interloper

} // end of namespace Dis

#endif // !defined(_DIS_INTERLOPER_HPP_)

