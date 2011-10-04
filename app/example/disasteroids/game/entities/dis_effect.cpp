// ///////////////////////////////////////////////////////////////////////////
// dis_effect.cpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_effect.hpp"

#include "dis_mortal.hpp"
#include "dis_ship.hpp"
#include "dis_util.hpp"
#include "xrb_engine2_circle_physicshandler.hpp"
#include "xrb_engine2_sprite.hpp"
#include "xrb_math.hpp"

using namespace Xrb;

namespace Dis {

void FiniteLifetimeEffect::Think (Time time, Time::Delta frame_dt)
{
    ASSERT1(m_time_to_live > 0.0f);
    ASSERT1(m_color_mask_interpolation_power > 0.0f);

    Float interpolation_parameter = RunInReverse() ? (1.0f - LifetimeRatio(time)) : LifetimeRatio(time);
    interpolation_parameter = Math::Pow(interpolation_parameter, m_color_mask_interpolation_power);
    OwnerObject()->ColorMask() = Math::LinearlyInterpolate(m_initial_color_mask, m_final_color_mask, 0.0f, 1.0f, interpolation_parameter);

    if (m_time_at_birth + m_time_to_live <= time)
        ScheduleForDeletion(0.0f);
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

void Explosion::Think (Time time, Time::Delta frame_dt)
{
    ASSERT1(m_scale_interpolation_power > 0.0f);

    Float interpolation_parameter = RunInReverse() ? (1.0f - LifetimeRatio(time)) : LifetimeRatio(time);
    interpolation_parameter = Math::Pow(interpolation_parameter, m_scale_interpolation_power);
    SetScaleFactor(Math::LinearlyInterpolate(m_initial_size, m_final_size, 0.0f, 1.0f, interpolation_parameter));

    FiniteLifetimeEffect::Think(time, frame_dt);
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

void DamageExplosion::Think (Time time, Time::Delta frame_dt)
{
    if (!m_has_done_impact)
    {
        m_has_done_impact = true;

        ASSERT1(GetPhysicsHandler() != NULL);
        ASSERT1(GetObjectLayer() != NULL);
        
        RadiusKnockback(
            *GetPhysicsHandler(),
            *GetObjectLayer(),
            Translation(),
            FinalSize(),
            Math::Pow(m_damage_amount, 0.82f), // explosion area radius - linear was too much
            time,
            frame_dt);

        if (m_damage_radius > 0.0f)
            RadiusDamage(
                *GetPhysicsHandler(),
                *GetObjectLayer(),
                *m_owner,
                this,
                m_damage_amount,
                Translation(),
                m_damage_radius,
                Mortal::D_EXPLOSION,
                m_owner, // do not damage the owner
                time,
                frame_dt);
    }

    Explosion::Think(time, frame_dt);
}

void DamageExplosion::Collide (
    Entity &collider,
    FloatVector2 const &collision_location,
    FloatVector2 const &collision_normal,
    Float collision_force,
    Time time,
    Time::Delta frame_dt)
{
    // can't damage nonsolid objects
    if (collider.GetCollisionType() == Engine2::Circle::CT_NONSOLID_COLLISION)
        return;

    // return if nothing would actually be done
    Float reverse_lifetime_ratio = 1.0f - LifetimeRatio(time);
    if (reverse_lifetime_ratio < 0.0f)
        return;

    // center_to_center points towards the collider
    FloatVector2 center_to_center = collider.Translation() - Translation();
    Float distance = center_to_center.Length() - collider.PhysicalRadius();
    if (distance < 0.0f)
        distance = 0.0f;
    Float distance_factor;
    if (distance < 1.0f)
        distance_factor = 1.0f;
    else
        distance_factor = 1.0f / Math::Sqrt(distance);

    // if it's a Mortal, damage it (unless it's the owner)
    if (collider.IsMortal() && &collider != *m_owner)
        DStaticCast<Mortal *>(&collider)->Damage(
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
    Entity &collider,
    FloatVector2 const &collision_location,
    FloatVector2 const &collision_normal,
    Float collision_force,
    Time time,
    Time::Delta frame_dt)
{
    if (collider.GetCollisionType() == Engine2::Circle::CT_NONSOLID_COLLISION)
        return;

    // only affect ships that aren't the owner
    if (collider.IsShip() && &collider != *m_owner)
        DStaticCast<Ship *>(&collider)->AccumulateDisableTime(m_disable_time_factor * Min(frame_dt, 1.0f / 20.0f));
}

void EMPExplosion::HandleNewOwnerObject ()
{
    // set the color mask of the sprite to the correct color and opacity.
    OwnerObject()->ColorMask() = Color(0.0f, 0.0f, 1.0f, 1.0f);
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

void Fireball::Think (Time time, Time::Delta frame_dt)
{
    // the power decays as time goes on
    m_current_damage -= m_potential_damage * frame_dt / TimeToLive();

    Explosion::Think(time, frame_dt);

    // update the sprite's alpha value to reflect the damage left
    Float alpha_value = Max(0.0f, m_current_damage / m_potential_damage);
    ASSERT1(alpha_value <= 1.0f);
    OwnerObject()->ColorMask() = Color(1.0f, 1.0f, 1.0f, alpha_value);
}

void Fireball::Collide (
    Entity &collider,
    FloatVector2 const &collision_location,
    FloatVector2 const &collision_normal,
    Float collision_force,
    Time time,
    Time::Delta frame_dt)
{
    // if there is no power left, return (this can happen when a fireball
    // gets used up on one object before all its collisions are computed)
    if (m_current_damage <= 0.0f)
        return;

    // don't damage the owner
    if (&collider == *m_owner)
        return;

    // TODO: when napalm is done, check if it hit napalm

    // we only care about hitting solid things
    if (collider.GetCollisionType() == Engine2::Circle::CT_NONSOLID_COLLISION)
        return;

    static Float const s_damage_dissipation_rate = 2.0f;

    if (collider.IsMortal())
    {
        Mortal &mortal = *DStaticCast<Mortal *>(&collider);
        Float damage_to_inflict = Min(m_current_damage, s_damage_dissipation_rate * m_potential_damage * frame_dt);
        m_current_damage -= damage_to_inflict;
        mortal.Damage(
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

void LaserBeam::SetIntensity (Float intensity)
{
    ASSERT1(intensity >= 0.0f && intensity <= 1.0f);
    ASSERT1(IsInWorld());
    OwnerObject()->ColorMask() = Color(1.0f, 1.0f, 1.0f, intensity);
}

void LaserBeam::SnapToShip (FloatVector2 const &muzzle_location, FloatVector2 const &hit_location, Float beam_width)
{
    ASSERT1(IsInWorld());
    FloatVector2 beam_vector(hit_location - muzzle_location);
    SetTranslation(0.5f * (muzzle_location + hit_location));
    SetScaleFactors(FloatVector2(0.5f * beam_vector.Length(), 0.5f * beam_width));
    SetAngle(Math::Arg(beam_vector));
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

void LaserImpactEffect::SnapToLocationAndSetScaleFactor (FloatVector2 const &location, Float scale_factor)
{
    RemoveFromWorld();
    SetTranslation(location);
    SetScaleFactor(scale_factor);
    AddBackIntoWorld();
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

void TractorBeam::SetParameters (Color const &color, Float intensity)
{
    ASSERT1(intensity >= 0.0f && intensity <= 1.0f);
    ASSERT1(IsInWorld());
    static Float const s_opacity = 0.25f;
    OwnerObject()->ColorMask() = color;
    OwnerObject()->ColorMask()[Dim::A] = s_opacity*intensity;
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

void ShieldEffect::SetIntensity (Float intensity)
{
    ASSERT1(intensity >= 0.0f && intensity <= 1.0f);
    static Float const s_opacity = 0.5f;
    OwnerObject()->ColorMask() = Color(1.0f, 1.0f, 1.0f, s_opacity*intensity);
}

void ShieldEffect::SnapToShip (FloatVector2 const &ship_translation, Float ship_radius)
{
    static Float const s_shield_size_ratio = 1.666f;

    RemoveFromWorld();
    SetTranslation(ship_translation);
    SetScaleFactor(s_shield_size_ratio * ship_radius);
    AddBackIntoWorld();
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

void LightningEffect::SnapToShip (FloatVector2 const &ship_translation, Float ship_radius)
{
    static Float const s_lightning_size_ratio = 1.2f;

    RemoveFromWorld();
    SetTranslation(ship_translation);
    SetScaleFactor(s_lightning_size_ratio * ship_radius);
    AddBackIntoWorld();
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

void ReticleEffect::SnapToLocationAndSetScaleFactor (FloatVector2 const &location, Float scale_factor)
{
    RemoveFromWorld();
    SetTranslation(location);
    SetScaleFactor(scale_factor);
    AddBackIntoWorld();
}

} // end of namespace Dis

