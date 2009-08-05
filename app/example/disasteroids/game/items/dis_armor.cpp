// ///////////////////////////////////////////////////////////////////////////
// dis_armor.cpp by Victor Dods, created 2005/12/03
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_armor.hpp"

using namespace Xrb;

namespace Dis
{

Float const Armor::ms_damage_dissipation_rate[UPGRADE_LEVEL_COUNT] = { 1.5f, 3.0f, 6.0f, 12.0f };
Float const Armor::ms_mass[UPGRADE_LEVEL_COUNT] = { 0.0f, 100.0f, 300.0f, 700.0f };

} // end of namespace Dis
