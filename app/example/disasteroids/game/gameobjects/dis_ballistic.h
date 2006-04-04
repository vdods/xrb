// ///////////////////////////////////////////////////////////////////////////
// dis_ballistic.h by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_BALLISTIC_H_)
#define _DIS_BALLISTIC_H_

#include "dis_entity.h"

using namespace Xrb;

namespace Dis
{

class Ballistic : public Entity
{
public:

    Ballistic (
        Float impact_damage,
        Float time_to_live,
        Float time_at_birth,
        EntityReference<Entity> const &owner,
        bool perform_line_trace_for_accuracy)
        :
        Entity(ET_BALLISTIC, CT_SOLID_COLLISION),
        m_owner(owner)
    {
        ASSERT1(time_to_live > 0.0f);
        ASSERT1(time_at_birth >= 0.0f)
        m_first_think = true;
        m_impact_damage = impact_damage;
        m_time_to_live = time_to_live;
        m_time_at_birth = time_at_birth;
        m_perform_line_trace_for_accuracy = perform_line_trace_for_accuracy;
    }

    virtual bool GetIsBallistic () const { return true; }
    
    virtual void Think (Float time, Float frame_dt);
    virtual void Collide (
        Entity *collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
        Float time,
        Float frame_dt);

protected:

    bool CollidePrivate (
        Entity *collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
        Float time,
        Float frame_dt,
        bool was_collision_from_line_trace);

    bool m_first_think;
    Float m_impact_damage;
    Float m_time_to_live;
    Float m_time_at_birth;
    EntityReference<Entity> m_owner;
    bool m_perform_line_trace_for_accuracy;
    FloatVector2 m_initial_velocity;
}; // end of class Ballistic

} // end of namespace Dis

#endif // !defined(_DIS_BALLISTIC_H_)

