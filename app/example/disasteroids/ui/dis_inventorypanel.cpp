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

#include "dis_playership.h"
#include "xrb_input_events.h"
#include "xrb_math.h"
#include "xrb_layout.h"
#include "xrb_screen.h"
#include "xrb_spacerwidget.h"
#include "xrb_valuelabel.h"
#include "xrb_widgetbackground.h"

using namespace Xrb;

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
    m_internal_receiver_deactivate(&InventoryPanel::Deactivate, this)
{
    m_accepts_focus = true;

    m_inventory_owner_ship = NULL;

    // a vertical layout to hold the sublayouts
    Layout *main_layout = new Layout(VERTICAL, this, "main InventoryPanel layout");
    SetMainWidget(main_layout);

    // a grid layout for all the inventory buttons
    Layout *button_grid_layout = new Layout(COLUMN, UPGRADE_LEVEL_COUNT, main_layout, "InventoryButton grid layout");

    for (Uint8 item = 0; item < IT_COUNT; ++item)
    {
        for (Uint8 level = UPGRADE_LEVEL_COUNT-1; level < UPGRADE_LEVEL_COUNT; --level)
        {
            m_inventory_button[item][level] =
                new InventoryButton(
                    static_cast<ItemType>(item),
                    level,
                    button_grid_layout);
            m_inventory_button[item][level]->FixSizeRatios(FloatVector2(0.07f, 0.07f));
            ConnectInventoryButton(m_inventory_button[item][level]);
        }
    }

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

} // end of namespace Dis

