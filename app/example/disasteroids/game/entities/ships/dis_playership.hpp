// ///////////////////////////////////////////////////////////////////////////
// dis_playership.hpp by Victor Dods, created 2006/01/07
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_PLAYERSHIP_HPP_)
#define _DIS_PLAYERSHIP_HPP_

#include "dis_enums.hpp"
#include "dis_ship.hpp"
#include "xrb_signalhandler.hpp"

using namespace Xrb;

namespace Dis
{

class Armor;
class Engine;
class LaserBeam;
class PoweredDevice;
class PowerGenerator;
class Shield;
class ShieldEffect;
class TractorBeam;
class Weapon;

class PlayerShip : public Ship, public SignalHandler
{
public:

    static Float const ms_difficulty_protection_factor[DL_COUNT];
    static Float const ms_max_stoke;

    PlayerShip (Float max_health, EntityType entity_type);
    virtual ~PlayerShip ();

    // Entity interface method
    virtual bool IsPlayerShip () const { return true; }

    inline Uint32 GetScore () const { return m_score; }
    inline Float Stoke () const { return m_stoke; }
    inline Uint32 WaveCount () const { return m_wave_count; }
    inline Uint32 LivesRemaining () const { return m_lives_remaining; }

    inline Weapon const *MainWeapon () const { return m_main_weapon; }
    inline Weapon const *AuxiliaryWeapon () const { return m_auxiliary_weapon; }
    inline Engine const *GetEngine () const { return m_engine; }
    inline Armor const *GetArmor () const { return m_armor; }
    inline Shield const *GetShield () const { return m_shield; }
    inline PowerGenerator const *GetPowerGenerator () const { return m_power_generator; }

    Float ArmorStatus () const;
    Float ShieldStatus () const;
    Float PowerStatus () const;
    Float WeaponStatus () const;
    inline Float MineralInventory (Uint8 mineral_type) const
    {
        ASSERT1(mineral_type < MINERAL_COUNT);
        return m_mineral_inventory[mineral_type];
    }
    bool IsItemEquipped (ItemType item_type, Uint8 upgrade_level) const;
    bool IsItemInInventory (ItemType item_type, Uint8 upgrade_level) const;
    bool IsItemAffordable (ItemType item_type, Uint8 upgrade_level) const;

    void SetMainWeaponType (ItemType main_weapon_type);
    void SetAuxiliaryWeaponType (ItemType auxiliary_weapon_type);
    inline void SetEngineAuxiliaryInput (Uint8 const engine_auxiliary_input)
    {
        m_engine_auxiliary_input = engine_auxiliary_input;
    }
    void SetIsUsingAuxiliaryWeapon (bool is_using_auxiliary_weapon);
    void SetMainWeapon (Weapon *main_weapon);
    void SetAuxiliaryWeapon (Weapon *auxiliary_weapon);
    void SetEngine (Engine *engine);
    void SetArmor (Armor *armor);
    void SetShield (Shield *shield);
    void SetPowerGenerator (PowerGenerator *power_generator);

    // SignalSender accessors
    inline SignalSender1<Uint32> const *SenderScoreChanged () { return &m_sender_score_changed; }
    inline SignalSender1<Float> const *SenderStokeChanged () { return &m_sender_stoke_changed; }
    inline SignalSender1<Uint32> const *SenderLivesRemainingChanged () { return &m_sender_lives_remaining_changed; }
    inline SignalSender1<Uint32> const *SenderWaveCountChanged () { return &m_sender_wave_count_changed; }
    inline SignalSender1<Float> const *SenderArmorStatusChanged () { return &m_sender_armor_status_changed; }
    inline SignalSender1<Float> const *SenderShieldStatusChanged () { return &m_sender_shield_status_changed; }
    inline SignalSender1<Float> const *SenderPowerStatusChanged () { return &m_sender_power_status_changed; }
    inline SignalSender1<Float> const *SenderWeaponStatusChanged () { return &m_sender_weapon_status_changed; }
    inline SignalSender2<Uint8, Float> const *SenderMineralInventoryChanged () { return &m_sender_mineral_inventory_changed; }

    void IncrementWaveCount ();
    void IncrementScore (Uint32 score_delta);
    void IncrementLivesRemaining (Sint32 lives_remaining_delta);
    void CreditEnemyKill (EntityType enemy_ship_type, Uint8 enemy_level);
    void GiveLotsOfMinerals ();

    // attempts to add the item to the ship's inventory.  returns true if
    // the ship took the item (and responsibility for deleting the item),
    // false if the ship did not take the item (and did NOT take
    // responsibility for deleting the item).
    bool AddItem (Item *item);
    void EquipItem (ItemType item_type, Uint8 const upgrade_level);

