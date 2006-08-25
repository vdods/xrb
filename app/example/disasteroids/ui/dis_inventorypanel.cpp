// ///////////////////////////////////////////////////////////////////////////
// dis_inventorypanel.cpp by Victor Dods, created 2005/12/17
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_inventorypanel.h"

#include "dis_optionspanel.h"
#include "dis_playership.h"
#include "xrb_input_events.h"
#include "xrb_layout.h"
#include "xrb_math.h"
#include "xrb_screen.h"
#include "xrb_spacerwidget.h"
#include "xrb_valuelabel.h"
#include "xrb_widgetbackground.h"

using namespace Xrb;

extern Dis::Config g_config;

namespace Dis
{

Color const InventoryPanel::ms_affordable_mineral_color_mask(1.0f, 1.0f, 1.0f, 1.0f);
Color const InventoryPanel::ms_not_affordable_mineral_color_mask(1.0f, 0.3f, 0.0f, 1.0f);

InventoryPanel::InventoryPanel (
    ContainerWidget *const parent,
    std::string const &name)
    :
    ModalWidget(parent, name),
    m_sender_deactivate(this),
    m_receiver_attempt_to_buy_item(&InventoryPanel::AttemptToBuyItem, this),
    m_receiver_equip_item(&InventoryPanel::EquipItem, this),
    m_receiver_show_price(&InventoryPanel::ShowPrice, this),
    m_receiver_hide_price(&InventoryPanel::HidePrice, this),
    m_internal_receiver_deactivate(&InventoryPanel::Deactivate, this),
    m_internal_receiver_activate_options_dialog(&InventoryPanel::ActivateOptionsDialog, this),
    m_internal_receiver_options_dialog_returned(&InventoryPanel::OptionsDialogReturned, this)
{
    m_accepts_focus = true;

    m_inventory_owner_ship = NULL;

    Label *l;
    Float const grid_label_font_height_ratio = 0.018f;

    // a vertical layout to hold the sublayouts
    Layout *main_layout = new Layout(VERTICAL, this, "main InventoryPanel layout");
    main_layout->SetIsUsingZeroedFrameMargins(false);
    SetMainWidget(main_layout);

    // a horizontal layout to hold the sublayouts for each of
    // weapons/tractor/engine/shield/armor/power
    Layout *penultimate_layout = new Layout(HORIZONTAL, main_layout, "penultimate InventoryPanel layout");

    // weapons layout
    Layout *weapons_layout = new Layout(VERTICAL, penultimate_layout, "weapons layout");
    l = new Label("WEAPONS", weapons_layout, "weapons layout label");
    l->SetFontHeightRatio(grid_label_font_height_ratio);
    l->SetIsHeightFixedToTextHeight(true);
    Layout *weapons_grid_layout = new Layout(COLUMN, UPGRADE_LEVEL_COUNT, weapons_layout, "weapons grid-layout");
    // this doesn't include IT_WEAPON_TRACTOR
    for (Uint8 item_type = IT_WEAPON_LOWEST; item_type < IT_WEAPON_HIGHEST; ++item_type)
        CreateInventoryButtonColumn(static_cast<ItemType>(item_type), weapons_grid_layout);

    // tractor layout
    Layout *tractor_layout = new Layout(VERTICAL, penultimate_layout, "tractor layout");
    l = new Label("TRACTOR", tractor_layout, "tractor layout label");
    l->SetFontHeightRatio(grid_label_font_height_ratio);
    l->SetIsHeightFixedToTextHeight(true);
    Layout *tractor_grid_layout = new Layout(COLUMN, UPGRADE_LEVEL_COUNT, tractor_layout, "tractor grid-layout");
    CreateInventoryButtonColumn(IT_WEAPON_TRACTOR, tractor_grid_layout);

    // engine layout
    Layout *engine_layout = new Layout(VERTICAL, penultimate_layout, "engine layout");
    l = new Label("ENGINE", engine_layout, "engine layout label");
    l->SetFontHeightRatio(grid_label_font_height_ratio);
    l->SetIsHeightFixedToTextHeight(true);
    Layout *engine_grid_layout = new Layout(COLUMN, UPGRADE_LEVEL_COUNT, engine_layout, "engine grid-layout");
    CreateInventoryButtonColumn(IT_ENGINE, engine_grid_layout);

    // armor layout
    Layout *armor_layout = new Layout(VERTICAL, penultimate_layout, "armor layout");
    l = new Label("ARMOR", armor_layout, "armor layout label");
    l->SetFontHeightRatio(grid_label_font_height_ratio);
    l->SetIsHeightFixedToTextHeight(true);
    Layout *armor_grid_layout = new Layout(COLUMN, UPGRADE_LEVEL_COUNT, armor_layout, "armor grid-layout");
    CreateInventoryButtonColumn(IT_ARMOR, armor_grid_layout);

    // shield layout
    Layout *shield_layout = new Layout(VERTICAL, penultimate_layout, "shield layout");
    l = new Label("SHIELD", shield_layout, "shield layout label");
    l->SetFontHeightRatio(grid_label_font_height_ratio);
    l->SetIsHeightFixedToTextHeight(true);
    Layout *shield_grid_layout = new Layout(COLUMN, UPGRADE_LEVEL_COUNT, shield_layout, "shield grid-layout");
    CreateInventoryButtonColumn(IT_SHIELD, shield_grid_layout);

    // power layout
    Layout *power_layout = new Layout(VERTICAL, penultimate_layout, "power layout");
    l = new Label("POWER", power_layout, "power layout label");
    l->SetFontHeightRatio(grid_label_font_height_ratio);
    l->SetIsHeightFixedToTextHeight(true);
    Layout *power_grid_layout = new Layout(COLUMN, UPGRADE_LEVEL_COUNT, power_layout, "power grid-layout");
    CreateInventoryButtonColumn(IT_POWER_GENERATOR, power_grid_layout);

    // a horizontal layout for the price display
    Layout *price_layout = new Layout(HORIZONTAL, main_layout, "InventoryPanel price layout");

    for (Uint8 mineral_index = 0; mineral_index < MINERAL_COUNT; ++mineral_index)
    {
        m_mineral_cost_label[mineral_index] =
            new ValueLabel<Uint32>(
                "%u",
                Util::TextToUint32,
                price_layout);
        m_mineral_cost_label[mineral_index]->SetIsHeightFixedToTextHeight(true);
        m_mineral_cost_label[mineral_index]->SetAlignment(Dim::X, RIGHT);
        m_mineral_cost_label[mineral_index]->SetValue(0);
        m_mineral_cost_label[mineral_index]->Disable();

        m_mineral_icon_label[mineral_index] =
            new Label(
                Singletons::ResourceLibrary().
                    LoadFilename<GLTexture>(
                        GLTexture::Create,
                        Item::GetMineralSpriteFilename(mineral_index)),
                price_layout);
        m_mineral_icon_label[mineral_index]->FixWidth(m_mineral_cost_label[mineral_index]->GetHeight());
        m_mineral_icon_label[mineral_index]->FixHeight(m_mineral_cost_label[mineral_index]->GetHeight());
        m_mineral_icon_label[mineral_index]->Disable();
    }
    // a spacer to make things look even
    new SpacerWidget(price_layout);
    m_prices_are_shown = false;

    Layout *menu_button_layout = new Layout(HORIZONTAL, main_layout, "menu button layout");

    m_return_button = new Button("RETURN", menu_button_layout, "return button");
    m_return_button->SetIsHeightFixedToTextHeight(true);

    SignalHandler::Connect0(
        m_return_button->SenderReleased(),
        &m_internal_receiver_deactivate);

    m_options_button = new Button("OPTIONS", menu_button_layout, "options button");
    m_options_button->SetIsHeightFixedToTextHeight(true);

    SignalHandler::Connect0(
        m_options_button->SenderReleased(),
        &m_internal_receiver_activate_options_dialog);

    m_end_button = new Button("END", menu_button_layout, "end button");
    m_end_button->SetIsHeightFixedToTextHeight(true);

    m_quit_button = new Button("QUIT", menu_button_layout, "quit button");
    m_quit_button->SetIsHeightFixedToTextHeight(true);

    SetBackground(new WidgetBackgroundColored(Color(0.0f, 0.0f, 0.0f, 1.0f)));
}

SignalSender0 const *InventoryPanel::SenderEndGame ()
{
    ASSERT1(m_end_button != NULL)
    return m_end_button->SenderReleased();
}

SignalSender0 const *InventoryPanel::SenderQuitGame ()
{
    ASSERT1(m_quit_button != NULL)
    return m_quit_button->SenderReleased();
}

void InventoryPanel::SetItemStatus (
    ItemType const item_type,
    Uint8 const upgrade_level,
    InventoryButton::Status const status)
{
    ASSERT1(item_type < IT_COUNT)
    ASSERT1(upgrade_level < UPGRADE_LEVEL_COUNT)

    m_inventory_button[item_type][upgrade_level]->SetStatus(status);
}

void InventoryPanel::UpdatePanelState ()
{
    if (m_inventory_owner_ship != NULL)
    {
        for (Uint32 item = 0; item < IT_COUNT; ++item)
        {
            for (Uint32 level = UPGRADE_LEVEL_COUNT-1; level < UPGRADE_LEVEL_COUNT; --level)
            {
                ASSERT1(m_inventory_button[item][level] != NULL)

                InventoryButton::Status status;

                if (m_inventory_owner_ship->GetIsItemEquipped(static_cast<ItemType>(item), level))
                    status = InventoryButton::S_EQUIPPED;
                else if (m_inventory_owner_ship->GetIsItemInInventory(static_cast<ItemType>(item), level))
                    status = InventoryButton::S_OWNED;
                else if (m_inventory_owner_ship->GetIsItemAffordable(static_cast<ItemType>(item), level))
                    status = InventoryButton::S_AFFORDABLE;
                else
                    status = InventoryButton::S_NOT_AFFORDABLE;

                m_inventory_button[item][level]->Enable();
                m_inventory_button[item][level]->SetStatus(status);
            }
        }

        if (m_prices_are_shown)
        {
            for (Uint32 mineral_index = 0; mineral_index < MINERAL_COUNT; ++mineral_index)
            {
                Uint32 mineral_cost;
                if (m_inventory_owner_ship->GetIsItemInInventory(m_currently_shown_price_item_type, m_currently_shown_price_upgrade_level))
                    mineral_cost = 0;
                else
                    mineral_cost = Item::GetItemPrice(
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
                        m_inventory_owner_ship->GetMineralInventory(mineral_index));
                Uint32 item_cost_minerals = m_mineral_cost_label[mineral_index]->GetValue();
                if (player_ship_minerals < item_cost_minerals)
                    m_mineral_cost_label[mineral_index]->SetColorMask(ms_not_affordable_mineral_color_mask);
                else
                    m_mineral_cost_label[mineral_index]->SetColorMask(ms_affordable_mineral_color_mask);
            }
        }
    }
    else
    {
        for (Uint32 i = 0; i < IT_COUNT; ++i)
        {
            for (Uint32 j = UPGRADE_LEVEL_COUNT-1; j < UPGRADE_LEVEL_COUNT; --j)
            {
                ASSERT1(m_inventory_button[i][j] != NULL)
                m_inventory_button[i][j]->Disable();
                m_inventory_button[i][j]->SetStatus(InventoryButton::S_NOT_AFFORDABLE);
            }
        }

        for (Uint32 i = 0; i < MINERAL_COUNT; ++i)
            m_mineral_cost_label[i]->SetColorMask(ms_affordable_mineral_color_mask);
    }
}

bool InventoryPanel::ProcessKeyEvent (EventKey const *const e)
{
    if (e->GetIsKeyDownEvent() && e->GetKeyCode() == Key::ESCAPE)
    {
        Deactivate();
        return true;
    }
    else
        return false;
}

void InventoryPanel::CreateInventoryButtonColumn (ItemType item_type, Layout *parent)
{
    ASSERT1(parent != NULL)
    for (Uint8 level = UPGRADE_LEVEL_COUNT-1; level < UPGRADE_LEVEL_COUNT; --level)
    {
        m_inventory_button[item_type][level] =
            new InventoryButton(
                static_cast<ItemType>(item_type),
                level,
                parent);
        m_inventory_button[item_type][level]->FixSizeRatios(FloatVector2(0.07f, 0.07f));
        ConnectInventoryButton(m_inventory_button[item_type][level]);
    }
}

void InventoryPanel::ConnectInventoryButton (InventoryButton *const inventory_button)
{
    ASSERT1(inventory_button != NULL)
    SignalHandler::Connect2(
        inventory_button->SenderAttemptToBuyItem(),
        &m_receiver_attempt_to_buy_item);
    SignalHandler::Connect2(
        inventory_button->SenderEquipItem(),
        &m_receiver_equip_item);
    SignalHandler::Connect2(
        inventory_button->SenderShowPrice(),
        &m_receiver_show_price);
    SignalHandler::Connect2(
        inventory_button->SenderHidePrice(),
        &m_receiver_hide_price);
}

void InventoryPanel::AttemptToBuyItem (ItemType const item_type, Uint8 const upgrade_level)
{
    ASSERT1(item_type < IT_COUNT)
    ASSERT1(upgrade_level < UPGRADE_LEVEL_COUNT)

    if (m_inventory_owner_ship->BuyItem(item_type, upgrade_level))
        UpdatePanelState();
}

void InventoryPanel::EquipItem (ItemType const item_type, Uint8 const upgrade_level)
{
    ASSERT1(item_type < IT_COUNT)
    ASSERT1(upgrade_level < UPGRADE_LEVEL_COUNT)

    m_inventory_owner_ship->EquipItem(item_type, upgrade_level);
    UpdatePanelState();
}

void InventoryPanel::ShowPrice (ItemType const item_type, Uint8 const upgrade_level)
{
    ASSERT1(item_type < IT_COUNT)
    ASSERT1(upgrade_level < UPGRADE_LEVEL_COUNT)
    ASSERT1(!m_prices_are_shown)

    m_prices_are_shown = true;
    m_currently_shown_price_item_type = item_type;
    m_currently_shown_price_upgrade_level = upgrade_level;

    // enable the widgets and set the prices
    for (Uint8 mineral_index = 0; mineral_index < MINERAL_COUNT; ++mineral_index)
    {
        ASSERT1(!m_mineral_cost_label[mineral_index]->GetIsEnabled())
        ASSERT1(!m_mineral_icon_label[mineral_index]->GetIsEnabled())

        Uint32 mineral_cost;
        if (m_inventory_owner_ship->GetIsItemInInventory(m_currently_shown_price_item_type, m_currently_shown_price_upgrade_level))
            mineral_cost = 0;
        else
            mineral_cost = Item::GetItemPrice(
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
                    m_inventory_owner_ship->GetMineralInventory(mineral_index));
            Uint32 item_cost_minerals = m_mineral_cost_label[mineral_index]->GetValue();
            if (player_ship_minerals < item_cost_minerals)
                m_mineral_cost_label[mineral_index]->SetColorMask(ms_not_affordable_mineral_color_mask);
            else
                m_mineral_cost_label[mineral_index]->SetColorMask(ms_affordable_mineral_color_mask);
        }
    }

