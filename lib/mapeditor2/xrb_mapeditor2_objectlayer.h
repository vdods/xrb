// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_objectlayer.h by Victor Dods, created 2005/03/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_MAPEDITOR2_OBJECTLAYER_H_)
#define _XRB_MAPEDITOR2_OBJECTLAYER_H_

#include "xrb.h"

#include <set>

#include "xrb_engine2_objectlayer.h"
#include "xrb_mapeditor2_compound.h"
#include "xrb_mapeditor2_object.h"
#include "xrb_signalhandler.h"

namespace Xrb
{

namespace MapEditor2
{

    struct Polygon;
    struct CompoundVertex;
    class Entity;
    class VisibilityQuadTree;
    class World;
    
    // the ObjectLayer derives from Engine2::ObjectLayer and implements the
    // stuff needed by the map editor (WorldView) for things like object
    // selection and transformation.  It also inherits from SignalHandler
    // for signal sending capabilities.
    class ObjectLayer : public Engine2::ObjectLayer, public SignalHandler
    {
    public:
    
        ~ObjectLayer () { }
    
        static ObjectLayer *Create (
            World *owner_world,
            bool wrapped,
            Float side_length,
            Uint32 tree_depth,
            Float z_depth = 1.0);
        static ObjectLayer *Create (
            Serializer &serializer,
            World *owner_world);
    
        World *GetOwnerMapEditorWorld () const;
        inline bool GetIsObjectSelectionSetEmpty () const
        {
            return m_object_selection_set.empty();
        }
    
        inline Uint32 GetSelectedObjectCount () const
        {
            return m_selected_object_count;
        }
        Object *GetSingleSelectedObject ();
        inline FloatVector2 const &GetObjectSelectionSetOrigin () const
        {
            return m_object_selection_set_origin;
        }

        inline Uint32 GetSelectedNonEntityCount () const
        {
            ASSERT1(m_selected_entity_count <= m_selected_object_count)
            return m_selected_object_count - m_selected_entity_count;
        }
        Object *GetSingleSelectedNonEntity ();
            
        inline Uint32 GetSelectedEntityCount () const
        {
            return m_selected_entity_count;
        }
        Entity *GetSingleSelectedEntity ();
        inline Float GetObjectSelectionSetFirstMoment () const
        {
            return m_object_selection_set_first_moment;
        }
        inline FloatVector2 const &GetObjectSelectionSetVelocity () const
        {
            return m_object_selection_set_velocity;
        }
        inline Float GetObjectSelectionSetSecondMoment () const
        {
            return m_object_selection_set_first_moment;
        }
        inline Float GetObjectSelectionSetAngularVelocity () const
        {
            return m_object_selection_set_angular_velocity;
        }
        inline Float GetObjectSelectionSetElasticity () const
        {
            return m_object_selection_set_elasticity;
        }
        inline Float GetObjectSelectionSetDensity () const
        {
            return m_object_selection_set_density;
        }

        inline Uint32 GetSelectedCompoundCount () const
        {
            return m_selected_compound_count;
        }
        Compound *GetSingleSelectedCompound ();
            
        inline Uint32 GetSelectedVertexCount () const
        {
            return m_selected_vertex_count;
        }
            
        inline Uint32 GetSelectedPolygonCount () const
        {
            return m_selected_polygon_count;
        }
            
        inline SignalSender0 const *SenderObjectSelectionSetChanged () { return &m_sender_object_selection_set_changed; }
        inline SignalSender1<Uint32> const *SenderSelectedObjectCountChanged () { return &m_sender_selected_object_count_changed; }
        inline SignalSender1<FloatVector2 const &> const *SenderObjectSelectionSetOriginChanged () { return &m_sender_object_selection_set_origin_changed; }
    
        inline SignalSender1<Uint32> const *SenderSelectedEntityCountChanged () { return &m_sender_selected_entity_count_changed; }
        inline SignalSender1<Float> const *SenderObjectSelectionSetFirstMomentChanged () { return &m_sender_object_selection_set_first_moment_changed; }
        inline SignalSender1<FloatVector2 const &> const *SenderObjectSelectionSetCenterOfGravityChanged () { return &m_sender_object_selection_set_center_of_gravity_changed; }
        inline SignalSender1<FloatVector2 const &> const *SenderObjectSelectionSetVelocityChanged () { return &m_sender_object_selection_set_velocity_changed; }
        inline SignalSender1<Float> const *SenderObjectSelectionSetSecondMomentChanged () { return &m_sender_object_selection_set_second_moment_changed; }
        inline SignalSender1<Float> const *SenderObjectSelectionSetAngularVelocityChanged () { return &m_sender_object_selection_set_angular_velocity_changed; }
        inline SignalSender1<Float> const *SenderObjectSelectionSetElasticityChanged () { return &m_sender_object_selection_set_elasticity_changed; }
        inline SignalSender1<Float> const *SenderObjectSelectionSetDensityChanged () { return &m_sender_object_selection_set_density_changed; }

