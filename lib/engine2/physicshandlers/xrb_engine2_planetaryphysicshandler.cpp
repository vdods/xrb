// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_planetaryphysicshandler.cpp by Victor Dods, created 2005/04/14
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_planetaryphysicshandler.h"

#include "xrb_engine2_entity.h"
#include "xrb_engine2_world.h"

namespace Xrb
{

Engine2::PlanetaryPhysicsHandler::PlanetaryPhysicsHandler ()
    :
    PhysicsHandler()
{
    m_gravitational_constant = 1.0;
}

Engine2::PlanetaryPhysicsHandler::~PlanetaryPhysicsHandler ()
{
    m_entity_set.clear();
    m_applies_gravity_set.clear();
    m_applies_and_reacts_to_gravity_set.clear();
    m_reacts_to_gravity_set.clear();
}

void Engine2::PlanetaryPhysicsHandler::AddEntity (
    Engine2::Entity *const entity)
{
    ASSERT1(entity != NULL)

    // add it to the master entity set
    m_entity_set.insert(entity);

    // add it to the appropriate gravitational set
    if (entity->GetAppliesGravity() && entity->GetReactsToGravity())
        m_applies_and_reacts_to_gravity_set.insert(entity);
    else if (entity->GetAppliesGravity())
        m_applies_gravity_set.insert(entity);
    else if (entity->GetReactsToGravity())
        m_reacts_to_gravity_set.insert(entity);
}

void Engine2::PlanetaryPhysicsHandler::RemoveEntity (
    Engine2::Entity *const entity)
{
    ASSERT1(entity != NULL)

    // remove it from the master entity set
    m_entity_set.erase(entity);

    // remove it from the appropriate gravitational set
    if (entity->GetAppliesGravity() && entity->GetReactsToGravity())
        m_applies_and_reacts_to_gravity_set.erase(entity);
    else if (entity->GetAppliesGravity())
        m_applies_gravity_set.erase(entity);
    else if (entity->GetReactsToGravity())
        m_reacts_to_gravity_set.erase(entity);
}

void Engine2::PlanetaryPhysicsHandler::HandleChangedEntityAppliesGravity (
    Entity *const entity,
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

void Engine2::PlanetaryPhysicsHandler::HandleChangedEntityReactsToGravity (
    Entity *const entity,
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

void Engine2::PlanetaryPhysicsHandler::ProcessFrameOverride ()
{
    // resolve interpenetrations
    HandleInterpenetrations();
    // calculate the entities' gravitational forces
    ApplyGravitationalForce();
    // apply the accumulated forces and torques
    UpdateVelocities();
    // perform collision handling
    HandleCollisions();
    // update the entities' positions
    UpdatePositions();
}

void Engine2::PlanetaryPhysicsHandler::ApplyGravitationalForce ()
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

void Engine2::PlanetaryPhysicsHandler::UpdateVelocities ()
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
            entity->AccumulateVelocity(GetFrameDT() * entity->GetForce() /
                                         entity->GetFirstMoment());
            entity->ResetForce();
        }

        if (entity->GetTorque() != 0.0)
        {
            entity->AccumulateAngularVelocity(GetFrameDT() * entity->GetTorque() /
                                                entity->GetSecondMoment());
            entity->ResetTorque();
        }
    }
}

void Engine2::PlanetaryPhysicsHandler::UpdatePositions ()
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
            entity->ReAddToQuadTree(QTT_VISIBILITY);
        }

        if (entity->GetAngularVelocity() != 0.0)
            entity->Rotate(GetFrameDT() * entity->GetAngularVelocity());
    }
}

