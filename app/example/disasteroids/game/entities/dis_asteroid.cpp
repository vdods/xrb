// ///////////////////////////////////////////////////////////////////////////
// dis_asteroid.cpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_asteroid.hpp"

#include "dis_item.hpp"
#include "dis_spawn.hpp"
#include "dis_world.hpp"
#include "xrb_engine2_objectlayer.hpp"

using namespace Xrb;

namespace Dis
{

Uint8 const Asteroid::ms_mineral_distribution_lookup_table
    [Asteroid::DISTRIBUTION_LOOKUP_TABLE_COUNT]
    [Asteroid::DISTRIBUTION_LOOKUP_TABLE_SIZE] =
{
    { //1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        // no mineral 1
        // no mineral 2
        // no mineral 3
    },
    { //1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
        // no mineral 2
        // no mineral 3
    },
    { //1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
        // no mineral 3
    },
    { //1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        3, 3, 3, 3, 3, 3, 3,
    },
};

Uint8 const Asteroid::ms_number_of_fragments_to_spawn = 5;
Float const Asteroid::ms_minimum_breakup_mass = 40.0f;
Float const Asteroid::ms_decay_scale_factor = 5.0f;
Float const Asteroid::ms_decay_delay = 10.0f;
Float const Asteroid::ms_decay_time = 2.0f;
Float const Asteroid::ms_health_factor = 0.2f;

Asteroid::Asteroid (
    Float const mass,
    Float const mineral_content,
    bool const is_a_secondary_asteroid)
    :
    Mortal(
        ms_health_factor * mass,
        ms_health_factor * mass,
        ET_ASTEROID,
        Engine2::Circle::CT_SOLID_COLLISION)
{
    m_is_a_secondary_asteroid = is_a_secondary_asteroid;
    m_delete_upon_next_think = false;
    m_time_at_decay_start = -1.0f;
    m_mineral_content_byte = MineralContentByte(mineral_content);
    SetStrength(D_EXPLOSION);
    SetImmunity(D_FIRE|D_EMP);
}

void Asteroid::Think (Float const time, Float const frame_dt)
{
    // we don't bother calling Mortal::Think here because the think time is so
    // high, and we don't want to change it so all asteroids Think every frame.

    // if this asteroid shouldn't decay, then defer the think for a long time
    if (!m_is_a_secondary_asteroid || ScaleFactor() > ms_decay_scale_factor)
    {
        SetNextTimeToThink(time + 10000.0f);
        return;
    }
    else
    {
        if (m_delete_upon_next_think)
        {
            // let the world know this asteroid went bye-bye
            DStaticCast<World *>(GetWorld())->RecordDestroyedAsteroid(this);
            // go bye-bye
            ScheduleForDeletion(0.0f);
        }
        else
            m_delete_upon_next_think = true;

        SetNextTimeToThink(time + ms_decay_delay);
    }
}

void Asteroid::Collide (
    Entity *const collider,
    FloatVector2 const &collision_location,
    FloatVector2 const &collision_normal,
    Float const collision_force,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(collider != NULL);

    Mortal::Collide(
        collider,
        collision_location,
        collision_normal,
        collision_force,
        time,
        frame_dt);

    Float s_asteroid_debris_force_threshold = 200000.0f;

    // spawn some debris if the force was high enough
    if (collision_force >= s_asteroid_debris_force_threshold)
    {
        // TODO
        // spawn two debris effect sprites, going perpendicular to the
        // collision normal, in opposite directions
    }
}

void Asteroid::Die (
    Entity *const killer,
    Entity *const kill_medium,
    FloatVector2 const &kill_location,
    FloatVector2 const &kill_normal,
    Float const kill_force,
    DamageType const kill_type,
    Float const time,
    Float const frame_dt)
{
    SpawnNoDamageExplosion(
        GetObjectLayer(),
        Translation(),
        Velocity(),
        7.0f * ScaleFactor(),
        0.2f,
        time);

    // this will store the amount of mass (multiplied by the mineral
    // content) to use to spawn minerals
    Float converted_mass;

    // if it's big enough, spawn some smaller asteroids, to simulate
    // this asteroid breaking up
    if (Mass() > ms_minimum_breakup_mass)
    {
        converted_mass = ProportionToConvertToMinerals() * Mass();
        Float mass =
            Mass() / static_cast<Float>(ms_number_of_fragments_to_spawn);
        FloatVector2 source_velocity = Velocity();

        FloatVector2 kill_offset(GetObjectLayer()->AdjustedDifference(kill_location, Translation()));
        if (kill_offset.IsZero())
            kill_offset = FloatVector2(1.0f, 0.0f);
        Float seed_angle = Math::Atan(kill_offset);

        // let the world more asteroids are being created.  the added asteroid
        // mass is exactly equal to the destroyed asteroid's mass.
        DStaticCast<World *>(GetWorld())->
            RecordCreatedAsteroids(ms_number_of_fragments_to_spawn, Mass());

        for (Uint8 i = 0; i < ms_number_of_fragments_to_spawn; ++i)
        {
            Float velocity_angle = seed_angle;
            seed_angle += 360.0f / static_cast<Float>(ms_number_of_fragments_to_spawn);
            ASSERT1(CurrentHealth() <= 0.0f);
            ASSERT1(MaxHealth() > 0.0f);
            Float health_ratio = CurrentHealth() / MaxHealth();
            Float explosion_speed = ScaleFactor() * health_ratio * health_ratio / Mass();
            if (explosion_speed > 5.0f)
                explosion_speed = 5.0f;
            FloatVector2 velocity =
                source_velocity +
                explosion_speed * Math::UnitVector(velocity_angle);
            Float scale_factor = Math::Sqrt(mass);
            FloatVector2 translation(
                Translation() +
                1.7f * scale_factor * Math::UnitVector(velocity_angle));

            Float proportion_of_mineral_content_to_allocate;
            proportion_of_mineral_content_to_allocate =
                Math::RandomFloat(0.0f, 1.0f / (ms_number_of_fragments_to_spawn - i));
            Float mineral_content =
                    MineralContent() *
                    (1.0f - proportion_of_mineral_content_to_allocate);

            Asteroid *new_asteroid =
                SpawnAsteroid(
                    GetObjectLayer(),
                    translation,
                    scale_factor,
                    mass,
                    velocity,
                    mineral_content,
                    true);
            // this makes the explosion way awesomer
            if (i == 0 && mass > ms_minimum_breakup_mass)
                new_asteroid->Kill(
                    NULL, // no killer
                    NULL, // no kill medium
                    kill_location,
                    kill_normal,
                    kill_force,
                    kill_type,
                    time,
                    frame_dt);
        }
    }
    else
        converted_mass = Mass();

    // of the destroyed mass, spawn minerals according to the mineral content
    // and mineral weights of this asteroid.
    {
        static Float const s_min_mineral_mass = 5.0f;
        static Float const s_max_mineral_mass = 30.0f;

        Float mineral_mass_to_spawn = converted_mass * MineralContent();
        while (mineral_mass_to_spawn > s_min_mineral_mass)
        {
            Float mass =
                Math::RandomFloat(
                    s_min_mineral_mass,
                    Min(s_max_mineral_mass, mineral_mass_to_spawn));
            Float velocity_angle = Math::RandomAngle();
            FloatVector2 velocity = Velocity() + 10.0f * Math::UnitVector(velocity_angle);
            Float scale_factor = Math::Sqrt(mass);
            Uint8 mineral_index = RandomMineral();
            SpawnPowerup(
                GetObjectLayer(),
                Translation() + scale_factor * Math::UnitVector(velocity_angle),
                scale_factor,
                mass,
                velocity,
                Item::MineralSpritePath(mineral_index),
                static_cast<ItemType>(IT_MINERAL_LOWEST+mineral_index));

            mineral_mass_to_spawn -= mass;
        }
    }

    // let the world know this asteroid went bye-bye
    DStaticCast<World *>(GetWorld())->RecordDestroyedAsteroid(this);
    // go bye-bye
    ScheduleForDeletion(0.0f);
}

Uint8 Asteroid::RandomMineral () const
{
    Uint8 mineral_level = DStaticCast<World *>(GetWorld())->AsteroidMineralLevel();
    ASSERT1(mineral_level < DISTRIBUTION_LOOKUP_TABLE_COUNT);
    Uint16 random_element =
        Math::RandomUint16(0, DISTRIBUTION_LOOKUP_TABLE_SIZE-1);
    DEBUG1_CODE(
        Uint8 mineral_type =
            ms_mineral_distribution_lookup_table[mineral_level][random_element]);
    ASSERT1(mineral_type < MINERAL_COUNT);
    return ms_mineral_distribution_lookup_table[mineral_level][random_element];
}

Float Asteroid::ProportionToConvertToMinerals () const
{
    static Float const s_n = 400.0f;
    static Float const s_N = Math::Sqrt(1.0f / 50.0f);
    static Float const s_m = ms_minimum_breakup_mass;
    static Float const s_M = Math::Sqrt(1.0f / 1.0f);
    static Float const s_slope = (s_N - s_M) / (s_n - s_m);
    static Float const s_offset = (s_M * s_n - s_N * s_m) / (s_n - s_m);

    Float proportion_to_convert;
    if (Mass() > s_n)
        proportion_to_convert = s_N;
    else
        proportion_to_convert = s_slope * Mass() + s_offset;

    return proportion_to_convert * proportion_to_convert;
}

} // end of namespace Dis

