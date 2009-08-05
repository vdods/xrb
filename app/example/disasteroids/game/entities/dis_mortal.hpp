// ///////////////////////////////////////////////////////////////////////////
// dis_mortal.hpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_MORTAL_HPP_)
#define _DIS_MORTAL_HPP_

#include "dis_entity.hpp"

using namespace Xrb;

namespace Dis
{

class Mortal : public Entity
{
public:

    typedef Uint8 DamageType;

    enum
    {
        D_NONE         = 0x00,

        D_BALLISTIC    = 0x01,
        D_MINING_LASER = 0x02,
        D_COMBAT_LASER = 0x04,
        D_FIRE         = 0x08,
        D_EXPLOSION    = 0x10,
        D_EMP          = 0x20,
        D_COLLISION    = 0x40,
        D_GRINDING     = 0x80,

        D_ALL          = D_BALLISTIC|D_MINING_LASER|D_COMBAT_LASER|D_FIRE|D_EXPLOSION|D_EMP|D_COLLISION|D_GRINDING
    }; // end of enum

    Mortal (
        Float const current_health,
        Float const max_health,
        EntityType const entity_type,
        CollisionType const collision_type)
        :
        Entity(entity_type, collision_type)
    {
        ASSERT1(current_health > 0.0f);
        ASSERT1(max_health > 0.0f);

        m_current_health = current_health;
        m_max_health = max_health;
        m_is_invincible = false;
        m_recent_change_in_health = 0.0f;
        // 1/5 of fatal damage (or opposite in healing) will produce full
        // damage/healing flashing by default.
        m_full_flash_intensity_health = 0.2f * m_max_health;
        m_damage_dissipation_rate = 0.0f;
        m_dissipated_damage_accumulator = 0.0f;
        m_time_last_damaged = -1.0f;
        m_weakness = D_NONE;
        m_strength = D_NONE;
        m_immunity = D_NONE;
    }
    virtual ~Mortal () { }

    inline Float CurrentHealth () const { return m_current_health; }
    inline Float GetMaxHealth () const { return m_max_health; }
    inline bool IsDead () const { return m_current_health <= 0.0f; }
    inline bool IsInvincible () const { return m_is_invincible; }
    inline Float GetDamageDissipationRate () const { return m_damage_dissipation_rate; }
    inline DamageType GetWeakness () const { return m_weakness; }
    inline DamageType GetStrength () const { return m_strength; }
    inline DamageType GetImmunity () const { return m_immunity; }
    inline bool IsWeakAgainst (DamageType const damage_type) const
    {
        // make sure only one bit in the damage type is set
        ASSERT1((damage_type & (damage_type - 1)) == 0);
        return (damage_type & m_weakness) != 0;
    }
    inline bool IsStrongAgainst (DamageType const damage_type) const
    {
        // make sure only one bit in the damage type is set
        ASSERT1((damage_type & (damage_type - 1)) == 0);
        return (damage_type & m_strength) != 0;
    }
    inline bool IsImmuneAgainst (DamageType const damage_type) const
    {
        // make sure only one bit in the damage type is set
        ASSERT1((damage_type & (damage_type - 1)) == 0);
        return (damage_type & m_immunity) != 0;
    }
    virtual bool IsMortal () const { return true; }
    Sint32 GetTargetPriority () const;

    inline void SetIsInvincible (bool is_invincible) { m_is_invincible = is_invincible; }
    void SetDamageDissipationRate (Float damage_dissipation_rate);
    void SetWeakness (DamageType weakness);
    void SetStrength (DamageType strength);
    void SetImmunity (DamageType immunity);

    void AddWeakness (DamageType weakness);
    void AddStrength (DamageType strength);
    void AddImmunity (DamageType immunity);

    void RemoveWeakness (DamageType weakness);
    void RemoveStrength (DamageType strength);
    void RemoveImmunity (DamageType immunity);

    void Revive (Float time, Float frame_dt);
    void Kill (
        Entity *killer,
        Entity *kill_medium,
        FloatVector2 const &kill_location,
        FloatVector2 const &kill_normal,
        Float kill_force,
        DamageType kill_type,
        Float time,
        Float frame_dt);
    // the return value is true iff the Mortal died due to this damage.
    virtual bool Damage (
        Entity *damager,
        Entity *damage_medium,
        Float damage_amount,
        Float *damage_amount_used,
        FloatVector2 const &damage_location,
        FloatVector2 const &damage_normal,
        Float damage_force,
        DamageType damage_type,
        Float time,
        Float frame_dt);
    virtual void Heal (
        Entity *healer,
        Entity *heal_medium,
        Float heal_amount,
        FloatVector2 const &heal_location,
        FloatVector2 const &heal_normal,
        Float heal_force,
        Float time,
        Float frame_dt);
    virtual void Die (
        Entity *killer,
        Entity *kill_medium,
        FloatVector2 const &kill_location,
        FloatVector2 const &kill_normal,
        Float kill_force,
        DamageType kill_type,
        Float time,
        Float frame_dt)
    { }

    virtual void Think (Float time, Float frame_dt);
    virtual void Collide (
        Entity *collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
        Float time,
        Float frame_dt);

protected:

    virtual void SetCurrentHealth (Float current_health)
    {
        // add the health delta to m_recent_change_in_health
        m_recent_change_in_health += current_health - m_current_health;
        // set the current health
        m_current_health = current_health;
        // can't go above max health
        m_current_health = Min(m_current_health, m_max_health);
    }
    virtual void SetMaxHealth (Float max_health)
    {
        ASSERT1(max_health > 0.0f);
        // save off the full flash intensity health ratio for later updating
        Float factor_of_max_health = m_full_flash_intensity_health / m_max_health;
        // set the max health
        m_max_health = max_health;
        // update m_full_flash_intensity_health
        m_full_flash_intensity_health = factor_of_max_health * m_max_health;
        // current health can't go above max health
        m_current_health = Min(m_current_health, m_max_health);
    }
    void ResetRecentChangeInHealth ()
    {
        m_recent_change_in_health = 0.0f;
    }
    void SetFullFlashIntensityHealthRatio (Float factor_of_max_health)
    {
        ASSERT1(factor_of_max_health >= 0.0f && factor_of_max_health <= 1.0f);
        m_full_flash_intensity_health = factor_of_max_health * m_max_health;
    }

private:

    Float m_current_health;
    Float m_max_health;
    bool m_is_invincible;
    Float m_recent_change_in_health;
    Float m_full_flash_intensity_health;
    Float m_damage_dissipation_rate;
    Float m_dissipated_damage_accumulator;
    Float m_time_last_damaged;
    DamageType m_weakness;
    DamageType m_strength;
    DamageType m_immunity;
}; // end of class Mortal

} // end of namespace Dis

#endif // !defined(_DIS_MORTAL_HPP_)

