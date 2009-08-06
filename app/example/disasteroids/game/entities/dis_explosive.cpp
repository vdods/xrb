// ///////////////////////////////////////////////////////////////////////////
// dis_explosive.cpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_explosive.hpp"

#include "dis_physicshandler.hpp"
#include "dis_ship.hpp"
#include "dis_spawn.hpp"
#include "dis_util.hpp"
#include "dis_weapon.hpp"
#include "dis_world.hpp"
#include "xrb_engine2_objectlayer.hpp"

using namespace Xrb;

namespace Dis
{

Float const Missile::ms_acceleration[UPGRADE_LEVEL_COUNT] = { 200.0f, 225.0f, 250.0f, 300.0f };

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

void Explosive::Collide (
    Entity *const collider,
    FloatVector2 const &collision_location,
    FloatVector2 const &collision_normal,
    Float const collision_force,
    Float const time,
    Float const frame_dt)
{
    // we don't want to detonate on powerups
    if (collider->IsPowerup())
        return;

    // if it's a solid collision object, potentially detonate
    if (collider->GetCollisionType() == CT_SOLID_COLLISION && !IsDead())
        if (!HasDetonated() && CheckIfItShouldDetonate(collider, time, frame_dt))
            Detonate(time, frame_dt);

    // call the superclass collide
    Mortal::Collide(
        collider,
        collision_location,
        collision_normal,
        collision_force,
        time,
        frame_dt);
}

void Explosive::Die (
    Entity *const killer,
    Entity *const kill_medium,
    FloatVector2 const &kill_location,
    FloatVector2 const &kill_normal,
    Float const kill_force,
    DamageType const kill_type,
    Float const time,
    Float const frame_dt)
{
    if (!m_has_detonated)
    {
        // do not detonate. spawn a small explosion
        SpawnNoDamageExplosion(
            GetWorld(),
            GetObjectLayer(),
            GetTranslation(),
            GetVelocity(),
            GetScaleFactor(),
            1.0f,
            time);

        // delete this object
        ScheduleForDeletion(0.0f);
    }
}

void Explosive::Detonate (
    Float const time,
    Float const frame_dt)
{
    ASSERT1(!HasDetonated());
    m_has_detonated = true;
    ScheduleForDeletion(0.0f);
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

Float const Grenade::ms_default_mass = 4.0f;
Float const Grenade::ms_merge_power_boost = 1.1f;

Grenade::Grenade (
    GrenadeLauncher *const owner_grenade_launcher,
    Float const damage_to_inflict,
    Float const damage_radius,
    Float const explosion_radius,
    Uint32 const weapon_level,
    EntityReference<Entity> const &owner,
    Float const max_health)
    :
    Explosive(weapon_level, owner, max_health, max_health, ET_GRENADE, CT_SOLID_COLLISION),
    m_damage_to_inflict(damage_to_inflict),
    m_damage_radius(damage_radius),
    m_explosion_radius(explosion_radius)
{
    ASSERT1(m_damage_to_inflict > 0.0f);
    m_owner_grenade_launcher = owner_grenade_launcher;
    SetImmunity(D_COLLISION|D_EXPLOSION);
}

void Grenade::Collide (
    Entity *const collider,
    FloatVector2 const &collision_location,
    FloatVector2 const &collision_normal,
    Float const collision_force,
    Float const time,
    Float const frame_dt)
{
    // if we hit a grenade of the same owner, don't detonate.  instead, merge together,
    if (collider->GetEntityType() == ET_GRENADE && DStaticCast<Grenade *>(collider)->m_owner == m_owner)
    {
        Grenade *other_grenade = static_cast<Grenade *>(collider);
        // because Collide will be called on both grenades, we need a way to ensure the
        // grenade merging only happens once, and that only one grenade goes away.  thus
        // we'll merge the smaller grenade into the larger one, or if they're the same
        // size, we'll use the lower pointer value grenade.
        ASSERT1(this != other_grenade); // a grenade should never collide with itself
        if (Mass() < other_grenade->Mass()
            ||
            (Mass() == other_grenade->Mass() &&
             this > other_grenade))
        {
            return;
        }

        // figure out the new damage radius, explosion radius and damage to inflict.
        Float scale_factor = Math::Sqrt(1.0f + other_grenade->Mass() / Mass());
        OwnerObject()->Scale(scale_factor);
        m_damage_radius *= scale_factor;    // maybe this is too much.  maybe use sqrt(scale_factor).
        m_explosion_radius *= scale_factor; // maybe this is too much.  maybe use sqrt(scale_factor).
        m_damage_to_inflict += other_grenade->m_damage_to_inflict;

        // set the new max health for the new grenade is the sum of the source grenades'
        // max healths.  the current health for the new grenade is also the sum.
        SetMaxHealth(MaxHealth() + other_grenade->MaxHealth());
        SetCurrentHealth(CurrentHealth() + other_grenade->CurrentHealth());
        ResetRecentChangeInHealth();

        // figure out the new mass, velocity, and radius.
        FloatVector2 new_momentum = Momentum() + other_grenade->Momentum();
        SetMass(Mass() + other_grenade->Mass());
        SetMomentum(new_momentum); // have to set this after setting the first moment

        // let the other grenade's owner's grenade launcher know that it's going bye-bye
        if (other_grenade->m_owner_grenade_launcher != NULL)
            other_grenade->m_owner_grenade_launcher->ActiveGrenadeDestroyed(other_grenade);
        // make it go bye-bye
        other_grenade->ScheduleForDeletion(0.0f);
    }
    // otherwise, if we did not hit the owner of this grenade, do normal explosive collision handling
    else if (collider != *m_owner)
    {
        // TODO only call the superclass collide if the collision registers as above
        // the threshold for detonation -- i.e. a large merged grenade won't detonate
        // on some tiny asteroid, but a normal sized grenade will detonate on anything.

        // call the superclass collide
        Explosive::Collide(
            collider,
            collision_location,
            collision_normal,
            collision_force,
            time,
            frame_dt);
    }
}

void Grenade::Die (
    Entity *const killer,
    Entity *const kill_medium,
    FloatVector2 const &kill_location,
    FloatVector2 const &kill_normal,
    Float const kill_force,
    DamageType const kill_type,
    Float const time,
    Float const frame_dt)
{
    if (HasDetonated())
        return;

    // if there is an owner grenade launcher, signal to it that this
    // grenade has been destroyed
    if (m_owner_grenade_launcher != NULL)
        m_owner_grenade_launcher->ActiveGrenadeDestroyed(this);

    Explosive::Die(
        killer,
        kill_medium,
        kill_location,
        kill_normal,
        kill_force,
        kill_type,
        time,
        frame_dt);
}

bool Grenade::CheckIfItShouldDetonate (
    Entity *const collider,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(collider != NULL);
    ASSERT1(collider->GetCollisionType() == CT_SOLID_COLLISION);
    ASSERT1(!IsDead());

    // this grenade is dumb, just detonate
    return true;
}

void Grenade::Detonate (
    Float const time,
    Float const frame_dt)
{
    ASSERT1(!HasDetonated());

    // can't detonate if we're already dead
    if (IsDead())
        return;

    // grenade merging will result in grenades which inflict more damage
    // than just the sum of their source grenades.  thus we multiply
    // the amount of damage to inflict by a factor which depends on the
    // ratio of mass to default mass.
    Float damage_factor = Math::Pow(ms_merge_power_boost, Mass() / ms_default_mass);

    // spawn a damage explosion
    SpawnDamageExplosion(
        GetWorld(),
        GetObjectLayer(),
        GetTranslation(),
        GetVelocity(),
        m_damage_to_inflict*damage_factor,
        m_damage_radius,
        m_explosion_radius,
        0.2f,
        time,
        m_owner);

    // if there is an owner grenade launcher, signal to it that this
    // grenade has been destroyed
    if (m_owner_grenade_launcher != NULL)
        m_owner_grenade_launcher->ActiveGrenadeDestroyed(this);

    Explosive::Detonate(time, frame_dt);
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

void Missile::Think (
    Float const time,
    Float const frame_dt)
{
    // call the superclass' Think
    Explosive::Think(time, frame_dt);

    // if we're dead or have detonated, don't bother thinking
    if (IsDead() || HasDetonated())
        return;

    AccumulateForce(ms_acceleration[GetWeaponLevel()] * Mass() * Math::UnitVector(Angle()));

    // lazily initialize m_initial_velocity with the owner's velocity
    // (if the owner even still exists)
    if (m_first_think && m_owner.IsValid())
    {
        m_initial_velocity = m_owner->GetVelocity();
        m_first_think = false;
    }

    // update the angle to reflect the direction of motion
    {
        FloatVector2 velocity_delta(GetVelocity() - m_initial_velocity);
        if (!velocity_delta.IsZero())
            SetAngle(Math::Atan(velocity_delta));
    }

    // if the time to live has expired, detonate.
    if (m_time_at_birth + m_time_to_live <= time)
        Detonate(time, frame_dt);
    // otherwise, peform a line trace for frame_dt/2 time's
    // worth of velocity in front and behind this missile
    else
    {
        FloatVector2 trace_vector(frame_dt * GetVelocity());
        FloatVector2 trace_start(GetTranslation() - 0.5f * trace_vector);
        LineTraceBindingSet line_trace_binding_set;
        GetPhysicsHandler()->LineTrace(
            GetObjectLayer(),
            trace_start,
            trace_vector,
            PhysicalRadius(),
            false,
            &line_trace_binding_set);

        FloatVector2 collision_normal(trace_vector.Normalization());
        for (LineTraceBindingSetIterator it = line_trace_binding_set.begin(),
                                         it_end = line_trace_binding_set.end();
             it != it_end;
             ++it)
        {
            if (CheckIfItShouldDetonate(it->m_entity, time, frame_dt))
            {
                Detonate(time, frame_dt);
                break;
            }
        }
    }
}

bool Missile::CheckIfItShouldDetonate (
    Entity *const collider,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(collider != NULL);
    ASSERT1(collider->GetCollisionType() == CT_SOLID_COLLISION);
    ASSERT1(!HasDetonated());
    ASSERT1(!IsDead());

    // don't detonate on ourselves
    if (collider == this)
        return false;

    // don't detonate on powerups
    if (collider->IsPowerup())
        return false;

    // don't detonate on ballistics (because then it would make it
    // pointless to try to shoot missiles down with ballistic weapons)
    if (collider->IsBallistic())
        return false;

    // don't detonate on the entity that fired this missile
    if (collider == *m_owner)
        return false;

    // otherwise, detonate
    return true;
}

void Missile::Detonate (
    Float const time,
    Float const frame_dt)
{
    ASSERT1(!HasDetonated());

    // can't detonate if we're dead
    if (IsDead())
        return;

    // spawn a damage explosion
    SpawnDamageExplosion(
        GetWorld(),
        GetObjectLayer(),
        GetTranslation(),
        GetVelocity(),
        m_damage_to_inflict,
        m_damage_radius,
        m_explosion_radius,
        0.2f,
        time,
        m_owner);

    Explosive::Detonate(time, frame_dt);
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

void GuidedMissile::Think (Float const time, Float const frame_dt)
{
    if (time >= m_next_search_time)
        Search(time, frame_dt);
    else if (m_target.IsValid())
    {
        // if the missile is actively seeking a target, let it live longer
        m_time_to_live += 0.75f * frame_dt;
        Seek(time, frame_dt);
    }

    // call the superclass' Think
    Missile::Think(time, frame_dt);
}

EntityReference<Ship> GuidedMissile::FindTarget (LineTraceBindingSet const &scan_set)
{
    for (LineTraceBindingSetConstIterator it = scan_set.begin(),
                                          it_end = scan_set.end();
         it != it_end;
         ++it)
    {
        if (it->m_entity->IsShip() && it->m_entity != *m_owner)
            return it->m_entity->GetReference();
    }
    // if no target found, return an invalid reference
    return EntityReference<Ship>();
}

void GuidedMissile::Search (Float const time, Float const frame_dt)
{
    // don't search if there is a living target
    if (m_target.IsValid() && !m_target->IsDead())
    {
        m_next_search_time = time + 0.25f;
        return;
    }

    // do a line trace in front of the missile to find a target
    if (GetVelocity().LengthSquared() >= 0.001f)
    {
        m_next_search_time = time + 0.25f;

        static Float const s_search_distance = 400.0f;
        static Float const s_search_radius = 70.0f;

        LineTraceBindingSet line_trace_binding_set;
        GetPhysicsHandler()->LineTrace(
            GetObjectLayer(),
            GetTranslation(),
            s_search_distance * Math::UnitVector(Angle()),
            s_search_radius,
            false,
            &line_trace_binding_set);

        m_target = FindTarget(line_trace_binding_set);
    }
}

void GuidedMissile::Seek (Float const time, Float const frame_dt)
{
    ASSERT1(m_target.IsValid());

    if (m_target->IsDead())
    {
        m_target.Release();
        return;
    }

    FloatVector2 target_position(
        GetObjectLayer()->AdjustedCoordinates(
            m_target->GetTranslation(),
            GetTranslation()));

    // adjust our course to hit the target -- plot intercept course
    Float interceptor_acceleration = ms_acceleration[GetWeaponLevel()];
    FloatVector2 p(target_position - GetTranslation());
    FloatVector2 v(m_target->GetVelocity() - GetVelocity());
    FloatVector2 a(m_target->Force() / m_target->Mass());

    Polynomial poly;
    poly.Set(4, a.LengthSquared() - interceptor_acceleration*interceptor_acceleration);
    poly.Set(3, 4.0f * (a | v));
    poly.Set(2, 4.0f * ((a | p) + (v | v)));
    poly.Set(1, 8.0f * (p | v));
    poly.Set(0, 4.0f * (p | p));

    Polynomial::SolutionSet solution_set;
    poly.Solve(&solution_set, 0.001f);

    Float T = -1.0f;
    for (Polynomial::SolutionSetIterator it = solution_set.begin(),
                                         it_end = solution_set.end();
         it != it_end;
         ++it)
    {
        if (*it >= 0.0f)
        {
            T = *it;
            break;
        }
    }

    if (T <= 0.0f)
    {
        // if no acceptable solution, just do dumb approach
        AimAt(target_position);
    }
    else
    {
        FloatVector2 real_approach_direction((2.0f*p + 2.0f*v*T + a*T*T) / (interceptor_acceleration*T*T));
        AimAt(GetTranslation() + real_approach_direction);
    }
}

void GuidedMissile::AimAt (FloatVector2 const &position)
{
    FloatVector2 delta(position - GetTranslation());
    if (delta.LengthSquared() >= 0.001f)
        SetAngle(Math::Atan(delta));
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

EntityReference<Ship> GuidedEnemyMissile::FindTarget (LineTraceBindingSet const &scan_set)
{
    for (LineTraceBindingSetConstIterator it = scan_set.begin(),
                                          it_end = scan_set.end();
         it != it_end;
         ++it)
    {
        if (it->m_entity->GetEntityType() == ET_SOLITARY)
            return it->m_entity->GetReference();
    }
    // if no target found, return an invalid reference
    return EntityReference<Ship>();
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////
/*
void EMPBomb::Collide (
    Entity *const collider,
    FloatVector2 const &collision_location,
    FloatVector2 const &collision_normal,
    Float const collision_force,
    Float const time,
    Float const frame_dt)
{
    // we don't want to detonate on collisions, so just call the superclass collide.
    Mortal::Collide(
        collider,
        collision_location,
        collision_normal,
        collision_force,
        time,
        frame_dt);
}

void EMPBomb::Die (
    Entity *const killer,
    Entity *const kill_medium,
    FloatVector2 const &kill_location,
    FloatVector2 const &kill_normal,
    Float const kill_force,
    DamageType const kill_type,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(!HasDetonated());

    // if there is an owner emp_bomb launcher, signal to it that this
    // emp_bomb has been destroyed
    if (m_owner_emp_bomb_launcher != NULL)
        m_owner_emp_bomb_launcher->ActiveEMPBombDestroyed(this);

    Explosive::Die(
        killer,
        kill_medium,
        kill_location,
        kill_normal,
        kill_force,
        kill_type,
        time,
        frame_dt);
}

bool EMPBomb::CheckIfItShouldDetonate (
    Entity *const collider,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(collider != NULL);
    ASSERT1(collider->GetCollisionType() == CT_SOLID_COLLISION);
    ASSERT1(!IsDead());

    // the emp_bomb is dumb, just detonate
    return true;
}

void EMPBomb::Detonate (
    Float const time,
    Float const frame_dt)
{
    ASSERT1(!IsDead());

    if (!HasDetonated())
    {
        // TODO: EMP explosion
        // spawn a damage explosion
        SpawnEMPExplosion(
            GetWorld(),
            GetObjectLayer(),
            GetTranslation(),
            GetVelocity(),
            m_disable_time_factor,
            m_blast_radius,
            1.0f,
            time,
            m_owner);

        // if there is an owner emp_bomb launcher, signal to it that this
        // emp_bomb has been destroyed
        if (m_owner_emp_bomb_launcher != NULL)
            m_owner_emp_bomb_launcher->ActiveEMPBombDestroyed(this);

        Explosive::Detonate(time, frame_dt);
    }
}
*/
} // end of namespace Dis

