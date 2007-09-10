// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_compoundvertex.h by Victor Dods, created 2005/07/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_MAPEDITOR2_COMPOUNDVERTEX_H_)
#define _XRB_MAPEDITOR2_COMPOUNDVERTEX_H_

#include "xrb.h"

#include "xrb_vector.h"

namespace Xrb
{

namespace MapEditor2
{

    class Compound;
    class ObjectLayer;

    struct CompoundVertex
    {
        // the spatial vertex coordinate.
        FloatVector2 m_coordinate;
        // this index is meaningless until the VertexList is traversed
        // and indices assigned to each Vertex.
        mutable Uint32 m_index;
        // the Compound that owns this vertex.
        Compound *m_owner_compound;
        // indicates if this vertex is currently selected.
        bool m_is_selected;
        // the number of owner Polygons that are currently selected
        Uint32 m_selected_owner_polygon_count;
        
        inline CompoundVertex (Compound *const owner_compound)
        {
            ASSERT1(owner_compound != NULL);
            m_index = UINT32_UPPER_BOUND;
            m_owner_compound = owner_compound;
            m_is_selected = false;
            m_selected_owner_polygon_count = 0;
        }

        void SetSelectionStateFromSelectionOwnerPolygonCount (ObjectLayer *object_layer);
        
        void IncrementSelectedOwnerPolygonCount (ObjectLayer *object_layer);
        void DecrementSelectedOwnerPolygonCount (ObjectLayer *object_layer);
        
        void SetIsSelected (bool is_selected, ObjectLayer *object_layer);
        void ToggleIsSelected (ObjectLayer *object_layer);
        void AddToVertexSelectionSet (ObjectLayer *object_layer);
        void RemoveFromVertexSelectionSet (ObjectLayer *object_layer);
    }; // end of MapEditor2::CompoundVertex

} // end of namespace MapEditor2

} // end of namespace Xrb

#endif // !defined(_XRB_MAPEDITOR2_COMPOUNDVERTEX_H_)

