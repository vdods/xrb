// ///////////////////////////////////////////////////////////////////////////
// dis_explosive.hpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_EXPLOSIVE_HPP_)
#define _DIS_EXPLOSIVE_HPP_

#include "dis_mortal.hpp"

#include "dis_linetracebinding.hpp"

using namespace Xrb;

namespace Dis
{

// class EMPBombLayer;
class GrenadeLauncher;
class PhysicsHandler;
class Ship;

class Explosive : public Mortal
{
public:

    Explosive (
        Uint8 const weapon_level,
        EntityReference<Entity> const &owner,
        Float const current_health,
        Float const max_health,
        EntityType const entity_type,
        CollisionType const collision_type)
        :
        Mortal(current_health, max_health, entity_type, collision_type),
        m_owner(owner),
        m_weapon_level(weapon_level)
    {
        ASSERT1(m_weapon_level < UPGRADE_LEVEL_COUNT);
        SetWeakness(D_BALLISTIC);
        SetDamageDissipationRate(1.0f);
        m_has_detonated = false;
    }
    virtual ~Explosive () { }

    inline Uint8 GetWeaponLevel () const { return m_weapon_level; }
    inline bool GetHasDetonated () const { return m_has_detonated; }
    virtual bool IsExplosive () const { return true; }

    virtual void Collide (
        Entity *collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
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
        Float frame_dt);

    virtual bool CheckIfItShouldDetonate (
        Entity *collider,
        Float time,
        Float frame_dt) = 0;
    virtual void Detonate (
        Float time,
        Float frame_dt);

protected:

    EntityReference<Entity> m_owner;

private:

