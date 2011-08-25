// ///////////////////////////////////////////////////////////////////////////
// dis_inventorypanel.cpp by Victor Dods, created 2005/12/17
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_inventorypanel.hpp"

#include "dis_controlspanel.hpp"
#include "dis_playership.hpp"
#include "xrb_cellpaddingwidget.hpp"
#include "xrb_input_events.hpp"
#include "xrb_layout.hpp"
#include "xrb_math.hpp"
#include "xrb_parse_datafile.hpp"
#include "xrb_screen.hpp"
#include "xrb_spacerwidget.hpp"
#include "xrb_valuelabel.hpp"
#include "xrb_widgetbackground.hpp"
#include "xrb_widgetstack.hpp"

using namespace Xrb;

extern Dis::Config g_config;

namespace Dis {

Color const InventoryPanel::ms_affordable_mineral_color_mask(1.0f, 1.0f, 1.0f, 1.0f);
Color const InventoryPanel::ms_not_affordable_mineral_color_mask(1.0f, 0.3f, 0.0f, 1.0f);

InventoryPanel::InventoryPanel (ContainerWidget *parent, std::string const &name)
    :
    ModalWidget(parent, name),
    m_sender_deactivate(this),
    m_receiver_attempt_to_upgrade_item(&InventoryPanel::AttemptToUpgradeItem, this),
    m_receiver_equip_item(&InventoryPanel::EquipItem, this),
    m_receiver_show_price(&InventoryPanel::ShowPrice, this),
    m_receiver_hide_price(&InventoryPanel::HidePrice, this),
    m_internal_receiver_deactivate(&InventoryPanel::Deactivate, this),
    m_internal_receiver_activate_controls_dialog(&InventoryPanel::ActivateControlsDialog, this),
    m_internal_receiver_controls_dialog_returned(&InventoryPanel::ControlsDialogReturned, this)
{
    m_accepts_focus = true;

    m_inventory_owner_ship = NULL;
    m_controls_panel = NULL;

    m_game_tip_data = Parse::DataFile::Load("resources/game_tip.data");
    // TODO: maybe check if the load failed if so, put an error message in the tip data

//     static Float const s_grid_label_font_height_ratio = 0.018f;

    // a vertical layout to hold the sublayouts
    Layout *main_layout = new Layout(VERTICAL, this, "main InventoryPanel layout");
    main_layout->SetIsUsingZeroedFrameMargins(false);
    SetMainWidget(main_layout);

    {
        Label *label = new Label("INVENTORY", main_layout);
        label->SetIsHeightFixedToTextHeight(true);
    }

    {
        Layout *tractor_and_weapons_layout = new Layout(HORIZONTAL, main_layout, "tractor and weapons layout");

        // weapons
        ASSERT0(IT_WEAPON_LOWEST == 0);
        for (Uint32 i = IT_WEAPON_LOWEST; i <= IT_WEAPON_HIGHEST; ++i)
            CreateInventoryButtonAndFriends(static_cast<ItemType>(i), tractor_and_weapons_layout);
    }

    {
        Layout *other_items_and_game_tip_layout = new Layout(HORIZONTAL, main_layout, "other items and game tip layout");

        // other items
        for (Uint32 i = IT_WEAPON_HIGHEST+1; i < IT_COUNT; ++i)
            CreateInventoryButtonAndFriends(static_cast<ItemType>(i), other_items_and_game_tip_layout);

        // game tip
        {
            Layout *l = new Layout(VERTICAL, other_items_and_game_tip_layout);
            Layout *header = new Layout(HORIZONTAL, l);
            m_game_tip_header_label = new ValueLabel<Uint32>(FORMAT("GAME TIP (%u of " << m_game_tip_data->ValueCount() << ')'), Util::TextToUint<Uint32>, header);
//             m_game_tip_header_label->SetFontHeightRatio(s_grid_label_font_height_ratio);
            m_game_tip_header_label->SetIsHeightFixedToTextHeight(true);
            m_game_tip_header_label->SetValue(1); // NOTE: TEMP
            // TODO: hide-game-tips button

            Layout *mid = new Layout(HORIZONTAL, l);
            m_game_tip_text_label = new Label("X", mid);
//             m_game_tip_text_label->SetFontHeightRatio(s_grid_label_font_height_ratio);
            m_game_tip_picture_label = new Label(Resource<GlTexture>(), mid);

            Layout *footer = new Layout(HORIZONTAL, l);
            m_game_tip_previous_button = new Button("PREVIOUS TIP", footer);
//             m_game_tip_previous_button->SetFontHeightRatio(s_grid_label_font_height_ratio);
            m_game_tip_previous_button->SetIsHeightFixedToTextHeight(true);
            m_game_tip_next_button = new Button("NEXT TIP", footer);
//             m_game_tip_next_button->SetFontHeightRatio(s_grid_label_font_height_ratio);
            m_game_tip_next_button->SetIsHeightFixedToTextHeight(true);
        }
    }

    // a horizontal layout for the price display
    {
        Layout *price_layout = new Layout(HORIZONTAL, main_layout, "InventoryPanel price layout");

        new SpacerWidget(price_layout);

        Label *l = new Label("UPGRADE COST", price_layout);
        l->SetIsHeightFixedToTextHeight(true);

        for (Uint8 mineral_index = 0; mineral_index < MINERAL_COUNT; ++mineral_index)
        {
            m_mineral_cost_label[mineral_index] =
                new ValueLabel<Uint32>(
                    "%u",
                    Util::TextToUint<Uint32>,
                    price_layout);
            m_mineral_cost_label[mineral_index]->SetIsHeightFixedToTextHeight(true);
            m_mineral_cost_label[mineral_index]->SetAlignment(Dim::X, RIGHT);
            m_mineral_cost_label[mineral_index]->SetValue(0);
            m_mineral_cost_label[mineral_index]->Disable();

            m_mineral_icon_label[mineral_index] =
                new Label(
                    GlTexture::Load(Item::MineralSpritePath(mineral_index)),
                    price_layout);
            m_mineral_icon_label[mineral_index]->FixWidth(m_mineral_cost_label[mineral_index]->Height());
            m_mineral_icon_label[mineral_index]->FixHeight(m_mineral_cost_label[mineral_index]->Height());
            m_mineral_icon_label[mineral_index]->Disable();
        }

        new SpacerWidget(price_layout);

        m_prices_are_shown = false;
    }

    {
        Layout *menu_button_layout = new Layout(HORIZONTAL, main_layout, "menu button layout");

        m_return_button = new Button("RETURN", menu_button_layout, "return button");
        m_return_button->SetIsHeightFixedToTextHeight(true);

        SignalHandler::Connect0(
            m_return_button->SenderReleased(),
            &m_internal_receiver_deactivate);

        m_controls_button = new Button("CONTROLS", menu_button_layout, "controls button");
        m_controls_button->SetIsHeightFixedToTextHeight(true);

        SignalHandler::Connect0(
            m_controls_button->SenderReleased(),
            &m_internal_receiver_activate_controls_dialog);

        m_end_button = new Button("END", menu_button_layout, "end button");
        m_end_button->SetIsHeightFixedToTextHeight(true);

        m_quit_button = new Button("QUIT", menu_button_layout, "quit button");
        m_quit_button->SetIsHeightFixedToTextHeight(true);
    }

    SetBackground(new WidgetBackgroundColored(Color(0.0f, 0.0f, 0.0f, 1.0f)));
}

SignalSender0 const *InventoryPanel::SenderEndGame ()
{
    ASSERT1(m_end_button != NULL);
    return m_end_button->SenderReleased();
}

SignalSender0 const *InventoryPanel::SenderQuitGame ()
{
    ASSERT1(m_quit_button != NULL);
    return m_quit_button->SenderReleased();
}

void InventoryPanel::UpdatePanelState ()
{
    if (m_inventory_owner_ship != NULL)
    {
        for (Uint32 item = 0; item < IT_COUNT; ++item)
        {
            ASSERT1(m_inventory_button[item] != NULL);

            Uint8 owned_upgrade_level = m_inventory_owner_ship->HighestUpgradeLevelInInventory(static_cast<ItemType>(item));
            ASSERT1(Uint8(owned_upgrade_level+1) <= UPGRADE_LEVEL_COUNT);
            bool is_equipped = false;
            if (Uint8(owned_upgrade_level+1) > 0) // item is owned
                is_equipped = m_inventory_owner_ship->IsItemEquipped(static_cast<ItemType>(item), owned_upgrade_level);
            else // item is not owned
                owned_upgrade_level = Uint8(-1);
            bool is_affordable = false;
            if (Uint8(owned_upgrade_level+1) != UPGRADE_LEVEL_COUNT)
                is_affordable = m_inventory_owner_ship->IsItemAffordable(static_cast<ItemType>(item), Uint8(owned_upgrade_level+1));

            m_inventory_button[item]->SetProperties(is_equipped, owned_upgrade_level, is_affordable);
            m_upgrade_indication_label[item]->SetText(m_inventory_button[item]->UpgradeIndication());
            m_upgrade_level_label[item]->SetValue(m_inventory_button[item]->UpgradeLevelNumeral());
        }

        if (m_prices_are_shown)
        {
            for (Uint32 mineral_index = 0; mineral_index < MINERAL_COUNT; ++mineral_index)
            {
                Uint32 mineral_cost;
                if (m_inventory_owner_ship->IsItemInInventory(m_currently_shown_price_item_type, m_currently_shown_price_upgrade_level))
                    mineral_cost = 0;
                else
                    mineral_cost = Item::ItemPrice(
                        m_currently_shown_price_item_type,
                        m_currently_shown_price_upgrade_level,
                        mineral_index);
                m_mineral_cost_label[mineral_index]->SetValue(mineral_cost);

                if (mineral_cost > 0)
                {
                    m_mineral_cost_label[mineral_index]->Enable();
                    m_mineral_icon_label[mineral_index]->Enable();
                }
                else
                {
                    m_mineral_cost_label[mineral_index]->Disable();
                    m_mineral_icon_label[mineral_index]->Disable();
                }

                Uint32 player_ship_minerals =
                    static_cast<Uint32>(
                        m_inventory_owner_ship->MineralInventory(mineral_index));
                Uint32 item_cost_minerals = m_mineral_cost_label[mineral_index]->Value();
                if (player_ship_minerals < item_cost_minerals)
                    m_mineral_cost_label[mineral_index]->ColorMask() = ms_not_affordable_mineral_color_mask;
                else
                    m_mineral_cost_label[mineral_index]->ColorMask() = ms_affordable_mineral_color_mask;
            }
        }
    }
    else
    {
        for (Uint32 i = 0; i < IT_COUNT; ++i)
        {
            ASSERT1(m_inventory_button[i] != NULL);
            m_inventory_button[i]->SetProperties(false, Uint8(-1), false);
        }

        for (Uint32 i = 0; i < MINERAL_COUNT; ++i)
        {
            ASSERT1(m_mineral_cost_label[i] != NULL);
            m_mineral_cost_label[i]->ColorMask() = ms_affordable_mineral_color_mask;
        }
    }
}

bool InventoryPanel::ProcessKeyEvent (EventKey const *e)
{
    if (e->IsKeyDownEvent() && e->KeyCode() == Key::ESCAPE)
    {
        Deactivate();
        return true;
    }
    else
        return false;
}

void InventoryPanel::CreateInventoryButtonAndFriends (ItemType item_type, ContainerWidget *parent)
{
    ASSERT1(item_type < IT_COUNT);
    ASSERT1(parent != NULL);

    static std::string const s_item_name[IT_COUNT] =
    {
        "TRACTOR\nBEAM",                    // IT_WEAPON_TRACTOR = 0,
        "PLASMA\nCANNON",                   // IT_WEAPON_PEA_SHOOTER,
        "LASER\nDRILL",                     // IT_WEAPON_LASER,
        "FLAME\nTHROWER",                   // IT_WEAPON_FLAME_THROWER,
        "GAUSS\nGUN",                       // IT_WEAPON_GAUSS_GUN,
        "GRENADE\nLAUNCHER",                // IT_WEAPON_GRENADE_LAUNCHER,
        "MISSILE\nLAUNCHER",                // IT_WEAPON_MISSILE_LAUNCHER,
        "ENGINE",                           // IT_ENGINE,
        "ARMOR",                            // IT_ARMOR,
        "SHIELD",                           // IT_SHIELD,
        "POWER",                            // IT_POWER_GENERATOR,
    };
    static FloatVector2 const s_inventory_button_size_ratios(0.14f, 0.14f);

    Layout *l = new Layout(VERTICAL, parent);
    Label *label = new Label(s_item_name[item_type], l);
    label->SetIsHeightFixedToTextHeight(true);

    WidgetStack *ws = new WidgetStack(l);

    // bottom of the stack
    m_inventory_button[item_type] = new InventoryButton(item_type, ws);
    m_inventory_button[item_type]->FixSizeRatios(s_inventory_button_size_ratios);
    // middle of the stack
    m_upgrade_indication_label[item_type] = new Label("X", ws);
    m_upgrade_indication_label[item_type]->SetFontHeightRatio(0.25f * s_inventory_button_size_ratios[Dim::Y]);
    m_upgrade_indication_label[item_type]->SetAlignment(Alignment2(CENTER, CENTER));
    m_upgrade_indication_label[item_type]->SetTextColor(Color(0.5f, 0.5f, 0.5f, 0.6f));
    // top of the stack
    CellPaddingWidget *cpw = new CellPaddingWidget(ws);
    cpw->SetAlignment(Alignment2(LEFT, BOTTOM));
    cpw->SetFrameMarginRatios(0.1f * s_inventory_button_size_ratios);
    m_upgrade_level_label[item_type] = new ValueLabel<Uint32>("%u", Util::TextToUint<Uint32>, cpw);
//     m_upgrade_level_label[item_type]->FixSizeRatios(0.3f * s_inventory_button_size_ratios);
    m_upgrade_level_label[item_type]->FixWidth(m_upgrade_level_label[item_type]->Height());
    m_upgrade_level_label[item_type]->SetBackground(new WidgetBackgroundTextured("resources/radiobutton_black.png"));

    ConnectInventoryButton(*m_inventory_button[item_type], *(m_upgrade_indication_label[item_type]), *(m_upgrade_level_label[item_type]));
}

void InventoryPanel::ConnectInventoryButton (InventoryButton &inventory_button, Label &upgrade_indication_label, ValueLabel<Uint32> &upgrade_level_label)
{
    SignalHandler::Connect2(
        inventory_button.SenderAttemptToUpgradeItem(),
        &m_receiver_attempt_to_upgrade_item);
    SignalHandler::Connect2(
        inventory_button.SenderEquipItem(),
        &m_receiver_equip_item);
    SignalHandler::Connect2(
        inventory_button.SenderShowPrice(),
        &m_receiver_show_price);
    SignalHandler::Connect1(
        inventory_button.SenderHidePrice(),
        &m_receiver_hide_price);

    SignalHandler::Connect1(
        inventory_button.SenderUpgradeIndication(),
        upgrade_indication_label.ReceiverSetTextV());
    SignalHandler::Connect1(
        inventory_button.SenderUpgradeLevelNumeral(),
        upgrade_level_label.ReceiverSetValue());
}

void InventoryPanel::AttemptToUpgradeItem (ItemType item_type, Uint8 upgrade_level)
{
    ASSERT1(item_type < IT_COUNT);
    ASSERT1(Uint8(upgrade_level+1) <= UPGRADE_LEVEL_COUNT);

    if (m_inventory_owner_ship->BuyItem(item_type, Uint8(upgrade_level+1)))
        UpdatePanelState();
}

void InventoryPanel::EquipItem (ItemType item_type, Uint8 upgrade_level)
{
    ASSERT1(item_type < IT_COUNT);
    ASSERT1(upgrade_level < UPGRADE_LEVEL_COUNT);

    m_inventory_owner_ship->EquipItem(item_type, upgrade_level);
    UpdatePanelState();
}

void InventoryPanel::ShowPrice (ItemType item_type, Uint8 upgrade_level)
{
    ASSERT1(item_type < IT_COUNT);
    ASSERT1(upgrade_level < UPGRADE_LEVEL_COUNT);
    ASSERT1(!m_prices_are_shown);

    m_currently_shown_price_item_type = item_type;
    m_currently_shown_price_upgrade_level = upgrade_level;

    // enable the widgets and set the prices
    for (Uint8 mineral_index = 0; mineral_index < MINERAL_COUNT; ++mineral_index)
    {
        ASSERT1(!m_mineral_cost_label[mineral_index]->IsEnabled());
        ASSERT1(!m_mineral_icon_label[mineral_index]->IsEnabled());

        Uint32 mineral_cost;
        if (m_inventory_owner_ship->IsItemInInventory(m_currently_shown_price_item_type, m_currently_shown_price_upgrade_level))
            mineral_cost = 0;
        else
            mineral_cost = Item::ItemPrice(
                m_currently_shown_price_item_type,
                m_currently_shown_price_upgrade_level,
                mineral_index);

        if (mineral_cost > 0)
        {
            m_mineral_cost_label[mineral_index]->Enable();
            m_mineral_icon_label[mineral_index]->Enable();

            m_mineral_cost_label[mineral_index]->SetValue(mineral_cost);

            Uint32 player_ship_minerals =
                static_cast<Uint32>(
                    m_inventory_owner_ship->MineralInventory(mineral_index));
            Uint32 item_cost_minerals = m_mineral_cost_label[mineral_index]->Value();
            if (player_ship_minerals < item_cost_minerals)
                m_mineral_cost_label[mineral_index]->ColorMask() = ms_not_affordable_mineral_color_mask;
            else
                m_mineral_cost_label[mineral_index]->ColorMask() = ms_affordable_mineral_color_mask;
        }
    }

    m_prices_are_shown = true;
}

void InventoryPanel::HidePrice (ItemType item_type)
{
    ASSERT1(item_type < IT_COUNT);

    // clear all the prices and disable the widgets
    for (Uint8 i = 0; i < MINERAL_COUNT; ++i)
    {
        if (m_mineral_cost_label[i]->Value() > 0)
        {
            ASSERT1(m_mineral_cost_label[i]->IsEnabled());
            ASSERT1(m_mineral_icon_label[i]->IsEnabled());

            m_mineral_cost_label[i]->SetValue(0);

            m_mineral_cost_label[i]->Disable();
            m_mineral_icon_label[i]->Disable();

            m_mineral_cost_label[i]->ColorMask() = ms_affordable_mineral_color_mask;
        }
        else
        {
            ASSERT1(!m_mineral_cost_label[i]->IsEnabled());
            ASSERT1(!m_mineral_icon_label[i]->IsEnabled());
        }
    }

    m_prices_are_shown = false;
}

void InventoryPanel::Deactivate ()
{
    m_sender_deactivate.Signal();
}

void InventoryPanel::ActivateControlsDialog ()
{
    ASSERT1(m_controls_panel == NULL);
    // create the dialog and add a new ControlsPanel to it
    Dialog *controls_dialog = new Dialog(Dialog::DT_OK_CANCEL, this, "controls dialog");
    m_controls_panel = new ControlsPanel(controls_dialog->DialogLayout());
    // initialize the ControlsPanel with the Config values
    m_controls_panel->ReadValuesFromConfig(g_config);
    // make the dialog full-screen
    ASSERT1(TopLevelParent() != NULL);
    controls_dialog->MoveToAndResize(TopLevelParent()->ScreenRect());
    // connect up the dialog OK button to ControlsDialogReturned
    SignalHandler::Connect1(
        controls_dialog->SenderDialogReturned(),
        &m_internal_receiver_controls_dialog_returned);
}

void InventoryPanel::ControlsDialogReturned (Dialog::ButtonID button_id)
{
    ASSERT1(m_controls_panel != NULL);

    // only save the ControlsPanel values back into the Config if OK button was hit
    if (button_id == Dialog::ID_OK)
        m_controls_panel->WriteValuesToConfig(&g_config);

    m_controls_panel = NULL;
}

} // end of namespace Dis

