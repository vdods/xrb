// ///////////////////////////////////////////////////////////////////////////
// dis_effect.h by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_EFFECT_H_)
#define _DIS_EFFECT_H_

#include "dis_gameobject.h"

using namespace Xrb;

namespace Dis
{

class Effect : public GameObject
{
public:

    Effect (
        Float const time_to_live,
        Float const time_at_birth,
        Type const type,
        CollisionType const collision_type)
        :
        GameObject(type, collision_type)
    {
        ASSERT1(time_to_live != 0.0f)
        ASSERT1(time_at_birth >= 0.0f)
        m_time_to_live = time_to_live;
        m_time_at_birth = time_at_birth;
    }
    virtual ~Effect () { }

    virtual void Think (Float time, Float frame_dt);

protected:

    inline Float GetTimeToLive () const
    {
        return m_time_to_live;
    }
    inline Float GetTimeAtBirth () const
    {
        return m_time_at_birth;
    }
    inline Float GetLifetimeRatio (Float const current_time) const
    {
        ASSERT1(current_time >= m_time_at_birth)
        if (m_time_to_live < 0.0f)
            return 0.0f;
        else
            return (current_time - m_time_at_birth) / m_time_to_live;
    }
    virtual bool GetIsEffect () const { return true; }
    
private:

    // indicates how long this effect will last (negative values indicate that
    // "IT... WILL LAST... FOREVAH!! ...IT... WILL LAST... FOREVAH!!!")
    Float m_time_to_live;
    // gives a reference time which can be used in calculations in Think()
    Float m_time_at_birth;
}; // end of class DamageEffect

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class Explosion : public Effect
{
public:

    Explosion (
        Float const final_size,
        Float const time_to_live,
        Float const time_at_birth,
        Type const type,
        CollisionType const collision_type)
        :
        Effect(time_to_live, time_at_birth, type, collision_type)
    {
        ASSERT1(final_size > 0.0f)
        m_final_size = final_size;
    }

    inline Float GetFinalSize () const { return m_final_size; }
    
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
        Float const damage_amount,
        Float const damage_radius,
        Float const explosion_radius,
        Float const time_to_live,
        Float const time_at_birth,
        GameObjectReference<GameObject> const &owner)
        :
        Explosion(explosion_radius, time_to_live, time_at_birth, T_DAMAGE_EXPLOSION, CT_NONSOLID_COLLISION),
        m_damage_amount(damage_amount),
        m_damage_radius(damage_radius),
        m_owner(owner)
    {
        ASSERT1(m_damage_amount > 0.0f)
        m_has_done_impact = false;
    }

    virtual void Think (Float time, Float frame_dt);
    virtual void Collide (
        GameObject *collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
        Float time,
        Float frame_dt);

private:

    Float const m_damage_amount;
    Float const m_damage_radius;
    bool m_has_done_impact;
    GameObjectReference<GameObject> m_owner;
}; // end of class DamageExplosion

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class NoDamageExplosion : public Explosion
{
public:

    NoDamageExplosion (
        Float const final_size,
        Float const time_to_live,
        Float const time_at_birth)
        :
        Explosion(final_size, time_to_live, time_at_birth, T_NO_DAMAGE_EXPLOSION, CT_NO_COLLISION)
    { }
}; // end of class NoDamageExplosion

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class EMPExplosion : public Explosion
{
public:

    EMPExplosion (
        Float const disable_time_factor,
        Float const final_size,
        Float const time_to_live,
        Float const time_at_birth,
        GameObjectReference<GameObject> const &owner)
        :
        Explosion(final_size, time_to_live, time_at_birth, T_EMP_EXPLOSION, CT_NONSOLID_COLLISION),
        m_disable_time_factor(disable_time_factor),
        m_owner(owner)
    {
        ASSERT1(m_disable_time_factor > 1.0f)
    }

    virtual void Collide (
        GameObject *collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
        Float time,
        Float frame_dt);

private:

    Float const m_disable_time_factor;
    GameObjectReference<GameObject> m_owner;
}; // end of class EMPExplosion

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class Fireball : public Explosion
{
public:

    Fireball (
        Float damage,
        Float final_size,
        Float time_to_live,
        Float time_at_birth,
        GameObjectReference<GameObject> const &owner)
        :
        Explosion(final_size, time_to_live, time_at_birth, T_FIREBALL, CT_NONSOLID_COLLISION),
        m_initial_damage(damage),
        m_owner(owner)
    {
        ASSERT1(damage > 0.0f)
        m_current_damage = m_initial_damage;
    }
    virtual ~Fireball () { }

    virtual void Think (Float time, Float frame_dt);
    virtual void Collide (
        GameObject *collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
        Float time,
        Float frame_dt);
    
private:

    // the initial amount of damage given to this fireball to dissipate
    Float const m_initial_damage;
    // the amount of damage this fireball still has left to inflict on stuff
    Float m_current_damage;
    // the thing that fired this fireball
    GameObjectReference<GameObject> m_owner;
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
        Effect(-1.0f, 0.0f, T_LASER_BEAM, CT_NO_COLLISION)
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

    GaussGunTrail (Float const time_to_live, Float const time_at_birth)
        :
        Effect(time_to_live, time_at_birth, T_GAUSS_GUN_TRAIL, CT_NO_COLLISION)
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
        Effect(-1.0f, 0.0f, T_TRACTOR_BEAM, CT_NO_COLLISION)
    { }
    
    // this is mainly just to override Effect::Think which changes the alpha
    virtual void Think (Float time, Float frame_dt) { }

    void SetPullingInputAndIntensity (
        bool pull_everything,
        Float pulling_input,
        Float intensity);
}; // end of class TractorBeam

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class ShieldEffect : public Effect
{
public:

    ShieldEffect ()
        :
        Effect(-1.0f, 0.0f, T_SHIELD_EFFECT, CT_NO_COLLISION)
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
        Effect(-1.0f, 0.0f, T_RETICLE_EFFECT, CT_NO_COLLISION)
    { }

    // this is mainly just to override Effect::Think which changes the alpha
    virtual void Think (Float time, Float frame_dt) { }

    void SnapToLocationAndSetScaleFactor (
        FloatVector2 const &location,
        Float scale_factor);
}; // end of class ReticleEffect
        
} // end of namespace Dis

#endif // !defined(_DIS_EFFECT_H_)

