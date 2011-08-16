// ///////////////////////////////////////////////////////////////////////////
// dis_solitary.cpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_solitary.hpp"

#include "dis_armor.hpp"
#include "dis_engine.hpp"
#include "dis_powergenerator.hpp"
#include "dis_powerup.hpp"
#include "dis_shield.hpp"
#include "dis_spawn.hpp"
#include "dis_weapon.hpp"

using namespace Xrb;

namespace Dis
{

void Solitary::Die (
    Entity *const killer,
    Entity *const kill_medium,
    FloatVector2 const &kill_location,
    FloatVector2 const &kill_normal,
    Float const kill_force,
    DamageType const kill_type,
    Float const time,
    Float const frame_dt)
{
    PlayerShip::Die(
        killer,
        kill_medium,
        kill_location,
        kill_normal,
        kill_force,
        kill_type,
        time,
        frame_dt);

    // TODO: spawn appropriate ship gibs
}

} // end of namespace Dis

