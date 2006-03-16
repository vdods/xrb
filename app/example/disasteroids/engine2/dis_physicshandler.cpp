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
#include "dis_gameobject.h"
#include "xrb_engine2_entity.h"
#include "xrb_engine2_objectlayer.h"
#include "xrb_engine2_spriteentity.h"
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

    // add it to the master entity set
    m_entity_set.insert(entity);

    // add it to the appropriate gravitational set
    if (entity->GetAppliesGravity())
    {
        if (entity->GetReactsToGravity())
            m_applies_and_reacts_to_gravity_set.insert(entity);
        else
            m_applies_gravity_set.insert(entity);
    }
    else
    {
        if (entity->GetReactsToGravity())
            m_reacts_to_gravity_set.insert(entity);
    }

    // add it to the collision quadtree (if it has collision type
    // that isn't CT_NO_COLLISION).
    ASSERT1(m_main_object_layer != NULL)
    if (entity->GetCollisionType() != Engine2::CT_NO_COLLISION)
    {
        bool add_was_successful = m_quad_tree->AddObject(entity);
        ASSERT1(add_was_successful)
    }
}

void PhysicsHandler::RemoveEntity (
    Engine2::Entity *const entity)
{
    ASSERT1(entity != NULL)

    // remove it from the master entity set
    m_entity_set.erase(entity);

    // remove it from the appropriate gravitational set
    if (entity->GetAppliesGravity())
    {
        if (entity->GetReactsToGravity())
            m_applies_and_reacts_to_gravity_set.erase(entity);
        else
            m_applies_gravity_set.erase(entity);
    }
    else
    {
        if (entity->GetReactsToGravity())
            m_reacts_to_gravity_set.erase(entity);
    }

    // remove it from the collision quadtree (if it has collision
    // type that isn't CT_NO_COLLISION).
    if (entity->GetCollisionType() != Engine2::CT_NO_COLLISION)
        entity->GetOwnerQuadTree(Engine2::QTT_PHYSICS_HANDLER)->RemoveObject(entity);
}

void PhysicsHandler::HandleChangedEntityRadius (
    Engine2::Entity *const entity,
    Float const old_radius,
    Float const new_radius)
{
    ASSERT1(entity != NULL)
    ASSERT1(old_radius != new_radius)
    // TODO: this might be unnecessary -- the physics handler can
    // go through its master list of entities each frame and make
    // sure they are at the right quadtree node
}

void PhysicsHandler::HandleChangedEntityCollisionType (
    Engine2::Entity *const entity,
    Engine2::CollisionType const old_collision_type,
    Engine2::CollisionType const new_collision_type)
{
    ASSERT1(entity != NULL)
    ASSERT1(old_collision_type != new_collision_type)

    // if the collision changed from no-collision to non-no-collision,
    // then add it to the collision quadtree.  otherwise, remove it.
    if (old_collision_type == Engine2::CT_NO_COLLISION)
        m_quad_tree->AddObject(entity);
    else if (new_collision_type == Engine2::CT_NO_COLLISION)
        entity->GetOwnerQuadTree(Engine2::QTT_PHYSICS_HANDLER)->RemoveObject(entity);
}

void PhysicsHandler::HandleChangedEntityAppliesGravity (
    Engine2::Entity *const entity,
    bool const old_applies_gravity,
    bool const new_applies_gravity)
{
    ASSERT1(entity != NULL)
    ASSERT1(old_applies_gravity != new_applies_gravity)
    if (old_applies_gravity)
    {
        if (entity->GetReactsToGravity())
        {
            m_applies_and_reacts_to_gravity_set.erase(entity);
            m_reacts_to_gravity_set.insert(entity);
        }
        else
            m_applies_gravity_set.erase(entity);
    }
    else
    {
        if (entity->GetReactsToGravity())
        {
            m_reacts_to_gravity_set.erase(entity);
            m_applies_and_reacts_to_gravity_set.insert(entity);
        }
        else
            m_applies_gravity_set.insert(entity);
    }
}

