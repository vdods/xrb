// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_entity.cpp by Victor Dods, created 2004/07/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_entity.h"

#include "xrb_engine2_entityguts.h"
#include "xrb_engine2_events.h"
#include "xrb_engine2_objectlayer.h"
#include "xrb_engine2_physicshandler.h"
#include "xrb_engine2_quadtree.h"
#include "xrb_engine2_world.h"
#include "xrb_render.h"
#include "xrb_serializer.h"

namespace Xrb
{

Engine2::Entity::~Entity ()
{
    ASSERT1(!GetIsInWorld())
    if (m_entity_guts != NULL)
    {
        ASSERT1(m_entity_guts->m_owner_entity == this)
        m_entity_guts->m_owner_entity = NULL;
    }
    Delete(m_entity_guts);
}

void Engine2::Entity::Write (Serializer &serializer) const
{
    WriteSubType(serializer);
    
    // call WriteClassSpecific for this and all superclasses
    Object::WriteClassSpecific(serializer);
    Entity::WriteClassSpecific(serializer);
}

Engine2::PhysicsHandler *Engine2::Entity::GetPhysicsHandler ()
{
    ASSERT1(GetIsInWorld())
    ASSERT1(GetWorld() != NULL)
    return GetWorld()->GetPhysicsHandler();
}

void Engine2::Entity::SetEntityGuts (EntityGuts *const entity_guts)
{
    ASSERT1(
        (entity_guts == NULL || m_entity_guts == NULL) &&
        "You must set the guts to NULL before supplying new guts")
    if (m_entity_guts != NULL)
    {
        ASSERT1(m_entity_guts->m_owner_entity == this)
        m_entity_guts->m_owner_entity = NULL;
    }
    if (entity_guts != NULL)
    {
        ASSERT1(entity_guts->m_owner_entity == NULL)
        entity_guts->m_owner_entity = this;
        entity_guts->HandleNewOwnerEntity();
    }
    m_entity_guts = entity_guts;
}

void Engine2::Entity::SetAppliesGravity (bool const applies_gravity)
{
    if (m_applies_gravity != applies_gravity)
    {
        m_applies_gravity = applies_gravity;
        if (GetIsInWorld())
        {
            ASSERT1(GetWorld() != NULL)
            GetWorld()->
                GetPhysicsHandler()->
                    HandleChangedEntityAppliesGravity(
                        this,
                        !m_applies_gravity,
                        m_applies_gravity);
        }
    }
}

void Engine2::Entity::SetReactsToGravity (bool const reacts_to_gravity)
{
    if (m_reacts_to_gravity != reacts_to_gravity)
    {
        m_reacts_to_gravity = reacts_to_gravity;
        if (GetIsInWorld())
        {
            ASSERT1(GetWorld() != NULL)
            GetWorld()->
                GetPhysicsHandler()->
                    HandleChangedEntityReactsToGravity(
                        this,
                        !m_reacts_to_gravity,
                        m_reacts_to_gravity);
        }
    }
}

void Engine2::Entity::SetCollisionType (CollisionType const collision_type)
{
    if (m_collision_type != collision_type)
    {
        CollisionType old_collision_type = m_collision_type;
        m_collision_type = collision_type;
        if (GetIsInWorld())
        {
            ASSERT1(GetWorld() != NULL)
            GetWorld()->
                GetPhysicsHandler()->
                    HandleChangedEntityCollisionType(
                        this,
                        old_collision_type,
                        m_collision_type);
        }
    }
}

void Engine2::Entity::SetScaleFactors (FloatVector2 const &scale_factors)
{
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factors[Dim::X]))
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factors[Dim::Y]))
    Float old_radius = GetRadius();
    Object::SetScaleFactors(scale_factors);
    Float new_radius = GetRadius();
    if (new_radius != old_radius)
        HandleChangedRadius(old_radius, new_radius);
}

void Engine2::Entity::SetScaleFactors (Float const r, Float const s)
{
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(r))
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(s))
    Float old_radius = GetRadius();
    Object::SetScaleFactors(r, s);
    Float new_radius = GetRadius();
    if (new_radius != old_radius)
        HandleChangedRadius(old_radius, new_radius);
}

void Engine2::Entity::SetScaleFactor (Float const scale_factor)
{
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factor))
    Float old_radius = GetRadius();
    Object::SetScaleFactor(scale_factor);
    Float new_radius = GetRadius();
    if (new_radius != old_radius)
        HandleChangedRadius(old_radius, new_radius);
}

void Engine2::Entity::Scale (FloatVector2 const &scale_factors)
{
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factors[Dim::X]))
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factors[Dim::Y]))
    Float old_radius = GetRadius();
    Object::Scale(scale_factors);
    Float new_radius = GetRadius();
    if (new_radius != old_radius)
        HandleChangedRadius(old_radius, new_radius);
}

void Engine2::Entity::Scale (Float const r, Float const s)
{
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(r))
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(s))
    Float old_radius = GetRadius();
    Object::Scale(r, s);
    Float new_radius = GetRadius();
    if (new_radius != old_radius)
        HandleChangedRadius(old_radius, new_radius);
}

void Engine2::Entity::Scale (Float const scale_factor)
{
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factor))
    Float old_radius = GetRadius();
    Object::Scale(scale_factor);
    Float new_radius = GetRadius();
    if (new_radius != old_radius)
        HandleChangedRadius(old_radius, new_radius);
}

