// ///////////////////////////////////////////////////////////////////////////
// dis_world.hpp by Victor Dods, created 2006/01/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_WORLD_HPP_)
#define _DIS_WORLD_HPP_

#include "xrb_engine2_world.hpp"

#include "dis_enemyship.hpp"
#include "dis_entity.hpp"
#include "dis_highscores.hpp"
#include "xrb_signalhandler.hpp"
#include "xrb_statemachine.hpp"

using namespace Xrb;

/*

wave spawning:

each wave is made up of:
    number of each type/level of enemy to spawn
    the threshold of remaining enemies before spawning the boss(es)
    maximum amount of time before spawnining the boss(es)
    number of each type/level of boss to spawn
    post-wave pause duration
    bonus score for completing the wave?

//////////////////////////////////////////////////////////////////////////////
// stuff to be considered if disasteroids was going to be made network-aware
//////////////////////////////////////////////////////////////////////////////

- control the player's buying of upgrades

//////////////////////////////////////////////////////////////////////////////

notes:
                               (^^^^^^^^^^^^^^^^^^^^^^^^^)
user <--> worldview <--> world ( <-network-> serverworld )
                               (_________________________)
*/

namespace Xrb {
namespace Engine2 {
namespace Circle {

class PhysicsHandler;

} // end of namespace Circle
} // end of namespace Engine2
} // end of namespace Xrb

namespace Dis {

class Asteroid;
class EnemyShip;
class PlayerShip;

class World : public Engine2::World, public SignalHandler
{
public:

    virtual ~World ();

    static World *Create (
        DifficultyLevel difficulty_level,
        Uint32 entity_capacity = DEFAULT_ENTITY_CAPACITY);

    SignalSender1<Score const &> const *SenderSubmitScore () { return &m_sender_submit_score; }
    SignalSender0 const *SenderEndGame () { return &m_sender_end_game; }

    PlayerShip *GetPlayerShip () { return m_player_ship; }
    DifficultyLevel GetDifficultyLevel () const
    {
        ASSERT1(m_difficulty_level < DL_COUNT);
        return m_difficulty_level;
    }
    Engine2::Circle::PhysicsHandler *GetPhysicsHandler ();
    Uint8 AsteroidMineralLevel () const { return m_asteroid_mineral_level; }

    // for use by Master to indicate that the high score processing is done
    // and the world can now transition to StateOutro
    void SubmitScoreDone ();

    void RecordDestroyedPlayerShip (PlayerShip const *player_ship);

    void RecordCreatedAsteroids (Uint32 created_asteroid_count, Float created_asteroids_mass);
    void RecordDestroyedAsteroid (Asteroid const *asteroid);
    // when this amount drops below zero, an option powerup should be spawned.
    bool SpawnOptionPowerupFromAsteroid () const { return m_destroyed_asteroid_area_left_before_spawning_option_powerup <= 0.0f; }
    void ResetSpawnOptionPowerupFromAsteroid ();

    void RecordCreatedEnemyShip (EnemyShip const *enemy_ship);
    void RecordDestroyedEnemyShip (EnemyShip const *enemy_ship);

protected:

    World (
        DifficultyLevel difficulty_level,
        Engine2::Circle::PhysicsHandler *physics_handler,
        Uint32 entity_capacity = DEFAULT_ENTITY_CAPACITY);

    virtual bool HandleEvent (Event const &e);
    virtual void HandleFrame ();

    virtual void HandleAttachWorldView (Engine2::WorldView *engine2_world_view);

private:

    // ///////////////////////////////////////////////////////////////////////
    // begin state machine stuff

    enum
    {
        IN_PROCESS_FRAME = 0,

        IN_BEGIN_INTRO,
        IN_END_INTRO,
        IN_BEGIN_WAVE,
        IN_END_WAVE,
        IN_END_WAVE_INTERMISSION,
        IN_PLAYER_SHIP_DIED,
        IN_END_GAME,
        IN_WAIT_AFTER_PLAYER_DEATH_DONE,
        IN_WAIT_AFTER_FINAL_PLAYER_DEATH_DONE,
        IN_GAME_OVER_DONE,
        IN_SUBMIT_SCORE_DONE,
        IN_END_OUTRO,
    };

