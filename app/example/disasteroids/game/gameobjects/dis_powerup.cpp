// ///////////////////////////////////////////////////////////////////////////
// dis_powerup.cpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_powerup.h"

#include "dis_ship.h"

using namespace Xrb;

namespace Dis
{

void Powerup::Think (Float const time, Float const frame_dt)
{
    // TODO: nice fading out/decaying
    ScheduleForDeletion(40.0f);
}

void Powerup::Collide (
    GameObject *const collider,
    FloatVector2 const &collision_location,
    FloatVector2 const &collision_normal,
    Float const collision_force,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(collider != NULL)

    // early-out if already taken
    if (m_pickup_flags == PU_NONE)
        return;

    // early-out if the collider is not a ship
    if (!collider->GetIsShip())
        return;

    Ship *collider_ship = static_cast<Ship *>(collider);

    // TODO: test the pickup flags against what the ship actually is

    // if the ship accepted the powerup, delete it
    if (collider_ship->TakePowerup(this))
    {
        // make sure the ship took the powerup item and cleared it
        ASSERT1(m_item == NULL)
        // this will prevent any other ships from getting the powerup
        // in the same frame
        m_pickup_flags = PU_NONE;
        // schedule this for deletion        
        ScheduleForDeletion(0.0f);
    }
}

} // end of namespace Dis

