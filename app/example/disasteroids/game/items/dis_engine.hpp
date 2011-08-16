// ///////////////////////////////////////////////////////////////////////////
// dis_engine.hpp by Victor Dods, created 2005/11/30
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_ENGINE_HPP_)
#define _DIS_ENGINE_HPP_

#include "xrb.hpp"

#include "dis_powereddevice.hpp"

using namespace Xrb;

namespace Dis
{

class Engine : public PoweredDevice
{
public:

    Engine (Uint8 const upgrade_level)
        :
        PoweredDevice(upgrade_level, IT_ENGINE)
    { }
    virtual ~Engine () { }

    Float MaxThrustForce () const { return ms_max_thrust_force[UpgradeLevel()]; }

    void SetInputs (Float right_left_input, Float up_down_input, Float auxiliary_input);

    // ///////////////////////////////////////////////////////////////////////
    // PoweredDevice virtual method overrides
    // ///////////////////////////////////////////////////////////////////////

    virtual Float PowerToBeUsedBasedOnInputs (bool attack_boost_is_enabled, bool defense_boost_is_enabled, Float time, Float frame_dt) const;

    virtual bool Activate (Float power, bool attack_boost_is_enabled, bool defense_boost_is_enabled, Float time, Float frame_dt);

private:

    static Float const ms_max_primary_power_output_rate[UPGRADE_LEVEL_COUNT];
    static Float const ms_max_auxiliary_power_output_rate[UPGRADE_LEVEL_COUNT];
    static Float const ms_max_thrust_force[UPGRADE_LEVEL_COUNT];

    Float m_right_left_input;
    Float m_up_down_input;
    Float m_auxiliary_input;
}; // end of class Engine

} // end of namespace Dis

#endif // !defined(_DIS_ENGINE_HPP_)

