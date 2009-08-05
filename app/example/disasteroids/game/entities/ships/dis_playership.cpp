// ///////////////////////////////////////////////////////////////////////////
// dis_playership.cpp by Victor Dods, created 2006/01/07
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_playership.hpp"

#include "dis_armor.hpp"
#include "dis_effect.hpp"
#include "dis_enemyship.hpp"
#include "dis_engine.hpp"
#include "dis_powergenerator.hpp"
#include "dis_powerup.hpp"
#include "dis_shield.hpp"
#include "dis_spawn.hpp"
#include "dis_weapon.hpp"
#include "dis_world.hpp"
#include "xrb_engine2_world.hpp"

using namespace Xrb;

namespace Dis
{

Float const PlayerShip::ms_difficulty_protection_factor[DL_COUNT] = { 3.0f, 2.333f, 1.666f, 1.0f };
Float const PlayerShip::ms_max_stoke = 4.0f;

PlayerShip::PlayerShip (
    Float const max_health,
    EntityType const entity_type)
    :
    Ship(max_health, entity_type),
    SignalHandler(),
    m_sender_score_changed(this),
    m_sender_stoke_changed(this),
    m_sender_lives_remaining_changed(this),
    m_sender_wave_count_changed(this),
    m_sender_armor_status_changed(this),
    m_sender_shield_status_changed(this),
    m_sender_power_status_changed(this),
    m_sender_weapon_status_changed(this),
    m_sender_mineral_inventory_changed(this)
{
    // PlayerShip needs to be more sensitive to damage with respect
    // to damage/healing flashes.  the default is 0.2, but we need
    // something much lower.
    SetFullFlashIntensityHealthRatio(0.003f);

    m_score = 0;
    m_stoke = 1.0f;
    m_wave_count = 0;
    m_lives_remaining = 0;

    m_engine_auxiliary_input = 0;
    m_is_using_auxiliary_weapon = false;
    m_main_weapon = NULL;
    m_auxiliary_weapon = NULL;
    m_engine = NULL;
    m_armor = NULL;
    m_shield = NULL;
    m_power_generator = NULL;

    m_armor_status = 1.0f;
    m_shield_status = 0.0f;
    m_power_status = 1.0f;
    m_weapon_status = 1.0f;

    for (Uint32 i = 0; i < IT_COUNT; ++i)
        for (Uint32 j = 0; j < UPGRADE_LEVEL_COUNT; ++j)
            m_item_inventory[i][j] = NULL;

    for (Uint32 i = 0; i < MINERAL_COUNT; ++i)
        m_mineral_inventory[i] = 0.0f;
}

PlayerShip::~PlayerShip ()
{
    SetMainWeapon(NULL);
    SetEngine(NULL);
    SetArmor(NULL);
    SetPowerGenerator(NULL);

    for (Uint32 i = 0; i < IT_COUNT; ++i)
        for (Uint32 j = 0; j < UPGRADE_LEVEL_COUNT; ++j)
            Delete(m_item_inventory[i][j]);

    if (m_laser_beam.IsValid() && !m_laser_beam->IsInWorld())
        delete m_laser_beam->GetOwnerObject();

    if (m_tractor_beam.IsValid() && !m_tractor_beam->IsInWorld())
        delete m_tractor_beam->GetOwnerObject();

    if (m_shield_effect.IsValid() && !m_shield_effect->IsInWorld())
        delete m_shield_effect->GetOwnerObject();
}

Float PlayerShip::ArmorStatus () const
{
    return Max(0.0f, CurrentHealth() / GetMaxHealth());
}

Float PlayerShip::GetShieldStatus () const
{
    if (IsDead())
        return 0.0f;

    return (GetShield() != NULL) ? GetShield()->GetIntensity() : 0.0f;
}

Float PlayerShip::GetPowerStatus () const
{
    if (IsDead())
        return 0.0f;

    return (GetPowerGenerator() != NULL) ?
           GetPowerGenerator()->GetStoredPower() / GetPowerGenerator()->GetMaxPower() :
           0.0f;
}

Float PlayerShip::GetWeaponStatus () const
{
    if (IsDead())
        return 0.0f;

    Weapon const *current_weapon = CurrentWeapon();
    // getting the time from the world in this manner
    // is slightly ugly, but its the easiest way to do it.
    return (current_weapon != NULL) ?
           CurrentWeapon()->GetReadinessStatus(GetWorld()->GetMostRecentFrameTime()) :
           0.0f;
}

bool PlayerShip::IsItemEquipped (
    ItemType const item_type,
    Uint8 const upgrade_level) const
{
    ASSERT1(item_type < IT_COUNT);
    ASSERT1(upgrade_level < UPGRADE_LEVEL_COUNT);

    if (item_type >= IT_WEAPON_LOWEST && item_type <= IT_WEAPON_HIGHEST)
        return (GetMainWeapon() != NULL &&
                GetMainWeapon()->GetItemType() == item_type &&
                GetMainWeapon()->GetUpgradeLevel() == upgrade_level)
               ||
               (AuxiliaryWeapon() != NULL &&
                AuxiliaryWeapon()->GetItemType() == item_type &&
                AuxiliaryWeapon()->GetUpgradeLevel() == upgrade_level);

    switch (item_type)
    {
        case IT_ENGINE:
            return GetEngine() != NULL &&
                   GetEngine()->GetUpgradeLevel() == upgrade_level;

        case IT_ARMOR:
            return GetArmor() != NULL &&
                   GetArmor()->GetUpgradeLevel() == upgrade_level;

        case IT_SHIELD:
            return GetShield() != NULL &&
                   GetShield()->GetUpgradeLevel() == upgrade_level;

        case IT_POWER_GENERATOR:
            return GetPowerGenerator() != NULL &&
                   GetPowerGenerator()->GetUpgradeLevel() == upgrade_level;

        default:
            ASSERT1(false && "Invalid ItemType");
            return false;
    }
}

bool PlayerShip::IsItemInInventory (
    ItemType const item_type,
    Uint8 const upgrade_level) const
{
    ASSERT1(item_type < IT_COUNT);
    ASSERT1(upgrade_level < UPGRADE_LEVEL_COUNT);

    return m_item_inventory[item_type][upgrade_level] != NULL;
}

bool PlayerShip::IsItemAffordable (
    ItemType const item_type,
    Uint8 const upgrade_level) const
{
    ASSERT1(item_type < IT_COUNT);
    ASSERT1(upgrade_level < UPGRADE_LEVEL_COUNT);

    for (Uint8 mineral_type = 0; mineral_type < MINERAL_COUNT; ++mineral_type)
    {
        if (static_cast<Uint32>(m_mineral_inventory[mineral_type]) <
            Item::GetItemPrice(item_type, upgrade_level, mineral_type))
        {
            return false;
        }
    }

    return true;
}

void PlayerShip::SetIsUsingAuxiliaryWeapon (bool const is_using_auxiliary_weapon)
{
    m_is_using_auxiliary_weapon = is_using_auxiliary_weapon;
    SetWeaponStatus(GetWeaponStatus());
}

void PlayerShip::SetMainWeapon (Weapon *const main_weapon)
{
    // unequip the old one
    if (m_main_weapon != NULL)
        m_main_weapon->Unequip(this);

    // equip the new one
    if (main_weapon != NULL)
        main_weapon->Equip(this);

    m_main_weapon = main_weapon;
    SetWeaponStatus(GetWeaponStatus());
}

void PlayerShip::SetAuxiliaryWeapon (Weapon *const auxiliary_weapon)
{
    // unequip the old one
    if (m_auxiliary_weapon != NULL)
        m_auxiliary_weapon->Unequip(this);

    // equip the new one
    if (auxiliary_weapon != NULL)
        auxiliary_weapon->Equip(this);

    m_auxiliary_weapon = auxiliary_weapon;
    SetWeaponStatus(GetWeaponStatus());
}

void PlayerShip::SetEngine (Engine *const engine)
{
    // unequip the old one
    if (m_engine != NULL)
        m_engine->Unequip(this);

    // equip the new one
    if (engine != NULL)
        engine->Equip(this);

    m_engine = engine;
}

void PlayerShip::SetArmor (Armor *const armor)
{
    // unequip the old one
    if (m_armor != NULL)
        m_armor->Unequip(this);

    // equip the new one
    if (armor != NULL)
        armor->Equip(this);

    m_armor = armor;
    // checking the owner entity pointer is necessary because it will be
    // NULL when deleting (and this function is called in ~PlayerShip)
    if (GetOwnerObject() != NULL)
        SetFirstMoment(
            GetShipBaselineFirstMoment() +
            ((armor != NULL) ? armor->GetFirstMoment() : 0.0f));
    SetDamageDissipationRate(
        ((armor != NULL) ? armor->DamageDissipationRate() : 0.0f));

    // TODO: add the strength/immunity/weakness that the armor provides
}

void PlayerShip::SetShield (Shield *shield)
{
    // unequip the old one
    if (m_shield != NULL)
        m_shield->Unequip(this);

    // equip the new one
    if (shield != NULL)
        shield->Equip(this);

    m_shield = shield;

    // TODO: add the strength/immunity/weakness that the shield provides

    SetShieldStatus(GetShieldStatus());
}

void PlayerShip::SetPowerGenerator (PowerGenerator *const power_generator)
{
    // unequip the old one
    if (m_power_generator != NULL)
        m_power_generator->Unequip(this);

    // equip the new one
    if (power_generator != NULL)
        power_generator->Equip(this);

    m_power_generator = power_generator;
    SetPowerStatus(GetPowerStatus());
}

void PlayerShip::IncrementWaveCount ()
{
    ++m_wave_count;
    m_sender_wave_count_changed.Signal(m_wave_count);
}

void PlayerShip::IncrementScore (Uint32 const score_delta)
{
    if (score_delta != 0)
    {
        m_score += score_delta;
        m_sender_score_changed.Signal(m_score);
    }
}

void PlayerShip::IncrementLivesRemaining (Sint32 const lives_remaining_delta)
{
    if (lives_remaining_delta != 0)
    {
        m_lives_remaining += lives_remaining_delta;
        m_sender_lives_remaining_changed.Signal(m_lives_remaining);
    }
}

void PlayerShip::CreditEnemyKill (EntityType const enemy_ship_type, Uint8 const enemy_level)
{
    static Uint32 const s_baseline_score[ET_ENEMY_SHIP_COUNT][EnemyShip::ENEMY_LEVEL_COUNT] =
    {
        {  100,  150,  225,  325 }, // Interloper
        {  200,  400,  700, 1100 }, // Shade
        {  300,  500,  900, 1400 }, // Revulsion
        { 1000, 1500, 2200, 3000 }  // Devourment
    };

    ASSERT1(enemy_ship_type >= ET_ENEMY_SHIP_LOWEST);
    ASSERT1(enemy_ship_type <= ET_ENEMY_SHIP_HIGHEST);
    ASSERT1(enemy_level < EnemyShip::ENEMY_LEVEL_COUNT);

    Uint32 enemy_ship_index = enemy_ship_type - ET_ENEMY_SHIP_LOWEST;
    IncrementScore(static_cast<Uint32>(m_stoke * s_baseline_score[enemy_ship_index][enemy_level]));

    static Float const s_stoke_factor = Math::Pow(2.0f, 0.5f);
    SetStoke(m_stoke * s_stoke_factor);
}

void PlayerShip::GiveLotsOfMinerals ()
{
    for (Uint8 mineral_type = 0; mineral_type < MINERAL_COUNT; ++mineral_type)
        ChangeMineralInventory(mineral_type, 6666.0f);
}

bool PlayerShip::AddItem (Item *item)
{
    ASSERT1(item != NULL);
    ASSERT1(item->GetItemType() < IT_COUNT);
    ASSERT1(item->GetUpgradeLevel() < UPGRADE_LEVEL_COUNT);

    if (m_item_inventory[item->GetItemType()][item->GetUpgradeLevel()] != NULL)
        return false;

    m_item_inventory[item->GetItemType()][item->GetUpgradeLevel()] = item;

    if (item->GetItemType() >= IT_WEAPON_LOWEST &&
        item->GetItemType() <= IT_WEAPON_HIGHEST)
    {
        Weapon *weapon = DStaticCast<Weapon *>(item);

        if (item->GetItemType() == IT_WEAPON_TRACTOR)
        {
            if (AuxiliaryWeapon() == NULL ||
                weapon->GetUpgradeLevel() > AuxiliaryWeapon()->GetUpgradeLevel())
            {
                SetAuxiliaryWeapon(weapon);
            }
        }
        else
        {
            if (GetMainWeapon() == NULL ||
                weapon->GetUpgradeLevel() > GetMainWeapon()->GetUpgradeLevel())
            {
                SetMainWeapon(weapon);
            }
        }

        return true;
    }

    switch (item->GetItemType())
    {
        case IT_ENGINE:
        {
            Engine *engine = DStaticCast<Engine *>(item);

            if (GetEngine() == NULL ||
                engine->GetUpgradeLevel() > GetEngine()->GetUpgradeLevel())
            {
                SetEngine(engine);
            }

            return true;
        }

        case IT_ARMOR:
        {
            Armor *armor = DStaticCast<Armor *>(item);

            if (GetArmor() == NULL ||
                armor->GetUpgradeLevel() > GetArmor()->GetUpgradeLevel())
            {
                SetArmor(armor);
            }

            return true;
        }

        case IT_SHIELD:
        {
            Shield *shield = DStaticCast<Shield *>(item);

            if (GetShield() == NULL ||
                shield->GetUpgradeLevel() > GetShield()->GetUpgradeLevel())
            {
                SetShield(shield);
            }

            return true;
        }

        case IT_POWER_GENERATOR:
        {
            PowerGenerator *power_generator = DStaticCast<PowerGenerator *>(item);

            if (GetPowerGenerator() == NULL ||
                power_generator->GetUpgradeLevel() > GetPowerGenerator()->GetUpgradeLevel())
            {
                SetPowerGenerator(power_generator);
            }

            return true;
        }

        default:
            ASSERT1(false && "Invalid ItemType");
            break;
    }

    return false;
}

void PlayerShip::EquipItem (ItemType item_type, Uint8 const upgrade_level)
{
    ASSERT1(item_type < IT_COUNT);
    ASSERT1(upgrade_level < UPGRADE_LEVEL_COUNT);
    ASSERT1(m_item_inventory[item_type][upgrade_level] != NULL);

    if (item_type >= IT_WEAPON_LOWEST &&
        item_type <= IT_WEAPON_HIGHEST)
    {
        if (item_type == IT_WEAPON_TRACTOR)
            SetAuxiliaryWeapon(DStaticCast<Weapon *>(m_item_inventory[item_type][upgrade_level]));
        else
            SetMainWeapon(DStaticCast<Weapon *>(m_item_inventory[item_type][upgrade_level]));
    }
    else
    {
        switch (item_type)
        {
            case IT_ENGINE:
                SetEngine(DStaticCast<Engine *>(m_item_inventory[item_type][upgrade_level]));
                break;

            case IT_ARMOR:
                SetArmor(DStaticCast<Armor *>(m_item_inventory[item_type][upgrade_level]));
                break;

            case IT_SHIELD:
                SetShield(DStaticCast<Shield *>(m_item_inventory[item_type][upgrade_level]));
                break;

            case IT_POWER_GENERATOR:
                SetPowerGenerator(DStaticCast<PowerGenerator *>(m_item_inventory[item_type][upgrade_level]));
                break;

            default:
                ASSERT1(false && "Invalid ItemType");
                break;
        }
    }
}

void PlayerShip::Think (Float const time, Float const frame_dt)
{
    // can't think if we're dead.
    if (IsDead())
        return;

    bool is_disabled = IsDisabled();
    Ship::Think(time, frame_dt);
    if (is_disabled)
    {
        // reset the stoke O meter (getting disabled bums me out, man)
        SetStoke(1.0f);

        // drain the shields (if they exist)
        if (m_shield != NULL)
            m_shield->Drain();
        // drain the power generator
        ASSERT1(m_power_generator != NULL);
        m_power_generator->Drain();

        // remove all the effects
        if (m_laser_beam.IsValid() && m_laser_beam->IsInWorld())
            m_laser_beam->ScheduleForRemovalFromWorld(0.0f);
        if (m_tractor_beam.IsValid() && m_tractor_beam->IsInWorld())
            m_tractor_beam->ScheduleForRemovalFromWorld(0.0f);
        if (m_shield_effect.IsValid() && m_shield_effect->IsInWorld())
            m_shield_effect->ScheduleForRemovalFromWorld(0.0f);
    }
    else
    {
        AimShipAtReticleCoordinates(frame_dt);

        // update the stoke O meter (exponential decay, with a lower limit of 1.0)
        static Float const s_stoke_halflife = 2.0f;
        SetStoke(Max(1.0f, m_stoke * Math::Pow(0.5f, frame_dt / s_stoke_halflife)));

        // figure out which weapon to use.
        Weapon *current_weapon = CurrentWeapon();

        // figure out what to do with the LaserBeam/TractorBeam
        if (current_weapon != NULL)
        {
            // special treatment for Lasers (because the LaserBeam is effectively
            // attached to the ship's main weapon muzzle.
            if (current_weapon->GetItemType() == IT_WEAPON_LASER)
            {
                // ensure the laser beam is allocated (lazy allocation)
                if (!m_laser_beam.IsValid())
                    m_laser_beam = SpawnLaserBeam(GetWorld(), GetObjectLayer())->GetReference();
                // if the laser beam is already allocated but not in the world, re-add it.
                else if (!m_laser_beam->IsInWorld())
                    m_laser_beam->AddBackIntoWorld();
                // set the laser beam effect in the Laser weapon
                DStaticCast<Laser *>(current_weapon)->SetLaserBeam(*m_laser_beam);
            }
            // if there is no Laser equipped and the laser beam is allocated
            // AND in the world, remove it from the world.
            else if (m_laser_beam.IsValid() && m_laser_beam->IsInWorld())
                m_laser_beam->ScheduleForRemovalFromWorld(0.0f);

            // special treatment for Tractors (because the TractorBeam is effectively
            // attached to the ship's main weapon muzzle.
            if (current_weapon->GetItemType() == IT_WEAPON_TRACTOR)
            {
                // ensure the tractor beam is allocated (lazy allocation)
                if (!m_tractor_beam.IsValid())
                    m_tractor_beam = SpawnTractorBeam(GetWorld(), GetObjectLayer())->GetReference();
                // if the tractor beam is already allocated but not in the world, re-add it.
                else if (!m_tractor_beam->IsInWorld())
                    m_tractor_beam->AddBackIntoWorld();
                // set the tractor beam effect in the Tractor weapon
                DStaticCast<Tractor *>(current_weapon)->SetTractorBeam(*m_tractor_beam);
            }
            // if there is no Tractor equipped and the tractor beam is allocated
            // AND in the world, remove it from the world.
            else if (m_tractor_beam.IsValid() && m_tractor_beam->IsInWorld())
                m_tractor_beam->ScheduleForRemovalFromWorld(0.0f);
        }
        else
        {
            if (m_laser_beam.IsValid() && m_laser_beam->IsInWorld())
                m_laser_beam->ScheduleForRemovalFromWorld(0.0f);

            if (m_tractor_beam.IsValid() && m_tractor_beam->IsInWorld())
                m_tractor_beam->ScheduleForRemovalFromWorld(0.0f);
        }

        // set the devices' inputs

        if (current_weapon != NULL)
            current_weapon->SetInputs(
                GetNormalizedWeaponPrimaryInput(),
                GetNormalizedWeaponSecondaryInput(),
                GetMuzzleLocation(GetMainWeapon()),
                GetMuzzleDirection(GetMainWeapon()),
                GetReticleCoordinates());

        if (m_engine != NULL)
            m_engine->SetInputs(
                GetNormalizedEngineRightLeftInput(),
                GetNormalizedEngineUpDownInput(),
                GetNormalizedEngineAuxiliaryInput());

        // apply power to the devices

        m_devices_to_power[DTP_WEAPON] = current_weapon;
        m_devices_to_power[DTP_ENGINE] = m_engine;
        m_devices_to_power[DTP_SHIELD] = m_shield;
        ASSERT1(m_power_generator != NULL);
        m_power_generator->PowerDevices(
            m_devices_to_power,
            m_power_allocator,
            DTP_COUNT,
            time,
            frame_dt);

        // here is where the power generator recharges (after supplying power)
        m_power_generator->Think(time, frame_dt);

        // update the shield effect position and intensity
        if (m_shield != NULL && !IsDead())
        {
            // ensure the shield effect is allocated (lazy allocation)
            if (!m_shield_effect.IsValid())
                m_shield_effect = SpawnShieldEffect(GetWorld(), GetObjectLayer())->GetReference();
            // if the shield effect is already allocated but not in the world, re-add it.
            else if (!m_shield_effect->IsInWorld())
                m_shield_effect->AddBackIntoWorld();
            // update the shield's intensity and position
            m_shield_effect->SetIntensity(m_shield->GetIntensity());
            // TODO: real entity attachment -- temp hack
            m_shield_effect->SnapToShip(GetTranslation() + frame_dt * GetVelocity(), GetScaleFactor());
        }
        // if there is no shield equipped and the shield effect is
        // allocated AND in the world, remove it from the world.
        else if (m_shield_effect.IsValid() && m_shield_effect->IsInWorld())
            m_shield_effect->ScheduleForRemovalFromWorld(0.0f);
    }

    ResetInputs();

    // update the shield status
    SetShieldStatus(GetShieldStatus());
    // update the power status
    SetPowerStatus(GetPowerStatus());
}

bool PlayerShip::Damage (
    Entity *const damager,
    Entity *const damage_medium,
    Float damage_amount,
    Float *const damage_amount_used,
    FloatVector2 const &damage_location,
    FloatVector2 const &damage_normal,
    Float const damage_force,
    DamageType const damage_type,
    Float const time,
    Float const frame_dt)
{
    Float temp_damage_taken = 0.0f;

    // scale the damage amount by the protection factor
    ASSERT1(ms_difficulty_protection_factor[GetWorld()->GetDifficultyLevel()] > 0.0f);
    damage_amount /= ms_difficulty_protection_factor[GetWorld()->GetDifficultyLevel()];

    // let the shield take the damage first
    Float unblocked_damage = damage_amount;
    if (GetShield() != NULL)
        unblocked_damage = GetShield()->Damage(
            damager,
            damage_medium,
            damage_amount,
            &temp_damage_taken,
            damage_location,
            damage_normal,
            damage_force,
            damage_type,
            time,
            frame_dt);

    if (damage_amount_used != NULL)
        *damage_amount_used = temp_damage_taken;

    bool player_ship_died = false;

    // if there was any unblocked damage, inflict it on the ship
    if (unblocked_damage > 0.0f)
    {
        temp_damage_taken = 0.0f;
        player_ship_died = Mortal::Damage(
            damager,
            damage_medium,
            unblocked_damage,
            &temp_damage_taken,
            damage_location,
            damage_normal,
            damage_force,
            damage_type,
            time,
            frame_dt);

        if (damage_amount_used != NULL)
            *damage_amount_used += temp_damage_taken;
    }

    // if damage_amount_used is provided, scale it back up by the protection factor
    if (damage_amount_used != NULL)
        *damage_amount_used *= ms_difficulty_protection_factor[GetWorld()->GetDifficultyLevel()];

    return player_ship_died;
}

void PlayerShip::Die (
    Entity *const killer,
    Entity *const kill_medium,
    FloatVector2 const &kill_location,
    FloatVector2 const &kill_normal,
    Float const kill_force,
    DamageType const kill_type,
    Float const time,
    Float const frame_dt)
{
    DStaticCast<World *>(GetWorld())->RecordDestroyedPlayerShip(this);

    // reset the stoke O meter (dying bums me out, man)
    SetStoke(1.0f);
    // zero out the shield, armor, power and weapon status
    SetArmorStatus(0.0f);
    SetShieldStatus(0.0f);
    SetPowerStatus(0.0f);
    SetWeaponStatus(0.0f);

    // spawn a really big explosion
    SpawnNoDamageExplosion(
        GetWorld(),
        GetObjectLayer(),
        GetTranslation(),
        GetVelocity(),
        20.0f * GetScaleFactor(),
        2.0f,
        time);

    // the player's ship is not deleted
    ScheduleForRemovalFromWorld(0.0f);
    // remove the laser beam, if it exists
    if (m_laser_beam.IsValid() && m_laser_beam->IsInWorld())
        m_laser_beam->ScheduleForRemovalFromWorld(0.0f);
    // remove the tractor beam, if it exists
    if (m_tractor_beam.IsValid() && m_tractor_beam->IsInWorld())
        m_tractor_beam->ScheduleForRemovalFromWorld(0.0f);
    // remove the shield effect, if it exists
    if (m_shield_effect.IsValid() && m_shield_effect->IsInWorld())
        m_shield_effect->ScheduleForRemovalFromWorld(0.0f);

    // eject the currently equipped inventory as powerups (with the exception
    // of the starting inventory), re-equipping the next-best item.

    EjectPowerup(m_main_weapon, 0.0f);
    EjectPowerup(m_auxiliary_weapon, 60.0f);
    EjectPowerup(m_engine, 120.0f);
    EjectPowerup(m_armor, 180.0f);
    EjectPowerup(m_shield, 240.0f);
    EjectPowerup(m_power_generator, 300.0f);

    // make sure we have at least the bare minimum equipped inventory
    ASSERT1(m_main_weapon != NULL);
    ASSERT1(m_engine != NULL);
    ASSERT1(m_power_generator != NULL);
}

void PlayerShip::SetMainWeaponType (ItemType const main_weapon_type)
{
    ASSERT1(main_weapon_type >= IT_WEAPON_LOWEST && main_weapon_type <= IT_WEAPON_HIGHEST);
    for (Uint32 i = UPGRADE_LEVEL_COUNT-1; i <= UPGRADE_LEVEL_COUNT; --i)
    {
        Weapon *weapon = GetInventoryWeapon(main_weapon_type, i);
        if (weapon != NULL)
        {
            SetMainWeapon(weapon);
            break;
        }
    }
}

void PlayerShip::SetAuxiliaryWeaponType (ItemType const auxiliary_weapon_type)
{
    ASSERT1(auxiliary_weapon_type >= IT_WEAPON_LOWEST && auxiliary_weapon_type <= IT_WEAPON_HIGHEST);
    for (Uint32 i = UPGRADE_LEVEL_COUNT-1; i <= UPGRADE_LEVEL_COUNT; --i)
    {
        Weapon *weapon = GetInventoryWeapon(auxiliary_weapon_type, i);
        if (weapon != NULL)
        {
            SetAuxiliaryWeapon(weapon);
            break;
        }
    }
}

bool PlayerShip::TakePowerup (Powerup *const powerup, Float const time, Float const frame_dt)
{
    ASSERT1(powerup != NULL);

    // check if its a mineral
    if (powerup->GetItemType() >= IT_MINERAL_LOWEST &&
        powerup->GetItemType() <= IT_MINERAL_HIGHEST)
    {
        ASSERT1(powerup->GetItem() == NULL);
        ChangeMineralInventory(powerup->GetItemType() - IT_MINERAL_LOWEST, powerup->GetEffectiveValue());
        return true;
    }
    // check if its a health powerup
    else if (powerup->GetItemType() == IT_POWERUP_HEALTH)
    {
        ASSERT1(powerup->GetItem() == NULL);
        Heal(
            powerup,
            powerup,
            powerup->GetEffectiveValue(),
            (GetFirstMoment()*powerup->GetTranslation() + powerup->GetFirstMoment()*GetTranslation()) /
                (GetFirstMoment() + powerup->GetFirstMoment()),
            (GetTranslation() - powerup->GetTranslation()).GetNormalization(),
            0.0f,
            time,
            frame_dt);
        return true;
    }
    // otherwise try to add the powerup's Item
    else
    {
        ASSERT1(powerup->GetItem() != NULL);
        if (AddItem(powerup->GetItem()))
        {
            powerup->ClearItem();
            return true;
        }
        else
        {
            delete powerup->GetItem();
            powerup->ClearItem();
            return false;
        }
    }
}

bool PlayerShip::BuyItem (ItemType const item_type, Uint8 const upgrade_level)
{
    ASSERT1(item_type < IT_COUNT);
    ASSERT1(upgrade_level < UPGRADE_LEVEL_COUNT);

    // check if we have enough minerals
    for (Uint32 mineral_type = 0; mineral_type < MINERAL_COUNT; ++mineral_type)
    {
        Uint32 item_price = Item::GetItemPrice(item_type, upgrade_level, mineral_type);
        if (static_cast<Uint32>(m_mineral_inventory[mineral_type]) < item_price)
            return false;
    }

    // check if we have the item already
    if (m_item_inventory[item_type][upgrade_level] != NULL)
        return false;

    // if we don't have an item of this type and upgrade level, create
    // it and add it to the inventory
    AddItem(Item::Create(item_type, upgrade_level));

    // subtract the mineral cost from our mineral inventory
    for (Uint32 mineral_type = 0; mineral_type < MINERAL_COUNT; ++mineral_type)
    {
        Uint32 item_price = Item::GetItemPrice(item_type, upgrade_level, mineral_type);
        ChangeMineralInventory(mineral_type, -static_cast<Float>(item_price));
    }

    EquipItem(item_type, upgrade_level);
    return true;
}

void PlayerShip::SetStoke (Float stoke)
{
    if (IsDead())
        stoke = 1.0f;

    if (stoke > ms_max_stoke)
        stoke = ms_max_stoke;

    if (m_stoke != stoke)
    {
        m_stoke = stoke;
        m_sender_stoke_changed.Signal(m_stoke);
    }
}

void PlayerShip::ResetInputs ()
{
    Ship::ResetInputs();
    m_engine_auxiliary_input = 0;
}

void PlayerShip::SetCurrentHealth (Float const current_health)
{
    Mortal::SetCurrentHealth(current_health);

    SetArmorStatus(ArmorStatus());
    SetShieldStatus(GetShieldStatus());
}

Weapon const *PlayerShip::CurrentWeapon () const
{
    if (m_is_using_auxiliary_weapon && m_auxiliary_weapon != NULL)
        return m_auxiliary_weapon;
    else
        return m_main_weapon;
}

Weapon *PlayerShip::CurrentWeapon ()
{
    if (m_is_using_auxiliary_weapon && m_auxiliary_weapon != NULL)
        return m_auxiliary_weapon;
    else
        return m_main_weapon;
}

Weapon *PlayerShip::GetInventoryWeapon (
    ItemType const weapon_type,
    Uint8 const upgrade_level)
{
    ASSERT1(weapon_type < IT_COUNT);
    ASSERT1(weapon_type >= IT_WEAPON_LOWEST);
    ASSERT1(weapon_type <= IT_WEAPON_HIGHEST);
    ASSERT1(upgrade_level < UPGRADE_LEVEL_COUNT);
    ASSERT1(m_item_inventory[weapon_type][upgrade_level] == NULL ||
            dynamic_cast<Weapon *>(m_item_inventory[weapon_type][upgrade_level]) != NULL);
    return static_cast<Weapon *>(m_item_inventory[weapon_type][upgrade_level]);
}

Engine *PlayerShip::GetInventoryEngine (Uint8 const upgrade_level)
{
    ASSERT1(upgrade_level < UPGRADE_LEVEL_COUNT);
    ASSERT1(m_item_inventory[IT_ENGINE][upgrade_level] == NULL ||
            dynamic_cast<Engine *>(m_item_inventory[IT_ENGINE][upgrade_level]) != NULL);
    return static_cast<Engine *>(m_item_inventory[IT_ENGINE][upgrade_level]);
}

Armor *PlayerShip::GetInventoryArmor (Uint8 const upgrade_level)
{
    ASSERT1(upgrade_level < UPGRADE_LEVEL_COUNT);
    ASSERT1(m_item_inventory[IT_ARMOR][upgrade_level] == NULL ||
            dynamic_cast<Armor *>(m_item_inventory[IT_ARMOR][upgrade_level]) != NULL);
    return static_cast<Armor *>(m_item_inventory[IT_ARMOR][upgrade_level]);
}

Shield *PlayerShip::GetInventoryShield (Uint8 const upgrade_level)
{
    ASSERT1(upgrade_level < UPGRADE_LEVEL_COUNT);
    ASSERT1(m_item_inventory[IT_SHIELD][upgrade_level] == NULL ||
            dynamic_cast<Shield *>(m_item_inventory[IT_SHIELD][upgrade_level]) != NULL);
    return static_cast<Shield *>(m_item_inventory[IT_SHIELD][upgrade_level]);
}

PowerGenerator *PlayerShip::GetInventoryPowerGenerator (Uint8 const upgrade_level)
{
    ASSERT1(upgrade_level < UPGRADE_LEVEL_COUNT);
    ASSERT1(m_item_inventory[IT_POWER_GENERATOR][upgrade_level] == NULL ||
            dynamic_cast<PowerGenerator *>(m_item_inventory[IT_POWER_GENERATOR][upgrade_level]) != NULL);
    return static_cast<PowerGenerator *>(m_item_inventory[IT_POWER_GENERATOR][upgrade_level]);
}

bool PlayerShip::IsInStartingInventory (Item *const item)
{
    ASSERT1(item != NULL);

    return item->GetUpgradeLevel() == 0 &&
           (item->GetItemType() == IT_WEAPON_PEA_SHOOTER ||
            item->GetItemType() == IT_ENGINE ||
            item->GetItemType() == IT_ARMOR ||
            item->GetItemType() == IT_POWER_GENERATOR);
}

void PlayerShip::SetArmorStatus (Float const armor_status)
{
    ASSERT1(armor_status >= 0.0f && armor_status <= 1.0f);
    if (m_armor_status != armor_status)
    {
        m_armor_status = armor_status;
        m_sender_armor_status_changed.Signal(m_armor_status);
    }
}

void PlayerShip::SetShieldStatus (Float const shield_status)
{
    ASSERT1(shield_status >= 0.0f && shield_status <= 1.0f);
    if (m_shield_status != shield_status)
    {
        m_shield_status = shield_status;
        m_sender_shield_status_changed.Signal(m_shield_status);
    }
}

void PlayerShip::SetPowerStatus (Float const power_status)
{
    ASSERT1(power_status >= 0.0f && power_status <= 1.0f);
    if (m_power_status != power_status)
    {
        m_power_status = power_status;
        m_sender_power_status_changed.Signal(m_power_status);
    }
}

void PlayerShip::SetWeaponStatus (Float const weapon_status)
{
    ASSERT1(weapon_status >= 0.0f && weapon_status <= 1.0f);
    if (m_weapon_status != weapon_status)
    {
        m_weapon_status = weapon_status;
        m_sender_weapon_status_changed.Signal(m_weapon_status);
    }
}

void PlayerShip::EjectPowerup (Item *const ejectee, Float const ejection_angle)
{
    if (ejectee == NULL || IsInStartingInventory(ejectee))
        return;
/*
    static Float const s_powerup_scale_factor = 5.0f;
    static Float const s_powerup_ejection_speed = 50.0f;

    FloatVector2 ejection_normal(Math::UnitVector(Angle() + ejection_angle));
    SpawnPowerup(
        GetWorld(),
        GetObjectLayer(),
        s_powerup_scale_factor * ejection_normal + GetTranslation(),
        s_powerup_scale_factor,
        s_powerup_scale_factor * s_powerup_scale_factor,
        s_powerup_ejection_speed * ejection_normal,
        "resources/powerup.png",
        ejectee);
*/
    // remove the item from the inventory
    ItemType item_type = ejectee->GetItemType();
    ASSERT1(item_type < IT_COUNT);
    ASSERT1(ejectee->GetUpgradeLevel() < UPGRADE_LEVEL_COUNT);
    m_item_inventory[item_type][ejectee->GetUpgradeLevel()] = NULL;

    // figure out what item to equip it its place
    Item *item_to_equip = NULL;

    for (Uint8 i = UPGRADE_LEVEL_COUNT-1; i < UPGRADE_LEVEL_COUNT; --i)
    {
        if (m_item_inventory[item_type][i] != NULL)
        {
            item_to_equip = m_item_inventory[item_type][i];
            break;
        }
    }

    // if we didn't find an item to equip (and the item type is
    // a main weapon type), figure out what main weapon to equip.
    if (item_to_equip == NULL &&
        item_type >= IT_WEAPON_LOWEST &&
        item_type <= IT_WEAPON_HIGHEST &&
        item_type != IT_WEAPON_TRACTOR)
    {
        // the priority goes GaussGun, FlameThrower, MissileLauncher,
        // Laser, GrenadeLauncher, PeaShooter, EMPCore
        static ItemType const s_weapon_priority[] =
        {
            IT_WEAPON_GAUSS_GUN,
            IT_WEAPON_FLAME_THROWER,
            IT_WEAPON_MISSILE_LAUNCHER,
            IT_WEAPON_LASER,
            IT_WEAPON_GRENADE_LAUNCHER,
            IT_WEAPON_PEA_SHOOTER,
            IT_WEAPON_EMP_CORE
        };
        static Uint32 const s_weapon_priority_count =
            sizeof(s_weapon_priority) / sizeof(ItemType);

        for (Uint32 priority = 0; priority < s_weapon_priority_count; ++priority)
        {
            for (Uint8 i = UPGRADE_LEVEL_COUNT-1; i < UPGRADE_LEVEL_COUNT; --i)
            {
                ASSERT1(s_weapon_priority[priority] < IT_COUNT);
                if (m_item_inventory[s_weapon_priority[priority]][i] != NULL)
                {
                    item_to_equip = m_item_inventory[s_weapon_priority[priority]][i];
                    break;
                }
            }
            if (item_to_equip != NULL)
                break;
        }
    }

    // set the equipped item
    if (item_type == IT_WEAPON_TRACTOR)
    {
        SetAuxiliaryWeapon(DStaticCast<Weapon *>(item_to_equip));
    }
    else if (item_type >= IT_WEAPON_LOWEST &&
             item_type <= IT_WEAPON_HIGHEST)
    {
        SetMainWeapon(DStaticCast<Weapon *>(item_to_equip));
    }
    else if (item_type == IT_ENGINE)
    {
        SetEngine(DStaticCast<Engine *>(item_to_equip));
    }
    else if (item_type == IT_ARMOR)
    {
        SetArmor(DStaticCast<Armor *>(item_to_equip));
    }
    else if (item_type == IT_SHIELD)
    {
        SetShield(DStaticCast<Shield *>(item_to_equip));
    }
    else if (item_type == IT_POWER_GENERATOR)
    {
        SetPowerGenerator(DStaticCast<PowerGenerator *>(item_to_equip));
    }
    else
    {
        ejectee->Unequip(this);
    }
}

void PlayerShip::ChangeMineralInventory (
    Uint8 const mineral_type,
    Float const mineral_delta)
{
    ASSERT1(mineral_type < MINERAL_COUNT);

    if (mineral_delta == 0.0f)
        return;

    ASSERT1(mineral_delta >= 0.0f ||
            mineral_delta + m_mineral_inventory[mineral_type] >= 0.0f);
    m_mineral_inventory[mineral_type] += mineral_delta;
    m_sender_mineral_inventory_changed.Signal(mineral_type, m_mineral_inventory[mineral_type]);
}

} // end of namespace Dis

