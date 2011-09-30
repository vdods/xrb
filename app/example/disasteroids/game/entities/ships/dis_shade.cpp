// ///////////////////////////////////////////////////////////////////////////
// dis_shade.cpp by Victor Dods, created 2005/12/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_shade.hpp"

#include "dis_effect.hpp"
#include "dis_engine.hpp"
#include "dis_powergenerator.hpp"
#include "dis_spawn.hpp"
#include "dis_weapon.hpp"
#include "xrb_engine2_circle_physicshandler.hpp"
#include "xrb_engine2_objectlayer.hpp"
#include "xrb_engine2_sprite.hpp"
#include "xrb_polynomial.hpp"

using namespace Xrb;

#define THINK_STATE(x) &Shade::x

namespace Dis
{

Float const Shade::ms_max_health[ENEMY_LEVEL_COUNT] = { 20.0f, 80.0f, 320.0f, 1280.0f };
Float const Shade::ms_engine_thrust[ENEMY_LEVEL_COUNT] = { 8000.0f, 9000.0f, 11000.0f, 14000.0f };
Float const Shade::ms_max_angular_velocity[ENEMY_LEVEL_COUNT] = { 720.0f, 720.0f, 720.0f, 720.0f };
Float const Shade::ms_ship_radius[ENEMY_LEVEL_COUNT] = { 9.0f, 11.0f, 13.0f, 15.0f };
Float const Shade::ms_baseline_mass[ENEMY_LEVEL_COUNT] = { 140.0f, 140.0f, 140.0f, 140.0f };
Float const Shade::ms_damage_dissipation_rate[ENEMY_LEVEL_COUNT] = { 0.5f, 1.0f, 2.0f, 4.0f };
Float const Shade::ms_alarm_distance[ENEMY_LEVEL_COUNT] = { 50.0f, 75.0f, 100.0f, 125.0f };
Float const Shade::ms_stalk_minimum_distance[ENEMY_LEVEL_COUNT] = { 80.0f, 100.0f, 125.0f, 150.0f };
Float const Shade::ms_stalk_maximum_distance[ENEMY_LEVEL_COUNT] = { 130.0f, 150.0f, 175.0f, 200.0f };
Float const Shade::ms_move_relative_velocity[ENEMY_LEVEL_COUNT] = { 50.0f, 60.0f, 70.0f, 80.0f };
Float const Shade::ms_wander_speed[ENEMY_LEVEL_COUNT] = { 70.0f, 80.0f, 90.0f, 100.0f };
Float const Shade::ms_circling_speed[ENEMY_LEVEL_COUNT] = { 20.0f, 40.0f, 80.0f, 120.0f };
Float const Shade::ms_in_crosshairs_teleport_time[ENEMY_LEVEL_COUNT] = { 1.0f, 1.0f, 1.0f, 1.0f };
Float const Shade::ms_teleportation_duration[ENEMY_LEVEL_COUNT] = { 0.25f, 0.25f, 0.25f, 0.25f };

Shade::Shade (Uint8 enemy_level)
    :
    EnemyShip(enemy_level, ms_max_health[enemy_level], ET_SHADE)
{
    m_think_state = THINK_STATE(Teleport); // start invisible (see HandleNewOwnerObject), teleport in
    m_saved_state = THINK_STATE(PickWanderDirection);

    m_in_crosshairs = false;

    m_weapon = new SlowBulletGun(EnemyLevel());
    m_weapon->Equip(this);

    SetStrength(D_MINING_LASER|D_COLLISION);
    SetDamageDissipationRate(ms_damage_dissipation_rate[EnemyLevel()]);
}

Shade::~Shade ()
{
    ASSERT1(m_weapon != NULL);
    Delete(m_weapon);
}

void Shade::Think (Float time, Float frame_dt)
{
    // can't think if we're dead.
    if (IsDead())
        return;

    bool is_disabled = IsDisabled();
    Ship::Think(time, frame_dt);
    if (is_disabled)
    {
        // if disabled, then reset the think state to PickWanderDirection (a way out for
        // players that are being ganged up on)
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }

    // call the think state function (which will set the inputs)
    (this->*m_think_state)(time, frame_dt);

    // since enemy ships do not use the PlayerShip device code, engines
    // weapons, etc must be activated/simulated manually here.

    // we don't aim the ship, because it stays at the same angle all the time
    m_weapon->SetInputs(
        NormalizedWeaponPrimaryInput(),
        NormalizedWeaponSecondaryInput(),
        MuzzleLocation(m_weapon),
        MuzzleDirection(m_weapon),
        ReticleCoordinates());
    m_weapon->Activate(
        m_weapon->PowerToBeUsedBasedOnInputs(false, false, time, frame_dt),
        false, // no attack boost
        false, // no defense boost
        time,
        frame_dt);

    ResetInputs();
}

FloatVector2 Shade::MuzzleLocation (Weapon const *weapon) const
{
    ASSERT1(weapon != NULL);
    FloatVector2 reticle_direction(GetObjectLayer()->AdjustedDifference(ReticleCoordinates(), Translation()));
    reticle_direction.Normalize();
    return Translation() + VisibleRadius() * reticle_direction;
}

FloatVector2 Shade::MuzzleDirection (Weapon const *weapon) const
{
    ASSERT1(weapon != NULL);
    FloatVector2 reticle_direction(GetObjectLayer()->AdjustedDifference(ReticleCoordinates(), Translation()));
    reticle_direction.Normalize();
    return reticle_direction;
}

void Shade::SetTarget (Mortal *target)
{
    if (target == NULL)
        m_target.Release();
    else
    {
        m_target = target->GetReference();
        m_think_state = THINK_STATE(Teleport);
        m_saved_state = THINK_STATE(MoveToAttackRange);
    }
}

void Shade::HandleNewOwnerObject ()
{
    EnemyShip::HandleNewOwnerObject();

    ASSERT1(OwnerObject() != NULL);
    OwnerObject()->ColorMask() = Color::ms_transparent_white;
}

void Shade::PickWanderDirection (Float time, Float frame_dt)
{
    // update the next time to pick a wander direction
    m_next_whatever_time = time + 6.0f;
    // pick a direction/speed to wander in
    m_wander_angle = Math::RandomAngle();
    m_think_state = THINK_STATE(Wander);
}

void Shade::Wander (Float time, Float frame_dt)
{
    static Float const s_scan_radius = 200.0f;
    static Float const s_collision_lookahead_time = 3.0f;

    // scan area for targets
    Engine2::Circle::AreaTraceList area_trace_list;
    GetPhysicsHandler()->AreaTrace(
        GetObjectLayer(),
        Translation(),
        s_scan_radius,
        false,
        area_trace_list);
    // check the area trace list for targets and collisions
    Float collision_time = -1.0f;
    Entity *collision_entity = NULL;
    for (Engine2::Circle::AreaTraceList::iterator it = area_trace_list.begin(),
                                                  it_end = area_trace_list.end();
         it != it_end;
         ++it)
    {
        Entity *entity = DStaticCast<Entity *>(*it);
        ASSERT1(entity != NULL);

        // ignore ourselves
        if (entity == this)
            continue;

        // if this entity is a solitary, set m_target and transition
        // to MoveToAttackRange
        if (entity->GetEntityType() == ET_SOLITARY)
        {
            m_target = entity->GetReference();
            m_think_state = THINK_STATE(MoveToAttackRange);
            return;
        }
        // otherwise if we will collide with something in the next short
        // while, perform collision avoidance calculations
        else
        {
            Float potential_collision_time = CollisionTime(entity, s_collision_lookahead_time);
            if (potential_collision_time >= 0.0f &&
                (collision_entity == NULL || potential_collision_time < collision_time))
            {
                collision_time = potential_collision_time;
                collision_entity = entity;
            }
        }
    }

    // if there is an imminent collision, pick a new direction to avoid it
    if (collision_entity != NULL)
    {
        FloatVector2 delta_velocity(collision_entity->Velocity() - Velocity());
        FloatVector2 perpendicular_velocity(PerpendicularVector2(delta_velocity));
        ASSERT1(!perpendicular_velocity.IsZero());
        if ((perpendicular_velocity | Velocity()) > -(perpendicular_velocity | Velocity()))
            m_wander_angle = Math::Arg(perpendicular_velocity);
        else
            m_wander_angle = Math::Arg(-perpendicular_velocity);
        m_next_whatever_time = time + 6.0f;
    }

    // incrementally accelerate up to the wander direction/speed
    FloatVector2 wander_velocity(ms_wander_speed[EnemyLevel()] * Math::UnitVector(m_wander_angle));
    MatchVelocity(wander_velocity, frame_dt);

    if (time >= m_next_whatever_time)
        m_think_state = THINK_STATE(PickWanderDirection);
}

void Shade::Stalk (Float time, Float frame_dt)
{
    if (!m_target.IsValid() || m_target->IsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        m_in_crosshairs = false; // reset when leaving this state
        return;
    }

    FloatVector2 target_position_delta(GetObjectLayer()->AdjustedDifference(m_target->Translation(), Translation()));
    Float distance_to_target = target_position_delta.Length();

    ProcessInCrosshairsState(target_position_delta, distance_to_target, time);

    ASSERT1(ms_alarm_distance[EnemyLevel()] < ms_stalk_minimum_distance[EnemyLevel()]);
    ASSERT1(ms_stalk_minimum_distance[EnemyLevel()] < ms_stalk_maximum_distance[EnemyLevel()]);
    // if we're inside the alarm distance, or if the target has been aiming at us for
    // long enough, then teleport away
    if (distance_to_target < ms_alarm_distance[EnemyLevel()]
        ||
        (m_in_crosshairs && time - m_in_crosshairs_start_time > ms_in_crosshairs_teleport_time[EnemyLevel()]))
    {
        m_saved_state = THINK_STATE(Stalk); // save the current state
        m_think_state = THINK_STATE(Teleport);
        m_in_crosshairs = false; // reset when leaving this state
        return;
    }
    // if we're not within the stalk donut, move to attack range
    else if (distance_to_target < ms_stalk_minimum_distance[EnemyLevel()] ||
             distance_to_target > ms_stalk_maximum_distance[EnemyLevel()])
    {
        m_think_state = THINK_STATE(MoveToAttackRange);
        m_in_crosshairs = false; // reset when leaving this state
        return;
    }

    // figure out the direction to circle
    FloatVector2 circling_direction(PerpendicularVector2(target_position_delta).Normalization());
    // try to attain the velocity necessary for circling the target
    MatchVelocity(m_target->Velocity() + ms_circling_speed[EnemyLevel()]*circling_direction, frame_dt);

    AimWeaponAtTarget();
//     SetWeaponPrimaryInput(UINT8_UPPER_BOUND);
}

void Shade::MoveToAttackRange (Float time, Float frame_dt)
{
    if (!m_target.IsValid() || m_target->IsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        m_in_crosshairs = false; // reset when leaving this state
        return;
    }

    FloatVector2 target_position_delta(GetObjectLayer()->AdjustedDifference(m_target->Translation(), Translation()));
    Float distance_to_target = target_position_delta.Length();

    ProcessInCrosshairsState(target_position_delta, distance_to_target, time);

    ASSERT1(ms_stalk_minimum_distance[EnemyLevel()] < ms_stalk_maximum_distance[EnemyLevel()]);
    // if we're inside the alarm distance, or if the target has been aiming at us for
    // long enough, then teleport away
    if (distance_to_target < ms_alarm_distance[EnemyLevel()]
        ||
        (m_in_crosshairs && time - m_in_crosshairs_start_time > ms_in_crosshairs_teleport_time[EnemyLevel()]))
    {
        m_saved_state = THINK_STATE(MoveToAttackRange); // save the current state
        m_think_state = THINK_STATE(Teleport);
        m_in_crosshairs = false; // reset when leaving this state
        return;
    }
    // check if we want to move away from the target
    else if (distance_to_target <= 0.75f * ms_stalk_minimum_distance[EnemyLevel()] + 0.25f * ms_stalk_maximum_distance[EnemyLevel()])
    {
        FloatVector2 velocity_delta(Velocity() - m_target->Velocity());
        FloatVector2 desired_velocity_delta(-ms_move_relative_velocity[EnemyLevel()] * target_position_delta.Normalization());
        FloatVector2 thrust_vector((desired_velocity_delta - velocity_delta) * Mass());
        if (thrust_vector.Length() > ms_engine_thrust[EnemyLevel()])
        {
            thrust_vector.Normalize();
            thrust_vector *= ms_engine_thrust[EnemyLevel()];
        }
        AccumulateForce(thrust_vector);

        AimWeaponAtTarget();
//         if (distance_to_target >= ms_stalk_minimum_distance[EnemyLevel()] &&
//             distance_to_target <= ms_stalk_maximum_distance[EnemyLevel()])
//         {
//             SetWeaponPrimaryInput(UINT8_UPPER_BOUND);
//         }
    }
    // check if we want to move away from the target
    else if (distance_to_target >= 0.25f * ms_stalk_minimum_distance[EnemyLevel()] + 0.75f * ms_stalk_maximum_distance[EnemyLevel()])
    {
        FloatVector2 velocity_delta(Velocity() - m_target->Velocity());
        FloatVector2 desired_velocity_delta(ms_move_relative_velocity[EnemyLevel()] * target_position_delta.Normalization());
        FloatVector2 thrust_vector((desired_velocity_delta - velocity_delta) * Mass());
        if (thrust_vector.Length() > ms_engine_thrust[EnemyLevel()])
        {
            thrust_vector.Normalize();
            thrust_vector *= ms_engine_thrust[EnemyLevel()];
        }
        AccumulateForce(thrust_vector);

        AimWeaponAtTarget();
//         if (distance_to_target >= ms_stalk_minimum_distance[EnemyLevel()] &&
//             distance_to_target <= ms_stalk_maximum_distance[EnemyLevel()])
//         {
//             SetWeaponPrimaryInput(UINT8_UPPER_BOUND);
//         }
    }
    // otherwise we're along the ring of the attack donut, so transition to Stalk.
    else
    {
        m_think_state = THINK_STATE(Stalk);
        // don't reset in-crosshairs state
    }
}

void Shade::Teleport (Float time, Float frame_dt)
{
    ASSERT1(OwnerObject() != NULL);

    // check if we're invisible (should be created invisible)
    bool starting_invisible = (OwnerObject()->ColorMask() == Color::ms_transparent_white);

    // make a few attempts to find a nearby place to teleport to.
    FloatVector2 teleport_destination;
    {
        Uint32 placement_attempt_count = 0;
        static Uint32 placement_attempt_max = 10;
        bool overlap;
        do
        {
            // if we've tried to many times without success, just return, and
            // we can try again in the next call of this function, next frame.
            if (placement_attempt_count == placement_attempt_max)
                return;
            ++placement_attempt_count;

            FloatVector2 teleport_range_center(Translation());
            if (m_target.IsValid())
                teleport_range_center = m_target->Translation();
            teleport_destination = ms_stalk_maximum_distance[EnemyLevel()] * Math::UnitVector(Math::RandomAngle()) + teleport_range_center;
            overlap =
                GetPhysicsHandler()->
                    DoesAreaOverlapAnyEntityInObjectLayer(
                        GetObjectLayer(),
                        teleport_destination,
                        1.5f * VisibleRadius(),
                        false);
        }
        while (overlap);

        // set it back to visible (could still be invisible from having been
        // created invisible).
        OwnerObject()->ColorMask() = Color::ms_opaque_white;
    }

    ASSERT1(ms_teleportation_duration[EnemyLevel()] > 0.0f);

    // effects at the current location
    if (!starting_invisible)
    {
        // spawn a teleportation effect (the shade will appear to shrink to a point and disappear)
        {
            ASSERT1(OwnerObject()->GetObjectType() == Engine2::OT_SPRITE);
            std::string sprite_path = static_cast<Engine2::Sprite *>(OwnerObject())->GetTexture().LoadParameters().As<GlTexture::LoadParameters>().Path();
            Explosion *shrinking_effect =
                SpawnExplosion(
                    GetObjectLayer(),
                    sprite_path,
                    time,
                    Translation(),
                    Velocity(),
                    Angle(),
                    VisibleRadius(),
                    0.1f, // 0 isn't allowed
                    0.25f,
                    time,
                    ET_EXPLOSION,
                    Engine2::Circle::CT_SOLID_COLLISION);
            shrinking_effect->SetMass(Mass());
            shrinking_effect->SetScaleInterpolationPower(4.0f);
            shrinking_effect->FinalColorMask() = Color::ms_opaque_white;
        }

        // spawn a fast shock wave, something like entering hyperspace
        {
            NoDamageExplosion *shockwave =
                SpawnNoDamageExplosion(
                    GetObjectLayer(),
                    ExplosionAssetPath(EXPLO_SHOCKWAVE),
                    time,
                    Translation(),
                    Velocity(),
                    VisibleRadius(),        // initial_size
                    4.0f * VisibleRadius(), // final_size
                    ms_teleportation_duration[EnemyLevel()],
                    time);
            shockwave->InitialColorMask() = Color(0.3f, 0.0f, 0.45f, 1.0f);
            shockwave->FinalColorMask() = Color(0.3f, 0.0f, 0.45f, 0.0f);
            shockwave->SetScaleInterpolationPower(2.0f); // squared scale interpolation
        }
    }

    FloatVector2 target_velocity(AmbientVelocity(100.0f));

    // effects at the target location
    {
        // spawn a fast shock wave, something like leaving hyperspace
        // this should be exactly the reverse of the above one.
        {
            NoDamageExplosion *shockwave =
                SpawnNoDamageExplosion(
                    GetObjectLayer(),
                    ExplosionAssetPath(EXPLO_SHOCKWAVE),
                    time,
                    teleport_destination,
                    target_velocity,
                    VisibleRadius(),        // initial_size
                    4.0f * VisibleRadius(), // final_size
                    ms_teleportation_duration[EnemyLevel()],
                    time);
            shockwave->InitialColorMask() = Color(0.3f, 0.0f, 0.45f, 1.0f);
            shockwave->FinalColorMask() = Color(0.3f, 0.0f, 0.45f, 0.0f);
            shockwave->SetScaleInterpolationPower(2.0f); // squared scale interpolation
            shockwave->RunInReverse(true); // run this in reverse
        }
    }

    m_saved_radius = VisibleRadius(); // save the scale factor to recover later
    SetTranslation(teleport_destination);
    SetVelocity(target_velocity);
    SetScaleFactor(0.1f); // shrink to a point to grow to normal size at teleport destination.

    // pause for a 1/2 second
    m_think_state = THINK_STATE(RecoverAfterTeleporting);
    m_next_whatever_time = time + ms_teleportation_duration[EnemyLevel()];
}

void Shade::RecoverAfterTeleporting (Float time, Float frame_dt)
{
    if (time >= m_next_whatever_time)
    {
        m_think_state = m_saved_state;
        m_saved_state = NULL;
        SetScaleFactor(m_saved_radius);
    }
    else
    {
        // this should duplicate the interpolation of Explosion -- note that it is reversed
        Float scale_power = 4.0f; // same as shrinking effect in Shade::Teleport
        Float initial_size = 0.1f;
        Float lifetime_ratio = 1.0f - (m_next_whatever_time - time) / ms_teleportation_duration[EnemyLevel()];
        ASSERT1(lifetime_ratio >= 0.0f);
        ASSERT1(lifetime_ratio <= 1.0f);
        bool run_in_reverse = true;
        Float interpolation_parameter = run_in_reverse ? (1.0f - lifetime_ratio) : lifetime_ratio;
        SetScaleFactor((initial_size - m_saved_radius) * Math::Pow(interpolation_parameter, scale_power) + m_saved_radius + 0.1f);
    }
}

void Shade::ProcessInCrosshairsState (FloatVector2 const &target_position_delta, Float distance_to_target, Float current_time)
{
    // check if we're being aimed at by the target
    Float target_aim_angle_delta = Math::Arg(-target_position_delta) - m_target->Angle();
    // basically do some trig with target_position_delta being the hypotenuse of a
    // right triangle having angle target_aim_angle_delta.  then check if the opposite
    // side is shorter than the radius of this Shade (to within some tolerance).
    // we must be in front of the target for this to be valid (i.e. cos > 0)
    Float aim_distance = Abs(distance_to_target * Math::Sin(target_aim_angle_delta));
    if (Math::Cos(target_aim_angle_delta) > 0.0f && aim_distance < 2.0f * PhysicalRadius()) // tolerance is plus or minus 100%
    {
        // the target is considered to be aiming at us.  if this is a change in condition,
        // record the current time as the start of the in-crosshairs time
        if (!m_in_crosshairs)
            m_in_crosshairs_start_time = current_time;
        m_in_crosshairs = true;
    }
    else
        m_in_crosshairs = false;
}

void Shade::MatchVelocity (FloatVector2 const &velocity, Float const frame_dt)
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

