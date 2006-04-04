// ///////////////////////////////////////////////////////////////////////////
// dis_interloper.h by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_INTERLOPER_H_)
#define _DIS_INTERLOPER_H_

#include "dis_enemyship.h"

using namespace Xrb;

namespace Dis
{

class Interloper : public EnemyShip
{
public:

    Interloper (Uint8 enemy_level);
    virtual ~Interloper ();

    virtual void Think (Float time, Float frame_dt);
    virtual void Collide (
        Entity *collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
        Float time,
        Float frame_dt);

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
    void Charge (Float time, Float frame_dt);
    void Retreat (Float time, Float frame_dt);

    static Float const ms_max_health[ENEMY_LEVEL_COUNT];
    static Float const ms_engine_thrust[ENEMY_LEVEL_COUNT];
    static Float const ms_scale_factor[ENEMY_LEVEL_COUNT];
    static Float const ms_baseline_first_moment[ENEMY_LEVEL_COUNT];
    static Float const ms_damage_dissipation_rate[ENEMY_LEVEL_COUNT];

    typedef void (Interloper::*ThinkState)(Float time, Float frame_dt);

    ThinkState m_think_state;
    EntityReference<Ship> m_target;
    Float m_time_at_retreat_start;
}; // end of class Interloper

} // end of namespace Dis

#endif // !defined(_DIS_INTERLOPER_H_)

