// ///////////////////////////////////////////////////////////////////////////
// dis_physicshandler.cpp by Victor Dods, created 2005/11/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_physicshandler.h"

#include "dis_collisionquadtree.h"
#include "dis_entity.h"
#include "xrb_engine2_objectlayer.h"
#include "xrb_engine2_world.h"

using namespace Xrb;

namespace Dis
{

PhysicsHandler::PhysicsHandler ()
    :
    Engine2::PhysicsHandler()
{
    m_gravitational_constant = 1.0;
    m_main_object_layer = NULL;
    m_quad_tree = NULL;
}

PhysicsHandler::~PhysicsHandler ()
{
    m_entity_set.clear();
    m_applies_gravity_set.clear();
    m_applies_and_reacts_to_gravity_set.clear();
    m_reacts_to_gravity_set.clear();
    m_quad_tree->Clear();
    Delete(m_quad_tree);
}

bool PhysicsHandler::GetDoesAreaOverlapAnyEntityInObjectLayer (
    Engine2::ObjectLayer const *const object_layer,
    FloatVector2 const &area_center,
    Float const area_radius,
    bool const check_nonsolid_collision_entities) const
{
    ASSERT1(object_layer != NULL)
    ASSERT1(object_layer == m_main_object_layer)
    ASSERT1(m_quad_tree != NULL)
    if (m_main_object_layer->GetIsWrapped())
        return m_quad_tree->GetDoesAreaOverlapAnyEntityWrapped(
            area_center,
            area_radius,
            check_nonsolid_collision_entities,
            m_main_object_layer->GetSideLength(),
            0.5f * m_main_object_layer->GetSideLength());
    else
        return m_quad_tree->GetDoesAreaOverlapAnyEntity(
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
    ASSERT1(object_layer != NULL)
    ASSERT1(object_layer == m_main_object_layer)
    ASSERT1(trace_radius >= 0.0f)
    ASSERT1(line_trace_binding_set != NULL)

    if (m_main_object_layer->GetIsWrapped())
    {
        ASSERT1(trace_vector.GetLength() <= 0.5f * m_main_object_layer->GetSideLength())
        m_quad_tree->LineTraceWrapped(
            trace_start,
            trace_vector,
            trace_radius,
            check_nonsolid_collision_entities,
            line_trace_binding_set,
            m_main_object_layer->GetSideLength(),
            0.5f * m_main_object_layer->GetSideLength());
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
    ASSERT1(object_layer != NULL)
    ASSERT1(object_layer == m_main_object_layer)
    ASSERT1(trace_area_radius > 0.0f)
    ASSERT1(area_trace_list != NULL)

    if (m_main_object_layer->GetIsWrapped())
    {
        Float object_layer_side_length = m_main_object_layer->GetSideLength();
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
            m_main_object_layer->GetSideLength(),
            0.5f * m_main_object_layer->GetSideLength());
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
    ASSERT1(object_layer != NULL)
}

void PhysicsHandler::SetMainObjectLayer (Engine2::ObjectLayer *const object_layer)
{
    ASSERT1(object_layer != NULL)
    ASSERT1(m_main_object_layer == NULL && "Only set the main object layer once")

    // store a pointer to the main object layer
    m_main_object_layer = object_layer;
    // now that the main object layer is set, we can create a
    // collision quadtree to match it.
    m_quad_tree = CollisionQuadTree::Create(0.5f * m_main_object_layer->GetSideLength(), 5);
}

void PhysicsHandler::AddEntity (
    Engine2::Entity *const entity)
{
    ASSERT1(entity != NULL)
    Entity *dis_entity = DStaticCast<Entity *>(entity);

    // add it to the master entity set
    m_entity_set.insert(dis_entity);

    // if it's not CT_NO_COLLISION, add its
    // owner object into the collision quadtree
    ASSERT1(m_main_object_layer != NULL)
    if (dis_entity->GetCollisionType() != CT_NO_COLLISION)
    {
        DEBUG1_CODE(bool add_was_successful =)
        m_quad_tree->AddObject(dis_entity->GetOwnerObject());
        ASSERT1(add_was_successful)
    }
}

void PhysicsHandler::RemoveEntity (
    Engine2::Entity *const entity)
{
    ASSERT1(entity != NULL)
    Entity *dis_entity = DStaticCast<Entity *>(entity);

    // remove it from the master entity set
    m_entity_set.erase(dis_entity);

    // if it's not CT_NO_COLLISION, remove its
    // owner object from the collision quadtree
    if (dis_entity->GetCollisionType() != CT_NO_COLLISION)
        dis_entity->GetOwnerQuadTree(Engine2::QTT_PHYSICS_HANDLER)->RemoveObject(dis_entity->GetOwnerObject());
}

void PhysicsHandler::ProcessFrameOverride ()
{
    ASSERT1(m_main_object_layer != NULL)
    ASSERT1(GetFrameDT() >= 0.0f)

    // if the frame time delta is zero (e.g. the game is paused), return.
    if (GetFrameDT() == 0.0f)
        return;
    
    // resolve interpenetrations / calculate collisions
    if (m_main_object_layer->GetIsWrapped())
        HandleInterpenetrationsUsingCollisionQuadTreeWrapped();
    else
        HandleInterpenetrationsUsingCollisionQuadTree();

    // call Think on all entity guts.  the funky incrementing
    // is so that we don't have to worry about not removing the
    // currently-iterating entity during its Think method.
    for (EntitySetIterator inc_it = m_entity_set.begin(),
                           it = inc_it++,
                           it_end = m_entity_set.end();
         it != it_end;
         it = (inc_it != it_end) ? inc_it++ : inc_it)
    {
        Entity *entity = *it;
        ASSERT1(entity != NULL)
        if (GetFrameTime() >= entity->GetNextTimeToThink())
            entity->Think(GetFrameTime(), GetFrameDT());
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
            GetFrameTime(),
            GetFrameDT());
        collision_pair.m_entity1->Collide(
            collision_pair.m_entity0,
            collision_pair.m_collision_location,
            -collision_pair.m_collision_normal,
            collision_pair.m_collision_force,
            GetFrameTime(),
            GetFrameDT());
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
        ASSERT1(entity != NULL)

        if (!entity->GetForce().GetIsZero())
        {
            ASSERT1(entity->GetForce()[Dim::X] != Math::Nan())
            ASSERT1(entity->GetForce()[Dim::Y] != Math::Nan())
            ASSERT1(entity->GetFirstMoment() > 0.0f)
            entity->AccumulateVelocity(
                GetFrameDT() * entity->GetForce() /
                entity->GetFirstMoment());
            entity->ResetForce();
        }

        // limit the speed for entities with CT_SOLID_COLLISION only
        if (entity->GetCollisionType() == CT_SOLID_COLLISION)
        {
            static Float const s_max_speed = 300.0f;
            static Float const s_max_speed_squared = s_max_speed * s_max_speed;

            // limit the speed for non-projectiles only
            if ((entity->GetEntityType() < ET_GRENADE || entity->GetEntityType() > ET_EMP_BOMB) &&
                entity->GetEntityType() != ET_BALLISTIC)
            {                
                Float entity_speed_squared = entity->GetVelocity().GetLengthSquared();
                if (entity_speed_squared > s_max_speed_squared)
                    entity->SetVelocity(s_max_speed / Math::Sqrt(entity_speed_squared) * entity->GetVelocity());
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

        if (!entity->GetVelocity().GetIsZero())
        {
            entity->Translate(GetFrameDT() * entity->GetVelocity());
            ASSERT1(entity->GetObjectLayer() != NULL)
            entity->ReAddToQuadTree(Engine2::QTT_VISIBILITY);
            if (entity->GetCollisionType() != CT_NO_COLLISION)
            {
                ASSERT1(entity->GetOwnerQuadTree(Engine2::QTT_PHYSICS_HANDLER) != NULL)
                entity->ReAddToQuadTree(Engine2::QTT_PHYSICS_HANDLER);
            }
        }

        if (entity->GetAngularVelocity() != 0.0)
            entity->Rotate(GetFrameDT() * entity->GetAngularVelocity());
    }
}

void PhysicsHandler::HandleInterpenetrationsUsingCollisionQuadTree ()
{
    ASSERT1(m_quad_tree != NULL)
    ASSERT1(m_collision_pair_list.empty());

    for (EntitySetIterator it = m_entity_set.begin(),
                           it_end = m_entity_set.end();
         it != it_end;
         ++it)
    {
        Entity *entity = *it;
        ASSERT1(entity != NULL)

        // don't attempt to collide no-collision entities
        if (entity->GetCollisionType() == CT_NO_COLLISION)
            continue;

        // traverse the collision quad tree and calculate collision pairs
        m_quad_tree->CollideEntity(entity, GetFrameDT(), &m_collision_pair_list);
    }
}

void PhysicsHandler::HandleInterpenetrationsUsingCollisionQuadTreeWrapped ()
{
    ASSERT1(m_main_object_layer->GetIsWrapped())
    ASSERT1(m_quad_tree != NULL)
    ASSERT1(m_collision_pair_list.empty());

    for (EntitySetIterator it = m_entity_set.begin(),
                           it_end = m_entity_set.end();
         it != it_end;
         ++it)
    {
        Entity *entity = *it;
        ASSERT1(entity != NULL)

        // don't attempt to collide no-collision entities
        if (entity->GetCollisionType() == CT_NO_COLLISION)
            continue;

        // traverse the collision quad tree and calculate collision pairs
        m_quad_tree->CollideEntityWrapped(
            entity,
            GetFrameDT(),
            &m_collision_pair_list,
            m_main_object_layer->GetSideLength());
    }
}

} // end of namespace Dis
