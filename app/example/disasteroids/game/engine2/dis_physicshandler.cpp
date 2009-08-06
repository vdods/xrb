// ///////////////////////////////////////////////////////////////////////////
// dis_physicshandler.cpp by Victor Dods, created 2005/11/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_physicshandler.hpp"

#include "dis_collisionquadtree.hpp"
#include "dis_entity.hpp"
#include "xrb_engine2_objectlayer.hpp"
#include "xrb_engine2_world.hpp"

using namespace Xrb;

namespace Dis
{

PhysicsHandler::PhysicsHandler ()
    :
    Engine2::PhysicsHandler()
{
    m_main_object_layer = NULL;
    m_quad_tree = NULL;
}

PhysicsHandler::~PhysicsHandler ()
{
    m_entity_set.clear();
    m_quad_tree->Clear();
    Delete(m_quad_tree);
}

bool PhysicsHandler::DoesAreaOverlapAnyEntityInObjectLayer (
    Engine2::ObjectLayer const *const object_layer,
    FloatVector2 const &area_center,
    Float const area_radius,
    bool const check_nonsolid_collision_entities) const
{
    ASSERT1(object_layer != NULL);
    ASSERT1(object_layer == m_main_object_layer);
    ASSERT1(m_quad_tree != NULL);
    if (m_main_object_layer->IsWrapped())
        return m_quad_tree->DoesAreaOverlapAnyEntityWrapped(
            area_center,
            area_radius,
            check_nonsolid_collision_entities,
            m_main_object_layer->SideLength(),
            0.5f * m_main_object_layer->SideLength());
    else
        return m_quad_tree->DoesAreaOverlapAnyEntity(
            area_center,
            area_radius,
            check_nonsolid_collision_entities);
}

void PhysicsHandler::LineTrace (
    Engine2::ObjectLayer const *const object_layer,
    FloatVector2 const &trace_start,
    FloatVector2 const &trace_vector,
    Float const trace_radius,
    bool const check_nonsolid_collision_entities,
    LineTraceBindingSet *const line_trace_binding_set) const
{
    ASSERT1(object_layer != NULL);
    ASSERT1(object_layer == m_main_object_layer);
    ASSERT1(trace_radius >= 0.0f);
    ASSERT1(line_trace_binding_set != NULL);

    if (m_main_object_layer->IsWrapped())
    {
        ASSERT1(trace_vector.Length() <= 0.5f * m_main_object_layer->SideLength());
        m_quad_tree->LineTraceWrapped(
            trace_start,
            trace_vector,
            trace_radius,
            check_nonsolid_collision_entities,
            line_trace_binding_set,
            m_main_object_layer->SideLength(),
            0.5f * m_main_object_layer->SideLength());
    }
    else
        m_quad_tree->LineTrace(
            trace_start,
            trace_vector,
            trace_radius,
            check_nonsolid_collision_entities,
            line_trace_binding_set);
}

void PhysicsHandler::AreaTrace (
    Engine2::ObjectLayer const *const object_layer,
    FloatVector2 trace_area_center,
    Float const trace_area_radius,
    bool const check_nonsolid_collision_entities,
    AreaTraceList *const area_trace_list) const
{
    ASSERT1(object_layer != NULL);
    ASSERT1(object_layer == m_main_object_layer);
    ASSERT1(trace_area_radius > 0.0f);
    ASSERT1(area_trace_list != NULL);

    if (m_main_object_layer->IsWrapped())
    {
        Float object_layer_side_length = m_main_object_layer->SideLength();
        Float half_object_layer_side_length = 0.5f * object_layer_side_length;

        if (trace_area_center[Dim::X] < -half_object_layer_side_length)
            trace_area_center[Dim::X] += object_layer_side_length;
        else if (trace_area_center[Dim::X] > half_object_layer_side_length)
            trace_area_center[Dim::X] -= object_layer_side_length;

        if (trace_area_center[Dim::Y] < -half_object_layer_side_length)
            trace_area_center[Dim::Y] += object_layer_side_length;
        else if (trace_area_center[Dim::Y] > half_object_layer_side_length)
            trace_area_center[Dim::Y] -= object_layer_side_length;

        m_quad_tree->AreaTraceWrapped(
            trace_area_center,
            trace_area_radius,
            check_nonsolid_collision_entities,
            area_trace_list,
            m_main_object_layer->SideLength(),
            0.5f * m_main_object_layer->SideLength());
    }
    else
        m_quad_tree->AreaTrace(
            trace_area_center,
            trace_area_radius,
            check_nonsolid_collision_entities,
            area_trace_list);
}

void PhysicsHandler::AddObjectLayer (Engine2::ObjectLayer *const object_layer)
{
    ASSERT1(object_layer != NULL);
}

void PhysicsHandler::SetMainObjectLayer (Engine2::ObjectLayer *const object_layer)
{
    ASSERT1(object_layer != NULL);
    ASSERT1(m_main_object_layer == NULL && "Only set the main object layer once");

    // store a pointer to the main object layer
    m_main_object_layer = object_layer;
    // now that the main object layer is set, we can create a
    // collision quadtree to match it.
    m_quad_tree = CollisionQuadTree::Create(0.5f * m_main_object_layer->SideLength(), 5);
}

void PhysicsHandler::AddEntity (
    Engine2::Entity *const entity)
{
    ASSERT1(entity != NULL);
    Entity *dis_entity = DStaticCast<Entity *>(entity);

    // add it to the master entity set
    m_entity_set.insert(dis_entity);

    // if it's not CT_NO_COLLISION, add its
    // owner object into the collision quadtree
    ASSERT1(m_main_object_layer != NULL);
    if (dis_entity->GetCollisionType() != CT_NO_COLLISION)
    {
        DEBUG1_CODE(bool add_was_successful =)
        m_quad_tree->AddObject(dis_entity->OwnerObject());
        ASSERT1(add_was_successful);
    }
}

void PhysicsHandler::RemoveEntity (
    Engine2::Entity *const entity)
{
    ASSERT1(entity != NULL);
    Entity *dis_entity = DStaticCast<Entity *>(entity);

    // remove it from the master entity set
    m_entity_set.erase(dis_entity);

    // if it's not CT_NO_COLLISION, remove its
    // owner object from the collision quadtree
    if (dis_entity->GetCollisionType() != CT_NO_COLLISION)
        dis_entity->OwnerQuadTree(Engine2::QTT_PHYSICS_HANDLER)->RemoveObject(dis_entity->OwnerObject());
}

void PhysicsHandler::HandleFrame ()
{
    ASSERT1(m_main_object_layer != NULL);
    ASSERT1(FrameDT() >= 0.0f);

    // if the frame time delta is zero (e.g. the game is paused), return.
    if (FrameDT() == 0.0f)
        return;

    // resolve interpenetrations / calculate collisions
    if (m_main_object_layer->IsWrapped())
        HandleInterpenetrationsUsingCollisionQuadTreeWrapped();
    else
        HandleInterpenetrationsUsingCollisionQuadTree();

    // call Think on all entity guts.  no entities must be left
    // removed during this loop.  removing and re-adding is ok --
    // see ShieldEffect::SnapToShip().
    for (EntitySetIterator it = m_entity_set.begin(),
                           it_end = m_entity_set.end();
         it != it_end;
         ++it)
    {
        Entity *entity = *it;
        ASSERT1(entity != NULL);

        DEBUG1_CODE(Uint32 entity_set_size = m_entity_set.size());

        if (FrameTime() >= entity->NextTimeToThink())
            entity->Think(FrameTime(), FrameDT());

        ASSERT1(m_entity_set.size() >= entity_set_size &&
                "You must not remove entities during the Think loop -- "
                "use ScheduleForRemovalFromWorld(0.0f) instead");
    }

    // apply the accumulated forces and torques
    UpdateVelocities();

    // update the entities' positions
    UpdatePositions();

    // call the collision handlers for the entities.
    // the calls to Entity::Collide are done after the velocities and
    // positions are updated because the updated velocities/positions are
    // required for some computations (e.g. setting the velocity for an
    // explosion resulting from an asteroid impacting another).
    for (CollisionPairListIterator it = m_collision_pair_list.begin(),
                                   it_end = m_collision_pair_list.end();
         it != it_end;
         ++it)
    {
        CollisionPair &collision_pair = *it;
        collision_pair.m_entity0->Collide(
            collision_pair.m_entity1,
            collision_pair.m_collision_location,
            collision_pair.m_collision_normal,
            collision_pair.m_collision_force,
            FrameTime(),
            FrameDT());
        collision_pair.m_entity1->Collide(
            collision_pair.m_entity0,
            collision_pair.m_collision_location,
            -collision_pair.m_collision_normal,
            collision_pair.m_collision_force,
            FrameTime(),
            FrameDT());
    }
    // clear the collision pair list
    m_collision_pair_list.clear();
}

void PhysicsHandler::UpdateVelocities ()
{
    // apply the accumulated forces to the entities,
    // and then reset their accelerations.
    for (EntitySetIterator it = m_entity_set.begin(),
                           it_end = m_entity_set.end();
         it != it_end;
         ++it)
    {
        Entity *entity = *it;
        ASSERT1(entity != NULL);

        if (!entity->Force().IsZero())
        {
            ASSERT1(Math::IsFinite(entity->Force()[Dim::X]));
            ASSERT1(Math::IsFinite(entity->Force()[Dim::Y]));
            ASSERT1(entity->Mass() > 0.0f);
            entity->AccumulateVelocity(
                FrameDT() * entity->Force() /
                entity->Mass());
            entity->ResetForce();
        }

        // limit the speed for entities with CT_SOLID_COLLISION only
        // NOTE: this is game-specific and should be somehow generalized
        // or taken out if/when this particular PhysicsHandler is added
        // to the library code.
        if (entity->GetCollisionType() == CT_SOLID_COLLISION)
        {
            // limit the speed for non-projectiles only
            if ((entity->GetEntityType() < ET_GRENADE || entity->GetEntityType() > ET_EMP_BOMB) &&
                entity->GetEntityType() != ET_BALLISTIC)
            {
                static Float const s_max_solid_speed = 350.0f;
                static Float const s_max_solid_speed_squared = s_max_solid_speed * s_max_solid_speed;

                Float entity_speed_squared = entity->GetVelocity().LengthSquared();
                if (entity_speed_squared > s_max_solid_speed_squared)
                    entity->SetVelocity(s_max_solid_speed / Math::Sqrt(entity_speed_squared) * entity->GetVelocity());
            }
            // limit the speed for nonsolid objects and those excepted above
            // to some higher, but still finite amount.
            else
            {
                static Float const s_max_nonsolid_speed = 800.0f;
                static Float const s_max_nonsolid_speed_squared = s_max_nonsolid_speed * s_max_nonsolid_speed;

                Float entity_speed_squared = entity->GetVelocity().LengthSquared();
                if (entity_speed_squared > s_max_nonsolid_speed_squared)
                    entity->SetVelocity(s_max_nonsolid_speed / Math::Sqrt(entity_speed_squared) * entity->GetVelocity());
            }
        }
    }
}

void PhysicsHandler::UpdatePositions ()
{
    // apply the velocities to the entities,
    // and then reset them in the object layers.
    for (EntitySetIterator it = m_entity_set.begin(),
                           it_end = m_entity_set.end();
         it != it_end;
         ++it)
    {
        Entity *entity = *it;
        ASSERT1(entity != NULL);

        if (!entity->GetVelocity().IsZero())
        {
            entity->Translate(FrameDT() * entity->GetVelocity());
            ASSERT1(entity->GetObjectLayer() != NULL);
            entity->ReAddToQuadTree(Engine2::QTT_VISIBILITY);
            if (entity->GetCollisionType() != CT_NO_COLLISION)
            {
                ASSERT1(entity->OwnerQuadTree(Engine2::QTT_PHYSICS_HANDLER) != NULL);
                entity->ReAddToQuadTree(Engine2::QTT_PHYSICS_HANDLER);
            }
        }

        if (entity->AngularVelocity() != 0.0)
            entity->Rotate(FrameDT() * entity->AngularVelocity());
    }
}

void PhysicsHandler::HandleInterpenetrationsUsingCollisionQuadTree ()
{
    ASSERT1(m_quad_tree != NULL);
    ASSERT1(m_collision_pair_list.empty());

    for (EntitySetIterator it = m_entity_set.begin(),
                           it_end = m_entity_set.end();
         it != it_end;
         ++it)
    {
        Entity *entity = *it;
        ASSERT1(entity != NULL);

        // don't attempt to collide no-collision entities
        if (entity->GetCollisionType() == CT_NO_COLLISION)
            continue;

        // traverse the collision quad tree and calculate collision pairs
        m_quad_tree->CollideEntity(entity, FrameDT(), &m_collision_pair_list);
    }
}

void PhysicsHandler::HandleInterpenetrationsUsingCollisionQuadTreeWrapped ()
{
    ASSERT1(m_main_object_layer->IsWrapped());
    ASSERT1(m_quad_tree != NULL);
    ASSERT1(m_collision_pair_list.empty());

    for (EntitySetIterator it = m_entity_set.begin(),
                           it_end = m_entity_set.end();
         it != it_end;
         ++it)
    {
        Entity *entity = *it;
        ASSERT1(entity != NULL);

        // don't attempt to collide no-collision entities
        if (entity->GetCollisionType() == CT_NO_COLLISION)
            continue;

        // traverse the collision quad tree and calculate collision pairs
        m_quad_tree->CollideEntityWrapped(
            entity,
            FrameDT(),
            &m_collision_pair_list,
            m_main_object_layer->SideLength());
    }
}

} // end of namespace Dis
