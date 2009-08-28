// ///////////////////////////////////////////////////////////////////////////
// dis_entity.cpp by Victor Dods, created 2005/11/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_entity.hpp"

#include "dis_world.hpp"

using namespace Xrb;

namespace Dis
{

Entity::Entity (EntityType entity_type, Engine2::Circle::CollisionType collision_type)
    :
    Engine2::Circle::Entity(collision_type),
    m_entity_type(entity_type)
{
    ASSERT1(m_entity_type < ET_COUNT);
}

Entity::~Entity ()
{
    if (m_reference.IsValid())
        m_reference.NullifyEntity();
}

Dis::World *Entity::GetWorld () const
{
    return DStaticCast<Dis::World *>(OwnerObject()->GetWorld());
}

bool Entity::CollisionExemption (
    Engine2::Circle::Entity const *entity0,
    Engine2::Circle::Entity const *entity1)
{
    Dis::Entity const *dis_entity0 = DStaticCast<Entity const *>(entity0);
    Dis::Entity const *dis_entity1 = DStaticCast<Entity const *>(entity1);

    // we don't want the powerups to hurt the player by collision, and
    // when ballistics (e.g. pea shooter) are allowed to interact with
    // powerups, it just looks dumb.

    if (dis_entity1->IsPowerup() &&
        (dis_entity0->IsPlayerShip() ||
         dis_entity0->GetEntityType() == ET_BALLISTIC))
        return true;

    if (dis_entity0->IsPowerup() &&
        (dis_entity1->IsPlayerShip() ||
         dis_entity1->GetEntityType() == ET_BALLISTIC))
        return true;

    return false;
}

Float Entity::MaxEntitySpeed (Engine2::Circle::Entity const *entity)
{
    Dis::Entity const *dis_entity = DStaticCast<Entity const *>(entity);

    // nonsolid and no-collision
    if (dis_entity->GetCollisionType() != Engine2::Circle::CT_SOLID_COLLISION)
        return 800.0f;

    // projectiles
    if (dis_entity->GetEntityType() >= ET_PROJECTILE_LOWEST && dis_entity->GetEntityType() <= ET_PROJECTILE_HIGHEST)
        return 800.0f;

    // everything else (solid non-projectiles)
    return 350.0f;
}

} // end of namespace Dis
