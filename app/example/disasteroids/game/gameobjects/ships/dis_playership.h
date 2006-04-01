// ///////////////////////////////////////////////////////////////////////////
// dis_playership.h by Victor Dods, created 2006/01/07
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_PLAYERSHIP_H_)
#define _DIS_PLAYERSHIP_H_

#include "dis_enums.h"
#include "dis_ship.h"
#include "xrb_signalhandler.h"

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

    PlayerShip (Float max_health, Type type);
    virtual ~PlayerShip ();

    // GameObject interface method
    virtual bool GetIsPlayerShip () const { return true; }
    
    inline Uint32 GetScore () const { return m_score; }
    inline Float GetStoke () const { return m_stoke; }
    inline Float GetTimeAlive () const { return m_time_alive; }
    inline Uint32 GetLivesRemaining () const { return m_lives_remaining; }
    
    inline Weapon const *GetMainWeapon () const { return m_main_weapon; }
    inline Weapon const *GetAuxiliaryWeapon () const { return m_auxiliary_weapon; }
    inline Engine const *GetEngine () const { return m_engine; }
    inline Armor const *GetArmor () const { return m_armor; }
    inline Shield const *GetShield () const { return m_shield; }
    inline PowerGenerator const *GetPowerGenerator () const { return m_power_generator; }

    Float GetArmorStatus () const;
    Float GetShieldStatus () const;
    Float GetPowerStatus () const;
    Float GetWeaponStatus () const;
    inline Float GetMineralInventory (Uint8 mineral_type) const
    {
        ASSERT1(mineral_type < MINERAL_COUNT)
        return m_mineral_inventory[mineral_type];
    }
    bool GetIsItemEquipped (ItemType item_type, Uint8 upgrade_level) const;
    bool GetIsItemInInventory (ItemType item_type, Uint8 upgrade_level) const;
    bool GetIsItemAffordable (ItemType item_type, Uint8 upgrade_level) const;

    void SetMainWeaponNumber (Uint32 weapon_number);
    void SetAuxiliaryWeaponNumber (Uint32 weapon_number);    
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
    inline SignalSender1<Float> const *SenderTimeAliveChanged () { return &m_sender_time_alive_changed; }
    inline SignalSender1<Float> const *SenderArmorStatusChanged () { return &m_sender_armor_status_changed; }
    inline SignalSender1<Float> const *SenderShieldStatusChanged () { return &m_sender_shield_status_changed; }
    inline SignalSender1<Float> const *SenderPowerStatusChanged () { return &m_sender_power_status_changed; }
    inline SignalSender1<Float> const *SenderWeaponStatusChanged () { return &m_sender_weapon_status_changed; }
    inline SignalSender2<Uint8, Float> const *SenderMineralInventoryChanged () { return &m_sender_mineral_inventory_changed; }

    void IncrementTimeAlive (Float time_alive_delta);
    void IncrementScore (Uint32 score_delta);
    void IncrementLivesRemaining (Sint32 lives_remaining_delta);
    void CreditEnemyKill (Type enemy_ship_type, Uint8 enemy_level);
    void GiveLotsOfMinerals ();
        
    // attempts to add the item to the ship's inventory.  returns true if
    // the ship took the item (and responsibility for deleting the item),
    // false if the ship did not take the item (and did NOT take
    // responsibility for deleting the item).
    bool AddItem (Item *item);
    void EquipItem (ItemType item_type, Uint8 const upgrade_level);

    virtual void Think (Float time, Float frame_dt);
    virtual bool Damage (
        GameObject *damager,
        GameObject *damage_medium,
        Float damage_amount,
        Float *damage_amount_used,
        FloatVector2 const &damage_location,
        FloatVector2 const &damage_normal,
        Float damage_force,
        DamageType damage_type,
        Float time,
        Float frame_dt);
    virtual void Die (
        GameObject *killer,
        GameObject *kill_medium,
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

    virtual bool TakePowerup (Powerup *powerup);

protected:

    inline Float GetNormalizedEngineAuxiliaryInput () const
    {
        return static_cast<Float>(m_engine_auxiliary_input) /
               static_cast<Float>(UINT8_UPPER_BOUND);
    }
    inline bool GetIsUsingAuxiliaryWeapon () const
    {
        return m_is_using_auxiliary_weapon;
    }
    inline Weapon *GetMainWeapon ()
    {
        return m_main_weapon;
    }
    inline Weapon *GetAuxiliaryWeapon ()
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

    Weapon const *GetCurrentWeapon () const;
    Weapon *GetCurrentWeapon ();

    Weapon *GetInventoryWeapon (ItemType weapon_type, Uint8 upgrade_level);
    Engine *GetInventoryEngine (Uint8 upgrade_level);
    Armor *GetInventoryArmor (Uint8 upgrade_level);
    Shield *GetInventoryShield (Uint8 upgrade_level);
    PowerGenerator *GetInventoryPowerGenerator (Uint8 upgrade_level);

    void SetArmorStatus (Float armor_status);
    void SetShieldStatus (Float shield_status);
    void SetPowerStatus (Float power_status);
    void SetWeaponStatus (Float weapon_status);

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
    Float m_time_alive;
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

    GameObjectReference<LaserBeam> m_laser_beam;
    GameObjectReference<TractorBeam> m_tractor_beam;
    GameObjectReference<ShieldEffect> m_shield_effect;

    SignalSender1<Uint32> m_sender_score_changed;
    SignalSender1<Float> m_sender_stoke_changed;
    SignalSender1<Uint32> m_sender_lives_remaining_changed;
    SignalSender1<Float> m_sender_time_alive_changed;
    SignalSender1<Float> m_sender_armor_status_changed;
    SignalSender1<Float> m_sender_shield_status_changed;
    SignalSender1<Float> m_sender_power_status_changed;
    SignalSender1<Float> m_sender_weapon_status_changed;
    SignalSender2<Uint8, Float> m_sender_mineral_inventory_changed;
}; // end of class PlayerShip
        
} // end of namespace Dis

#endif // !defined(_DIS_PLAYERSHIP_H_)
