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

#define RESOURCE_GLTEXTURE_INITIALIZATION(basename) \
    m_##basename##_png = \
        Singleton::ResourceLibrary(). \
            LoadFilename<GLTexture>( \
                GLTexture::Create, \
                "resources/" #basename ".png");

ResourceCache::ResourceCache ()
{
    RESOURCE_GLTEXTURE_INITIALIZATION(asteroid_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(beam_gradient_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(devourment0_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(devourment1_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(devourment2_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(devourment3_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(emp_explosion_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(explosion1a_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(fireball)
    RESOURCE_GLTEXTURE_INITIALIZATION(grenade_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(grinder0_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(grinder1_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(grinder2_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(grinder3_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(interloper0_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(mineral_0)
    RESOURCE_GLTEXTURE_INITIALIZATION(mineral_1)
    RESOURCE_GLTEXTURE_INITIALIZATION(mineral_2)
    RESOURCE_GLTEXTURE_INITIALIZATION(mineral_3)
    RESOURCE_GLTEXTURE_INITIALIZATION(missile_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(powerup)
    RESOURCE_GLTEXTURE_INITIALIZATION(reticle1)
    RESOURCE_GLTEXTURE_INITIALIZATION(revulsion0_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(revulsion1_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(revulsion2_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(revulsion3_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(sadface_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(shade0_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(shade1_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(shade2_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(shade3_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(solitary_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(tractor_beam)
}

} // end of namespace Dis

