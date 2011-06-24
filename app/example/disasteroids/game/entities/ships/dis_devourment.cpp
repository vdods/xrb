// ///////////////////////////////////////////////////////////////////////////
// dis_devourment.cpp by Victor Dods, created 2006/02/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_devourment.hpp"

#include "dis_effect.hpp"
#include "dis_engine.hpp"
#include "dis_healthtrigger.hpp"
#include "dis_powergenerator.hpp"
#include "dis_powerup.hpp"
#include "dis_spawn.hpp"
#include "dis_weapon.hpp"
#include "dis_world.hpp"
#include "xrb_engine2_circle_physicshandler.hpp"
#include "xrb_engine2_objectlayer.hpp"
#include "xrb_polynomial.hpp"

using namespace Xrb;

#define THINK_STATE(x) &Devourment::x

namespace Dis
{

Float const Devourment::ms_max_health[ENEMY_LEVEL_COUNT] = { 1000.0f, 2500.0f, 6250.0f, 15625.0f };
Float const Devourment::ms_engine_thrust[ENEMY_LEVEL_COUNT] = { 20000.0f, 25000.0f, 30000.0f, 35000.0f };
Float const Devourment::ms_wander_speed[ENEMY_LEVEL_COUNT] = { 70.0f, 85.0f, 110.0f, 125.0f };
Float const Devourment::ms_max_angular_velocity[ENEMY_LEVEL_COUNT] = { 90.0f, 90.0f, 90.0f, 90.0f };
Float const Devourment::ms_scale_factor[ENEMY_LEVEL_COUNT] = { 40.0f, 50.0f, 60.0f, 70.0f };
Float const Devourment::ms_baseline_mass[ENEMY_LEVEL_COUNT] = { 1600.0f, 2000.0f, 2400.0f, 2800.0f };
Float const Devourment::ms_damage_dissipation_rate[ENEMY_LEVEL_COUNT] = { 0.5f, 0.7f, 1.2f, 2.5f };
Float const Devourment::ms_mouth_damage_rate[ENEMY_LEVEL_COUNT] = { 10.0f, 25.0f, 50.0f, 80.0f };
Float const Devourment::ms_mouth_tractor_range[ENEMY_LEVEL_COUNT] = { 100.0f, 133.0f, 166.0f, 200.0f };
Float const Devourment::ms_mouth_tractor_strength[ENEMY_LEVEL_COUNT] = { 700.0f, 1200.0f, 1800.0f, 2500.0f };
Float const Devourment::ms_mouth_tractor_max_force[ENEMY_LEVEL_COUNT] = { 2000000.0f, 2000000.0f, 2000000.0f, 2000000.0f };
Float const Devourment::ms_mouth_tractor_beam_radius[ENEMY_LEVEL_COUNT] = { 80.0f, 100.0f, 120.0f, 140.0f };
Float const Devourment::ms_max_spawn_mineral_mass[ENEMY_LEVEL_COUNT] = { 100.0f, 180.0f, 250.0f, 400.0f };
Float const Devourment::ms_max_single_mineral_mass[ENEMY_LEVEL_COUNT] = { 30.0f, 40.0f, 50.0f, 60.0f };
Float const Devourment::ms_health_powerup_amount_to_spawn[ENEMY_LEVEL_COUNT] = { 8.0f, 10.0f, 12.0f, 15.0f };

Devourment::Devourment (Uint8 const enemy_level)
    :
    EnemyShip(enemy_level, ms_max_health[enemy_level], ET_DEVOURMENT)
{
    // Devourment needs to be more sensitive to damage with respect
    // to damage/healing flashes.  the default is 0.2, but we need
    // something much lower.
    SetFullFlashIntensityHealthRatio(0.01f);

    m_think_state = THINK_STATE(PickWanderDirection);

    // Devourment is weak against mining laser (sort of an association between
    // it and the asteroids' weakness against mining laser).
    SetWeakness(D_MINING_LASER);
    SetImmunity(D_COLLISION|D_GRINDING);
    SetDamageDissipationRate(ms_damage_dissipation_rate[EnemyLevel()]);

    m_mouth_tractor = new Tractor(0);
    m_mouth_tractor->SetRangeOverride(ms_mouth_tractor_range[EnemyLevel()]);
    m_mouth_tractor->SetStrengthOverride(ms_mouth_tractor_strength[EnemyLevel()]);
    m_mouth_tractor->SetMaxForceOverride(ms_mouth_tractor_max_force[EnemyLevel()]);
    m_mouth_tractor->SetBeamRadiusOverride(ms_mouth_tractor_beam_radius[EnemyLevel()]);
    m_mouth_tractor->Equip(this);

    for (Uint32 i = 0; i < MINERAL_COUNT; ++i)
        m_mineral_inventory[i] = 0.0f;
}

Devourment::~Devourment ()
{
    if (m_mouth_health_trigger.IsValid())
    {
        if (m_mouth_health_trigger->IsInWorld())
            m_mouth_health_trigger->RemoveFromWorld();
        delete m_mouth_health_trigger->OwnerObject();
    }

    ASSERT1(m_mouth_tractor != NULL);
    Delete(m_mouth_tractor);

    if (m_mouth_tractor_beam.IsValid())
    {
        if (m_mouth_tractor_beam->IsInWorld())
            m_mouth_tractor_beam->RemoveFromWorld();
        delete m_mouth_tractor_beam->OwnerObject();
    }
}

void Devourment::Think (Float const time, Float const frame_dt)
{
    // can't think if we're dead.
    if (IsDead())
        return;

    Ship::Think(time, frame_dt);

    // NOTE: Devourment can't be disabled -- the
    // disabled code would go here otherwise

    // if the mouth health trigger has not yet been created, create it.
    if (!m_mouth_health_trigger.IsValid())
    {
        HealthTrigger *health_trigger =
            SpawnDevourmentMouthHealthTrigger(
                GetObjectLayer(),
                Translation(),
                0.72f * ScaleFactor(),
                FloatVector2::ms_zero, // moot, since we must move it ourselves,
                -ms_mouth_damage_rate[EnemyLevel()],
                Mortal::D_GRINDING,
                GetReference(),
                GetReference(),
                EnemyLevel());
        m_mouth_health_trigger = health_trigger->GetReference();
        // set the mouth damage rate
        m_mouth_health_trigger->SetHealthDeltaRate(-ms_mouth_damage_rate[EnemyLevel()]);
    }

    // the grinder sprite's color bias (i.e. damage/healing flashing)
    // must be matched here, since it was updated above in Ship::Think().
    m_mouth_health_trigger->OwnerObject()->ColorBias() = OwnerObject()->ColorBias();

    // ensure the tractor beam is allocated (lazy allocation)
    if (!m_mouth_tractor_beam.IsValid())
        m_mouth_tractor_beam = SpawnTractorBeam(GetObjectLayer())->GetReference();
    // if the tractor beam is already allocated but not in the world, re-add it.
    else if (!m_mouth_tractor_beam->IsInWorld())
        m_mouth_tractor_beam->AddBackIntoWorld();
    // set the tractor beam effect in the Tractor weapon
    m_mouth_tractor->SetTractorBeam(*m_mouth_tractor_beam);

    SetReticleCoordinates(Translation() + Math::UnitVector(Angle()));

    // call the think state function (which will set the inputs)
    (this->*m_think_state)(time, frame_dt);

    // since enemy ships do not use the PlayerShip device code, engines
    // weapons, etc must be activated/simulated manually here.

    AimShipAtReticleCoordinates(frame_dt);
    // if we're not facing the target nearly enough, set the tractor inputs to 0
    {
        static Float const s_max_tractor_angle = 15.0f;

        FloatVector2 target_direction(ReticleCoordinates() - Translation());
        if (!target_direction.IsZero())
        {
            Float angle_delta = Math::CanonicalAngle(Math::Atan(target_direction) - Angle());
            if (Abs(angle_delta) > s_max_tractor_angle)
            {
                SetWeaponPrimaryInput(0);
                SetWeaponSecondaryInput(0);
            }
        }
    }
    // set the weapon inputs and activate
    m_mouth_tractor->SetInputs(
        NormalizedWeaponPrimaryInput(),
        NormalizedWeaponSecondaryInput(),
        MuzzleLocation(m_mouth_tractor),
        MuzzleDirection(m_mouth_tractor),
        ReticleCoordinates());
    m_mouth_tractor->Activate(
        m_mouth_tractor->PowerToBeUsedBasedOnInputs(time, frame_dt),
        time,
        frame_dt);

    ResetInputs();

    // set the translation and velocity of the mouth health trigger
    ASSERT1(m_mouth_health_trigger.IsValid());
    m_mouth_health_trigger->SetTranslation(Translation() + 0.48f * ScaleFactor() * Math::UnitVector(Angle()));
    m_mouth_health_trigger->SetAngle(Angle());
    m_mouth_health_trigger->SetVelocity(Velocity());
}

void Devourment::Collide (
    Entity *collider,
    FloatVector2 const &collision_location,
    FloatVector2 const &collision_normal,
    Float collision_force,
    Float time,
    Float frame_dt)
{
    Mortal::Collide(
        collider,
        collision_location,
        collision_normal,
        collision_force,
        time,
        frame_dt);

    ASSERT1(collider != NULL);
    if (collider->IsMortal() &&
        collider->GetEntityType() != ET_DEVOURMENT &&
        collider->GetEntityType() != ET_DEMI &&
        m_think_state == THINK_STATE(PickWanderDirection))
    {
        m_target = collider->GetReference();
        m_think_state = THINK_STATE(Pursue);
        m_next_whatever_time = time + 3.0f;
        SetReticleCoordinates(m_target->Translation());
    }
}

void Devourment::Die (
    Entity *const killer,
    Entity *const kill_medium,
    FloatVector2 const &kill_location,
    FloatVector2 const &kill_normal,
    Float const kill_force,
    DamageType const kill_type,
    Float const time,
    Float const frame_dt)
{
    EnemyShip::Die(
        killer,
        kill_medium,
        kill_location,
        kill_normal,
        kill_force,
        kill_type,
        time,
        frame_dt);

    // remove the mouth tractor beam, if it exists
    if (m_mouth_tractor_beam.IsValid() && m_mouth_tractor_beam->IsInWorld())
        m_mouth_tractor_beam->ScheduleForRemovalFromWorld(0.0f);

    // adjust the minerals so their sum isn't greater than the max spawn mineral mass
    Float mineral_mass_total = 0.0f;
    for (Uint32 i = 0; i < MINERAL_COUNT; ++i)
        mineral_mass_total += m_mineral_inventory[i];
    if (mineral_mass_total > ms_max_spawn_mineral_mass[EnemyLevel()])
        for (Uint32 i = 0; i < MINERAL_COUNT; ++i)
            m_mineral_inventory[i] = ms_max_spawn_mineral_mass[EnemyLevel()] * m_mineral_inventory[i] / mineral_mass_total;

    // spawn the collected minerals
    static Float const s_min_powerup_mass = 5.0f;
    static Float const s_powerup_ejection_speed = 100.0f;

    for (Uint32 mineral_index = 0; mineral_index < MINERAL_COUNT; ++mineral_index)
    {
        // if the value is negative, obviously no minerals will be spawned
        while (m_mineral_inventory[mineral_index] > s_min_powerup_mass)
        {
            Float mass =
                Math::RandomFloat(
                    s_min_powerup_mass,
                    Min(ms_max_single_mineral_mass[EnemyLevel()], m_mineral_inventory[mineral_index]));
            Float scale_factor = Math::Sqrt(mass);
            Float velocity_angle = Math::RandomAngle();
            Float velocity_ratio = Math::RandomFloat(scale_factor, 0.5f * ScaleFactor()) / (0.5f * ScaleFactor());
            FloatVector2 velocity = Velocity() + s_powerup_ejection_speed * velocity_ratio * Math::UnitVector(velocity_angle);

            SpawnPowerup(
                GetObjectLayer(),
                Translation() + 0.5f * ScaleFactor() * velocity_ratio * Math::UnitVector(velocity_angle),
                scale_factor,
                mass,
                velocity,
                Item::MineralSpritePath(mineral_index),
                static_cast<ItemType>(IT_MINERAL_LOWEST+mineral_index));

            m_mineral_inventory[mineral_index] -= mass;
        }
    }

    static Float const s_min_powerup_amount = 1.0f;
    static Float const s_max_powerup_amount = 3.0f;
    static Float const s_powerup_coefficient = 0.1f;

    Float health_powerup_amount_left_to_spawn =
        Min(mineral_mass_total, ms_max_spawn_mineral_mass[EnemyLevel()]) / ms_max_spawn_mineral_mass[EnemyLevel()] *
        ms_health_powerup_amount_to_spawn[EnemyLevel()];
    while (health_powerup_amount_left_to_spawn > s_min_powerup_amount)
    {
        Float health_powerup_amount =
            Math::RandomFloat(
                s_min_powerup_amount,
                Min(s_max_powerup_amount, health_powerup_amount_left_to_spawn));
        Float mass = health_powerup_amount / s_powerup_coefficient;
        Float scale_factor = Math::Sqrt(mass);
        Float velocity_angle = Math::RandomAngle();
        Float velocity_ratio = Math::RandomFloat(scale_factor, 0.5f * ScaleFactor()) / (0.5f * ScaleFactor());
        FloatVector2 velocity = Velocity() + s_powerup_ejection_speed * velocity_ratio * Math::UnitVector(velocity_angle);

        Powerup *health_powerup =
            SpawnPowerup(
                GetObjectLayer(),
                Translation() + 0.5f * ScaleFactor() * velocity_ratio * Math::UnitVector(velocity_angle),
                scale_factor,
                mass,
                velocity,
                "resources/powerup.png",
                IT_POWERUP_HEALTH);
        health_powerup->SetEffectiveValue(health_powerup_amount);

        health_powerup_amount_left_to_spawn -= health_powerup_amount;
    }
}

bool Devourment::TakePowerup (Powerup *const powerup, Float const time, Float const frame_dt)
{
    ASSERT1(powerup != NULL);

    // just suck up all powerups, to piss off the player

    // add minerals to the inventory
    if (powerup->GetItemType() >= IT_MINERAL_LOWEST &&
        powerup->GetItemType() <= IT_MINERAL_HIGHEST)
    {
        ASSERT1(powerup->GetItem() == NULL);
        Uint32 mineral_index = powerup->GetItemType() - IT_MINERAL_LOWEST;
        ASSERT1(mineral_index < MINERAL_COUNT);
        m_mineral_inventory[mineral_index] += powerup->EffectiveValue();
    }
    // health powerups heal
    else if (powerup->GetItemType() == IT_POWERUP_HEALTH)
    {
        ASSERT1(powerup->GetItem() == NULL);
        Heal(
            powerup,
            powerup,
            powerup->EffectiveValue(),
            (Mass()*powerup->Translation() + powerup->Mass()*Translation()) /
                (Mass() + powerup->Mass()),
            (Translation() - powerup->Translation()).Normalization(),
            0.0f,
            time,
            frame_dt);
    }

    delete powerup->GetItem();
    powerup->ClearItem();
    return true;
}

void Devourment::SetTarget (Mortal *const target)
{
    if (target == NULL)
        m_target.Release();
    else
    {
        m_target = target->GetReference();
        m_think_state = THINK_STATE(Pursue);
    }
}

void Devourment::PickWanderDirection (Float const time, Float const frame_dt)
{
    // update the next time to pick a wander direction
    m_next_whatever_time = time + 20.0f;
    // pick a direction/speed to wander in
    m_wander_angle = Math::RandomAngle();
    // transition to Wander
    m_think_state = THINK_STATE(Wander);
}

void Devourment::Wander (Float const time, Float const frame_dt)
{
    m_target = ScanAreaForTargets();

    // if we acquired a target, transition to Pursue
    if (m_target.IsValid())
    {
        m_think_state = THINK_STATE(Pursue);
        m_next_whatever_time = time + 3.0f;
        SetReticleCoordinates(m_target->Translation());
    }
    // otherwise handle wandering
    else
    {
        FloatVector2 wander_velocity(ms_wander_speed[EnemyLevel()] * Math::UnitVector(m_wander_angle));
        MatchVelocity(wander_velocity, frame_dt);

        if (time >= m_next_whatever_time)
            m_think_state = THINK_STATE(PickWanderDirection);
    }
}

void Devourment::Pursue (Float const time, Float const frame_dt)
{
    if (time >= m_next_whatever_time)
        m_target = ScanAreaForTargets();

    if (!m_target.IsValid() || (m_target->IsMortal() && DStaticCast<Mortal *>(*m_target)->IsDead()))
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        SetReticleCoordinates(Translation() + Math::UnitVector(Angle()));
        return;
    }

