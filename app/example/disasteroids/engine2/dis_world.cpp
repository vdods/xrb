// ///////////////////////////////////////////////////////////////////////////
// dis_world.cpp by Victor Dods, created 2006/01/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_world.h"

#include "dis_armor.h" 
#include "dis_asteroid.h"
#include "dis_devourment.h"
#include "dis_engine.h"
#include "dis_events.h"
#include "dis_gameobject.h"
#include "dis_interloper.h"
#include "dis_physicshandler.h"
#include "dis_playership.h"
#include "dis_powereddevice.h"
#include "dis_powergenerator.h"
#include "dis_revulsion.h"
#include "dis_shade.h"
#include "dis_shield.h"
#include "dis_solitary.h"
#include "dis_spawn.h"
#include "dis_weapon.h"
#include "dis_worldview.h"
#include "xrb_engine2_objectlayer.h"
#include "xrb_engine2_physicshandler.h"
#include "xrb_engine2_sprite.h"
#include "xrb_engine2_spriteentity.h"

using namespace Xrb;

namespace Dis
{

Float const World::ms_asteroid_mineral_content_factor[World::MINERAL_CONTENT_LEVEL_COUNT] = { 0.3f, 0.4f, 0.5f, 0.6f };
Uint32 const World::ms_max_enemy_ship_count[GAME_STAGE_COUNT][GameObject::T_ENEMY_SHIP_COUNT] =
{
    {  1,  0,  0,  1 },
    {  2,  0,  0,  1 },
    {  3,  1,  1,  1 },
    {  4,  1,  1,  1 },
    {  4,  2,  1,  1 },
    {  5,  3,  2,  1 },
    {  5,  3,  2,  2 },
    {  6,  4,  2,  2 },
    {  6,  4,  3,  2 },
    {  7,  5,  3,  2 },
    {  7,  5,  3,  2 },
    {  8,  6,  4,  2 },
    {  8,  6,  4,  3 },
    {  8,  7,  4,  3 },
    {  8,  7,  5,  3 },
    {  9,  7,  5,  3 },
    {  9,  8,  5,  3 },
    {  9,  8,  6,  4 },
    { 10,  8,  6,  4 },
    { 10,  9,  6,  4 }
};
Uint8 const World::ms_enemy_level_distribution_table[GAME_STAGE_COUNT][GameObject::T_ENEMY_SHIP_COUNT][DISTRIBUTION_TABLE_SIZE] =
{
    { // stage 0
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // Interloper
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // Shade
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // Revulsion
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }  // Devourment
    },
    { // stage 1
        { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 }, // Interloper
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, // Shade
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // Revulsion
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }  // Devourment
    },
    { // stage 2
        { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1 }, // Interloper
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, // Shade
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // Revulsion
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }  // Devourment
    },
    { // stage 3
        { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1 }, // Interloper
        { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 }, // Shade
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, // Revulsion
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }  // Devourment
    },
    { // stage 4
        { 0, 0, 0, 0, 1, 1, 1, 1, 1, 2 }, // Interloper
        { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1 }, // Shade
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, // Revulsion
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }  // Devourment
    },
    { // stage 5
        { 0, 0, 0, 1, 1, 1, 1, 1, 2, 2 }, // Interloper
        { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1 }, // Shade
        { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 }, // Revulsion
        { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 }  // Devourment
    },
    { // stage 6
        { 0, 0, 1, 1, 1, 1, 1, 2, 2, 2 }, // Interloper
        { 0, 0, 0, 0, 0, 1, 1, 1, 1, 2 }, // Shade
        { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 }, // Revulsion
        { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1 }  // Devourment
    },
    { // stage 7
        { 0, 1, 1, 1, 1, 1, 2, 2, 2, 2 }, // Interloper
        { 0, 0, 0, 0, 1, 1, 1, 1, 1, 2 }, // Shade
        { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1 }, // Revulsion
        { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1 }  // Devourment
    },
    { // stage 8
        { 0, 1, 1, 1, 1, 2, 2, 2, 2, 3 }, // Interloper
        { 0, 0, 0, 1, 1, 1, 1, 1, 1, 2 }, // Shade
        { 0, 0, 0, 0, 0, 0, 1, 1, 1, 2 }, // Revulsion
        { 0, 0, 0, 0, 0, 1, 1, 1, 1, 2 }  // Devourment
    },
    { // stage 9
        { 0, 1, 1, 1, 2, 2, 2, 2, 3, 3 }, // Interloper
        { 0, 0, 0, 1, 1, 1, 1, 1, 2, 2 }, // Shade
        { 0, 0, 0, 0, 0, 0, 1, 1, 1, 2 }, // Revulsion
        { 0, 0, 0, 0, 1, 1, 1, 1, 2, 2 }  // Devourment
    },
    { // stage 10
        { 0, 1, 1, 2, 2, 2, 2, 3, 3, 3 }, // Interloper
        { 0, 0, 1, 1, 1, 1, 1, 1, 2, 2 }, // Shade
        { 0, 0, 0, 0, 0, 1, 1, 1, 2, 2 }, // Revulsion
        { 0, 0, 0, 1, 1, 1, 1, 2, 2, 2 }  // Devourment
    },
    { // stage 11
        { 0, 1, 2, 2, 2, 2, 3, 3, 3, 3 }, // Interloper
        { 0, 0, 1, 1, 1, 1, 1, 2, 2, 2 }, // Shade
        { 0, 0, 0, 0, 1, 1, 1, 2, 2, 2 }, // Revulsion
        { 0, 0, 1, 1, 1, 1, 2, 2, 2, 2 }  // Devourment
    },
    { // stage 12
        { 0, 1, 2, 2, 2, 3, 3, 3, 3, 3 }, // Interloper
        { 0, 1, 1, 1, 1, 1, 2, 2, 2, 2 }, // Shade
        { 0, 0, 0, 1, 1, 1, 1, 2, 2, 2 }, // Revulsion
        { 0, 0, 1, 1, 1, 1, 2, 2, 2, 3 }  // Devourment
    },
    { // stage 13
        { 0, 1, 2, 2, 3, 3, 3, 3, 3, 3 }, // Interloper
        { 0, 1, 1, 1, 1, 2, 2, 2, 2, 3 }, // Shade
        { 0, 0, 0, 1, 1, 1, 1, 2, 2, 2 }, // Revulsion
        { 0, 1, 1, 1, 1, 2, 2, 2, 2, 3 }  // Devourment
    },
    { // stage 14
        { 0, 1, 2, 3, 3, 3, 3, 3, 3, 3 }, // Interloper
        { 0, 1, 1, 1, 1, 2, 2, 2, 3, 3 }, // Shade
        { 0, 0, 1, 1, 1, 1, 2, 2, 2, 2 }, // Revulsion
        { 0, 1, 1, 1, 1, 2, 2, 2, 3, 3 }  // Devourment
    },
    { // stage 15
        { 0, 1, 2, 3, 3, 3, 3, 3, 3, 3 }, // Interloper
        { 0, 1, 1, 1, 2, 2, 2, 2, 3, 3 }, // Shade
        { 0, 0, 1, 1, 1, 1, 2, 2, 2, 2 }, // Revulsion
        { 0, 1, 1, 1, 2, 2, 2, 2, 3, 3 }  // Devourment
    },
    { // stage 16
        { 0, 1, 2, 3, 3, 3, 3, 3, 3, 3 }, // Interloper
        { 0, 1, 1, 2, 2, 2, 2, 3, 3, 3 }, // Shade
        { 0, 1, 1, 1, 1, 2, 2, 2, 2, 3 }, // Revulsion
        { 0, 1, 1, 2, 2, 2, 2, 3, 3, 3 }  // Devourment
    },
    { // stage 17
        { 0, 1, 2, 3, 3, 3, 3, 3, 3, 3 }, // Interloper
        { 0, 1, 1, 2, 2, 2, 3, 3, 3, 3 }, // Shade
        { 0, 1, 1, 1, 2, 2, 2, 2, 3, 3 }, // Revulsion
        { 0, 1, 1, 2, 2, 2, 3, 3, 3, 3 }  // Devourment
    },
    { // stage 18
        { 0, 1, 2, 3, 3, 3, 3, 3, 3, 3 }, // Interloper
        { 0, 1, 2, 2, 2, 3, 3, 3, 3, 3 }, // Shade
        { 0, 1, 1, 1, 2, 2, 2, 3, 3, 3 }, // Revulsion
        { 0, 1, 2, 2, 2, 3, 3, 3, 3, 3 }  // Devourment
    },
    { // stage 19
        { 0, 1, 2, 3, 3, 3, 3, 3, 3, 3 }, // Interloper
        { 0, 1, 2, 2, 3, 3, 3, 3, 3, 3 }, // Shade
        { 0, 1, 1, 2, 2, 2, 3, 3, 3, 3 }, // Revulsion
        { 0, 1, 2, 2, 3, 3, 3, 3, 3, 3 }  // Devourment
    }
};
Float const World::ms_enemy_spawn_interval[GAME_STAGE_COUNT] =
{
    5.0f,
    5.5f,
    6.0f,
    6.5f,
    7.0f,
    7.0f,
    7.5f,
    7.5f,
    8.0f,
    8.0f,
    8.0f,
    8.5f,
    8.5f,
    8.5f,
    8.5f,
    9.0f,
    9.0f,
    9.0f,
    9.0f,
    9.0f
};

