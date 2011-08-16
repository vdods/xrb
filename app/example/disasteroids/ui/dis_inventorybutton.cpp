// ///////////////////////////////////////////////////////////////////////////
// dis_inventorybutton.cpp by Victor Dods, created 2005/12/17
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_inventorybutton.hpp"

#include "xrb_math.hpp"

using namespace Xrb;

namespace Dis
{

Float const InventoryButton::ms_unequipped_size_ratio = 0.75f;
Float const InventoryButton::ms_size_parameter_change_rate = 4.0f;

InventoryButton::InventoryButton (
    ItemType const item_type,
    Uint8 const upgrade_level,
    ContainerWidget *const parent,
    std::string const &name)
    :
    Button(ButtonTexture(item_type, upgrade_level), parent, name),
    m_item_type(item_type),
    m_upgrade_level(upgrade_level),
    m_sender_attempt_to_buy_item(this),
    m_sender_equip_item(this),
    m_sender_show_price(this),
    m_sender_hide_price(this)
{
    ASSERT1(m_item_type < IT_COUNT);
    ASSERT1(m_upgrade_level < UPGRADE_LEVEL_COUNT);

    // this is intentionally close to, but above 0.0f
    m_current_size_parameter = 0.01f;
    SetStatus(S_NOT_AFFORDABLE);
    SetContentMargins(-FrameMargins());

    UpdateRenderBackground();
}

Resource<GlTexture> InventoryButton::ButtonTexture (
    ItemType const item_type,
    Uint8 const upgrade_level)
{
    ASSERT1(item_type < IT_COUNT);
    ASSERT1(upgrade_level < UPGRADE_LEVEL_COUNT);

    static std::string const s_button_texture_path[IT_COUNT][UPGRADE_LEVEL_COUNT] =
    {
        // IT_WEAPON_PEA_SHOOTER
        {
            "resources/button_pea_shooter_1.png",
            "resources/button_pea_shooter_2.png",
            "resources/button_pea_shooter_3.png",
            "resources/button_pea_shooter_4.png"
        },
        // IT_WEAPON_LASER
        {
            "resources/button_laser_1.png",
            "resources/button_laser_2.png",
            "resources/button_laser_3.png",
            "resources/button_laser_4.png"
        },
        // IT_WEAPON_FLAME_THROWER
        {
            "resources/button_flame_thrower_1.png",
            "resources/button_flame_thrower_2.png",
            "resources/button_flame_thrower_3.png",
            "resources/button_flame_thrower_4.png"
        },
        // IT_WEAPON_GAUSS_GUN
        {
            "resources/button_gauss_gun_1.png",
            "resources/button_gauss_gun_2.png",
            "resources/button_gauss_gun_3.png",
            "resources/button_gauss_gun_4.png"
        },
        // IT_WEAPON_GRENADE_LAUNCHER
        {
            "resources/button_grenade_launcher_1.png",
            "resources/button_grenade_launcher_2.png",
            "resources/button_grenade_launcher_3.png",
            "resources/button_grenade_launcher_4.png"
        },
        // IT_WEAPON_MISSILE_LAUNCHER
        {
            "resources/button_missile_launcher_1.png",
            "resources/button_missile_launcher_2.png",
            "resources/button_missile_launcher_3.png",
            "resources/button_missile_launcher_4.png"
        },
        // IT_WEAPON_TRACTOR
        {
            "resources/button_tractor_1.png",
            "resources/button_tractor_2.png",
            "resources/button_tractor_3.png",
            "resources/button_tractor_4.png"
        },
        // IT_ENGINE
        {
            "resources/button_engine_1.png",
            "resources/button_engine_2.png",
            "resources/button_engine_3.png",
            "resources/button_engine_4.png"
        },
        // IT_ARMOR
        {
            "resources/button_armor_1.png",
            "resources/button_armor_2.png",
            "resources/button_armor_3.png",
            "resources/button_armor_4.png"
        },
        // IT_SHIELD
        {
            "resources/button_shield_1.png",
            "resources/button_shield_2.png",
            "resources/button_shield_3.png",
            "resources/button_shield_4.png"
        },
        // IT_POWER_GENERATOR
        {
            "resources/button_power_generator_1.png",
            "resources/button_power_generator_2.png",
            "resources/button_power_generator_3.png",
            "resources/button_power_generator_4.png"
        }
    };

    return GlTexture::Load(s_button_texture_path[item_type][upgrade_level]);
}

void InventoryButton::SetStatus (Status const status)
{
    ASSERT1(status < S_COUNT);
    m_status = status;
    switch (m_status)
    {
        case S_NOT_AFFORDABLE:
            ColorMask() = Color(0.5f, 0.5f, 0.5f, 1.0f);
            m_target_size_parameter = 0.0f;
            break;

        case S_AFFORDABLE:
            ColorMask() = Color(0.75f, 0.75f, 0.75f, 1.0f);
            m_target_size_parameter = 0.0f;
            break;

        case S_OWNED:
            ColorMask() = Color(1.0f, 1.0f, 1.0f, 1.0f);
            m_target_size_parameter = 1.0f;
            break;

        case S_EQUIPPED:
            // the color mask is set in HandleFrame
            m_target_size_parameter = 1.0f;
            break;

        default:
            ASSERT1(false && "Invalid InventoryButton::Status");
            break;
    }

//     SetCurrentSizeParameter(m_current_size_parameter);
}

void InventoryButton::HandleFrame ()
{
    Button::HandleFrame();

    // if the item is equipped, cause the button to flash
    if (m_status == S_EQUIPPED)
    {
        Float brightness = 0.875f + 0.125f * Math::Sin(360.0f * FrameTime());
        ColorMask() = Color(brightness, brightness, brightness, 1.0f);
    }

    if (m_current_size_parameter < m_target_size_parameter)
    {
        Float size_parameter_delta = m_target_size_parameter - m_current_size_parameter;
        SetCurrentSizeParameter(
            m_current_size_parameter +
            Min(size_parameter_delta,
                ms_size_parameter_change_rate * FrameDT()));
    }
    else if (m_current_size_parameter > m_target_size_parameter)
    {
        Float size_parameter_delta = m_target_size_parameter - m_current_size_parameter;
        SetCurrentSizeParameter(
            m_current_size_parameter +
            Max(size_parameter_delta,
                -ms_size_parameter_change_rate * FrameDT()));
    }
}

void InventoryButton::HandleMouseoverOn ()
{
    m_sender_show_price.Signal(m_item_type, m_upgrade_level);
}

void InventoryButton::HandleMouseoverOff ()
{
    m_sender_hide_price.Signal(m_item_type, m_upgrade_level);
}

void InventoryButton::HandleReleased ()
{
    if (m_status == S_AFFORDABLE)
        m_sender_attempt_to_buy_item.Signal(m_item_type, m_upgrade_level);
    else if (m_status == S_OWNED)
        m_sender_equip_item.Signal(m_item_type, m_upgrade_level);
}

void InventoryButton::UpdateRenderBackground ()
{
    // no background for InventoryButton.
    SetRenderBackground(NULL);
}

void InventoryButton::SetCurrentSizeParameter (Float const current_size_parameter)
{
    ASSERT1(current_size_parameter >= 0.0f);
    ASSERT1(current_size_parameter <= 1.0f);
    if (m_current_size_parameter != current_size_parameter)
    {
        m_current_size_parameter = current_size_parameter;

        FloatVector2 equipped_size(Size().StaticCast<Float>());
        FloatVector2 unequipped_size(ms_unequipped_size_ratio * equipped_size);
        FloatVector2 current_size(
            m_current_size_parameter * equipped_size +
            (1.0f - m_current_size_parameter) * unequipped_size);

        SetContentMargins(ScreenCoordMargins((Size() - current_size.StaticCast<ScreenCoord>()) / 2) - FrameMargins());
    }
}

} // end of namespace Dis