    m_prices_are_shown = true;
}

void InventoryPanel::HidePrice (ItemType const item_type, Uint8 const upgrade_level)
{
    ASSERT1(item_type < IT_COUNT)
    ASSERT1(upgrade_level < UPGRADE_LEVEL_COUNT)
    ASSERT1(m_prices_are_shown)
    ASSERT1(m_currently_shown_price_item_type == item_type)
    ASSERT1(m_currently_shown_price_upgrade_level == upgrade_level)

    // clear all the prices and disable the widgets
    for (Uint8 i = 0; i < MINERAL_COUNT; ++i)
    {
        if (m_mineral_cost_label[i]->GetValue() > 0)
        {
            ASSERT1(m_mineral_cost_label[i]->GetIsEnabled())
            ASSERT1(m_mineral_icon_label[i]->GetIsEnabled())

            m_mineral_cost_label[i]->SetValue(0);

            m_mineral_cost_label[i]->Disable();
            m_mineral_icon_label[i]->Disable();

            m_mineral_cost_label[i]->SetColorMask(ms_affordable_mineral_color_mask);
        }
        else
        {
            ASSERT1(!m_mineral_cost_label[i]->GetIsEnabled())
            ASSERT1(!m_mineral_icon_label[i]->GetIsEnabled())
        }
    }

    m_prices_are_shown = false;
}