    SetReticleCoordinates(m_target->Translation());

    FloatVector2 position_delta = GetObjectLayer()->AdjustedDifference(m_target->Translation(), Translation());

    // if we're close enough to the target, transition to Consume
    static Float const s_consume_distance = 30.0f;
    Float const give_up_distance =
        ms_mouth_tractor_range[EnemyLevel()] +
        0.5f * ms_mouth_tractor_beam_radius[EnemyLevel()] +
        ScaleFactor();

    Float target_distance = position_delta.Length();
    if (target_distance <= s_consume_distance + ScaleFactor() + m_target->ScaleFactor())
    {
        m_think_state = THINK_STATE(Consume);
        m_next_whatever_time = time + 3.0f;
        return;
    }
    else if (target_distance > give_up_distance)
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        SetReticleCoordinates(Translation() + Math::UnitVector(Angle()));
        return;
    }

    // adjust our course to hit the target

    // TODO: expensive polynomial solving shouldn't happen every single frame

    Float interceptor_acceleration =
        ms_engine_thrust[EnemyLevel()] / Mass();
    FloatVector2 const &p = position_delta;
    FloatVector2 v(m_target->Velocity() - Velocity());
    FloatVector2 a(m_target->Force() / m_target->Mass());

    Polynomial poly;
    poly.Set(4, a.LengthSquared() - interceptor_acceleration*interceptor_acceleration);
    poly.Set(3, 4.0f * (a | v));
    poly.Set(2, 4.0f * ((a | p) + (v | v)));
    poly.Set(1, 8.0f * (p | v));
    poly.Set(0, 4.0f * (p | p));

    Polynomial::SolutionSet solution_set;
    poly.Solve(&solution_set, 0.001f);

    Float T = -1.0f;
    for (Polynomial::SolutionSet::iterator it = solution_set.begin(),
                                         it_end = solution_set.end();
         it != it_end;
         ++it)
    {
        if (*it >= 0.0f)
        {
            T = *it;
            break;
        }
    }

    // activate the mouth tractor
    SetWeaponPrimaryInput(UINT8_UPPER_BOUND);
    SetWeaponSecondaryInput(0);
    if (T <= 0.0f)
    {
        // if no acceptable solution, just do dumb approach
        AccumulateForce(ms_engine_thrust[EnemyLevel()] * position_delta.Normalization());
    }
    else
    {
        // accelerate towards the target
        FloatVector2 calculated_acceleration((2.0f*p + 2.0f*v*T + a*T*T) / (T*T));
        AccumulateForce(calculated_acceleration * Mass());
    }
}

