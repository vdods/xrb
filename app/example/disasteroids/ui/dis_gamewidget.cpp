// ///////////////////////////////////////////////////////////////////////////
// dis_gamewidget.cpp by Victor Dods, created 2005/12/17
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_gamewidget.hpp"

#include "dis_inventorypanel.hpp"
#include "dis_powergenerator.hpp"
#include "dis_playership.hpp"
#include "dis_shield.hpp"
#include "dis_util.hpp"
#include "dis_weapon.hpp"
#include "dis_world.hpp"
#include "dis_worldview.hpp"
#include "xrb_engine2_world.hpp"
#include "xrb_engine2_worldviewwidget.hpp"
#include "xrb_label.hpp"
#include "xrb_layout.hpp"
#include "xrb_progressbar.hpp"
#include "xrb_screen.hpp"
#include "xrb_spacerwidget.hpp"
#include "xrb_transformation.hpp"
#include "xrb_valuelabel.hpp"
#include "xrb_widgetbackground.hpp"
#include "xrb_widgetcontext.hpp"

using namespace Xrb;

namespace Dis {

// for use in transforming the stoke signal from the player ship
// into the stoke-o-meter progress value
Float NormalizeStoke (Float stoke)
{
    ASSERT1(stoke >= 1.0f);
    ASSERT1(stoke <= PlayerShip::ms_max_stoke);
    return (stoke - 1.0f) / (PlayerShip::ms_max_stoke - 1.0f);
}

GameWidget::GameWidget (World *world, WidgetContext &context)
    :
    WidgetStack(context, "Disasteroids GameWidget"),
    m_receiver_set_player_ship(&GameWidget::SetPlayerShip, this),
    m_internal_receiver_set_wave_count(&GameWidget::SetWaveCount, this),
    m_internal_receiver_show_controls(&GameWidget::ShowControls, this),
    m_internal_receiver_hide_controls(&GameWidget::HideControls, this),
    m_internal_receiver_activate_inventory_panel(&GameWidget::ActivateInventoryPanel, this),
    m_internal_receiver_deactivate_inventory_panel(&GameWidget::DeactivateInventoryPanel, this),
    m_receiver_set_mineral_inventory(&GameWidget::SetMineralInventory, this)
{
    ASSERT1(world != NULL);

    // the world view goes under the HUD in the game widget stack
    m_world_view_widget = new Engine2::WorldViewWidget(context);
    this->AttachChild(m_world_view_widget);
    
    m_world_view = new WorldView(m_world_view_widget);
    world->AttachWorldView(m_world_view);

    // connect the controls [de]activate signal
    SignalHandler::Connect0(
        m_world_view->SenderShowControls(),
        &m_internal_receiver_show_controls);
    SignalHandler::Connect0(
        m_world_view->SenderHideControls(),
        &m_internal_receiver_hide_controls);
    // connect the inventory panel [de]activate signal
    SignalHandler::Connect0(
        m_world_view->SenderActivateInventoryPanel(),
        &m_internal_receiver_activate_inventory_panel);
    SignalHandler::Connect0(
        m_world_view->SenderDeactivateInventoryPanel(),
        &m_internal_receiver_deactivate_inventory_panel);

    m_saved_game_timescale = -1.0f;
    // create the inventory panel and hide it.
    m_inventory_panel = new InventoryPanel(context);
    Context().GetScreen().AttachAsModalChildWidget(*m_inventory_panel);
    m_inventory_panel->Hide();
    
    // connect the inventory panel deactivate signal
    SignalHandler::Connect0(
        m_inventory_panel->SenderDeactivate(),
        &m_internal_receiver_deactivate_inventory_panel);
    // connect the inventory panel's end game button to the world view
    SignalHandler::Connect0(
        m_inventory_panel->SenderEndGame(),
        m_world_view->ReceiverEndGame());

    // create the layout for the HUD
    Layout *main_layout = new Layout(VERTICAL, Context(), "main GameWidget layout");
    main_layout->SetIsUsingZeroedLayoutSpacingMargins(true);
    {
        // framerate (and other stuff later) layout
        {
            m_debug_info_layout = new Layout(HORIZONTAL, Context(), "debugging info layout");
            m_debug_info_layout->SetIsUsingZeroedFrameMargins(false);
            m_debug_info_layout->SetBackgroundStyle("black_half_opaque"); // see Dis::Master::Master()

            m_world_frame_dt_label =
                new ValueLabel<Time::Delta>(
                    "%0.3f s (world)",
                    Util::TextToFloat,
                    context,
                    "world frame time label");
            m_world_frame_dt_label->SetIsHeightFixedToTextHeight(true);
            m_world_frame_dt_label->SetAlignment(Dim::X, RIGHT);
            m_debug_info_layout->AttachChild(m_world_frame_dt_label);

            m_gui_frame_dt_label =
                new ValueLabel<Time::Delta>(
                    "%0.3f s (gui process)",
                    Util::TextToFloat,
                    context,
                    "gui frame time label");
            m_gui_frame_dt_label->SetIsHeightFixedToTextHeight(true);
            m_gui_frame_dt_label->SetAlignment(Dim::X, RIGHT);
            m_debug_info_layout->AttachChild(m_gui_frame_dt_label);

            m_render_frame_dt_label =
                new ValueLabel<Time::Delta>(
                    "%0.3f s (render)",
                    Util::TextToFloat,
                    context,
                    "render frame time label");
            m_render_frame_dt_label->SetIsHeightFixedToTextHeight(true);
            m_render_frame_dt_label->SetAlignment(Dim::X, RIGHT);
            m_debug_info_layout->AttachChild(m_render_frame_dt_label);

            m_entity_count_label =
                new ValueLabel<Uint32>(
                    "%u entities",
                    Util::TextToUint<Uint32>,
                    context,
                    "entity count label");
            m_entity_count_label->SetIsHeightFixedToTextHeight(true);
            m_entity_count_label->SetAlignment(Dim::X, RIGHT);
            m_debug_info_layout->AttachChild(m_entity_count_label);

            m_bind_texture_call_count_label =
                new ValueLabel<Uint32>(
                    "%u bind texture calls",
                    Util::TextToUint<Uint32>,
                    context,
                    "bind texture call count label");
            m_bind_texture_call_count_label->SetIsHeightFixedToTextHeight(true);
            m_bind_texture_call_count_label->SetAlignment(Dim::X, RIGHT);
            m_debug_info_layout->AttachChild(m_bind_texture_call_count_label);
            m_bind_texture_call_count_label->Hide(); // for now

            m_bind_texture_call_hit_percent_label =
                new ValueLabel<Uint32>(
                    "%u%% bind texture hit",
                    Util::TextToUint<Uint32>,
                    context,
                    "bind texture call hit percent label");
            m_bind_texture_call_hit_percent_label->SetIsHeightFixedToTextHeight(true);
            m_bind_texture_call_hit_percent_label->SetAlignment(Dim::X, RIGHT);
            m_debug_info_layout->AttachChild(m_bind_texture_call_hit_percent_label);
    //         m_bind_texture_call_hit_percent_label->Hide(); // for now

            m_framerate_label =
                new ValueLabel<Float>(
                    "%.1f fps",
                    Util::TextToFloat,
                    context,
                    "framerate label");
            m_framerate_label->SetIsHeightFixedToTextHeight(true);
            m_framerate_label->SetAlignment(Dim::X, RIGHT);
            m_debug_info_layout->AttachChild(m_framerate_label);

            // start hidden
            main_layout->AttachChild(m_debug_info_layout);
            m_debug_info_layout->Hide();
            
            // connect the debug info enabled signal
            SignalHandler::Connect1(
                m_world_view->SenderIsGameLoopInfoEnabledChanged(),
                &Transformation::BooleanNegation,
                m_debug_info_layout->ReceiverSetIsHidden());
        }

        // time, mineral inventory, score layout and stoke-o-meter
        m_stats_and_inventory_layout = new Layout(HORIZONTAL, Context(), "time-alive and score layout");
        {
            m_stats_and_inventory_layout->SetIsUsingZeroedFrameMargins(false);
            m_stats_and_inventory_layout->SetBackgroundStyle("black_half_opaque"); // see Dis::Master::Master()
            // NOTE: here is where m_stats_and_inventory_layout->Hide() was called before

            m_wave_count_label =
                new ValueLabel<Uint32>(
                    "WAVE %u",
                    Util::TextToUint<Uint32>,
                    context,
                    "wave count label");
            m_wave_count_label->SetIsHeightFixedToTextHeight(true);
            m_wave_count_label->SetAlignment(Dim::X, LEFT);
            m_wave_count_label->SetFontStyle("hud_element");
            m_stats_and_inventory_layout->AttachChild(m_wave_count_label);

            m_wave_count = 0;

            UpdateWaveCountLabel();

    /*
            // extra lives label and icon
            {
                m_lives_remaining_label =
                    new ValueLabel<Uint32>(
                        "%u",
                        Util::TextToUint<Uint32>,
                        context,
                        m_stats_and_inventory_layout);
                m_lives_remaining_label->SetIsHeightFixedToTextHeight(true);
                m_lives_remaining_label->SetAlignment(Dim::X, RIGHT);

                Label *lives_remaining_icon_label =
                    new Label(
                        GlTexture::Load("fs://solitary_small.png"),
                        m_stats_and_inventory_layout,
                        context);
                lives_remaining_icon_label->FixWidth(m_lives_remaining_label->Height());
                lives_remaining_icon_label->FixHeight(m_lives_remaining_label->Height());
            }
    */

            for (Uint8 mineral_index = 0; mineral_index < MINERAL_COUNT; ++mineral_index)
            {
                m_mineral_inventory_label[mineral_index] = new ValueLabel<Uint32>("%u", Util::TextToUint<Uint32>, Context());
                m_mineral_inventory_label[mineral_index]->SetIsHeightFixedToTextHeight(true);
                m_mineral_inventory_label[mineral_index]->SetAlignment(Dim::X, RIGHT);
                m_stats_and_inventory_layout->AttachChild(m_mineral_inventory_label[mineral_index]);

                Label *mineral_icon_label = new Label(GlTexture::Load(Item::MineralSpritePath(mineral_index)), Context());
                mineral_icon_label->FixWidth(m_mineral_inventory_label[mineral_index]->Height());
                mineral_icon_label->FixHeight(m_mineral_inventory_label[mineral_index]->Height());
                m_stats_and_inventory_layout->AttachChild(mineral_icon_label);
            }

            // a spacer to make things look even
            m_stats_and_inventory_layout->AttachChild(new SpacerWidget(context));

            {
                m_option_inventory_label = new ValueLabel<Uint32>("%u", Util::TextToUint<Uint32>, Context());
                m_option_inventory_label->SetIsHeightFixedToTextHeight(true);
                m_option_inventory_label->SetAlignment(Dim::X, RIGHT);
                m_stats_and_inventory_layout->AttachChild(m_option_inventory_label);

                Label *option_icon_label = new Label(GlTexture::Load("fs://radiobutton_blue.png"), Context());
                option_icon_label->FixWidth(m_option_inventory_label->Height() * 7 / 5);
                option_icon_label->FixHeight(m_option_inventory_label->Height() * 7 / 5);
                m_stats_and_inventory_layout->AttachChild(option_icon_label);
            }

            // a spacer to make things look even
            m_stats_and_inventory_layout->AttachChild(new SpacerWidget(context));

            // a widget stack for the score label and stoke-o-meter
            WidgetStack *widget_stack = new WidgetStack(context, "score label and stoke-o-meter widget stack");
            {
                m_stoke_o_meter = new ProgressBar(ProgressBar::GO_LEFT, Context(), "stoke-o-meter");
                m_stoke_o_meter->SetColor(Color(1.0f, 1.0f, 1.0f, 0.3f));
                widget_stack->AttachChild(m_stoke_o_meter);

                m_score_label = new ValueLabel<Uint32>("%u", Util::TextToUint<Uint32>, Context(), "score label");
                m_score_label->SetIsHeightFixedToTextHeight(true);
                m_score_label->SetAlignment(Dim::X, RIGHT);
                m_score_label->SetFontStyle("hud_element");
                widget_stack->AttachChild(m_score_label);
            }
            m_stats_and_inventory_layout->AttachChild(widget_stack);
        }
        main_layout->AttachChild(m_stats_and_inventory_layout);
        // start with this layout hidden, because the WorldView will show it later
        m_stats_and_inventory_layout->Hide();

        // spacer widget (for the vertical space in the HUD between
        // the upper and lower status bars)
        main_layout->AttachChild(new SpacerWidget(context));

        // armor/shield, power and weapon status layout
        m_ship_status_layout = new Layout(HORIZONTAL, Context(), "ship status layout");
        m_ship_status_layout->SetIsUsingZeroedFrameMargins(false);
        {
            // contains the armor and shield status bars, and keeps the
            // shield status bar on top of the armor status bar
            WidgetStack *armor_and_shield_stack = new WidgetStack(context);
            {
                m_armor_status = new ProgressBar(ProgressBar::GO_RIGHT, Context(), "armor status");
                m_armor_status->FixHeight(m_score_label->Height()/2);
                m_armor_status->SetColor(Color(0.5f, 1.0f, 0.5f, 0.5f));
                armor_and_shield_stack->AttachChild(m_armor_status);

                m_shield_status = new ProgressBar(ProgressBar::GO_RIGHT, Context(), "shield status");
                m_shield_status->FixHeight(m_score_label->Height()/2);
                m_shield_status->SetColor(Color(0.5f, 0.5f, 1.0f, 0.5f));
                armor_and_shield_stack->AttachChild(m_shield_status);
            }
            m_ship_status_layout->AttachChild(armor_and_shield_stack);

            m_power_status = new ProgressBar(ProgressBar::GO_RIGHT, Context(), "power status");
            m_power_status->FixHeight(m_score_label->Height()/2);
            m_power_status->SetColor(Color(1.0f, 1.0f, 0.5f, 0.5f));
            m_ship_status_layout->AttachChild(m_power_status);

            m_weapon_status = new ProgressBar(ProgressBar::GO_RIGHT, Context(), "weapon status");
            m_weapon_status->FixHeight(m_score_label->Height()/2);
            m_weapon_status->SetColor(Color(1.0f, 0.5f, 0.5f, 0.5f));
            m_ship_status_layout->AttachChild(m_weapon_status);
        }
        main_layout->AttachChild(m_ship_status_layout);
        // start with this layout hidden, because the WorldView will show it later
        m_ship_status_layout->Hide();
    }
    this->AttachChild(main_layout);

    // create the game over label and hide it
    {
        m_game_over_label = new Label("GAME OVER", Context(), "game over label");
        m_game_over_label->SetFontStyle("giant");
        m_game_over_label->CenterOnWidget(*this);
        this->AttachChild(m_game_over_label);
        m_game_over_label->Hide();
    }

    // connect the show/hide game over label signals
    SignalHandler::Connect0(
        m_world_view->SenderShowGameOverLabel(),
        m_game_over_label->ReceiverShow());
    SignalHandler::Connect0(
        m_world_view->SenderHideGameOverLabel(),
        m_game_over_label->ReceiverHide());

    // initialize the player ship pointer, and connect up the
    // corresponding signal/receiver from the world view
    SetPlayerShip(m_world_view->GetPlayerShip());
    SignalHandler::Connect1(
        m_world_view->SenderPlayerShipChanged(),
        &m_receiver_set_player_ship);

    // the world view widget will be focused upon activation (see HandleActivate)
}

SignalSender0 const *GameWidget::SenderQuitGame ()
{
    ASSERT1(m_inventory_panel != NULL);
    return m_inventory_panel->SenderQuitGame();
}

void GameWidget::SetPlayerShip (PlayerShip *player_ship)
{
    // disconnect old player ship connections
//     m_lives_remaining_label->DetachAll();
    for (Uint32 i = 0; i < MINERAL_COUNT; ++i)
        m_mineral_inventory_label[i]->DetachAll();
    m_option_inventory_label->DetachAll();
    m_score_label->DetachAll();
    m_internal_receiver_set_wave_count.DetachAll();
    m_armor_status->DetachAll();
    m_shield_status->DetachAll();
    m_power_status->DetachAll();
    m_weapon_status->DetachAll();
    m_receiver_set_mineral_inventory.DetachAll();

    // if the player ship exists, connect the signals and initialize the UI elements
    if (player_ship != NULL)
    {
        // connect up the PlayerShip signals
        SignalHandler::Connect1(
            player_ship->SenderStokeChanged(),
            NormalizeStoke,
            m_stoke_o_meter->ReceiverSetProgress());
        SignalHandler::Connect1(
            player_ship->SenderWaveCountChanged(),
            &m_internal_receiver_set_wave_count);
//         SignalHandler::Connect1(
//             player_ship->SenderLivesRemainingChanged(),
//             m_lives_remaining_label->ReceiverSetValue());
        SignalHandler::Connect1(
            player_ship->SenderScoreChanged(),
            m_score_label->ReceiverSetValue());
        SignalHandler::Connect1(
            player_ship->SenderArmorStatusChanged(),
            m_armor_status->ReceiverSetProgress());
        SignalHandler::Connect1(
            player_ship->SenderShieldStatusChanged(),
            m_shield_status->ReceiverSetProgress());
        SignalHandler::Connect1(
            player_ship->SenderPowerStatusChanged(),
            m_power_status->ReceiverSetProgress());
        SignalHandler::Connect1(
            player_ship->SenderWeaponStatusChanged(),
            m_weapon_status->ReceiverSetProgress());
        SignalHandler::Connect2(
            player_ship->SenderMineralInventoryChanged(),
            &m_receiver_set_mineral_inventory);
        SignalHandler::Connect1(
            player_ship->SenderOptionInventoryChanged(),
            m_option_inventory_label->ReceiverSetValue());

        // make sure the UI elements are enabled
        m_wave_count_label->Enable();
//         m_lives_remaining_label->Enable();
        for (Uint32 i = 0; i < MINERAL_COUNT; ++i)
            m_mineral_inventory_label[i]->Enable();
        m_option_inventory_label->Enable();
        m_score_label->Enable();
        m_armor_status->Enable();
        m_shield_status->Enable();
        m_power_status->Enable();
        m_weapon_status->Enable();

        // initialize the UI elements
        m_wave_count = player_ship->WaveCount();
        UpdateWaveCountLabel();
//         m_lives_remaining_label->SetValue(player_ship->LivesRemaining());
        m_stoke_o_meter->SetProgress(NormalizeStoke(player_ship->Stoke()));
        m_score_label->SetValue(player_ship->GetScore());
        m_armor_status->SetProgress(player_ship->ArmorStatus());
        m_shield_status->SetProgress(player_ship->ShieldStatus());
        m_power_status->SetProgress(player_ship->PowerStatus());
        m_weapon_status->SetProgress(player_ship->WeaponStatus());
        for (Uint32 i = 0; i < MINERAL_COUNT; ++i)
            m_mineral_inventory_label[i]->SetValue(static_cast<Uint32>(player_ship->MineralInventory(i)));
        m_option_inventory_label->SetValue(player_ship->OptionInventory());

        m_inventory_panel->SetInventoryOwnerShip(player_ship);
    }
    // otherwise set the UI elements to zero, and disable them
    else
    {
        m_wave_count = 0;
        UpdateWaveCountLabel();
        m_wave_count_label->Disable();
//         m_lives_remaining_label->Disable();
        for (Uint32 i = 0; i < MINERAL_COUNT; ++i)
            m_mineral_inventory_label[i]->Disable();
        m_option_inventory_label->Disable();
        m_score_label->Disable();
        m_armor_status->Disable();
        m_shield_status->Disable();
        m_power_status->Disable();
        m_weapon_status->Disable();
    }
}

void GameWidget::SetWorldFrameDT (Time::Delta world_frame_dt)
{
    ASSERT1(m_world_frame_dt_label != NULL);
    m_world_frame_dt_label->SetValue(world_frame_dt);
}

void GameWidget::SetGUIFrameDT (Time::Delta gui_frame_dt)
{
    ASSERT1(m_gui_frame_dt_label != NULL);
    m_gui_frame_dt_label->SetValue(gui_frame_dt);
}

void GameWidget::SetRenderFrameDT (Time::Delta render_frame_dt)
{
    ASSERT1(m_render_frame_dt_label != NULL);
    m_render_frame_dt_label->SetValue(render_frame_dt);
}

void GameWidget::SetEntityCount (Uint32 entity_count)
{
    ASSERT1(m_entity_count_label != NULL);
    m_entity_count_label->SetValue(entity_count);
}

void GameWidget::SetBindTextureCallCount (Uint32 bind_texture_call_count)
{
    ASSERT1(m_bind_texture_call_count_label != NULL);
    m_bind_texture_call_count_label->SetValue(bind_texture_call_count);
}

void GameWidget::SetBindTextureCallHitPercent (Uint32 bind_texture_call_hit_percent)
{
    ASSERT1(m_bind_texture_call_hit_percent_label != NULL);
    m_bind_texture_call_hit_percent_label->SetValue(bind_texture_call_hit_percent);
}

void GameWidget::SetFramerate (Float const framerate)
{
    ASSERT1(m_framerate_label != NULL);
    m_framerate_label->SetValue(framerate);
}

void GameWidget::SetMineralInventory (Uint8 mineral_index, Float mineral_inventory)
{
    ASSERT1(mineral_index < MINERAL_COUNT);
    ASSERT1(mineral_inventory >= 0.0f);
    m_mineral_inventory_label[mineral_index]->SetValue(static_cast<Uint32>(mineral_inventory));
}

void GameWidget::ShowControls ()
{
    m_stats_and_inventory_layout->Show();
    m_ship_status_layout->Show();
}

void GameWidget::HideControls ()
{
    DeactivateInventoryPanel();
    m_stats_and_inventory_layout->Hide();
    m_ship_status_layout->Hide();
}

void GameWidget::ActivateInventoryPanel ()
{
    ASSERT1(m_inventory_panel->IsModal());
    ASSERT1(m_inventory_panel->IsHidden());

    // pause the game
    ASSERT1(m_saved_game_timescale == -1.0f);
    m_saved_game_timescale = m_world_view->GetWorld()->Timescale();
    m_world_view->GetWorld()->SetTimescale(0.0f);

    // update the owned inventory items
    m_inventory_panel->UpdatePanelState();

    // show the inventory panel and resize appropriately
    m_inventory_panel->Show();
    m_inventory_panel->ResizeByRatios(FloatVector2(0.8f, 0.8f));
    m_inventory_panel->CenterOnWidget(Context().GetScreen());
    m_inventory_panel->Focus();
}

void GameWidget::DeactivateInventoryPanel ()
{
    ASSERT1(m_inventory_panel->IsModal());
    if (!m_inventory_panel->IsHidden())
    {

        // hide the inventory panel
        m_inventory_panel->Hide();
        // move the focus back to the world view widget
        m_world_view_widget->Focus();

        // unpause the game
        ASSERT1(m_saved_game_timescale >= 0.0f);
        m_world_view->GetWorld()->SetTimescale(m_saved_game_timescale);
        m_saved_game_timescale = -1.0f;
    }
}

void GameWidget::HandleActivate ()
{
    WidgetStack::HandleActivate();
    
    ASSERT1(m_world_view_widget != NULL);
    m_world_view_widget->Focus();
}

void GameWidget::SetWaveCount (Uint32 const wave_count)
{
    m_wave_count = wave_count;
    UpdateWaveCountLabel();
}

void GameWidget::UpdateWaveCountLabel ()
{
    m_wave_count_label->SetValue(m_wave_count);
}

} // end of namespace Dis

