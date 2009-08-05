// ///////////////////////////////////////////////////////////////////////////
// dis_devourment.hpp by Victor Dods, created 2006/02/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_DEVOURMENT_HPP_)
#define _DIS_DEVOURMENT_HPP_

#include "dis_enemyship.hpp"

using namespace Xrb;

namespace Dis
{

class HealthTrigger;
class Tractor;
class TractorBeam;

class Devourment : public EnemyShip
{
public:

    static Float const ms_max_health[ENEMY_LEVEL_COUNT];
    static Float const ms_engine_thrust[ENEMY_LEVEL_COUNT];
    static Float const ms_wander_speed[ENEMY_LEVEL_COUNT];
    static Float const ms_max_angular_velocity[ENEMY_LEVEL_COUNT];
    static Float const ms_scale_factor[ENEMY_LEVEL_COUNT];
    static Float const ms_baseline_first_moment[ENEMY_LEVEL_COUNT];
    static Float const ms_damage_dissipation_rate[ENEMY_LEVEL_COUNT];
    static Float const ms_mouth_damage_rate[ENEMY_LEVEL_COUNT];
    static Float const ms_mouth_tractor_range[ENEMY_LEVEL_COUNT];
    static Float const ms_mouth_tractor_strength[ENEMY_LEVEL_COUNT];
    static Float const ms_mouth_tractor_max_force[ENEMY_LEVEL_COUNT];
    static Float const ms_mouth_tractor_beam_radius[ENEMY_LEVEL_COUNT];
    static Float const ms_max_spawn_mineral_mass[ENEMY_LEVEL_COUNT];
    static Float const ms_max_single_mineral_mass[ENEMY_LEVEL_COUNT];
    static Float const ms_health_powerup_amount_to_spawn[ENEMY_LEVEL_COUNT];

    Devourment (Uint8 enemy_level);
    virtual ~Devourment ();

    virtual void Think (Float time, Float frame_dt);
    virtual void Collide (
        Entity *collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
        Float time,
        Float frame_dt);
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

    virtual bool TakePowerup (Powerup *powerup, Float time, Float frame_dt);

    // ///////////////////////////////////////////////////////////////////////
    // EnemyShip interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual void SetTarget (Mortal *target);

private:

    void PickWanderDirection (Float time, Float frame_dt);
    void Wander (Float time, Float frame_dt);
    void Pursue (Float time, Float frame_dt);
    void Consume (Float time, Float frame_dt);

    void MatchVelocity (FloatVector2 const &velocity, Float frame_dt);
    EntityReference<Entity> ScanAreaForTargets ();

    typedef void (Devourment::*ThinkState)(Float time, Float frame_dt);

    ThinkState m_think_state;
    Float m_next_whatever_time;
    Float m_wander_angle;
    EntityReference<Entity> m_target;

    EntityReference<HealthTrigger> m_mouth_health_trigger;
    Tractor *m_mouth_tractor;
    EntityReference<TractorBeam> m_mouth_tractor_beam;

    Float m_mineral_inventory[MINERAL_COUNT];
}; // end of class Devourment

} // end of namespace Dis

#endif // !defined(_DIS_DEVOURMENT_HPP_)

