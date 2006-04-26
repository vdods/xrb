// ///////////////////////////////////////////////////////////////////////////
// dis_explosive.cpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_explosive.h"

#include "dis_physicshandler.h"
#include "dis_ship.h"
#include "dis_spawn.h"
#include "dis_util.h"
#include "dis_weapon.h"
#include "dis_world.h"
#include "xrb_engine2_objectlayer.h"

using namespace Xrb;

namespace Dis
{

void Explosive::Collide (
    Entity *const collider,
    FloatVector2 const &collision_location,
    FloatVector2 const &collision_normal,
    Float const collision_force,
    Float const time,
    Float const frame_dt)
{
    // we don't want to detonate on powerups
    if (collider->GetIsPowerup())
        return;

    // if it's a solid collision object, potentially detonate
    if (collider->GetCollisionType() == CT_SOLID_COLLISION && !GetIsDead())
        if (!GetHasDetonated() && CheckIfItShouldDetonate(collider, time, frame_dt))
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
    ASSERT1(!GetHasDetonated())
    m_has_detonated = true;
    ScheduleForDeletion(0.0f);
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

Grenade::Grenade (
    GrenadeLauncher *const owner_grenade_launcher,
    Float const damage_to_inflict,
    Float const damage_radius,
    Float const explosion_radius,
    Uint32 const weapon_level,
    EntityReference<Entity> const &owner,
    Float const max_health)
    :
    Explosive(owner, max_health, max_health, ET_GRENADE, CT_SOLID_COLLISION),
    m_damage_to_inflict(damage_to_inflict),
    m_damage_radius(damage_radius),
    m_explosion_radius(explosion_radius),
    m_weapon_level(weapon_level)
{
    ASSERT1(m_damage_to_inflict > 0.0f)
    m_owner_grenade_launcher = owner_grenade_launcher;
    SetImmunity(D_COLLISION|D_EXPLOSION);
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
    if (GetHasDetonated())
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
    ASSERT1(collider != NULL)
    ASSERT1(collider->GetCollisionType() == CT_SOLID_COLLISION)
    ASSERT1(!GetIsDead())

    // this grenade is dumb, just detonate
    return true;
}

void Grenade::Detonate (
    Float const time,
    Float const frame_dt)
{
    ASSERT1(!GetHasDetonated())
    
    // can't detonate if we're already dead
    if (GetIsDead())
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

    // if there is an owner grenade launcher, signal to it that this
    // grenade has been destroyed
    if (m_owner_grenade_launcher != NULL)
        m_owner_grenade_launcher->ActiveGrenadeDestroyed(this);

    Explosive::Detonate(time, frame_dt);
}

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

void Mine::GiveSeekCoordinates (
    FloatVector2 const &seek_coordinates,
    Float const seek_start_time)
{
    ASSERT1(seek_start_time > m_seek_start_time)

    m_seek_coordinates = seek_coordinates;
    m_seek_start_time = seek_start_time;
    m_think_state = &Mine::Level0MoveTowardsSeekCoordinates;
}

void Mine::Think (
    Float const time,
    Float const frame_dt)
{
    // if we're dead, don't bother thinking
    if (GetIsDead())
        return;
    
    // level 0 : no seeking, dumb triggering, no ambient velocity matching (2 mines)
    // level 1 : no seeking, dumb triggering, uses ambient velocity matching (3 mines)
    // level 2 : dumb seeking, smart triggering, uses ambient velocity matching (4 mines)
    // level 3 : smart seeking, smart triggering, uses ambient velocity matching (6 mines)

    (this->*m_think_state)(time, frame_dt);
}

void Mine::Die (
    Entity *const killer,
    Entity *const kill_medium,
    FloatVector2 const &kill_location,
    FloatVector2 const &kill_normal,
    Float const kill_force,
    DamageType const kill_type,
    Float const time,
    Float const frame_dt)
{
    if (GetHasDetonated())
        return;

    // if there is an owner mine layer, signal to it that this
    // mine has been destroyed
    if (m_owner_mine_layer != NULL)
        m_owner_mine_layer->ActiveMineDestroyed(this);
        
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

bool Mine::CheckIfItShouldDetonate (
    Entity *const collider,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(collider != NULL)
    ASSERT1(collider->GetCollisionType() == CT_SOLID_COLLISION)
    ASSERT1(!GetIsDead())

    // this mine is dumb, just detonate
    return true;
}

void Mine::Detonate (
    Float const time,
    Float const frame_dt)
{
    ASSERT1(!GetHasDetonated())

    // can't detonate if we're dead
    if (GetIsDead())
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

    // if there is an owner mine layer, signal to it that this
    // mine has been destroyed
    if (m_owner_mine_layer != NULL)
        m_owner_mine_layer->ActiveMineDestroyed(this);

    Explosive::Detonate(time, frame_dt);
}

void Mine::Level0Survey (Float const time, Float const frame_dt)
{
    static Float const s_survey_area_radius = 100.0f;

    m_most_recent_survey_time = time;

    // store the ambient velocity, ignoring the presence of the owner ship
    m_ambient_velocity =
        GetAmbientVelocity(
            s_survey_area_radius,
            GetOwnerMineLayer()->GetOwnerShip());

    // call Level0FireThrusters
    Level0FireThrusters(time, frame_dt);

    // set Level0FireThrusters as the think state
    m_think_state = &Mine::Level0FireThrusters;
}

void Mine::Level0FireThrusters (Float const time, Float const frame_dt)
{
    static Float const s_survey_interval_time = 0.5f;
    static Float const s_max_thrust_force = 100.0f * GetFirstMoment();

    // calculate what thrust is required to match the ambient velocity
    FloatVector2 velocity_differential =
        m_ambient_velocity -
        (GetVelocity() + frame_dt * GetForce() / GetFirstMoment());
    FloatVector2 thrust_vector = GetFirstMoment() * velocity_differential / frame_dt;
    if (!thrust_vector.GetIsZero())
    {
        Float thrust_force = thrust_vector.GetLength();
        if (thrust_force > s_max_thrust_force)
            thrust_vector = s_max_thrust_force * thrust_vector.GetNormalization();

        AccumulateForce(thrust_vector);
    }

    // check if enough time has passed since the last survey, and if so
    // change the think state to Level0Survey
    if (m_most_recent_survey_time + s_survey_interval_time <= time)
        m_think_state = &Mine::Level0Survey;
}

void Mine::Level0MoveTowardsSeekCoordinates (Float const time, Float const frame_dt)
{
    static Float const s_seek_attention_span = 0.1f;
    static Float const s_max_thrust_force = 100.0f * GetFirstMoment();

    if (m_seek_start_time + s_seek_attention_span <= time)
    {
        m_think_state = &Mine::Level0Survey;
        (this->*m_think_state)(time, frame_dt);
        return;
    }
    
    // TODO: the fancy intercept course shit later

    // adjust the seek coordinates for space wrapping
    FloatVector2 adjusted_seek_coordinates(
        GetObjectLayer()->GetAdjustedCoordinates(
            m_seek_coordinates,
            GetTranslation()));
    // for now just accelerate towards the seek coordinates
    FloatVector2 thrust_vector(s_max_thrust_force * (adjusted_seek_coordinates - GetTranslation()).GetNormalization());
    AccumulateForce(thrust_vector);
}


// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

void Missile::Think (
    Float const time,
    Float const frame_dt)
{
    // if we're dead, don't bother thinking
    if (GetIsDead())
        return;

    static Float const s_thrust_force = 300.0f * GetFirstMoment();
    AccumulateForce(s_thrust_force * Math::UnitVector(GetAngle()));

    // lazily initialize m_initial_velocity with the owner's velocity
    if (m_first_think)
    {
        m_initial_velocity = m_owner->GetVelocity();
        m_first_think = false;
    }

    // update the angle to reflect the direction of motion
    {
        FloatVector2 velocity_delta(GetVelocity() - m_initial_velocity);
        if (!velocity_delta.GetIsZero())
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
            GetPhysicalRadius(),
            false,
            &line_trace_binding_set);
    
        FloatVector2 collision_normal(trace_vector.GetNormalization());
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
    ASSERT1(collider != NULL)
    ASSERT1(collider->GetCollisionType() == CT_SOLID_COLLISION)
    ASSERT1(!GetIsDead())

    // don't detonate on ourselves
    if (collider == this)
        return false;

    // don't detonate on powerups
    if (collider->GetIsPowerup())
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
    ASSERT1(!GetHasDetonated())

    // can't detonate if we're dead
    if (GetIsDead())
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
    ASSERT1(!GetHasDetonated())

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
    ASSERT1(collider != NULL)
    ASSERT1(collider->GetCollisionType() == CT_SOLID_COLLISION)
    ASSERT1(!GetIsDead())

    // the emp_bomb is dumb, just detonate
    return true;
}

void EMPBomb::Detonate (
    Float const time,
    Float const frame_dt)
{
    ASSERT1(!GetIsDead())

    if (!GetHasDetonated())
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

