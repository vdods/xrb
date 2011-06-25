// ///////////////////////////////////////////////////////////////////////////
// dis_worldview.hpp by Victor Dods, created 2005/11/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_WORLDVIEW_HPP_)
#define _DIS_WORLDVIEW_HPP_

#include "xrb_engine2_worldview.hpp"
#include "xrb_eventhandler.hpp"
#include "xrb_key.hpp"
#include "xrb_signalhandler.hpp"
#include "xrb_statemachine.hpp"

using namespace Xrb;

namespace Dis
{

class InventoryPanel;
class PlayerShip;

class WorldView : public Engine2::WorldView, public EventHandler, public SignalHandler
{
public:

    WorldView (Engine2::WorldViewWidget *parent_world_view_widget);
    virtual ~WorldView ();

    inline SignalSender1<PlayerShip *> const *SenderPlayerShipChanged () { return &m_sender_player_ship_changed; }
    inline SignalSender1<bool> const *SenderIsGameLoopInfoEnabledChanged () { return &m_sender_is_game_loop_info_enabled_changed; }
    inline SignalSender1<bool> const *SenderIsDebugZoomModeEnabledChanged () { return &m_sender_is_debug_zoom_mode_enabled_changed; }
    inline SignalSender0 const *SenderBeginNextWave () { return &m_sender_begin_next_wave; }
    inline SignalSender0 const *SenderShowControls () { return &m_sender_show_controls; }
    inline SignalSender0 const *SenderHideControls () { return &m_sender_hide_controls; }
    inline SignalSender0 const *SenderActivateInventoryPanel () { return &m_sender_activate_inventory_panel; }
    inline SignalSender0 const *SenderDeactivateInventoryPanel () { return &m_sender_deactivate_inventory_panel; }
    inline SignalSender0 const *SenderShowGameOverLabel () { return &m_sender_show_game_over_label; }
    inline SignalSender0 const *SenderHideGameOverLabel () { return &m_sender_hide_game_over_label; }
    inline SignalSender0 const *SenderAlertZoomDone () { return &m_sender_alert_zoom_done; }
    inline SignalSender0 const *SenderEndGame () { return &m_sender_end_game; }
    inline SignalSender0 const *SenderEndIntro () { return &m_sender_end_intro; }
    inline SignalSender0 const *SenderEndOutro () { return &m_sender_end_outro; }

    inline SignalReceiver1<bool> const *ReceiverSetIsAlertWave () { return &m_receiver_set_is_alert_wave; }
    inline SignalReceiver0 const *ReceiverEndGame () { return &m_receiver_end_game; }
    inline SignalReceiver0 const *ReceiverBeginIntro () { return &m_receiver_begin_intro; }
    inline SignalReceiver0 const *ReceiverBeginDeathRattle () { return &m_receiver_begin_death_rattle; }
    inline SignalReceiver0 const *ReceiverBeginGameOver () { return &m_receiver_begin_game_over; }
    inline SignalReceiver0 const *ReceiverBeginOutro () { return &m_receiver_begin_outro; }

    inline PlayerShip *GetPlayerShip () { return m_player_ship; }
    inline bool IsGameLoopInfoEnabled () const { return m_is_game_loop_info_enabled; }
    inline bool IsDebugZoomModeEnabled () const { return m_is_debug_zoom_mode_enabled; }

    void SetPlayerShip (PlayerShip *player_ship);
    void SetIsGameLoopInfoEnabled (bool is_game_loop_info_enabled);
    // a side effect of debug zoom mode is that the waves are halted
    void SetIsDebugZoomModeEnabled (bool is_debug_mode_enabled);

    // NOT ASSOCIATED WITH EventHandler !!
    // these are overrides of Engine2::WorldView methods!
    virtual bool ProcessKeyEvent (EventKey const *e);
    virtual bool ProcessMouseButtonEvent (EventMouseButton const *e);
    virtual bool ProcessMouseWheelEvent (EventMouseWheel const *e);
    virtual bool ProcessMouseMotionEvent (EventMouseMotion const *e);

protected:

    virtual bool HandleEvent (Event const *e);
    virtual void HandleFrame ();

private:

    void HandleInput (Key::Code input);
    void ProcessPlayerInput ();

    void EndGame ();

    void SetIsAlertWave (bool is_alert_wave);

    void BeginIntro ();
    void BeginDeathRattle ();
    void BeginGameOver ();
    void BeginOutro ();

    void InitiateZoom (Float starting_zoom_factor, Float ending_zoom_factor, Float zoom_duration, bool signal_alert_zoom_done);
    void ProcessZoom (Float frame_dt);

    void InitiateSpin (Float starting_spin_rate, Float ending_spin_rate, Float spin_duration);
    void ProcessSpin (Float frame_dt);

    // a fade coefficient of 0 is faded to black, while 1 is not faded.
    void InitiateFade (Float starting_fade_coefficient, Float ending_fade_coefficient, Float fade_duration);
    void ProcessFade (Float frame_dt);

    // ///////////////////////////////////////////////////////////////////////
    // begin state machine stuff

    enum
    {
        IN_PROCESS_FRAME = 0,

