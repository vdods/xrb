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

#include "xrb_engine2_compound.hpp"
#include "xrb_engine2_events.hpp"
#include "xrb_engine2_objectlayer.hpp"
#include "xrb_engine2_physicshandler.hpp"
#include "xrb_engine2_quadtree.hpp"
#include "xrb_engine2_sprite.hpp"
#include "xrb_engine2_world.hpp"
#include "xrb_render.hpp"
#include "xrb_serializer.hpp"

namespace Xrb
{

Engine2::Sprite *Engine2::Entity::OwnerSprite () const
{
    if (m_owner_object == NULL)
        return NULL;

    ASSERT1(m_owner_object->GetObjectType() == OT_SPRITE);
    return static_cast<Sprite *>(m_owner_object);
}

Engine2::Compound *Engine2::Entity::OwnerCompound () const
{
    if (m_owner_object == NULL)
        return NULL;

    ASSERT1(m_owner_object->GetObjectType() == OT_COMPOUND);
    return static_cast<Compound *>(m_owner_object);
}

void Engine2::Entity::SetWrappedOffset (FloatVector2 const &wrapped_offset)
{
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(wrapped_offset[Dim::X]));
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(wrapped_offset[Dim::Y]));
    m_wrapped_offset = wrapped_offset;
}

void Engine2::Entity::AccumulateWrappedOffset (FloatVector2 const &wrapped_offset_delta)
{
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(wrapped_offset_delta[Dim::X]));
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(wrapped_offset_delta[Dim::Y]));
    m_wrapped_offset += wrapped_offset_delta;
}

void Engine2::Entity::RemoveFromWorld ()
{
    ASSERT1(IsInWorld());
    ASSERT1(OwnerObject()->GetWorld() != NULL);
    ASSERT1(GetObjectLayer() != NULL);
    OwnerObject()->GetWorld()->RemoveDynamicObject(OwnerObject());
}

void Engine2::Entity::AddBackIntoWorld ()
{
    ASSERT1(!IsInWorld());
    ASSERT1(OwnerObject()->GetWorld() != NULL);
    ASSERT1(GetObjectLayer() != NULL);
    OwnerObject()->GetWorld()->AddDynamicObject(OwnerObject(), GetObjectLayer());
}

void Engine2::Entity::ReAddToQuadTree (QuadTreeType const quad_tree_type)
{
    ASSERT1(m_owner_object != NULL);
    if (OwnerQuadTree(quad_tree_type) != NULL)
    {
        GetObjectLayer()->HandleContainmentOrWrapping(m_owner_object);
        OwnerQuadTree(quad_tree_type)->ReAddObject(m_owner_object);
    }
}

void Engine2::Entity::ScheduleForDeletion (Float time_delay)
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

void Engine2::Entity::ScheduleForRemovalFromWorld (Float time_delay)
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

void Engine2::Entity::CloneProperties (Engine2::Entity const *const entity)
{
    ASSERT1(entity != NULL);
    m_wrapped_offset = entity->m_wrapped_offset;
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_wrapped_offset[Dim::X]));
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_wrapped_offset[Dim::Y]));
}

void Engine2::Entity::ReadClassSpecific (Serializer &serializer)
{
    serializer.ReadFloatVector2(&m_wrapped_offset);
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_wrapped_offset[Dim::X]));
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_wrapped_offset[Dim::Y]));
}

void Engine2::Entity::WriteClassSpecific (Serializer &serializer) const
{
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_wrapped_offset[Dim::X]));
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_wrapped_offset[Dim::Y]));
    serializer.WriteFloatVector2(m_wrapped_offset);
}

} // end of namespace Xrb