void Devourment::Consume (Float const time, Float const frame_dt)
{
    if (time >= m_next_whatever_time)
        m_target = ScanAreaForTargets();

    if (!m_target.IsValid() || (m_target->IsMortal() && DStaticCast<Mortal *>(*m_target)->IsDead()))
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        SetReticleCoordinates(Translation() + Math::UnitVector(Angle()));
        return;
    }

    // set the reticle coordinates and aim the mouth tractor
    SetReticleCoordinates(m_target->Translation());
    SetWeaponPrimaryInput(UINT8_UPPER_BOUND);
    SetWeaponSecondaryInput(0);

    // if we're no longer close enough to the target, transition to Pursue
    Float target_distance = GetObjectLayer()->AdjustedDistance(m_target->Translation(), Translation());
    Float const pursue_distance = 40.0f + ScaleFactor();
    if (target_distance > pursue_distance + m_target->ScaleFactor())
    {
        m_think_state = THINK_STATE(Pursue);
        m_next_whatever_time = time + 3.0f;
        return;
    }

    // we want drag the target to a stop, otherwise there will
    // be Devourments flying all over the joint
    if (!Velocity().IsZero())
    {
        FloatVector2 braking_thrust(-ms_engine_thrust[EnemyLevel()] * Velocity().Normalization());
        AccumulateForce(braking_thrust);
    }
}