    Uint8 const m_weapon_level;
    bool m_has_detonated;
}; // end of class Explosive

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class Grenade : public Explosive
{
public:

    static Float const ms_default_mass;
    static Float const ms_merge_power_boost;

    Grenade (
        GrenadeLauncher *owner_grenade_launcher,
        Float damage_to_inflict,
        Float damage_radius,
        Float explosion_radius,
        Uint32 weapon_level,
        EntityReference<Entity> const &owner,
        Float max_health);
    virtual ~Grenade () { }

    inline GrenadeLauncher *GetOwnerGrenadeLauncher ()
    {
        return m_owner_grenade_launcher;
    }

    inline void SetOwnerGrenadeLauncher (GrenadeLauncher *const owner_grenade_launcher)
    {
        m_owner_grenade_launcher = owner_grenade_launcher;
    }

    virtual void Collide (
        Entity *collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
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
        Float frame_dt);

    virtual bool CheckIfItShouldDetonate (
        Entity *collider,
        Float time,
        Float frame_dt);
    virtual void Detonate (
        Float time,
        Float frame_dt);

private:

    GrenadeLauncher *m_owner_grenade_launcher;
    Float m_damage_to_inflict;
    Float m_damage_radius;
    Float m_explosion_radius;
}; // end of class Grenade

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class Missile : public Explosive
{
public:

    Missile (
        Float const time_to_live,
        Float const time_at_birth,
        Float const damage_to_inflict,
        Float const damage_radius,
        Float const explosion_radius,
        Uint32 const weapon_level,
        EntityReference<Entity> const &owner,
        Float const max_health,
        EntityType const entity_type = ET_MISSILE)
        :
        Explosive(weapon_level, owner, max_health, max_health, entity_type, CT_SOLID_COLLISION),
        m_time_to_live(time_to_live),
        m_time_at_birth(time_at_birth),
        m_damage_to_inflict(damage_to_inflict),
        m_damage_radius(damage_radius),
        m_explosion_radius(explosion_radius)
    {
        ASSERT1(m_time_to_live > 0.0f);
        ASSERT1(m_damage_to_inflict > 0.0f);
        ASSERT1(m_explosion_radius > 0.0f);
        ASSERT1(entity_type == ET_MISSILE ||
                entity_type == ET_GUIDED_MISSILE ||
                entity_type == ET_ENEMY_MISSILE ||
                entity_type == ET_GUIDED_ENEMY_MISSILE);
        m_first_think = true;
        SetImmunity(D_COLLISION|D_EXPLOSION);
    }
    virtual ~Missile () { }

    virtual void Think (Float time, Float frame_dt);

    virtual bool CheckIfItShouldDetonate (
        Entity *collider,
        Float time,
        Float frame_dt);
    virtual void Detonate (
        Float time,
        Float frame_dt);

protected:

    static Float const ms_acceleration[UPGRADE_LEVEL_COUNT];

    Float m_time_to_live;

private:

    Float const m_time_at_birth;
    Float const m_damage_to_inflict;
    Float const m_damage_radius;
    Float const m_explosion_radius;
    bool m_first_think;
    FloatVector2 m_initial_velocity;
}; // end of class Missile

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class GuidedMissile : public Missile
{
public:

    GuidedMissile (
        Float const time_to_live,
        Float const time_at_birth,
        Float const damage_to_inflict,
        Float const damage_radius,
        Float const explosion_radius,
        Uint32 const weapon_level,
        EntityReference<Entity> const &owner,
        Float const max_health,
        EntityType const entity_type = ET_GUIDED_MISSILE)
        :
        Missile(
            time_to_live,
            time_at_birth,
            damage_to_inflict,
            damage_radius,
            explosion_radius,
            weapon_level,
            owner,
            max_health,
            entity_type)
    {
        ASSERT1(entity_type == ET_GUIDED_MISSILE ||
                entity_type == ET_GUIDED_ENEMY_MISSILE);
        m_next_search_time = -1.0f;
    }
    virtual ~GuidedMissile () { }

    virtual void Think (Float time, Float frame_dt);

protected:

    virtual EntityReference<Ship> FindTarget (LineTraceBindingSet const &scan_set);

private:

    void Search (Float time, Float frame_dt);
    void Seek (Float time, Float frame_dt);

    void AimAt (FloatVector2 const &position);

    Float m_next_search_time;
    EntityReference<Ship> m_target;
}; // end of class GuidedMissile

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class GuidedEnemyMissile : public GuidedMissile
{
public:

    GuidedEnemyMissile (
        Float const time_to_live,
        Float const time_at_birth,
        Float const damage_to_inflict,
        Float const damage_radius,
        Float const explosion_radius,
        Uint32 const weapon_level,
        EntityReference<Entity> const &owner,
        Float const max_health)
        :
        GuidedMissile(
            time_to_live,
            time_at_birth,
            damage_to_inflict,
            damage_radius,
            explosion_radius,
            weapon_level,
            owner,
            max_health,
            ET_GUIDED_ENEMY_MISSILE)
    { }
    virtual ~GuidedEnemyMissile () { }

protected:

    virtual EntityReference<Ship> FindTarget (LineTraceBindingSet const &scan_set);
}; // end of class GuidedMissile

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////
/*

class EMPBomb : public Explosive
{
public:

    EMPBomb (
        EMPBombLayer *const owner_emp_bomb_launcher,
        Float const disable_time_factor,
        Float const blast_radius,
        Uint32 const weapon_level,
        EntityReference<Entity> const &owner,
        Float const max_health)
        :
        Explosive(owner, max_health, max_health, ET_EMP_BOMB, CT_SOLID_COLLISION),
        m_disable_time_factor(disable_time_factor),
        m_blast_radius(blast_radius),
        m_weapon_level(weapon_level)
    {
        ASSERT1(m_disable_time_factor > 1.0f);
        ASSERT1(m_blast_radius > 0.0f);
        m_owner_emp_bomb_launcher = owner_emp_bomb_launcher;
        SetImmunity(D_COLLISION);
    }
    virtual ~EMPBomb () { }

    inline EMPBombLayer *GetOwnerEMPBombLayer ()
    {
        return m_owner_emp_bomb_launcher;
    }

    inline void SetOwnerEMPBombLayer (EMPBombLayer *const owner_emp_bomb_launcher)
    {
        m_owner_emp_bomb_launcher = owner_emp_bomb_launcher;
    }

    virtual void Collide (
        Entity *collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
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
        Float frame_dt);

    virtual bool CheckIfItShouldDetonate (
        Entity *collider,
        Float time,
        Float frame_dt);
    virtual void Detonate (
        Float time,
        Float frame_dt);

private:

    EMPBombLayer *m_owner_emp_bomb_launcher;
    Float const m_disable_time_factor;
    Float const m_blast_radius;
    Uint32 m_weapon_level;
}; // end of class EMPBomb
*/
} // end of namespace Dis

#endif // !defined(_DIS_EXPLOSIVE_HPP_)

