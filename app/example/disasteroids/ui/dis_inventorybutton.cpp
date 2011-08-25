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

namespace Dis {

Float const InventoryButton::ms_unequipped_size_ratio = 0.75f;
Float const InventoryButton::ms_size_parameter_change_rate = 4.0f;

InventoryButton::InventoryButton (ItemType item_type, ContainerWidget *parent, std::string const &name)
    :
    Button(ButtonTexture(item_type), parent, name),
    m_item_type(item_type),
    m_sender_attempt_to_upgrade_item(this),
    m_sender_equip_item(this),
    m_sender_show_price(this),
    m_sender_hide_price(this),
    m_sender_upgrade_indication(this),
    m_sender_upgrade_level_numeral(this)
{
    SetProperties(false, Uint8(-1), false);

    // this is intentionally close to, but above 0.0f
    m_current_size_parameter = 0.01f;
    SetContentMargins(-FrameMargins());
    UpdateRenderBackground();
}

void InventoryButton::SetProperties (bool is_equipped, Uint8 owned_upgrade_level, bool is_affordable)
{
    // parameter validation
    ASSERT1(Uint8(owned_upgrade_level+1) <= UPGRADE_LEVEL_COUNT);
    if (is_equipped) // if item is equipped
        ASSERT1(Uint8(owned_upgrade_level+1) > 0); // item must be owned
    if (Uint8(owned_upgrade_level+1) == 0) // if item is not owned
        ASSERT1(!is_equipped); // item must not be equipped
    if (Uint8(owned_upgrade_level+1) == UPGRADE_LEVEL_COUNT) // if max upgrade level
        ASSERT1(!is_affordable); // item must not be upgradeable
    if (is_affordable) // if item is upgradeable
        ASSERT1(Uint8(owned_upgrade_level+1) != UPGRADE_LEVEL_COUNT); // must not be max level

    bool upgrade_numeral_changed = (m_owned_upgrade_level != owned_upgrade_level);
    std::string upgrade_indication(UpgradeIndication());
    Uint32 upgrade_level_numeral = UpgradeLevelNumeral();
    m_is_equipped = is_equipped;
    m_owned_upgrade_level = owned_upgrade_level;
    m_is_affordable = is_affordable;
    std::string new_upgrade_indication(UpgradeIndication());
    if (new_upgrade_indication != upgrade_indication)
        m_sender_upgrade_indication.Signal(upgrade_indication);
    if (upgrade_numeral_changed)
        m_sender_upgrade_level_numeral.Signal(upgrade_level_numeral);
    UpdateRenderParameters();
}

std::string InventoryButton::UpgradeIndication () const
{
    std::string upgrade_indication;
    if (Uint8(m_owned_upgrade_level+1) == 0) // item is not owned
        upgrade_indication = (m_is_affordable ? "BUY" : "");
    else
    {
        if (Uint8(m_owned_upgrade_level+1) == UPGRADE_LEVEL_COUNT)
            upgrade_indication = "MAX"; // ignore m_is_affordable in this case
        else if (m_is_affordable)
            upgrade_indication = "UPGR";
        else
            upgrade_indication = "";
    }
    return upgrade_indication;
}

void InventoryButton::HandleFrame ()
{
    Button::HandleFrame();

    // if the item is equipped, cause the button to flash
    if (m_is_equipped)
    {
        Float brightness = 0.8f + 0.2f * Math::Sin(360.0f * FrameTime());
        ColorMask() = Color(brightness, brightness, brightness, 1.0f);
    }

    if (m_current_size_parameter < m_target_size_parameter)
    {
        Float size_parameter_delta = m_target_size_parameter - m_current_size_parameter;
        SetCurrentSizeParameter(
            m_current_size_parameter +
            Min(size_parameter_delta, ms_size_parameter_change_rate * FrameDT()));
    }
    else if (m_current_size_parameter > m_target_size_parameter)
    {
        Float size_parameter_delta = m_target_size_parameter - m_current_size_parameter;
        SetCurrentSizeParameter(
            m_current_size_parameter +
            Max(size_parameter_delta, -ms_size_parameter_change_rate * FrameDT()));
    }
}

void InventoryButton::HandleMouseoverOn ()
{
    if (Uint8(m_owned_upgrade_level+1) != UPGRADE_LEVEL_COUNT)
        m_sender_show_price.Signal(m_item_type, Uint8(m_owned_upgrade_level+1));
}

void InventoryButton::HandleMouseoverOff ()
{
    m_sender_hide_price.Signal(m_item_type);
}

void InventoryButton::HandleReleased ()
{
    if (m_is_affordable)
        m_sender_attempt_to_upgrade_item.Signal(m_item_type, m_owned_upgrade_level);
    else if (Uint8(m_owned_upgrade_level+1) > 0) // item is owned
        m_sender_equip_item.Signal(m_item_type, m_owned_upgrade_level);
}

void InventoryButton::UpdateRenderBackground ()
{
    // no background for InventoryButton.
    SetRenderBackground(NULL);
}

void InventoryButton::UpdateRenderParameters ()
{
    ASSERT1(Uint8(m_owned_upgrade_level+1) <= UPGRADE_LEVEL_COUNT);
    if (Uint8(m_owned_upgrade_level+1) == 0) // item is not owned
    {
        if (m_is_affordable)
            ColorMask() = Color(0.8f, 0.8f, 0.8f, 1.0f);
        else
            ColorMask() = Color(0.5f, 0.5f, 0.5f, 1.0f);
        m_target_size_parameter = 0.0f;
    }
    else
    {
        if (m_is_equipped)
            { } // do nothing, as the color mask is set in HandleFrame
        else
            ColorMask() = Color(1.0f, 1.0f, 1.0f, 1.0f);
        m_target_size_parameter = 1.0f;
    }
}

void InventoryButton::SetCurrentSizeParameter (Float current_size_parameter)
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

Resource<GlTexture> InventoryButton::ButtonTexture (ItemType item_type)
{
    ASSERT1(item_type < IT_COUNT);
    static std::string const s_button_texture_path[IT_COUNT] =
    {
        "resources/icon_tractor.png",           // IT_WEAPON_TRACTOR
        "resources/icon_pea_shooter.png",       // IT_WEAPON_PEA_SHOOTER
        "resources/icon_laser.png",             // IT_WEAPON_LASER
        "resources/icon_flame_thrower.png",     // IT_WEAPON_FLAME_THROWER
        "resources/icon_gauss_gun.png",         // IT_WEAPON_GAUSS_GUN
        "resources/icon_grenade_launcher.png",  // IT_WEAPON_GRENADE_LAUNCHER
        "resources/icon_missile_launcher.png",  // IT_WEAPON_MISSILE_LAUNCHER
        "resources/icon_engine.png",            // IT_ENGINE
        "resources/icon_armor.png",             // IT_ARMOR
        "resources/icon_shield.png",            // IT_SHIELD
        "resources/icon_power_generator.png",   // IT_POWER_GENERATOR
    };
    return GlTexture::Load(s_button_texture_path[item_type]);
}

} // end of namespace Dis

