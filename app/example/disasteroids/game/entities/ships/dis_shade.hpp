// ///////////////////////////////////////////////////////////////////////////
// dis_shade.hpp by Victor Dods, created 2005/12/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_SHADE_HPP_)
#define _DIS_SHADE_HPP_

#include "dis_enemyship.hpp"

using namespace Xrb;

namespace Dis
{

class Shade : public EnemyShip
{
public:

    static Float const ms_max_health[ENEMY_LEVEL_COUNT];
    static Float const ms_engine_thrust[ENEMY_LEVEL_COUNT];
    static Float const ms_max_angular_velocity[ENEMY_LEVEL_COUNT];
    static Float const ms_scale_factor[ENEMY_LEVEL_COUNT];
    static Float const ms_baseline_first_moment[ENEMY_LEVEL_COUNT];
    static Float const ms_damage_dissipation_rate[ENEMY_LEVEL_COUNT];
    static Float const ms_alarm_distance[ENEMY_LEVEL_COUNT];
    static Float const ms_stalk_minimum_distance[ENEMY_LEVEL_COUNT];
    static Float const ms_stalk_maximum_distance[ENEMY_LEVEL_COUNT];
    static Float const ms_move_relative_velocity[ENEMY_LEVEL_COUNT];
    static Float const ms_wander_speed[ENEMY_LEVEL_COUNT];

    Shade (Uint8 enemy_level);
    virtual ~Shade ();

    virtual void Think (Float time, Float frame_dt);

    // ///////////////////////////////////////////////////////////////////////
    // Ship interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual FloatVector2 GetMuzzleLocation (Weapon const *weapon) const;
    virtual FloatVector2 GetMuzzleDirection (Weapon const *weapon) const;

    virtual Float GetMaxAngularVelocity () const
    {
        return ms_max_angular_velocity[EnemyLevel()];
    }
    virtual Float GetShipScaleFactor () const
    {
        return ms_scale_factor[EnemyLevel()];
    }
    virtual Float GetShipBaselineFirstMoment () const
    {
        return ms_baseline_first_moment[EnemyLevel()];
    }

    // ///////////////////////////////////////////////////////////////////////
    // EnemyShip interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual void SetTarget (Mortal *target);

private:

    void PickWanderDirection (Float time, Float frame_dt);
    void Wander (Float time, Float frame_dt);
    void Stalk (Float time, Float frame_dt);
    void MoveToAttackRange (Float time, Float frame_dt);
    void Teleport (Float time, Float frame_dt);
    void PauseAfterTeleport (Float time, Float frame_dt);

    void MatchVelocity (FloatVector2 const &velocity, Float frame_dt);
    void AimWeapon (FloatVector2 const &target_position);

    typedef void (Shade::*ThinkState)(Float time, Float frame_dt);

    ThinkState m_think_state;
    Float m_next_whatever_time;
    Float m_wander_angle;
    EntityReference<Ship> m_target;
    Weapon *m_weapon;
}; // end of class Shade

} // end of namespace Dis

#endif // !defined(_DIS_SHADE_HPP_)

