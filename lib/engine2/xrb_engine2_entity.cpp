// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_entity.cpp by Victor Dods, created 2004/07/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_entity.hpp"

#include "xrb_engine2_animatedsprite.hpp"
#include "xrb_engine2_compound.hpp"
#include "xrb_engine2_events.hpp"
#include "xrb_engine2_objectlayer.hpp"
#include "xrb_engine2_physicshandler.hpp"
#include "xrb_engine2_quadtree.hpp"
#include "xrb_engine2_sprite.hpp"
#include "xrb_engine2_world.hpp"
#include "xrb_serializer.hpp"

namespace Xrb {
namespace Engine2 {

Sprite const *Entity::OwnerSprite () const
{
    if (m_owner_object == NULL)
        return NULL;

    ASSERT1(m_owner_object->GetObjectType() == OT_SPRITE);
    return DStaticCast<Sprite const *>(m_owner_object);
}

Sprite *Entity::OwnerSprite ()
{
    if (m_owner_object == NULL)
        return NULL;

    ASSERT1(m_owner_object->GetObjectType() == OT_SPRITE);
    return DStaticCast<Sprite *>(m_owner_object);
}

AnimatedSprite const *Entity::OwnerAnimatedSprite () const
{
    if (m_owner_object == NULL)
        return NULL;

    ASSERT1(m_owner_object->GetObjectType() == OT_ANIMATED_SPRITE);
    return DStaticCast<AnimatedSprite const *>(m_owner_object);
}

AnimatedSprite *Entity::OwnerAnimatedSprite ()
{
    if (m_owner_object == NULL)
        return NULL;

    ASSERT1(m_owner_object->GetObjectType() == OT_ANIMATED_SPRITE);
    return DStaticCast<AnimatedSprite *>(m_owner_object);
}

Compound const *Entity::OwnerCompound () const
{
    if (m_owner_object == NULL)
        return NULL;

    ASSERT1(m_owner_object->GetObjectType() == OT_COMPOUND);
    return DStaticCast<Compound const *>(m_owner_object);
}

Compound *Entity::OwnerCompound ()
{
    if (m_owner_object == NULL)
        return NULL;

    ASSERT1(m_owner_object->GetObjectType() == OT_COMPOUND);
    return DStaticCast<Compound *>(m_owner_object);
}

void Entity::SetWrappedOffset (FloatVector2 const &wrapped_offset)
{
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(wrapped_offset[Dim::X]));
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(wrapped_offset[Dim::Y]));
    m_wrapped_offset = wrapped_offset;
}

void Entity::AccumulateWrappedOffset (FloatVector2 const &wrapped_offset_delta)
{
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(wrapped_offset_delta[Dim::X]));
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(wrapped_offset_delta[Dim::Y]));
    m_wrapped_offset += wrapped_offset_delta;
}

void Entity::RemoveFromWorld ()
{
    ASSERT1(IsInWorld());
    ASSERT1(OwnerObject()->GetWorld() != NULL);
    ASSERT1(GetObjectLayer() != NULL);
    OwnerObject()->GetWorld()->RemoveDynamicObject(OwnerObject());
}

void Entity::AddBackIntoWorld ()
{
    ASSERT1(!IsInWorld());
    ASSERT1(OwnerObject()->GetWorld() != NULL);
    ASSERT1(GetObjectLayer() != NULL);
    OwnerObject()->GetWorld()->AddDynamicObject(OwnerObject(), GetObjectLayer());
}

void Entity::ReAddToQuadTree (QuadTreeType const quad_tree_type)
{
    ASSERT1(m_owner_object != NULL);
    if (OwnerQuadTree(quad_tree_type) != NULL)
    {
        GetObjectLayer()->HandleContainmentOrWrapping(m_owner_object);
        OwnerQuadTree(quad_tree_type)->ReAddObject(m_owner_object);
    }
}

void Entity::CloneProperties (Entity const &entity)
{
    ASSERT1(!IsInWorld() && "can't CloneProperties() on an Entity that is in the world");
    m_wrapped_offset = entity.m_wrapped_offset;
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_wrapped_offset[Dim::X]));
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_wrapped_offset[Dim::Y]));
}

void Entity::ScheduleForDeletion (Float time_delay)
{
    ASSERT1(IsInWorld());

    if (time_delay < 0.0f)
        time_delay = 0.0f;

    OwnerObject()->GetWorld()->EnqueueEvent(
        new EventEntity(
            this,
            OwnerObject()->GetWorld()->MostRecentFrameTime() + time_delay,
            Event::ENGINE2_DELETE_ENTITY));
}

void Entity::ScheduleForRemovalFromWorld (Float time_delay)
{
    ASSERT1(IsInWorld());

    if (time_delay < 0.0f)
        time_delay = 0.0f;

    OwnerObject()->GetWorld()->EnqueueEvent(
        new EventEntity(
            this,
            OwnerObject()->GetWorld()->MostRecentFrameTime() + time_delay,
            Event::ENGINE2_REMOVE_ENTITY_FROM_WORLD));
}

void Entity::ReadClassSpecific (Serializer &serializer)
{
    serializer.ReadAggregate<FloatVector2>(m_wrapped_offset);
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_wrapped_offset[Dim::X]));
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_wrapped_offset[Dim::Y]));
}

void Entity::WriteClassSpecific (Serializer &serializer) const
{
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_wrapped_offset[Dim::X]));
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_wrapped_offset[Dim::Y]));
    serializer.WriteAggregate<FloatVector2>(m_wrapped_offset);
}

} // end of namespace Engine2
} // end of namespace Xrb