void PhysicsHandler::HandleChangedEntityReactsToGravity (
    Engine2::Entity *const entity,
    bool const old_reacts_to_gravity,
    bool const new_reacts_to_gravity)
{
    ASSERT1(entity != NULL)
    ASSERT1(old_reacts_to_gravity != new_reacts_to_gravity)
    if (old_reacts_to_gravity)
    {
        if (entity->GetAppliesGravity())
        {
            m_applies_and_reacts_to_gravity_set.erase(entity);
            m_applies_gravity_set.insert(entity);
        }
        else
            m_reacts_to_gravity_set.erase(entity);
    }
    else
    {
        if (entity->GetAppliesGravity())
        {
            m_applies_gravity_set.erase(entity);
            m_applies_and_reacts_to_gravity_set.insert(entity);
        }
        else
            m_reacts_to_gravity_set.insert(entity);
    }
}

void PhysicsHandler::ProcessFrameOverride ()
{
    ASSERT1(m_main_object_layer != NULL)
    ASSERT1(GetFrameDT() >= 0.0f)

    // if the frame time delta is zero (e.g. the game is paused), return.
    if (GetFrameDT() == 0.0f)
        return;
    
    // resolve interpenetrations
    if (m_main_object_layer->GetIsWrapped())
    {
//         HandleInterpenetrationsWrapped();
        HandleInterpenetrationsUsingCollisionQuadTreeWrapped();
    }
    else
    {
//         HandleInterpenetrations();
        HandleInterpenetrationsUsingCollisionQuadTree();
    }
    // calculate the entities' gravitational forces
    ApplyGravitationalForce();

    // call Think on all entity guts.  the funky incrementing
    // is so that we don't have to worry about not removing the
    // currently-iterating entity during its Think method.
    for (EntitySetIterator inc_it = m_entity_set.begin(),
                           it = inc_it++,
                           it_end = m_entity_set.end();
         it != it_end;
         it = (inc_it != it_end) ? inc_it++ : inc_it)
    {
        Engine2::Entity *entity = *it;
        ASSERT1(entity != NULL)
        if (entity->GetEntityGuts() != NULL)
        {
            GameObject *game_object =
                static_cast<GameObject *>(entity->GetEntityGuts());
            if (GetFrameTime() >= game_object->GetNextTimeToThink())
                game_object->Think(GetFrameTime(), GetFrameDT());
        }
    } 
    
    // apply the accumulated forces and torques
    UpdateVelocities();
    // update the entities' positions
    UpdatePositions();

    // call the collision handlers for the entities.
    // the calls to GameObject::Collide are done after the velocities and
    // positions are updated because the updated velocities/positions are
    // required for some computations (e.g. setting the velocity for an
    // explosion resulting from an asteroid impacting another).
    for (CollisionPairListIterator it = m_collision_pair_list.begin(),
                                   it_end = m_collision_pair_list.end();
         it != it_end;
         ++it)
    {
        CollisionPair &collision_pair = *it;
        GameObject *guts0 =
            static_cast<GameObject *>(collision_pair.m_entity0->GetEntityGuts());
        GameObject *guts1 =
            static_cast<GameObject *>(collision_pair.m_entity1->GetEntityGuts());
        if (guts0 != NULL)
            guts0->Collide(
                guts1,
                collision_pair.m_collision_location,
                collision_pair.m_collision_normal,
                collision_pair.m_collision_force,
                GetFrameTime(),
                GetFrameDT());
        if (guts1 != NULL)
            guts1->Collide(
                guts0,
                collision_pair.m_collision_location,
                -collision_pair.m_collision_normal,
                collision_pair.m_collision_force,
                GetFrameTime(),
                GetFrameDT());
    }
    // clear the collision pair list
    m_collision_pair_list.clear();    
}