        inline SignalSender1<Uint32> const *SenderSelectedCompoundCountChanged () { return &m_sender_selected_compound_count_changed; }
            
        inline SignalSender1<Uint32> const *SenderSelectedVertexCountChanged () { return &m_sender_selected_vertex_count_changed; }
            
        inline SignalSender1<Uint32> const *SenderSelectedPolygonCountChanged () { return &m_sender_selected_polygon_count_changed; }
            
        Uint32 DrawMetrics (
            RenderContext const &render_context,
            FloatMatrix2 const &world_to_screen,
            Float pixels_in_view_radius,
            FloatVector2 const &view_center,
            Float view_radius,
            Object::MetricMode metric_mode);
                
        // applies the given selection operation to the smallest object touching
        // the given point.
        void SelectSmallestObjectTouchingPoint (
            FloatVector2 const &point,
            Object::SelectionOperation selection_operation);
        // applies the given selection operation to the objects
        // touching/inside the given circle
        void SelectObjectsByCircle (
            FloatVector2 const &center,
            Float radius,
            Object::SelectionOperation selection_operation,
            bool select_touching);
        // selects all objects in this layer
        void SelectAllObjects ();
        // inverts the current object selection set
        void InvertObjectSelectionSet ();
        // clears the object selection set
        void ClearObjectSelectionSet ();

        // clones the current object selection set, deselecting the old set
        // and selecting the newly cloned objects
        void ObjectSelectionSetClone (FloatVector2 const &position_offset);
        // deletes all objects in the current object selection set.
        void ObjectSelectionSetDelete ();
    
        // object transformations to apply to all objects in the set
        void ObjectSelectionSetTranslate (
            FloatVector2 const &translation_delta);
        void ObjectSelectionSetScale (
            Float scale_factor_delta,
            FloatVector2 const &transformation_origin,
            Object::TransformationMode transformation_mode);
        void ObjectSelectionSetRotate (
            Float angle_delta,
            FloatVector2 const &transformation_origin,
            Object::TransformationMode transformation_mode);
    
        // vertex transformations to apply to all vertices in the set
        void VertexSelectionSetTranslate (
            FloatVector2 const &translation_delta);
        void VertexSelectionSetScale (
            Float scale_factor_delta,
            FloatVector2 const &transformation_origin,
            Object::TransformationMode transformation_mode);
        void VertexSelectionSetRotate (
            Float angle_delta,
            FloatVector2 const &transformation_origin,
            Object::TransformationMode transformation_mode);
    
        void ObjectSelectionSetAssignPerObjectTranslationX (Float translation_x);
        void ObjectSelectionSetAssignPerObjectTranslationY (Float translation_y);
        void ObjectSelectionSetAssignPerObjectTranslation (FloatVector2 const &translation);
        void ObjectSelectionSetAssignPerObjectScale (Float scale_factor);
        void ObjectSelectionSetAssignPerObjectRotation (Float angle);
    
        void ObjectSelectionSetAssignPerEntityFirstMoment (Float first_moment);
        void ObjectSelectionSetAssignPerEntityVelocity (FloatVector2 const &velocity);
        void ObjectSelectionSetAssignPerEntityVelocityX (Float velocity_x);
        void ObjectSelectionSetAssignPerEntityVelocityY (Float velocity_y);
        void ObjectSelectionSetAssignPerEntitySpeed (Float speed);
        void ObjectSelectionSetAssignPerEntityVelocityAngle (Float velocity_angle);
        void ObjectSelectionSetAssignPerEntitySecondMoment (Float second_moment);
        void ObjectSelectionSetAssignPerEntityAngularVelocity (Float angular_velocity);
        void ObjectSelectionSetAssignPerEntityElasticity (Float elasticity);
        void ObjectSelectionSetAssignPerEntityDensity (Float density);
        void ObjectSelectionSetAssignPerEntityAppliesGravity (bool applies_gravity);
        void ObjectSelectionSetAssignPerEntityReactsToGravity (bool reacts_to_gravity);
    
        void ObjectSelectionSetScaleFirstMoment (Float first_moment_scale_factor);
    
