// ///////////////////////////////////////////////////////////////////////////
// dis_ship.h by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_SHIP_H_)
#define _DIS_SHIP_H_

#include "dis_mortal.h"

#include "xrb_math.h" // TEMP

using namespace Xrb;

namespace Dis
{

class Item;
class Powerup;
class Weapon;

class Ship : public Mortal
{
public:

    Ship (Float max_health, EntityType entity_type);
    virtual ~Ship ();

    static std::string const &GetShipSpriteFilename (EntityType ship_type, Uint8 enemy_level);
    static Float GetShipScaleFactor (EntityType ship_type, Uint8 enemy_level);

    virtual bool GetIsShip () const { return true; }
    bool GetIsDisabled () const { return m_disable_time > 0.0f; }

    inline void SetReticleCoordinates (FloatVector2 const &reticle_coordinates)
    {
        ASSERT1(Math::IsFinite(reticle_coordinates[Dim::X]))
        ASSERT1(Math::IsFinite(reticle_coordinates[Dim::Y]))
        m_reticle_coordinates = reticle_coordinates;
    }
    inline void SetEngineRightLeftInput (Sint8 const engine_right_left_input)
    {
        ASSERT1(engine_right_left_input >= -SINT8_UPPER_BOUND)
        m_engine_right_left_input = engine_right_left_input;
    }
    inline void SetEngineUpDownInput (Sint8 const engine_up_down_input)
    {
        ASSERT1(engine_up_down_input >= -SINT8_UPPER_BOUND)
        m_engine_up_down_input = engine_up_down_input;
    }
    inline void SetWeaponPrimaryInput (Uint8 const weapon_primary_input)
    {
        m_weapon_primary_input = weapon_primary_input;
    }
    inline void SetWeaponSecondaryInput (Uint8 const weapon_secondary_input)
    {
        m_weapon_secondary_input = weapon_secondary_input;
    }

    inline void AccumulateDisableTime (Float const disable_time)
    {
        ASSERT1(disable_time > 0.0f)
        m_disable_time += disable_time;
    }

    virtual void HandleNewOwnerObject ();

    virtual void Think (Float time, Float frame_dt);
    virtual void Die (
        Entity *killer,
        Entity *kill_medium,
        FloatVector2 const &kill_location,
        FloatVector2 const &kill_normal,
        Float kill_force,
        DamageType kill_type,
        Float time,
        Float frame_dt);

    virtual FloatVector2 GetMuzzleLocation (Weapon const *weapon) const
    {
        ASSERT1(weapon != NULL)
        // by default, just return the front of the ship (so not all
        // ships have to override this method)
        return GetTranslation() + GetScaleFactor() * Math::UnitVector(GetAngle());
    }
    virtual FloatVector2 GetMuzzleDirection (Weapon const *weapon) const
    {
        ASSERT1(weapon != NULL)
        // by default, just return the direction the ship is facing,
        // (so that not all ships have to override this method)
        return Math::UnitVector(GetAngle());
    }
    virtual Float GetMaxAngularVelocity () const = 0;
    virtual Float GetShipScaleFactor () const = 0;
    virtual Float GetShipBaselineFirstMoment () const = 0;

    virtual bool TakePowerup (Powerup *powerup) = 0;

protected:

    inline FloatVector2 const &GetReticleCoordinates () const
    {
        return m_reticle_coordinates;
    }
    inline Float GetNormalizedEngineRightLeftInput () const
    {
        ASSERT1(m_engine_right_left_input >= -SINT8_UPPER_BOUND)
        return static_cast<Float>(m_engine_right_left_input) /
               static_cast<Float>(SINT8_UPPER_BOUND);
    }
    inline Float GetNormalizedEngineUpDownInput () const
    {
        ASSERT1(m_engine_right_left_input >= -SINT8_UPPER_BOUND)
        return static_cast<Float>(m_engine_up_down_input) /
               static_cast<Float>(SINT8_UPPER_BOUND);
    }
    inline Float GetNormalizedWeaponPrimaryInput () const
    {
        return static_cast<Float>(m_weapon_primary_input) /
               static_cast<Float>(UINT8_UPPER_BOUND);
    }
    inline Float GetNormalizedWeaponSecondaryInput () const
    {
        return static_cast<Float>(m_weapon_secondary_input) /
               static_cast<Float>(UINT8_UPPER_BOUND);
    }

    inline void AimShipAtReticleCoordinates (Float frame_dt) { AimShipAtCoordinates(m_reticle_coordinates, frame_dt); }
    void AimShipAtCoordinates (FloatVector2 const &coordinates, Float frame_dt);
    virtual void ResetInputs ();

private:

    Float m_disable_time;

    FloatVector2 m_reticle_coordinates;
    Sint8 m_engine_right_left_input;
    Sint8 m_engine_up_down_input;
    Uint8 m_weapon_primary_input;
    Uint8 m_weapon_secondary_input;
}; // end of class Ship

} // end of namespace Dis

#endif // !defined(_DIS_SHIP_H_)

