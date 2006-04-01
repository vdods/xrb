// ///////////////////////////////////////////////////////////////////////////
// dis_enemyship.h by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_ENEMYSHIP_H_)
#define _DIS_ENEMYSHIP_H_

#include "dis_ship.h"

using namespace Xrb;

namespace Dis
{

class EnemyShip : public Ship
{
public:

    enum
    {
        ENEMY_LEVEL_COUNT = 4
    };

    EnemyShip (Uint8 const enemy_level, Float const max_health, Type const type)
        :
        Ship(max_health, type),
        m_enemy_level(enemy_level)
    {
        ASSERT1(enemy_level < ENEMY_LEVEL_COUNT)
    }
    virtual ~EnemyShip () { }

    // GameObject interface method
    virtual bool GetIsEnemyShip () const { return true; }
    
    inline Uint8 GetEnemyLevel () const { return m_enemy_level; }

    virtual void Die (
        GameObject *killer,
        GameObject *kill_medium,
        FloatVector2 const &kill_location,
        FloatVector2 const &kill_normal,
        Float kill_force,
        DamageType kill_type,
        Float time,
        Float frame_dt);
    
    // ///////////////////////////////////////////////////////////////////////
    // Ship interface methods
    // ///////////////////////////////////////////////////////////////////////
    
    virtual bool TakePowerup (Powerup *powerup)
    {
        return false;
    }            
    
private:

    Uint8 m_enemy_level;
}; // end of class EnemyShip
        
} // end of namespace Dis

#endif // !defined(_DIS_ENEMYSHIP_H_)
