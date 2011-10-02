// ///////////////////////////////////////////////////////////////////////////
// dis_enemyship.hpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_ENEMYSHIP_HPP_)
#define _DIS_ENEMYSHIP_HPP_

#include "dis_ship.hpp"

using namespace Xrb;

namespace Dis {

class EnemyShip : public Ship
{
public:

    enum
    {
        ENEMY_LEVEL_COUNT = 4
    };

    EnemyShip (Uint8 enemy_level, Float max_health, EntityType entity_type)
        :
        Ship(max_health, entity_type),
        m_enemy_level(enemy_level)
    {
        ASSERT1(enemy_level < ENEMY_LEVEL_COUNT);
    }
    virtual ~EnemyShip () { }

    // Entity interface method
    virtual bool IsEnemyShip () const { return true; }

    Uint8 EnemyLevel () const { return m_enemy_level; }

    virtual void Die (
        Entity *killer,
        Entity *kill_medium,
        FloatVector2 const &kill_location,
        FloatVector2 const &kill_normal,
        Float kill_force,
        DamageType kill_type,
        Time time,
        Time::Delta frame_dt);

    // ///////////////////////////////////////////////////////////////////////
    // Ship interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual bool TakePowerup (Powerup *powerup, Time time, Time::Delta frame_dt)
    {
        return false;
    }

    // ///////////////////////////////////////////////////////////////////////
    // EnemyShip interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual void SetTarget (Mortal *target) = 0;

private:

    Uint8 m_enemy_level;
}; // end of class EnemyShip

} // end of namespace Dis

#endif // !defined(_DIS_ENEMYSHIP_HPP_)

