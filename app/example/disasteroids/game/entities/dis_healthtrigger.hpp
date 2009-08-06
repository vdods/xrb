// ///////////////////////////////////////////////////////////////////////////
// dis_healthtrigger.hpp by Victor Dods, created 2006/02/23
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_HEALTHTRIGGER_HPP_)
#define _DIS_HEALTHTRIGGER_HPP_

#include "dis_entity.hpp"

#include "dis_mortal.hpp"

using namespace Xrb;

namespace Dis
{

class HealthTrigger : public Entity
{
public:

    HealthTrigger (
        Float health_delta_rate,
        Mortal::DamageType damage_type,
        EntityReference<Mortal> const &ignore_this_mortal,
        EntityReference<Entity> const &owner);

    virtual void Collide (
        Entity *collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
        Float time,
        Float frame_dt);

    inline Float HealthDeltaRate () const { return m_health_delta_rate; }

    inline void SetHealthDeltaRate (Float health_delta_rate) { m_health_delta_rate = health_delta_rate; }
            
private:

    Float m_health_delta_rate;
    Mortal::DamageType m_damage_type;
    EntityReference<Mortal> m_ignore_this_mortal;
    EntityReference<Entity> m_owner;
}; // end of class HealthTrigger
        
} // end of namespace Dis

#endif // !defined(_DIS_HEALTHTRIGGER_HPP_)

