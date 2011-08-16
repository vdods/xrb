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
    RESOURCE_GLTEXTURE_DECLARATION(demi0_small)
    RESOURCE_GLTEXTURE_DECLARATION(demi1_small)
    RESOURCE_GLTEXTURE_DECLARATION(demi2_small)
    RESOURCE_GLTEXTURE_DECLARATION(demi3_small)
    RESOURCE_GLTEXTURE_DECLARATION(devourment0_small)
    RESOURCE_GLTEXTURE_DECLARATION(devourment1_small)
    RESOURCE_GLTEXTURE_DECLARATION(devourment2_small)
    RESOURCE_GLTEXTURE_DECLARATION(devourment3_small)
    RESOURCE_GLTEXTURE_DECLARATION(explosion1a_small)
    RESOURCE_GLTEXTURE_DECLARATION(explosion_dense_00)
    RESOURCE_GLTEXTURE_DECLARATION(explosion_rock_00)
    RESOURCE_GLTEXTURE_DECLARATION(fireball)
    RESOURCE_GLTEXTURE_DECLARATION(gauss_gun_trail)
    RESOURCE_GLTEXTURE_DECLARATION(grenade_small)
    RESOURCE_GLTEXTURE_DECLARATION(grinder0_small)
    RESOURCE_GLTEXTURE_DECLARATION(grinder1_small)
    RESOURCE_GLTEXTURE_DECLARATION(grinder2_small)
    RESOURCE_GLTEXTURE_DECLARATION(grinder3_small)
    RESOURCE_GLTEXTURE_DECLARATION(interloper0_small)
    RESOURCE_GLTEXTURE_DECLARATION(interloper1_small)
    RESOURCE_GLTEXTURE_DECLARATION(interloper2_small)
    RESOURCE_GLTEXTURE_DECLARATION(interloper3_small)
    RESOURCE_GLTEXTURE_DECLARATION(laser_beam)
    RESOURCE_GLTEXTURE_DECLARATION(lightning0)
    RESOURCE_GLTEXTURE_DECLARATION(lightning1)
    RESOURCE_GLTEXTURE_DECLARATION(lightning2)
    RESOURCE_GLTEXTURE_DECLARATION(lightning3)
    RESOURCE_GLTEXTURE_DECLARATION(mineral_0)
    RESOURCE_GLTEXTURE_DECLARATION(mineral_1)
    RESOURCE_GLTEXTURE_DECLARATION(mineral_2)
    RESOURCE_GLTEXTURE_DECLARATION(mineral_3)
    RESOURCE_GLTEXTURE_DECLARATION(missile_small)
    RESOURCE_GLTEXTURE_DECLARATION(plasma_ball)
    RESOURCE_GLTEXTURE_DECLARATION(reticle1)
    RESOURCE_GLTEXTURE_DECLARATION(revulsion0_small)
    RESOURCE_GLTEXTURE_DECLARATION(revulsion1_small)
    RESOURCE_GLTEXTURE_DECLARATION(revulsion2_small)
    RESOURCE_GLTEXTURE_DECLARATION(revulsion3_small)
    RESOURCE_GLTEXTURE_DECLARATION(shade0_small)
    RESOURCE_GLTEXTURE_DECLARATION(shade1_small)
    RESOURCE_GLTEXTURE_DECLARATION(shade2_small)
    RESOURCE_GLTEXTURE_DECLARATION(shade3_small)
    RESOURCE_GLTEXTURE_DECLARATION(shield_effect_small)
    RESOURCE_GLTEXTURE_DECLARATION(solitary_small)
    RESOURCE_GLTEXTURE_DECLARATION(star0)
    RESOURCE_GLTEXTURE_DECLARATION(star1)
    RESOURCE_GLTEXTURE_DECLARATION(star2)
    RESOURCE_GLTEXTURE_DECLARATION(star3)
    RESOURCE_GLTEXTURE_DECLARATION(starfield00)
    RESOURCE_GLTEXTURE_DECLARATION(tractor_beam)

    RESOURCE_ANIMATION_DECLARATION(health_powerup)
    RESOURCE_ANIMATION_DECLARATION(laser_impact)
    RESOURCE_ANIMATION_DECLARATION(lightning)
    RESOURCE_ANIMATION_DECLARATION(option_powerup)
}; // end of class ResourceCache

} // end of namespace Dis

#endif // !defined(_DIS_RESOURCECACHE_HPP_)