void Engine2::Entity::ResetScale ()
{
    Float old_radius = GetRadius();
    Object::ResetScale();
    Float new_radius = GetRadius();
    if (new_radius != old_radius)
        HandleChangedRadius(old_radius, new_radius);
}

void Engine2::Entity::ReAddToQuadTree (
    Engine2::QuadTreeType const quad_tree_type)
{
    if (GetOwnerQuadTree(quad_tree_type) != NULL)
    {
        GetObjectLayer()->HandleContainmentOrWrapping(this);
        GetOwnerQuadTree(quad_tree_type)->ReAddObject(this);
    }
}

void Engine2::Entity::RemoveFromWorld ()
{
    ASSERT1(GetIsInWorld())
    ASSERT1(GetWorld() != NULL)
    ASSERT1(GetObjectLayer() != NULL)
    GetWorld()->RemoveEntity(this);
}

void Engine2::Entity::AddBackIntoWorld ()
{
    ASSERT1(!GetIsInWorld())
    ASSERT1(GetWorld() != NULL)
    ASSERT1(GetObjectLayer() != NULL)
    GetWorld()->AddEntity(this, GetObjectLayer());
}

void Engine2::Entity::ScheduleForRemovalFromWorld (Float time_delay)
{
    ASSERT1(GetIsInWorld())
    ASSERT1(GetWorld() != NULL)
    ASSERT1(GetObjectLayer() != NULL)

    if (time_delay < 0.0f)
        time_delay = 0.0f;

    GetWorld()->EnqueueEvent(
        new EventEntity(
            this,
            GetWorld()->GetMostRecentFrameTime() + time_delay,
            Event::ENGINE2_REMOVE_ENTITY_FROM_WORLD));
}

void Engine2::Entity::ScheduleForDeletion (Float time_delay)
{
    ASSERT1(GetIsInWorld())

    if (time_delay < 0.0f)
        time_delay = 0.0f;

    GetWorld()->EnqueueEvent(
        new EventEntity(
            this,
            GetWorld()->GetMostRecentFrameTime() + time_delay,
            Event::ENGINE2_DELETE_ENTITY));
}

Engine2::Entity::Entity ()
    :
    Engine2::Object()
{
    m_sub_type = Object::ST_ENTITY;

    m_entity_guts = NULL;
    m_number = -1;
    m_name = "";
    m_wrapped_offset = FloatVector2::ms_zero;

    // default values
    m_elasticity = 1.0;
    m_first_moment = 1.0;
    m_velocity = FloatVector2::ms_zero;
    m_force = FloatVector2::ms_zero;
    m_second_moment = 1.0;
    m_angular_velocity = 0.0;
    m_torque = 0.0;
    m_applies_gravity = false;
    m_reacts_to_gravity = true;
    m_collision_type = CT_NO_COLLISION;
}

void Engine2::Entity::ReadClassSpecific (Serializer &serializer)
{
    // read in the guts
    m_name = serializer.ReadStdString();
    serializer.ReadFloat(&m_elasticity);
    serializer.ReadFloat(&m_first_moment);
    m_velocity = serializer.ReadFloatVector2();
    serializer.ReadFloat(&m_second_moment);
    serializer.ReadFloat(&m_angular_velocity);
    m_applies_gravity = serializer.ReadBool();
    m_reacts_to_gravity = serializer.ReadBool();

    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_elasticity))
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_first_moment))
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_velocity[Dim::X]))
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_velocity[Dim::Y]))
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_second_moment))
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_angular_velocity))
}

void Engine2::Entity::WriteClassSpecific (Serializer &serializer) const
{
    // write out the guts
    serializer.WriteStdString(m_name);
    serializer.WriteFloat(m_elasticity);
    serializer.WriteFloat(m_first_moment);
    serializer.WriteFloatVector2(m_velocity);
    serializer.WriteFloat(m_second_moment);
    serializer.WriteFloat(m_angular_velocity);
    serializer.WriteBool(m_applies_gravity);
    serializer.WriteBool(m_reacts_to_gravity);

    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_elasticity))
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_first_moment))
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_velocity[Dim::X]))
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_velocity[Dim::Y]))
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_second_moment))
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_angular_velocity))
}

void Engine2::Entity::CloneProperties (Engine2::Object const *const object)
{
    Entity const *entity = dynamic_cast<Entity const *>(object);
    ASSERT1(entity != NULL)

    m_elasticity = entity->GetElasticity();
    m_first_moment = entity->GetFirstMoment();
    m_velocity = entity->GetVelocity();
    m_second_moment = entity->GetSecondMoment();
    m_angular_velocity = entity->GetAngularVelocity();
    m_applies_gravity = entity->GetAppliesGravity();
    m_reacts_to_gravity = entity->GetReactsToGravity();

    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_elasticity))
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_first_moment))
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_velocity[Dim::X]))
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_velocity[Dim::Y]))
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_second_moment))
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(m_angular_velocity))
}

void Engine2::Entity::HandleChangedRadius (
    Float const old_radius,
    Float const new_radius)
{
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(new_radius))
    if (new_radius != old_radius)
    {
        if (GetIsInWorld())
        {
            ReAddToQuadTree(QTT_VISIBILITY);
            ASSERT1(GetWorld() != NULL)
            ASSERT1(GetWorld()->GetPhysicsHandler() != NULL)
            GetWorld()->
                GetPhysicsHandler()->
                    HandleChangedEntityRadius(
                        this,
                        old_radius,
                        new_radius);
        }
    }
}

} // end of namespace Xrb
