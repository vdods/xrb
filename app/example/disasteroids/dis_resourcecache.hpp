// ///////////////////////////////////////////////////////////////////////////
// dis_resourcecache.hpp by Victor Dods, created 2006/03/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_RESOURCECACHE_HPP_)
#define _DIS_RESOURCECACHE_HPP_

#include "xrb.hpp"

#include "xrb_animation.hpp"
#include "xrb_gltexture.hpp"
#include "xrb_resourcelibrary.hpp"

using namespace Xrb;

namespace Dis {

#define RESOURCE_GLTEXTURE_DECLARATION(basename) Resource<GlTexture> m_##basename##_png;
#define RESOURCE_ANIMATION_DECLARATION(basename) Resource<Animation::Sequence> m_##basename##_anim;

// temp object to hold cached resources until a formal class is written
class ResourceCache
{
public:

    ResourceCache ();

private:

    RESOURCE_GLTEXTURE_DECLARATION(asteroid_small)
    RESOURCE_GLTEXTURE_DECLARATION(demi_0)
    RESOURCE_GLTEXTURE_DECLARATION(demi_1)
    RESOURCE_GLTEXTURE_DECLARATION(demi_2)
    RESOURCE_GLTEXTURE_DECLARATION(demi_3)
    RESOURCE_GLTEXTURE_DECLARATION(devourment_0)
    RESOURCE_GLTEXTURE_DECLARATION(devourment_1)
    RESOURCE_GLTEXTURE_DECLARATION(devourment_2)
    RESOURCE_GLTEXTURE_DECLARATION(devourment_3)
    RESOURCE_GLTEXTURE_DECLARATION(energy_ball_0)
    RESOURCE_GLTEXTURE_DECLARATION(energy_ball_1)
    RESOURCE_GLTEXTURE_DECLARATION(explosion1a_small)
    RESOURCE_GLTEXTURE_DECLARATION(explosion_dense_00)
    RESOURCE_GLTEXTURE_DECLARATION(explosion_rock_00)
    RESOURCE_GLTEXTURE_DECLARATION(fireball)
    RESOURCE_GLTEXTURE_DECLARATION(gauss_gun_trail)
    RESOURCE_GLTEXTURE_DECLARATION(grenade)
    RESOURCE_GLTEXTURE_DECLARATION(grinder_0_0)
    RESOURCE_GLTEXTURE_DECLARATION(grinder_0_1)
    RESOURCE_GLTEXTURE_DECLARATION(grinder_1_0)
    RESOURCE_GLTEXTURE_DECLARATION(grinder_1_1)
    RESOURCE_GLTEXTURE_DECLARATION(grinder_2_0)
    RESOURCE_GLTEXTURE_DECLARATION(grinder_2_1)
    RESOURCE_GLTEXTURE_DECLARATION(grinder_3_0)
    RESOURCE_GLTEXTURE_DECLARATION(grinder_3_1)
    RESOURCE_GLTEXTURE_DECLARATION(interloper_0)
    RESOURCE_GLTEXTURE_DECLARATION(interloper_1)
    RESOURCE_GLTEXTURE_DECLARATION(interloper_2)
    RESOURCE_GLTEXTURE_DECLARATION(interloper_3)
    RESOURCE_GLTEXTURE_DECLARATION(laser_beam)
    RESOURCE_GLTEXTURE_DECLARATION(lightning0)
    RESOURCE_GLTEXTURE_DECLARATION(lightning1)
    RESOURCE_GLTEXTURE_DECLARATION(lightning2)
    RESOURCE_GLTEXTURE_DECLARATION(lightning3)
    RESOURCE_GLTEXTURE_DECLARATION(mineral_0)
    RESOURCE_GLTEXTURE_DECLARATION(mineral_1)
    RESOURCE_GLTEXTURE_DECLARATION(mineral_2)
    RESOURCE_GLTEXTURE_DECLARATION(mineral_3)
    RESOURCE_GLTEXTURE_DECLARATION(missile)
    RESOURCE_GLTEXTURE_DECLARATION(plasma_ball_green)
    RESOURCE_GLTEXTURE_DECLARATION(plasma_ball_yellow)
    RESOURCE_GLTEXTURE_DECLARATION(reticle1)
    RESOURCE_GLTEXTURE_DECLARATION(revulsion_0)
    RESOURCE_GLTEXTURE_DECLARATION(revulsion_1)
    RESOURCE_GLTEXTURE_DECLARATION(revulsion_2)
    RESOURCE_GLTEXTURE_DECLARATION(revulsion_3)
    RESOURCE_GLTEXTURE_DECLARATION(shade_0)
    RESOURCE_GLTEXTURE_DECLARATION(shade_1)
    RESOURCE_GLTEXTURE_DECLARATION(shade_2)
    RESOURCE_GLTEXTURE_DECLARATION(shade_3)
    RESOURCE_GLTEXTURE_DECLARATION(shield_0)
    RESOURCE_GLTEXTURE_DECLARATION(shield_1)
    RESOURCE_GLTEXTURE_DECLARATION(shockwave)
    RESOURCE_GLTEXTURE_DECLARATION(solitary)
    RESOURCE_GLTEXTURE_DECLARATION(star0)
    RESOURCE_GLTEXTURE_DECLARATION(star1)
    RESOURCE_GLTEXTURE_DECLARATION(star2)
    RESOURCE_GLTEXTURE_DECLARATION(star3)
    RESOURCE_GLTEXTURE_DECLARATION(tractor_beam)

    RESOURCE_ANIMATION_DECLARATION(energy_ball)
    RESOURCE_ANIMATION_DECLARATION(grinder_0)
    RESOURCE_ANIMATION_DECLARATION(grinder_1)
    RESOURCE_ANIMATION_DECLARATION(grinder_2)
    RESOURCE_ANIMATION_DECLARATION(grinder_3)
    RESOURCE_ANIMATION_DECLARATION(health_powerup)
    RESOURCE_ANIMATION_DECLARATION(laser_impact)
    RESOURCE_ANIMATION_DECLARATION(lightning)
    RESOURCE_ANIMATION_DECLARATION(option_powerup)
    RESOURCE_ANIMATION_DECLARATION(shield)
}; // end of class ResourceCache

} // end of namespace Dis

#endif // !defined(_DIS_RESOURCECACHE_HPP_)

