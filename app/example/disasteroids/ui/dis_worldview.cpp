// ///////////////////////////////////////////////////////////////////////////
// dis_worldview.cpp by Victor Dods, created 2005/11/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_worldview.hpp"

#include "dis_config.hpp"
#include "dis_entity.hpp"
#include "dis_playership.hpp"
#include "dis_world.hpp"
#include "xrb_engine2_entity.hpp"
#include "xrb_engine2_objectlayer.hpp"
#include "xrb_engine2_worldviewwidget.hpp"
#include "xrb_eventqueue.hpp"
#include "xrb_input_events.hpp"
#include "xrb_inputstate.hpp"

using namespace Xrb;

extern Dis::Config g_config;

namespace Dis
{

Float const WorldView::ms_zoom_factor_intro_begin = 0.03f;
Float const WorldView::ms_zoom_factor_non_alert_wave = 0.00288f;
Float const WorldView::ms_zoom_factor_alert_wave = 0.0018f;
Float const WorldView::ms_zoom_factor_outro_end = 0.00024f;

Float const WorldView::ms_intro_duration = 2.0f;
Float const WorldView::ms_non_alert_wave_zoom_duration = 1.5f;
Float const WorldView::ms_alert_wave_zoom_duration = 1.5f;
Float const WorldView::ms_outro_duration = 2.0f;

WorldView::WorldView (Engine2::WorldViewWidget *const parent_world_view_widget)
    :
    Engine2::WorldView(parent_world_view_widget),
    EventHandler(parent_world_view_widget->OwnerEventQueue()),
    SignalHandler(),
    m_state_machine(this),
    m_sender_player_ship_changed(this),
    m_sender_is_game_loop_info_enabled_changed(this),
    m_sender_is_debug_zoom_mode_enabled_changed(this),
    m_sender_begin_next_wave(this),
    m_sender_show_controls(this),
    m_sender_hide_controls(this),
    m_sender_activate_inventory_panel(this),
    m_sender_deactivate_inventory_panel(this),
    m_sender_show_game_over_label(this),
    m_sender_hide_game_over_label(this),
    m_sender_alert_zoom_done(this),
    m_sender_end_game(this),
    m_sender_end_intro(this),
    m_sender_end_outro(this),
    m_receiver_end_game(&WorldView::EndGame, this),
    m_receiver_set_is_alert_wave(&WorldView::SetIsAlertWave, this),
    m_receiver_begin_intro(&WorldView::BeginIntro, this),
    m_receiver_begin_death_rattle(&WorldView::BeginDeathRattle, this),
    m_receiver_begin_game_over(&WorldView::BeginGameOver, this),
    m_receiver_begin_outro(&WorldView::BeginOutro, this)
{
    m_zoom_time_total = 0.0f;
    m_zoom_time_left = 0.0f;
    m_zoom_factor_begin = ms_zoom_factor_intro_begin;
    m_zoom_factor_end = ms_zoom_factor_intro_begin;
    m_signal_alert_zoom_done = false;
    SetZoomFactor(ms_zoom_factor_intro_begin);

    m_spin_time_total = 0.0f;
    m_spin_time_left = 0.0f;
    m_spin_rate_begin = 0.0f;
    m_spin_rate_end = 0.0f;

    m_fade_time_total = 0.0f;
    m_fade_time_left = 0.0f;
    m_fade_coefficient_begin = 0.0f;
    m_fade_coefficient_end = 0.0f;

    m_player_ship = NULL;

    m_key_movement_speed_factor = 0.5f;
    m_dragging_factor = 3.0f;
    m_view_velocity = FloatVector2::ms_zero;
    m_is_view_recovering = false;
    m_recover_parameter = 0.0f;

    m_zoom_accumulator = 0.0f;
    m_zoom_increment = pow(2.0f, 0.25f);
    m_zoom_speed = 10.0f;

    m_rotation_accumulator = 0.0f;
    m_rotation_increment = 15.0f;
    m_rotation_speed = 120.0f;

    m_is_game_loop_info_enabled = false;
    m_is_debug_zoom_mode_enabled = false;
    SetDrawBorderGridLines(m_is_debug_zoom_mode_enabled);
    SetIsTransformScalingBasedUponWidgetRadius(true);

    m_state_machine.Initialize(&WorldView::StatePreIntro);
}

WorldView::~WorldView ()
{
}

void WorldView::SetPlayerShip (PlayerShip *const player_ship)
{
    if (m_player_ship != player_ship)
    {
        m_player_ship = player_ship;
        m_sender_player_ship_changed.Signal(m_player_ship);
    }
}

void WorldView::SetIsGameLoopInfoEnabled (bool is_game_loop_info_enabled)
{
    if (m_is_game_loop_info_enabled != is_game_loop_info_enabled)
    {
        m_is_game_loop_info_enabled = is_game_loop_info_enabled;
        m_sender_is_game_loop_info_enabled_changed.Signal(m_is_game_loop_info_enabled);
    }
}

void WorldView::SetIsDebugZoomModeEnabled (bool is_debug_zoom_mode_enabled)
{
    if (m_is_debug_zoom_mode_enabled != is_debug_zoom_mode_enabled)
    {
        m_is_debug_zoom_mode_enabled = is_debug_zoom_mode_enabled;
        SetDrawBorderGridLines(m_is_debug_zoom_mode_enabled);
        m_sender_is_debug_zoom_mode_enabled_changed.Signal(m_is_debug_zoom_mode_enabled);
    }
}

bool WorldView::ProcessKeyEvent (EventKey const *const e)
{
    if (e->IsKeyDownEvent())
        HandleInput(e->KeyCode());
    return true;
}

bool WorldView::ProcessMouseButtonEvent (EventMouseButton const *const e)
{
    if (e->IsMouseButtonDownEvent())
        HandleInput(e->ButtonCode());
    return true;
}

bool WorldView::ProcessMouseWheelEvent (EventMouseWheel const *const e)
{
    // don't allow mouse wheel input while the widget is not focused
    if (!ParentWorldViewWidget()->IsFocused())
        return false;

    HandleInput(e->ButtonCode());

/*
    if (e->IsEitherAltKeyPressed())
    {
        // when the alt key is held down, change the view's rotation

        if (e->ButtonCode() == Key::MOUSEWHEELUP)
        {
            if (m_rotation_accumulator > 0.0f)
                m_rotation_accumulator -=
                    m_rotation_increment *
                    static_cast<Sint32>(m_rotation_accumulator / m_rotation_increment);
            m_rotation_accumulator -= m_rotation_increment;
        }
        else if (e->ButtonCode() == Key::MOUSEWHEELDOWN)
        {
            if (m_rotation_accumulator < 0.0f)
                m_rotation_accumulator -=
                    m_rotation_increment *
                    static_cast<Sint32>(m_rotation_accumulator / m_rotation_increment);
            m_rotation_accumulator += m_rotation_increment;
        }
        else
            ASSERT0(false && "Invalid mouse wheel event (button code not recognized)");
    }
    else
    */
    if (m_is_debug_zoom_mode_enabled)
    {
        // otherwise, change the view's zoom

        if (e->ButtonCode() == Key::MOUSEWHEELUP)
        {
            if (m_zoom_accumulator < 0.0f)
                m_zoom_accumulator -= static_cast<Sint32>(m_zoom_accumulator);
            m_zoom_accumulator += 1.0f;
        }
        else if (e->ButtonCode() == Key::MOUSEWHEELDOWN)
        {
            if (m_zoom_accumulator > 0.0f)
                m_zoom_accumulator -= static_cast<Sint32>(m_zoom_accumulator);
            m_zoom_accumulator -= 1.0f;
        }
        else
            ASSERT0(false && "Invalid mouse wheel event (button code not recognized)");
    }

    return true;
}

bool WorldView::ProcessMouseMotionEvent (EventMouseMotion const *const e)
{
    return true;
}

bool WorldView::HandleEvent (Event const *const e)
{
    ASSERT1(e != NULL);

    if (e->GetEventType() == Event::STATE_MACHINE_INPUT)
    {
        m_state_machine.RunCurrentState(DStaticCast<EventStateMachineInput const *>(e)->GetInput());
        return true;
    }

    return false;
}

void WorldView::HandleFrame ()
{
    m_state_machine.RunCurrentState(IN_PROCESS_FRAME);

    // don't do anything if this view is hidden
    if (ParentWorldViewWidget()->IsHidden())
        return;

    ProcessZoom(FrameDT());
    ProcessSpin(FrameDT());
    ProcessFade(FrameDT());

    // update the view position
    if (m_player_ship != NULL)
    {
        /*
        { // simple dead-on follow
            SetCenter(m_player_ship->UnwrappedTranslation());
            m_view_velocity = m_player_ship->Velocity();
        }
        */
        // lookahead
        if (FrameDT() > 0.0f)
        {
            Float minor_axis_radius = MinorAxisRadius();
            FloatVector2 ship_velocity_direction;
            if (m_player_ship->Velocity().IsZero())
                ship_velocity_direction = FloatVector2::ms_zero;
            else
                ship_velocity_direction = m_player_ship->Velocity().Normalization();
            Float view_distance = 0.8f * minor_axis_radius * Math::Arctan(ms_zoom_factor_alert_wave * m_player_ship->Speed()) / 90.0f;
            FloatVector2 traveling_at(m_player_ship->UnwrappedTranslation() + view_distance * ship_velocity_direction);

            FloatVector2 view_center_delta(traveling_at - (Center() + m_view_velocity * FrameDT()));
            bool is_view_recovering_this_frame;
            Float const max_view_center_delta = 2.4f / ZoomFactor();
            static Float const s_time_to_recover = 0.5f;
            if (view_center_delta.Length() > max_view_center_delta * FrameDT())
            {
                m_view_velocity += view_center_delta.Normalization() * max_view_center_delta * FrameDT();
                is_view_recovering_this_frame = true;
            }
            else
            {
                m_view_velocity = m_player_ship->Velocity();
                is_view_recovering_this_frame = false;
            }

            if (!m_is_view_recovering && is_view_recovering_this_frame)
            {
                m_is_view_recovering = true;
                m_calculated_view_center = Center() + m_view_velocity * FrameDT();
                m_recover_parameter = 0.0f;
            }
            else if (m_is_view_recovering && !is_view_recovering_this_frame)
                m_calculated_view_center = m_calculated_view_center + m_view_velocity * FrameDT();

            if (m_is_view_recovering && m_recover_parameter < 1.0f)
            {
                ASSERT1(m_recover_parameter >= 0.0f);
                SetCenter(
                    m_calculated_view_center * (1.0f - m_recover_parameter) +
                    traveling_at * m_recover_parameter);
                m_recover_parameter += FrameDT() / s_time_to_recover;
            }
            else
            {
                SetCenter(traveling_at);
                m_is_view_recovering = false;
            }
        }
        /*
        { // view dragging
            Float minor_axis_radius = MinorAxisRadius();
            FloatVector2 view_to_ship = Center() - m_player_ship->UnwrappedTranslation();
            Float view_to_ship_ratio = view_to_ship.Length() / (0.5f * minor_axis_radius);
            Float dragging_factor;
            if (view_to_ship_ratio >= 1.0f)
                dragging_factor = m_dragging_factor * view_to_ship_ratio;
            else
                dragging_factor = m_dragging_factor;
            dragging_factor *= FrameDT();

            SetCenter(
                Center() * (1.0f - dragging_factor)
                +
                m_player_ship->UnwrappedTranslation() * dragging_factor);
            m_view_velocity = m_player_ship->Velocity();
        }
        */
    }
    else
    {
        SetCenter(Center() + m_view_velocity * FrameDT());
    }
}

void WorldView::HandleInput (Key::Code input)
{
    if (input == g_config.InputAction(INPUT__IN_GAME_INVENTORY_PANEL))
    {
        // only allowed to use the inventory panel during normal gameplay
        if (m_state_machine.CurrentState() == &WorldView::StateNormalGameplay)
            m_sender_activate_inventory_panel.Signal();
    }
    else if (input == g_config.InputAction(INPUT__EQUIP_PEA_SHOOTER))
    {
        if (m_player_ship != NULL)
            m_player_ship->SetMainWeaponType(IT_WEAPON_PEA_SHOOTER);
    }
    else if (input == g_config.InputAction(INPUT__EQUIP_LASER))
    {
        if (m_player_ship != NULL)
            m_player_ship->SetMainWeaponType(IT_WEAPON_LASER);
    }
    else if (input == g_config.InputAction(INPUT__EQUIP_FLAME_THROWER))
    {
        if (m_player_ship != NULL)
            m_player_ship->SetMainWeaponType(IT_WEAPON_FLAME_THROWER);
    }
    else if (input == g_config.InputAction(INPUT__EQUIP_GAUSS_GUN))
    {
        if (m_player_ship != NULL)
            m_player_ship->SetMainWeaponType(IT_WEAPON_GAUSS_GUN);
    }
    else if (input == g_config.InputAction(INPUT__EQUIP_GRENADE_LAUNCHER))
    {
        if (m_player_ship != NULL)
            m_player_ship->SetMainWeaponType(IT_WEAPON_GRENADE_LAUNCHER);
    }
    else if (input == g_config.InputAction(INPUT__EQUIP_MISSILE_LAUNCHER))
    {
        if (m_player_ship != NULL)
            m_player_ship->SetMainWeaponType(IT_WEAPON_MISSILE_LAUNCHER);
    }
    else if (input == g_config.InputAction(INPUT__ACTIVATE_OPTION__EMP))
    {
        if (m_player_ship != NULL)
            m_player_ship->ActivateOption(INPUT__ACTIVATE_OPTION__EMP, m_player_ship->GetWorld()->MostRecentFrameTime());
    }
    else if (input == g_config.InputAction(INPUT__ACTIVATE_OPTION__ATTACK_BOOST))
    {
        if (m_player_ship != NULL)
            m_player_ship->ActivateOption(INPUT__ACTIVATE_OPTION__ATTACK_BOOST, m_player_ship->GetWorld()->MostRecentFrameTime());
    }
    else if (input == g_config.InputAction(INPUT__ACTIVATE_OPTION__DEFENSE_BOOST))
    {
        if (m_player_ship != NULL)
            m_player_ship->ActivateOption(INPUT__ACTIVATE_OPTION__DEFENSE_BOOST, m_player_ship->GetWorld()->MostRecentFrameTime());
    }
    else if (input == g_config.InputAction(INPUT__ACTIVATE_OPTION__TIME_STRETCH))
    {
        if (m_player_ship != NULL)
            m_player_ship->ActivateOption(INPUT__ACTIVATE_OPTION__TIME_STRETCH, m_player_ship->GetWorld()->MostRecentFrameTime());
    }
    else if (input == g_config.InputAction(INPUT__ACTIVATE_OPTION__SELECTED))
    {
        if (m_player_ship != NULL)
            m_player_ship->ActivateOption(INPUT__ACTIVATE_OPTION__SELECTED, m_player_ship->GetWorld()->MostRecentFrameTime());
    }
    else if (input == g_config.InputAction(INPUT__SELECT_OPTION__NEXT))
    {
        if (m_player_ship != NULL)
            m_player_ship->SelectNextOption();
    }
    else if (input == g_config.InputAction(INPUT__SELECT_OPTION__PREVIOUS))
    {
        if (m_player_ship != NULL)
            m_player_ship->SelectPreviousOption();
    }
    else switch (input)
    {
/*
        case Key::KP_DIVIDE:
            if (m_rotation_accumulator < 0.0f)
                m_rotation_accumulator -=
                    m_rotation_increment *
                    static_cast<Sint32>(m_rotation_accumulator / m_rotation_increment);
            m_rotation_accumulator += m_rotation_increment;
            break;

        case Key::KP_MULTIPLY:
            if (m_rotation_accumulator > 0.0f)
                m_rotation_accumulator -=
                    m_rotation_increment *
                    static_cast<Sint32>(m_rotation_accumulator / m_rotation_increment);
            m_rotation_accumulator -= m_rotation_increment;
            break;

        case Key::KP_PLUS:
            if (m_zoom_accumulator < 0.0f)
                m_zoom_accumulator -= static_cast<Sint32>(m_zoom_accumulator);
            m_zoom_accumulator += 1.0f;
            break;

        case Key::KP_MINUS:
            if (m_zoom_accumulator > 0.0f)
                m_zoom_accumulator -= static_cast<Sint32>(m_zoom_accumulator);
            m_zoom_accumulator -= 1.0f;
            break;
*/

        // ///////////////////////////////////////////////////////////////////
        // begin debug mode keys
        // ///////////////////////////////////////////////////////////////////

        case Key::F1:
            if (g_config.Boolean(SYSTEM__DEBUG_MODE))
                SetIsGameLoopInfoEnabled(!IsGameLoopInfoEnabled());
            break;

        case Key::F2:
            if (g_config.Boolean(SYSTEM__DEBUG_MODE))
                SetIsDebugZoomModeEnabled(!IsDebugZoomModeEnabled());
            break;

        case Key::F3:
            if (g_config.Boolean(SYSTEM__DEBUG_MODE))
                if (m_player_ship != NULL)
                    m_player_ship->GiveAllItems();
            break;

        case Key::F4:
            if (g_config.Boolean(SYSTEM__DEBUG_MODE))
                if (m_player_ship != NULL)
                    m_player_ship->GiveLotsOfMinerals();
            break;

        case Key::F5:
            if (g_config.Boolean(SYSTEM__DEBUG_MODE))
                if (m_player_ship != NULL)
                    m_player_ship->IncrementScore(50000);
            break;

        case Key::F6:
            if (g_config.Boolean(SYSTEM__DEBUG_MODE))
                m_sender_begin_next_wave.Signal();
            break;

        case Key::F7:
            if (g_config.Boolean(SYSTEM__DEBUG_MODE))
                if (m_player_ship != NULL)
                    m_player_ship->SetIsInvincible(!m_player_ship->IsInvincible());
            break;

        case Key::F8:
            if (g_config.Boolean(SYSTEM__DEBUG_MODE))
                if (m_player_ship != NULL && !m_player_ship->IsDead())
                    m_player_ship->Kill(
                        NULL,
                        NULL,
                        FloatVector2::ms_zero,
                        FloatVector2::ms_zero,
                        0.0f,
                        Mortal::D_NONE,
                        m_player_ship->GetWorld()->MostRecentFrameTime(),
                        0.0f);
            break;

        // ///////////////////////////////////////////////////////////////////
        // end debug mode keys
        // ///////////////////////////////////////////////////////////////////

        default:
            break;
    }
}

void WorldView::ProcessPlayerInput ()
{
    // handle view zooming (from accumulated mouse wheel events)
    {
        Float zoom_by_power;

        if (m_zoom_accumulator > 0)
            zoom_by_power = Min(m_zoom_accumulator, m_zoom_speed*FrameDT());
        else if (m_zoom_accumulator < 0)
            zoom_by_power = Max(m_zoom_accumulator, -m_zoom_speed*FrameDT());
        else
            zoom_by_power = 0.0f;

        ZoomView(pow(m_zoom_increment, zoom_by_power));
        m_zoom_accumulator -= zoom_by_power;
    }

    // handle view rotation (from acculumated mouse motion events)
    {
        Float rotate_by_angle;

        if (m_rotation_accumulator > 0)
            rotate_by_angle = Min(m_rotation_accumulator, m_rotation_speed*FrameDT());
        else if (m_rotation_accumulator < 0)
            rotate_by_angle = Max(m_rotation_accumulator, -m_rotation_speed*FrameDT());
        else
            rotate_by_angle = 0.0f;

        RotateView(rotate_by_angle);
        m_rotation_accumulator -= rotate_by_angle;
    }

    // handle player ship input (movement, shooting and reticle coordinates)
    if (ParentWorldViewWidget()->IsFocused())
    {
        Sint8 engine_right_left_input =
            (Singleton::InputState().IsKeyPressed(g_config.InputAction(INPUT__MOVE_RIGHT))    ?  SINT8_UPPER_BOUND : 0) +
            (Singleton::InputState().IsKeyPressed(g_config.InputAction(INPUT__MOVE_LEFT))     ? -SINT8_UPPER_BOUND : 0);
        Sint8 engine_up_down_input =
            (Singleton::InputState().IsKeyPressed(g_config.InputAction(INPUT__MOVE_FORWARD))  ?  SINT8_UPPER_BOUND : 0) +
            (Singleton::InputState().IsKeyPressed(g_config.InputAction(INPUT__MOVE_BACK))     ? -SINT8_UPPER_BOUND : 0);
        Uint8 engine_auxiliary_input =
            Singleton::InputState().IsKeyPressed(g_config.InputAction(INPUT__ENGINE_BRAKE))   ?  UINT8_UPPER_BOUND : 0;

        Uint8 weapon_primary_input =
            Singleton::InputState().IsKeyPressed(g_config.InputAction(INPUT__PRIMARY_FIRE))   ? UINT8_UPPER_BOUND : 0;
        Uint8 weapon_secondary_input =
            Singleton::InputState().IsKeyPressed(g_config.InputAction(INPUT__SECONDARY_FIRE)) ? UINT8_UPPER_BOUND : 0;

        bool is_using_auxiliary_weapon =
            Singleton::InputState().IsKeyPressed(g_config.InputAction(INPUT__USE_TRACTOR));

        if (m_player_ship != NULL)
        {
            m_player_ship->SetReticleCoordinates(
                ParallaxedScreenToWorld() *
                ParentWorldViewWidget()->LastMousePosition().StaticCast<Float>()
                -
                m_player_ship->WrappedOffset());
            m_player_ship->SetEngineRightLeftInput(engine_right_left_input);
            m_player_ship->SetEngineUpDownInput(engine_up_down_input);
            m_player_ship->SetEngineAuxiliaryInput(engine_auxiliary_input);
            m_player_ship->SetWeaponPrimaryInput(weapon_primary_input);
            m_player_ship->SetWeaponSecondaryInput(weapon_secondary_input);
            m_player_ship->SetIsUsingAuxiliaryWeapon(is_using_auxiliary_weapon);
        }
    }
}

void WorldView::EndGame ()
{
    m_sender_end_game.Signal();
}

void WorldView::SetIsAlertWave (bool const is_alert_wave)
{
    if (is_alert_wave)
        InitiateZoom(ZoomFactor(), ms_zoom_factor_alert_wave, ms_alert_wave_zoom_duration, true);
    else
        InitiateZoom(ZoomFactor(), ms_zoom_factor_non_alert_wave, ms_non_alert_wave_zoom_duration, true);
}

void WorldView::BeginIntro ()
{
    ScheduleStateMachineInput(IN_BEGIN_INTRO, 0.0f);
}

void WorldView::BeginDeathRattle ()
{
    ScheduleStateMachineInput(IN_BEGIN_DEATH_RATTLE, 0.0f);
}

void WorldView::BeginGameOver ()
{
    ScheduleStateMachineInput(IN_BEGIN_GAME_OVER, 0.0f);
}

void WorldView::BeginOutro ()
{
    ScheduleStateMachineInput(IN_BEGIN_OUTRO, 0.0f);
}

void WorldView::InitiateZoom (
    Float const starting_zoom_factor,
    Float const ending_zoom_factor,
    Float const zoom_duration,
    bool const signal_alert_zoom_done)
{
    ASSERT1(ending_zoom_factor > 0.0f);
    m_zoom_factor_begin = starting_zoom_factor;
    m_zoom_factor_end = ending_zoom_factor;
    m_zoom_time_total = zoom_duration;
    m_zoom_time_left = zoom_duration;
    m_signal_alert_zoom_done = signal_alert_zoom_done;
}

void WorldView::ProcessZoom (Float const frame_dt)
{
    ASSERT1(m_zoom_factor_begin > 0.0f);
    ASSERT1(m_zoom_factor_end > 0.0f);

    // don't adjust the zoom while in debug mode
    if (m_is_debug_zoom_mode_enabled)
        return;

    if (m_zoom_time_left < 0.0f)
        m_zoom_time_left = 0.0f;

    if (m_zoom_time_left == 0.0f)
    {
        SetZoomFactor(m_zoom_factor_end);
        if (m_signal_alert_zoom_done)
        {
            m_sender_alert_zoom_done.Signal();
            m_signal_alert_zoom_done = false;
        }
    }
    else
    {
        ASSERT1(m_zoom_time_total > 0.0f);
        ASSERT1(m_zoom_time_left <= m_zoom_time_total);
        Float parameter = 1.0f - m_zoom_time_left / m_zoom_time_total;
        SetZoomFactor(m_zoom_factor_begin * (1.0f - parameter) + m_zoom_factor_end * parameter);
        m_zoom_time_left -= frame_dt;
    }
}

void WorldView::InitiateSpin (Float const starting_spin_rate, Float const ending_spin_rate, Float const spin_duration)
{
    m_spin_rate_begin = starting_spin_rate;
    m_spin_rate_end = ending_spin_rate;
    m_spin_time_total = spin_duration;
    m_spin_time_left = spin_duration;
}

void WorldView::ProcessSpin (Float const frame_dt)
{
    if (m_spin_time_left < 0.0f)
        m_spin_time_left = 0.0f;

    if (m_spin_time_left == 0.0f)
        m_spin_time_total = 1.0f;

    ASSERT1(m_spin_time_total > 0.0f);
    ASSERT1(m_spin_time_left <= m_spin_time_total);
    Float parameter = 1.0f - m_spin_time_left / m_spin_time_total;
    RotateView(frame_dt * (m_spin_rate_begin * (1.0f - parameter) + m_spin_rate_end * parameter));
    m_spin_time_left -= frame_dt;
}

void WorldView::InitiateFade (Float const starting_fade_coefficient, Float const ending_fade_coefficient, Float const fade_duration)
{
    m_fade_coefficient_begin = starting_fade_coefficient;
    m_fade_coefficient_end = ending_fade_coefficient;
    m_fade_time_total = fade_duration;
    m_fade_time_left = fade_duration;
}

void WorldView::ProcessFade (Float const frame_dt)
{
    if (m_fade_time_left < 0.0f)
        m_fade_time_left = 0.0f;

    if (m_fade_time_left == 0.0f)
        m_fade_time_total = 1.0f;

    ASSERT1(m_fade_time_total > 0.0f);
    ASSERT1(m_fade_time_left <= m_fade_time_total);
    Float parameter = 1.0f - m_fade_time_left / m_fade_time_total;
    Float fade_coefficient = m_fade_coefficient_begin * (1.0f - parameter) + m_fade_coefficient_end * parameter;
    ParentWorldViewWidget()->ColorBias() = Color(0.0f, 0.0f, 0.0f, 1.0f-fade_coefficient);
    m_fade_time_left -= frame_dt;
}

// ///////////////////////////////////////////////////////////////////////////
// begin state machine stuff
// ///////////////////////////////////////////////////////////////////////////

#define STATE_MACHINE_STATUS(state_name) \
    /* if (input == SM_ENTER) \
        fprintf(stderr, "WorldView: --> " state_name "\n"); \
    else if (input == SM_EXIT) \
        fprintf(stderr, "WorldView: <-- " state_name "\n"); \
    else if (input != IN_PROCESS_FRAME) \
        fprintf(stderr, "WorldView: input: %u\n", input); */

#define TRANSITION_TO(x) m_state_machine.SetNextState(&WorldView::x)

bool WorldView::StatePreIntro (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StatePreIntro")
    switch (input)
    {
        case IN_PROCESS_FRAME:
            // we're just waiting for the intro to begin.
            return true;

        case IN_BEGIN_INTRO:
            TRANSITION_TO(StateIntro);
            return true;
    }
    return false;
}

bool WorldView::StateIntro (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StateIntro")
    switch (input)
    {
        case SM_ENTER:
            InitiateZoom(ZoomFactor(), ms_zoom_factor_non_alert_wave, ms_intro_duration, false);
            InitiateSpin(360.0f, 0.0f, ms_intro_duration);
            InitiateFade(0.0f, 1.0f, ms_intro_duration);
            ScheduleStateMachineInput(IN_END_INTRO, ms_intro_duration);
            // hide the GameWidget's controls
            m_sender_hide_controls.Signal();
            return true;

        case IN_PROCESS_FRAME:
            return true;

        case IN_END_INTRO:
            m_sender_end_intro.Signal();
            TRANSITION_TO(StateNormalGameplay);
            return true;

        case IN_BEGIN_DEATH_RATTLE:
            TRANSITION_TO(StateDeathRattle);
            return true;

        case SM_EXIT:
            // show the GameWidget's controls
            m_sender_show_controls.Signal();
            // zero out the zoom and rotation accumulators
            m_zoom_accumulator = 0.0f;
            m_rotation_accumulator = 0.0f;
            return true;
    }
    return false;
}

bool WorldView::StateNormalGameplay (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StateNormalGameplay")
    switch (input)
    {
        case IN_PROCESS_FRAME:
            ProcessPlayerInput();
            return true;

        case IN_BEGIN_DEATH_RATTLE:
            TRANSITION_TO(StateDeathRattle);
            return true;

        case IN_BEGIN_OUTRO:
            // we can go straight to the outro if the player hits
            // the "end" button in the inventory panel
            TRANSITION_TO(StateOutro);
            return true;
    }
    return false;
}

bool WorldView::StateDeathRattle (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StateDeathRattle")
    switch (input)
    {
        case IN_PROCESS_FRAME:
            // we're just waiting for the game-over state to begin.
            return true;

        case IN_BEGIN_GAME_OVER:
            TRANSITION_TO(StateGameOver);
            return true;
    }
    return false;
}

bool WorldView::StateGameOver (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StateGameOver")
    switch (input)
    {
        case SM_ENTER:
            m_sender_show_game_over_label.Signal();
            return true;

        case IN_PROCESS_FRAME:
            // we're just waiting for the outro to begin.
            return true;

        case IN_BEGIN_OUTRO:
            TRANSITION_TO(StateOutro);
            return true;

        case SM_EXIT:
            m_sender_hide_game_over_label.Signal();
            return true;
    }
    return false;
}

bool WorldView::StateOutro (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StateOutro")
    switch (input)
    {
        case SM_ENTER:
            InitiateZoom(ZoomFactor(), ms_zoom_factor_outro_end, ms_outro_duration, false);
            InitiateSpin(0.0f, 360.0f, ms_outro_duration);
            InitiateFade(1.0f, 0.0f, ms_outro_duration);
            ScheduleStateMachineInput(IN_END_OUTRO, ms_outro_duration);
            // hide the GameWidget's controls
            m_sender_hide_controls.Signal();
            return true;

        case IN_PROCESS_FRAME:
            return true;

        case IN_END_OUTRO:
            m_sender_end_outro.Signal();
            TRANSITION_TO(StatePostOutro);
            return true;
    }
    return false;
}

bool WorldView::StatePostOutro (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StatePostOutro")
    switch (input)
    {
        case IN_PROCESS_FRAME:
            // we're in eternal limbo, so do nothing.
            return true;
    }
    return false;
}

void WorldView::ScheduleStateMachineInput (StateMachineInput const input, Float const time_delay)
{
    CancelScheduledStateMachineInput();
    EnqueueEvent(new EventStateMachineInput(input, MostRecentFrameTime() + time_delay));
}

void WorldView::CancelScheduledStateMachineInput ()
{
    OwnerEventQueue()->ScheduleMatchingEventsForDeletion(
        MatchEventType,
        Event::STATE_MACHINE_INPUT);
}

// ///////////////////////////////////////////////////////////////////////////
// end state machine stuff
// ///////////////////////////////////////////////////////////////////////////

} // end of namespace Dis

