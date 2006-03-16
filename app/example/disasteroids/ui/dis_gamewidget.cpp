// ///////////////////////////////////////////////////////////////////////////
// dis_gamewidget.cpp by Victor Dods, created 2005/12/17
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_gamewidget.h"

#include "dis_inventorypanel.h"
#include "dis_powergenerator.h"
#include "dis_playership.h"
#include "dis_progressbar.h"
#include "dis_shield.h"
#include "dis_weapon.h"
#include "dis_worldview.h"
#include "xrb_engine2_world.h"
#include "xrb_engine2_worldviewwidget.h"
#include "xrb_label.h"
#include "xrb_layout.h"
#include "xrb_screen.h"
#include "xrb_spacerwidget.h"
#include "xrb_transformation.h"
#include "xrb_valuelabel.h"
#include "xrb_widgetbackground.h"

using namespace Xrb;

namespace Dis
{

// for use in transforming the stoke signal from the player ship
// into the stoke-o-meter progress value
Float NormalizeStoke (Float const stoke)
{
    ASSERT1(stoke >= 1.0f)
    return Math::Atan(stoke - 1.0f) / 90.0f;
}

GameWidget::GameWidget (
    Engine2::World *const world,
    Widget *const parent)
    :
    WidgetStack(parent, "disasteroids game widget"),
    m_receiver_set_player_ship(&GameWidget::SetPlayerShip, this),
    m_receiver_activate_inventory_panel(&GameWidget::ActivateInventoryPanel, this),
    m_receiver_deactivate_inventory_panel(&GameWidget::DeactivateInventoryPanel, this),
    m_receiver_set_mineral_inventory(&GameWidget::SetMineralInventory, this)
{
    ASSERT1(world != NULL)

    // the world view goes under the HUD in the game widget stack
    m_world_view_widget =
        new Engine2::WorldViewWidget(NULL, this);
    m_world_view =
        new WorldView(m_world_view_widget);
    world->AttachWorldView(m_world_view);
    m_world_view->SetViewZoomFactor(1.0f/256.0f);
    m_world_view_widget->SetWorldView(m_world_view);

    // connect the inventory panel activate signal
    SignalHandler::Connect0(
        m_world_view->SenderActivateInventoryPanel(),
        &m_receiver_activate_inventory_panel);

    m_saved_game_timescale = -1.0f;
    // create the inventory panel and hide it.
    m_inventory_panel = new InventoryPanel(this);
    m_inventory_panel->Hide();
    // connect the inventory panel deactivate signal
    SignalHandler::Connect0(
        m_inventory_panel->SenderDeactivate(),
        &m_receiver_deactivate_inventory_panel);
        
    // create the layout for the HUD
    Layout *main_layout = new Layout(VERTICAL, this, "main GameWidget layout");
    main_layout->SetIsUsingZeroedFrameMargins(true);
    main_layout->SetIsUsingZeroedLayoutSpacingMargins(true);

    // framerate (and other stuff later) layout
    {
        m_debug_info_layout = new Layout(HORIZONTAL, main_layout, "debugging info layout");
        m_debug_info_layout->SetBackground(new WidgetBackgroundColored(Color(0.0f, 0.0f, 0.0f, 0.5f)));
        
        m_world_frame_time_label =
            new ValueLabel<Uint32>(
                "%u ms (world frame)",
                Util::TextToUint32,
                m_debug_info_layout,
                "world frame time label");
        m_world_frame_time_label->SetIsHeightFixedToTextHeight(true);
        m_world_frame_time_label->SetAlignment(Dim::X, RIGHT);
        
        m_gui_frame_time_label =
            new ValueLabel<Uint32>(
                "%u ms (gui processing)",
                Util::TextToUint32,
                m_debug_info_layout,
                "gui frame time label");
        m_gui_frame_time_label->SetIsHeightFixedToTextHeight(true);
        m_gui_frame_time_label->SetAlignment(Dim::X, RIGHT);
        
        m_render_frame_time_label =
            new ValueLabel<Uint32>(
                "%u ms (render)",
                Util::TextToUint32,
                m_debug_info_layout,
                "render frame time label");
        m_render_frame_time_label->SetIsHeightFixedToTextHeight(true);
        m_render_frame_time_label->SetAlignment(Dim::X, RIGHT);
        
        m_framerate_label =
            new ValueLabel<Float>(
                "%.1f fps",
                Util::TextToFloat,
                m_debug_info_layout,
                "framerate label");
        m_framerate_label->SetIsHeightFixedToTextHeight(true);
        m_framerate_label->SetAlignment(Dim::X, RIGHT);

        // start hidden
        m_debug_info_layout->Hide();    
        // connect the debug info enabled signal
        SignalHandler::Connect1(
            m_world_view->SenderIsDebugInfoEnabledChanged(),
            &Transformation::BooleanNegation,
            m_debug_info_layout->ReceiverSetIsHidden());
    }

    // time, mineral inventory, score layout and stoke-o-meter
    {
        Layout *time_alive_minerals_and_score_layout = new Layout(HORIZONTAL, main_layout, "time-alive and score layout");
        time_alive_minerals_and_score_layout->SetBackground(new WidgetBackgroundColored(Color(0.0f, 0.0f, 0.0f, 0.5f)));
        
        m_time_alive_label = new Label("", time_alive_minerals_and_score_layout, "time-alive label");
        m_time_alive_label->SetIsHeightFixedToTextHeight(true);
        m_time_alive_label->SetAlignment(Dim::X, LEFT);
        m_time_alive_label->SetFontHeightRatio(0.05f);

        for (Uint8 mineral_type = 0; mineral_type < MINERAL_COUNT; ++mineral_type)
        {
            m_mineral_inventory_label[mineral_type] =
                new ValueLabel<Uint32>(
                    "%u",
                    Util::TextToUint32,
                    time_alive_minerals_and_score_layout);
            m_mineral_inventory_label[mineral_type]->SetIsHeightFixedToTextHeight(true);
            m_mineral_inventory_label[mineral_type]->SetAlignment(Dim::X, RIGHT);
            
            Label *mineral_icon_label =
                new Label(
                    Singletons::ResourceLibrary()->
                        LoadFilename<GLTexture>(
                            GLTexture::Create,
                            Item::GetMineralSpriteFilename(mineral_type)),
                    time_alive_minerals_and_score_layout);
            mineral_icon_label->FixWidth(m_mineral_inventory_label[mineral_type]->GetHeight());
            mineral_icon_label->FixHeight(m_mineral_inventory_label[mineral_type]->GetHeight());
        }
        // a spacer to make things look even
        new SpacerWidget(time_alive_minerals_and_score_layout);

        // a widget stack for the score label and stoke-o-meter
        WidgetStack *widget_stack =
            new WidgetStack(
                time_alive_minerals_and_score_layout,
                "score label and stoke-o-meter widget stack");

        m_stoke_o_meter = new ProgressBar(ProgressBar::GO_LEFT, widget_stack, "stoke-o-meter");
        m_stoke_o_meter->SetColor(Color(1.0f, 1.0f, 1.0f, 0.3f));
                
        m_score_label =
            new ValueLabel<Uint32>(
                "%u",
                Util::TextToUint32,
                widget_stack,
                "score label");
        m_score_label->SetIsHeightFixedToTextHeight(true);
        m_score_label->SetAlignment(Dim::X, RIGHT);
        m_score_label->SetFontHeightRatio(0.05f);
    }

    // spacer widget (for the vertical space in the HUD between
    // the upper and lower status bars)
    m_view_spacer_widget = new SpacerWidget(main_layout);

    // armor/shield, power and weapon status layout
    {
        Layout *ship_status_layout = new Layout(HORIZONTAL, main_layout, "ship status layout");

        // contains the armor and shield status bars, and keeps the
        // shield status bar on top of the armor status bar        
        WidgetStack *armor_and_shield_stack = new WidgetStack(ship_status_layout);
        
        m_armor_status =
            new ProgressBar(
                ProgressBar::GO_RIGHT,
                armor_and_shield_stack,
                "armor status");
        m_armor_status->FixHeight(m_score_label->GetHeight()/2);
        m_armor_status->SetColor(Color(0.5f, 1.0f, 0.5f, 0.5f));
        
        m_shield_status =
            new ProgressBar(
                ProgressBar::GO_RIGHT,
                armor_and_shield_stack,
                "shield status");
        m_shield_status->FixHeight(m_score_label->GetHeight()/2);
        m_shield_status->SetColor(Color(0.5f, 0.5f, 1.0f, 0.5f));
        
        m_power_status =
            new ProgressBar(
                ProgressBar::GO_RIGHT,
                ship_status_layout,
                "power status");
        m_power_status->FixHeight(m_score_label->GetHeight()/2);
        m_power_status->SetColor(Color(1.0f, 1.0f, 0.5f, 0.5f));
        
        m_weapon_status =
            new ProgressBar(
                ProgressBar::GO_RIGHT,
                ship_status_layout,
                "weapon status");    
        m_weapon_status->FixHeight(m_score_label->GetHeight()/2);
        m_weapon_status->SetColor(Color(1.0f, 0.5f, 0.5f, 0.5f));
    }

    // initialize the player ship pointer, and connect up the
    // corresponding signal/receiver from the world view
    SetPlayerShip(m_world_view->GetPlayerShip());
    SignalHandler::Connect1(
        m_world_view->SenderPlayerShipChanged(),
        &m_receiver_set_player_ship);
    
    m_world_view_widget->Focus();
}

SignalSender0 const *GameWidget::SenderEndGame ()
{
    ASSERT1(m_inventory_panel != NULL)
    return m_inventory_panel->SenderEndGame();
}

SignalSender0 const *GameWidget::SenderQuitGame ()
{
    ASSERT1(m_inventory_panel != NULL)
    return m_inventory_panel->SenderQuitGame();
}

void GameWidget::SetPlayerShip (PlayerShip *const player_ship)
{
    // disconnect old player ship connections
    m_score_label->DetachAll();
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

        // make sure the UI elements are enabled
        m_score_label->Enable();
        m_armor_status->Enable();
        m_shield_status->Enable();
        m_power_status->Enable();
        m_weapon_status->Enable();
        for (Uint32 i = 0; i < MINERAL_COUNT; ++i)
            m_mineral_inventory_label[i]->Enable();
            
        // initialize the UI elements
        m_stoke_o_meter->SetProgress(NormalizeStoke(player_ship->GetStoke()));
        m_score_label->SetValue(player_ship->GetScore());
        m_armor_status->SetProgress(player_ship->GetArmorStatus());
        m_shield_status->SetProgress(player_ship->GetShieldStatus());
        m_power_status->SetProgress(player_ship->GetPowerStatus());
        m_weapon_status->SetProgress(player_ship->GetWeaponStatus());
        for (Uint32 i = 0; i < MINERAL_COUNT; ++i)
            m_mineral_inventory_label[i]->SetValue(static_cast<Uint32>(player_ship->GetMineralInventory(i)));

        m_inventory_panel->SetInventoryOwnerShip(player_ship);
    }
    // otherwise set the UI elements to zero, and disable them
    else
    {
        m_score_label->Disable();
        m_armor_status->Disable();
        m_shield_status->Disable();
        m_power_status->Disable();
        m_weapon_status->Disable();
        for (Uint32 i = 0; i < MINERAL_COUNT; ++i)
            m_mineral_inventory_label[i]->Disable();
    }    
}

