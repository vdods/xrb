// ///////////////////////////////////////////////////////////////////////////
// dis_ship.cpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_ship.hpp"

#include "dis_armor.hpp"
#include "dis_demi.hpp"
#include "dis_devourment.hpp"
#include "dis_effect.hpp"
#include "dis_enemyship.hpp"
#include "dis_engine.hpp"
#include "dis_interloper.hpp"
#include "dis_powergenerator.hpp"
#include "dis_revulsion.hpp"
#include "dis_shade.hpp"
#include "dis_shield.hpp"
#include "dis_solitary.hpp"
#include "dis_spawn.hpp"
#include "dis_weapon.hpp"
#include "dis_world.hpp"
#include "xrb_engine2_objectlayer.hpp"
#include "xrb_engine2_sprite.hpp"

using namespace Xrb;

namespace Dis {

Ship::Ship (Float max_health, EntityType entity_type)
    :
    Mortal(max_health, max_health, entity_type, Engine2::Circle::CT_SOLID_COLLISION),
    m_reticle_coordinates(FloatVector2::ms_zero)
{
    m_disable_duration = 0.0f;
    Ship::ResetInputs();
}

Ship::~Ship ()
{
    if (m_lightning_effect.IsValid() && !m_lightning_effect->IsInWorld())
        delete m_lightning_effect->OwnerObject();
}

std::string const &Ship::ShipSpritePath (EntityType ship_type, Uint8 enemy_level)
{
    static std::string const s_ship_sprite_path[ET_SHIP_COUNT][EnemyShip::ENEMY_LEVEL_COUNT] =
    {
        {   // ET_SOLITARY
            "fs://solitary.png",
            "fs://solitary.png",
            "fs://solitary.png",
            "fs://solitary.png"
        },
        {   // ET_INTERLOPER
            "fs://interloper_0.png",
            "fs://interloper_1.png",
            "fs://interloper_2.png",
            "fs://interloper_3.png"
        },
        {   // ET_SHADE
            "fs://shade_0.png",
            "fs://shade_1.png",
            "fs://shade_2.png",
            "fs://shade_3.png"
        },
        {   // ET_REVULSION
            "fs://revulsion_0.png",
            "fs://revulsion_1.png",
            "fs://revulsion_2.png",
            "fs://revulsion_3.png"
        },
        {   // ET_DEVOURMENT
            "fs://devourment_0.png",
            "fs://devourment_1.png",
            "fs://devourment_2.png",
            "fs://devourment_3.png"
        },
        {   // ET_DEMI
            "fs://demi_0.png",
            "fs://demi_1.png",
            "fs://demi_2.png",
            "fs://demi_3.png"
        }
    };

    Uint32 ship_index = ship_type - ET_SHIP_LOWEST;
    ASSERT1(ship_index < ET_SHIP_COUNT);
    ASSERT1(enemy_level < EnemyShip::ENEMY_LEVEL_COUNT);
    return s_ship_sprite_path[ship_index][enemy_level];
}

Float Ship::ShipRadius (EntityType ship_type, Uint8 enemy_level)
{
    ASSERT1(enemy_level < EnemyShip::ENEMY_LEVEL_COUNT);
    switch (ship_type)
    {
        case ET_SOLITARY:   return Solitary::ms_ship_radius;
        case ET_INTERLOPER: return Interloper::ms_ship_radius[enemy_level];
        case ET_SHADE:      return Shade::ms_ship_radius[enemy_level];
        case ET_REVULSION:  return Revulsion::ms_ship_radius[enemy_level];
        case ET_DEVOURMENT: return Devourment::ms_ship_radius[enemy_level];
        case ET_DEMI:       return Demi::ms_ship_radius[enemy_level];
        default: ASSERT1(false && "Invalid enemy ship type"); return 1.0f;
    }
}

void Ship::SetReticleCoordinates (FloatVector2 const &reticle_coordinates)
{
    ASSERT1(Math::IsFinite(reticle_coordinates[Dim::X]));
    ASSERT1(Math::IsFinite(reticle_coordinates[Dim::Y]));
    m_reticle_coordinates = reticle_coordinates;
}

void Ship::HandleNewOwnerObject ()
{
    SetScaleFactor(ShipRadius());
    SetMass(ShipBaselineMass());
}

void Ship::Think (Time time, Time::Delta frame_dt)
{
    // call the superclass' Think
    Mortal::Think(time, frame_dt);

    // nothing can happen when the ship is disabled.
    m_disable_duration = Max(0.0f, m_disable_duration - frame_dt);

    // if the ship is not dead and is disabled, apply the lightning bolt effect
    // to indicate that it is disabled.
    if (IsDisabled())
    {
        // ensure the lightning effect is allocated (lazy allocation)
        if (!m_lightning_effect.IsValid())
            m_lightning_effect = SpawnLightningEffect(GetObjectLayer(), time)->GetReference();
        // if the lightning effect is already allocated but not in the world, re-add it.
        else if (!m_lightning_effect->IsInWorld())
            m_lightning_effect->AddBackIntoWorld();
        // TODO: real entity attachment -- temp hack
        m_lightning_effect->SnapToShip(Translation() + frame_dt * Velocity(), VisibleRadius());
    }
    else
    {
        if (m_lightning_effect.IsValid() && m_lightning_effect->IsInWorld())
            m_lightning_effect->ScheduleForRemovalFromWorld(0.0f);
    }
}

void Ship::Die (
    Entity *killer,
    Entity *kill_medium,
    FloatVector2 const &kill_location,
    FloatVector2 const &kill_normal,
    Float kill_force,
    DamageType kill_type,
    Time time,
    Time::Delta frame_dt)
{
    ASSERT1(OwnerObject() != NULL);
    ASSERT1(OwnerObject()->GetObjectType() == Engine2::OT_SPRITE);
    std::string sprite_path = static_cast<Engine2::Sprite *>(OwnerObject())->GetTexture().LoadParameters().As<GlTexture::LoadParameters>().Path();

    // the "explosion" sprite will be the same as the ship, as if the ship's soul is being liberated
    {
        NoDamageExplosion *soul =
            SpawnNoDamageExplosion(
                GetObjectLayer(),
                sprite_path,
                time,
                Translation(),
                Velocity(),
                1.0f * VisibleRadius(), // initial_size
                2.0f * VisibleRadius(), // final_size
                0.5f,
                time);
        ASSERT1(soul != NULL);
        soul->SetAngle(Angle());
    }

    // spawn a fast shock wave
    {
        NoDamageExplosion *shockwave =
            SpawnNoDamageExplosion(
                GetObjectLayer(),
                ExplosionAssetPath(EXPLO_SHOCKWAVE),
                time,
                Translation(),
                Velocity(),
                0.0f, // initial_size
                8.0f * VisibleRadius(),
                0.25f,
                time);
        shockwave->InitialColorMask() = Color(1.0f, 1.0f, 1.0f, 0.3f);
        shockwave->SetScaleInterpolationPower(1.0f); // linear scale interpolation
    }

    // get rid of the lightning effect if the ship was disabled
    if (m_lightning_effect.IsValid() && m_lightning_effect->IsInWorld())
        m_lightning_effect->ScheduleForRemovalFromWorld(0.0f);

    ScheduleForDeletion(0.0f);
}

void Ship::AimShipAtCoordinates (FloatVector2 const &coordinates, Time::Delta frame_dt)
{
    FloatVector2 aim_direction(GetObjectLayer()->AdjustedDifference(coordinates, Translation()));
    if (!aim_direction.IsZero())
    {
        Float angle_delta = Math::CanonicalAngle(Math::Arg(aim_direction) - Angle());
        Float max_angle_delta = MaxAngularVelocity() * frame_dt;
        if (angle_delta > max_angle_delta)
            angle_delta = max_angle_delta;
        else if (angle_delta < -max_angle_delta)
            angle_delta = -max_angle_delta;
        SetAngle(Angle() + angle_delta);
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

