// ///////////////////////////////////////////////////////////////////////////
// dis_enemyship.cpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_enemyship.h"

#include "dis_playership.h"
#include "dis_world.h"

using namespace Xrb;

namespace Dis
{

void EnemyShip::Die (
    Entity *const killer,
    Entity *const kill_medium,
    FloatVector2 const &kill_location,
    FloatVector2 const &kill_normal,
    Float const kill_force,
    DamageType const kill_type,
    Float const time,
    Float const frame_dt)
{
    Ship::Die(
        killer,
        kill_medium,
        kill_location,
        kill_normal,
        kill_force,
        kill_type,
        time,
        frame_dt);

    // handle scoring
    if (killer != NULL && killer->GetIsPlayerShip())
        static_cast<PlayerShip *>(killer)->CreditEnemyKill(GetType(), GetEnemyLevel());
                
    // notify the world that this enemyship is going bye-bye
    DStaticCast<World *>(GetWorld())->RecordDestroyedEnemyShip(this);
}

} // end of namespace Dis

