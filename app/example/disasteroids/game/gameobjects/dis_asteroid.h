// ///////////////////////////////////////////////////////////////////////////
// dis_asteroid.h by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_ASTEROID_H_)
#define _DIS_ASTEROID_H_

#include "dis_mortal.h"

#include "dis_enums.h"

using namespace Xrb;

namespace Dis
{

class Asteroid : public Mortal
{
public:

    enum
    {
        MAX_MINERAL_LEVEL = 3
    };

    Asteroid (
        Float const first_moment,
        Float const mineral_content,
        bool const is_a_secondary_asteroid)
        :
        Mortal(
            ms_health_factor * first_moment,
            ms_health_factor * first_moment,
            T_ASTEROID,
            CT_SOLID_COLLISION)
    {
        m_is_a_secondary_asteroid = is_a_secondary_asteroid;
        m_delete_upon_next_think = false;
        m_time_at_decay_start = -1.0f;
        m_mineral_content_byte = GetMineralContentByte(mineral_content);
        SetStrength(D_FIRE|D_EXPLOSION);
        SetImmunity(D_EMP);
    }
    virtual ~Asteroid () { }

    virtual void HandleScheduledDeletion (Float time);
        
    virtual void Think (Float time, Float frame_dt);
    virtual void Collide (
        Entity *collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
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

private:

    enum
    {
        DISTRIBUTION_LOOKUP_TABLE_COUNT = MAX_MINERAL_LEVEL + 1,
        DISTRIBUTION_LOOKUP_TABLE_SIZE = 50
    };

    Uint8 GetRandomMineral () const;
    Float GetProportionToConvertToMinerals () const;
    inline Float GetMineralContent () const
    {
        return static_cast<Float>(m_mineral_content_byte) / 255.0f;
    }
    inline Uint8 GetMineralContentByte (Float const mineral_content)
    {
        ASSERT1(mineral_content >= 0.0f && mineral_content <= 1.0f)
        return static_cast<Uint8>(255.0f * mineral_content);
    }

    static Uint8 const ms_mineral_distribution_lookup_table[DISTRIBUTION_LOOKUP_TABLE_COUNT][DISTRIBUTION_LOOKUP_TABLE_SIZE];
    static Uint8 const ms_number_of_fragments_to_spawn;
    static Float const ms_minimum_breakup_mass;
    static Float const ms_decay_scale_factor;
    static Float const ms_decay_delay;
    static Float const ms_decay_time;
    static Float const ms_health_factor;

    bool m_is_a_secondary_asteroid;
    bool m_delete_upon_next_think;
    Float m_time_at_decay_start;
    Uint8 m_mineral_content_byte;
}; // end of class Asteroid

} // end of namespace Dis

#endif // !defined(_DIS_ASTEROID_H_)