void Engine2::PlanetaryPhysicsHandler::HandleCollisions ()
{
    Float collision_dt_lower_bound = 0.0;
    Float collision_dt;
    Entity *entity0;
    Entity *entity1;
    Entity *ce0;
    Entity *ce1;
    Entity *last_ce0 = 0;
    Entity *last_ce1 = 0;
    Entity *temp_ent;
    FloatVector2 V, P;
    Float R;
    Float discriminant;
    Float a, b, c;
    Float temp0, temp1;
    Float t0, t1;

    // NOTE: ce stands for collision entity

    // loop until all the collisions during this time step
    // have been resolved
    while (1)
    {
        collision_dt = GetFrameDT();
        ce0 = 0;
        ce1 = 0;

        // figure out the first collision in the time interval between
        // collision_dt_lower_bound and next_collision_time
        EntitySetIterator it0;
        EntitySetIterator it0_end = m_entity_set.end();
        EntitySetIterator it1_begin = m_entity_set.begin();
        EntitySetIterator it1;
        EntitySetIterator it1_end = m_entity_set.end();
        for (it0 = m_entity_set.begin(); it0 != it0_end; ++it0)
        {
            Entity *ent0 = *it0;
            ASSERT1(ent0 != NULL)

            for (it1 = it1_begin; it0 != it1 && it1 != it1_end; ++it1)
            {
                Entity *ent1 = *it1;
                ASSERT1(ent1 != NULL)

                ASSERT1(ent0 != ent1)

                entity0 = ent0;
                entity1 = ent1;

                // make sure the entities are ordered
                if (entity0 > entity1)
                {
                    temp_ent = entity0;
                    entity0 = entity1;
                    entity1 = temp_ent;
                }

                // make sure we don't collide the same entities
                // 2 times in a row (in the same time step)
                if (last_ce0 &&
                    entity0 == last_ce0 &&
                    entity1 == last_ce1)
                    continue;

                V = entity0->GetVelocity() - entity1->GetVelocity();
                P = entity0->GetTranslation() - entity1->GetTranslation();
                R = entity0->GetRadius() + entity1->GetRadius();

                // sphere/sphere collision check
                // parameterized by time
                a = V | V;
                b = 2.0f * (V | P);
                c = (P | P) - R*R;

                discriminant = b*b - 4.0f*a*c;
                if (discriminant >= 0.0f)
                {
                    temp0 = sqrt(discriminant);
                    temp1 = 2.0f * a;
                    t0 = (-b - temp0) / temp1;
                    t1 = (-b + temp0) / temp1;
                    // check if either of the solutions are within
                    // the current time step, with the earlier
                    // solution taking priority.
//                     if (-GetFrameDT() <= t0 && t0 <= 2.0*GetFrameDT())
//                         fprintf(stderr, "\t##      potential collision: %s/%s - time %.15e, limit %.15e\n", entity0->GetName().c_str(), entity1->GetName().c_str(), t0, collision_dt_lower_bound);
//                     if (t0 >= collision_dt_lower_bound && t0 <= collision_dt)
//                     {
                    if (collision_dt_lower_bound <= t0 && t0 <= collision_dt)
                    {
//                         fprintf(stderr, "\t--- %s/%s - collision time %.15e\n", entity0->GetName().c_str(), entity1->GetName().c_str(), t0);
                        // save the collision time
                        collision_dt = t0;
                        ce0 = entity0;
                        ce1 = entity1;
                    }
                }
            }
        }
        // update the time interval to exclude collisions which already
        // took place
        if (collision_dt > collision_dt_lower_bound)
            collision_dt_lower_bound = collision_dt;

        // if there was a collision, calculate the forces involved
        if (ce0 != NULL)
        {
            ASSERT1(ce1 != NULL)

            last_ce0 = ce0;
            last_ce1 = ce1;

            ASSERT1(ce0->GetForce().GetIsZero())
            ASSERT1(ce0->GetTorque() == 0.0f)
            ASSERT1(ce1->GetForce().GetIsZero())
            ASSERT1(ce1->GetTorque() == 0.0f)

            // calculate force
            FloatVector2 N; // collision's normal vector
            FloatVector2 R; // point of collision
            Float b0, b1; // normalized torque 'vectors'
            FloatVector2 s0, s1; // instantaneous pointwise linear velocity
            Float E; // combined elasticity
            Float inv_m0, inv_m1; // inverse of the first moments
            Float inv_M0, inv_M1; // scalar second moments
            Float force; // calculated magnitude of force
//             Float torque; // calculated torque 'vector'

            // calculate the force
            N = ce0->GetTranslation() - ce1->GetTranslation();
            N.Normalize();

            R = ce1->GetTranslation() + ce1->GetRadius() * N;

            b0 = N & (R - ce0->GetTranslation());
            b1 = N & (R - ce1->GetTranslation());

            s0 = ce0->GetVelocity() +
                 ce0->GetAngularVelocity() * (R - ce0->GetTranslation());
            s1 = ce1->GetVelocity() +
                 ce1->GetAngularVelocity() * (R - ce1->GetTranslation());

            E = ce0->GetElasticity() * ce1->GetElasticity();

            inv_m0 = 1.0f / ce0->GetFirstMoment();
            inv_m1 = 1.0f / ce1->GetFirstMoment();

            inv_M0 = 1.0f / ce0->GetSecondMoment();
            inv_M1 = 1.0f / ce1->GetSecondMoment();

            force = (1.0f + E) * (N | (s1 - s0)) /
                    (inv_m0 + inv_m1 + inv_M0*b0*b0 + inv_M1*b1*b1);
//             if (force < 6.0e9f)
//                 force = 6.0e9f;
//             force *= 1.1f; // an attempt to prevent very long decay loops

//             fprintf(stderr, "\t\tforce = %e\n", force);

            // calculate the new velocities
            FloatVector2 v0(ce0->GetVelocity() + force / ce0->GetFirstMoment() * N);
            FloatVector2 v1(ce1->GetVelocity() - force / ce1->GetFirstMoment() * N);

            // reset the position of each entity such that its position
            // at the time of collision is the same, but it has the new
            // velocity
            ce0->SetTranslation(ce0->GetTranslation() +
                                collision_dt * (ce0->GetVelocity() - v0));
            ce1->SetTranslation(ce1->GetTranslation() +
                                collision_dt * (ce1->GetVelocity() - v1));

            // update the velocities to the newly calculated ones
            ce0->SetVelocity(v0);
            ce1->SetVelocity(v1);

#if 0
            // calculate the torque
            if (0) // disabled for now
            {

                // get the linear speed of the collision point for each entity
                FloatVector2 B(GetPerpendicularVector2(N));
                FloatVector2 S(((ce0->GetVelocity() - ce1->GetVelocity()) | B) * B -
                          (ce0->GetAngularVelocity() & (R - ce0->GetTranslation())) -
                          (ce1->GetAngularVelocity() & (R - ce1->GetTranslation())));

                // friction constants are 1 right now
                torque = S & N;

                // calculate the new angular velocities
                Float a0 = ce0->GetAngularVelocity() + torque / ce0->GetSecondMoment();
                Float a1 = ce1->GetAngularVelocity() - torque / ce1->GetSecondMoment();

                // reset the angle of each entity such that its angle
                // at the time of collision is the same, but it has the new
                // angular velocity
                ce0->SetRotation(ce0->GetRotation() +
                                 collision_dt * (ce0->GetAngularVelocity() - a0));
                ce1->SetRotation(ce1->GetRotation() +
                                 collision_dt * (ce1->GetAngularVelocity() - a1));

                ce0->SetAngularVelocity(a0);
                ce1->SetAngularVelocity(a1);
            }
#endif // 0
        }
        else
        {
            break;
        }
    }
}