        IN_BEGIN_INTRO,
        IN_END_INTRO,
        IN_BEGIN_DEATH_RATTLE,
        IN_BEGIN_GAME_OVER,
        IN_BEGIN_OUTRO,
        IN_END_OUTRO
    };

    bool StatePreIntro (StateMachineInput);
    bool StateIntro (StateMachineInput);
    bool StateNormalGameplay (StateMachineInput);
    bool StateDeathRattle (StateMachineInput);
    bool StateGameOver (StateMachineInput);
    bool StateOutro (StateMachineInput);
    bool StatePostOutro (StateMachineInput);

    void ScheduleStateMachineInput (StateMachineInput input, Float time_delay);
    void CancelScheduledStateMachineInput ();

    StateMachine<WorldView> m_state_machine;

    // end state machine stuff
    // ///////////////////////////////////////////////////////////////////////

    // ///////////////////////////////////////////////////////////////////////
    // static constants

    static Float const ms_zoom_factor_intro_begin;
    static Float const ms_zoom_factor_non_alert_wave;
    static Float const ms_zoom_factor_alert_wave;
    static Float const ms_zoom_factor_outro_end;

    static Float const ms_intro_duration;
    static Float const ms_non_alert_wave_zoom_duration;
    static Float const ms_alert_wave_zoom_duration;
    static Float const ms_outro_duration;

    // ///////////////////////////////////////////////////////////////////////
    // intro/outro vars

    Float m_zoom_time_total;
    Float m_zoom_time_left;
    Float m_zoom_factor_begin;
    Float m_zoom_factor_end;
    bool m_signal_alert_zoom_done;

    Float m_spin_time_total;
    Float m_spin_time_left;
    Float m_spin_rate_begin;
    Float m_spin_rate_end;

    Float m_fade_time_total;
    Float m_fade_time_left;
    // a fade coefficient of 0 is faded to black, while 1 is not faded.
    Float m_fade_coefficient_begin;
    Float m_fade_coefficient_end;

    // ///////////////////////////////////////////////////////////////////////
    // the player's ship

    PlayerShip *m_player_ship;

    // ///////////////////////////////////////////////////////////////////////
    // view movement vars

    // speed coefficient for using the arrow keys to move the view
    Float m_key_movement_speed_factor;
    // used to control how closely the view follows the ship
    Float m_dragging_factor;
    // the view center velocity (used to track the ship smoothly)
    FloatVector2 m_view_velocity;
    // used in interpolating the view center during recovery
    FloatVector2 m_calculated_view_center;
    // indicates if the view positioning is recovering from the
    // ship accelerating suddenly (e.g. hitting an asteroid)
    bool m_is_view_recovering;
    // indicates the current recover parameter in the range of [0, 1]
    // (used for interpolation from bad view position to recovered
    // view position)
    Float m_recover_parameter;

    // ///////////////////////////////////////////////////////////////////////
    // view zooming vars

    // accumulates weighted zoom in/out requests.  zooming in increases
    // this value by m_zoom_increment, zooming out decreases it by the same.
    Float m_zoom_accumulator;
    // the quanta for m_zoom_accumulator.
    Float m_zoom_increment;
    // the rate at which to zoom.
    Float m_zoom_speed;

    // ///////////////////////////////////////////////////////////////////////
    // view rotation vars

    // accumulates weighted rotate requests.  rotating counterclockwise
    // increases this value by m_rotation_increment, while rotating clockwise
    // decreases it by the same.
    Float m_rotation_accumulator;
    // the quanta for m_rotation_accumulator.
    Float m_rotation_increment;
    // the rate at which to rotate.
    Float m_rotation_speed;

    // ///////////////////////////////////////////////////////////////////////
    // other shit

    bool m_is_game_loop_info_enabled;
    bool m_is_debug_zoom_mode_enabled;

    // ///////////////////////////////////////////////////////////////////////
    // SignalSenders

    SignalSender1<PlayerShip *> m_sender_player_ship_changed;
    SignalSender1<bool> m_sender_is_game_loop_info_enabled_changed;
    SignalSender1<bool> m_sender_is_debug_zoom_mode_enabled_changed;
    SignalSender0 m_sender_begin_next_wave;
    SignalSender0 m_sender_show_controls;
    SignalSender0 m_sender_hide_controls;
    SignalSender0 m_sender_activate_inventory_panel;
    SignalSender0 m_sender_deactivate_inventory_panel;
    SignalSender0 m_sender_show_game_over_label;
    SignalSender0 m_sender_hide_game_over_label;
    SignalSender0 m_sender_alert_zoom_done;
    SignalSender0 m_sender_end_game;
    SignalSender0 m_sender_end_intro;
    SignalSender0 m_sender_end_outro;

    // ///////////////////////////////////////////////////////////////////////
    // SignalReceivers

    SignalReceiver0 m_receiver_end_game;
    SignalReceiver1<bool> m_receiver_set_is_alert_wave;
    SignalReceiver0 m_receiver_begin_intro;
    SignalReceiver0 m_receiver_begin_death_rattle;
    SignalReceiver0 m_receiver_begin_game_over;
    SignalReceiver0 m_receiver_begin_outro;
}; // end of class WorldView

} // end of namespace Dis

#endif // !defined(_DIS_WORLDVIEW_HPP_)

