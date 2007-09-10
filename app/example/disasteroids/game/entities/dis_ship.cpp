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
#include "dis_demi.h"
#include "dis_devourment.h"
#include "dis_effect.h"
#include "dis_enemyship.h"
#include "dis_engine.h"
#include "dis_interloper.h"
#include "dis_powergenerator.h"
#include "dis_revulsion.h"
#include "dis_shade.h"
#include "dis_shield.h"
#include "dis_solitary.h"
#include "dis_spawn.h"
#include "dis_weapon.h"
#include "dis_world.h"
#include "xrb_engine2_objectlayer.h"

using namespace Xrb;

namespace Dis
{

Ship::Ship (
    Float const max_health,
    EntityType const entity_type)
    :
    Mortal(max_health, max_health, entity_type, CT_SOLID_COLLISION),
    m_reticle_coordinates(FloatVector2::ms_zero)
{
    m_disable_time = 0.0f;
    Ship::ResetInputs();
}

Ship::~Ship ()
{
}

std::string const &Ship::GetShipSpriteFilename (
    EntityType const ship_type,
    Uint8 const enemy_level)
{
    static std::string const s_ship_sprite_filename[ET_SHIP_COUNT][EnemyShip::ENEMY_LEVEL_COUNT] =
    {
        {   // ET_SOLITARY
            "resources/solitary_small.png",
            "resources/solitary_small.png",
            "resources/solitary_small.png",
            "resources/solitary_small.png"
        },
        {   // ET_INTERLOPER
            "resources/interloper0_small.png",
            "resources/interloper1_small.png",
            "resources/interloper2_small.png",
            "resources/interloper3_small.png"
        },
        {   // ET_SHADE
            "resources/shade0_small.png",
            "resources/shade1_small.png",
            "resources/shade2_small.png",
            "resources/shade3_small.png"
        },
        {   // ET_REVULSION
            "resources/revulsion0_small.png",
            "resources/revulsion1_small.png",
            "resources/revulsion2_small.png",
            "resources/revulsion3_small.png"
        },
        {   // ET_DEVOURMENT
            "resources/devourment0_small.png",
            "resources/devourment1_small.png",
            "resources/devourment2_small.png",
            "resources/devourment3_small.png"
        },
        {   // ET_DEMI
            "resources/demi0_small.png",
            "resources/demi1_small.png",
            "resources/demi2_small.png",
            "resources/demi3_small.png"
        }
    };

    Uint32 ship_index = ship_type - ET_SHIP_LOWEST;
    ASSERT1(ship_index < ET_SHIP_COUNT);
    ASSERT1(enemy_level < EnemyShip::ENEMY_LEVEL_COUNT);
    return s_ship_sprite_filename[ship_index][enemy_level];
}

Float Ship::GetShipScaleFactor (EntityType const ship_type, Uint8 const enemy_level)
{
    ASSERT1(enemy_level < EnemyShip::ENEMY_LEVEL_COUNT);
    switch (ship_type)
    {
        case ET_SOLITARY:   return Solitary::ms_scale_factor;
        case ET_INTERLOPER: return Interloper::ms_scale_factor[enemy_level];
        case ET_SHADE:      return Shade::ms_scale_factor[enemy_level];
        case ET_REVULSION:  return Revulsion::ms_scale_factor[enemy_level];
        case ET_DEVOURMENT: return Devourment::ms_scale_factor[enemy_level];
        case ET_DEMI:       return Demi::ms_scale_factor[enemy_level];
        default: ASSERT1(false && "Invalid enemy ship type"); return 1.0f;
    }
}

void Ship::SetReticleCoordinates (FloatVector2 const &reticle_coordinates)
{
    ASSERT1(Math::IsFinite(reticle_coordinates[Dim::X]));
    ASSERT1(Math::IsFinite(reticle_coordinates[Dim::Y]));
    m_reticle_coordinates = GetObjectLayer()->GetAdjustedCoordinates(reticle_coordinates, GetTranslation());
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

void Ship::AimShipAtCoordinates (FloatVector2 const &coordinates, Float const frame_dt)
{
    FloatVector2 aim_direction(
        GetObjectLayer()->GetAdjustedCoordinates(
            coordinates,
            GetTranslation())
        -
        GetTranslation());
    if (!aim_direction.GetIsZero())
    {
        Float angle_delta = Math::GetCanonicalAngle(Math::Atan(aim_direction) - GetAngle());
        Float max_angle_delta = GetMaxAngularVelocity() * frame_dt;
        if (angle_delta > max_angle_delta)
            angle_delta = max_angle_delta;
        else if (angle_delta < -max_angle_delta)
            angle_delta = -max_angle_delta;
        SetAngle(GetAngle() + angle_delta);
    }
}

void Ship::ResetInputs ()
{
    m_engine_right_left_input = 0;
    m_engine_up_down_input = 0;
    m_weapon_primary_input = 0;
    m_weapon_secondary_input = 0;
}

} // end of namespace Dis

