// ///////////////////////////////////////////////////////////////////////////
// dis_ballistic.cpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_ballistic.h"

#include "dis_mortal.h"

using namespace Xrb;

namespace Dis
{

void Ballistic::Think (Float const time, Float const frame_dt)
{
    if (m_delete_upon_next_think)
        ScheduleForDeletion(0.0f);
    else
    {
        m_delete_upon_next_think = true;
        SetNextTimeToThink(time + m_time_to_live);
    }
}

void Ballistic::Collide (
    GameObject *const collider,
    FloatVector2 const &collision_location,
    FloatVector2 const &collision_normal,
    Float const collision_force,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(collider != NULL)

    // we only care about hitting solid things
    if (collider->GetCollisionType() == Engine2::CT_NONSOLID_COLLISION)
        return;

    // also, don't hit powerups
    if (collider->GetIsPowerup())
        return;

    // if we hit a mortal, damage it.
    if (collider->GetIsMortal())
    {
        Mortal *mortal = DStaticCast<Mortal *>(collider);
        mortal->Damage(
            *m_owner,
            this,
            m_impact_damage,
            NULL,
            collision_location,
            collision_normal,
            collision_force,
            Mortal::D_BALLISTIC,
            time,
            frame_dt);
    }

    // spawn some sort of effect here
    
    // it hit something, so get rid of it.
    ScheduleForDeletion(0.0f);
}

} // end of namespace Dis

