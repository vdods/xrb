// ///////////////////////////////////////////////////////////////////////////
// dis_powerup.cpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_powerup.hpp"

#include "dis_ship.hpp"
#include "xrb_math.hpp"

using namespace Xrb;

namespace Dis
{

void Powerup::SetEffectiveCoefficient (Float const effective_coefficient)
{
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(effective_coefficient));
    m_effective_coefficient = effective_coefficient;
}

void Powerup::SetEffectiveValue (Float const effective_value)
{
    ASSERT1(OwnerObject() != NULL && "may only use this method on an Entity with an owner Object");
    ASSERT1(Mass() > 0.0f);
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(effective_value));
    m_effective_coefficient = effective_value / Mass();
}

void Powerup::Think (Float const time, Float const frame_dt)
{
    if (m_delete_upon_next_think)
        ScheduleForDeletion(0.0f);
    else
    {
        m_delete_upon_next_think = true;
        SetNextTimeToThink(time + 40.0f);
    }
}

void Powerup::Collide (
    Entity *const collider,
    FloatVector2 const &collision_location,
    FloatVector2 const &collision_normal,
    Float const collision_force,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(collider != NULL);

    // early-out if already taken
    if (m_has_been_picked_up)
        return;

    // early-out if the collider is not a ship
    if (!collider->IsShip())
        return;

    if (DStaticCast<Ship *>(collider)->TakePowerup(this, time, frame_dt))
    {
        // make sure the ship took the powerup item and cleared it
        ASSERT1(m_item == NULL);
        // this will prevent any other ships from getting the powerup
        m_has_been_picked_up = true;
        // schedule this for deletion
        ScheduleForDeletion(0.0f);
    }
}

} // end of namespace Dis

