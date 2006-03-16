// ///////////////////////////////////////////////////////////////////////////
// dis_resourcecache.h by Victor Dods, created 2006/03/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_RESOURCECACHE_H_)
#define _DIS_RESOURCECACHE_H_

#include "xrb.h"

#include "xrb_gltexture.h"
#include "xrb_resourcelibrary.h"

using namespace Xrb;

namespace Dis
{

#define RESOURCE_GLTEXTURE_DECLARATION(basename) \
    Resource<GLTexture> m_##basename##_png;

// temp object to hold cached resources until a formal class is written
class ResourceCache
{
public:

    ResourceCache ();

private:

    RESOURCE_GLTEXTURE_DECLARATION(asteroid)
    RESOURCE_GLTEXTURE_DECLARATION(beam_gradient_small)
    RESOURCE_GLTEXTURE_DECLARATION(devourment)
    RESOURCE_GLTEXTURE_DECLARATION(emp_explosion_small)
    RESOURCE_GLTEXTURE_DECLARATION(explosion1a_small)
    RESOURCE_GLTEXTURE_DECLARATION(fireball)
    RESOURCE_GLTEXTURE_DECLARATION(grenade_small)
    RESOURCE_GLTEXTURE_DECLARATION(interloper)
    RESOURCE_GLTEXTURE_DECLARATION(mine_small)
    RESOURCE_GLTEXTURE_DECLARATION(mineral_0)
    RESOURCE_GLTEXTURE_DECLARATION(mineral_1)
    RESOURCE_GLTEXTURE_DECLARATION(mineral_2)
    RESOURCE_GLTEXTURE_DECLARATION(mineral_3)
    RESOURCE_GLTEXTURE_DECLARATION(missile_small)
    RESOURCE_GLTEXTURE_DECLARATION(reticle1)
    RESOURCE_GLTEXTURE_DECLARATION(revulsion)
    RESOURCE_GLTEXTURE_DECLARATION(sadface_small)
    RESOURCE_GLTEXTURE_DECLARATION(shade)
    RESOURCE_GLTEXTURE_DECLARATION(tractor_beam)
}; // end of class ResourceCache

        
} // end of namespace Dis

#endif // !defined(_DIS_RESOURCECACHE_H_)

