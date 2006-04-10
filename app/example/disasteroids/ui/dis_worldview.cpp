// ///////////////////////////////////////////////////////////////////////////
// dis_worldview.cpp by Victor Dods, created 2005/11/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_worldview.h"

#include "dis_entity.h"
#include "dis_events.h"
#include "dis_playership.h"
#include "xrb_engine2_entity.h"
#include "xrb_engine2_objectlayer.h"
#include "xrb_engine2_worldviewwidget.h"
#include "xrb_input_events.h"
#include "xrb_keybinds.h"

using namespace Xrb;

namespace Dis
{

WorldView::WorldView (Engine2::WorldViewWidget *const parent_world_view_widget)
    :
    Engine2::WorldView(parent_world_view_widget),
    EventHandler(parent_world_view_widget->GetOwnerEventQueue()),
    SignalHandler(),
    m_state_machine(this),
    m_sender_player_ship_changed(this),
    m_sender_is_debug_info_enabled_changed(this),
    m_sender_show_controls(this),
    m_sender_hide_controls(this),
    m_sender_activate_inventory_panel(this),
    m_sender_deactivate_inventory_panel(this),
    m_sender_show_game_over_label(this),
    m_sender_hide_game_over_label(this),
    m_sender_end_game(this),
    m_sender_end_intro(this),
    m_sender_end_outro(this),
    m_receiver_end_game(&WorldView::EndGame, this),
    m_receiver_begin_intro(&WorldView::BeginIntro, this),
    m_receiver_begin_game_over(&WorldView::BeginGameOver, this),
    m_receiver_begin_outro(&WorldView::BeginOutro, this)
{
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

    m_use_dvorak = false;
    m_is_debug_info_enabled = false;
    SetDrawBorderGridLines(m_is_debug_info_enabled);

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

void WorldView::SetIsDebugInfoEnabled (bool is_debug_info_enabled)
{
    if (m_is_debug_info_enabled != is_debug_info_enabled)
    {
        m_is_debug_info_enabled = is_debug_info_enabled;
        SetDrawBorderGridLines(m_is_debug_info_enabled);
        m_sender_is_debug_info_enabled_changed.Signal(m_is_debug_info_enabled);
    }
}

bool WorldView::ProcessKeyEvent (EventKey const *const e)
{
    if (e->GetIsKeyDownEvent())
    {
        switch (e->GetKeyCode())
        {
            case Key::ESCAPE:
                // not allowed to use the inventory panel if we're in the intro or outro
                if (m_state_machine.GetCurrentState() != &WorldView::StatePreIntro &&
                    m_state_machine.GetCurrentState() != &WorldView::StateIntro &&
                    m_state_machine.GetCurrentState() != &WorldView::StateOutro &&
                    m_state_machine.GetCurrentState() != &WorldView::StatePostOutro)
                {
                    m_sender_activate_inventory_panel.Signal();
                }
                break;
        
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

            case Key::ONE:
            case Key::TWO:
            case Key::THREE:
            case Key::FOUR:
            case Key::FIVE:
            case Key::SIX:
            case Key::SEVEN:
            case Key::EIGHT:
            case Key::NINE:
                // this looks wrong, but the keys start at 1 and end at 0,
                // and the weapon indices start at 0 and end at 9.
                if (m_player_ship != NULL)
                    m_player_ship->SetMainWeaponNumber(e->GetKeyCode() - Key::ONE);
                break;

            case Key::ZERO:
                // this looks wrong, but the keys start at 1 and end at 0,
                // and the weapon indices start at 0 and end at 9.
                if (m_player_ship != NULL)
                    m_player_ship->SetMainWeaponNumber(9);
                break;

            case Key::F1:
                SetIsDebugInfoEnabled(!GetIsDebugInfoEnabled());
                break;
                
            case Key::F2:
                m_player_ship->GiveLotsOfMinerals();
                break;

            case Key::F3:
                m_use_dvorak = !m_use_dvorak;
                break;
                
            default:
                break;
        }
    }

    return true;
}

bool WorldView::ProcessMouseButtonEvent (EventMouseButton const *const e)
{
    return true;
}

bool WorldView::ProcessMouseWheelEvent (EventMouseWheel const *const e)
{
    // don't allow mouse wheel input while the widget is not focused
    if (!GetParentWorldViewWidget()->GetIsFocused())
        return false;

    if (e->GetIsEitherAltKeyPressed())
    {
        // when the alt key is held down, change the view's rotation

        if (e->GetButtonCode() == Key::MOUSEWHEELUP)
        {
            if (m_rotation_accumulator > 0.0f)
                m_rotation_accumulator -=
                    m_rotation_increment *
                    static_cast<Sint32>(m_rotation_accumulator / m_rotation_increment);
            m_rotation_accumulator -= m_rotation_increment;
        }
        else if (e->GetButtonCode() == Key::MOUSEWHEELDOWN)
        {
            if (m_rotation_accumulator < 0.0f)
                m_rotation_accumulator -=
                    m_rotation_increment *
                    static_cast<Sint32>(m_rotation_accumulator / m_rotation_increment);
            m_rotation_accumulator += m_rotation_increment;
        }
        else
            ASSERT0(false && "Invalid mouse wheel event (button code not recognized)")
    }
    else
    {
        // otherwise, change the view's zoom

        if (e->GetButtonCode() == Key::MOUSEWHEELUP)
        {
            if (m_zoom_accumulator < 0.0f)
                m_zoom_accumulator -= static_cast<Sint32>(m_zoom_accumulator);
            m_zoom_accumulator += 1.0f;
        }
        else if (e->GetButtonCode() == Key::MOUSEWHEELDOWN)
        {
            if (m_zoom_accumulator > 0.0f)
                m_zoom_accumulator -= static_cast<Sint32>(m_zoom_accumulator);
            m_zoom_accumulator -= 1.0f;
        }
        else
            ASSERT0(false && "Invalid mouse wheel event (button code not recognized)")
    }

    return true;
}

bool WorldView::ProcessMouseMotionEvent (EventMouseMotion const *const e)
{
    return true;
}

bool WorldView::ProcessEventOverride (Event const *const e)
{
    ASSERT1(e != NULL)

    if (e->GetType() != Event::CUSTOM)
        return false;
        
    EventBase const *dis_event = DStaticCast<EventBase const *>(e);
    switch (dis_event->GetCustomType())
    {
        case EventBase::STATE_MACHINE_INPUT:
            m_state_machine.RunCurrentState(DStaticCast<EventStateMachineInput const *>(dis_event)->GetInput());
            break;

        default:
            ASSERT1(false && "Unhandled custom event")
            break;
    }

    return true;
}

void WorldView::ProcessFrameOverride ()
{
    m_state_machine.RunCurrentState(IN_PROCESS_FRAME);

    // don't do anything if this view is hidden
    if (GetParentWorldViewWidget()->GetIsHidden())
        return;

    // update the view position
    if (m_player_ship != NULL)
    {
        /*
        { // simple dead-on follow
            SetCenter(m_player_ship->GetUnwrappedTranslation());
            m_view_velocity = m_player_ship->GetVelocity();
        }
        */
        // lookahead
        if (GetFrameDT() > 0.0f)
        { 
            Float minor_axis_radius = GetMinorAxisRadius();
            FloatVector2 ship_velocity_direction;
            if (m_player_ship->GetVelocity().GetIsZero())
                ship_velocity_direction = FloatVector2::ms_zero;
            else
                ship_velocity_direction = m_player_ship->GetVelocity().GetNormalization();
            Float view_distance = 0.8f * minor_axis_radius * Math::Atan(0.003f * m_player_ship->GetSpeed()) / 90.0f;
            FloatVector2 traveling_at(m_player_ship->GetUnwrappedTranslation() + view_distance * ship_velocity_direction);
    
            FloatVector2 view_center_delta(traveling_at - (GetCenter() + m_view_velocity * GetFrameDT()));
            bool is_view_recovering_this_frame;
            static Float const s_max_view_center_delta = 500.0f;
            static Float const s_time_to_recover = 0.5f;
            if (view_center_delta.GetLength() > s_max_view_center_delta * GetFrameDT())
            {
                m_view_velocity += view_center_delta.GetNormalization() * s_max_view_center_delta * GetFrameDT();
                is_view_recovering_this_frame = true;
            }
            else
            {
                m_view_velocity = m_player_ship->GetVelocity();
                is_view_recovering_this_frame = false;
            }
    
            if (!m_is_view_recovering && is_view_recovering_this_frame)
            {
                m_is_view_recovering = true;
                m_calculated_view_center = GetCenter() + m_view_velocity * GetFrameDT();
                m_recover_parameter = 0.0f;
            }
            else if (m_is_view_recovering && !is_view_recovering_this_frame)
                m_calculated_view_center = m_calculated_view_center + m_view_velocity * GetFrameDT();
            
            if (m_is_view_recovering && m_recover_parameter < 1.0f)
            {
                ASSERT1(m_recover_parameter >= 0.0f)
                SetCenter(
                    m_calculated_view_center * (1.0f - m_recover_parameter) +
                    traveling_at * m_recover_parameter);
                m_recover_parameter += GetFrameDT() / s_time_to_recover;
            }
            else
            {
                SetCenter(traveling_at);
                m_is_view_recovering = false;
            }
        }
        /*
        { // view dragging
            Float minor_axis_radius = GetMinorAxisRadius();
            FloatVector2 view_to_ship = GetCenter() - m_player_ship->GetUnwrappedTranslation();
            Float view_to_ship_ratio = view_to_ship.GetLength() / (0.5f * minor_axis_radius);
            Float dragging_factor;
            if (view_to_ship_ratio >= 1.0f)
                dragging_factor = m_dragging_factor * view_to_ship_ratio;
            else
                dragging_factor = m_dragging_factor;
            dragging_factor *= GetFrameDT();
        
            SetCenter(
                GetCenter() * (1.0f - dragging_factor)
                +
                m_player_ship->GetUnwrappedTranslation() * dragging_factor);
            m_view_velocity = m_player_ship->GetVelocity();
        }
        */
    }
    else
    {
        SetCenter(GetCenter() + m_view_velocity * GetFrameDT());
    }
}

void WorldView::ProcessPlayerInput ()
{
    // handle view zooming (from accumulated mouse wheel events)
    {
        Float zoom_by_power;

        if (m_zoom_accumulator > 0)
            zoom_by_power = Min(m_zoom_accumulator, m_zoom_speed*GetFrameDT());
        else if (m_zoom_accumulator < 0)
            zoom_by_power = Max(m_zoom_accumulator, -m_zoom_speed*GetFrameDT());
        else
            zoom_by_power = 0.0f;

        ZoomView(pow(m_zoom_increment, zoom_by_power));
        m_zoom_accumulator -= zoom_by_power;
    }

    // handle view rotation (from acculumated mouse motion events)
    {
        Float rotate_by_angle;

        if (m_rotation_accumulator > 0)
            rotate_by_angle = Min(m_rotation_accumulator, m_rotation_speed*GetFrameDT());
        else if (m_rotation_accumulator < 0)
            rotate_by_angle = Max(m_rotation_accumulator, -m_rotation_speed*GetFrameDT());
        else
            rotate_by_angle = 0.0f;

        RotateView(rotate_by_angle);
        m_rotation_accumulator -= rotate_by_angle;
    }

    // handle player ship input (movement, shooting and reticle coordinates)
    if (GetParentWorldViewWidget()->GetIsFocused())
    {
        Sint8 engine_right_left_input;
        Sint8 engine_up_down_input;
        Uint8 engine_auxiliary_input;
        if (m_use_dvorak)
        {
            engine_right_left_input =
                (Singletons::KeyBinds()->GetIsKeyPressed(Key::I) ?  127 : 0) +
                (Singletons::KeyBinds()->GetIsKeyPressed(Key::E) ? -127 : 0);
            engine_up_down_input =
                (Singletons::KeyBinds()->GetIsKeyPressed(Key::P) ?  127 : 0) +
                (Singletons::KeyBinds()->GetIsKeyPressed(Key::U) ? -127 : 0);
            engine_auxiliary_input =
                (Singletons::KeyBinds()->GetIsKeyPressed(Key::X) ? UINT8_UPPER_BOUND : 0);
        }
        else
        {
            engine_right_left_input =
                (Singletons::KeyBinds()->GetIsKeyPressed(Key::D) ?  127 : 0) +
                (Singletons::KeyBinds()->GetIsKeyPressed(Key::A) ? -127 : 0);
            engine_up_down_input =
                (Singletons::KeyBinds()->GetIsKeyPressed(Key::W) ?  127 : 0) +
                (Singletons::KeyBinds()->GetIsKeyPressed(Key::S) ? -127 : 0);
            engine_auxiliary_input =
                (Singletons::KeyBinds()->GetIsKeyPressed(Key::C) ? UINT8_UPPER_BOUND : 0);
        }

        Uint8 weapon_primary_input =
            Singletons::KeyBinds()->GetIsKeyPressed(Key::LMOUSE) ? UINT8_UPPER_BOUND : 0;
        Uint8 weapon_secondary_input =
            Singletons::KeyBinds()->GetIsKeyPressed(Key::RMOUSE) ? UINT8_UPPER_BOUND : 0;

        bool is_using_auxiliary_weapon =
            Singletons::KeyBinds()->GetIsKeyPressed(Key::SPACE);

        if (m_player_ship != NULL)
        {
            m_player_ship->SetReticleCoordinates(
                GetParallaxedScreenToWorld() *
                GetParentWorldViewWidget()->GetLastMousePosition().StaticCast<Float>()
                -
                m_player_ship->GetWrappedOffset());
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

void WorldView::BeginIntro ()
{
    ScheduleStateMachineInput(IN_BEGIN_INTRO, 0.0f);
}

void WorldView::BeginGameOver ()
{
    ScheduleStateMachineInput(IN_BEGIN_GAME_OVER, 0.0f);
}

void WorldView::BeginOutro ()
{
    ScheduleStateMachineInput(IN_BEGIN_OUTRO, 0.0f);
}

void WorldView::SetIntroTimeLeft (Float intro_time_left)
{
    ASSERT1(m_intro_outro_time_total > 0.0f)
    ASSERT1(m_zoom_factor_begin > 0.0f)
    ASSERT1(m_zoom_factor_end > 0.0f)

    if (intro_time_left < 0.0f)
        intro_time_left = 0.0f;

    m_intro_outro_time_left = intro_time_left;
    ASSERT1(m_intro_outro_time_left <= m_intro_outro_time_total)
    Float parameter = 1.0f - m_intro_outro_time_left / m_intro_outro_time_total;
    GetParentWorldViewWidget()->SetColorMask(Color(parameter, parameter, parameter, 1.0f));
    SetZoomFactor(m_zoom_factor_begin * (1.0 - parameter) + m_zoom_factor_end * parameter);
}

void WorldView::SetOutroTimeLeft (Float outro_time_left)
{
    ASSERT1(m_intro_outro_time_total > 0.0f)
    ASSERT1(m_zoom_factor_begin > 0.0f)
    ASSERT1(m_zoom_factor_end > 0.0f)

    if (outro_time_left < 0.0f)
        outro_time_left = 0.0f;

    m_intro_outro_time_left = outro_time_left;
    ASSERT1(m_intro_outro_time_left <= m_intro_outro_time_total)
    Float parameter = m_intro_outro_time_left / m_intro_outro_time_total;
    GetParentWorldViewWidget()->SetColorMask(Color(parameter, parameter, parameter, 1.0f));
    SetZoomFactor(m_zoom_factor_end * (1.0 - parameter) + m_zoom_factor_begin * parameter);
}

// ///////////////////////////////////////////////////////////////////////////
// begin state machine stuff
// ///////////////////////////////////////////////////////////////////////////

#define STATE_MACHINE_STATUS(state_name) \
    if (input == SM_ENTER) \
        fprintf(stderr, "WorldView: --> " state_name "\n"); \
    else if (input == SM_EXIT) \
        fprintf(stderr, "WorldView: <-- " state_name "\n"); \
    else if (input != IN_PROCESS_FRAME) \
        fprintf(stderr, "WorldView: input: %u\n", input);

#define TRANSITION_TO(x) m_state_machine.SetNextState(&WorldView::x)

bool WorldView::StatePreIntro (StateMachineInput const input)
{
    STATE_MACHINE_STATUS("StatePreIntro")
    switch (input)
    {
        case SM_ENTER:
            // relax the min/max zoom factors
            SetMinZoomFactor(0.0f);
            SetMaxZoomFactor(10000.0f);
            // set the begin/end zoom factors
            m_zoom_factor_begin = 0.0005f;
            m_zoom_factor_end = 0.0039f;
            // initialize the intro time        
            m_intro_outro_time_total = 2.0f;
            SetIntroTimeLeft(m_intro_outro_time_total);
            return true;

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
            // hide the GameWidget's controls
            m_sender_hide_controls.Signal();
            return true;

        case IN_PROCESS_FRAME:
            SetIntroTimeLeft(m_intro_outro_time_left - GetFrameDT());
            if (m_intro_outro_time_left <= 0.0f)
            {
                m_sender_end_intro.Signal();
                TRANSITION_TO(StateNormalGameplay);
            }
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
        case SM_ENTER:
            // set the in-game zoom factor limits
            SetMinZoomFactor(0.0025f);
            SetMaxZoomFactor(0.0078f);
            break;
    
        case IN_PROCESS_FRAME:
            ProcessPlayerInput();        
            return true;

        case IN_BEGIN_GAME_OVER:
            TRANSITION_TO(StateGameOver);
            return true;

        case IN_BEGIN_OUTRO:
            // we can go straight to the outro if the player hits
            // the "end" button in the inventory panel
            TRANSITION_TO(StateOutro);
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
            // relax the min/max zoom factors
            SetMinZoomFactor(0.0f);
            SetMaxZoomFactor(10000.0f);
            // set the begin/end zoom factors
            m_zoom_factor_begin = GetZoomFactor();
            m_zoom_factor_end = 0.0005f;
            // initialize the outro time        
            m_intro_outro_time_total = 2.0f;
            SetOutroTimeLeft(m_intro_outro_time_total);
            // hide the GameWidget's controls
            m_sender_hide_controls.Signal();
            return true;

        case IN_PROCESS_FRAME:
            SetOutroTimeLeft(m_intro_outro_time_left - GetFrameDT());
            if (m_intro_outro_time_left <= 0.0f)
            {
                m_sender_end_outro.Signal();
                TRANSITION_TO(StatePostOutro);
            }
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
    EnqueueEvent(new EventStateMachineInput(input, GetMostRecentFrameTime() + time_delay));
}

void WorldView::CancelScheduledStateMachineInput ()
{
    GetOwnerEventQueue()->ScheduleMatchingEventsForDeletion(
        MatchCustomType,
        static_cast<EventCustom::CustomType>(EventBase::STATE_MACHINE_INPUT));
}

// ///////////////////////////////////////////////////////////////////////////
// end state machine stuff
// ///////////////////////////////////////////////////////////////////////////

} // end of namespace Dis

