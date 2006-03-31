// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_visibilityquadtree.h by Victor Dods, created 2005/03/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_MAPEDITOR2_VISIBILITYQUADTREE_H_)
#define _XRB_MAPEDITOR2_VISIBILITYQUADTREE_H_

#include "xrb.h"

#include "xrb_mapeditor2_object.h"
#include "xrb_engine2_visibilityquadtree.h"

namespace Xrb
{

namespace MapEditor2
{

    class Compound;
    struct Polygon;
    struct CompoundVertex;

    class VisibilityQuadTree : public Engine2::VisibilityQuadTree
    {
    public:
    
        VisibilityQuadTree (
            FloatVector2 const &center,
            Float half_side_length,
            Uint8 depth);
        virtual ~VisibilityQuadTree () { }
    
        static VisibilityQuadTree *Create (Serializer &serializer);
    
        inline Object *GetSmallestMapEditorObjectTouchingPoint (
            FloatVector2 const &point)
        {
            Engine2::Object *object = GetSmallestObjectTouchingPoint(point);
            Object *retval =
                dynamic_cast<Object *>(object);
            ASSERT1(object == NULL || retval != NULL)
            return retval;
        }
        Polygon *GetSmallestMapEditorPolygonTouchingPoint (
            FloatVector2 const &point,
            bool mask_by_object_selection_set);
    
        virtual void ReadStructure (Serializer &serializer);
        virtual void ReadObjects (
            Serializer &serializer,
            ObjectLayer *object_layer);
    
        void DrawMetrics (
            RenderContext const &render_context,
            FloatMatrix2 const &world_to_screen,
            Float pixels_in_view_radius,
            FloatVector2 const &view_center,
            Float view_radius,
            Object::MetricMode metric_mode);
        void DrawMetricsWrapped (
            RenderContext const &render_context,
            FloatMatrix2 const &world_to_screen,
            Float pixels_in_view_radius,
            FloatVector2 const &view_center,
            Float view_radius,
            Object::MetricMode metric_mode);
    
        // selects/selection-toggles all objects touching/inside the given circle
        void SelectObjectsByCircle (
            FloatVector2 const &center,
            Float radius,
            Object::SelectionOperation selection_operation,
            bool select_touching);
        // selects/selection-toggles all objects in this tree
        void SelectAllObjects (bool toggle_selection);
    
        // selects/selection-toggles all vertices inside the given circle
        void SelectVerticesByCircle (
            FloatVector2 const &center,
            Float radius,
            Object::SelectionOperation selection_operation);
        // selects/selection-toggles all vertices in this tree
        void SelectAllVertices (bool toggle_selection, bool mask_by_object_selection_set);
        // returns the vertex closest to the center of the given circle,
        // but only if inside the circle.
        void ComputeNearestVertex (
            FloatVector2 const &center,
            Float radius,
            Compound **compound_containing_nearest,
            CompoundVertex **nearest_vertex);

        // selects/selection-toggles all the compound polygons in this tree
        void SelectAllPolygons (bool toggle_selection, bool mask_by_object_selection_set);
        // sets the vertex selection set based upon the selected
        // owner polygon count for each vertex
        void SetVertexSelectionStateFromSelectionOwnerPolygonCount ();
            
    protected:
    
        // for use in Create
        VisibilityQuadTree (VisibilityQuadTree *parent) : Engine2::VisibilityQuadTree(parent) { }
    
    private:

        void DrawMetrics (
            DrawLoopFunctor const &draw_data,
            Object::MetricMode metric_mode);
        void DrawMetricsWrapped (
            DrawLoopFunctor draw_data,
            Object::MetricMode metric_mode);
    
        void ComputeNearestVertexPrivate (
            FloatVector2 const &center,
            Float radius,
            Compound **compound_containing_nearest,
            CompoundVertex **nearest_vertex,
            Float *nearest_distance);
    }; // end of class MapEditor2::VisibilityQuadTree

} // end of namespace MapEditor2

} // end of namespace Xrb

#endif // !defined(_XRB_MAPEDITOR2_VISIBILITYQUADTREE_H_)

