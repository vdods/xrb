// ///////////////////////////////////////////////////////////////////////////
// dis_world.h by Victor Dods, created 2006/01/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_WORLD_H_)
#define _DIS_WORLD_H_

#include "xrb_engine2_world.h"

#include <time.h>

#include "dis_entity.h"
#include "dis_highscores.h"
#include "xrb_signalhandler.h"
#include "xrb_statemachine.h"

using namespace Xrb;

/*
world design:

this module controls the game logic for an instantiated game (e.g. spawning
enemies/asteroids, etc), as well as game synchronization over the network,
if it is decided ever to make this a network game.

- control the player as it is killed and respawns
- control the player's score and time alive
- control the [rate of] spawning of asteroids
- control the [rate of] spawning of enemy ships

state machine for game logic

1. create the world
2. spawn the player
3. normal gameplay
4. player is killed (backtrack spawning stage?)
5. if player has remaining lives, delay a bit and then goto 2.
6. game over.  delay a bit and then continue to 7 (destroy the world)
7. destroy the world

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

namespace Dis
{

class Asteroid;
class EnemyShip;
class PlayerShip;
class PhysicsHandler;

class World : public Engine2::World, public SignalHandler
{
public:

    virtual ~World ();

    static World *Create (Uint32 entity_capacity = DEFAULT_ENTITY_CAPACITY);

    inline SignalSender1<Score const &> const *SenderSubmitScore () { return &m_sender_submit_score; }
    inline SignalSender0 const *SenderEndGame () { return &m_sender_end_game; }

    inline PlayerShip *GetPlayerShip () { return m_player_ship; }
    PhysicsHandler *GetPhysicsHandler ();
    inline Uint8 GetAsteroidMineralLevel () const { return m_asteroid_mineral_level; }

    // for use by Master to indicate that the high score processing is done
    // and the world can now transition to StateOutro
    void SubmitScoreDone ();
    
    void RecordDestroyedPlayerShip (PlayerShip const *player_ship);
    
    void RecordCreatedAsteroids (
        Uint32 created_asteroid_count,
        Float created_asteroids_mass);
    void RecordDestroyedAsteroid (Asteroid const *asteroid);

    void RecordCreatedEnemyShip (EntityType enemy_ship_type);
    void RecordDestroyedEnemyShip (EnemyShip const *enemy_ship);
            
protected:

    World (
        Engine2::PhysicsHandler *physics_handler,
        Uint32 entity_capacity = DEFAULT_ENTITY_CAPACITY);

    virtual bool ProcessEventOverride (Event const *e);
    virtual void ProcessFrameOverride ();

    virtual void HandleAttachWorldView (Engine2::WorldView *engine2_world_view);
    
private:

    // ///////////////////////////////////////////////////////////////////////
    // begin state machine stuff

    enum
    {
        IN_PROCESS_FRAME = SM_USER_DEFINED_INPUT_STARTS_AT_THIS_VALUE,
        
        IN_BEGIN_INTRO,
        IN_END_INTRO,
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
    bool StateNormalGameplay (StateMachineInput);
    bool StateCheckLivesRemaining (StateMachineInput);
    bool StateWaitAfterPlayerDeath (StateMachineInput);
    bool StateWaitAfterFinalPlayerDeath (StateMachineInput);
    bool StateGameOver (StateMachineInput);
    bool StateSubmitScore (StateMachineInput);
    bool StateWaitingForSubmitScoreResponse (StateMachineInput);
    bool StateOutro (StateMachineInput);
    bool StateEndGame (StateMachineInput);

    void ScheduleStateMachineInput (StateMachineInput input, Float time_delay);
    void CancelScheduledStateMachineInput ();

    StateMachine<World> m_state_machine;
    
    // end state machine stuff
    // ///////////////////////////////////////////////////////////////////////
    
    void ProcessNormalGameplayLogic ();

    void EndGame ();
    void EndIntro ();
    void EndOutro ();
    
    Asteroid *SpawnAsteroidOutOfView ();
    EnemyShip *SpawnEnemyShipOutOfView (
        EntityType enemy_ship_type,
        Uint8 enemy_level);
    bool IsAreaNotVisibleAndNotOverlappingAnyEntities (
        FloatVector2 const &translation,
        Float scale_factor);

    void CreateAndPopulateBackgroundObjectLayers ();
    void CreateAndPopulateForegroundObjectLayer ();

    enum
    {
        MINERAL_CONTENT_LEVEL_COUNT = 4,
        GAME_STAGE_COUNT = 20,
        DISTRIBUTION_TABLE_SIZE = 10
    };
    
    static Float const ms_asteroid_mineral_content_factor[MINERAL_CONTENT_LEVEL_COUNT];
    static Uint32 const ms_max_enemy_ship_count[GAME_STAGE_COUNT][ET_ENEMY_SHIP_COUNT];
    static Uint8 const ms_enemy_level_distribution_table[GAME_STAGE_COUNT][ET_ENEMY_SHIP_COUNT][DISTRIBUTION_TABLE_SIZE];
    static Float const ms_enemy_spawn_interval[GAME_STAGE_COUNT];
    
    PlayerShip *m_player_ship;
    Uint32 m_score_required_for_extra_life;

    Uint32 m_game_stage;
    Float m_next_game_stage_time;
    Uint32 m_asteroid_count;
    Float m_asteroid_mass;
    Float m_next_asteroid_spawn_time;
    Uint8 m_asteroid_mineral_level;
    Float m_next_asteroid_mineral_level_time;
    Uint8 m_asteroid_mineral_content_level;
    Float m_next_asteroid_mineral_content_level_time;

    Uint32 m_enemy_ship_count[ET_ENEMY_SHIP_COUNT];
    Float m_next_enemy_ship_spawn_time[ET_ENEMY_SHIP_COUNT];

    SignalSender1<Score const &> m_sender_submit_score;
    SignalSender0 m_sender_end_game;

    SignalSender0 m_internal_sender_begin_intro;
    SignalSender0 m_internal_sender_begin_game_over;
    SignalSender0 m_internal_sender_begin_outro;

    SignalReceiver0 m_internal_receiver_end_game;
    SignalReceiver0 m_internal_receiver_end_intro;
    SignalReceiver0 m_internal_receiver_end_outro;
}; // end of class GameController

} // end of namespace Dis

#endif // !defined(_DIS_WORLD_H_)

