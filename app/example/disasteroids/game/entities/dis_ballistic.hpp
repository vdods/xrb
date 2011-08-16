// ///////////////////////////////////////////////////////////////////////////
// dis_ballistic.hpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_BALLISTIC_HPP_)
#define _DIS_BALLISTIC_HPP_

#include "dis_entity.hpp"

using namespace Xrb;

namespace Dis {

class Ballistic : public Entity
{
public:

    enum ImpactEffect
    {
        IE_NONE = 0,
        IE_PLASMA_BALL,

        IE_COUNT
    }; // end of enum Ballistic::ImpactEffect

    Ballistic (
        Float impact_damage,
        Float time_to_live,
        Float time_at_birth,
        Uint8 const weapon_level,
        EntityReference<Entity> const &owner,
        bool perform_line_trace_for_accuracy)
        :
        Entity(ET_BALLISTIC, Engine2::Circle::CT_SOLID_COLLISION),
        m_weapon_level(weapon_level),
        m_impact_effect(IE_NONE),
        m_owner(owner)
    {
        ASSERT1(time_to_live > 0.0f);
        ASSERT1(time_at_birth >= 0.0f);
        ASSERT1(m_weapon_level < UPGRADE_LEVEL_COUNT);
        m_first_think = true;
        m_impact_damage = impact_damage;
        m_time_to_live = time_to_live;
        m_time_at_birth = time_at_birth;
        m_perform_line_trace_for_accuracy = perform_line_trace_for_accuracy;
    }

    virtual bool IsBallistic () const { return true; }

    Uint8 WeaponLevel () const { return m_weapon_level; }
    ImpactEffect GetImpactEffect () const { return m_impact_effect; }

    void SetImpactEffect (ImpactEffect impact_effect) { ASSERT1(impact_effect < IE_COUNT); m_impact_effect = impact_effect; }

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
    Uint8 const m_weapon_level;
    ImpactEffect m_impact_effect;
    EntityReference<Entity> m_owner;
    bool m_perform_line_trace_for_accuracy;
    FloatVector2 m_initial_velocity;
}; // end of class Ballistic

} // end of namespace Dis

#endif // !defined(_DIS_BALLISTIC_HPP_)