        void ObjectSelectionSetTranslateVelocity (FloatVector2 const &translation_delta);
        void ObjectSelectionSetScaleVelocity (
            Float scale_factor_delta,
            FloatVector2 const &transformation_origin,
            Object::TransformationMode transformation_mode);
        void ObjectSelectionSetRotateVelocity (
            Float angle_delta,
            FloatVector2 const &transformation_origin,
            Object::TransformationMode transformation_mode);
        void ObjectSelectionSetAccumulateAngularVelocity (Float angle_delta);
        void ObjectSelectionSetScaleAngularVelocity (Float scale_factor_delta);
    
        // this can be used by the WorldView to demand update signals
        // from the ObjectLayer (e.g. after running the world,
        // the origin cursors and such may be wrong, which this will fix).
        void ForceObjectSelectionSetSignals ();

        // applies the given selection operation to the vertices
        // inside the given circle (within the scope of the set of
        // selected compounds)
        void SelectVerticesByCircle (
            FloatVector2 const &center,
            Float radius,
            Object::SelectionOperation selection_operation);
        // selects the vertex closest to the center of the given circle,
        // but only if inside the circle.  if the object selection set
        // is not empty, operates only on vertices of objects in the object
        // selection set.
        // the vertex selection set must be empty.
        void SelectNearestVertex (
            FloatVector2 const &center,
            Float radius,
            Object::SelectionOperation selection_operation);
        // selects all vertices in the set of selected compounds
        // (within the scope of the set of selected compounds)
        void SelectAllVertices (bool mask_by_object_selection_set);
        // inverts the current vertex selection set (within the
        // scope of the set of selected compounds)
        void InvertVertexSelectionSet (bool mask_by_object_selection_set);
        // clears the vertex selection set
        void ClearVertexSelectionSet ();
        // causes all vertices that do not belong to selected objects
        // to become deselected.
        void MaskSelectedVerticesByObjectSelectionSet ();
        // unwelds the selected vertices (duplicates each as many times
        // as necessary so that none of the selected vertices are used
        // by more than one polygon).
        void UnweldSelectedVertices ();
            
        // applies the given selection operation to the smallest polygon
        // touching the given point (within the scope of the set of
        // selected compounds)
        void SelectSmallestPolygonTouchingPoint (
            FloatVector2 const &point,
            Object::SelectionOperation selection_operation);
        // selects all polygons in this layer (within the scope of the
        // set of selected compounds)
        void SelectAllPolygons (bool mask_by_object_selection_set);
        // inverts the current polygon selection set (within the scope
        // of the set of selected compounds)
        void InvertPolygonSelectionSet (bool mask_by_object_selection_set);
        // clears the polygon selection set
        void ClearPolygonSelectionSet ();
        // causes all polygon that do not belong to selected objects
        // to become deselected.
        void MaskSelectedPolygonsByObjectSelectionSet ();
        // sets the vertex selection set based upon the selected
        // owner polygon count for each vertex
        void SetVertexSelectionStateFromSelectionOwnerPolygonCount ();
        // unwelds the vertices shared between the selected and the
        // unselected polygons (detaching the selected polygons)
        void UnweldSelectedPolygons ();
        // deletes the selected polygons.
        void DeleteSelectedPolygons ();
                                            
    protected:
    
        // protected constructor so that you must use Create()
        ObjectLayer (
            World *owner_world,
            bool wrapped,
            Float side_length,
            Float z_depth);
    
        VisibilityQuadTree *GetMapEditorQuadTree () const;
    
    private:

        // ///////////////////////////////////////////////////////////////////
        // object selection set private functions
        // ///////////////////////////////////////////////////////////////////
    
        void SetSelectedObjectCount (Uint32 selected_object_count);
        void SetObjectSelectionSetOrigin (FloatVector2 const &object_selection_set_origin);
    
        void SetSelectedEntityCount (Uint32 selected_entity_count);
        void SetObjectSelectionSetFirstMoment (Float object_selection_set_first_moment);
        void SetObjectSelectionSetCenterOfGravity (
            FloatVector2 const &object_selection_set_center_of_gravity);
    
        // adds the given object to the object selection set
        bool AddObjectToObjectSelectionSet (Object *object);
        // removes the given object from the object selection set
        bool RemoveObjectFromObjectSelectionSet (Object *object);
        // updates the cached number of selected objects and the set origin
        void UpdateObjectsAndEntitiesProperties ();
    
