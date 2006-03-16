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

    void Seek (Float time, Float frame_dt);
    void Stalk (Float time, Float frame_dt);
    void MoveToAttackRange (Float time, Float frame_dt);
    void Teleport (Float time, Float frame_dt);

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

    typedef void (Shade::*ThinkState)(Float time, Float frame_dt);

    ThinkState m_think_state;
    GameObjectReference<Ship> m_target;
    Weapon *m_weapon;
}; // end of class Shade

} // end of namespace Dis

#endif // !defined(_DIS_SHADE_H_)