void PhysicsHandler::ApplyGravitationalForce ()
{
    // have to do 4 separate set-cross-products:
    
    // applies-gravity x reacts-to-gravity
    CrossProductBetweenGravitationalEntitySets(
        &m_applies_gravity_set,
        &m_reacts_to_gravity_set);
    // applies-gravity x applies-and-reacts-to-gravity
    CrossProductBetweenGravitationalEntitySets(
        &m_applies_gravity_set,
        &m_applies_and_reacts_to_gravity_set);
    // applies-and-reacts-to-gravity x applies-and-reacts-to-gravity
    CrossProductBetweenGravitationalEntitySets(
        &m_applies_and_reacts_to_gravity_set,
        &m_applies_and_reacts_to_gravity_set);
    // applies-and-reacts-to-gravity x reacts-to-gravity
    CrossProductBetweenGravitationalEntitySets(
        &m_applies_and_reacts_to_gravity_set,
        &m_reacts_to_gravity_set);
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
        Engine2::Entity *entity = *it;
        ASSERT1(entity != NULL)

        #if 0
        // check all the forces, velocities and positions for value validity
        ASSERT1(entity->GetForce()[Dim::X] != Math::Nan())
        ASSERT1(entity->GetForce()[Dim::Y] != Math::Nan())
        
        ASSERT1(entity->GetVelocity()[Dim::X] != Math::Nan())
        ASSERT1(entity->GetVelocity()[Dim::Y] != Math::Nan())
        
        ASSERT1(entity->GetTranslation()[Dim::X] != Math::Nan())
        ASSERT1(entity->GetTranslation()[Dim::Y] != Math::Nan())
        #endif
        
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

        if (entity->GetTorque() != 0.0)
        {
            ASSERT1(entity->GetTorque() != Math::Nan())
            ASSERT1(entity->GetSecondMoment() > 0.0f)
            entity->AccumulateAngularVelocity(
                GetFrameDT() * entity->GetTorque() /
                entity->GetSecondMoment());
            entity->ResetTorque();
        }

        static Float const s_max_speed = 300.0f;
        static Float const s_max_speed_squared = s_max_speed * s_max_speed;
        Float entity_speed_squared = entity->GetVelocity().GetLengthSquared();
        if (entity_speed_squared > s_max_speed_squared)
            entity->SetVelocity(s_max_speed / Math::Sqrt(entity_speed_squared) * entity->GetVelocity());
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
        Engine2::Entity *entity = *it;
        ASSERT1(entity != NULL);

        if (!entity->GetVelocity().GetIsZero())
        {
            entity->Translate(GetFrameDT() * entity->GetVelocity());
            ASSERT1(entity->GetObjectLayer() != NULL)
            entity->ReAddToQuadTree(Engine2::QTT_VISIBILITY);
            if (entity->GetCollisionType() != Engine2::CT_NO_COLLISION)
            {
                ASSERT1(entity->GetOwnerQuadTree(Engine2::QTT_PHYSICS_HANDLER) != NULL)
                entity->ReAddToQuadTree(Engine2::QTT_PHYSICS_HANDLER);
            }
        }

        if (entity->GetAngularVelocity() != 0.0)
            entity->Rotate(GetFrameDT() * entity->GetAngularVelocity());
    }
}

