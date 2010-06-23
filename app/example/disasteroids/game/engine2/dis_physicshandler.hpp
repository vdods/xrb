// ///////////////////////////////////////////////////////////////////////////
// dis_physicshandler.hpp by Victor Dods, created 2010/06/22
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_PHYSICSHANDLER_HPP_)
#define _DIS_PHYSICSHANDLER_HPP_

#include "xrb_engine2_circle_physicshandler.hpp"

using namespace Xrb;

namespace Xrb {
namespace Engine2 {
namespace Circle {

class Entity;

} // end of namespace Circle
} // end of namespace Engine2
} // end of namespace Xrb

namespace Dis
{

class PhysicsHandler : public Engine2::Circle::PhysicsHandler
{
public:

    PhysicsHandler ()
        :
        Engine2::Circle::PhysicsHandler()
    { }
    virtual ~PhysicsHandler () { }

    virtual bool CollisionExemption (Engine2::Circle::Entity const &entity0, Engine2::Circle::Entity const &entity1) const;
    virtual Float MaxSpeed (Engine2::Circle::Entity const &entity) const;
}; // end of class PhysicsHandler

} // end of namespace Dis

#endif // !defined(_DIS_PHYSICSHANDLER_HPP_)