void InventoryPanel::Deactivate ()
{
    m_sender_deactivate.Signal();
}

void InventoryPanel::ActivateOptionsDialog ()
{
    ASSERT1(m_options_panel == NULL)
    // create the dialog and add a new OptionsPanel to it
    Dialog *options_dialog = new Dialog(Dialog::DT_OK_CANCEL, this, "options dialog");
    m_options_panel = new OptionsPanel(options_dialog->GetDialogLayout());
    options_dialog->Resize(options_dialog->GetParent()->GetSize() * 4 / 5);
    options_dialog->CenterOnWidget(options_dialog->GetParent());
    // initialize the OptionsPanel with the Config values
    m_options_panel->ReadValuesFromConfig(g_config);
    // connect up the dialog OK button to OptionsDialogReturnedOK
    SignalHandler::Connect1(
        options_dialog->SenderDialogReturned(),
        &m_internal_receiver_options_dialog_returned);
}

void InventoryPanel::OptionsDialogReturned (Dialog::ButtonID const button_id)
{
    ASSERT1(m_options_panel != NULL)

    // only save the OptionsPanel values back into the Config if OK button was hit
    if (button_id == Dialog::ID_OK)
        m_options_panel->WriteValuesToConfig(&g_config);

    m_options_panel = NULL;
}

} // end of namespace Dis

