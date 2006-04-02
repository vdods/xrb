// ///////////////////////////////////////////////////////////////////////////
// dis_ship.cpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_ship.h"

#include "dis_armor.h"
#include "dis_effect.h"
#include "dis_engine.h"
#include "dis_powergenerator.h"
#include "dis_shield.h"
#include "dis_spawn.h"
#include "dis_weapon.h"
#include "dis_world.h"

using namespace Xrb;

namespace Dis
{

Ship::Ship (
    Float const max_health,
    Type const type)
    :
    Mortal(max_health, max_health, type, CT_SOLID_COLLISION),
    m_reticle_coordinates(FloatVector2::ms_zero)
{
    m_disable_time = 0.0f;
    Ship::ResetInputs();
}

Ship::~Ship ()
{
}

void Ship::HandleNewOwnerObject ()
{
    SetScaleFactor(GetShipScaleFactor());
    SetFirstMoment(GetShipBaselineFirstMoment());
}

void Ship::Think (Float const time, Float const frame_dt)
{
    // nothing can happen when the ship is disabled.
    m_disable_time = Max(0.0f, m_disable_time - frame_dt);
}

void Ship::Die (
    Entity *const killer,
    Entity *const kill_medium,
    FloatVector2 const &kill_location,
    FloatVector2 const &kill_normal,
    Float const kill_force,
    DamageType const kill_type,
    Float const time,
    Float const frame_dt)
{
    SpawnNoDamageExplosion(
        GetWorld(),
        GetObjectLayer(),
        GetTranslation(),
        GetVelocity(),
        5.0f * GetScaleFactor(),
        2.0f,
        time);

    ScheduleForDeletion(0.0f);
}

void Ship::TakeOwnershipOfItem (Item *const item)
{
    ASSERT1(item != NULL)
    if (dynamic_cast<PoweredDevice *>(item) != NULL)
        static_cast<PoweredDevice *>(item)->SetOwnerShip(this);
}

void Ship::AimShipAtReticleCoordinates ()
{
    // set the ship's angle based on where it's aiming at
    SetAngle(Math::Atan(m_reticle_coordinates - GetTranslation()));
}

void Ship::ResetInputs ()
{
    m_engine_right_left_input = 0;
    m_engine_up_down_input = 0;
    m_weapon_primary_input = 0;
    m_weapon_secondary_input = 0;
}

} // end of namespace Dis

