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

namespace Dis
{

class Effect : public Entity
{
public:

    Effect (
        Float time_to_live,
        Float time_at_birth,
        EntityType entity_type,
        Engine2::Circle::CollisionType collision_type)
        :
        Entity(entity_type, collision_type),
        m_base_color_mask(Color::ms_opaque_white)
    {
        ASSERT1(time_to_live != 0.0f);
        ASSERT1(time_at_birth >= 0.0f);
        m_time_to_live = time_to_live;
        m_time_at_birth = time_at_birth;
    }
    virtual ~Effect () { }

    virtual void Think (Float time, Float frame_dt);

protected:

    Float TimeToLive () const { return m_time_to_live; }
    Float TimeAtBirth () const { return m_time_at_birth; }
    Float LifetimeRatio (Float current_time) const
    {
        ASSERT1(current_time >= m_time_at_birth);
        if (m_time_to_live < 0.0f)
            return 0.0f;
        else
            return Min(1.0f, (current_time - m_time_at_birth) / m_time_to_live);
    }
    Color const &BaseColorMask () const { return m_base_color_mask; }
    Color &BaseColorMask () { return m_base_color_mask; }
    virtual bool IsEffect () const { return true; }

private:

    // indicates how long this effect will last (negative values indicate that
    // "IT... WILL LAST... FOREVAH!! ...IT... WILL LAST... FOREVAH!!!")
    Float m_time_to_live;
    // gives a reference time which can be used in calculations in Think()
    Float m_time_at_birth;
    // the baseline color mask (to whose alpha channel the value
    // 1.0f - LifetimeRatio(time) will be applied).  The default is opaque white.
    Color m_base_color_mask;
}; // end of class DamageEffect

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class Explosion : public Effect
{
public:

    Explosion (
        Float final_size,
        Float time_to_live,
        Float time_at_birth,
        EntityType entity_type,
        Engine2::Circle::CollisionType collision_type)
        :
        Effect(time_to_live, time_at_birth, entity_type, collision_type)
    {
        ASSERT1(final_size > 0.0f);
        m_final_size = final_size;
    }

    inline Float FinalSize () const { return m_final_size; }

    virtual void Think (Float time, Float frame_dt);

private:

    Float m_final_size;
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
        Float explosion_radius,
        Float time_to_live,
        Float time_at_birth,
        EntityReference<Entity> const &owner)
        :
        Explosion(explosion_radius, time_to_live, time_at_birth, ET_DAMAGE_EXPLOSION, Engine2::Circle::CT_NONSOLID_COLLISION),
        m_damage_amount(damage_amount),
        m_damage_radius(damage_radius),
        m_owner(owner)
    {
        ASSERT1(m_damage_amount > 0.0f);
        m_has_done_impact = false;
    }

    virtual void Think (Float time, Float frame_dt);
    virtual void Collide (
        Entity *collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
        Float time,
        Float frame_dt);

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
        Float final_size,
        Float time_to_live,
        Float time_at_birth)
        :
        Explosion(final_size, time_to_live, time_at_birth, ET_NO_DAMAGE_EXPLOSION, Engine2::Circle::CT_NO_COLLISION)
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
        Float final_size,
        Float time_to_live,
        Float time_at_birth,
        EntityReference<Entity> const &owner)
        :
        Explosion(final_size, time_to_live, time_at_birth, ET_EMP_EXPLOSION, Engine2::Circle::CT_NONSOLID_COLLISION),
        m_disable_time_factor(disable_time_factor),
        m_owner(owner)
    {
        ASSERT1(m_disable_time_factor > 1.0f);
        // i just blue myself.
        BaseColorMask() = Color(0.0f, 0.0f, 1.0f, 1.0f);
    }

    virtual void Collide (
        Entity *collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
        Float time,
        Float frame_dt);

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
        Float final_size,
        Float time_to_live,
        Float time_at_birth,
        EntityReference<Entity> const &owner)
        :
        Explosion(final_size, time_to_live, time_at_birth, ET_FIREBALL, Engine2::Circle::CT_NONSOLID_COLLISION),
        m_potential_damage(potential_damage),
        m_owner(owner)
    {
        ASSERT1(starting_damage >= 0.0f);
        ASSERT1(m_potential_damage > 0.0f);
        m_current_damage = starting_damage;
    }
    virtual ~Fireball () { }

    virtual void Think (Float time, Float frame_dt);
    virtual void Collide (
        Entity *collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
        Float time,
        Float frame_dt);

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

// class AsteroidDebris;

class LaserBeam : public Effect
{
public:

    LaserBeam ()
        :
        Effect(-1.0f, 0.0f, ET_LASER_BEAM, Engine2::Circle::CT_NO_COLLISION)
    { }

    // this is mainly just to override Effect::Think which changes the alpha
    virtual void Think (Float time, Float frame_dt) { }

    void SetIntensity (Float intensity);

    void SnapToShip (
        FloatVector2 const &muzzle_location,
        FloatVector2 const &hit_location,
        Float beam_width);
}; // end of class LaserBeam

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class GaussGunTrail : public Effect
{
public:

    GaussGunTrail (Float time_to_live, Float time_at_birth)
        :
        Effect(time_to_live, time_at_birth, ET_GAUSS_GUN_TRAIL, Engine2::Circle::CT_NO_COLLISION)
    { }
}; // end of class GaussGunTrail

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class TractorBeam : public Effect
{
public:

    TractorBeam ()
        :
        Effect(-1.0f, 0.0f, ET_TRACTOR_BEAM, Engine2::Circle::CT_NO_COLLISION)
    { }

    // this is mainly just to override Effect::Think which changes the alpha
    virtual void Think (Float time, Float frame_dt) { }

    void SetParameters (bool push_instead_of_pull, Float intensity);
}; // end of class TractorBeam

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class ShieldEffect : public Effect
{
public:

    ShieldEffect ()
        :
        Effect(-1.0f, 0.0f, ET_SHIELD_EFFECT, Engine2::Circle::CT_NO_COLLISION)
    { }

    // this is mainly just to override Effect::Think which changes the alpha
    virtual void Think (Float time, Float frame_dt) { }

    void SetIntensity (Float intensity);

    void SnapToShip (
        FloatVector2 const &ship_translation,
        Float ship_scale_factor);
}; // end of class ShieldEffect

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class ReticleEffect : public Effect
{
public:

    ReticleEffect ()
        :
        Effect(-1.0f, 0.0f, ET_RETICLE_EFFECT, Engine2::Circle::CT_NO_COLLISION)
    { }

    // this is mainly just to override Effect::Think which changes the alpha
    virtual void Think (Float time, Float frame_dt) { }

    void SnapToLocationAndSetScaleFactor (
        FloatVector2 const &location,
        Float scale_factor);
}; // end of class ReticleEffect

} // end of namespace Dis

#endif // !defined(_DIS_EFFECT_HPP_)