void GameWidget::SetWorldFrameTime (Uint32 const world_frame_time)
{
    ASSERT1(m_world_frame_time_label != NULL)
    m_world_frame_time_label->SetValue(world_frame_time);
}

void GameWidget::SetGUIFrameTime (Uint32 const gui_frame_time)
{
    ASSERT1(m_gui_frame_time_label != NULL)
    m_gui_frame_time_label->SetValue(gui_frame_time);
}

void GameWidget::SetRenderFrameTime (Uint32 const render_frame_time)
{
    ASSERT1(m_render_frame_time_label != NULL)
    m_render_frame_time_label->SetValue(render_frame_time);
}

void GameWidget::SetFramerate (Float const framerate)
{
    ASSERT1(m_framerate_label != NULL)
    m_framerate_label->SetValue(framerate);
}

void GameWidget::SetMineralInventory (
    Uint8 const mineral_type,
    Float const mineral_inventory)
{
    ASSERT1(mineral_type < MINERAL_COUNT)
    ASSERT1(mineral_inventory >= 0.0f)
    m_mineral_inventory_label[mineral_type]->SetValue(static_cast<Uint32>(mineral_inventory));
}

void GameWidget::ActivateInventoryPanel ()
{
    ASSERT1(m_inventory_panel->GetIsHidden())
    ASSERT1(m_inventory_panel->GetIsModal())

    // pause the game
    ASSERT1(m_saved_game_timescale == -1.0f)
    m_saved_game_timescale = m_world_view->GetWorld()->GetTimescale();
    m_world_view->GetWorld()->SetTimescale(0.0f);

    // update the owned inventory items
    
    
    // show the inventory panel and resize appropriately
    m_inventory_panel->UpdatePanelState();
    m_inventory_panel->Show();
    m_inventory_panel->ResizeByRatios(FloatVector2(0.8f, 0.8f));
    m_inventory_panel->CenterOnWidget(GetTopLevelParent());
    m_inventory_panel->Focus();
}

