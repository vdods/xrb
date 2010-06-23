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

World *Entity::GetWorld () const
{
    return DStaticCast<World *>(OwnerObject()->GetWorld());
}

} // end of namespace Dis
