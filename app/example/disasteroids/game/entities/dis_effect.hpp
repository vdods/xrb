// ///////////////////////////////////////////////////////////////////////////
// dis_effect.hpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_EFFECT_HPP_)
#define _DIS_EFFECT_HPP_

#include "dis_entity.hpp"

using namespace Xrb;

namespace Dis {

// base class for all effect entities
class Effect : public Entity
{
public:

    Effect (EntityType entity_type, Engine2::Circle::CollisionType collision_type)
        :
        Entity(entity_type, collision_type)
    { }
    virtual ~Effect () { }

    virtual bool IsEffect () const { return true; }
}; // end of class Effect

// base class for finite-lifetime entities, e.g. explosions, fireballs, etc.
class FiniteLifetimeEffect : public Effect
{
public:

    FiniteLifetimeEffect (
        Time::Delta time_to_live,
        Time time_at_birth,
        EntityType entity_type,
        Engine2::Circle::CollisionType collision_type)
        :
        Effect(entity_type, collision_type),
        m_time_to_live(time_to_live),
        m_time_at_birth(time_at_birth),
        m_initial_color_mask(Color::ms_opaque_white),
        m_final_color_mask(Color::ms_transparent_white),
        m_color_mask_interpolation_power(1.0f),
        m_run_in_reverse(false)
    {
        ASSERT1(m_time_to_live > 0.0f);
    }
    virtual ~FiniteLifetimeEffect () { }

    Time::Delta TimeToLive () const { return m_time_to_live; }
    Time TimeAtBirth () const { return m_time_at_birth; }
    Float LifetimeRatio (Time current_time) const
    {
        ASSERT1(current_time >= m_time_at_birth);
        ASSERT1(m_time_to_live > 0.0f);
        return Min(1.0f, (current_time - m_time_at_birth) / m_time_to_live);
    }
    Color const &InitialColorMask () const { return m_initial_color_mask; }
    Color &InitialColorMask () { return m_initial_color_mask; }
    Color const &FinalColorMask () const { return m_final_color_mask; }
    Color &FinalColorMask () { return m_final_color_mask; }

    // the color mask interpolation power is the power that the interpolation parameter
    // is taken to to control the interpolation between the initial and final color masks
    Float GetColorMaskInterpolationPower () const { return m_color_mask_interpolation_power; }
    void SetColorMaskInterpolationPower (Float color_mask_interpolation_power) { ASSERT1(color_mask_interpolation_power > 0.0f && "color_mask_interpolation_power must be positive"); m_color_mask_interpolation_power = color_mask_interpolation_power; }

    bool RunInReverse () const { return m_run_in_reverse; }
    void RunInReverse (bool run_in_reverse) { m_run_in_reverse = run_in_reverse; }

    virtual void Think (Time time, Time::Delta frame_dt);

private:

