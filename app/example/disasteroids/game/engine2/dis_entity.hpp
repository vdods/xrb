// ///////////////////////////////////////////////////////////////////////////
// dis_entity.hpp by Victor Dods, created 2005/11/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_ENTITY_HPP_)
#define _DIS_ENTITY_HPP_

#include "xrb_engine2_circle_entity.hpp"

#include "dis_enums.hpp"
#include "dis_entityreference.hpp"

using namespace Xrb;

namespace Dis
{

class Entity;
class PhysicsHandler;
class World;

// baseclass for all disasteroids game object classes
class Entity : public Engine2::Circle::Entity
{
public:

    Entity (EntityType entity_type, Engine2::Circle::CollisionType collision_type);
    virtual ~Entity ();

    // ///////////////////////////////////////////////////////////////////////
    // public accessors
    // ///////////////////////////////////////////////////////////////////////

    EntityType GetEntityType () const { return m_entity_type; }
    EntityReference<Entity> const &GetReference ()
    {
        if (!m_reference.IsValid())
        {
            m_reference.SetInstance(new EntityInstance(this));
            ASSERT1(m_reference.IsValid());
            ASSERT1(*m_reference == this);
        }
        return m_reference;
    }

    Dis::World *GetWorld () const;

    static bool CollisionExemption (
        Engine2::Circle::Entity const *entity0,
        Engine2::Circle::Entity const *entity1);
    static Float MaxEntitySpeed (Engine2::Circle::Entity const *entity);

    // ///////////////////////////////////////////////////////////////////////
    // public interface methods
    // ///////////////////////////////////////////////////////////////////////

    // define our own interface for Collide, since we don't want to have to
    // manually cast to Dis::Entity each time.
    virtual void Collide (
        Entity *collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
        Float time,
        Float frame_dt)
    { }

    virtual bool IsMortal () const { return false; }
    virtual bool IsShip () const { return false; }
    virtual bool IsPlayerShip () const { return false; }
    virtual bool IsEnemyShip () const { return false; }
    virtual bool IsExplosive () const { return false; }
    virtual bool IsPowerup () const { return false; }
    virtual bool IsEffect () const { return false; }
    virtual bool IsBallistic () const { return false; }

private:

    // this is an Engine2::Circle::Entity interface method, which we're
    // overriding in order to perform the cast to Dis::Entity.
    virtual void Collide_ (
        Engine2::Circle::Entity *collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
        Float time,
        Float frame_dt)
    {
        Collide(
            DStaticCast<Dis::Entity *>(collider),
            collision_location,
            collision_normal,
            collision_force,
            time,
            frame_dt);
    }

    EntityType const m_entity_type;
    EntityReference<Entity> m_reference;
}; // end of class Entity

} // end of namespace Dis

#endif // !defined(_DIS_ENTITY_HPP_)

