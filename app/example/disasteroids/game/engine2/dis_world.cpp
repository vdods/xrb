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
#include "dis_demi.h"
#include "dis_devourment.h"
#include "dis_enemyship.h"
#include "dis_engine.h"
#include "dis_entity.h"
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
#include "xrb_eventqueue.h"

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
        20.0f   // wave intermission duration
    },
    {
        {
            {  40,   0,   0,   0 }, // Interloper
            {   5,   0,   0,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   3,   0,   0,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.15f,  // enemy ship threshold
        5.0f    // wave intermission duration
    },
    {
        {
            {  10,   0,   0,   0 }, // Interloper
            {  40,   0,   0,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   3,   0,   0,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.15f,  // enemy ship threshold
        10.0f   // wave intermission duration
    },
    {
        {
            {   0,   0,   5,   0 }, // Interloper
            {   0,   0,   0,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   3,   0,   0,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.25f,  // enemy ship threshold
        5.0f    // wave intermission duration
    },
    {
        {
            {  10,   0,   0,   0 }, // Interloper
            {  10,   0,   0,   0 }, // Shade
            {  10,   0,   0,   0 }, // Revulsion
            {   3,   0,   0,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.15f,  // enemy ship threshold
        0.0f    // wave intermission duration
    },
    {
        {
            {   0,   0,   0,   0 }, // Interloper
            {   0,   0,   0,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   1,   2,   0,   0 }, // Devourment
            {   1,   0,   0,   0 }  // Demi
        },
        -1.0f,  // enemy ship threshold
        10.0f   // wave intermission duration
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
        5.0f    // wave intermission duration
    },
    {
        {
            {   0,   0,   0,   0 }, // Interloper
            {   0,   0,   5,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   1,   2,   1,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.25f,  // enemy ship threshold
        3.0f    // wave intermission duration
    },
    {
        {
            {   0,  25,   0,   0 }, // Interloper
            {   0,   2,   0,   0 }, // Shade
            {   5,   0,   0,   0 }, // Revulsion
            {   1,   2,   1,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.15f,  // enemy ship threshold
        5.0f    // wave intermission duration
    },
    {
        {
            {   0,   5,   0,   0 }, // Interloper
            {   0,  25,   0,   0 }, // Shade
            {   5,   0,   0,   0 }, // Revulsion
            {   1,   2,   1,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.15f,  // enemy ship threshold
        5.0f    // wave intermission duration
    },
    {
        {
            {   0,   0,   0,   0 }, // Interloper
            {   0,   0,   0,   0 }, // Shade
            {   0,   0,   0,   1 }, // Revulsion
            {   1,   2,   1,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.0f,   // enemy ship threshold
        5.0f    // wave intermission duration
    },
    {
        {
            {   0,  13,   0,   0 }, // Interloper
            {   0,  13,   0,   0 }, // Shade
            {   0,  13,   0,   0 }, // Revulsion
            {   1,   2,   1,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.15f,  // enemy ship threshold
        5.0f    // wave intermission duration
    },
    {
        {
            {   0,   0,   0,   0 }, // Interloper
            {   0,   0,   0,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   1,   2,   0,   0 }, // Devourment
            {   0,   1,   0,   0 }  // Demi
        },
        -1.0f,  // enemy ship threshold
        10.0f   // wave intermission duration
    },
    {
        {
            {   0,   0,   0,   0 }, // Interloper
            {   0,   0,  20,   0 }, // Shade
            {   0,   0,   0,   0 }, // Revulsion
            {   1,   2,   1,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.2f,   // enemy ship threshold
        2.0f    // wave intermission duration
    },
    {
        {
            {   0,   0,  10,   0 }, // Interloper
            {   0,   0,   0,   0 }, // Shade
            {   0,   0,  10,   0 }, // Revulsion
            {   1,   2,   1,   0 }, // Devourment
            {   0,   0,   0,   0 }  // Demi
        },
        0.2f,   // enemy ship threshold
        2.0f    // wave intermission duration
    }
};
static Uint32 const gs_wave_count = sizeof(gs_wave) / sizeof(Wave);

Float const World::ms_asteroid_mineral_content_factor[World::MINERAL_CONTENT_LEVEL_COUNT] = { 0.35f, 0.4f, 0.5f, 0.6f };

World::~World ()
{
    // if the ship exists and has been removed from the world, add
    // it back in, so it gets deleted properly.
    // TODO: think of a better way to handle this
    if (m_player_ship != NULL && !m_player_ship->GetIsInWorld())
    {
        m_player_ship->AddBackIntoWorld();
    }
}

World *World::Create (Uint32 entity_capacity)
{
    ASSERT1(entity_capacity > 0)
    return new World(new PhysicsHandler(), entity_capacity);
}

PhysicsHandler *World::GetPhysicsHandler ()
{
    return DStaticCast<Dis::PhysicsHandler *>(m_physics_handler);
}

void World::SubmitScoreDone ()
{
    ScheduleStateMachineInput(IN_SUBMIT_SCORE_DONE, 0.0f);
}

void World::RecordDestroyedPlayerShip (PlayerShip const *const player_ship)
{
    ASSERT1(player_ship != NULL)
    ASSERT1(m_player_ship == player_ship)
    ScheduleStateMachineInput(IN_PLAYER_SHIP_DIED, 0.0f);
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

void World::RecordCreatedEnemyShip (EnemyShip const *const enemy_ship)
{
    ASSERT1(enemy_ship != NULL)
    Uint32 enemy_ship_index = enemy_ship->GetEntityType() - ET_ENEMY_SHIP_LOWEST;
    ASSERT1(enemy_ship_index < ET_ENEMY_SHIP_COUNT)
    ASSERT1(m_is_demi_wave
            ||
            m_enemy_ship_count[enemy_ship_index][enemy_ship->GetEnemyLevel()] <
            gs_wave[m_current_wave_index].m_enemy_ship_spawn_count[enemy_ship_index][enemy_ship->GetEnemyLevel()])

    ++m_enemy_ship_count[enemy_ship_index][enemy_ship->GetEnemyLevel()];
    if (enemy_ship->GetEntityType() != ET_DEVOURMENT)
        if (m_enemy_ship_left[enemy_ship_index][enemy_ship->GetEnemyLevel()] > 0)
            --m_enemy_ship_left[enemy_ship_index][enemy_ship->GetEnemyLevel()];
}

void World::RecordDestroyedEnemyShip (EnemyShip const *const enemy_ship)
{
    ASSERT1(enemy_ship != NULL)
    Uint32 enemy_ship_index = enemy_ship->GetEntityType() - ET_ENEMY_SHIP_LOWEST;
    ASSERT1(enemy_ship_index < ET_ENEMY_SHIP_COUNT)
    ASSERT1(m_enemy_ship_count[enemy_ship_index][enemy_ship->GetEnemyLevel()] > 0)

    --m_enemy_ship_count[enemy_ship_index][enemy_ship->GetEnemyLevel()];
    if (enemy_ship->GetEntityType() != ET_DEVOURMENT)
        if (m_enemy_ship_wave_left > 0)
            --m_enemy_ship_wave_left;
}

World::World (
    Engine2::PhysicsHandler *physics_handler,
    Uint32 entity_capacity)
    :
    Engine2::World(physics_handler, entity_capacity),
    SignalHandler(),
    m_state_machine(this),
    m_sender_submit_score(this),
    m_sender_end_game(this),
    m_internal_sender_begin_intro(this),
    m_internal_sender_begin_death_rattle(this),
    m_internal_sender_begin_game_over(this),
    m_internal_sender_begin_outro(this),
    m_internal_receiver_end_game(&World::EndGame, this),
    m_internal_receiver_end_intro(&World::EndIntro, this),
    m_internal_receiver_end_outro(&World::EndOutro, this)
{
    m_player_ship = NULL;
    m_score_required_for_extra_life = 50000;

    m_asteroid_count = 0;
    m_asteroid_mass = 0.0f;
    m_next_asteroid_spawn_time = 0.0f;
    m_asteroid_mineral_level = 0;
    m_next_asteroid_mineral_level_time = 1.0f * 60.0f;
    m_asteroid_mineral_content_level = 0;
    m_next_asteroid_mineral_content_level_time = 1.0f * 60.0f;

    m_current_wave_index = 0;
    for (Uint8 enemy_ship_index = 0; enemy_ship_index < ET_ENEMY_SHIP_COUNT; ++enemy_ship_index)
        for (Uint8 enemy_level = 0; enemy_level < EnemyShip::ENEMY_LEVEL_COUNT; ++enemy_level)
            m_enemy_ship_count[enemy_ship_index][enemy_level] = 0;
    m_enemy_ship_wave_total = 0;
    m_enemy_ship_wave_left = 0;

    CreateAndPopulateBackgroundObjectLayers();
    CreateAndPopulateForegroundObjectLayer();

    ASSERT1(m_player_ship != NULL)

    // don't initialize the state machine just yet.  wait until
    // ProcessFrameOverride, so that the WorldViews will be active
}

bool World::ProcessEventOverride (Event const *const e)
{
    ASSERT1(e != NULL)

    if (e->GetEventType() == Event::STATE_MACHINE_INPUT)
    {
        m_state_machine.RunCurrentState(DStaticCast<EventStateMachineInput const *>(e)->GetInput());
        return true;
    }

    return Engine2::World::ProcessEventOverride(e);
}

void World::ProcessFrameOverride ()
{
    Engine2::World::ProcessFrameOverride();

    if (!m_state_machine.GetIsInitialized())
        m_state_machine.Initialize(&World::StateIntro);

    m_state_machine.RunCurrentState(IN_PROCESS_FRAME);
}

void World::HandleAttachWorldView (Engine2::WorldView *const engine2_world_view)
{
    WorldView *dis_world_view = DStaticCast<WorldView *>(engine2_world_view);

    dis_world_view->SetPlayerShip(m_player_ship);
    // connect the worldview's end game signal
    SignalHandler::Connect0(
        dis_world_view->SenderEndGame(),
        &m_internal_receiver_end_game);
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

bool World::StateIntro (StateMachineInput const input)
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
    }
    return false;
}

bool World::StateSpawnPlayerShip (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StateSpawnPlayerShip")
    switch (input)
    {
        case IN_PROCESS_FRAME:
            ASSERT1(m_player_ship != NULL)
            ASSERT1(!m_player_ship->GetIsInWorld())
            ASSERT1(m_player_ship->GetIsDead())
            ASSERT1(m_player_ship->GetLivesRemaining() > 0)
            m_player_ship->Revive(GetFrameTime(), GetFrameDT());
            m_player_ship->SetVelocity(FloatVector2::ms_zero);
            // TODO place the player ship so it doesn't intersect anything
            m_player_ship->AddBackIntoWorld();
            m_player_ship->IncrementLivesRemaining(-1);
//             TRANSITION_TO(StateNormalGameplay);
            return true;
    }
    return false;
}

bool World::StateWaveInitialize (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StateWaveInitialize")
    switch (input)
    {
        case SM_ENTER:
            fprintf(stderr, "m_current_wave_index = %u\n", m_current_wave_index);
            m_enemy_ship_wave_total = 0;
            m_is_demi_wave = false;
            for (Uint32 enemy_level = 0; enemy_level < EnemyShip::ENEMY_LEVEL_COUNT; ++enemy_level)
                if (gs_wave[m_current_wave_index].m_enemy_ship_spawn_count[ET_DEMI - ET_ENEMY_SHIP_LOWEST][enemy_level] > 0)
                    m_is_demi_wave = true;
            for (Uint8 enemy_ship_index = 0; enemy_ship_index < ET_ENEMY_SHIP_COUNT; ++enemy_ship_index)
            {
                for (Uint8 enemy_level = 0; enemy_level < EnemyShip::ENEMY_LEVEL_COUNT; ++enemy_level)
                {
                    if (enemy_ship_index + ET_ENEMY_SHIP_LOWEST != ET_DEVOURMENT)
                    {
                        m_enemy_ship_left[enemy_ship_index][enemy_level] =
                            gs_wave[m_current_wave_index].m_enemy_ship_spawn_count[enemy_ship_index][enemy_level] -
                            Min(m_enemy_ship_count[enemy_ship_index][enemy_level],
                                gs_wave[m_current_wave_index].m_enemy_ship_spawn_count[enemy_ship_index][enemy_level]);
                        ASSERT1(m_enemy_ship_left[enemy_ship_index][enemy_level] <=
                                gs_wave[m_current_wave_index].m_enemy_ship_spawn_count[enemy_ship_index][enemy_level])
                        m_enemy_ship_wave_total +=
                            gs_wave[m_current_wave_index].m_enemy_ship_spawn_count[enemy_ship_index][enemy_level];
                    }
                }
            }
            m_enemy_ship_wave_left += m_enemy_ship_wave_total;
            TRANSITION_TO(StateWaveGameplay);
            return true;
    }
    return false;
}

bool World::StateWaveGameplay (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StateWaveGameplay")
    switch (input)
    {
        case IN_PROCESS_FRAME:
            ProcessWaveGameplayLogic();
            if (m_is_demi_wave)
            {
                bool all_demis_killed = true;
                for (Uint32 enemy_level = 0; enemy_level < EnemyShip::ENEMY_LEVEL_COUNT; ++enemy_level)
                {
                    if (m_enemy_ship_left[ET_DEMI - ET_ENEMY_SHIP_LOWEST][enemy_level] > 0 ||
                        m_enemy_ship_count[ET_DEMI - ET_ENEMY_SHIP_LOWEST][enemy_level] > 0)
                    {
                        all_demis_killed = false;
                    }
                }
                if (all_demis_killed)
                    ScheduleStateMachineInput(IN_END_WAVE, 0.0f);
            }
            else if (gs_wave[m_current_wave_index].m_enemy_ship_threshold >= 0.0f &&
                     m_enemy_ship_wave_left <= gs_wave[m_current_wave_index].m_enemy_ship_threshold * m_enemy_ship_wave_total)
            {
                ScheduleStateMachineInput(IN_END_WAVE, 0.0f);
            }
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

bool World::StateWaveIntermissionGameplay (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StateWaveIntermissionGameplay")
    switch (input)
    {
        case SM_ENTER:
            ASSERT1(gs_wave[m_current_wave_index].m_wave_intermission_duration >= 0.0f)
            ScheduleStateMachineInput(IN_END_WAVE_INTERMISSION, gs_wave[m_current_wave_index].m_wave_intermission_duration);
            if (m_current_wave_index < gs_wave_count-1)
                ++m_current_wave_index;
            return true;

        case IN_PROCESS_FRAME:
            ProcessWaveIntermissionGameplayLogic();
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

bool World::StateCheckLivesRemaining (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StateCheckLivesRemaining")
    switch (input)
    {
        case SM_ENTER:
            ASSERT1(m_player_ship != NULL)
            if (m_player_ship->GetLivesRemaining() > 0)
                TRANSITION_TO(StateWaitAfterPlayerDeath);
            else
                TRANSITION_TO(StateDeathRattle);
            return true;
    }
    return false;
}

bool World::StateWaitAfterPlayerDeath (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StateWaitAfterPlayerDeath")
    switch (input)
    {
        case SM_ENTER:
            ScheduleStateMachineInput(IN_WAIT_AFTER_PLAYER_DEATH_DONE, 3.0f);
            return true;

        case IN_WAIT_AFTER_PLAYER_DEATH_DONE:
            ASSERT0(false) // TODO: stuff later
//             TRANSITION_TO(StateSpawnPlayerShip);
            return true;

        case IN_PROCESS_FRAME:
            m_player_ship->IncrementTimeAlive(GetFrameDT());
            return true;
    }
    return false;
}

bool World::StateDeathRattle (StateMachineInput const input)
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

bool World::StateGameOver (StateMachineInput const input)
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

bool World::StateSubmitScore (StateMachineInput const input)
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
                    m_player_ship->GetTimeAlive(),
                    time(NULL)));
            TRANSITION_TO(StateWaitingForSubmitScoreResponse);
            return true;
    }
    return false;
}

bool World::StateWaitingForSubmitScoreResponse (StateMachineInput const input)
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

bool World::StateOutro (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StateOutro")
    switch (input)
    {
        case SM_ENTER:
            // signal the WorldViews that the outro has started
            m_internal_sender_begin_outro.Signal();
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

bool World::StateEndGame (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StateEndGame")
    switch (input)
    {
        case SM_ENTER:
            m_sender_end_game.Signal();
            return true;

        case IN_PROCESS_FRAME:
            return true;
    }
    return false;
}

void World::ScheduleStateMachineInput (StateMachineInput const input, Float const time_delay)
{
    CancelScheduledStateMachineInput();
    EnqueueEvent(new EventStateMachineInput(input, GetMostRecentFrameTime() + time_delay));
}

void World::CancelScheduledStateMachineInput ()
{
    GetOwnerEventQueue()->ScheduleMatchingEventsForDeletion(
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
        for (Uint8 enemy_level = 0; enemy_level < EnemyShip::ENEMY_LEVEL_COUNT; ++enemy_level)
        {
            if (m_enemy_ship_count[enemy_ship_index][enemy_level] <
                gs_wave[m_current_wave_index].m_enemy_ship_spawn_count[enemy_ship_index][enemy_level]
                &&
                (enemy_ship_index + ET_ENEMY_SHIP_LOWEST == ET_DEVOURMENT
                 ||
                 enemy_ship_index + ET_ENEMY_SHIP_LOWEST != ET_DEVOURMENT &&
                 m_enemy_ship_left[enemy_ship_index][enemy_level] > 0))
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
    m_player_ship->IncrementTimeAlive(GetFrameDT());

    // asteroid spawning
    static Uint32 const s_max_asteroid_count = 80;
    static Float const s_max_asteroid_mass = 16000.0f;
    while (m_asteroid_count < s_max_asteroid_count &&
           m_asteroid_mass < s_max_asteroid_mass &&
           SpawnAsteroidOutOfView() != NULL)
    {
        // hi.
    }

    /*
    if (m_next_asteroid_spawn_time <= GetFrameTime() &&
        m_asteroid_count < s_max_asteroid_count &&
        m_asteroid_mass < s_max_asteroid_mass)
    {
        Asteroid *asteroid = SpawnAsteroidOutOfView();
        if (asteroid != NULL)
            m_next_asteroid_spawn_time = GetFrameTime() + Math::RandomFloat(0.5f, 1.0f);
    }
    */

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

    // extra life handling
/*
    if (m_player_ship->GetScore() >= m_score_required_for_extra_life)
    {
        static Float const s_extra_live_score_factor = 2.0f;
        m_player_ship->IncrementLivesRemaining(1);
        m_score_required_for_extra_life =
            static_cast<Uint32>(s_extra_live_score_factor * m_score_required_for_extra_life);
    }
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
    if (m_state_machine.GetCurrentState() == &World::StateIntro)
        ScheduleStateMachineInput(IN_END_INTRO, 0.0f);
}

void World::EndOutro ()
{
    // ignore this signal unless we're in the intro state (because i don't
    // want to have to ignore the state machine input in each event).
    if (m_state_machine.GetCurrentState() == &World::StateOutro)
        ScheduleStateMachineInput(IN_END_OUTRO, 0.0f);
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
        scale_seed = Math::Pow(scale_seed, 2.5f);
        scale_factor = 55.0f * scale_seed + 5.0f;
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
        Math::RandomFloat(0.5f, 1.0f) * ms_asteroid_mineral_content_factor[m_asteroid_mineral_content_level],
        false);
}

EnemyShip *World::SpawnEnemyShipOutOfView (
    EntityType const enemy_ship_type,
    Uint8 const enemy_level)
{
    ASSERT1(enemy_ship_type >= ET_ENEMY_SHIP_LOWEST)
    ASSERT1(enemy_ship_type <= ET_ENEMY_SHIP_HIGHEST)
    ASSERT1(enemy_level < EnemyShip::ENEMY_LEVEL_COUNT)

    Float object_layer_side_length = GetMainObjectLayer()->GetSideLength();
    FloatVector2 translation;
    Float personal_space_radius = Ship::GetShipScaleFactor(enemy_ship_type, enemy_level);

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
        case ET_INTERLOPER:
            return SpawnInterloper(
                this,
                GetMainObjectLayer(),
                translation,
                FloatVector2::ms_zero,
                enemy_level);

        case ET_SHADE:
            return SpawnShade(
                this,
                GetMainObjectLayer(),
                translation,
                FloatVector2::ms_zero,
                enemy_level);

        case ET_REVULSION:
            return SpawnRevulsion(
                this,
                GetMainObjectLayer(),
                translation,
                FloatVector2::ms_zero,
                enemy_level);

        case ET_DEVOURMENT:
            return SpawnDevourment(
                this,
                GetMainObjectLayer(),
                translation,
                FloatVector2::ms_zero,
                enemy_level);

        case ET_DEMI:
            return SpawnDemi(
                this,
                GetMainObjectLayer(),
                translation,
                FloatVector2::ms_zero,
                enemy_level);

        default:
            ASSERT1(false && "Invalid EntityType")
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
        FloatVector2 world_view_center(world_view->GetCenter());

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
    if (GetPhysicsHandler()->GetDoesAreaOverlapAnyEntityInObjectLayer(
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
        add_succeeded = m_player_ship->AddItem(new Armor(0));
        ASSERT1(add_succeeded)
        add_succeeded = m_player_ship->AddItem(new PowerGenerator(0));
        ASSERT1(add_succeeded)
    }
/*
    static Uint32 const s_number_of_asteroids_to_spawn = 40;
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
        while (GetPhysicsHandler()->
                    GetDoesAreaOverlapAnyEntityInObjectLayer(
                        GetMainObjectLayer(),
                        translation,
                        scale_factor + 40.0f,
                        false));
        if (placement_attempts == s_max_placement_attempts)
            continue;

        Float first_moment = scale_factor * scale_factor;
        FloatVector2 velocity(
            Math::RandomFloat(100.0f, 2000.0f) /
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
            Math::RandomFloat(0.5f, 1.0f) * ms_asteroid_mineral_content_factor[m_asteroid_mineral_content_level],
            false);
    }
*/
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
                0.0f);                    // z depth
        object_layer->SetZDepth(7500.0f);
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
                FloatVector2(
                    Math::RandomFloat(-0.5f*object_layer_side_length, 0.5f*object_layer_side_length),
                    Math::RandomFloat(-0.5f*object_layer_side_length, 0.5f*object_layer_side_length)));
            sprite->SetScaleFactor(
                Math::RandomFloat(
                    0.002f*object_layer_side_length,
                    0.003f*object_layer_side_length));
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

            AddStaticObject(sprite, object_layer);
        }
    }
}

} // end of namespace Dis