    virtual void Think (Float time, Float frame_dt);
    virtual bool Damage (
        Entity *damager,
        Entity *damage_medium,
        Float damage_amount,
        Float *damage_amount_used,
        FloatVector2 const &damage_location,
        FloatVector2 const &damage_normal,
        Float damage_force,
        DamageType damage_type,
        Float time,
        Float frame_dt);
    virtual void Die (
        Entity *killer,
        Entity *kill_medium,
        FloatVector2 const &kill_location,
        FloatVector2 const &kill_normal,
        Float kill_force,
        DamageType kill_type,
        Float time,
        Float frame_dt);

    bool BuyItem (ItemType item_type, Uint8 upgrade_level);

    // ///////////////////////////////////////////////////////////////////////
    // Ship public interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual bool TakePowerup (Powerup *powerup, Float time, Float frame_dt);

protected:

    inline Float NormalizedEngineAuxiliaryInput () const
    {
        return static_cast<Float>(m_engine_auxiliary_input) /
               static_cast<Float>(UINT8_UPPER_BOUND);
    }
    inline bool IsUsingAuxiliaryWeapon () const
    {
        return m_is_using_auxiliary_weapon;
    }
    inline Weapon *MainWeapon ()
    {
        return m_main_weapon;
    }
    inline Weapon *AuxiliaryWeapon ()
    {
        return m_auxiliary_weapon;
    }
    inline Engine *GetEngine ()
    {
        return m_engine;
    }
    inline Armor *GetArmor ()
    {
        return m_armor;
    }
    inline Shield *GetShield ()
    {
        return m_shield;
    }
    inline PowerGenerator *GetPowerGenerator ()
    {
        return m_power_generator;
    }

    void SetStoke (Float stoke);

    virtual void ResetInputs ();
    virtual void SetCurrentHealth (Float current_health);

private:

    Weapon const *CurrentWeapon () const;
    Weapon *CurrentWeapon ();

    Weapon *InventoryWeapon (ItemType weapon_type, Uint8 upgrade_level);
    Engine *InventoryEngine (Uint8 upgrade_level);
    Armor *InventoryArmor (Uint8 upgrade_level);
    Shield *InventoryShield (Uint8 upgrade_level);
    PowerGenerator *InventoryPowerGenerator (Uint8 upgrade_level);

    bool IsInStartingInventory (Item *item);

    void SetArmorStatus (Float armor_status);
    void SetShieldStatus (Float shield_status);
    void SetPowerStatus (Float power_status);
    void SetWeaponStatus (Float weapon_status);

    void EjectPowerup (Item *ejectee, Float ejection_angle);
    void ChangeMineralInventory (Uint8 mineral_type, Float mineral_delta);

    enum DeviceToPower
    {
        DTP_ENGINE = 0,
        DTP_WEAPON,
        DTP_SHIELD,

        DTP_COUNT
    }; // end of enum PlayerShip::DeviceToPower

    Uint32 m_score;
    Float m_stoke;
    Uint32 m_wave_count;
    Uint32 m_lives_remaining;

    Uint8 m_engine_auxiliary_input;
    bool m_is_using_auxiliary_weapon;

    PoweredDevice *m_devices_to_power[DTP_COUNT];
    Float m_power_allocator[DTP_COUNT];

    Weapon *m_main_weapon;
    Weapon *m_auxiliary_weapon;
    Engine *m_engine;
    Armor *m_armor;
    Shield *m_shield;
    PowerGenerator *m_power_generator;

    Float m_armor_status;
    Float m_shield_status;
    Float m_power_status;
    Float m_weapon_status;

    Item *m_item_inventory[IT_COUNT][UPGRADE_LEVEL_COUNT];
    Float m_mineral_inventory[MINERAL_COUNT];

    EntityReference<LaserBeam> m_laser_beam;
    EntityReference<TractorBeam> m_tractor_beam;
    EntityReference<ShieldEffect> m_shield_effect;

    SignalSender1<Uint32> m_sender_score_changed;
    SignalSender1<Float> m_sender_stoke_changed;
    SignalSender1<Uint32> m_sender_lives_remaining_changed;
    SignalSender1<Uint32> m_sender_wave_count_changed;
    SignalSender1<Float> m_sender_armor_status_changed;
    SignalSender1<Float> m_sender_shield_status_changed;
    SignalSender1<Float> m_sender_power_status_changed;
    SignalSender1<Float> m_sender_weapon_status_changed;
    SignalSender2<Uint8, Float> m_sender_mineral_inventory_changed;
}; // end of class PlayerShip

} // end of namespace Dis

#endif // !defined(_DIS_PLAYERSHIP_HPP_)

