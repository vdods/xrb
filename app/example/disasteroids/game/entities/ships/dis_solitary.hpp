// ///////////////////////////////////////////////////////////////////////////
// dis_solitary.hpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_SOLITARY_HPP_)
#define _DIS_SOLITARY_HPP_

#include "dis_playership.hpp"

using namespace Xrb;

namespace Dis
{

class Solitary : public PlayerShip
{
public:

    static Float const ms_max_angular_velocity;
    static Float const ms_scale_factor;
    static Float const ms_baseline_mass;

    Solitary () : PlayerShip(100.0f, ET_SOLITARY) { }
    virtual ~Solitary () { }

    virtual void Die (
        Entity *killer,
        Entity *kill_medium,
        FloatVector2 const &kill_location,
        FloatVector2 const &kill_normal,
        Float kill_force,
        DamageType kill_type,
        Float time,
        Float frame_dt);

    // ///////////////////////////////////////////////////////////////////////
    // Ship interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual Float MaxAngularVelocity () const
    {
        return ms_max_angular_velocity;
    }
    virtual Float ShipScaleFactor () const
    {
        return ms_scale_factor;
    }
    virtual Float ShipBaselineMass () const
    {
        return ms_baseline_mass;
    }
}; // end of class Solitary

} // end of namespace Dis

#endif // !defined(_DIS_SOLITARY_HPP_)