void Engine2::PlanetaryPhysicsHandler::HandleInterpenetrations ()
{
    Entity *ce0;
    Entity *ce1;

    // find any interpenetrating entities and apply separation forces
    EntitySetIterator it0;
    EntitySetIterator it0_end = m_entity_set.end();
    EntitySetIterator it1_begin = m_entity_set.begin();
    EntitySetIterator it1;
    EntitySetIterator it1_end = m_entity_set.end();
    for (it0 = m_entity_set.begin(); it0 != it0_end; ++it0)
    {
        Entity *ent0 = *it0;
        ASSERT1(ent0 != NULL)

        for (it1 = it1_begin; it0 != it1 && it1 != it1_end; ++it1)
        {
            Entity *ent1 = *it1;
            ASSERT1(ent1 != NULL)

            ASSERT1(ent0 != ent1)

            ce0 = ent0;
            ce1 = ent1;

            // check for interpenetrations
            Float r = ce0->GetRadius() + ce1->GetRadius();
            FloatVector2 P = ce0->GetTranslation() - ce1->GetTranslation();
            FloatVector2 V = ce0->GetVelocity() - ce1->GetVelocity();
            Float distance = P.GetLength();

            // if the circles are interpenetrating
            // and if they're moving toward each other
            if (distance < r)// && (V | P) < 0.0f)
            {
//                 fprintf(stderr, "%s/%s interpenetrating\n", ce0->GetName().c_str(), ce1->GetName().c_str());
//                 V.Fprint(stderr);

                Float dt_squared = GetFrameDT()*GetFrameDT();
                Float M = 1.0f / ce0->GetFirstMoment() + 1.0f / ce1->GetFirstMoment();
                FloatVector2 N(P.GetNormalization());
                FloatVector2 Q(P + GetFrameDT()*V);
                FloatVector2 A(dt_squared*M*N);

                Float a = A | A;
                Float b = 2.0f * (Q | A);
                Float c = (Q | Q) - r*r;
                Float discriminant = b*b - 4.0f*a*c;
                Float temp0, temp1;
                Float force0, force1;
                Float min_force, max_force;
                Float force;
                if (discriminant >= 0.0f)
                {
                    temp0 = sqrt(discriminant);
                    temp1 = 2.0f * a;

                    force0 = 0.8f * (-b - temp0) / temp1;
                    force1 = 0.8f * (-b + temp0) / temp1;
                    min_force = Min(force0, force1);
                    max_force = Max(force0, force1);
                    if (min_force > 0.0f)
                        force = min_force;
                    else if (max_force > 0.0f)
                        force = max_force;
                    else
                        force = 0.0f;

                    ce0->AccumulateForce(force*N);
                    ce1->AccumulateForce(-force*N);

#if 0
                    // calculate the torque
                    if (0 && force > 0.0f)
                    {

                        // get the linear speed of the collision point for each entity
                        FloatVector2 R(ce1->GetTranslation() +
                                  distance * ce1->GetRadius() / r * N);
                        FloatVector2 B(GetPerpendicularVector2(N));
                        FloatVector2 S(((ce0->GetVelocity() -
                                  ce1->GetVelocity()) | B) * B -
                                  (ce0->GetAngularVelocity() &
                                  (R - ce0->GetTranslation())) -
                                  (ce1->GetAngularVelocity() &
                                  (R - ce1->GetTranslation())));

                        // friction constants are 1 right now
                        Float torque = N & S;

                        ce0->AccumulateTorque(torque);
                        ce1->AccumulateTorque(-torque);
                    }
#endif // 0
                }
            }
        }
    }
}

void Engine2::PlanetaryPhysicsHandler::CrossProductBetweenGravitationalEntitySets (
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
        Entity *entity0 = *it0;
        ASSERT1(entity0 != NULL)

        for (it1 = it1_begin; it0 != it1 && it1 != it1_end; ++it1)
        {
            Entity *entity1 = *it1;
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
            entity_r_sum_squared *= entity_r_sum_squared * 0.5f * sqrt(2.0f);
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

} // end of namespace Xrb
