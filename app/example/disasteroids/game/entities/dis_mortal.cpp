// ///////////////////////////////////////////////////////////////////////////
// dis_mortal.cpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_mortal.h"

using namespace Xrb;

namespace Dis
{

void Mortal::SetDamageDissipationRate (Float const damage_dissipation_rate)
{
    ASSERT1(damage_dissipation_rate >= 0.0f)
    m_damage_dissipation_rate = damage_dissipation_rate;
    if (m_dissipated_damage_accumulator > m_damage_dissipation_rate)
        m_dissipated_damage_accumulator = m_damage_dissipation_rate;
}

void Mortal::SetWeakness (DamageType const weakness)
{
    ASSERT1((weakness & m_strength) == 0 && "Can't be weak and strong against the same thing")
    ASSERT1((weakness & m_immunity) == 0 && "Can't be weak and immune against the same thing")
    m_weakness = weakness;
}

void Mortal::SetStrength (DamageType const strength)
{
    ASSERT1((m_weakness & strength) == 0 && "Can't be weak and strong against the same thing")
    ASSERT1((strength & m_immunity) == 0 && "Can't be strong and immune against the same thing")
    m_strength = strength;
}

void Mortal::SetImmunity (DamageType const immunity)
{
    ASSERT1((m_weakness & immunity) == 0 && "Can't be weak and immune against the same thing")
    ASSERT1((m_strength & immunity) == 0 && "Can't be strong and immune against the same thing")
    m_immunity = immunity;
}

void Mortal::AddWeakness (DamageType const weakness)
{
    ASSERT1((weakness & m_strength) == 0 && "Can't be weak and strong against the same thing")
    ASSERT1((weakness & m_immunity) == 0 && "Can't be weak and immune against the same thing")
    m_weakness |= weakness;
}

void Mortal::AddStrength (DamageType const strength)
{
    ASSERT1((m_weakness & strength) == 0 && "Can't be weak and strong against the same thing")
    ASSERT1((strength & m_immunity) == 0 && "Can't be strong and immune against the same thing")
    m_strength |= strength;
}

void Mortal::AddImmunity (DamageType const immunity)
{
    ASSERT1((m_weakness & immunity) == 0 && "Can't be weak and immune against the same thing")
    ASSERT1((m_strength & immunity) == 0 && "Can't be strong and immune against the same thing")
    m_immunity |= immunity;
}

void Mortal::RemoveWeakness (DamageType weakness)
{
    m_weakness &= ~weakness;
}

void Mortal::RemoveStrength (DamageType strength)
{
    m_strength &= ~strength;
}

void Mortal::RemoveImmunity (DamageType immunity)
{
    m_immunity &= ~immunity;
}

void Mortal::Revive (Float time, Float frame_dt)
{
    ASSERT1(GetIsDead())
    SetCurrentHealth(m_max_health);
}

void Mortal::Kill (
    Entity *const killer,
    Entity *const kill_medium,
    FloatVector2 const &kill_location,
    FloatVector2 const &kill_normal,
    Float const kill_force,
    DamageType const kill_type,
    Float const time,
    Float const frame_dt)
{
    SetCurrentHealth(0.0f);
    Die(killer,
        kill_medium,
        kill_location,
        kill_normal,
        kill_force,
        kill_type,
        time,
        frame_dt);
}

void Mortal::Collide (
    Entity *collider,
    FloatVector2 const &collision_location,
    FloatVector2 const &collision_normal,
    Float collision_force,
    Float time,
    Float frame_dt)
{
    ASSERT1(collider != NULL)
    ASSERT1(collision_force >= 0.0f)

    static Float const s_collision_damage_coefficient = 0.0005f;

    // ships should not take damage from powerups
    if (GetIsShip() && collider->GetIsPowerup())
        return;        
    
    if (collision_force > 0.0f)
    {
        ASSERT1(collider->GetCollisionType() == CT_SOLID_COLLISION)
        Damage(
            collider,
            collider,
            s_collision_damage_coefficient * collision_force / GetFirstMoment(),
            NULL,
            collision_location,
            collision_normal,
            collision_force,
            D_COLLISION,
            time,
            frame_dt);
    }
}

bool Mortal::Damage (
    Entity *const damager,
    Entity *const damage_medium,
    Float const damage_amount,
    Float *const damage_amount_used,
    FloatVector2 const &damage_location,
    FloatVector2 const &damage_normal,
    Float const damage_force,
    DamageType const damage_type,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(damage_amount >= 0.0f)

    if (m_current_health <= 0.0f)
        return false;

    // make sure only one bit in the damage type is set
    ASSERT1((damage_type & (damage_type - 1)) == 0)

    ASSERT1((m_weakness & m_strength) == 0 && "Can't be weak and strong against the same thing")
    ASSERT1((m_weakness & m_immunity) == 0 && "Can't be weak and immune against the same thing")
    ASSERT1((m_strength & m_immunity) == 0 && "Can't be strong and immune against the same thing")

    if (damage_amount_used != NULL)
        *damage_amount_used = 0.0f;

    Float damage_factor;
    if (GetIsWeakAgainst(damage_type))
        damage_factor = 1.5f;
    else if (GetIsStrongAgainst(damage_type))
        damage_factor = 0.2f;
    else if (GetIsImmuneAgainst(damage_type))
        return false;
    else
        damage_factor = 1.0f;

    // update the damage threshold accumulator
    ASSERT1(m_dissipated_damage_accumulator >= 0.0f)
    ASSERT1(m_dissipated_damage_accumulator <= m_damage_dissipation_rate + 0.001f)
    ASSERT1(m_time_last_damaged <= time)
    // if it has been more than 1 second since the last time damaged, then
    // reset the damage threshold accumulator (because the damage threshold
    // measures the amount of damage that can be blocked per second).
    if (m_time_last_damaged + 1.0f <= time)
        m_dissipated_damage_accumulator = 0.0f;
    // otherwise, just linearly decay the accumulator down with time.
    else
        m_dissipated_damage_accumulator -= (time - m_time_last_damaged) * m_damage_dissipation_rate;

    // make sure the damage threshold accumulator doesn't go below zero
    if (m_dissipated_damage_accumulator < 0.0f)
        m_dissipated_damage_accumulator = 0.0f;
            
    // renew the last time damaged
    m_time_last_damaged = time;

    // calculate the damage to give accounting for the damage threshold
    Float factored_damage = damage_factor * damage_amount;
    Float available_damage_threshold = m_damage_dissipation_rate - m_dissipated_damage_accumulator;
    Float damage_threshold_to_use = Min(factored_damage, available_damage_threshold);
    ASSERT1(damage_threshold_to_use >= -0.001f)
    Float adjusted_damage_amount = factored_damage - damage_threshold_to_use;
    ASSERT1(adjusted_damage_amount >= 0.0f)

    // update the damage threshold accumulator
    m_dissipated_damage_accumulator += damage_threshold_to_use;
    ASSERT1(m_dissipated_damage_accumulator <= m_damage_dissipation_rate + 0.001f)

    // record the amount of damage actually used by this Mortal
    if (damage_amount_used != NULL)
    {
        *damage_amount_used = Min(m_current_health + damage_threshold_to_use, factored_damage) / damage_factor;
        ASSERT1(*damage_amount_used <= damage_amount)
    }
    
    // if the adjusted damage amount is below zero, return (no damage should be done)
    if (adjusted_damage_amount <= 0.0f)
        return false;

    // TODO: debris spawning
        
    // calculate the new health, and call Die on this if it went below zero.
    Float new_health = m_current_health - adjusted_damage_amount;
    bool mortal_died_from_this_damage = new_health <= 0.0f && m_current_health > 0.0f;
    SetCurrentHealth(new_health);
    if (mortal_died_from_this_damage)
    {
        Die(damager,
            damage_medium,
            damage_location,
            damage_normal,
            damage_force,
            damage_type,
            time,
            frame_dt);
        return true;
    }
    else
        return false;
}

void Mortal::Heal (
    Entity *const healer,
    Entity *const heal_medium,
    Float const heal_amount,
    FloatVector2 const &heal_location,
    FloatVector2 const &heal_normal,  
    Float const heal_force,           
    Float const time,
    Float const frame_dt)
{
    ASSERT1(heal_amount >= 0.0f)

    // can only heal a live Mortal
    if (m_current_health > 0.0f)
        SetCurrentHealth(Min(m_current_health + heal_amount, m_max_health));
}

} // end of namespace Dis
