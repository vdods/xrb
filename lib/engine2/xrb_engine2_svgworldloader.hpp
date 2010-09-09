// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_svgworldloader.hpp by Victor Dods, created 2009/09/16
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_SVGWORLDLOADER_HPP_)
#define _XRB_ENGINE2_SVGWORLDLOADER_HPP_

#include "xrb.hpp"

#include <string>

/*

design:

if an element has an xrb_ignore='true' attribute, XRB will not attempt to
read it, and will not issue any applicable errors/warnings about it.
unrecognized elements will cause a warning to be emitted.

each ObjectLayer corresponds to a <g> (group) element.
each <g> element will have an attribute xrb_bounding_box='value' where
'value' gives the id of the intended bounding box (a <rect> representing
a square) of the ObjectLayer.  the square must be axially aligned (no
rotation), and must reside inside the layer.  the attribute xrb_z_depth
will give the z-depth of the ObjectLayer, and these z-depths don't need to
appear in order (though maybe a warning should be issued).
xrb_quadtree_depth gives the depth of the quadtree to create in the object
layer.

game objects can only be <image> elements.  xlink:href="melty.00.png" gives
the relative path to the sprite image.  if xrb_entity_type is given, then
the <image> element will be used to create an Entity of the specified type
(see World::CreateEntity).  xrb_entity_name is an optional attribute which
gives a string identifying the entity for retrieving it out of the svg, e.g.
for getting pointers to entities. xrb_entity_type gives a (game-dependent)
string identifying the type of entity to create and attach to this sprite.
xrb_is_transparent='true' indicates that the entity is partially transparent
(such as a nebula) and must be dealt with specially.  xrb_invisible='true' (or
'false') indicates if the entity is invisible (i.e. has no attached Sprite)
-- TODO: implement xrb_invisible

in general, a transform="matrix(a,b,c,d,x,y)" specifies a Transform2 in the
following way:

    [a  c  x]                                   [r cos A  -s sin B  x]
    [b  d  y]       which can be expressed as   [r sin A   s cos B  y]
    [0  0  1]                                   [   0         0     1]

XRB can only deal with transforms of the following form:

          [1  0  x]   [cos A  -sin A  0]   [r  0  0]   [r cos A  -s sin A  x]
    TRS = [0  1  y] * [sin A   cos A  0] * [0  s  0] = [r sin A   s cos A  y]
          [0  0  1]   [  0       0    1]   [0  0  1]   [   0         0     1]

These represent a scaling, a rotation, and a translation (post-translate).
the matrices of this form are a 5-dimensional submanifold of the (6-dimensional
manifold) matrices of the general "a b c d x y" form given above.

So in order to get a TRS form one must attempt to match A and B.  if A and B
are too far apart, then this constitutes an error (there was a skew transform
applied to the <image> element in the SVG, which can't be handled by XRB).

The other type of transform is transform="translate(x,y)"

The x and y attributes give pre-translate coordinates.  width and height give
the width and height of the Sprite.  width and height must be matched, because
only square sprites are allowed.

*/

namespace Xrb {
namespace Engine2 {

class World;

void LoadSvgIntoWorld (std::string const &svg_path, World &world, Float current_time, Uint32 gltexture_flags);

} // end of namespace Engine2
} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_SVGWORLDLOADER_HPP_)
