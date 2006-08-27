// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_quadtree.cpp by Victor Dods, created 2004/06/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_quadtree.h"

#include "xrb_engine2_entity.h"

namespace Xrb
{

Engine2::QuadTree::~QuadTree ()
{
    // delete all the objects in this node's list
    for (ObjectSetIterator it = m_object_set.begin(),
                           it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Object *object = *it;
        ASSERT1(object != NULL)
        Delete(object);
    }
    m_object_set.clear();

    if (GetHasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            Delete(m_child[i]);
}

Engine2::QuadTree const *Engine2::QuadTree::GetRootNode () const
{
    QuadTree const *quad_node = this;
    while (quad_node->m_parent != NULL)
        quad_node = quad_node->m_parent;
    return quad_node;
}

Engine2::Object *Engine2::QuadTree::GetSmallestObjectTouchingPoint (
    FloatVector2 const &point)
{
    Object *retval = NULL;

    // if this node contains no objects, early out
    if (GetSubordinateObjectCount() == 0)
        return retval;

    // if the point is outside the reaches of this quad node, early out
    if ((point - m_center).GetLengthSquared() > 4.0*m_radius*m_radius)
        return retval;

    Object *smallest_candidate;

    // get the smallest object from the children
    if (m_child[0] != NULL)
    {
        for (Uint8 i = 0; i < 4; ++i)
        {
            ASSERT2(m_child[i] != NULL)
            smallest_candidate = m_child[i]->GetSmallestObjectTouchingPoint(point);
            if (retval == NULL ||
                smallest_candidate != NULL &&
                smallest_candidate->GetRadius(GetQuadTreeType()) < retval->GetRadius(GetQuadTreeType()))
                retval = smallest_candidate;
        }
    }

    // check if the current object is smaller than the minimum allowable size.
    // if it is, return it, because no objects owned by this node will be
    // smaller than it
    if (retval != NULL && !GetIsAllowableObjectRadius(retval))
        return retval;

    // check against all the objects owned by this node
    for (ObjectSetIterator it = m_object_set.begin(),
                           it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        smallest_candidate = *it;
        ASSERT1(smallest_candidate != NULL)

        // if the point is touching the object
        if ((point - smallest_candidate->GetTranslation()).GetLengthSquared() <=
            smallest_candidate->GetRadiusSquared(GetQuadTreeType()))
            // and either retval is null, or smallest_candidate is smaller
            if (retval == NULL ||
                smallest_candidate->GetRadius(GetQuadTreeType()) < retval->GetRadius(GetQuadTreeType()))
                // assign it as the smallest
                retval = smallest_candidate;
    }

    return retval;
}

bool Engine2::QuadTree::GetDoesAreaOverlapAnyObject (
    FloatVector2 const &area_center,
    Float const area_radius) const
{
    // if there are no objects here or below, just return false
    if (GetSubordinateObjectCount() == 0)
        return false;

    // return false if the area doesn't intersect this node
    if (!GetDoesAreaOverlapQuadBounds(area_center, area_radius))
        return false;

    // check if the area overlaps any object in this node's list.
    for (ObjectSetConstIterator it = m_object_set.begin(),
                                it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Object const *object = *it;
        ASSERT1(object != NULL)
        ASSERT1(object->GetOwnerQuadTree(m_quad_tree_type) == this)
        if ((object->GetTranslation() - area_center).GetLength()
            <
            (object->GetRadius(GetQuadTreeType()) + area_radius))
            return true;
    }

    // if there are child nodes, call this method on each until one returns true
    if (GetHasChildren())
        return
            m_child[0]->GetDoesAreaOverlapAnyObject(
                area_center,
                area_radius)
            ||
            m_child[1]->GetDoesAreaOverlapAnyObject(
                area_center,
                area_radius)
            ||
            m_child[2]->GetDoesAreaOverlapAnyObject(
                area_center,
                area_radius)
            ||
            m_child[3]->GetDoesAreaOverlapAnyObject(
                area_center,
                area_radius);
    else
        return false;
}

bool Engine2::QuadTree::GetDoesAreaOverlapAnyObjectWrapped (
    FloatVector2 const &area_center,
    Float const area_radius,
    Float const object_layer_side_length,
    Float const half_object_layer_side_length) const
{
    // if there are no objects here or below, just return false
    if (GetSubordinateObjectCount() == 0)
        return false;

    // return false if the area doesn't intersect this node
    if (!GetDoesAreaOverlapQuadBoundsWrapped(
            area_center,
            area_radius,
            object_layer_side_length,
            half_object_layer_side_length))
        return false;

    // check if the area overlaps any object in this node's list.
    for (ObjectSetConstIterator it = m_object_set.begin(),
                                it_end = m_object_set.end();
         it != it_end;
         ++it)
    {
        Object const *object = *it;
        ASSERT1(object != NULL)
        ASSERT1(object->GetOwnerQuadTree(m_quad_tree_type) == this)

        FloatVector2 object_translation(object->GetTranslation());
        FloatVector2 adjusted_area_center(area_center);

        while (adjusted_area_center[Dim::X] < object_translation[Dim::X] - half_object_layer_side_length)
            adjusted_area_center[Dim::X] += object_layer_side_length;
        while (adjusted_area_center[Dim::X] > object_translation[Dim::X] + half_object_layer_side_length)
            adjusted_area_center[Dim::X] -= object_layer_side_length;

        while (adjusted_area_center[Dim::Y] < object_translation[Dim::Y] - half_object_layer_side_length)
            adjusted_area_center[Dim::Y] += object_layer_side_length;
        while (adjusted_area_center[Dim::Y] > object_translation[Dim::Y] + half_object_layer_side_length)
            adjusted_area_center[Dim::Y] -= object_layer_side_length;

        if ((object_translation - adjusted_area_center).GetLength()
            <
            (object->GetRadius(GetQuadTreeType()) + area_radius))
            return true;
    }

    // if there are child nodes, call this method on each until one returns true
    if (GetHasChildren())
        return
            m_child[0]->GetDoesAreaOverlapAnyObjectWrapped(
                area_center,
                area_radius,
                object_layer_side_length,
                half_object_layer_side_length)
            ||
            m_child[1]->GetDoesAreaOverlapAnyObjectWrapped(
                area_center,
                area_radius,
                object_layer_side_length,
                half_object_layer_side_length)
            ||
            m_child[2]->GetDoesAreaOverlapAnyObjectWrapped(
                area_center,
                area_radius,
                object_layer_side_length,
                half_object_layer_side_length)
            ||
            m_child[3]->GetDoesAreaOverlapAnyObjectWrapped(
                area_center,
                area_radius,
                object_layer_side_length,
                half_object_layer_side_length);
    else
        return false;
}

void Engine2::QuadTree::Clear ()
{
    // clear the object list
    m_object_set.clear();
    // if there are children, clear the children
    if (GetHasChildren())
    {
        for (Uint8 i = 0; i < 4; ++i)
        {
            ASSERT2(m_child[i] != NULL)
            m_child[i]->Clear();
        }
    }
}

bool Engine2::QuadTree::AddObject (Engine2::Object *const object)
{
    ASSERT1(object != NULL)
    ASSERT1(object->GetOwnerQuadTree(m_quad_tree_type) == NULL)

    // range checking -- an object can't have a larger radius
    // than the quadnode that owns it
    ASSERT1(object->GetRadius(GetQuadTreeType()) <= m_radius)

    // return if the object's center is not inside this node
    if (!GetIsPointInsideQuad(object->GetTranslation()))
        return false;

    // check if the object should be added to this node:
    // add to the object list if the object's radius is within the nominal size
    // for this quadtree's radius or if there are no child quadtree nodes.
    if (GetIsAllowableObjectRadius(object) || !GetHasChildren())
    {
        // add to this node
        object->SetOwnerQuadTree(m_quad_tree_type, this);
        m_object_set.insert(object);
        ++m_subordinate_object_count;
        if (!object->GetIsDynamic())
            ++m_subordinate_static_object_count;
    }
    else
    {
        for (Uint8 i = 0; i < 4; ++i)
            ASSERT2(m_child[i] != NULL)
        // add it to only one child quadtree
        bool add_succeeded = false;
        for (Uint8 i = 0; i < 4; ++i)
        {
            add_succeeded = add_succeeded || m_child[i]->AddObject(object);
            // we only want to add it to one
            if (add_succeeded)
                break;
        }
        ASSERT1(add_succeeded)

        ++m_subordinate_object_count;
        if (!object->GetIsDynamic())
            ++m_subordinate_static_object_count;
    }
    return true;
}

bool Engine2::QuadTree::RemoveObject (Engine2::Object *const object)
{
    ASSERT1(object != NULL)
    ASSERT1(object->GetOwnerQuadTree(m_quad_tree_type) == this)

    ObjectSetIterator it = m_object_set.find(object);
    if (it != m_object_set.end())
    {
        // remove the object from the object set of its owner
        object->GetOwnerQuadTree(m_quad_tree_type)->m_object_set.erase(it);
        // set the object to un-owned
        object->SetOwnerQuadTree(m_quad_tree_type, NULL);
        // decrement subordinate object count
        DecrementSubordinateObjectCount();
        // decrement the subordinate static object count if appropriate
        if (!object->GetIsDynamic())
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

bool Engine2::QuadTree::ReAddObject (Engine2::Object *const object)
{
    ASSERT1(object != NULL)
    ASSERT1(object->GetOwnerQuadTree(m_quad_tree_type) != NULL)

    bool object_was_added = false;

    // if the object's position is inside current quadnode
    if (GetIsPointInsideQuad(object->GetTranslation()))
    {
        Float object_radius_over_quad_radius = object->GetRadius(GetQuadTreeType()) / m_radius;
        // if the object is too big for current quadnode
        if (object_radius_over_quad_radius > 1.0f)
        {
            if (m_parent != NULL)
            {
                // traverse to the parent
                object_was_added = m_parent->ReAddObject(object);
                ASSERT1(object_was_added)
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
            if (GetHasChildren())
            {
                FloatVector2 object_translation(object->GetTranslation());
                if (object_translation[Dim::X] >= m_center[Dim::X])
                {
                    if (object_translation[Dim::Y] >= m_center[Dim::Y])
                    {
                        ASSERT1(m_child[0]->GetIsPointInsideQuad(object->GetTranslation()))
                        object_was_added = m_child[0]->ReAddObject(object);
                    }
                    else
                    {
                        ASSERT1(m_child[3]->GetIsPointInsideQuad(object->GetTranslation()))
                        object_was_added = m_child[3]->ReAddObject(object);
                    }
                }
                else
                {
                    if (object_translation[Dim::Y] >= m_center[Dim::Y])
                    {
                        ASSERT1(m_child[1]->GetIsPointInsideQuad(object->GetTranslation()))
                        object_was_added = m_child[1]->ReAddObject(object);
                    }
                    else
                    {
                        ASSERT1(m_child[2]->GetIsPointInsideQuad(object->GetTranslation()))
                        object_was_added = m_child[2]->ReAddObject(object);
                    }
                }
                ASSERT1(object_was_added)
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
            ASSERT1(object_was_added == false)
            Fprint(stderr, object->GetTranslation());
            ASSERT1(false)
            // error - nowhere to go
        }
    }

    ASSERT1(object_was_added)
    return object_was_added;
}

Engine2::QuadTree::QuadTree (QuadTree *const parent)
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

bool Engine2::QuadTree::GetIsPointInsideQuad (FloatVector2 const &point) const
{
    ASSERT1(m_half_side_length > 0.0)

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

bool Engine2::QuadTree::GetDoesAreaOverlapQuadBoundsWrapped (
    FloatVector2 area_center,
    Float const area_radius,
    Float const object_layer_side_length,
    Float const half_object_layer_side_length) const
{
    ASSERT1(area_center[Dim::X] >= -half_object_layer_side_length)
    ASSERT1(area_center[Dim::X] <=  half_object_layer_side_length)
    ASSERT1(area_center[Dim::Y] >= -half_object_layer_side_length)
    ASSERT1(area_center[Dim::Y] <=  half_object_layer_side_length)

    if (area_center[Dim::X] < m_center[Dim::X] - half_object_layer_side_length)
        area_center[Dim::X] += object_layer_side_length;
    else if (area_center[Dim::X] > m_center[Dim::X] + half_object_layer_side_length)
        area_center[Dim::X] -= object_layer_side_length;

    if (area_center[Dim::Y] < m_center[Dim::Y] - half_object_layer_side_length)
        area_center[Dim::Y] += object_layer_side_length;
    else if (area_center[Dim::Y] > m_center[Dim::Y] + half_object_layer_side_length)
        area_center[Dim::Y] -= object_layer_side_length;

    return GetDoesAreaOverlapQuadBounds(area_center, area_radius);
}

void Engine2::QuadTree::SetQuadTreeType (QuadTreeType const quad_tree_type)
{
    ASSERT1(quad_tree_type < QTT_COUNT)
    m_quad_tree_type = quad_tree_type;
    if (GetHasChildren())
        for (Uint8 i = 0; i < 4; ++i)
            m_child[i]->SetQuadTreeType(m_quad_tree_type);
}

void Engine2::QuadTree::IncrementSubordinateObjectCount ()
{
    QuadTree *quad_node = this;
    while (quad_node != NULL)
    {
        ASSERT3(quad_node->m_subordinate_object_count < UINT32_UPPER_BOUND)
        ++quad_node->m_subordinate_object_count;
        quad_node = quad_node->m_parent;
    }
}

void Engine2::QuadTree::IncrementSubordinateStaticObjectCount ()
{
    QuadTree *quad_node = this;
    while (quad_node != NULL)
    {
        ASSERT3(quad_node->m_subordinate_static_object_count < UINT32_UPPER_BOUND)
        ++quad_node->m_subordinate_static_object_count;
        quad_node = quad_node->m_parent;
    }
}

void Engine2::QuadTree::DecrementSubordinateObjectCount ()
{
    QuadTree *quad_node = this;
    while (quad_node != NULL)
    {
        ASSERT3(quad_node->m_subordinate_object_count > 0)
        --quad_node->m_subordinate_object_count;
        quad_node = quad_node->m_parent;
    }
}

void Engine2::QuadTree::DecrementSubordinateStaticObjectCount ()
{
    QuadTree *quad_node = this;
    while (quad_node != NULL)
    {
        ASSERT3(quad_node->m_subordinate_static_object_count > 0)
        --quad_node->m_subordinate_static_object_count;
        quad_node = quad_node->m_parent;
    }
}

void Engine2::QuadTree::NonRecursiveAddObject (Object *const object)
{
    ASSERT1(object != NULL)
    ASSERT1(object->GetOwnerQuadTree(m_quad_tree_type) == NULL)

    // do range adjusting -- an object can't have a larger radius
    // than the quadnode that owns it
    if (object->GetRadius(GetQuadTreeType()) > m_radius)
        object->SetScaleFactor(m_radius);

    ASSERT1(GetIsPointInsideQuad(object->GetTranslation()))

    // check that the object should be added to this node:
    // assert that the object's radius is within the nominal size
    // for this quadtree's radius or that there are no child quadtree nodes.
    ASSERT1(GetIsAllowableObjectRadius(object) || !GetHasChildren())
    // add to this node
    object->SetOwnerQuadTree(m_quad_tree_type, this);
    m_object_set.insert(object);
    IncrementSubordinateObjectCount();
    if (!object->GetIsDynamic())
        IncrementSubordinateStaticObjectCount();
}

void Engine2::QuadTree::AddObjectIfNotAlreadyAdded (
    Engine2::Object *const object)
{
    ASSERT1(object != NULL)
    ASSERT1(object->GetOwnerQuadTree(m_quad_tree_type) != NULL)

    // only move the object to this quadnode if it's not already here.
    if (object->GetOwnerQuadTree(m_quad_tree_type) != this)
    {
        object->GetOwnerQuadTree(m_quad_tree_type)->RemoveObject(object);
        NonRecursiveAddObject(object);
    }
}

} // end of namespace Xrb
