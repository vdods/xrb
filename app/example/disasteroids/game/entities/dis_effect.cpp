// ///////////////////////////////////////////////////////////////////////////
// dis_effect.cpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_effect.h"

#include "dis_mortal.h"
#include "dis_physicshandler.h"
#include "dis_ship.h"
#include "dis_util.h"
#include "xrb_engine2_sprite.h"

using namespace Xrb;

namespace Dis
{

void Effect::Think (Float const time, Float const frame_dt)
{
    GetOwnerSprite()->SetColorMask(Color(1.0f, 1.0f, 1.0f, 1.0f - GetLifetimeRatio(time)));

    if (m_time_at_birth + m_time_to_live <= time && m_time_to_live > 0.0f)
        ScheduleForDeletion(0.0f);
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

void Explosion::Think (Float const time, Float const frame_dt)
{
    SetScaleFactor(m_final_size * Math::Sqrt(GetLifetimeRatio(time)) + 0.1f);

    Effect::Think(time, frame_dt);
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

void DamageExplosion::Think (Float time, Float frame_dt)
{
    if (!m_has_done_impact)
    {
        m_has_done_impact = true;
        
        RadiusKnockback(
            GetPhysicsHandler(),
            GetObjectLayer(),
            GetTranslation(),
            GetFinalSize(),
            m_damage_amount,
            time,
            frame_dt);

        if (m_damage_radius > 0.0f)
            RadiusDamage(
                GetPhysicsHandler(),
                GetObjectLayer(),
                *m_owner,
                this,
                m_damage_amount,
                GetTranslation(),
                m_damage_radius,
                Mortal::D_EXPLOSION,
                m_owner, // do not damage the owner
                time,
                frame_dt);
    }

    Explosion::Think(time, frame_dt);    
}

void DamageExplosion::Collide (
    Entity *const collider,
    FloatVector2 const &collision_location,
    FloatVector2 const &collision_normal,
    Float const collision_force,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(collider != NULL)

    // can't damage nonsolid objects
    if (collider->GetCollisionType() == CT_NONSOLID_COLLISION)
        return;

    // return if nothing would actually be done
    Float reverse_lifetime_ratio = 1.0f - GetLifetimeRatio(time);
    if (reverse_lifetime_ratio < 0.0f)
        return;
        
    // center_to_center points towards the collider
    FloatVector2 center_to_center = collider->GetTranslation() - GetTranslation();
    Float distance = center_to_center.GetLength() - collider->GetScaleFactor();
    if (distance < 0.0f)
        distance = 0.0f;
    Float distance_factor;
    if (distance < 1.0f)
        distance_factor = 1.0f;
    else
        distance_factor = 1.0f / Math::Sqrt(distance);
    
    // if it's a Mortal, damage it (unless it's the owner)
    if (collider->GetIsMortal() && collider != *m_owner)
        DStaticCast<Mortal *>(collider)->Damage(
            *m_owner,
            this,
            m_damage_amount * reverse_lifetime_ratio * frame_dt * distance_factor,
            NULL,
            collision_location,
            collision_normal,
            collision_force,
            Mortal::D_EXPLOSION,
            time,
            frame_dt);
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

void EMPExplosion::Collide (
    Entity *const collider,
    FloatVector2 const &collision_location,
    FloatVector2 const &collision_normal,
    Float const collision_force,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(collider != NULL)
    if (collider->GetCollisionType() == CT_NONSOLID_COLLISION)
        return;

    // only affect ships that aren't the owner
    if (collider->GetIsShip() && collider != *m_owner)
        DStaticCast<Ship *>(collider)->AccumulateDisableTime(
            m_disable_time_factor * Min(frame_dt, 1.0f / 20.0f));
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

void Fireball::Think (Float const time, Float const frame_dt)
{
    // the power decays as time goes on
    m_current_damage -= m_potential_damage * frame_dt / GetTimeToLive();

    Explosion::Think(time, frame_dt);

    // update the sprite's alpha value to reflect the damage left
    Float alpha_value = Max(0.0f, m_current_damage / m_potential_damage);
    ASSERT1(alpha_value <= 1.0f)
    GetOwnerSprite()->SetColorMask(Color(1.0f, 1.0f, 1.0f, alpha_value));
}

void Fireball::Collide (
    Entity *const collider,
    FloatVector2 const &collision_location,
    FloatVector2 const &collision_normal,
    Float const collision_force,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(collider != NULL)

    // if there is no power left, return (this can happen when a fireball
    // gets used up on one object before all its collisions are computed)
    if (m_current_damage <= 0.0f)
        return;
    
    // TODO: when napalm is done, check if it hit napalm
    
    // we only care about hitting solid things
    if (collider->GetCollisionType() == CT_NONSOLID_COLLISION)
        return;

    static Float const s_damage_dissipation_rate = 2.0f;
        
    if (collider->GetIsMortal())
    {
        Mortal *mortal = DStaticCast<Mortal *>(collider);
        Float damage_to_inflict =
            Min(m_current_damage,
                s_damage_dissipation_rate * m_potential_damage * frame_dt);
        m_current_damage -= damage_to_inflict;
        mortal->Damage(
            *m_owner,
            this,
            damage_to_inflict,
            NULL,
            collision_location,
            collision_normal,
            collision_force,
            Mortal::D_FIRE,
            time,
            frame_dt);
    }

    // if the power has reached zero, delete it
    if (m_current_damage <= 0.0f)
        ScheduleForDeletion(0.0f);
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

void LaserBeam::SetIntensity (Float const intensity)
{
    ASSERT1(intensity >= 0.0f && intensity <= 1.0f)
    ASSERT1(GetIsInWorld())
    GetOwnerSprite()->SetColorMask(Color(1.0f, 1.0f, 1.0f, intensity));
}

void LaserBeam::SnapToShip (
    FloatVector2 const &muzzle_location,
    FloatVector2 const &hit_location,
    Float const beam_width)
{
    ASSERT1(GetIsInWorld())
    FloatVector2 beam_vector(hit_location - muzzle_location);
    SetTranslation(0.5f * (muzzle_location + hit_location));
    SetScaleFactors(FloatVector2(0.5f * beam_vector.GetLength(), 0.5f * beam_width));
    SetAngle(Math::Atan(beam_vector));
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

void TractorBeam::SetParameters (
    bool const pull_everything,
    bool const push_instead_of_pull,
    Float const pulling_input,
    Float const intensity)
{
    ASSERT1(pulling_input >= -1.0f && pulling_input <= 1.0f)
    ASSERT1(intensity >= 0.0f && intensity <= 1.0f)
    ASSERT1(GetIsInWorld())
    if (push_instead_of_pull)
        GetOwnerSprite()->SetColorMask(Color(1.0f, 0.0f, 0.0f, intensity));
    else if (pull_everything)
        GetOwnerSprite()->SetColorMask(Color(0.0f, 0.0f, 1.0f, intensity));
    else
        GetOwnerSprite()->SetColorMask(Color(0.0f, 1.0f, 0.0f, intensity));
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

void ShieldEffect::SetIntensity (Float const intensity)
{
    ASSERT1(intensity >= 0.0f && intensity <= 1.0f)
    GetOwnerSprite()->SetColorMask(Color(1.0f, 1.0f, 1.0f, intensity));
}

void ShieldEffect::SnapToShip (
    FloatVector2 const &ship_translation,
    Float const ship_scale_factor)
{
    static Float const s_shield_size_ratio = 1.666f;

    RemoveFromWorld();
    SetTranslation(ship_translation);
    SetScaleFactor(s_shield_size_ratio * ship_scale_factor);
    AddBackIntoWorld();
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

void ReticleEffect::SnapToLocationAndSetScaleFactor (
    FloatVector2 const &location,
    Float const scale_factor)
{
    RemoveFromWorld();
    SetTranslation(location);
    SetScaleFactor(scale_factor);
    AddBackIntoWorld();
}

} // end of namespace Dis