World::~World ()
{
    // if the ship exists and has been removed from the world, add
    // it back in, so it gets deleted properly.
    // TODO: think of a better way to handle this
    if (m_player_ship != NULL &&
        m_player_ship->GetOwnerEntity() != NULL &&
        !m_player_ship->GetOwnerEntity()->GetIsInWorld())
    {
        m_player_ship->GetOwnerEntity()->AddBackIntoWorld();
    }
}

World *World::Create (
    EventQueue *owner_event_queue,
    Uint32 entity_capacity)
{
    ASSERT1(owner_event_queue != NULL)
    ASSERT1(entity_capacity > 0)
    return new World(owner_event_queue, new PhysicsHandler(), entity_capacity);
}

PhysicsHandler *World::GetDisPhysicsHandler ()
{
    return DStaticCast<PhysicsHandler *>(GetPhysicsHandler());
}

void World::RecordDestroyedPlayerShip (PlayerShip const *const player_ship)
{
    ASSERT1(player_ship != NULL)
    ASSERT1(m_player_ship == player_ship)
    ASSERT1(m_game_state == GS_NORMAL_GAMEPLAY)
    if (m_player_ship->GetLivesRemaining() > 0)
    {
        SetGameState(GS_PLAYER_SHIP_IS_DEAD);
        EnqueueEvent(new EventSetGameState(GS_SPAWN_PLAYER_SHIP, GetMostRecentFrameTime() + 3.0f));
    }
    else
    {
        SetGameState(GS_GAME_OVER);
        EnqueueEvent(new EventSetGameState(GS_DESTROY_WORLD, GetMostRecentFrameTime() + 3.0f));
    }
}

