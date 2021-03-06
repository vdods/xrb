// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_quadtree.cpp by Victor Dods, created 2004/06/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_quadtree.hpp"

#include "xrb_engine2_entity.hpp"
#include "xrb_engine2_objectlayer.hpp"

namespace Xrb {
namespace Engine2 {

QuadTree::~QuadTree ()
{
    // delete all the objects in this node's list
    for (ObjectSet::iterator it = m_object_set.begin(),
                           it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL);
        Delete(object);
    }
    m_object_set.clear();

    if (HasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            Delete(m_child[i]);
}

QuadTree const *QuadTree::RootNode () const
{
    QuadTree const *quad_node = this;
    while (quad_node->m_parent != NULL)
        quad_node = quad_node->m_parent;
    return quad_node;
}

Object *QuadTree::SmallestObjectTouchingPoint (
    FloatVector2 const &point)
{
    Object *retval = NULL;

    // if this node contains no objects, early out
    if (SubordinateObjectCount() == 0)
        return retval;

    // if the point is outside the reaches of this quad node, early out
    if ((point - m_center).LengthSquared() > 4.0*m_radius*m_radius)
        return retval;

    Object *smallest_candidate;

    // get the smallest object from the children
    if (m_child[0] != NULL)
    {
        for (Uint8 i = 0; i < 4; ++i)
        {
            ASSERT2(m_child[i] != NULL);
            smallest_candidate = m_child[i]->SmallestObjectTouchingPoint(point);
            if (retval == NULL ||
                (smallest_candidate != NULL &&
                 smallest_candidate->Radius(GetQuadTreeType()) < retval->Radius(GetQuadTreeType())))
                retval = smallest_candidate;
        }
    }

    // check if the current object is smaller than the minimum allowable size.
    // if it is, return it, because no objects owned by this node will be
    // smaller than it
    if (retval != NULL && !IsAllowableSizedObject(*retval))
        return retval;

    // check against all the objects owned by this node
    for (ObjectSet::iterator it = m_object_set.begin(),
                           it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        smallest_candidate = *it;
        ASSERT1(smallest_candidate != NULL);

        // if the point is touching the object
        if ((point - smallest_candidate->Translation()).LengthSquared() <= Sqr(smallest_candidate->Radius(GetQuadTreeType())))
            // and either retval is null, or smallest_candidate is smaller
            if (retval == NULL ||
                smallest_candidate->Radius(GetQuadTreeType()) < retval->Radius(GetQuadTreeType()))
                // assign it as the smallest
                retval = smallest_candidate;
    }

    return retval;
}

bool QuadTree::DoesAreaOverlapAnyObject (
    FloatVector2 const &area_center,
    Float area_radius,
    ObjectLayer const &object_layer) const
{
    // if there are no objects here or below, just return false
    if (SubordinateObjectCount() == 0)
        return false;

    // return false if the area doesn't intersect this node
    if (!DoesAreaOverlapQuadBounds(area_center, area_radius, object_layer, false))
        return false;

    // check if the area overlaps any object in this node's list.
    for (ObjectSet::const_iterator it = m_object_set.begin(),
                                   it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Object const *object = *it;
        ASSERT1(object != NULL);
        ASSERT1(object->OwnerQuadTree(m_quad_tree_type) == this);
        if (object_layer.AdjustedDistance(object->Translation(), area_center)
            <
            (object->Radius(GetQuadTreeType()) + area_radius))
            return true;
    }

    // if there are child nodes, call this method on each until one returns true
    if (HasChildren())
    {
        bool retval = false;
        for (Uint32 i = 0; i < 4; ++i)
            retval = retval ||
                m_child[i]->DoesAreaOverlapAnyObject(area_center, area_radius, object_layer);
        return retval;
    }
    else
        return false;
}

void QuadTree::Clear ()
{
    // clear the object list
    m_object_set.clear();
    // if there are children, clear the children
    if (HasChildren())
    {
        for (Uint8 i = 0; i < 4; ++i)
        {
            ASSERT2(m_child[i] != NULL);
            m_child[i]->Clear();
        }
    }
}

bool QuadTree::AddObject (Object *object)
{
    ASSERT1(object != NULL);
    ASSERT1(object->OwnerQuadTree(m_quad_tree_type) == NULL);

    // range checking -- an object can't have a larger radius
    // than the quadnode that owns it
    ASSERT1(object->Radius(GetQuadTreeType()) <= m_radius);

    // return if the object's center is not inside this node
    if (!IsPointInsideQuad(object->Translation()))
        return false;

    // check if the object should be added to this node:
    // add to the object list if the object's radius is within the nominal size
    // for this quadtree's radius or if there are no child quadtree nodes.
    if (IsAllowableSizedObject(*object) || !HasChildren())
    {
        // add to this node
        object->SetOwnerQuadTree(m_quad_tree_type, this);
        m_object_set.insert(object);
        ++m_subordinate_object_count;
        if (!object->IsDynamic())
            ++m_subordinate_static_object_count;
    }
    else
    {
        for (Uint8 i = 0; i < 4; ++i)
            ASSERT2(m_child[i] != NULL);
        // add it to only one child quadtree
        bool add_succeeded = false;
        for (Uint8 i = 0; i < 4; ++i)
        {
            add_succeeded = add_succeeded || m_child[i]->AddObject(object);
            // we only want to add it to one
            if (add_succeeded)
                break;
        }
        ASSERT1(add_succeeded);

        ++m_subordinate_object_count;
        if (!object->IsDynamic())
            ++m_subordinate_static_object_count;
    }
    return true;
}

bool QuadTree::RemoveObject (Object *const object)
{
    ASSERT1(object != NULL);
    ASSERT1(object->OwnerQuadTree(m_quad_tree_type) == this);

    ObjectSet::iterator it = m_object_set.find(object);
    if (it != m_object_set.end())
    {
        // remove the object from the object set of its owner
        object->OwnerQuadTree(m_quad_tree_type)->m_object_set.erase(it);
        // set the object to un-owned
        object->SetOwnerQuadTree(m_quad_tree_type, NULL);
        // decrement subordinate object count
        DecrementSubordinateObjectCount();
        // decrement the subordinate static object count if appropriate
        if (!object->IsDynamic())
            DecrementSubordinateStaticObjectCount();
        // success
        return true;
    }
    else
    {
        // failure
        return false;
    }
}

bool QuadTree::ReAddObject (Object *const object)
{
    ASSERT1(object != NULL);
    ASSERT1(object->OwnerQuadTree(m_quad_tree_type) != NULL);

    bool object_was_added = false;

    // if the object's position is inside current quadnode
    if (IsPointInsideQuad(object->Translation()))
    {
        Float object_radius_over_quad_radius = object->Radius(GetQuadTreeType()) / m_radius;
        // if the object is too big for current quadnode
        if (object_radius_over_quad_radius > 1.0f)
        {
            if (m_parent != NULL)
            {
                // traverse to the parent
                object_was_added = m_parent->ReAddObject(object);
                ASSERT1(object_was_added);
            }
            else
            {
                // add it to this quadnode
                AddObjectIfNotAlreadyAdded(object);
                object_was_added = true;
            }
        }
        // otherwise if the object is too small for current quadnode
        else if (object_radius_over_quad_radius <= 0.5f)
        {
            if (HasChildren())
            {
                FloatVector2 object_translation(object->Translation());
                if (object_translation[Dim::X] >= m_center[Dim::X])
                {
                    if (object_translation[Dim::Y] >= m_center[Dim::Y])
                    {
                        ASSERT1(m_child[0]->IsPointInsideQuad(object->Translation()));
                        object_was_added = m_child[0]->ReAddObject(object);
                    }
                    else
                    {
                        ASSERT1(m_child[3]->IsPointInsideQuad(object->Translation()));
                        object_was_added = m_child[3]->ReAddObject(object);
                    }
                }
                else
                {
                    if (object_translation[Dim::Y] >= m_center[Dim::Y])
                    {
                        ASSERT1(m_child[1]->IsPointInsideQuad(object->Translation()));
                        object_was_added = m_child[1]->ReAddObject(object);
                    }
                    else
                    {
                        ASSERT1(m_child[2]->IsPointInsideQuad(object->Translation()));
                        object_was_added = m_child[2]->ReAddObject(object);
                    }
                }
                ASSERT1(object_was_added);
            }
            else
            {
                // add it to this quadnode
                AddObjectIfNotAlreadyAdded(object);
                object_was_added = true;
            }
        }
        // otherwise it's at the right location and size
        else
        {
            // add it
            AddObjectIfNotAlreadyAdded(object);
            object_was_added = true;
        }
    }
    // traverse up to the parent
    else
    {
        if (m_parent != NULL)
        {
            // traverse to the parent
            object_was_added = m_parent->ReAddObject(object);
        }
        else
        {
            ASSERT1(object_was_added == false);
            std::cerr << object->Translation() << std::endl; // HIPPO
            ASSERT1(false);
            // error - nowhere to go
        }
    }

    ASSERT1(object_was_added);
    return object_was_added;
}

QuadTree::QuadTree (QuadTree *const parent)
{
    m_parent = parent;
    m_half_side_length = 0.0f;
    m_radius = 0.0f;
    for (Uint8 i = 0; i < 4; ++i)
        m_child[i] = NULL;
    m_subordinate_object_count = 0;
    m_subordinate_static_object_count = 0;
    m_quad_tree_type = QTT_COUNT;
}

bool QuadTree::IsPointInsideQuad (FloatVector2 const &point) const
{
    ASSERT1(m_half_side_length > 0.0);

    if (point[Dim::X] < m_center[Dim::X] - m_half_side_length)
        return false;
    else if (point[Dim::X] > m_center[Dim::X] + m_half_side_length)
        return false;
    else if (point[Dim::Y] < m_center[Dim::Y] - m_half_side_length)
        return false;
    else if (point[Dim::Y] > m_center[Dim::Y] + m_half_side_length)
        return false;
    else
        return true;
}

bool QuadTree::DoesAreaOverlapQuadBounds (
    FloatVector2 area_center,
    Float area_radius,
    ObjectLayer const &object_layer,
    bool disable_wrapping_adjustment) const
{
    ASSERT1(area_radius > 0.0f);
    Float radius_sum = area_radius + 2.0f * Radius();
    if (disable_wrapping_adjustment)
        return (area_center - m_center).LengthSquared() < Sqr(radius_sum);
    else
        return object_layer.AdjustedDistanceSquared(area_center, m_center) < Sqr(radius_sum);
}

void QuadTree::SetQuadTreeType (QuadTreeType const quad_tree_type)
{
    ASSERT1(quad_tree_type < QTT_COUNT);
    m_quad_tree_type = quad_tree_type;
    if (HasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            m_child[i]->SetQuadTreeType(m_quad_tree_type);
}

void QuadTree::IncrementSubordinateObjectCount ()
{
    QuadTree *quad_node = this;
    while (quad_node != NULL)
    {
        ASSERT3(quad_node->m_subordinate_object_count < UINT32_UPPER_BOUND);
        ++quad_node->m_subordinate_object_count;
        quad_node = quad_node->m_parent;
    }
}

void QuadTree::IncrementSubordinateStaticObjectCount ()
{
    QuadTree *quad_node = this;
    while (quad_node != NULL)
    {
        ASSERT3(quad_node->m_subordinate_static_object_count < UINT32_UPPER_BOUND);
        ++quad_node->m_subordinate_static_object_count;
        quad_node = quad_node->m_parent;
    }
}

void QuadTree::DecrementSubordinateObjectCount ()
{
    QuadTree *quad_node = this;
    while (quad_node != NULL)
    {
        ASSERT3(quad_node->m_subordinate_object_count > 0);
        --quad_node->m_subordinate_object_count;
        quad_node = quad_node->m_parent;
    }
}

void QuadTree::DecrementSubordinateStaticObjectCount ()
{
    QuadTree *quad_node = this;
    while (quad_node != NULL)
    {
        ASSERT3(quad_node->m_subordinate_static_object_count > 0);
        --quad_node->m_subordinate_static_object_count;
        quad_node = quad_node->m_parent;
    }
}

void QuadTree::NonRecursiveAddObject (Object *object)
{
    ASSERT1(object != NULL);
    ASSERT1(object->OwnerQuadTree(m_quad_tree_type) == NULL);

    // do range adjusting -- an object can't have a larger radius
    // than the quadnode that owns it
    if (object->Radius(GetQuadTreeType()) > m_radius)
        object->SetScaleFactor(m_radius);

    ASSERT1(IsPointInsideQuad(object->Translation()));

    // check that the object should be added to this node:
    // assert that the object's radius is within the nominal size
    // for this quadtree's radius or that there are no child quadtree nodes.
    ASSERT1(IsAllowableSizedObject(*object) || !HasChildren());
    // add to this node
    object->SetOwnerQuadTree(m_quad_tree_type, this);
    m_object_set.insert(object);
    IncrementSubordinateObjectCount();
    if (!object->IsDynamic())
        IncrementSubordinateStaticObjectCount();
}

void QuadTree::AddObjectIfNotAlreadyAdded (Object *const object)
{
    ASSERT1(object != NULL);
    ASSERT1(object->OwnerQuadTree(m_quad_tree_type) != NULL);

    // only move the object to this quadnode if it's not already here.
    if (object->OwnerQuadTree(m_quad_tree_type) != this)
    {
        object->OwnerQuadTree(m_quad_tree_type)->RemoveObject(object);
        NonRecursiveAddObject(object);
    }
}

} // end of namespace Engine2
} // end of namespace Xrb