void PhysicsHandler::HandleInterpenetrations ()
{
    Engine2::SpriteEntity *ce0;
    Engine2::SpriteEntity *ce1;

    Float adjusted_dt = GetFrameDT();//1.0f/40.0f;
    Float dt_squared = adjusted_dt * adjusted_dt;
    
    // find any interpenetrating entities and apply separation forces
    EntitySetIterator it0;
    EntitySetIterator it0_end = m_entity_set.end();
    EntitySetIterator it1_begin = m_entity_set.begin();
    EntitySetIterator it1;
    EntitySetIterator it1_end = m_entity_set.end();
    for (it0 = m_entity_set.begin(); it0 != it0_end; ++it0)
    {
        Engine2::Entity *ent0 = *it0;
        ASSERT1(ent0 != NULL)
        // only support sprites for now
        ASSERT1(ent0->GetSubType() == Engine2::Object::ST_SPRITE_ENTITY)
        ce0 = dynamic_cast<Engine2::SpriteEntity *>(ent0);
        if (ce0->GetCollisionType() == Engine2::CT_NO_COLLISION)
            continue;

        for (it1 = it1_begin; it0 != it1 && it1 != it1_end; ++it1)
        {
            Engine2::Entity *ent1 = *it1;
            ASSERT1(ent1 != NULL)
            // only support sprites for now
            ASSERT1(ent1->GetSubType() == Engine2::Object::ST_SPRITE_ENTITY)
            ce1 = dynamic_cast<Engine2::SpriteEntity *>(ent1);
            if (ce1->GetCollisionType() == Engine2::CT_NO_COLLISION)
                continue;

            ASSERT1(ent0 != ent1)

            // check for interpenetrations
            Float r = ce0->GetRadius() + ce1->GetRadius();

            FloatVector2 P = ce0->GetTranslation() - ce1->GetTranslation();
            if (P.GetLength() < r) // if the circles are interpenetrating
            {
                
                FloatVector2 V = ce0->GetVelocity() - ce1->GetVelocity();
                FloatVector2 collision_location(
                    (ce0->GetScaleFactor() * ce0->GetTranslation() + ce1->GetScaleFactor() * ce1->GetTranslation())
                    /
                    (ce0->GetScaleFactor() + ce1->GetScaleFactor()));
                FloatVector2 collision_normal;
                if (P.GetIsZero())
                    collision_normal = FloatVector2(1.0f, 0.0f);
                else
                    collision_normal = P.GetNormalization();
                Float collision_force = 0.0f;
    
                if ((V | P) < 0.0f && // and if they're moving toward each other
                    ce0->GetCollisionType() == Engine2::CT_SOLID_COLLISION && // and if they're both solid
                    ce1->GetCollisionType() == Engine2::CT_SOLID_COLLISION &&
                    GameObject::GetShouldApplyCollisionForces( // and if this isn't an exception to the rule
                        DStaticCast<GameObject const *>(ce0->GetEntityGuts()),
                        DStaticCast<GameObject const *>(ce1->GetEntityGuts())))
                {
                    Float M = 1.0f / ce0->GetFirstMoment() + 1.0f / ce1->GetFirstMoment();
                    FloatVector2 Q(P + adjusted_dt*V);
                    FloatVector2 A(dt_squared*M*collision_normal);
    
                    Float a = A | A;
                    Float b = 2.0f * (Q | A);
                    Float c = (Q | Q) - r*r;
                    Float discriminant = b*b - 4.0f*a*c;
                    Float temp0, temp1;
                    Float force0, force1;
                    Float min_force, max_force;
                    if (discriminant >= 0.0f)
                    {
                        temp0 = Math::Sqrt(discriminant);
                        temp1 = 2.0f * a;
    
                        force0 = 0.8f * (-b - temp0) / temp1;
                        force1 = 0.8f * (-b + temp0) / temp1;
                        min_force = Min(force0, force1);
                        max_force = Max(force0, force1);
                        if (min_force > 0.0f)
                            collision_force = min_force;
                        else if (max_force > 0.0f)
                            collision_force = max_force;
                        else
                            collision_force = 0.0f;

                        collision_force *= (1.0f + ce0->GetElasticity() * ce1->GetElasticity());
    
                        ce0->AccumulateForce(collision_force*collision_normal);
                        ce1->AccumulateForce(-collision_force*collision_normal);
                    }
                }

                // record the collision in the collision pair list.
                m_collision_pair_list.push_back(
                    CollisionPair(
                        ce0,
                        ce1,
                        collision_location,
                        collision_normal,
                        collision_force));
            }
        }
    }
}