void World::RecordCreatedAsteroids (
    Uint32 const created_asteroid_count,
    Float const created_asteroids_mass)
{
    ASSERT1(created_asteroid_count > 0)
    ASSERT1(created_asteroids_mass > 0.0f)
    m_asteroid_count += created_asteroid_count;
    m_asteroid_mass += created_asteroids_mass;
}

void World::RecordDestroyedAsteroid (Asteroid const *const asteroid)
{
    ASSERT1(asteroid != NULL)
    ASSERT1(m_asteroid_count > 0)
    ASSERT1(m_asteroid_mass >= asteroid->GetFirstMoment())
    --m_asteroid_count;
    m_asteroid_mass -= asteroid->GetFirstMoment();
}

void World::RecordCreatedEnemyShip (GameObject::Type const enemy_ship_type)
{
    ASSERT1(enemy_ship_type >= GameObject::T_ENEMY_SHIP_LOWEST)
    ASSERT1(enemy_ship_type <= GameObject::T_ENEMY_SHIP_HIGHEST)
    ++m_enemy_ship_count[enemy_ship_type - GameObject::T_ENEMY_SHIP_LOWEST];
}

void World::RecordDestroyedEnemyShip (EnemyShip const *const enemy_ship)
{
    ASSERT1(enemy_ship != NULL)
    ASSERT1(m_enemy_ship_count[enemy_ship->GetType() - GameObject::T_ENEMY_SHIP_LOWEST] > 0)
    --m_enemy_ship_count[enemy_ship->GetType() - GameObject::T_ENEMY_SHIP_LOWEST];

    if (m_enemy_ship_count[enemy_ship->GetType()] == ms_max_enemy_ship_count[m_game_stage][enemy_ship->GetType()] - 1)
        m_next_enemy_ship_spawn_time[enemy_ship->GetType()] =
            GetFrameTime() +
            Math::RandomFloat(
                ms_enemy_spawn_interval[m_game_stage] - 1.0f,
                ms_enemy_spawn_interval[m_game_stage] + 1.0f);
}

World::World (
    EventQueue *owner_event_queue,
    Engine2::PhysicsHandler *physics_handler,
    Uint32 entity_capacity)
    :
    Engine2::World(owner_event_queue, physics_handler, entity_capacity)
{
    m_game_state = GS_CREATE_WORLD;

    m_player_ship = NULL;
    m_score_required_for_extra_life = 50000;

    m_game_stage = 0;
    m_next_game_stage_time = 30.0f;
    m_asteroid_count = 0;
    m_asteroid_mass = 0.0f;
    m_next_asteroid_spawn_time = 0.0f;
    m_asteroid_mineral_level = 0;
    m_next_asteroid_mineral_level_time = 1.0f * 60.0f;
    m_asteroid_mineral_content_level = 0;
    m_next_asteroid_mineral_content_level_time = 1.0f * 60.0f;

    for (Uint8 enemy_ship_type = 0; enemy_ship_type < GameObject::T_ENEMY_SHIP_COUNT; ++enemy_ship_type)
    {
        m_enemy_ship_count[enemy_ship_type] = 0;
        m_next_enemy_ship_spawn_time[enemy_ship_type] = 0.0f;
    }

    CreateAndPopulateBackgroundObjectLayers();
    CreateAndPopulateForegroundObjectLayer();

    ASSERT1(m_player_ship != NULL)

    SetGameState(GS_NORMAL_GAMEPLAY);
}

