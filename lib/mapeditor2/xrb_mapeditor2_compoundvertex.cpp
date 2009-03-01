// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_compoundvertex.cpp by Victor Dods, created 2005/07/31
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_mapeditor2_compoundvertex.hpp"

#include "xrb_mapeditor2_objectlayer.hpp"

namespace Xrb
{

void MapEditor2::CompoundVertex::SetSelectionStateFromSelectionOwnerPolygonCount (
    ObjectLayer *const object_layer)
{
    ASSERT1(object_layer != NULL);
    SetIsSelected(m_selected_owner_polygon_count > 0, object_layer);
}

void MapEditor2::CompoundVertex::IncrementSelectedOwnerPolygonCount (
    ObjectLayer *const object_layer)
{
    ASSERT1(object_layer != NULL);
    ASSERT1(m_selected_owner_polygon_count < UINT32_UPPER_BOUND);
    if (m_selected_owner_polygon_count == 0)
        SetIsSelected(true, object_layer);
    ++m_selected_owner_polygon_count;
}

void MapEditor2::CompoundVertex::DecrementSelectedOwnerPolygonCount (
    ObjectLayer *const object_layer)
{
    ASSERT1(object_layer != NULL);
    ASSERT1(
        m_selected_owner_polygon_count > 0 &&
        "If this assert failed while welding vertices, it's likely "
        "that the polygon selection set wasn't cleared beforehand.");
    if (m_selected_owner_polygon_count == 1)
        SetIsSelected(false, object_layer);
    --m_selected_owner_polygon_count;
}

void MapEditor2::CompoundVertex::SetIsSelected (
    bool const is_selected,
    ObjectLayer *const object_layer)
{
    if (m_is_selected != is_selected)
    {
        if (is_selected)
            AddToVertexSelectionSet(object_layer);
        else
            RemoveFromVertexSelectionSet(object_layer);
        ASSERT1(m_is_selected == is_selected);
    }
}

void MapEditor2::CompoundVertex::ToggleIsSelected (
    ObjectLayer *const object_layer)
{
    DEBUG1_CODE(bool old_is_selected = m_is_selected);
    if (m_is_selected)
        RemoveFromVertexSelectionSet(object_layer);
    else
        AddToVertexSelectionSet(object_layer);
    ASSERT1(m_is_selected != old_is_selected);
}

void MapEditor2::CompoundVertex::AddToVertexSelectionSet (
    ObjectLayer *const object_layer)
{
    ASSERT1(object_layer != NULL);
    object_layer->AddVertexToVertexSelectionSet(this);
}

void MapEditor2::CompoundVertex::RemoveFromVertexSelectionSet (
    ObjectLayer *const object_layer)
{
    ASSERT1(object_layer != NULL);
    object_layer->RemoveVertexFromVertexSelectionSet(this);
}

} // end of namespace Xrb
