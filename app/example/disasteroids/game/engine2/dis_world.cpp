// ///////////////////////////////////////////////////////////////////////////
// dis_world.cpp by Victor Dods, created 2006/01/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_world.hpp"

#include "dis_armor.hpp"
#include "dis_asteroid.hpp"
#include "dis_demi.hpp"
#include "dis_devourment.hpp"
#include "dis_enemyship.hpp"
#include "dis_engine.hpp"
#include "dis_entity.hpp"
#include "dis_interloper.hpp"
#include "dis_physicshandler.hpp"
#include "dis_playership.hpp"
#include "dis_powereddevice.hpp"
#include "dis_powergenerator.hpp"
#include "dis_revulsion.hpp"
#include "dis_shade.hpp"
#include "dis_shield.hpp"
#include "dis_solitary.hpp"
#include "dis_spawn.hpp"
#include "dis_weapon.hpp"
#include "dis_worldview.hpp"
#include "xrb_engine2_objectlayer.hpp"
#include "xrb_engine2_physicshandler.hpp"
#include "xrb_engine2_sprite.hpp"
#include "xrb_eventqueue.hpp"

using namespace Xrb;

namespace Dis
{

// number of each type/level of enemy to spawn
// the threshold of remaining non-demi non-devourment enemies to end the wave
// maximum amount of time this wave will last
// post-wave pause duration
// bonus score for completing the wave?
struct Wave
{
    // Devourments don't count when ending a wave from killing enough
    // enemy ships, therefore, the spawn count for Devourment is an
    // upper limit on the counts per wave.
    // the array size 3 is for Interloper, Shade and Revulsion
    Uint32 const m_enemy_ship_spawn_count[ET_ENEMY_SHIP_COUNT][EnemyShip::ENEMY_LEVEL_COUNT];
    // a negative value here indicates no threshold
    // (so there better be a max wave duration)
    Float const m_enemy_ship_threshold;
    // valid values are any non-negative number
    Float const m_wave_intermission_duration;
    // true iff the world will notify each newly spawned enemy of
    // the player ship as a target.
    bool const m_notify_new_spawns_of_target;
}; // end of struct World::Wave

static Wave const gs_wave[] =
{
    { // wave 0
        {
            {   0,   0,   0,   0 }, // Interloper
            {   0,   0,   0,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   0,   0,   0,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.0f,   // enemy ship threshold
        25.0f,  // wave intermission duration
        false   // notify new spawns of target
    },
    {
        {
            {  25,   0,   0,   0 }, // Interloper
            {   0,   0,   0,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   2,   0,   0,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.15f,  // enemy ship threshold
        7.0f,   // wave intermission duration
        false   // notify new spawns of target
    },
    {
        {
            {   0,   0,   0,   0 }, // Interloper
            {  25,   0,   0,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   2,   0,   0,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.15f,  // enemy ship threshold
        10.0f,  // wave intermission duration
        false   // notify new spawns of target
    },
    {
        {
            {   0,   5,   0,   0 }, // Interloper
            {   0,   0,   0,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   2,   0,   0,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.21f,  // enemy ship threshold
        5.0f,   // wave intermission duration
        true    // notify new spawns of target
    },
    {
        {
            {  10,   0,   0,   0 }, // Interloper
            {  10,   0,   0,   0 }, // Shade
            {  10,   0,   0,   0 }, // Revulsion
            {   2,   0,   0,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.15f,  // enemy ship threshold
        5.0f,   // wave intermission duration
        false   // notify new spawns of target
    },
    {
        {
            {   0,   0,   2,   0 }, // Interloper
            {   0,   0,   0,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   2,   0,   0,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.51f,  // enemy ship threshold
        0.0f,   // wave intermission duration
        true    // notify new spawns of target
    },
    {
        {
            {  40,   0,   0,   0 }, // Interloper
            {   0,   0,   0,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   2,   0,   0,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.21f,  // enemy ship threshold
        3.0f,   // wave intermission duration
        true    // notify new spawns of target
    },
    {
        {
            {   0,   0,   0,   0 }, // Interloper
            {   0,   0,   0,   0 }, // Shade
            {   8,   6,   0,   0 }, // Revulsion
            {   2,   0,   0,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.15f,  // enemy ship threshold
        3.0f,   // wave intermission duration
        false   // notify new spawns of target
    },
    {
        { // 1ST DEMI WAVE
            {   0,   0,   0,   0 }, // Interloper
            {   0,   0,   0,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   1,   2,   0,   0 }, // Devourment
            {   1,   0,   0,   0 }  // Demi
        },
        0.0f,   // enemy ship threshold
        20.0f,  // wave intermission duration
        true    // notify new spawns of target
    },
    {
        {
            {   0,   5,   0,   0 }, // Interloper
            {   0,   0,   0,   0 }, // Shade
            {  20,   0,   0,   0 }, // Revulsion
            {   1,   2,   0,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.15f,  // enemy ship threshold
        5.0f,   // wave intermission duration
        false   // notify new spawns of target
    },
    {
        {
            {   0,   0,   0,   0 }, // Interloper
            {   0,   0,   4,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   1,   2,   1,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.0f,   // enemy ship threshold
        3.0f,   // wave intermission duration
        true    // notify new spawns of target
    },
    {
        {
            {   0,  20,   0,   0 }, // Interloper
            {   0,   2,   0,   0 }, // Shade
            {   5,   0,   0,   0 }, // Revulsion
            {   0,   2,   1,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.15f,  // enemy ship threshold
        10.0f,  // wave intermission duration
        false   // notify new spawns of target
    },
    {
        {
            {   0,   5,   0,   0 }, // Interloper
            {   0,  25,   0,   0 }, // Shade
            {   5,   0,   0,   0 }, // Revulsion
            {   0,   2,   1,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.15f,  // enemy ship threshold
        11.0f,  // wave intermission duration
        false   // notify new spawns of target
    },
    {
        {
            {   0,   0,   3,   0 }, // Interloper
            {   0,   0,   0,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   0,   2,   1,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.0f,   // enemy ship threshold
        5.0f,   // wave intermission duration
        true    // notify new spawns of target
    },
    {
        {
            {   0,  13,   0,   0 }, // Interloper
            {   0,  13,   0,   0 }, // Shade
            {   0,  13,   0,   0 }, // Revulsion
            {   0,   2,   1,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.15f,  // enemy ship threshold
        8.0f,   // wave intermission duration
        false   // notify new spawns of target
    },
    {
        { // 2ND DEMI WAVE
            {   0,   0,   0,   0 }, // Interloper
            {   0,   0,   0,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   0,   2,   1,   0 }, // Devourment
            {   0,   1,   0,   0 }  // Demi
        },
        0.0f,   // enemy ship threshold
        20.0f,  // wave intermission duration
        true    // notify new spawns of target
    },
    {
        {
            {   0,   0,   0,   0 }, // Interloper
            {   0,   0,  20,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   0,   2,   1,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.2f,   // enemy ship threshold
        6.0f,   // wave intermission duration
        false   // notify new spawns of target
    },
    {
        {
            {   0,   0,  10,   0 }, // Interloper
            {   0,   0,   0,   0 }, // Shade
            {   0,   0,   6,   0 }, // Revulsion
            {   0,   2,   1,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.2f,   // enemy ship threshold
        15.0f,  // wave intermission duration
        false   // notify new spawns of target
    },
    {
        {
            {   0,   0,   0,   0 }, // Interloper
            {   0,   0,   0,   0 }, // Shade
            {   0,  20,   0,   0 }, // Revulsion
            {   0,   0,   2,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.1f,   // enemy ship threshold
        4.0f,   // wave intermission duration
        true    // notify new spawns of target
    },
    {
        {
            {  10,  10,  10,   1 }, // Interloper
            {   0,   0,   0,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   0,   0,   2,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.2f,   // enemy ship threshold
        8.0f,   // wave intermission duration
        false   // notify new spawns of target
    },
    {
        {
            {   0,   0,   0,   0 }, // Interloper
            {  10,  10,  10,   1 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   0,   0,   2,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.2f,   // enemy ship threshold
        8.0f,   // wave intermission duration
        false   // notify new spawns of target
    },
    {
        {
            {   0,   0,   0,   0 }, // Interloper
            {   0,   0,   0,   0 }, // Shade
            {  10,  10,  10,   1 }, // Revulsion
            {   0,   0,   2,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.2f,   // enemy ship threshold
        8.0f,   // wave intermission duration
        false   // notify new spawns of target
    },
    {
        { // 3RD DEMI WAVE
            {   0,   0,   0,   0 }, // Interloper
            {   0,   0,   0,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   0,   0,   2,   0 }, // Devourment
            {   0,   0,   1,   0 }  // Demi
        },
        0.0f,   // enemy ship threshold
        20.0f,  // wave intermission duration
        true    // notify new spawns of target
    },
    {
        {
            {   0,   5,   5,   0 }, // Interloper
            {   0,   5,   5,   0 }, // Shade
            {   0,   5,   5,   0 }, // Revulsion
            {   0,   0,   2,   1 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.2f,   // enemy ship threshold
        10.0f,  // wave intermission duration
        false   // notify new spawns of target
    },
    {
        {
            {  50,   0,   0,   0 }, // Interloper
            {   0,   0,   0,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   0,   0,   2,   1 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.1f,   // enemy ship threshold
        3.0f,   // wave intermission duration
        true    // notify new spawns of target
    },
    {
        {
            {   0,   5,   0,   0 }, // Interloper
            {   0,   0,  10,  10 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   0,   0,   2,   1 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.2f,   // enemy ship threshold
        9.0f,   // wave intermission duration
        false   // notify new spawns of target
    },
    {
        {
            {   0,   0,   0,   0 }, // Interloper
            {   0,  30,   0,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   0,   0,   2,   1 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.1f,   // enemy ship threshold
        6.0f,   // wave intermission duration
        true    // notify new spawns of target
    },
    {
        {
            {   0,   0,   8,   0 }, // Interloper
            {   0,   2,   8,   0 }, // Shade
            {   5,   8,  12,   2 }, // Revulsion
            {   0,   0,   2,   1 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.2f,   // enemy ship threshold
        7.0f,   // wave intermission duration
        false   // notify new spawns of target
    },
    {
        {
            {   0,   0,   0,   0 }, // Interloper
            {   0,   0,   0,   5 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   0,   0,   2,   1 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.21f,  // enemy ship threshold
        3.0f,   // wave intermission duration
        false   // notify new spawns of target
    },
    {
        { // 4TH DEMI WAVE
            {   0,   0,   0,   0 }, // Interloper
            {   0,   0,   0,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   0,   0,   2,   1 }, // Devourment
            {   0,   0,   0,   1 }  // Demi
        },
        0.2f,   // enemy ship threshold
        20.0f,  // wave intermission duration
        false   // notify new spawns of target
    }/*,
    {
        {
            {   0,   0,   0,   0 }, // Interloper
            {   0,   0,   0,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   0,   0,   1,   2 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.2f,   // enemy ship threshold
        5.0f,   // wave intermission duration
        false   // notify new spawns of target
    },
*/
};
static Uint32 const gs_wave_count = LENGTHOF(gs_wave);

Float const World::ms_asteroid_mineral_content_factor[World::MINERAL_CONTENT_LEVEL_COUNT] = { 0.37f, 0.37f, 0.37f, 0.37f };

World::~World ()
{
    // if the ship exists and has been removed from the world, add
    // it back in, so it gets deleted properly.
    // TODO: think of a better way to handle this
    if (m_player_ship != NULL && !m_player_ship->IsInWorld())
    {
        m_player_ship->AddBackIntoWorld();
    }
}

World *World::Create (DifficultyLevel difficulty_level, Uint32 entity_capacity)
{
    ASSERT1(difficulty_level < DL_COUNT);
    ASSERT1(entity_capacity > 0);
    return new World(
        difficulty_level,
        new PhysicsHandler(),
        entity_capacity);
}

Engine2::Circle::PhysicsHandler *World::GetPhysicsHandler ()
{
    return DStaticCast<Engine2::Circle::PhysicsHandler *>(m_physics_handler);
}

void World::SubmitScoreDone ()
{
    ScheduleStateMachineInput(IN_SUBMIT_SCORE_DONE, 0.0f);
}

void World::RecordDestroyedPlayerShip (PlayerShip const *player_ship)
{
    ASSERT1(player_ship != NULL);
    ASSERT1(m_player_ship == player_ship);
    ScheduleStateMachineInput(IN_PLAYER_SHIP_DIED, 0.0f);
}

void World::RecordCreatedAsteroids (Uint32 created_asteroid_count, Float created_asteroids_mass)
{
    ASSERT1(created_asteroid_count > 0);
    ASSERT1(created_asteroids_mass > 0.0f);
    m_asteroid_count += created_asteroid_count;
    m_asteroid_mass += created_asteroids_mass;
}

void World::RecordDestroyedAsteroid (Asteroid const *asteroid)
{
    ASSERT1(asteroid != NULL);
    ASSERT1(m_asteroid_count > 0);
    ASSERT1(m_asteroid_mass >= asteroid->Mass());
    --m_asteroid_count;
    m_asteroid_mass -= asteroid->Mass();
}

void World::RecordCreatedEnemyShip (EnemyShip const *enemy_ship)
{
    ASSERT1(enemy_ship != NULL);
    Uint32 enemy_ship_index = enemy_ship->GetEntityType() - ET_ENEMY_SHIP_LOWEST;
    ASSERT1(enemy_ship_index < ET_ENEMY_SHIP_COUNT);
//     ASSERT1(m_is_demi_wave
//             ||
//             m_enemy_ship_count[enemy_ship_index][enemy_ship->EnemyLevel()] <
//             gs_wave[m_current_wave_index].m_enemy_ship_spawn_count[enemy_ship_index][enemy_ship->EnemyLevel()]);

    ++m_enemy_ship_count[enemy_ship_index][enemy_ship->EnemyLevel()];
    if (enemy_ship->GetEntityType() != ET_DEVOURMENT)
    {
        if (m_enemy_ship_left_to_spawn[enemy_ship_index][enemy_ship->EnemyLevel()] > 0)
            --m_enemy_ship_left_to_spawn[enemy_ship_index][enemy_ship->EnemyLevel()];
    }
    else
    {
        ASSERT1(m_devourment_count < m_devourment_max);
        ++m_devourment_count;
    }
}

void World::RecordDestroyedEnemyShip (EnemyShip const *enemy_ship)
{
    ASSERT1(enemy_ship != NULL);
    Uint32 enemy_ship_index = enemy_ship->GetEntityType() - ET_ENEMY_SHIP_LOWEST;
    ASSERT1(enemy_ship_index < ET_ENEMY_SHIP_COUNT);
    ASSERT1(m_enemy_ship_count[enemy_ship_index][enemy_ship->EnemyLevel()] > 0);

    --m_enemy_ship_count[enemy_ship_index][enemy_ship->EnemyLevel()];
    if (enemy_ship->GetEntityType() != ET_DEVOURMENT)
    {
        if (m_enemy_ship_left_to_destroy[enemy_ship_index][enemy_ship->EnemyLevel()] > 0)
        {
            --m_enemy_ship_left_to_destroy[enemy_ship_index][enemy_ship->EnemyLevel()];
            if (m_enemy_ship_wave_left > 0)
                --m_enemy_ship_wave_left;
        }
    }
    else
    {
        ASSERT1(m_devourment_count > 0);
        --m_devourment_count;
    }
}

World::World (
    DifficultyLevel difficulty_level,
    Engine2::Circle::PhysicsHandler *physics_handler,
    Uint32 entity_capacity)
    :
    Engine2::World(physics_handler, entity_capacity),
    SignalHandler(),
    m_state_machine(this),
    m_sender_submit_score(this),
    m_sender_end_game(this),
    m_internal_sender_is_alert_wave(this),
    m_internal_sender_begin_intro(this),
    m_internal_sender_begin_death_rattle(this),
    m_internal_sender_begin_game_over(this),
    m_internal_sender_begin_outro(this),
    m_internal_receiver_begin_wave(&World::BeginWave, this),
    m_internal_receiver_end_game(&World::EndGame, this),
    m_internal_receiver_end_intro(&World::EndIntro, this),
    m_internal_receiver_end_outro(&World::EndOutro, this)
{
    ASSERT1(difficulty_level < DL_COUNT);

    m_player_ship = NULL;
    m_difficulty_level = difficulty_level;
    m_score_required_for_extra_life = 50000;

    m_asteroid_count = 0;
    m_asteroid_mass = 0.0f;
    m_next_asteroid_spawn_time = 0.0f;
    m_asteroid_mineral_level = 0;
    m_next_asteroid_mineral_level_time = 1.0f * 60.0f;
    m_asteroid_mineral_content_level = 0;
    m_next_asteroid_mineral_content_level_time = 1.0f * 60.0f;

    m_current_wave_index = 0;
    m_is_demi_wave = false;
    for (Uint8 enemy_ship_index = 0; enemy_ship_index < ET_ENEMY_SHIP_COUNT; ++enemy_ship_index)
        for (Uint8 enemy_level = 0; enemy_level < EnemyShip::ENEMY_LEVEL_COUNT; ++enemy_level)
            m_enemy_ship_count[enemy_ship_index][enemy_level] = 0;
    m_enemy_ship_wave_total = 0;
    m_enemy_ship_wave_left = 0;
    m_devourment_max = 0;
    m_devourment_count = 0;

    CreateAndPopulateBackgroundObjectLayers();
    CreateAndPopulateForegroundObjectLayer();

    ASSERT1(m_player_ship != NULL);

    // don't initialize the state machine just yet.  wait until
    // HandleFrame, so that the WorldViews will be active
}

bool World::HandleEvent (Event const *e)
{
    ASSERT1(e != NULL);

    if (e->GetEventType() == Event::STATE_MACHINE_INPUT)
    {
        m_state_machine.RunCurrentState(DStaticCast<EventStateMachineInput const *>(e)->GetInput());
        return true;
    }

    return Engine2::World::HandleEvent(e);
}

void World::HandleFrame ()
{
    Engine2::World::HandleFrame();

    if (!m_state_machine.IsInitialized())
        m_state_machine.Initialize(&World::StateIntro);

    m_state_machine.RunCurrentState(IN_PROCESS_FRAME);
}

void World::HandleAttachWorldView (Engine2::WorldView *engine2_world_view)
{
    WorldView *dis_world_view = DStaticCast<WorldView *>(engine2_world_view);

    dis_world_view->SetPlayerShip(m_player_ship);
    // connect the worldview's begin-next-wave signal (cheating for development)
    SignalHandler::Connect0(
        dis_world_view->SenderBeginNextWave(),
        &m_internal_receiver_begin_wave);
    // connect the worldview's set-is-alert-wave signal
    SignalHandler::Connect1(
        &m_internal_sender_is_alert_wave,
        dis_world_view->ReceiverSetIsAlertWave());
    // connect the worldview's begin intro/death-rattle/game-over/outro signals
    SignalHandler::Connect0(
        &m_internal_sender_begin_intro,
        dis_world_view->ReceiverBeginIntro());
    SignalHandler::Connect0(
        &m_internal_sender_begin_death_rattle,
        dis_world_view->ReceiverBeginDeathRattle());
    SignalHandler::Connect0(
        &m_internal_sender_begin_game_over,
        dis_world_view->ReceiverBeginGameOver());
    SignalHandler::Connect0(
        &m_internal_sender_begin_outro,
        dis_world_view->ReceiverBeginOutro());
    // connect the worldview's alert zoom done signal
    SignalHandler::Connect0(
        dis_world_view->SenderAlertZoomDone(),
        &m_internal_receiver_begin_wave);
    // connect the worldview's end game signal
    SignalHandler::Connect0(
        dis_world_view->SenderEndGame(),
        &m_internal_receiver_end_game);
    // connect the worldview's end intro/outro signals
    SignalHandler::Connect0(
        dis_world_view->SenderEndIntro(),
        &m_internal_receiver_end_intro);
    SignalHandler::Connect0(
        dis_world_view->SenderEndOutro(),
        &m_internal_receiver_end_outro);
}

// ///////////////////////////////////////////////////////////////////////////
// begin state machine stuff
// ///////////////////////////////////////////////////////////////////////////

#define STATE_MACHINE_STATUS(state_name) \
    if (input == SM_ENTER) \
        fprintf(stderr, "World: --> " state_name "\n"); \
    else if (input == SM_EXIT) \
        fprintf(stderr, "World: <-- " state_name "\n"); \
    else if (input != IN_PROCESS_FRAME) \
        fprintf(stderr, "World: input: %u\n", input);

#define TRANSITION_TO(x) m_state_machine.SetNextState(&World::x)

bool World::StateIntro (StateMachineInput input)
{
    STATE_MACHINE_STATUS("StateIntro")
    switch (input)
    {
        case SM_ENTER:
            // delay starting the intro a short while
            ScheduleStateMachineInput(IN_BEGIN_INTRO, 0.1f);
            return true;

        case IN_PROCESS_FRAME:
            return true;

        case IN_BEGIN_INTRO:
            // signal the WorldViews that the intro has started
            m_internal_sender_begin_intro.Signal();
            return true;

        case IN_END_INTRO:
            TRANSITION_TO(StateWaveInitialize);
            return true;

        case IN_PLAYER_SHIP_DIED:
            TRANSITION_TO(StateCheckLivesRemaining);
            return true;
    }
    return false;
}

bool World::StateSpawnPlayerShip (StateMachineInput input)
{
    STATE_MACHINE_STATUS("StateSpawnPlayerShip")
    switch (input)
    {
        case IN_PROCESS_FRAME:
            ASSERT1(m_player_ship != NULL);
            ASSERT1(!m_player_ship->IsInWorld());
            ASSERT1(m_player_ship->IsDead());
            ASSERT1(m_player_ship->LivesRemaining() > 0);
            m_player_ship->Revive(FrameTime(), FrameDT());
            m_player_ship->SetVelocity(FloatVector2::ms_zero);
            // TODO place the player ship so it doesn't intersect anything
            m_player_ship->AddBackIntoWorld();
            m_player_ship->IncrementLivesRemaining(-1);
//             TRANSITION_TO(StateNormalGameplay);
            return true;
    }
    return false;
}

bool World::StateWaveInitialize (StateMachineInput input)
{
    STATE_MACHINE_STATUS("StateWaveInitialize")
    switch (input)
    {
        case SM_ENTER:
            m_enemy_ship_wave_total = 0;
            m_devourment_max = 0;
            m_is_demi_wave = false;
            for (Uint32 enemy_level = 0; enemy_level < EnemyShip::ENEMY_LEVEL_COUNT; ++enemy_level)
                if (gs_wave[m_current_wave_index].m_enemy_ship_spawn_count[ET_DEMI - ET_ENEMY_SHIP_LOWEST][enemy_level] > 0)
                    m_is_demi_wave = true;
            SetIsAlertWave(m_is_demi_wave || gs_wave[m_current_wave_index].m_notify_new_spawns_of_target);
            for (Uint8 enemy_ship_index = 0; enemy_ship_index < ET_ENEMY_SHIP_COUNT; ++enemy_ship_index)
            {
                for (Uint8 enemy_level = 0; enemy_level < EnemyShip::ENEMY_LEVEL_COUNT; ++enemy_level)
                {
                    if (enemy_ship_index + ET_ENEMY_SHIP_LOWEST != ET_DEVOURMENT)
                    {
                        m_enemy_ship_left_to_destroy[enemy_ship_index][enemy_level] =
                            gs_wave[m_current_wave_index].m_enemy_ship_spawn_count[enemy_ship_index][enemy_level];
                        m_enemy_ship_left_to_spawn[enemy_ship_index][enemy_level] =
                            gs_wave[m_current_wave_index].m_enemy_ship_spawn_count[enemy_ship_index][enemy_level] -
                            Min(m_enemy_ship_count[enemy_ship_index][enemy_level],
                                gs_wave[m_current_wave_index].m_enemy_ship_spawn_count[enemy_ship_index][enemy_level]);
                        ASSERT1(m_enemy_ship_left_to_spawn[enemy_ship_index][enemy_level] <=
                                gs_wave[m_current_wave_index].m_enemy_ship_spawn_count[enemy_ship_index][enemy_level]);
                        m_enemy_ship_wave_total += m_enemy_ship_left_to_spawn[enemy_ship_index][enemy_level];
                    }
                    else
                    {
                        ++m_devourment_max;
                    }
                }
            }
            m_enemy_ship_wave_left = m_enemy_ship_wave_total;
            return true;

        case IN_PROCESS_FRAME:
            ProcessWaveIntermissionGameplayLogic();
            return true;

        case IN_BEGIN_WAVE:
            TRANSITION_TO(StateWaveGameplay);
            return true;

        case IN_PLAYER_SHIP_DIED:
            TRANSITION_TO(StateCheckLivesRemaining);
            return true;

        case IN_END_GAME:
            TRANSITION_TO(StateOutro);
            return true;
    }
    return false;
}

bool World::StateWaveGameplay (StateMachineInput input)
{
    STATE_MACHINE_STATUS("StateWaveGameplay")
    switch (input)
    {
        case SM_ENTER:
            if (m_current_wave_index > 0)
                m_player_ship->IncrementWaveCount();
            return true;

        case IN_PROCESS_FRAME:
            ProcessWaveGameplayLogic();
            ASSERT1(gs_wave[m_current_wave_index].m_enemy_ship_threshold >= 0.0f);
            if (m_enemy_ship_wave_left <= gs_wave[m_current_wave_index].m_enemy_ship_threshold * m_enemy_ship_wave_total)
            {
                ScheduleStateMachineInput(IN_END_WAVE, 0.0f);
            }
            return true;

        // this is only possible by using the dev cheat command
        case IN_BEGIN_WAVE:
            if (m_current_wave_index < gs_wave_count-1)
                ++m_current_wave_index;
            TRANSITION_TO(StateWaveInitialize);
            return true;

        case IN_END_WAVE:
            TRANSITION_TO(StateWaveIntermissionGameplay);
            return true;

        case IN_PLAYER_SHIP_DIED:
            TRANSITION_TO(StateCheckLivesRemaining);
            return true;

        case IN_END_GAME:
            TRANSITION_TO(StateOutro);
            return true;
    }
    return false;
}

bool World::StateWaveIntermissionGameplay (StateMachineInput input)
{
    STATE_MACHINE_STATUS("StateWaveIntermissionGameplay")
    switch (input)
    {
        case SM_ENTER:
            ASSERT1(gs_wave[m_current_wave_index].m_wave_intermission_duration >= 0.0f);
            ScheduleStateMachineInput(IN_END_WAVE_INTERMISSION, gs_wave[m_current_wave_index].m_wave_intermission_duration);
            if (m_current_wave_index < gs_wave_count-1)
                ++m_current_wave_index;
            return true;

        case IN_PROCESS_FRAME:
            ProcessWaveIntermissionGameplayLogic();
            return true;

        // this is only possible by using the dev cheat command
        case IN_BEGIN_WAVE:
            if (m_current_wave_index < gs_wave_count-1)
                ++m_current_wave_index;
            TRANSITION_TO(StateWaveInitialize);
            return true;

        case IN_END_WAVE_INTERMISSION:
            TRANSITION_TO(StateWaveInitialize);
            return true;

        case IN_PLAYER_SHIP_DIED:
            TRANSITION_TO(StateCheckLivesRemaining);
            return true;

        case IN_END_GAME:
            TRANSITION_TO(StateOutro);
            return true;
    }
    return false;
}

bool World::StateCheckLivesRemaining (StateMachineInput input)
{
    STATE_MACHINE_STATUS("StateCheckLivesRemaining")
    switch (input)
    {
        case SM_ENTER:
            ASSERT1(m_player_ship != NULL);
            if (m_player_ship->LivesRemaining() > 0)
                TRANSITION_TO(StateWaitAfterPlayerDeath);
            else
                TRANSITION_TO(StateDeathRattle);
            return true;
    }
    return false;
}

bool World::StateWaitAfterPlayerDeath (StateMachineInput input)
{
    STATE_MACHINE_STATUS("StateWaitAfterPlayerDeath")
    switch (input)
    {
        case SM_ENTER:
            ScheduleStateMachineInput(IN_WAIT_AFTER_PLAYER_DEATH_DONE, 3.0f);
            return true;

        case IN_WAIT_AFTER_PLAYER_DEATH_DONE:
            ASSERT0(false); // TODO: stuff later
//             TRANSITION_TO(StateSpawnPlayerShip);
            return true;

        case IN_PROCESS_FRAME:
            return true;
    }
    return false;
}

bool World::StateDeathRattle (StateMachineInput input)
{
    STATE_MACHINE_STATUS("StateDeathRattle")
    switch (input)
    {
        case SM_ENTER:
            // signal the worldview that the death rattle has started
            m_internal_sender_begin_death_rattle.Signal();
            ScheduleStateMachineInput(IN_WAIT_AFTER_FINAL_PLAYER_DEATH_DONE, 3.0f);
            return true;

        case IN_WAIT_AFTER_FINAL_PLAYER_DEATH_DONE:
            TRANSITION_TO(StateGameOver);
            return true;

        case IN_PROCESS_FRAME:
            return true;
    }
    return false;
}

bool World::StateGameOver (StateMachineInput input)
{
    STATE_MACHINE_STATUS("StateGameOver")
    switch (input)
    {
        case SM_ENTER:
            // signal the worldview that game over has started
            m_internal_sender_begin_game_over.Signal();
            ScheduleStateMachineInput(IN_GAME_OVER_DONE, 3.0f);
            return true;

        case IN_GAME_OVER_DONE:
            TRANSITION_TO(StateSubmitScore);
            return true;

        case IN_PROCESS_FRAME:
            return true;
    }
    return false;
}

bool World::StateSubmitScore (StateMachineInput input)
{
    STATE_MACHINE_STATUS("StateSubmitScore")
    switch (input)
    {
        case SM_ENTER:
            // send the player's score to Master
            m_sender_submit_score.Signal(
                Score(
                    "",
                    m_player_ship->GetScore(),
                    m_player_ship->WaveCount(),
                    time(NULL)));
            TRANSITION_TO(StateWaitingForSubmitScoreResponse);
            return true;
    }
    return false;
}

bool World::StateWaitingForSubmitScoreResponse (StateMachineInput input)
{
    STATE_MACHINE_STATUS("StateWaitingForSubmitScoreResponse")
    switch (input)
    {
        case IN_SUBMIT_SCORE_DONE:
            TRANSITION_TO(StateOutro);
            return true;

        case IN_PROCESS_FRAME:
            return true;
    }
    return false;
}

bool World::StateOutro (StateMachineInput input)
{
    STATE_MACHINE_STATUS("StateOutro")
    switch (input)
    {
        case SM_ENTER:
            // signal the WorldViews that the outro has started
            m_internal_sender_begin_outro.Signal();
            return true;

        case IN_PLAYER_SHIP_DIED:
            // ignore this
            return true;

        case IN_END_OUTRO:
            TRANSITION_TO(StateEndGame);
            return true;

        case IN_PROCESS_FRAME:
            return true;

        case SM_EXIT:
            // TODO: signal to the worldviews that the outro is ending
            return true;
    }
    return false;
}

bool World::StateEndGame (StateMachineInput input)
{
    STATE_MACHINE_STATUS("StateEndGame")
    switch (input)
    {
        case SM_ENTER:
            m_sender_end_game.Signal();
            return true;

        case IN_PLAYER_SHIP_DIED:
            // ignore this
            return true;

        case IN_PROCESS_FRAME:
            return true;
    }
    return false;
}

void World::ScheduleStateMachineInput (StateMachineInput input, Float time_delay)
{
    CancelScheduledStateMachineInput();
    EnqueueEvent(new EventStateMachineInput(input, MostRecentFrameTime() + time_delay));
}

void World::CancelScheduledStateMachineInput ()
{
    OwnerEventQueue()->ScheduleMatchingEventsForDeletion(
        MatchEventType,
        Event::STATE_MACHINE_INPUT);
}

// ///////////////////////////////////////////////////////////////////////////
// end state machine stuff
// ///////////////////////////////////////////////////////////////////////////

void World::ProcessWaveGameplayLogic ()
{
    ProcessCommonGameplayLogic();

    // enemy ship spawning (this behaves differently when the world runs
    // at different framerates, but it shouldn't matter)
    for (Uint8 enemy_ship_index = 0; enemy_ship_index < ET_ENEMY_SHIP_COUNT; ++enemy_ship_index)
    {
        if (enemy_ship_index + ET_ENEMY_SHIP_LOWEST == ET_DEVOURMENT)
            continue;

        for (Uint8 enemy_level = 0; enemy_level < EnemyShip::ENEMY_LEVEL_COUNT; ++enemy_level)
        {
            if (m_enemy_ship_count[enemy_ship_index][enemy_level] <
                gs_wave[m_current_wave_index].m_enemy_ship_spawn_count[enemy_ship_index][enemy_level]
                &&
                m_enemy_ship_left_to_spawn[enemy_ship_index][enemy_level] > 0)
            {
                SpawnEnemyShipOutOfView(
                    static_cast<EntityType>(ET_ENEMY_SHIP_LOWEST + enemy_ship_index),
                    enemy_level);
            }
        }
    }

    {
        Uint8 enemy_ship_index = ET_DEVOURMENT - ET_ENEMY_SHIP_LOWEST;
        ASSERT1(enemy_ship_index < ET_ENEMY_SHIP_COUNT);
        for (Uint8 enemy_level = EnemyShip::ENEMY_LEVEL_COUNT-1; enemy_level < EnemyShip::ENEMY_LEVEL_COUNT; --enemy_level)
        {
            if (m_enemy_ship_count[enemy_ship_index][enemy_level] <
                gs_wave[m_current_wave_index].m_enemy_ship_spawn_count[enemy_ship_index][enemy_level]
                &&
                m_devourment_count < m_devourment_max)
            {
                SpawnEnemyShipOutOfView(
                    static_cast<EntityType>(ET_ENEMY_SHIP_LOWEST + enemy_ship_index),
                    enemy_level);
            }
        }
    }
}

void World::ProcessWaveIntermissionGameplayLogic ()
{
    ProcessCommonGameplayLogic();
}

void World::ProcessCommonGameplayLogic ()
{
    // asteroid spawning
    static Uint32 const s_max_asteroid_count = 80;
    static Float const s_max_asteroid_mass = 16000.0f;
    while (m_asteroid_count < s_max_asteroid_count &&
           m_asteroid_mass < s_max_asteroid_mass &&
           SpawnAsteroidOutOfView() != NULL)
    {
        // hi.
    }

    // update the asteroid mineral level
    if (m_asteroid_mineral_level < Asteroid::MAX_MINERAL_LEVEL &&
        m_next_asteroid_mineral_level_time <= FrameTime())
    {
        ++m_asteroid_mineral_level;
        m_next_asteroid_mineral_level_time = FrameTime() + 60.0f;
    }

    // update the asteroid mineral content level
    if (m_asteroid_mineral_level < World::MINERAL_CONTENT_LEVEL_COUNT - 1 &&
        m_next_asteroid_mineral_content_level_time <= FrameTime())
    {
        ++m_asteroid_mineral_content_level;
        m_next_asteroid_mineral_content_level_time = FrameTime() + 60.0f;
    }

/*
    // extra life handling
    if (m_player_ship->GetScore() >= m_score_required_for_extra_life)
    {
        static Float const s_extra_live_score_factor = 2.0f;
        m_player_ship->IncrementLivesRemaining(1);
        m_score_required_for_extra_life =
            static_cast<Uint32>(s_extra_live_score_factor * m_score_required_for_extra_life);
    }
*/
}

void World::BeginWave ()
{
    ScheduleStateMachineInput(IN_BEGIN_WAVE, 0.0f);
/*  // this was changed to the above when the begin-next-wave dev-cheat command was added, and it might not be necessary
    if (m_state_machine.CurrentState() == &World::StateWaveInitialize)
        ScheduleStateMachineInput(IN_BEGIN_WAVE, 0.0f);
*/
}

void World::EndGame ()
{
    ScheduleStateMachineInput(IN_END_GAME, 0.0f);
}

void World::EndIntro ()
{
    // ignore this signal unless we're in the intro state (because i don't
    // want to have to ignore the state machine input in each event).
    if (m_state_machine.CurrentState() == &World::StateIntro)
        ScheduleStateMachineInput(IN_END_INTRO, 0.0f);
}

void World::EndOutro ()
{
    // ignore this signal unless we're in the intro state (because i don't
    // want to have to ignore the state machine input in each event).
    if (m_state_machine.CurrentState() == &World::StateOutro)
        ScheduleStateMachineInput(IN_END_OUTRO, 0.0f);
}

void World::SetIsAlertWave (bool is_alert_wave)
{
    m_internal_sender_is_alert_wave.Signal(is_alert_wave);
}

Asteroid *World::SpawnAsteroidOutOfView ()
{
    Float object_layer_side_length = MainObjectLayer()->SideLength();
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
        scale_seed = Math::Pow(scale_seed, 3.0f);
        scale_factor = 55.0f * scale_seed + 5.0f;
    }
    while (!IsAreaNotVisibleAndNotOverlappingAnyEntities(translation, scale_factor));

    Float mass = scale_factor * scale_factor;
    FloatVector2 velocity(
        Math::RandomFloat(75.0f, 1500.0f) /
        Math::Sqrt(mass) *
        Math::UnitVector(Math::RandomAngle()));

    ++m_asteroid_count;
    m_asteroid_mass += mass;
    return SpawnAsteroid(
        MainObjectLayer(),
        translation,
        scale_factor,
        mass,
        velocity,
        Math::RandomFloat(0.5f, 1.0f) * ms_asteroid_mineral_content_factor[m_asteroid_mineral_content_level],
        false);
}

EnemyShip *World::SpawnEnemyShipOutOfView (
    EntityType enemy_ship_type,
    Uint8 enemy_level)
{
    ASSERT1(enemy_ship_type >= ET_ENEMY_SHIP_LOWEST);
    ASSERT1(enemy_ship_type <= ET_ENEMY_SHIP_HIGHEST);
    ASSERT1(enemy_level < EnemyShip::ENEMY_LEVEL_COUNT);

    Float object_layer_side_length = MainObjectLayer()->SideLength();
    FloatVector2 translation;
    Float personal_space_radius = Ship::ShipScaleFactor(enemy_ship_type, enemy_level);

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

    EnemyShip *spawned_ship = NULL;
    switch (enemy_ship_type)
    {
        case ET_INTERLOPER:
            spawned_ship = SpawnInterloper(
                MainObjectLayer(),
                translation,
                FloatVector2::ms_zero,
                enemy_level);
            break;

        case ET_SHADE:
            spawned_ship = SpawnShade(
                MainObjectLayer(),
                translation,
                FloatVector2::ms_zero,
                enemy_level);
            break;

        case ET_REVULSION:
            spawned_ship = SpawnRevulsion(
                MainObjectLayer(),
                translation,
                FloatVector2::ms_zero,
                enemy_level);
            break;

        case ET_DEVOURMENT:
            spawned_ship = SpawnDevourment(
                MainObjectLayer(),
                translation,
                FloatVector2::ms_zero,
                enemy_level);
            break;

        case ET_DEMI:
            spawned_ship = SpawnDemi(
                MainObjectLayer(),
                translation,
                FloatVector2::ms_zero,
                enemy_level);
            break;

        default:
            ASSERT1(false && "Invalid EntityType");
            spawned_ship = NULL;
            break;
    }

    if (gs_wave[m_current_wave_index].m_notify_new_spawns_of_target &&
        enemy_ship_type != ET_DEVOURMENT)
    {
        spawned_ship->SetTarget(m_player_ship);
    }
    return spawned_ship;
}

bool World::IsAreaNotVisibleAndNotOverlappingAnyEntities (
    FloatVector2 const &translation,
    Float scale_factor)
{
    Float object_layer_side_length = MainObjectLayer()->SideLength();
    Float half_object_layer_side_length = 0.5f * object_layer_side_length;
    ASSERT1(translation[Dim::X] >= -0.5f * object_layer_side_length);
    ASSERT1(translation[Dim::X] <=  0.5f * object_layer_side_length);
    ASSERT1(translation[Dim::Y] >= -0.5f * object_layer_side_length);
    ASSERT1(translation[Dim::Y] <=  0.5f * object_layer_side_length);
    ASSERT1(scale_factor > 0.0f);

    // check that the area is not in view of any attached WorldView
    for (WorldViewList::iterator it = m_world_view_list.begin(),
                                 it_end = m_world_view_list.end();
         it != it_end;
         ++it)
    {
        WorldView *world_view = DStaticCast<WorldView *>(*it);
        ASSERT1(world_view != NULL);
        FloatVector2 world_view_center(world_view->Center());

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
            (MainObjectLayer()->AdjustedCoordinates(translation, world_view_center) -
             world_view_center).Length();
        // fail if the area is in sight
        if (distance_from_world_view < world_view->ParallaxedViewRadius(MainObjectLayer()) + scale_factor)
            return false;
    }

    // fail if the area overlaps any entity
    if (GetPhysicsHandler()->DoesAreaOverlapAnyEntityInObjectLayer(
            MainObjectLayer(),
            translation,
            scale_factor,
            false))
        return false;

    // if it reached this far, the area is in an acceptable place
    return true;
}

void World::CreateAndPopulateForegroundObjectLayer ()
{
    Float object_layer_side_length = 1000.0f;

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
            object_layer,
            FloatVector2::ms_zero,
            FloatVector2::ms_zero);

    // add weapons and engines to the ship
    {
        bool add_succeeded;
        add_succeeded = m_player_ship->AddItem(new PeaShooter(0));
        ASSERT1(add_succeeded);
        add_succeeded = m_player_ship->AddItem(new Engine(0));
        ASSERT1(add_succeeded);
        add_succeeded = m_player_ship->AddItem(new Armor(0));
        ASSERT1(add_succeeded);
        add_succeeded = m_player_ship->AddItem(new PowerGenerator(0));
        ASSERT1(add_succeeded);
    }
}

void World::CreateAndPopulateBackgroundObjectLayers ()
{
    // galaxies in the distance
    if (false)
    {
        Float object_layer_side_length = 200000.0f;

        // create an object layer
        Engine2::ObjectLayer *object_layer =
            Engine2::ObjectLayer::Create(
                this,                     // owner world
                true,                     // wrapped
                object_layer_side_length, // side length
                4,                        // visibility quad tree depth
                30000.0f);                // z depth
        AddObjectLayer(object_layer);

        static std::string const s_galaxy_sprite_path[] =
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
        static Uint32 const s_galaxy_sprite_path_count = LENGTHOF(s_galaxy_sprite_path);

        Uint32 const number_of_galaxies_to_create = 100;
        for (Uint32 i = 0; i < number_of_galaxies_to_create; ++i)
        {
            Engine2::Sprite *sprite =
                Engine2::Sprite::Create(
                    s_galaxy_sprite_path[
                        Math::RandomUint16(0, s_galaxy_sprite_path_count-1)]);
            sprite->SetTranslation(
                FloatVector2(
                    Math::RandomFloat(-0.5f*object_layer_side_length, 0.5f*object_layer_side_length),
                    Math::RandomFloat(-0.5f*object_layer_side_length, 0.5f*object_layer_side_length)));
            sprite->SetScaleFactor(
                Math::RandomFloat(
                    0.010f*object_layer_side_length,
                    0.015f*object_layer_side_length));
            sprite->SetAngle(Math::RandomAngle());

            AddStaticObject(sprite, object_layer);
        }
    }

    // starfield
    {
        Float object_layer_side_length = 50000.0f;

        // create an object layer
        Engine2::ObjectLayer *object_layer =
            Engine2::ObjectLayer::Create(
                this,                     // owner world
                true,                     // wrapped
                object_layer_side_length, // side length
                6,                        // visibility quad tree depth
                7500.0f);                 // z depth
        AddObjectLayer(object_layer);

        static std::string const s_starfield_sprite_path[] =
        {
            "resources/star0.png",
            "resources/star1.png",
            "resources/star2.png",
            "resources/star3.png",
        };
        static Uint32 const s_starfield_sprite_path_count = LENGTHOF(s_starfield_sprite_path);

        Uint32 const number_of_stars_to_create = 1000;
        for (Uint32 i = 0; i < number_of_stars_to_create; ++i)
        {
            Engine2::Sprite *sprite =
                Engine2::Sprite::Create(
                    s_starfield_sprite_path[
                        Math::RandomUint16(0, s_starfield_sprite_path_count-1)]);
            sprite->SetTranslation(
                FloatVector2(
                    Math::RandomFloat(-0.5f*object_layer_side_length, 0.5f*object_layer_side_length),
                    Math::RandomFloat(-0.5f*object_layer_side_length, 0.5f*object_layer_side_length)));
            sprite->SetScaleFactor(
                Math::RandomFloat(
                    0.0006f*object_layer_side_length,
                    0.002f*object_layer_side_length));
            sprite->SetAngle(Math::RandomAngle());

            AddStaticObject(sprite, object_layer);
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
                10000.0f);                // z depth
        AddObjectLayer(object_layer);

        static std::string const s_nebula_sprite_path[] =
        {
            "resources/nebulas/eta_carinae.png",
            "resources/nebulas/planetary_nebula_NGC6751.png",
            "resources/nebulas/reflection_nebula.png",
            "resources/nebulas/small_magellanic_cloud.png"
        };
        static Uint32 const s_nebula_sprite_path_count = LENGTHOF(s_nebula_sprite_path);

        for (Uint32 i = 0; i < s_nebula_sprite_path_count; ++i)
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
            while (retry_count < 50 && object_layer->DoesAreaOverlapAnyObject(translation, 1.2f * scale_factor));
            if (retry_count >= 50)
                break;

            Engine2::Sprite *sprite =
//                 Engine2::Sprite::Create(s_nebula_sprite_path[i]);
                Engine2::Sprite::Create(s_nebula_sprite_path[Math::RandomUint16(0, s_nebula_sprite_path_count-1)]);
            sprite->SetTranslation(translation);
            sprite->SetScaleFactor(scale_factor);
            sprite->SetAngle(Math::RandomAngle());

            AddStaticObject(sprite, object_layer);
        }
    }
}

} // end of namespace Dis