bool World::ProcessEventOverride (Event const *const e)
{
    ASSERT1(e != NULL)

    if (e->GetType() != Event::CUSTOM)
        return Engine2::World::ProcessEventOverride(e);

    EventBase const *dis_event = DStaticCast<EventBase const *>(e);
    switch (dis_event->GetSubType())
    {
        case EventBase::ST_SET_GAME_STATE:
            SetGameState(DStaticCast<EventSetGameState const *>(dis_event)->GetGameState());
            break;

        default:
            ASSERT1(false && "Unhandled custom event")
            break;
    }

    return true;
}

void World::ProcessFrameOverride ()
{
    Engine2::World::ProcessFrameOverride();

    // update the game stage
    if (m_next_game_stage_time <= GetFrameTime() &&
        m_game_stage < GAME_STAGE_COUNT - 1)
    {
        ++m_game_stage;
        fprintf(stderr, "increased game stage (%u)\n", m_game_stage);
        m_next_game_stage_time = GetFrameTime() + 30.0f;
    }
    
    // asteroid spawning
    static Uint32 const s_max_asteroid_count = 30;
    static Float const s_max_asteroid_mass = 6000.0f;
    if (m_next_asteroid_spawn_time <= GetFrameTime() &&
        m_asteroid_count < s_max_asteroid_count &&
        m_asteroid_mass < s_max_asteroid_mass)
    {
        Asteroid *asteroid = SpawnAsteroidOutOfView();
        if (asteroid != NULL)
            m_next_asteroid_spawn_time = GetFrameTime() + Math::RandomFloat(0.5f, 1.0f);
    }

    // update the asteroid mineral level
    if (m_asteroid_mineral_level < Asteroid::MAX_MINERAL_LEVEL &&
        m_next_asteroid_mineral_level_time <= GetFrameTime())
    {
        ++m_asteroid_mineral_level;
        m_next_asteroid_mineral_level_time = GetFrameTime() + 60.0f;
    }

    // update the asteroid mineral content level
    if (m_asteroid_mineral_level < World::MINERAL_CONTENT_LEVEL_COUNT - 1 &&
        m_next_asteroid_mineral_content_level_time <= GetFrameTime())
    {
        ++m_asteroid_mineral_content_level;
        m_next_asteroid_mineral_content_level_time = GetFrameTime() + 60.0f;
    }

    // enemy ship spawning
    for (Uint8 enemy_ship_type = 0; enemy_ship_type < GameObject::T_ENEMY_SHIP_COUNT; ++enemy_ship_type)
    {
        if (m_enemy_ship_count[enemy_ship_type] < ms_max_enemy_ship_count[m_game_stage][enemy_ship_type] &&
            m_next_enemy_ship_spawn_time[enemy_ship_type] <= GetFrameTime())
        {
            EnemyShip *enemy_ship =
                SpawnEnemyShipOutOfView(
                    static_cast<GameObject::Type>(GameObject::T_ENEMY_SHIP_LOWEST + enemy_ship_type),
                    ms_enemy_level_distribution_table[m_game_stage][enemy_ship_type][Math::RandomUint16(0, DISTRIBUTION_TABLE_SIZE-1)]);
            if (enemy_ship != NULL)
            {
                ++m_enemy_ship_count[enemy_ship_type];
                if (m_enemy_ship_count[enemy_ship_type] < ms_max_enemy_ship_count[m_game_stage][enemy_ship_type])
                    m_next_enemy_ship_spawn_time[enemy_ship_type] =
                        GetFrameTime() +
                        Math::RandomFloat(
                            ms_enemy_spawn_interval[m_game_stage] - 1.0f,
                            ms_enemy_spawn_interval[m_game_stage] + 1.0f);
            }
        }
    }
        
    // do game logic control stuff here

    if (m_player_ship->GetLivesRemaining() > 0 || !m_player_ship->GetIsDead())
        m_player_ship->IncrementTimeAlive(GetFrameDT());

    if (m_player_ship->GetScore() >= m_score_required_for_extra_life)
    {
        static Float const s_extra_live_score_factor = 2.0f;
        m_player_ship->IncrementLivesRemaining(1);
        m_score_required_for_extra_life =
            static_cast<Uint32>(s_extra_live_score_factor * m_score_required_for_extra_life);
    }
    
    switch (m_game_state)
    {
        case GS_CREATE_WORLD:
            // placeholder (though this technically should never happen)

            // this could be a zoom-in or a fade-in or otherwise be the pause
            // while some visual intro happens before gameplay begins.
            break;
    
        case GS_SPAWN_PLAYER_SHIP:
            ASSERT1(m_player_ship != NULL)
            ASSERT1(!m_player_ship->GetIsInWorld())
            ASSERT1(m_player_ship->GetIsDead())
            ASSERT1(m_player_ship->GetLivesRemaining() > 0)
            m_player_ship->Revive(GetFrameTime(), GetFrameDT());
            m_player_ship->SetVelocity(FloatVector2::ms_zero);
            // TODO place the player ship so it doesn't intersect anything
            m_player_ship->AddBackIntoWorld();
            m_player_ship->IncrementLivesRemaining(-1);
            fprintf(stderr, "respawning player ship (%u lives left)\n", m_player_ship->GetLivesRemaining());
            SetGameState(GS_NORMAL_GAMEPLAY);
            break;

        case GS_NORMAL_GAMEPLAY:
            // placeholder
            break;

        case GS_PLAYER_SHIP_IS_DEAD:
            // placeholder
            break;

        case GS_GAME_OVER:
            // placeholder
            
            // record the player's time alive and score here
            break;

        case GS_DESTROY_WORLD:
            // placeholder

            // fade-out or some other visual outro
            break;
            
        default:
            ASSERT1(false && "Invalid World::GameState")
            break;
    }
}

