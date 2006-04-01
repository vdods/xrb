// ///////////////////////////////////////////////////////////////////////////
// dis_resourcecache.cpp by Victor Dods, created 2006/03/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_resourcecache.h"

using namespace Xrb;

namespace Dis
{

#define RESOURCE_GLTEXTURE_INITIALIZATION(basename) \
    m_##basename##_png = \
        Singletons::ResourceLibrary()-> \
            LoadFilename<GLTexture>( \
                GLTexture::Create, \
                "resources/" #basename ".png");
                
ResourceCache::ResourceCache ()
{
    RESOURCE_GLTEXTURE_INITIALIZATION(asteroid)
    RESOURCE_GLTEXTURE_INITIALIZATION(beam_gradient_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(devourment)
    RESOURCE_GLTEXTURE_INITIALIZATION(emp_explosion_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(explosion1a_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(fireball)
    RESOURCE_GLTEXTURE_INITIALIZATION(grenade_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(interloper)
    RESOURCE_GLTEXTURE_INITIALIZATION(mine_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(mineral_0)
    RESOURCE_GLTEXTURE_INITIALIZATION(mineral_1)
    RESOURCE_GLTEXTURE_INITIALIZATION(mineral_2)
    RESOURCE_GLTEXTURE_INITIALIZATION(mineral_3)
    RESOURCE_GLTEXTURE_INITIALIZATION(missile_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(reticle1)
    RESOURCE_GLTEXTURE_INITIALIZATION(revulsion)
    RESOURCE_GLTEXTURE_INITIALIZATION(sadface_small)
    RESOURCE_GLTEXTURE_INITIALIZATION(shade)
    RESOURCE_GLTEXTURE_INITIALIZATION(tractor_beam)
}

} // end of namespace Dis
