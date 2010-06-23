// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_circle_physicshandler.cpp by Victor Dods, created 2005/11/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_circle_physicshandler.hpp"

#include "xrb_engine2_circle_collisionquadtree.hpp"
#include "xrb_engine2_circle_entity.hpp"
#include "xrb_engine2_objectlayer.hpp"
#include "xrb_engine2_world.hpp"

namespace Xrb {
namespace Engine2 {
namespace Circle {

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
    ObjectLayer const *object_layer,
    FloatVector2 const &area_center,
    Float area_radius,
    bool check_nonsolid_collision_entities) const
{
    ASSERT1(object_layer != NULL);
    ASSERT1(object_layer == m_main_object_layer);
    ASSERT1(m_quad_tree != NULL);
    return m_quad_tree->DoesAreaOverlapAnyEntity(
        area_center,
        area_radius,
        check_nonsolid_collision_entities,
        m_main_object_layer->IsWrapped(),
        m_main_object_layer->SideLength(),
        0.5f * m_main_object_layer->SideLength());
}

void PhysicsHandler::LineTrace (
    ObjectLayer const *object_layer,
    FloatVector2 const &trace_start,
    FloatVector2 const &trace_vector,
    Float trace_radius,
    bool check_nonsolid_collision_entities,
    LineTraceBindingSet *line_trace_binding_set) const
{
    ASSERT1(object_layer != NULL);
    ASSERT1(object_layer == m_main_object_layer);
    ASSERT1(trace_radius >= 0.0f);
    ASSERT1(line_trace_binding_set != NULL);
    ASSERT1(trace_vector.Length() <= 0.5f * m_main_object_layer->SideLength());
    m_quad_tree->LineTrace(
        trace_start,
        trace_vector,
        trace_radius,
        check_nonsolid_collision_entities,
        line_trace_binding_set,
        m_main_object_layer->IsWrapped(),
        m_main_object_layer->SideLength(),
        0.5f * m_main_object_layer->SideLength());
}

void PhysicsHandler::AreaTrace (
    ObjectLayer const *object_layer,
    FloatVector2 trace_area_center,
    Float trace_area_radius,
    bool check_nonsolid_collision_entities,
    AreaTraceList *area_trace_list) const
{
    ASSERT1(object_layer != NULL);
    ASSERT1(object_layer == m_main_object_layer);
    ASSERT1(trace_area_radius > 0.0f);
    ASSERT1(area_trace_list != NULL);

    // this is unfortunately necessary
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
    }