        void UpdateObjectSelectionSetVelocity ();
        void UpdateObjectSelectionSetSecondMoment ();
        void UpdateObjectSelectionSetAngularVelocity ();
        void UpdateObjectSelectionSetElasticity ();
        void UpdateObjectSelectionSetDensity ();

        // ///////////////////////////////////////////////////////////////////
        // compound selection set private functions
        // ///////////////////////////////////////////////////////////////////

        void SetSelectedCompoundCount (Uint32 selected_compound_count);
        
        // ///////////////////////////////////////////////////////////////////
        // vertex selection set private functions
        // ///////////////////////////////////////////////////////////////////

        void SetSelectedVertexCount (Uint32 selected_vertex_count);
                
        // adds the given vertex to the vertex selection set
        void AddVertexToVertexSelectionSet (CompoundVertex *vertex);
        // removes the given vertex from the vertex selection set
        void RemoveVertexFromVertexSelectionSet (CompoundVertex *vertex);
        
        // ///////////////////////////////////////////////////////////////////
        // polygon selection set private functions
        // ///////////////////////////////////////////////////////////////////

        void SetSelectedPolygonCount (Uint32 selected_polygon_count);

        // adds the given polygon to the polygon selection set
        void AddPolygonToPolygonSelectionSet (Polygon *polygon);
        // removes the given polygon from the polygon selection set
        void RemovePolygonFromPolygonSelectionSet (Polygon *polygon);
        
        // ///////////////////////////////////////////////////////////////////////
    
        typedef std::set<Object *> ObjectSet;
        typedef ObjectSet::iterator ObjectSetIterator;

        typedef std::set<CompoundVertex *> VertexSet;
        typedef VertexSet::iterator VertexSetIterator;

        typedef std::set<Polygon *> PolygonSet;
        typedef PolygonSet::iterator PolygonSetIterator;
            
        // the set of selection objects
        ObjectSet m_object_selection_set;    
        // number of selected objects
        Uint32 m_selected_object_count;
        // object selection set origin
        FloatVector2 m_object_selection_set_origin;
        
        // number of selected entities
        Uint32 m_selected_entity_count;
        // object selection set first moment
        Float m_object_selection_set_first_moment;
        // object selection set center of gravity
        FloatVector2 m_object_selection_set_center_of_gravity;
        // object selection set velocity
        FloatVector2 m_object_selection_set_velocity;
        // object selection set second moment
        Float m_object_selection_set_second_moment;
        // object selection set angular velocity
        Float m_object_selection_set_angular_velocity;
        // object selection set elasticity
        Float m_object_selection_set_elasticity;
        // object selection set density
        Float m_object_selection_set_density;

        // number of selected Compounds (always <= m_selected_object_count)
        Uint32 m_selected_compound_count;
        
        // the set of selected compound vertices
        VertexSet m_vertex_selection_set;
        // number of selected vertices
        Uint32 m_selected_vertex_count;

        // the set of selected compound polygons
        PolygonSet m_polygon_selection_set;
        // number of selected polygons
        Uint32 m_selected_polygon_count;
            
        //////////////////////////////////////////////////////////////////////////
        // SignalSenders
        SignalSender0 m_sender_object_selection_set_changed;
        SignalSender1<Uint32> m_sender_selected_object_count_changed;
        SignalSender1<FloatVector2 const &> m_sender_object_selection_set_origin_changed;
        SignalSender1<Uint32> m_sender_selected_entity_count_changed;
        SignalSender1<Float> m_sender_object_selection_set_first_moment_changed;
        SignalSender1<FloatVector2 const &> m_sender_object_selection_set_center_of_gravity_changed;
        SignalSender1<FloatVector2 const &> m_sender_object_selection_set_velocity_changed;
        SignalSender1<Float> m_sender_object_selection_set_second_moment_changed;
        SignalSender1<Float> m_sender_object_selection_set_angular_velocity_changed;
        SignalSender1<Float> m_sender_object_selection_set_elasticity_changed;
        SignalSender1<Float> m_sender_object_selection_set_density_changed;

        SignalSender1<Uint32> m_sender_selected_compound_count_changed;
        
        SignalSender1<Uint32> m_sender_selected_vertex_count_changed;
        
        SignalSender1<Uint32> m_sender_selected_polygon_count_changed;
        
        friend class Object;
        friend struct CompoundVertex;
        friend struct Polygon;
    }; // end of class MapEditor2::ObjectLayer

} // end of namespace MapEditor2

} // end of namespace Xrb

#endif // !defined(_XRB_MAPEDITOR2_OBJECTLAYER_H_)
