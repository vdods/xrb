// ///////////////////////////////////////////////////////////////////////////
// dis_healthtrigger.cpp by Victor Dods, created 2006/02/23
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_healthtrigger.hpp"

using namespace Xrb;

namespace Dis
{

HealthTrigger::HealthTrigger (
    Float const health_delta_rate,
    Mortal::DamageType const damage_type,
    EntityReference<Mortal> const &ignore_this_mortal,
    EntityReference<Entity> const &owner)
    :
    Entity(ET_HEALTH_TRIGGER, CT_NONSOLID_COLLISION),
    m_ignore_this_mortal(ignore_this_mortal),
    m_owner(owner)
{
    m_health_delta_rate = health_delta_rate;
    m_damage_type = damage_type;
}

void HealthTrigger::Collide (
    Entity *const collider,
    FloatVector2 const &collision_location,
    FloatVector2 const &collision_normal,
    Float const collision_force,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(collider != NULL);

    if (collider->IsMortal() && collider != *m_ignore_this_mortal)
    {
        if (m_health_delta_rate > 0.0f)
        {
            static_cast<Mortal *>(collider)->Heal(
                *m_owner,
                this,
                m_health_delta_rate * frame_dt,
                collision_location,
                collision_normal,
                0.0f,
                time,
                frame_dt);
        }
        else if (m_health_delta_rate < 0.0f)
        {
            static_cast<Mortal *>(collider)->Damage(
                *m_owner,
                this,
                -m_health_delta_rate * frame_dt,
                NULL,
                collision_location,
                collision_normal,
                0.0f,
                m_damage_type,
                time,
                frame_dt);
        }
    }
}

} // end of namespace Dis

