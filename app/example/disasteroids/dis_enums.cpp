// ///////////////////////////////////////////////////////////////////////////
// dis_enums.cpp by Victor Dods, created 2006/08/29
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_enums.hpp"

namespace Dis
{

std::string const &DifficultyLevelString (DifficultyLevel difficulty_level)
{
    static std::string const s_difficulty_level_string[DL_COUNT] =
    {
        "EASY",
        "MEDIUM",
        "HARD",
        "CRUEL"
    };

    ASSERT1(difficulty_level < DL_COUNT);
    return s_difficulty_level_string[difficulty_level];
}

std::string const &ExplosionAssetPath (ExplosionType explosion_type)
{
    static std::string const s_explosion_asset_path[EXPLO_COUNT] =
    {
        "fs://explosion_dense_00.png",
        "fs://explosion1a_small.png",
        "fs://explosion_rock_00.png",
        "fs://shockwave.png",
        "fs://fireball.png",
        "fs://shockwave.png"
    };

    ASSERT1(explosion_type < EXPLO_COUNT);
    return s_explosion_asset_path[explosion_type];
}

} // end of namespace Dis
