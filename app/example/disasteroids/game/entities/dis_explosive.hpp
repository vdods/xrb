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

using namespace Xrb;

namespace Dis {

class GrenadeLauncher;
class MissileLauncher;
class PhysicsHandler;
class Ship;

class Explosive : public Mortal
{
public:

    Explosive (
        Uint8 weapon_level,
        EntityReference<Entity> const &owner,
        Float current_health,
        Float max_health,
        EntityType entity_type,
        Engine2::Circle::CollisionType collision_type)
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

    Uint8 WeaponLevel () const { return m_weapon_level; }
    bool HasDetonated () const { return m_has_detonated; }
    virtual bool IsExplosive () const { return true; }

    virtual void Collide (
        Entity &collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
        Time time,
        Time::Delta frame_dt);
    virtual void Die (
        Entity *killer,
        Entity *kill_medium,
        FloatVector2 const &kill_location,
        FloatVector2 const &kill_normal,
        Float kill_force,
        DamageType kill_type,
        Time time,
        Time::Delta frame_dt);

    virtual bool CheckIfItShouldDetonate (
        Entity &collider,
        Time time,
        Time::Delta frame_dt) = 0;
    virtual void Detonate (
        Time time,
        Time::Delta frame_dt);

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
    virtual ~Grenade ();

    GrenadeLauncher *OwnerGrenadeLauncher () { return m_owner_grenade_launcher; }

    void SetOwnerGrenadeLauncher (GrenadeLauncher *owner_grenade_launcher)
    {
        m_owner_grenade_launcher = owner_grenade_launcher;
    }

    virtual void Collide (
        Entity &collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
        Time time,
        Time::Delta frame_dt);
    virtual void Die (
        Entity *killer,
        Entity *kill_medium,
        FloatVector2 const &kill_location,
        FloatVector2 const &kill_normal,
        Float kill_force,
        DamageType kill_type,
        Time time,
        Time::Delta frame_dt);

    virtual bool CheckIfItShouldDetonate (
        Entity &collider,
        Time time,
        Time::Delta frame_dt);
    virtual void Detonate (
        Time time,
        Time::Delta frame_dt);

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
        MissileLauncher *owner_missile_launcher,
        Time::Delta time_to_live,
        Time time_at_birth,
        Float damage_to_inflict,
        Float damage_radius,
        Float explosion_radius,
        Uint32 weapon_level,
        EntityReference<Entity> const &owner,
        Float max_health,
        EntityType entity_type = ET_MISSILE);
    virtual ~Missile ();

    MissileLauncher *OwnerMissileLauncher () { return m_owner_missile_launcher; }

    void SetOwnerMissileLauncher (MissileLauncher *owner_missile_launcher)
    {
        m_owner_missile_launcher = owner_missile_launcher;
    }

    virtual void Think (Time time, Time::Delta frame_dt);
    virtual void Die (
        Entity *killer,
        Entity *kill_medium,
        FloatVector2 const &kill_location,
        FloatVector2 const &kill_normal,
        Float kill_force,
        DamageType kill_type,
        Time time,
        Time::Delta frame_dt);

    virtual bool CheckIfItShouldDetonate (
        Entity &collider,
        Time time,
        Time::Delta frame_dt);
    virtual void Detonate (
        Time time,
        Time::Delta frame_dt);

protected:

    static Float const ms_acceleration[UPGRADE_LEVEL_COUNT];

    Time::Delta m_time_to_live;

private:

    MissileLauncher *m_owner_missile_launcher;
    Time const m_time_at_birth;
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
        Time::Delta time_to_live,
        Time time_at_birth,
        Float damage_to_inflict,
        Float damage_radius,
        Float explosion_radius,
        Uint32 weapon_level,
        EntityReference<Entity> const &owner,
        Float max_health,
        EntityType entity_type = ET_GUIDED_MISSILE)
        :
        Missile(
            NULL,
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
        m_next_search_time = Time::ms_negative_infinity;
    }
    virtual ~GuidedMissile () { }

    virtual void Think (Time time, Time::Delta frame_dt);

protected:

    virtual EntityReference<Ship> FindTarget (Engine2::Circle::LineTraceBindingSet const &scan_set);

private:

    void Search (Time time, Time::Delta frame_dt);
    void Seek (Time time, Time::Delta frame_dt);

    void AimAt (FloatVector2 const &position);

    Time m_next_search_time;
    EntityReference<Ship> m_target;
}; // end of class GuidedMissile

// ///////////////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////////////

class GuidedEnemyMissile : public GuidedMissile
{
public:

    GuidedEnemyMissile (
        Time::Delta time_to_live,
        Time time_at_birth,
        Float damage_to_inflict,
        Float damage_radius,
        Float explosion_radius,
        Uint32 weapon_level,
        EntityReference<Entity> const &owner,
        Float max_health)
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

    virtual EntityReference<Ship> FindTarget (Engine2::Circle::LineTraceBindingSet const &scan_set);
}; // end of class GuidedMissile

} // end of namespace Dis

#endif // !defined(_DIS_EXPLOSIVE_HPP_)