    m_quad_tree->AreaTrace(
        trace_area_center,
        trace_area_radius,
        check_nonsolid_collision_entities,
        area_trace_list,
        m_main_object_layer->IsWrapped(),
        m_main_object_layer->SideLength(),
        0.5f * m_main_object_layer->SideLength());
}

void PhysicsHandler::CalculateAmbientMomentum (
    ObjectLayer const *object_layer,
    FloatVector2 const &scan_area_center,
    Float scan_area_radius,
    Entity const *ignore_me,
    FloatVector2 &ambient_momentum,
    Float &ambient_mass) const
{
    // do an area trace
    AreaTraceList area_trace_list;
    AreaTrace(
        object_layer,
        scan_area_center,
        scan_area_radius,
        false,
        &area_trace_list);

    // calculate the ambient momentum
    ambient_momentum = FloatVector2::ms_zero;
    ambient_mass = 0.0f;
    for (AreaTraceList::iterator it = area_trace_list.begin(),
                                 it_end = area_trace_list.end();
         it != it_end;
         ++it)
    {
        Entity const *entity = *it;
        // ignore NULL game objects
        if (entity == NULL)
            continue;
        // if it matches the ignore object, skip it.
        else if (entity == ignore_me)
            continue;

        ambient_momentum += entity->Momentum();
        ambient_mass += entity->Mass();
    }
}

void PhysicsHandler::AddObjectLayer (ObjectLayer *object_layer)
{
    ASSERT1(object_layer != NULL);
}

void PhysicsHandler::SetMainObjectLayer (ObjectLayer *object_layer)
{
    ASSERT1(object_layer != NULL);
    ASSERT1(m_main_object_layer == NULL && "Only set the main object layer once");

    // store a pointer to the main object layer
    m_main_object_layer = object_layer;
    // now that the main object layer is set, we can create a
    // collision quadtree to match it.
    m_quad_tree = CollisionQuadTree::Create(0.5f * m_main_object_layer->SideLength(), 5);
}

void PhysicsHandler::AddEntity (Engine2::Entity *entity)
{
    ASSERT1(entity != NULL);
    Entity *circle_entity = DStaticCast<Entity *>(entity);

    // add it to the master entity set
    m_entity_set.insert(circle_entity);

    // if it's not CT_NO_COLLISION, add its
    // owner object into the collision quadtree
    ASSERT1(m_quad_tree != NULL);
    if (circle_entity->GetCollisionType() != CT_NO_COLLISION)
    {
        DEBUG1_CODE(bool add_was_successful =)
        m_quad_tree->AddObject(circle_entity->OwnerObject());
        ASSERT1(add_was_successful);
    }
}

void PhysicsHandler::RemoveEntity (Engine2::Entity *entity)
{
    ASSERT1(entity != NULL);
    Entity *circle_entity = DStaticCast<Entity *>(entity);

    // remove it from the master entity set
    m_entity_set.erase(circle_entity);

    // if it's not CT_NO_COLLISION, remove its
    // owner object from the collision quadtree
    if (circle_entity->GetCollisionType() != CT_NO_COLLISION)
        circle_entity->OwnerQuadTree(QTT_PHYSICS_HANDLER)->RemoveObject(circle_entity->OwnerObject());
}

void PhysicsHandler::HandleFrame ()
{
    ASSERT1(m_main_object_layer != NULL);
    ASSERT1(FrameDT() >= 0.0f);

    // if the frame time delta is zero (e.g. the game is paused), return.
    if (FrameDT() == 0.0f)
        return;

    // resolve interpenetrations / calculate collisions
    HandleInterpenetrations();

    // call Think on all entity guts.  no entities must be left
    // removed during this loop.  removing and re-adding is ok --
    // see ShieldEffect::SnapToShip().
    for (EntitySet::iterator it = m_entity_set.begin(),
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
    // the calls to Entity::Collide_ are done after the velocities and
    // positions are updated because the updated velocities/positions are
    // required for some computations (e.g. setting the velocity for an
    // explosion resulting from an asteroid impacting another).
    for (CollisionPairList::iterator it = m_collision_pair_list.begin(),
                                     it_end = m_collision_pair_list.end();
         it != it_end;
         ++it)
    {
        CollisionPair &collision_pair = *it;
        collision_pair.m_entity0->Collide_(
            collision_pair.m_entity1,
            collision_pair.m_collision_location,
            collision_pair.m_collision_normal,
            collision_pair.m_collision_force,
            FrameTime(),
            FrameDT());
        collision_pair.m_entity1->Collide_(
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
    for (EntitySet::iterator it = m_entity_set.begin(),
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

        // WHOA THERE BUDDY!  SLOW DOWN!
        Float max_entity_speed = MaxSpeed(*entity);
        Float max_entity_speed_squared = max_entity_speed * max_entity_speed;
        Float entity_speed_squared = entity->Velocity().LengthSquared();
        if (entity_speed_squared > max_entity_speed_squared)
            entity->SetVelocity(max_entity_speed / Math::Sqrt(entity_speed_squared) * entity->Velocity());
    }
}

void PhysicsHandler::UpdatePositions ()
{
    // apply the velocities to the entities,
    // and then reset them in the object layers.
    for (EntitySet::iterator it = m_entity_set.begin(),
                             it_end = m_entity_set.end();
         it != it_end;
         ++it)
    {
        Entity *entity = *it;
        ASSERT1(entity != NULL);

        if (!entity->Velocity().IsZero())
        {
            entity->Translate(FrameDT() * entity->Velocity());
            ASSERT1(entity->GetObjectLayer() != NULL);
            entity->ReAddToQuadTree(QTT_VISIBILITY);
            if (entity->GetCollisionType() != CT_NO_COLLISION)
            {
                ASSERT1(entity->OwnerQuadTree(QTT_PHYSICS_HANDLER) != NULL);
                entity->ReAddToQuadTree(QTT_PHYSICS_HANDLER);
            }
        }

        if (entity->AngularVelocity() != 0.0)
            entity->Rotate(FrameDT() * entity->AngularVelocity());
    }
}

void PhysicsHandler::HandleInterpenetrations ()
{
    ASSERT1(m_quad_tree != NULL);
    ASSERT1(m_collision_pair_list.empty());

    for (EntitySet::iterator it = m_entity_set.begin(),
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
        m_quad_tree->CollideEntity(
            entity,
            FrameDT(),
            &m_collision_pair_list,
            m_main_object_layer->IsWrapped(),
            m_main_object_layer->SideLength());
    }
}

} // end of namespace Circle
} // end of namespace Engine2
} // end of namespace Xrb