    bool StateIntro (StateMachineInput);
    bool StateSpawnPlayerShip (StateMachineInput);
    bool StateWaveInitialize (StateMachineInput);
    bool StateWaveGameplay (StateMachineInput);
    bool StateWaveIntermissionGameplay (StateMachineInput);
    bool StateCheckLivesRemaining (StateMachineInput);
    bool StateWaitAfterPlayerDeath (StateMachineInput);
    bool StateDeathRattle (StateMachineInput);
    bool StateGameOver (StateMachineInput);
    bool StateSubmitScore (StateMachineInput);
    bool StateWaitingForSubmitScoreResponse (StateMachineInput);
    bool StateOutro (StateMachineInput);
    bool StateEndGame (StateMachineInput);

    void ScheduleStateMachineInput (StateMachineInput input, Time::Delta time_delay);
    void CancelScheduledStateMachineInput ();

    StateMachine<World> m_state_machine;

    // end state machine stuff
    // ///////////////////////////////////////////////////////////////////////

    void ProcessWaveGameplayLogic ();
    void ProcessWaveIntermissionGameplayLogic ();
    void ProcessCommonGameplayLogic ();

    void AdvanceWaveState ();

    void EndGame ();
    void EndIntro ();
    void EndOutro ();

    void SetIsAlertWave (bool is_alert_wave);

    Asteroid *SpawnAsteroidOutOfView ();
    EnemyShip *SpawnEnemyShipOutOfView (EntityType enemy_ship_type, Uint8 enemy_level);
    bool IsAreaNotVisibleAndNotOverlappingAnyEntities (FloatVector2 const &translation, Float scale_factor);

    void CreateAndPopulateSuperforegroundObjectLayer ();
    void CreateAndPopulateForegroundObjectLayer ();
    void CreateAndPopulateBackgroundObjectLayers ();

    enum
    {
        MINERAL_CONTENT_LEVEL_COUNT = 4,
        GAME_STAGE_COUNT = 40,
        DISTRIBUTION_TABLE_SIZE = 10
    };

    static Float const ms_asteroid_mineral_content_factor[MINERAL_CONTENT_LEVEL_COUNT];

    PlayerShip *m_player_ship;
    DifficultyLevel m_difficulty_level;
    Uint32 m_score_required_for_extra_life;

    Uint32 m_asteroid_count;
    Float m_asteroid_mass;
    Uint8 m_asteroid_mineral_level;
    Time m_next_asteroid_mineral_level_time;
    Uint8 m_asteroid_mineral_content_level;
    Time m_next_asteroid_mineral_content_level_time;
    Float m_destroyed_asteroid_area_left_before_spawning_option_powerup;

    Uint32 m_current_wave_index;
    Uint32 m_current_wave_index_unwrapped;
    bool m_is_demi_wave;
    // TODO: change into std::vector?
    Uint32 m_enemy_ship_count[ET_ENEMY_SHIP_COUNT][EnemyShip::ENEMY_LEVEL_COUNT];
    Uint32 m_enemy_ship_left_to_spawn[ET_ENEMY_SHIP_COUNT][EnemyShip::ENEMY_LEVEL_COUNT];
    Uint32 m_enemy_ship_left_to_destroy[ET_ENEMY_SHIP_COUNT][EnemyShip::ENEMY_LEVEL_COUNT];
    Uint32 m_enemy_ship_wave_total;
    Uint32 m_enemy_ship_wave_left;
    Uint32 m_devourment_max;
    Uint32 m_devourment_count;

    SignalSender1<Score const &> m_sender_submit_score;
    SignalSender0 m_sender_end_game;

    SignalSender1<bool> m_internal_sender_is_alert_wave;
    SignalSender0 m_internal_sender_begin_intro;
    SignalSender0 m_internal_sender_begin_death_rattle;
    SignalSender0 m_internal_sender_begin_game_over;
    SignalSender0 m_internal_sender_begin_outro;

    SignalReceiver0 m_internal_receiver_advance_wave_state;
    SignalReceiver0 m_internal_receiver_end_game;
    SignalReceiver0 m_internal_receiver_end_intro;
    SignalReceiver0 m_internal_receiver_end_outro;
}; // end of class World

} // end of namespace Dis

#endif // !defined(_DIS_WORLD_HPP_)

