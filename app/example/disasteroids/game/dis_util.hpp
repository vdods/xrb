// ///////////////////////////////////////////////////////////////////////////
// dis_util.hpp by Victor Dods, created 2006/03/16
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_UTIL_HPP_)
#define _DIS_UTIL_HPP_

#include "xrb.hpp"

#include <string>

#include "dis_entityreference.hpp"
#include "dis_mortal.hpp"

using namespace Xrb;

namespace Xrb {
namespace Engine2 {
    
class ObjectLayer;

} // end of namespace Engine2
} // end of namespace Xrb

namespace Dis {

class Entity;
class PhysicsHandler;

void RadiusDamage (
    Engine2::Circle::PhysicsHandler &physics_handler,
    Engine2::ObjectLayer &object_layer,
    Entity *damager,
    Entity *damage_medium,
    Float damage_amount,
    FloatVector2 const &damage_area_center,
    Float damage_area_radius,
    Mortal::DamageType damage_type,
    EntityReference<Mortal> const &ignore_this_mortal,
    Time time,
    Time::Delta frame_dt);

void RadiusKnockback (
    Engine2::Circle::PhysicsHandler &physics_handler,
    Engine2::ObjectLayer &object_layer,
    FloatVector2 const &explosion_area_center,
    Float explosion_area_radius,
    Float power,
    Time time,
    Time::Delta frame_dt);

std::string FormattedTimeString (Time time);

} // end of namespace Dis

#endif // !defined(_DIS_UTIL_HPP_)

