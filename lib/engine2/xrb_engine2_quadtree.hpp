// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_quadtree.hpp by Victor Dods, created 2004/06/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_QUADTREEBASE_HPP_)
#define _XRB_ENGINE2_QUADTREEBASE_HPP_

#include "xrb.hpp"

#include <set>

#include "xrb_engine2_object.hpp"
#include "xrb_engine2_enums.hpp"
#include "xrb_vector.hpp"

namespace Xrb {
namespace Engine2 {

class Object;

// The QuadTree class implements a 2D space-organizing structure known as
// a quad tree.  The idea is to improve visibility/collision calculations
// by eliminating a large number of candidates quickly.  It should be
// noted that it actually uses circle-intersections to determine containment,
// which will give a boost in speed due to the ease of calculating radiuses
// (as opposed to messier (rotated) rectangle intersection calculations).
class QuadTree
{
public:

    virtual ~QuadTree () = 0;

    QuadTreeType GetQuadTreeType () const { return m_quad_tree_type; }
    QuadTree *Parent () const { return m_parent; }
    bool HasChildren () const { return m_child[0] != NULL; }
    // returns the root node (top level parent) of this quadtree node
    QuadTree const *RootNode () const;
    FloatVector2 const &Center () const { return m_center; }
    Float SideLength () const { return 2.0f * m_half_side_length; }
    Float HalfSideLength () const { return m_half_side_length; }
    Float Radius () const { return m_radius; }
    Uint32 SubordinateObjectCount () const { return m_subordinate_object_count; }
    Uint32 SubordinateStaticObjectCount () const { return m_subordinate_static_object_count; }
    Object *SmallestObjectTouchingPoint (FloatVector2 const &point);
    // TODO: write a wrapped version of SmallestObjectTouchingPoint
    bool IsAllowableSizedObject (Object const &object) const { return object.Radius(GetQuadTreeType()) > 0.5f*m_radius; }

    bool DoesAreaOverlapAnyObject (
        FloatVector2 const &area_center,
        Float area_radius,
        ObjectLayer const &object_layer) const;

    // clears out all objects but leaves the QuadTree structure intact
    void Clear ();
    // adds the object to the quadtree.
    // NOTE: any objects passed to this function will be resized if they are
    // larger than the quadnode itself -- they will be resized to have exactly
    // the same radius as the quadnode itself
    bool AddObject (Object *object);
    // removes the object from the quadtree in an efficient manner
    bool RemoveObject (Object *object);
    // moves an object that is already in this quadtree into the
    // correct place.
    bool ReAddObject (Object *object);

protected:

    // for use in Create
    QuadTree (QuadTree *parent);

    template <typename QuadTreeClass>
    QuadTreeClass const *Child (Uint32 index) const
    {
        ASSERT2(index < 4);
        ASSERT2(m_child[index] != NULL);
        return DStaticCast<QuadTreeClass const *>(m_child[index]);
    }
    template <typename QuadTreeClass>
    QuadTreeClass *Child (Uint32 index)
    {
        ASSERT2(index < 4);
        ASSERT2(m_child[index] != NULL);
        return DStaticCast<QuadTreeClass *>(m_child[index]);
    }
    // returns true if the given point is inside this quad.
    bool IsPointInsideQuad (FloatVector2 const &point) const;
    // returns true if this quad's bounding circle is intersecting the given
    // circle (e.g. used in determining the potential intersecting set of the
    // specified circle).  disable_wrapping_adjustment is used when AdjustedDifference
    // and friends should not be used, e.g. when drawing.
    bool DoesAreaOverlapQuadBounds (
        FloatVector2 area_center,
        Float area_radius,
        ObjectLayer const &object_layer,
        bool disable_wrapping_adjustment) const;

    void SetQuadTreeType (QuadTreeType quad_tree_type);

    // increment m_subordinate_object_count at this node and up through all parents
    void IncrementSubordinateObjectCount ();
    // increment m_subordinate_static_object_count at this node and up through all parents
    void IncrementSubordinateStaticObjectCount ();
    // decrement m_subordinate_object_count at this node and up through all parents
    void DecrementSubordinateObjectCount ();
    // decrement m_subordinate_static_object_count at this node and up through all parents
    void DecrementSubordinateStaticObjectCount ();

    template <typename QuadTreeClass>
    void Initialize (FloatVector2 const &center, Float const half_side_length, Uint8 const depth);

    typedef std::set<Object *> ObjectSet;

    // list of objects for this node
    ObjectSet m_object_set;
    // one child for each quadrant
    // 1st quadrant (X non-negative, Y non-negative)
    // 2nd quadrant (X non-positive, Y non-negative)
    // 3rd quadrant (X non-positive, Y non-positive)
    // 4th quadrant (X non-negative, Y non-positive)
    QuadTree *m_child[4];
    // parent node
    QuadTree *m_parent;
    // the center of the node
    FloatVector2 m_center;
    // the length of half of the edge of the quad node
    Float m_half_side_length;
    // the radius of this quad node
    Float m_radius;

private:

    void NonRecursiveAddObject (Object *object);
    void AddObjectIfNotAlreadyAdded (Object *object);

    // number of objects this quad node and all its children contain
    Uint32 m_subordinate_object_count;
    // number of non-entities this quad node and all its children contain
    Uint32 m_subordinate_static_object_count;
    // the quad_tree_type of quadtree this is (the index into
    // Engine2::Object::m_owner_quad_tree that stores a pointer to this
    // quadtree
    QuadTreeType m_quad_tree_type;
}; // end of class Engine2::QuadTree

template <typename QuadTreeClass>
void QuadTree::Initialize (
    FloatVector2 const &center,
    Float const half_side_length,
    Uint8 const depth)
{
    ASSERT1(half_side_length > 0.0f);
    ASSERT1(depth != 0);

    m_parent = NULL;
    m_center = center;
    m_half_side_length = half_side_length;
    m_radius = Math::Sqrt(2.0f) * m_half_side_length;
    m_subordinate_object_count = 0;
    m_subordinate_static_object_count = 0;
    if (depth <= 1)
    {
        for (Uint8 i = 0; i < 4; ++i)
            m_child[i] = NULL;
    }
    else
    {
        Float child_half_side_length = 0.5f * m_half_side_length;

        m_child[0] = new QuadTreeClass(
            center + child_half_side_length * FloatVector2( 1.0f,  1.0f),
            child_half_side_length,
            depth - 1);
        m_child[1] = new QuadTreeClass(
            center + child_half_side_length * FloatVector2(-1.0f,  1.0f),
            child_half_side_length,
            depth - 1);
        m_child[2] = new QuadTreeClass(
            center + child_half_side_length * FloatVector2(-1.0f, -1.0f),
            child_half_side_length,
            depth - 1);
        m_child[3] = new QuadTreeClass(
            center + child_half_side_length * FloatVector2( 1.0f, -1.0f),
            child_half_side_length,
            depth - 1);

        for (Uint8 i = 0; i < 4; ++i)
            m_child[i]->m_parent = this;
    }
}

} // end of namespace Engine2
} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_QUADTREEBASE_HPP_)