void World::HandleAttachWorldView (Engine2::WorldView *const world_view)
{
    DStaticCast<WorldView *>(world_view)->SetPlayerShip(m_player_ship);
}

void World::SetGameState (GameState const game_state)
{
    if (m_game_state != game_state)
    {
        m_game_state = game_state;
        fprintf(stderr, "setting game state to %u\n", m_game_state);
    }
}

Asteroid *World::SpawnAsteroidOutOfView ()
{
    Float object_layer_side_length = GetMainObjectLayer()->GetSideLength();
    FloatVector2 translation;
    Float scale_factor;
    Uint32 placement_attempts = 0;
    do
    {
        // don't bother spawning an asteroid if it takes
        // too many tries to place it.
        if (placement_attempts == 30)
            return NULL;
        ++placement_attempts;
        
        translation.SetComponents(
            Math::RandomFloat(-0.5f*object_layer_side_length, 0.5f*object_layer_side_length),
            Math::RandomFloat(-0.5f*object_layer_side_length, 0.5f*object_layer_side_length));
        // this causes the randomness to favor smaller asteroids
        Float scale_seed = Math::RandomFloat(0.0f, 1.0f);
        scale_factor = 55.0f * scale_seed * scale_seed + 5.0f;
    }
    while (!IsAreaNotVisibleAndNotOverlappingAnyEntities(translation, scale_factor));

    Float first_moment = scale_factor * scale_factor;
    FloatVector2 velocity(
        Math::RandomFloat(50.0f, 1000.0f) /
        Math::Sqrt(first_moment) *
        Math::UnitVector(Math::RandomAngle()));

    ++m_asteroid_count;
    m_asteroid_mass += first_moment;
    return SpawnAsteroid(
        this,
        GetMainObjectLayer(),
        translation,
        scale_factor,
        first_moment,
        velocity,
        Math::RandomFloat(0.5f, 1.0f) * ms_asteroid_mineral_content_factor[m_asteroid_mineral_content_level]);
}

