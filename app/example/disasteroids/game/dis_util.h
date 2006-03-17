// ///////////////////////////////////////////////////////////////////////////
// dis_util.h by Victor Dods, created 2006/03/16
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_UTIL_H_)
#define _DIS_UTIL_H_

#include "xrb.h"

#include "dis_gameobjectreference.h"
#include "dis_mortal.h"

using namespace Xrb;

namespace Xrb
{
namespace Engine2
{
    class ObjectLayer;
} // end of namespace Engine2
} // end of namespace Xrb

namespace Dis
{

class GameObject;
class PhysicsHandler;

void RadiusDamage (
    PhysicsHandler *physics_handler,
    Engine2::ObjectLayer *object_layer,
    GameObject *damager,
    GameObject *damage_medium,
    Float damage_amount,
    FloatVector2 const &damage_area_center,
    Float damage_area_radius,
    Mortal::DamageType damage_type,
    GameObjectReference<Mortal> const &ignore_this_mortal,
    Float time,
    Float frame_dt);
        
void RadiusKnockback (
    PhysicsHandler *physics_handler,
    Engine2::ObjectLayer *object_layer,
    FloatVector2 const &explosion_area_center,
    Float explosion_area_radius,
    Float power,
    Float time,
    Float frame_dt);
    
} // end of namespace Dis

#endif // !defined(_DIS_UTIL_H_)