void Devourment::MatchVelocity (FloatVector2 const &velocity, Float const frame_dt)
{
    // calculate what thrust is required to match the desired velocity
    FloatVector2 velocity_differential =
        velocity - (Velocity() + frame_dt * Force() / Mass());
    FloatVector2 thrust_vector = Mass() * velocity_differential / frame_dt;
    if (!thrust_vector.IsZero())
    {
        Float thrust_force = thrust_vector.Length();
        if (thrust_force > ms_engine_thrust[EnemyLevel()])
            thrust_vector = ms_engine_thrust[EnemyLevel()] * thrust_vector.Normalization();

        SetReticleCoordinates(Translation() + thrust_vector.Normalization());
        AccumulateForce(thrust_vector);
    }
}

EntityReference<Entity> Devourment::ScanAreaForTargets ()
{
    EntityReference<Entity> target;

    static Float const s_optimal_target_mass = 400.0f;

    Float scan_radius =
        ms_mouth_tractor_range[EnemyLevel()] +
        ScaleFactor();

    // scan area for targets
    Engine2::Circle::AreaTraceList area_trace_list;
    GetPhysicsHandler()->AreaTrace(
        GetObjectLayer(),
        Translation(),
        scan_radius,
        false,
        area_trace_list);

    static Float const s_powerup_mass_threshold = 15.0f;
    for (Engine2::Circle::AreaTraceList::iterator it = area_trace_list.begin(),
                                                  it_end = area_trace_list.end();
         it != it_end;
         ++it)
    {
        Entity *entity = DStaticCast<Entity *>(*it);
        ASSERT1(entity != NULL);
        ASSERT2(*entity->GetReference() == entity);

        // don't try to eat ourselves
        if (entity == this)
            continue;

        // don't seek other Devourments or Demis
        if (entity->GetEntityType() == ET_DEVOURMENT ||
            entity->GetEntityType() == ET_DEMI)
        {
            continue;
        }

        // large-enough powerups trump everything else
        if (entity->IsPowerup())
        {
            if (!target.IsValid()
                ||
                !target->IsPowerup()
                ||
                (entity->Mass() > s_powerup_mass_threshold &&
                 entity->Mass() > target->Mass()))
            {
                target = entity->GetReference();
            }
        }
        else if (entity->IsMortal())
        {
            ASSERT1(dynamic_cast<Mortal *>(entity) != NULL);

            // if no target, use the current game object,
            // or override the current target if the current game object is
            // closer to the optimal mass.  the -0.01f bidness is so if there
            // are multiple potential targets of the exact same mass, we don't
            // switch between them really fast between frames.
            if (!target.IsValid() ||
                Abs(entity->Mass() - s_optimal_target_mass) <
                Abs(target->Mass() - s_optimal_target_mass) - 0.01f)
            {
                target = entity->GetReference();
            }
        }
    }

    return target;
}

} // end of namespace Dis