EnemyShip *World::SpawnEnemyShipOutOfView (
    GameObject::Type const enemy_ship_type,
    Uint8 const enemy_level)
{
    ASSERT1(enemy_ship_type >= GameObject::T_ENEMY_SHIP_LOWEST)
    ASSERT1(enemy_ship_type <= GameObject::T_ENEMY_SHIP_HIGHEST)
    ASSERT1(enemy_level < EnemyShip::ENEMY_LEVEL_COUNT)

    Float object_layer_side_length = GetMainObjectLayer()->GetSideLength();
    FloatVector2 translation;
    Float personal_space_radius = 0.0f;
    switch (enemy_ship_type)
    {
        case GameObject::T_INTERLOPER: personal_space_radius = 12.0f; break;
        case GameObject::T_SHADE:      personal_space_radius = 8.0f;  break;
        case GameObject::T_REVULSION:  personal_space_radius = 15.0f; break;
        case GameObject::T_DEVOURMENT: personal_space_radius = 40.0f; break;
        default: ASSERT1(false && "Invalid GameObject::Type") break;
    }

    Uint32 placement_attempts = 0;
    do
    {
        // don't bother spawning an asteroid if it takes
        // too many tries to place it.
        if (placement_attempts == 30)
            return NULL;
        ++placement_attempts;
        
        translation.SetComponents(
            Math::RandomFloat(-0.5f*object_layer_side_length, 0.5f*object_layer_side_length),
            Math::RandomFloat(-0.5f*object_layer_side_length, 0.5f*object_layer_side_length));
    }
    while (!IsAreaNotVisibleAndNotOverlappingAnyEntities(translation, personal_space_radius));

    switch (enemy_ship_type)
    {
        case GameObject::T_INTERLOPER:
            return SpawnInterloper(
                this,
                GetMainObjectLayer(),
                translation,
                FloatVector2::ms_zero,
                enemy_level);
                
        case GameObject::T_SHADE:
            return SpawnShade(
                this,
                GetMainObjectLayer(),
                translation,
                FloatVector2::ms_zero,
                enemy_level);
            
        case GameObject::T_REVULSION:
            return SpawnRevulsion(
                this,
                GetMainObjectLayer(),
                translation,
                FloatVector2::ms_zero,
                enemy_level);

        case GameObject::T_DEVOURMENT:
            return SpawnDevourment(
                this,
                GetMainObjectLayer(),
                translation,
                FloatVector2::ms_zero,
                enemy_level);                
            
        default:
            ASSERT1(false && "Invalid GameObject::Type")
            return NULL;
    }
}

bool World::IsAreaNotVisibleAndNotOverlappingAnyEntities (
    FloatVector2 const &translation,
    Float const scale_factor)
{
    Float object_layer_side_length = GetMainObjectLayer()->GetSideLength();
    Float half_object_layer_side_length = 0.5f * object_layer_side_length;
    ASSERT1(translation[Dim::X] >= -0.5f * object_layer_side_length)
    ASSERT1(translation[Dim::X] <=  0.5f * object_layer_side_length)
    ASSERT1(translation[Dim::Y] >= -0.5f * object_layer_side_length)
    ASSERT1(translation[Dim::Y] <=  0.5f * object_layer_side_length)
    ASSERT1(scale_factor > 0.0f)

    // check that the area is not in view of any attached WorldView
    for (WorldViewListIterator it = m_world_view_list.begin(),
                               it_end = m_world_view_list.end();
         it != it_end;
         ++it)
    {
        WorldView *world_view = DStaticCast<WorldView *>(*it);
        ASSERT1(world_view != NULL)
        FloatVector2 world_view_center(world_view->GetViewCenter());

        // temp hack until real wrapped view coordinates are done
        {
            while (world_view_center[Dim::X] < -half_object_layer_side_length)
                world_view_center[Dim::X] += object_layer_side_length;
                
            while (world_view_center[Dim::X] > half_object_layer_side_length)
                world_view_center[Dim::X] -= object_layer_side_length;
                
            while (world_view_center[Dim::Y] < -half_object_layer_side_length)
                world_view_center[Dim::Y] += object_layer_side_length;
                
            while (world_view_center[Dim::Y] > half_object_layer_side_length)
                world_view_center[Dim::Y] -= object_layer_side_length;
        }
        
        Float distance_from_world_view =
            (GetMainObjectLayer()->GetAdjustedCoordinates(translation, world_view_center) -
             world_view_center).GetLength();
        // fail if the area is in sight
        if (distance_from_world_view < world_view->GetParallaxedViewRadius(NULL) + scale_factor)
            return false;
    }

    // fail if the area overlaps any entity
    if (GetDisPhysicsHandler()->GetDoesAreaOverlapAnyEntityInObjectLayer(
            GetMainObjectLayer(),
            translation,
            scale_factor,
            false))
        return false;

    // if it reached this far, the area is in an acceptable place
    return true;
}

