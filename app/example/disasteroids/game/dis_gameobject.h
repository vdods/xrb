// ///////////////////////////////////////////////////////////////////////////
// dis_gameobject.h by Victor Dods, created 2005/11/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_GAMEOBJECT_H_)
#define _DIS_GAMEOBJECT_H_

#include "xrb_engine2_entityguts.h"

#include "dis_gameobjectreference.h"
#include "xrb_polynomial.h"

using namespace Xrb;

namespace Dis
{

// ///////////////////////////////////////////////////////////////////////////
// 
// ///////////////////////////////////////////////////////////////////////////

// abstract baseclass for all disasteroids game object classes
class GameObject : public Engine2::EntityGuts
{
public:

    enum Type
    {
        T_ASTEROID = 0,
        T_SOLITARY,
        T_INTERLOPER,
        T_SHADE,
        T_REVULSION,
        T_DEVOURMENT,
        T_DEMI,
        T_GRENADE,
        T_MINE,
        T_MISSILE,
        T_EMP_BOMB,
        T_POWERUP,
        T_BALLISTIC,
        T_FIREBALL,
        T_DAMAGE_EXPLOSION,
        T_NO_DAMAGE_EXPLOSION,
        T_EMP_EXPLOSION,
        T_LASER_BEAM,
        T_GAUSS_GUN_TRAIL,
        T_TRACTOR_BEAM,
        T_SHIELD_EFFECT,
        T_RETICLE_EFFECT,
        T_HEALTH_TRIGGER,

        T_COUNT,

        T_ENEMY_SHIP_LOWEST = T_INTERLOPER,
        T_ENEMY_SHIP_HIGHEST = T_DEVOURMENT,
        T_ENEMY_SHIP_COUNT = T_ENEMY_SHIP_HIGHEST - T_ENEMY_SHIP_LOWEST + 1
    }; // end of enum Dis::GameObject::Type

    GameObject (Type const type)
        :
        Engine2::EntityGuts(),
        m_type(type)
    {
        ASSERT1(m_type < T_COUNT)
        m_next_time_to_think = 0.0f;
    }
    virtual ~GameObject ()
    {
        if (m_reference.GetIsValid())
            m_reference.NullifyGameObject();
    }

    inline Type GetType () const
    {
        return m_type;
    }
    inline GameObjectReference<GameObject> const &GetReference ()
    {
        if (!m_reference.GetIsValid())
        {
            m_reference.SetInstance(new GameObjectInstance(this));
            ASSERT1(m_reference.GetIsValid())
        }
        return m_reference;
    }
    virtual bool GetIsMortal () const { return false; }
    virtual bool GetIsShip () const { return false; }
    virtual bool GetIsPlayerShip () const { return false; }
    virtual bool GetIsEnemyShip () const { return false; }
    virtual bool GetIsExplosive () const { return false; }
    virtual bool GetIsPowerup () const { return false; }
    virtual bool GetIsEffect () const { return false; }
    
    // this is the Engine2::EntityGuts::ReadFunction to create
    // Dis::GameObject subclass objects from a serializer.
    static Engine2::EntityGuts *Create (Serializer &serializer);    
    virtual void Write (Serializer &serializer) const { }//= 0;

    inline Float GetNextTimeToThink () const
    {
        return m_next_time_to_think;
    }
    FloatVector2 GetAmbientVelocity (
        Float scan_area_radius,
        GameObject const *ignore_me) const;
    static inline bool GetShouldApplyCollisionForces (
        GameObject const *game_object_1,
        GameObject const *game_object_2)
    {
        if (game_object_1 == NULL || game_object_2 == NULL)
            return true;

        if (game_object_1->GetIsPlayerShip() && game_object_2->GetIsPowerup())
            return false;
            
        if (game_object_2->GetIsPlayerShip() && game_object_1->GetIsPowerup())
            return false;

        return true;
    }
    
    inline void SetNextTimeToThink (Float next_time_to_think)
    {
        m_next_time_to_think = next_time_to_think;
    }

    inline void ApplyInterceptCourseAcceleration (
        GameObject *target,
        Float maximum_thrust_force,
        bool apply_force_on_target_also)
    {
        Polynomial::SolutionSet solution_set;
        ApplyInterceptCourseAcceleration(
            target,
            maximum_thrust_force,
            apply_force_on_target_also,
            &solution_set);
    }
    void ApplyInterceptCourseAcceleration (
        GameObject *target,
        Float maximum_thrust_force,
        bool apply_force_on_target_also,
        Polynomial::SolutionSet *solution_set);

    virtual void Think (Float time, Float frame_dt)
    {
        // if this Think method is not overridden, then don't Think often.
        SetNextTimeToThink(time + 10000.0f);
    }
    // the collision normal value points towards the entity
    virtual void Collide (
        GameObject *collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
        Float time,
        Float frame_dt)
    { }

protected:

    static Type ReadType (Serializer &serializer);
    void WriteType (Serializer &serializer) const;

private:

    Type const m_type;
    Float m_next_time_to_think;
    GameObjectReference<GameObject> m_reference;
}; // end of class GameObject

} // end of namespace Dis

#endif // !defined(_DIS_GAMEOBJECT_H_)