        AccumulateForce(thrust_vector);
    }
}

void Shade::AimWeaponAtTarget ()
{
    if (!m_target.IsValid() || m_target->IsDead())
    {
        m_target.Release();
        m_think_state = THINK_STATE(PickWanderDirection);
        return;
    }

    {
        ASSERT1(m_weapon != NULL);
        FloatVector2 p(GetObjectLayer()->AdjustedDifference(m_target->Translation(), Translation()));
        FloatVector2 v(m_target->Velocity() - Velocity());
        FloatVector2 a;
        Float projectile_speed = SlowBulletGun::ms_muzzle_speed[m_weapon->UpgradeLevel()];
        ASSERT1(projectile_speed > 0.0f);
        if (EnemyLevel() == 1)
            a = FloatVector2::ms_zero;
        else
            a = m_target->Force() / m_target->Mass();

        Polynomial poly;
        poly.Set(4, 0.25f * (a|a));
        poly.Set(3, (a|v));
        poly.Set(2, (a|p) + (v|v) - projectile_speed*projectile_speed);
        poly.Set(1, 2.0f * (p|v));
        poly.Set(0, (p|p));

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

        if (T <= 0.0f)
        {
            // if no acceptable solution, just do dumb approach
            SetReticleCoordinates(m_target->Translation());
        }
        else
        {
            FloatVector2 direction_to_aim((p + v*T + 0.5f*a*Sqr(T)) / (projectile_speed*T));
            SetReticleCoordinates(Translation() + direction_to_aim.Normalization());
        }
        SetWeaponPrimaryInput(UINT8_UPPER_BOUND);
    }
}

} // end of namespace Dis