void World::CreateAndPopulateForegroundObjectLayer ()
{
    Float object_layer_side_length = 1250.0f;
    
    // create an object layer
    Engine2::ObjectLayer *object_layer =
        Engine2::ObjectLayer::Create(
            this,                     // owner world
            true,                     // wrapped
            object_layer_side_length, // side length
            6,                        // visibility quad tree depth
            0.0f);                    // z depth
    AddObjectLayer(object_layer);
    SetMainObjectLayer(object_layer);

    // create the player's ship
    m_player_ship =
        SpawnSolitary(
            this,
            object_layer,
            FloatVector2::ms_zero,
            FloatVector2::ms_zero);

    // add weapons and engines to the ship
    {
        bool add_succeeded;
        add_succeeded = m_player_ship->AddItem(new PeaShooter(0));
        ASSERT1(add_succeeded)
        add_succeeded = m_player_ship->AddItem(new Engine(0));
        ASSERT1(add_succeeded)
        add_succeeded = m_player_ship->AddItem(new PowerGenerator(0));
        ASSERT1(add_succeeded)
    }

    static Uint32 const s_number_of_asteroids_to_spawn = 10;
    static Float const s_asteroid_scale_factor_max = 35.0f;
    static Float const s_asteroid_scale_factor_min = 5.0f;
    for (Uint32 i = 0; i < s_number_of_asteroids_to_spawn; ++i)
    {
        FloatVector2 translation;
        Float scale_factor;
        Uint32 placement_attempts = 0;
        static Uint32 const s_max_placement_attempts = 30;
        do
        {
            // don't bother spawning an asteroid if it takes
            // too many tries to place it.
            if (placement_attempts == s_max_placement_attempts)
                break;
            ++placement_attempts;
            
            translation.SetComponents(
                Math::RandomFloat(-0.5f*object_layer_side_length, 0.5f*object_layer_side_length),
                Math::RandomFloat(-0.5f*object_layer_side_length, 0.5f*object_layer_side_length));
            // this causes the randomness to favor smaller asteroids
            Float scale_seed = Math::RandomFloat(0.0f, 1.0f);
            scale_factor =
                (s_asteroid_scale_factor_max - s_asteroid_scale_factor_min) *
                scale_seed * scale_seed + s_asteroid_scale_factor_min;
        }
        while (GetDisPhysicsHandler()->
                    GetDoesAreaOverlapAnyEntityInObjectLayer(
                        GetMainObjectLayer(),
                        translation,
                        scale_factor + 40.0f,
                        false));
        if (placement_attempts == s_max_placement_attempts)
            continue;
    
        Float first_moment = scale_factor * scale_factor;
        FloatVector2 velocity(
            Math::RandomFloat(50.0f, 1000.0f) /
            Math::Sqrt(first_moment) *
            Math::UnitVector(Math::RandomAngle()));

        ++m_asteroid_count;
        m_asteroid_mass += first_moment;
        SpawnAsteroid(
            this,
            GetMainObjectLayer(),
            translation,
            scale_factor,
            first_moment,
            velocity,
            Math::RandomFloat(0.5f, 1.0f) * ms_asteroid_mineral_content_factor[m_asteroid_mineral_content_level]);
    }
}

