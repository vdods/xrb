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

#include "xrb_emptystring.hpp"
#include "xrb_exception.hpp"
#include "xrb_time.hpp"

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

STAGING

multiple "stages" for each svg map can be worked by stage number.  if present in the root <svg>
element, the xrb_stage_count attribute will specify the number of stages used in the map.  stages
are indexed starting with 1 (not 0 as in arrays).

if stage-processing is active, any xrb_* (and possibly other) attribute which starts with \
(backslash) will be parsed as a backslash-delimited list of values.  for example:

    xrb_ignore='\true\false\false'

means that in stage 1 the relevant object is ignored, while in stages 2 and 3 the object is present.
this allows different configurations of a map to be used as 'different maps'.

if a xrb_* (or possible other) attribute does NOT begin with a backslash, then it is considered to
be a fixed value across all stage values.  this means that the map designer must only create
backslash-delimited attribute values for the attributes that should be different across different
stages.

xrb_stage_count is present so that validation of the backslash-delimited attribute values can be
performed.  if there is a mismatch between number of backslash-delimited values for a particular
attribute and the value of xrb_stage_count, an error message will be issued.


*/

namespace Lvd {
namespace Xml {

struct Element;

} // end of namespace Xml
} // end of namespace Lvd

namespace Xrb {
namespace Engine2 {

class World;

// for stage-processing the attributes of an XML element.
void StageProcessAttributes (
    Lvd::Xml::Element &element,
    Uint32 stage,
    Uint32 stage_count,
    std::string const &additional_stageable_attribute_name_prefix = g_empty_string);
// returns the xrb_stage_count attribute value of the root <svg> element (a positive number), or
// 0 if this value is invalid or if the attribute is not present.
Uint32 ParseSvgStageCount (std::string const &svg_path) throw(Exception);
// loads the given svg file into the specified world.  if stage is 0, then no stage processing is
// done (i.e. backslash-delimited values will not be parsed and will be taken as literal strings).
// if stage is a positive value, that stage number will be loaded, and backslash-delimited values
// will be parsed as described above.
void LoadSvgIntoWorld (
    std::string const &svg_path,
    World &world,
    Time current_time,
    Uint32 gltexture_flags,
    Uint32 stage = 0,
    std::string const &additional_stageable_attribute_name_prefix = g_empty_string) throw(Exception);

} // end of namespace Engine2
} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_SVGWORLDLOADER_HPP_)