void GameWidget::DeactivateInventoryPanel ()
{
    ASSERT1(!m_inventory_panel->GetIsHidden())
    ASSERT1(m_inventory_panel->GetIsModal())

    // hide the inventory panel
    m_inventory_panel->Hide();
    // move the focus back to the world view widget
    m_world_view_widget->Focus();

    // unpause the game
    ASSERT1(m_saved_game_timescale >= 0.0f)
    m_world_view->GetWorld()->SetTimescale(m_saved_game_timescale);
    m_saved_game_timescale = -1.0f;
}

void GameWidget::ProcessFrameOverride ()
{
    Widget::ProcessFrameOverride();

    // TODO: replace this time-getting stuff when the real game controller is written
    Float game_time = m_world_view->GetWorld()->GetMostRecentFrameTime();
    Uint32 game_time_seconds = static_cast<Uint32>(game_time);
    Uint32 minutes_alive = game_time_seconds / 60;
    Uint32 seconds_alive = game_time_seconds % 60;
    Uint32 centiseconds_alive = static_cast<Uint32>(100.0f * game_time) % 100;
    m_time_alive_label->SetText(
        Util::StringPrintf(
            "%02u:%02u.%02u",
            minutes_alive,
            seconds_alive,
            centiseconds_alive));
}

} // end of namespace Dis