void World::CreateAndPopulateBackgroundObjectLayers ()
{
    // galaxies in the distance
    {
        Float object_layer_side_length = 200000.0f;
    
        // create an object layer
        Engine2::ObjectLayer *object_layer =
            Engine2::ObjectLayer::Create(
                this,                     // owner world
                true,                     // wrapped
                object_layer_side_length, // side length
                4,                        // visibility quad tree depth
                0.0f);                    // z depth
        object_layer->SetZDepth(30000.0f);
        AddObjectLayer(object_layer);
    
        static std::string const s_galaxy_sprite_filenames[] =
        {
            "resources/starfield/galaxy_small01.png",
            "resources/starfield/galaxy_small02.png",
            "resources/starfield/galaxy_small03.png",
            "resources/starfield/galaxy_small04.png",
            "resources/starfield/galaxy_small05.png",
            "resources/starfield/galaxy_small06.png",
            "resources/starfield/galaxy_small07.png",
            "resources/starfield/galaxy_small08.png",
            "resources/starfield/galaxy_small09.png",
            "resources/starfield/galaxy_small10.png",
        };
        Uint32 const galaxy_sprite_filename_count =
            sizeof(s_galaxy_sprite_filenames) / sizeof(std::string);
    
        Uint32 const number_of_galaxies_to_create = 100;
        for (Uint32 i = 0; i < number_of_galaxies_to_create; ++i)
        {
            Engine2::Sprite *sprite =
                Engine2::Sprite::Create(
                    s_galaxy_sprite_filenames[
                        Math::RandomUint16(0, galaxy_sprite_filename_count-1)]);
            sprite->SetTranslation(
                Math::RandomFloat(-0.5f*object_layer_side_length, 0.5f*object_layer_side_length),
                Math::RandomFloat(-0.5f*object_layer_side_length, 0.5f*object_layer_side_length));
            sprite->SetScaleFactor(
                Math::RandomFloat(
                    0.007f*object_layer_side_length,
                    0.011f*object_layer_side_length));
            sprite->SetAngle(Math::RandomAngle());
    
            AddObject(sprite, object_layer);
        }
    }

    // starfield
    {
        Float object_layer_side_length = 100000.0f;
    
        // create an object layer
        Engine2::ObjectLayer *object_layer =
            Engine2::ObjectLayer::Create(
                this,                     // owner world
                true,                     // wrapped
                object_layer_side_length, // side length
                6,                        // visibility quad tree depth
                0.0f);                    // z depth
        object_layer->SetZDepth(15000.0f);
        AddObjectLayer(object_layer);
    
        static std::string const s_starfield_sprite_filenames[] =
        {
            "resources/starfield/star_dot01.png",
            "resources/starfield/star_dot02.png",
            "resources/starfield/star_dot03.png",
            "resources/starfield/star_dot04.png",
            "resources/starfield/star_dot05.png",
            "resources/starfield/star_dot06.png",
            "resources/starfield/star_dot07.png",
            "resources/starfield/star_dot08.png",
            "resources/starfield/star_dot09.png",
            "resources/starfield/star_flare01.png",
            "resources/starfield/star_flare02.png",
            "resources/starfield/star_flare03.png",
            "resources/starfield/star_flare04.png",
            "resources/starfield/star_flare05.png",
            "resources/starfield/star_flare06.png",
            "resources/starfield/star_flare07.png",
            "resources/starfield/star_flare08.png",
            "resources/starfield/star_flare09.png"
        };
        Uint32 const starfield_sprite_filename_count =
            sizeof(s_starfield_sprite_filenames) / sizeof(std::string);
    
        Uint32 const number_of_stars_to_create = 1000;
        for (Uint32 i = 0; i < number_of_stars_to_create; ++i)
        {
            Engine2::Sprite *sprite =
                Engine2::Sprite::Create(
                    s_starfield_sprite_filenames[
                        Math::RandomUint16(0, starfield_sprite_filename_count-1)]);
            sprite->SetTranslation(
                Math::RandomFloat(-0.5f*object_layer_side_length, 0.5f*object_layer_side_length),
                Math::RandomFloat(-0.5f*object_layer_side_length, 0.5f*object_layer_side_length));
            sprite->SetScaleFactor(
                Math::RandomFloat(
                    0.00125f*object_layer_side_length,
                    0.0025f*object_layer_side_length));
            sprite->SetAngle(Math::RandomAngle());
    
            AddObject(sprite, object_layer);
        }
    }

    // closer nebulas
    if (false)
    {
        Float object_layer_side_length = 100000.0f;
    
        // create an object layer
        Engine2::ObjectLayer *object_layer =
            Engine2::ObjectLayer::Create(
                this,                     // owner world
                true,                     // wrapped
                object_layer_side_length, // side length
                3,                        // visibility quad tree depth
                0.0f);                    // z depth
        object_layer->SetZDepth(10000.0f);
        AddObjectLayer(object_layer);

        static std::string const s_nebula_sprite_filenames[] =
        {
            "resources/nebulas/eta_carinae.png",
            "resources/nebulas/planetary_nebula_NGC6751.png",
            "resources/nebulas/reflection_nebula.png",
            "resources/nebulas/small_magellanic_cloud.png"
        };
        Uint32 const nebula_sprite_filename_count =
            sizeof(s_nebula_sprite_filenames) / sizeof(std::string);
    
        for (Uint32 i = 0; i < nebula_sprite_filename_count; ++i)
        {
            FloatVector2 translation;
            Float scale_factor;

            Uint32 retry_count = 0;
            do
            {
                translation.SetComponents(
                    Math::RandomFloat(-0.5f*object_layer_side_length, 0.5f*object_layer_side_length),
                    Math::RandomFloat(-0.5f*object_layer_side_length, 0.5f*object_layer_side_length));
                scale_factor = Math::RandomFloat(0.2f*object_layer_side_length, 0.3f*object_layer_side_length);

                ++retry_count;
            }
            while (retry_count < 50 && object_layer->GetDoesAreaOverlapAnyObject(translation, 1.2f * scale_factor));
            if (retry_count >= 50)
                break;
            
            Engine2::Sprite *sprite =
//                 Engine2::Sprite::Create(s_nebula_sprite_filenames[i]);
                Engine2::Sprite::Create(s_nebula_sprite_filenames[Math::RandomUint16(0, nebula_sprite_filename_count-1)]);
            sprite->SetTranslation(translation);
            sprite->SetScaleFactor(scale_factor);
            sprite->SetAngle(Math::RandomAngle());

            AddObject(sprite, object_layer);
        }
    }
}

} // end of namespace Dis
