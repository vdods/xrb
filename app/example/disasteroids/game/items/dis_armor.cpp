// ///////////////////////////////////////////////////////////////////////////
// dis_armor.cpp by Victor Dods, created 2005/12/03
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_armor.h"

using namespace Xrb;

namespace Dis
{

Float const Armor::ms_damage_dissipation_rate[UPGRADE_LEVEL_COUNT] = { 1.0f, 2.0f, 3.0f, 4.0f };
Float const Armor::ms_first_moment[UPGRADE_LEVEL_COUNT] = { 0.0f, 125.0f, 250.0f, 375.0f };

} // end of namespace Dis
