// ///////////////////////////////////////////////////////////////////////////
// dis_resourcecache.cpp by Victor Dods, created 2006/03/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_resourcecache.hpp"

using namespace Xrb;

namespace Dis
{

#define RESOURCE_GLTEXTURE_INITIALIZATION(basename) m_##basename##_png = GlTexture::Load("fs://" #basename ".png");
#define RESOURCE_ANIMATION_INITIALIZATION(basename) m_##basename##_anim = Animation::Sequence::Load("fs://" #basename ".anim");

ResourceCache::ResourceCache ()
{
    RESOURCE_GLTEXTURE_INITIALIZATION(asteroid_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(demi_0)
    RESOURCE_GLTEXTURE_INITIALIZATION(demi_1)
    RESOURCE_GLTEXTURE_INITIALIZATION(demi_2)
    RESOURCE_GLTEXTURE_INITIALIZATION(demi_3)
    RESOURCE_GLTEXTURE_INITIALIZATION(devourment_0)
    RESOURCE_GLTEXTURE_INITIALIZATION(devourment_1)
    RESOURCE_GLTEXTURE_INITIALIZATION(devourment_2)
    RESOURCE_GLTEXTURE_INITIALIZATION(devourment_3)
    RESOURCE_GLTEXTURE_INITIALIZATION(energy_ball_0)
    RESOURCE_GLTEXTURE_INITIALIZATION(energy_ball_1)
    RESOURCE_GLTEXTURE_INITIALIZATION(explosion1a_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(explosion_dense_00)
    RESOURCE_GLTEXTURE_INITIALIZATION(explosion_rock_00)
    RESOURCE_GLTEXTURE_INITIALIZATION(fireball)
    RESOURCE_GLTEXTURE_INITIALIZATION(gauss_gun_trail)
    RESOURCE_GLTEXTURE_INITIALIZATION(grenade)
    RESOURCE_GLTEXTURE_INITIALIZATION(grinder_0_0)
    RESOURCE_GLTEXTURE_INITIALIZATION(grinder_0_1)
    RESOURCE_GLTEXTURE_INITIALIZATION(grinder_1_0)
    RESOURCE_GLTEXTURE_INITIALIZATION(grinder_1_1)
    RESOURCE_GLTEXTURE_INITIALIZATION(grinder_2_0)
    RESOURCE_GLTEXTURE_INITIALIZATION(grinder_2_1)
    RESOURCE_GLTEXTURE_INITIALIZATION(grinder_3_0)
    RESOURCE_GLTEXTURE_INITIALIZATION(grinder_3_1)
    RESOURCE_GLTEXTURE_INITIALIZATION(interloper_0)
    RESOURCE_GLTEXTURE_INITIALIZATION(interloper_1)
    RESOURCE_GLTEXTURE_INITIALIZATION(interloper_2)
    RESOURCE_GLTEXTURE_INITIALIZATION(interloper_3)
    RESOURCE_GLTEXTURE_INITIALIZATION(laser_beam)
    RESOURCE_GLTEXTURE_INITIALIZATION(lightning0)
    RESOURCE_GLTEXTURE_INITIALIZATION(lightning1)
    RESOURCE_GLTEXTURE_INITIALIZATION(lightning2)
    RESOURCE_GLTEXTURE_INITIALIZATION(lightning3)
    RESOURCE_GLTEXTURE_INITIALIZATION(mineral_0)
    RESOURCE_GLTEXTURE_INITIALIZATION(mineral_1)
    RESOURCE_GLTEXTURE_INITIALIZATION(mineral_2)
    RESOURCE_GLTEXTURE_INITIALIZATION(mineral_3)
    RESOURCE_GLTEXTURE_INITIALIZATION(missile)
    RESOURCE_GLTEXTURE_INITIALIZATION(plasma_ball_green)
    RESOURCE_GLTEXTURE_INITIALIZATION(plasma_ball_yellow)
    RESOURCE_GLTEXTURE_INITIALIZATION(reticle1)
    RESOURCE_GLTEXTURE_INITIALIZATION(revulsion_0)
    RESOURCE_GLTEXTURE_INITIALIZATION(revulsion_1)
    RESOURCE_GLTEXTURE_INITIALIZATION(revulsion_2)
    RESOURCE_GLTEXTURE_INITIALIZATION(revulsion_3)
    RESOURCE_GLTEXTURE_INITIALIZATION(shade_0)
    RESOURCE_GLTEXTURE_INITIALIZATION(shade_1)
    RESOURCE_GLTEXTURE_INITIALIZATION(shade_2)
    RESOURCE_GLTEXTURE_INITIALIZATION(shade_3)
    RESOURCE_GLTEXTURE_INITIALIZATION(shield_0)
    RESOURCE_GLTEXTURE_INITIALIZATION(shield_1)
    RESOURCE_GLTEXTURE_INITIALIZATION(shockwave)
    RESOURCE_GLTEXTURE_INITIALIZATION(solitary)
    RESOURCE_GLTEXTURE_INITIALIZATION(star0)
    RESOURCE_GLTEXTURE_INITIALIZATION(star1)
    RESOURCE_GLTEXTURE_INITIALIZATION(star2)
    RESOURCE_GLTEXTURE_INITIALIZATION(star3)
    RESOURCE_GLTEXTURE_INITIALIZATION(tractor_beam)

    RESOURCE_ANIMATION_INITIALIZATION(energy_ball)
    RESOURCE_ANIMATION_INITIALIZATION(grinder_0)
    RESOURCE_ANIMATION_INITIALIZATION(grinder_1)
    RESOURCE_ANIMATION_INITIALIZATION(grinder_2)
    RESOURCE_ANIMATION_INITIALIZATION(grinder_3)
    RESOURCE_ANIMATION_INITIALIZATION(health_powerup)
    RESOURCE_ANIMATION_INITIALIZATION(laser_impact)
    RESOURCE_ANIMATION_INITIALIZATION(lightning)
    RESOURCE_ANIMATION_INITIALIZATION(option_powerup)
    RESOURCE_ANIMATION_INITIALIZATION(shield)
}

} // end of namespace Dis

