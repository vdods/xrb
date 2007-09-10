// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_quadtree.h by Victor Dods, created 2004/06/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_QUADTREEBASE_H_)
#define _XRB_ENGINE2_QUADTREEBASE_H_

#include "xrb.h"

#include <set>

#include "xrb_engine2_object.h"
#include "xrb_engine2_enums.h"
#include "xrb_vector.h"

namespace Xrb
{

namespace Engine2
{

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

    inline QuadTreeType GetQuadTreeType () const { return m_quad_tree_type; }
    inline QuadTree *GetParent () const { return m_parent; }
    inline bool GetHasChildren () const { return m_child[0] != NULL; }
    // returns the root node (top level parent) of this quadtree node
    QuadTree const *GetRootNode () const;
    inline FloatVector2 const &GetCenter () const { return m_center; }
    inline Float GetSideLength () const { return 2.0f * m_half_side_length; }
    inline Float GetHalfSideLength () const { return m_half_side_length; }
    inline Float GetRadius () const { return m_radius; }
    inline Uint32 GetSubordinateObjectCount () const { return m_subordinate_object_count; }
    inline Uint32 GetSubordinateStaticObjectCount () const { return m_subordinate_static_object_count; }
    Object *GetSmallestObjectTouchingPoint (FloatVector2 const &point);
    // TODO: write a wrapped version of GetSmallestObjectTouchingPoint
    inline bool GetIsAllowableObjectRadius (Object const *object) const { return object->GetRadius(GetQuadTreeType()) / m_radius > 0.5f; }

    bool GetDoesAreaOverlapAnyObject (
        FloatVector2 const &area_center,
        Float area_radius) const;
    bool GetDoesAreaOverlapAnyObjectWrapped (
        FloatVector2 const &area_center,
        Float area_radius,
        Float object_layer_side_length,
        Float half_object_layer_side_length) const;

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
    inline QuadTreeClass const *GetChild (Uint32 const index) const
    {
        ASSERT2(index < 4);
        ASSERT2(m_child[index] != NULL);
        return DStaticCast<QuadTreeClass const *>(m_child[index]);
    }
    template <typename QuadTreeClass>
    inline QuadTreeClass *GetChild (Uint32 const index)
    {
        ASSERT2(index < 4);
        ASSERT2(m_child[index] != NULL);
        return DStaticCast<QuadTreeClass *>(m_child[index]);
    }
    // returns true if the given point is inside this quad.
    bool GetIsPointInsideQuad (FloatVector2 const &point) const;
    // returns true if this quad's bounding circle is intersecting the given
    // circle (e.g. used in determining the potential intersecting set of the
    // specified circle)
    inline bool GetDoesAreaOverlapQuadBounds (
        FloatVector2 const &area_center,
        Float const area_radius) const
    {
        ASSERT1(area_radius > 0.0f);
        Float radius_sum = area_radius + 2.0f * GetRadius();
        return (area_center - m_center).GetLengthSquared() < radius_sum*radius_sum;
    }
    // returns true if this quad's bounding circle is intersecting the given
    // circle (e.g. used in determining the potential intersecting set of the
    // specified circle), for wrapped spaces
    bool GetDoesAreaOverlapQuadBoundsWrapped (
        FloatVector2 area_center,
        Float area_radius,
        Float object_layer_side_length,
        Float half_object_layer_side_length) const;

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
    void Initialize (
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

    typedef std::set<Object *> ObjectSet;
    typedef ObjectSet::iterator ObjectSetIterator;
    typedef ObjectSet::const_iterator ObjectSetConstIterator;

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

} // end of namespace Engine2
    
} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_QUADTREEBASE_H_)
