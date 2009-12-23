// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_objectlayer.hpp by Victor Dods, created 2004/07/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_OBJECTLAYER_HPP_)
#define _XRB_ENGINE2_OBJECTLAYER_HPP_

#include "xrb.hpp"

#include <string>

#include "xrb_engine2_object.hpp"
#include "xrb_engine2_types.hpp"
#include "xrb_matrix2.hpp"
#include "xrb_transform2.hpp"
#include "xrb_vector.hpp"

namespace Xrb {

class RenderContext;
class Serializer;

namespace Engine2 {

class VisibilityQuadTree;
class Entity;
class World;

class ObjectLayer
{
public:

    virtual ~ObjectLayer ();

    static ObjectLayer *Create (
        World *owner_world,
        bool is_wrapped,
        Float side_length,
        Uint32 tree_depth,
        Float z_depth);
    static ObjectLayer *Create (
        Serializer &serializer,
        World *owner_world);

    inline std::string const &Name () const { return m_name; }
    inline World *OwnerWorld () const { return m_owner_world; }
    inline bool IsWrapped () const { return m_is_wrapped; }
    inline Float SideLength () const { return m_side_length; }
    inline Float ZDepth () const { return m_z_depth; }
    Object *SmallestObjectTouchingPoint (FloatVector2 const &point) const;
    bool DoesAreaOverlapAnyObject (
        FloatVector2 const &area_center,
        Float area_radius) const;
    FloatVector2 NormalizedCoordinates (FloatVector2 const &coordinates) const;
    FloatVector2 AdjustedCoordinates (
        FloatVector2 const &coordinates,
        FloatVector2 const &reference_coordinates) const;

    inline void SetName (std::string const &name) { m_name = name; }
    inline void SetIsWrapped (bool is_wrapped) { m_is_wrapped = is_wrapped; }
    inline void SetZDepth (Float z_depth) { m_z_depth = z_depth; }

    void Write (Serializer &serializer) const;
    Uint32 Draw (
        RenderContext const &render_context,
        FloatMatrix2 const &world_to_screen,
        Float pixels_in_view_radius,
        FloatVector2 const &view_center,
        Float const &view_radius,
        TransparentObjectVector *transparent_object_vector);
    void AddObject (Object *object);
    void RemoveObject (Object *object);
    // makes sure that the given object is within the object layer's
    // boundaries, by either clamping or wrapping position, depending
    // on if the object layer has wrapping turned on.
    void HandleContainmentOrWrapping (Object *object);
    // utility function to perform position containment on a FloatVector2
    void ContainVector2 (FloatVector2 *vector) const;
    // utility function to perform position containment on a FloatTransform2
    void ContainTransform2 (FloatTransform2 *transform) const;
    // contains the entity's position within this object layer
    // and will zero the velocity in the constrained dimensions.
    void ContainEntity (Entity *entity) const;
    void WrapVector2 (FloatVector2 *vector) const;
    void WrapTransform2 (FloatTransform2 *transform) const;
    void WrapEntity (Entity *entity) const;

protected:

    // protected constructor so that you must use Create()
    ObjectLayer (
        World *owner_world,
        bool is_wrapped,
        Float side_length,
        Float z_depth);

    inline VisibilityQuadTree *GetQuadTree () const
    {
        return m_quad_tree;
    }

    // the name of this object layer.  you can use it for whatever
    // the hell you want (though its suggested use is for identifying
    // the different object layers to the physics handler, so it knows
    // which object layers to make quadtrees for).
    std::string m_name;
    // quad tree for this layer
    VisibilityQuadTree *m_quad_tree;

private:

    // the World which owns this object layer
    World *m_owner_world;
    // the length of one side of the quad tree
    Float m_side_length;
    // 1/2 the side length (stored for frequent calculations)
    Float m_half_side_length;
    // indicates the Z depth of the layer (for parallax scaling)
    Float m_z_depth;
    // indicates if this layer will do wrapped drawing
    bool m_is_wrapped;
}; // end of class Engine2::ObjectLayer

} // end of namespace Engine2

} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_OBJECTLAYER_HPP_)