    // indicates how long this effect will last (must be positive)
    Time::Delta m_time_to_live;
    // gives a reference time which can be used in calculations in Think()
    Time m_time_at_birth;
    // the initial color mask.  The default is opaque white.
    Color m_initial_color_mask;
    // the final color mask.  The default is transparent white
    Color m_final_color_mask;
    // the power that the color mask interpolation parameter is taken to
    Float m_color_mask_interpolation_power;
    // if true, run the interpolation in reverse (this applies to subclasses as well)
    bool m_run_in_reverse;
}; // end of class FiniteLifetimeEffect

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class Explosion : public FiniteLifetimeEffect
{
public:

    Explosion (
        Float initial_size,
        Float final_size,
        Time::Delta time_to_live,
        Time time_at_birth,
        EntityType entity_type,
        Engine2::Circle::CollisionType collision_type)
        :
        FiniteLifetimeEffect(time_to_live, time_at_birth, entity_type, collision_type)
    {
        ASSERT1(initial_size >= 0.0f);
        ASSERT1(final_size >= 0.0f);
        m_initial_size = Max(initial_size, 0.1f); // a 0-radius object has problems
        m_final_size = Max(final_size, 0.1f);     // a 0-radius object has problems
        m_scale_interpolation_power = 0.5f; // default is 1/2 (i.e. sqrt) which models explosions well
    }

    Float InitialSize () const { return m_initial_size; }
    Float FinalSize () const { return m_final_size; }

    // the scale interpolation power is the power that the interpolation parameter
    // is taken to to control the interpolation between initial_size and final_size.
    Float GetScaleInterpolationPower () const { return m_scale_interpolation_power; }
    void SetScaleInterpolationPower (Float scale_interpolation_power) { ASSERT1(scale_interpolation_power > 0.0f && "scale_interpolation_power must be positive"); m_scale_interpolation_power = scale_interpolation_power; }

    virtual void Think (Time time, Time::Delta frame_dt);

private:

    Float m_initial_size;
    Float m_final_size;
    Float m_scale_interpolation_power;
}; // end of class Explosion

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class DamageExplosion : public Explosion
{
public:

    DamageExplosion (
        Float damage_amount,
        Float damage_radius,
        Float initial_size,
        Float explosion_radius,
        Time::Delta time_to_live,
        Time time_at_birth,
        EntityReference<Entity> const &owner)
        :
        Explosion(initial_size, explosion_radius, time_to_live, time_at_birth, ET_DAMAGE_EXPLOSION, Engine2::Circle::CT_NONSOLID_COLLISION),
        m_damage_amount(damage_amount),
        m_damage_radius(damage_radius),
        m_owner(owner)
    {
        ASSERT1(m_damage_amount > 0.0f);
        m_has_done_impact = false;
    }

    virtual void Think (Time time, Time::Delta frame_dt);
    virtual void Collide (
        Entity &collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
        Time time,
        Time::Delta frame_dt);

private:

    Float const m_damage_amount;
    Float const m_damage_radius;
    bool m_has_done_impact;
    EntityReference<Entity> m_owner;
}; // end of class DamageExplosion

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class NoDamageExplosion : public Explosion
{
public:

    NoDamageExplosion (
        Float initial_size,
        Float final_size,
        Time::Delta time_to_live,
        Time time_at_birth)
        :
        Explosion(initial_size, final_size, time_to_live, time_at_birth, ET_NO_DAMAGE_EXPLOSION, Engine2::Circle::CT_NO_COLLISION)
    { }
}; // end of class NoDamageExplosion

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class EMPExplosion : public Explosion
{
public:

    EMPExplosion (
        Float disable_time_factor,
        Float initial_size,
        Float final_size,
        Time::Delta time_to_live,
        Time time_at_birth,
        EntityReference<Entity> const &owner)
        :
        Explosion(initial_size, final_size, time_to_live, time_at_birth, ET_EMP_EXPLOSION, Engine2::Circle::CT_NONSOLID_COLLISION),
        m_disable_time_factor(disable_time_factor),
        m_owner(owner)
    {
        ASSERT1(m_disable_time_factor > 1.0f);
        // i just blue myself.
        InitialColorMask() = Color(0.0f, 0.0f, 1.0f, 1.0f);
        FinalColorMask() = Color(0.0f, 0.0f, 1.0f, 0.0f);
    }

    virtual void Collide (
        Entity &collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
        Time time,
        Time::Delta frame_dt);

    virtual void HandleNewOwnerObject ();

private:

    Float const m_disable_time_factor;
    EntityReference<Entity> m_owner;
}; // end of class EMPExplosion

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class Fireball : public Explosion
{
public:

    Fireball (
        Float starting_damage,
        Float potential_damage,
        Float initial_size,
        Float final_size,
        Time::Delta time_to_live,
        Time time_at_birth,
        EntityReference<Entity> const &owner)
        :
        Explosion(initial_size, final_size, time_to_live, time_at_birth, ET_FIREBALL, Engine2::Circle::CT_NONSOLID_COLLISION),
        m_potential_damage(potential_damage),
        m_owner(owner)
    {
        ASSERT1(starting_damage >= 0.0f);
        ASSERT1(m_potential_damage > 0.0f);
        m_current_damage = starting_damage;
    }
    virtual ~Fireball () { }

    virtual void Think (Time time, Time::Delta frame_dt);
    virtual void Collide (
        Entity &collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
        Time time,
        Time::Delta frame_dt);

private:

    // the max amount of damage this (class of) fireball can dissipate
    Float const m_potential_damage;
    // the amount of damage this fireball still has left to inflict on stuff
    Float m_current_damage;
    // the thing that fired this fireball
    EntityReference<Entity> m_owner;
}; // end of class Fireball

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class GaussGunTrail : public FiniteLifetimeEffect
{
public:

    GaussGunTrail (Time::Delta time_to_live, Time time_at_birth)
        :
        FiniteLifetimeEffect(time_to_live, time_at_birth, ET_GAUSS_GUN_TRAIL, Engine2::Circle::CT_NO_COLLISION)
    { }
}; // end of class GaussGunTrail

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class LaserBeam : public Effect
{
public:

    LaserBeam () : Effect(ET_LASER_BEAM, Engine2::Circle::CT_NO_COLLISION) { }

    void SetIntensity (Float intensity);

    void SnapToShip (FloatVector2 const &muzzle_location, FloatVector2 const &hit_location, Float beam_width);
}; // end of class LaserBeam

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class LaserImpactEffect : public Effect
{
public:

    LaserImpactEffect () : Effect(ET_LASER_IMPACT_EFFECT, Engine2::Circle::CT_NO_COLLISION) { }

    void SnapToLocationAndSetScaleFactor (FloatVector2 const &location, Float scale_factor);
}; // end of class LaserBeam

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class TractorBeam : public Effect
{
public:

    TractorBeam () : Effect(ET_TRACTOR_BEAM, Engine2::Circle::CT_NO_COLLISION) { }

    void SetParameters (Color const &color, Float intensity);
}; // end of class TractorBeam

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class ShieldEffect : public Effect
{
public:

    ShieldEffect () : Effect(ET_SHIELD_EFFECT, Engine2::Circle::CT_NO_COLLISION) { }

    void SetIntensity (Float intensity);

    void SnapToShip (FloatVector2 const &ship_translation, Float ship_radius);
}; // end of class ShieldEffect

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class LightningEffect : public Effect
{
public:

    LightningEffect () : Effect(ET_LIGHTNING_EFFECT, Engine2::Circle::CT_NO_COLLISION) { }

    void SnapToShip (FloatVector2 const &ship_translation, Float ship_radius);
}; // end of class LightningEffect

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class ReticleEffect : public Effect
{
public:

    ReticleEffect () : Effect(ET_RETICLE_EFFECT, Engine2::Circle::CT_NO_COLLISION) { }

    void SnapToLocationAndSetScaleFactor (FloatVector2 const &location, Float scale_factor);
}; // end of class ReticleEffect

} // end of namespace Dis

#endif // !defined(_DIS_EFFECT_HPP_)