void PhysicsHandler::HandleInterpenetrationsWrapped ()
{
    Engine2::SpriteEntity *ce0;
    Engine2::SpriteEntity *ce1;

    ASSERT1(m_main_object_layer != NULL)
    Float object_layer_side_length = m_main_object_layer->GetSideLength();
    Float half_object_layer_side_length = 0.5f * object_layer_side_length;
    Float adjusted_dt = GetFrameDT();//1.0f/40.0f;
    Float dt_squared = adjusted_dt * adjusted_dt;
    
    // find any interpenetrating entities and apply separation forces
    EntitySetIterator it0;
    EntitySetIterator it0_end = m_entity_set.end();
    EntitySetIterator it1_begin = m_entity_set.begin();
    EntitySetIterator it1;
    EntitySetIterator it1_end = m_entity_set.end();
    for (it0 = m_entity_set.begin(); it0 != it0_end; ++it0)
    {
        Engine2::Entity *ent0 = *it0;
        ASSERT1(ent0 != NULL)
        // only support sprites for now
        ASSERT1(ent0->GetSubType() == Engine2::Object::ST_SPRITE_ENTITY)
        ce0 = dynamic_cast<Engine2::SpriteEntity *>(ent0);
        if (ce0->GetCollisionType() == Engine2::CT_NO_COLLISION)
            continue;

        FloatVector2 ce0_translation(ce0->GetTranslation());
            
        for (it1 = it1_begin; it0 != it1 && it1 != it1_end; ++it1)
        {
            Engine2::Entity *ent1 = *it1;
            ASSERT1(ent1 != NULL)
            // only support sprites for now
            ASSERT1(ent1->GetSubType() == Engine2::Object::ST_SPRITE_ENTITY)
            ce1 = dynamic_cast<Engine2::SpriteEntity *>(ent1);
            if (ce1->GetCollisionType() == Engine2::CT_NO_COLLISION)
                continue;

            ASSERT1(ent0 != ent1)

            // check for interpenetrations
            
            FloatVector2 ce1_translation(ce1->GetTranslation());
            
            if (ce1_translation[Dim::X] - ce0_translation[Dim::X] > half_object_layer_side_length)
                ce1_translation[Dim::X] -= object_layer_side_length;
            else if (ce1_translation[Dim::X] - ce0_translation[Dim::X] < -half_object_layer_side_length)
                ce1_translation[Dim::X] += object_layer_side_length;
                
            if (ce1_translation[Dim::Y] - ce0_translation[Dim::Y] > half_object_layer_side_length)
                ce1_translation[Dim::Y] -= object_layer_side_length;
            else if (ce1_translation[Dim::Y] - ce0_translation[Dim::Y] < -half_object_layer_side_length)
                ce1_translation[Dim::Y] += object_layer_side_length;
     
            Float r = ce0->GetRadius() + ce1->GetRadius();
            FloatVector2 P = ce0_translation - ce1_translation;

            if (P.GetLength() < r) // if the circles are interpenetrating
            {
                FloatVector2 V = ce0->GetVelocity() - ce1->GetVelocity();
                FloatVector2 collision_location(
                    (ce0->GetScaleFactor() * ce0_translation + ce1->GetScaleFactor() * ce1_translation)
                    /
                    (ce0->GetScaleFactor() + ce1->GetScaleFactor()));
                FloatVector2 collision_normal;
                if (P.GetIsZero())
                    collision_normal = FloatVector2(1.0f, 0.0f);
                else
                    collision_normal = P.GetNormalization();
                Float collision_force = 0.0f;
                
                if ((V | P) < 0.0f && // and if they're moving toward each other
                    ce0->GetCollisionType() == Engine2::CT_SOLID_COLLISION && // and if they're both solid
                    ce1->GetCollisionType() == Engine2::CT_SOLID_COLLISION)
                {
                    Float M = 1.0f / ce0->GetFirstMoment() + 1.0f / ce1->GetFirstMoment();
                    FloatVector2 Q(P + adjusted_dt*V);
                    FloatVector2 A(dt_squared*M*collision_normal);
    
                    Float a = A | A;
                    Float b = 2.0f * (Q | A);
                    Float c = (Q | Q) - r*r;
                    Float discriminant = b*b - 4.0f*a*c;
                    Float temp0, temp1;
                    Float force0, force1;
                    Float min_force, max_force;
                    if (discriminant >= 0.0f)
                    {
                        temp0 = Math::Sqrt(discriminant);
                        temp1 = 2.0f * a;
    
                        force0 = 0.8f * (-b - temp0) / temp1;
                        force1 = 0.8f * (-b + temp0) / temp1;
                        min_force = Min(force0, force1);
                        max_force = Max(force0, force1);
                        if (min_force > 0.0f)
                            collision_force = min_force;
                        else if (max_force > 0.0f)
                            collision_force = max_force;
                        else
                            collision_force = 0.0f;

                        collision_force *= (1.0f + ce0->GetElasticity() * ce1->GetElasticity());
    
                        ce0->AccumulateForce(collision_force*collision_normal);
                        ce1->AccumulateForce(-collision_force*collision_normal);
                    }
                }

                // record the collision in the collision pair list.
                m_collision_pair_list.push_back(
                    CollisionPair(
                        ce0,
                        ce1,
                        collision_location,
                        collision_normal,
                        collision_force));
            }
        }
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
        Engine2::Entity *entity = *it;
        ASSERT1(entity != NULL)

        // don't attempt to collide no-collision entities
        if (entity->GetCollisionType() == Engine2::CT_NO_COLLISION)
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

    Float object_layer_side_length = m_main_object_layer->GetSideLength();
    Float half_object_layer_side_length = 0.5f * object_layer_side_length;
    
    for (EntitySetIterator it = m_entity_set.begin(),
                           it_end = m_entity_set.end();
         it != it_end;
         ++it)
    {
        Engine2::Entity *entity = *it;
        ASSERT1(entity != NULL)

        // don't attempt to collide no-collision entities
        if (entity->GetCollisionType() == Engine2::CT_NO_COLLISION)
            continue;

        // traverse the collision quad tree and calculate collision pairs
        m_quad_tree->CollideEntityWrapped(
            entity,
            GetFrameDT(),
            &m_collision_pair_list,
            object_layer_side_length,
            half_object_layer_side_length);
    }
}

void PhysicsHandler::CrossProductBetweenGravitationalEntitySets (
    EntitySet *const set0,
    EntitySet *const set1)
{
    EntitySetIterator it0 = set0->begin();
    EntitySetIterator it0_end = set0->end();
    EntitySetIterator it1_begin = set1->begin();
    EntitySetIterator it1;
    EntitySetIterator it1_end = set1->end();
    for (it0 = set0->begin(); it0 != it0_end; ++it0)
    {
        Engine2::Entity *entity0 = *it0;
        ASSERT1(entity0 != NULL)

        for (it1 = it1_begin; it0 != it1 && it1 != it1_end; ++it1)
        {
            Engine2::Entity *entity1 = *it1;
            ASSERT1(entity1 != NULL)

            if (entity0 == entity1)
            {
                ASSERT1(set0 == set1)
                continue;
            }

            // gravity calculations
            FloatVector2 r(entity0->GetTranslation() - entity1->GetTranslation());
            Float force;
            Float r_length_squared = r.GetLengthSquared();
            Float entity_r_sum_squared = entity0->GetRadius() + entity1->GetRadius();
            entity_r_sum_squared *= entity_r_sum_squared * 0.5f * Math::Sqrt(2.0f);
            if (r_length_squared < entity_r_sum_squared)
                force = 0.0f;
            else
                force =
                    m_gravitational_constant *
                    entity0->GetFirstMoment() *
                    entity1->GetFirstMoment() /
                    r_length_squared;
            r.Normalize();
            if (entity0->GetReactsToGravity())
                entity0->AccumulateForce(force * -r);
            if (entity1->GetReactsToGravity())
                entity1->AccumulateForce(force * r);
        }
    }
}

} // end of namespace Dis
