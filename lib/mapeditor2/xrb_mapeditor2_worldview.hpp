// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_worldview.hpp by Victor Dods, created 2005/01/30
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_MAPEDITOR2_WORLDVIEW_HPP_)
#define _XRB_MAPEDITOR2_WORLDVIEW_HPP_

#include "xrb.hpp"

#include "xrb_mapeditor2_object.hpp"
#include "xrb_signalhandler.hpp"
#include "xrb_engine2_worldview.hpp"

namespace Xrb
{

namespace MapEditor2
{

    class ObjectLayer;
    class World;
    
    class WorldView : public Engine2::WorldView, public SignalHandler
    {
    public:

        enum EditingSubMode
        {
            ESM_DEFAULT = 0,
            ESM_DRAW_POLYGON,
            ESM_POSITION_GLOBAL_ORIGIN_CURSOR,
            ESM_ADD_SPRITE,

            ESM_EDITING_SUB_MODE_COUNT
        }; // end of enum EditingSubMode
        
        WorldView (Engine2::WorldViewWidget *parent_world_view_widget);
        virtual ~WorldView ();
    
        ObjectLayer *GetSavedMainObjectLayer () const;
        World *GetMapEditorWorld () const;
        // returns the transformation mode
        inline Object::TransformationMode GetTransformationMode () const
        {
            return m_transformation_mode;
        }
        // returns the transformation mode text
        inline std::string const &GetTransformationModeText () const
        {
            return m_transformation_mode_text;
        }
        // returns the metric editing mode
        inline Object::MetricMode GetMetricEditingMode () const
        {
            return m_metric_editing_mode;
        }
        // returns the metric editing mode text
        inline std::string const &GetMetricEditingModeText () const
        {
            return m_metric_editing_mode_text;
        }
        // returns the current grid scale text
        std::string CurrentGridScaleText () const;
        // returns the curretn polygon tesselation text
        std::string GetPolygonTesselationText () const;
    
        // returns the currently active origin cursor
        FloatVector2 const &GetOriginCursor () const;
    
        bool AreNoObjectsSelected () const;
        FloatVector2 const &GetObjectSelectionSetOrigin () const;
        inline Float GetObjectSelectionSetScale () const
        {
            return m_object_selection_set_scale;
        }
        inline Float GetObjectSelectionSetAngle () const
        {
            return m_object_selection_set_angle;
        }
    
        bool AreNoEntitiesSelected () const;
        inline Float GetObjectSelectionSetMass () const
        {
            return m_object_selection_set_mass;
        }
    
        void SetTransformationMode (
            Object::TransformationMode transformation_mode);
        void SetMetricEditingMode (
            Object::MetricMode metric_editing_mode);
        void SetEditingSubMode (
            EditingSubMode editing_sub_mode);
    
        //////////////////////////////////////////////////////////////////////////
        // SignalSender accessors
    
        inline SignalSender1<std::string const &> const *SenderStatusBarMessageTextChanged () { return &m_sender_status_bar_message_text_changed; }
        inline SignalSender1<std::string const &> const *SenderGridScaleTextChanged () { return &m_sender_grid_scale_text_changed; }
        inline SignalSender1<std::string const &> const *SenderPolygonTesselationTextChanged () { return &m_sender_polygon_tesselation_text_changed; }
        inline SignalSender1<Object::TransformationMode> const *SenderTransformationModeChanged () { return &m_sender_transformation_mode_changed; }
        inline SignalSender1<std::string const &> const *SenderTransformationModeTextChanged () { return &m_sender_transformation_mode_text_changed; }
        inline SignalSender1<Object::MetricMode> const *SenderMetricEditingModeChanged () { return &m_sender_metric_editing_mode_changed; }
        inline SignalSender1<std::string const &> const *SenderMetricEditingModeTextChanged () { return &m_sender_metric_editing_mode_text_changed; }
    
        inline SignalSender1<bool> const *SenderNoObjectsAreSelected () { return &m_sender_no_objects_are_selected; }
        inline SignalSender1<FloatVector2 const &> const *SenderObjectSelectionSetOriginChanged () { return &m_sender_object_selection_set_origin_changed; }
        inline SignalSender1<Float> const *SenderObjectSelectionSetScaleChanged () { return &m_sender_object_selection_set_scale_changed; }
        inline SignalSender1<Float> const *SenderObjectSelectionSetAngleChanged () { return &m_sender_object_selection_set_angle_changed; }
    
        inline SignalSender1<bool> const *SenderNoEntitiesAreSelected () { return &m_sender_no_entities_are_selected; }
        inline SignalSender1<Uint32> const *SenderSelectedEntityCountChanged () { return &m_sender_selected_entity_count_changed; }
        inline SignalSender1<Float> const *SenderObjectSelectionSetMassChanged () { return &m_sender_object_selection_set_mass_changed; }
        inline SignalSender1<FloatVector2 const &> const *SenderObjectSelectionSetVelocityChanged () { return &m_sender_object_selection_set_velocity_changed; }
        inline SignalSender1<Float> const *SenderObjectSelectionSetSecondMomentChanged () { return &m_sender_object_selection_set_second_moment_changed; }
        inline SignalSender1<Float> const *SenderObjectSelectionSetAngularVelocityChanged () { return &m_sender_object_selection_set_angular_velocity_changed; }
        inline SignalSender1<Float> const *SenderObjectSelectionSetElasticityChanged () { return &m_sender_object_selection_set_elasticity_changed; }
        inline SignalSender1<Float> const *SenderObjectSelectionSetDensityChanged () { return &m_sender_object_selection_set_density_changed; }
        inline SignalSender1<bool> const *SenderPerEntityAppliesGravityAssigned () { return &m_sender_per_entity_applies_gravity_assigned; }
        inline SignalSender1<bool> const *SenderPerEntityReactsToGravityAssigned () { return &m_sender_per_entity_reacts_to_gravity_assigned; }
    
        //////////////////////////////////////////////////////////////////////////
        // SignalReceiver accessors
    
        inline SignalReceiver1<Object::TransformationMode> const *ReceiverSetTransformationMode () { return &m_receiver_set_transformation_mode; }
        inline SignalReceiver1<Object::MetricMode> const *ReceiverSetMetricEditingMode () { return &m_receiver_set_metric_editing_mode; }
        inline SignalReceiver1<std::string const &> const *ReceiverCreateStaticSprite () { return &m_receiver_create_static_sprite; }
        inline SignalReceiver1<std::string const &> const *ReceiverCreateSpriteEntity () { return &m_receiver_create_sprite_entity; }
    
        inline SignalReceiver1<Float> const *ReceiverSetPerObjectOriginX () { return &m_receiver_set_per_object_origin_x; }
        inline SignalReceiver1<Float> const *ReceiverSetPerObjectOriginY () { return &m_receiver_set_per_object_origin_y; }
        inline SignalReceiver1<Float> const *ReceiverSetPerObjectScale () { return &m_receiver_set_per_object_scale; }
        inline SignalReceiver1<Float> const *ReceiverSetPerObjectAngle () { return &m_receiver_set_per_object_angle; }
    
        inline SignalReceiver1<Float> const *ReceiverSetObjectSelectionSetOriginX () { return &m_receiver_set_object_selection_set_origin_x; }
        inline SignalReceiver1<Float> const *ReceiverSetObjectSelectionSetOriginY () { return &m_receiver_set_object_selection_set_origin_y; }
        inline SignalReceiver1<Float> const *ReceiverSetObjectSelectionSetScale () { return &m_receiver_set_object_selection_set_scale; }
        inline SignalReceiver1<Float> const *ReceiverSetObjectSelectionSetAngle () { return &m_receiver_set_object_selection_set_angle; }
    
        inline SignalReceiver1<Float> const *ReceiverSetPerEntityMass () { return &m_receiver_set_per_entity_mass; }
        inline SignalReceiver1<Float> const *ReceiverSetPerEntityVelocityX () { return &m_receiver_set_per_entity_velocity_x; }
        inline SignalReceiver1<Float> const *ReceiverSetPerEntityVelocityY () { return &m_receiver_set_per_entity_velocity_y; }
        inline SignalReceiver1<Float> const *ReceiverSetPerEntitySpeed () { return &m_receiver_set_per_entity_speed; }
        inline SignalReceiver1<Float> const *ReceiverSetPerEntityVelocityAngle () { return &m_receiver_set_per_entity_velocity_angle; }
        inline SignalReceiver1<Float> const *ReceiverSetPerEntitySecondMoment () { return &m_receiver_set_per_entity_second_moment; }
        inline SignalReceiver1<Float> const *ReceiverSetPerEntityAngularVelocity () { return &m_receiver_set_per_entity_angular_velocity; }
        inline SignalReceiver1<Float> const *ReceiverSetPerEntityElasticity () { return &m_receiver_set_per_entity_elasticity; }
        inline SignalReceiver1<Float> const *ReceiverSetPerEntityDensity () { return &m_receiver_set_per_entity_density; }
        inline SignalReceiver1<bool> const *ReceiverSetPerEntityAppliesGravity () { return &m_receiver_set_per_entity_applies_gravity; }
        inline SignalReceiver1<bool> const *ReceiverSetPerEntityReactsToGravity () { return &m_receiver_set_per_entity_reacts_to_gravity; }
    
        inline SignalReceiver1<Float> const *ReceiverSetObjectSelectionSetMass () { return &m_receiver_set_object_selection_set_mass; }
        inline SignalReceiver1<Float> const *ReceiverSetObjectSelectionSetVelocityX () { return &m_receiver_set_object_selection_set_velocity_x; }
        inline SignalReceiver1<Float> const *ReceiverSetObjectSelectionSetVelocityY () { return &m_receiver_set_object_selection_set_velocity_y; }
        inline SignalReceiver1<Float> const *ReceiverSetObjectSelectionSetSpeed () { return &m_receiver_set_object_selection_set_speed; }
        inline SignalReceiver1<Float> const *ReceiverSetObjectSelectionSetVelocityAngle () { return &m_receiver_set_object_selection_set_velocity_angle; }
        inline SignalReceiver1<Float> const *ReceiverSetObjectSelectionSetSecondMoment () { return &m_receiver_set_object_selection_set_second_moment; }
        inline SignalReceiver1<Float> const *ReceiverSetObjectSelectionSetAngularVelocity () { return &m_receiver_set_object_selection_set_angular_velocity; }
        inline SignalReceiver1<Float> const *ReceiverSetObjectSelectionSetElasticity () { return &m_receiver_set_object_selection_set_elasticity; }
        inline SignalReceiver1<Float> const *ReceiverSetObjectSelectionSetDensity () { return &m_receiver_set_object_selection_set_density; }
    
        void CreateStaticSprite (std::string const &filename);
        void CreateSpriteEntity (std::string const &filename);
    
        void SetCurrentGridScale (Uint32 current_grid_scale);
        void SetPolygonTesselation (Uint32 polygon_tesselation);
    
        void SetPerObjectOriginX (Float origin_x);
        void SetPerObjectOriginY (Float origin_y);
        void SetPerObjectScale (Float scale);
        void SetPerObjectAngle (Float angle);
    
        void SetObjectSelectionSetOriginX (Float origin_x);
        void SetObjectSelectionSetOriginY (Float origin_y);
        void SetObjectSelectionSetScale (Float scale);
        void SetObjectSelectionSetAngle (Float angle);
    
        void SetPerEntityMass (Float mass);
        void SetPerEntityVelocityX (Float velocity_x);
        void SetPerEntityVelocityY (Float velocity_y);
        void SetPerEntitySpeed (Float speed);
        void SetPerEntityVelocityAngle (Float velocity_angle);
        void SetPerEntitySecondMoment (Float second_moment);
        void SetPerEntityAngularVelocity (Float angular_velocity);
        void SetPerEntityElasticity (Float elasticity);
        void SetPerEntityDensity (Float density);
        void SetPerEntityAppliesGravity (bool applies_gravity);
        void SetPerEntityReactsToGravity (bool reacts_to_gravity);
    
        void SetObjectSelectionSetMass (Float mass);
        void SetObjectSelectionSetVelocityX (Float velocity_x);
        void SetObjectSelectionSetVelocityY (Float velocity_y);
        void SetObjectSelectionSetSpeed (Float speed);
        void SetObjectSelectionSetVelocityAngle (Float velocity_angle);
        void SetObjectSelectionSetSecondMoment (Float second_moment);
        void SetObjectSelectionSetAngularVelocity (Float angular_velocity);
        void SetObjectSelectionSetElasticity (Float elasticity);
        void SetObjectSelectionSetDensity (Float density);
    
        virtual void Draw (RenderContext const &render_context);
    
        // process a key event
        virtual bool ProcessKeyEvent (EventKey const *e);
        // process a mouse button event
        virtual bool ProcessMouseButtonEvent (EventMouseButton const *e);
        // process a mouse wheel event
        virtual bool ProcessMouseWheelEvent (EventMouseWheel const *e);
        // process a mouse motion event
        virtual bool ProcessMouseMotionEvent (EventMouseMotion const *e);

        // this is necessary so that editing submodes can be canceled
        // when this widget loses focus.
        virtual void HandleUnfocus ();
        
    protected:

        virtual void HandleAttachedWorld ();
                        
        virtual void HandleFrame ();

        ObjectLayer *GetMainMapEditorObjectLayer () const;
        
    private:
    
        Object::SelectionOperation GetSelectionOperation (
            EventMouseButton const *e) const;
    
        void SetSelectedObjectCount (Uint32 selected_object_count);
        void UpdateObjectSelectionSetOrigin (FloatVector2 const &object_selection_set_origin);
        void UpdateObjectSelectionSetScale (Float object_selection_set_scale);
        void UpdateObjectSelectionSetAngle (Float object_selection_set_angle);
    
        void SetSelectedEntityCount (Uint32 selected_entity_count);
        void UpdateObjectSelectionSetMass (Float object_selection_set_mass);
        void UpdateObjectSelectionSetVelocity (FloatVector2 const &object_selection_set_velocity);
        void UpdateObjectSelectionSetSecondMoment (Float object_selection_set_second_moment);
        void UpdateObjectSelectionSetAngularVelocity (Float object_selection_set_angular_velocity);
        void UpdateObjectSelectionSetElasticity (Float object_selection_set_elasticity);
        void UpdateObjectSelectionSetDensity (Float object_selection_set_density);

        void SetSelectedCompoundCount (Uint32 selected_compound_count);
            
        void UpdateMainObjectLayerConnections ();
        void ChangeObjectSelectionSet ();
        void ChangeSelectionObjects ();
        void ChangeSelectionEntities ();
    
        void DrawOriginCursor (
            RenderContext const &render_context,
            FloatVector2 const &origin,
            Object::TransformationMode transformation_mode);
            
        void TransformationEdit (
            FloatVector2 const &last_mouse_position,
            FloatVector2 const &current_mouse_position,
            FloatVector2 const &position_delta,
            EventMouseMotion const *e);
        void LinearVelocityEdit (
            FloatVector2 const &last_mouse_position,
            FloatVector2 const &current_mouse_position,
            FloatVector2 const &position_delta,
            EventMouseMotion const *e);
        void AngularVelocityEdit (
            FloatVector2 const &last_mouse_position,
            FloatVector2 const &current_mouse_position,
            EventMouseMotion const *e);
        void VerticesEdit (
            FloatVector2 const &last_mouse_position,
            FloatVector2 const &current_mouse_position,
            FloatVector2 const &position_delta,
            EventMouseMotion const *e);
    
        void ResetEditDeltas ();
        void SignalEditDeltaText ();
        void AccumulateTranslationEditDelta (FloatVector2 const &translation_delta);
        void AccumulateScaleEditDelta (Float scale_delta);
        void AccumulateAngleEditDelta (Float angle_delta);
        void AccumulateScaleAndAngleEditDeltas (
            Float scale_delta,
            Float angle_delta);
    
        void ResetObjectSelectionSetScale ();
        void ResetObjectSelectionSetAngle ();
        void AccumulateObjectSelectionSetScale (Float scale_delta);
        void AccumulateObjectSelectionSetAngle (Float angle_delta);

        void ConnectSignals ();

        // this is necessary because m_world is set to NULL before the destructor
        // has a chance to fuck with it.
        World *m_view_owned_world;
        
        //////////////////////////////////////////////////////////////////////////
        // view option vars
    
        // indicates if object metrics will be drawn
        bool m_draw_object_metrics;
    
        //////////////////////////////////////////////////////////////////////////
        // object editing vars

        // the current transformation mode
        Object::TransformationMode m_transformation_mode;
        // the text describing the current transformation mode
        std::string m_transformation_mode_text;
        // the current metric editing mode
        Object::MetricMode m_metric_editing_mode;
        // the editing submode (e.g. adding a vertex)
        EditingSubMode m_editing_sub_mode;
        // the metric editing mode which was saved when a new
        // editing sub mode was entered
        Object::MetricMode m_saved_metric_editing_mode;
        // the text describing the current metric editing mode
        std::string m_metric_editing_mode_text;
        // indicates if a translation edit is in progress
        bool m_is_translation_edit_in_progress;
        // stores the translation delta
        FloatVector2 m_translation_delta;
        // indicates if a scale edit is in progress
        bool m_is_scale_edit_in_progress;
        // stores the scale delta factor
        Float m_scale_delta;
        // indicates if an angle edit is in progress
        bool m_is_angle_edit_in_progress;
        // stores the angle delta
        Float m_angle_delta;
        // stores the tesselation of new dragged-out polygons
        Uint32 m_polygon_tesselation;
        // indicates if an object was selected with a click, and it should
        // be unselected when the mouse is let go
        bool m_clear_object_selection_upon_mouse_button_up;
        // indicates if a polygon was selected with a click, and it should
        // be unselected when the mouse is let go
        bool m_clear_polygon_selection_upon_mouse_button_up;
        // indicates if a vertex was selected with a click, and it should
        // be unselected when the mouse is let go
        bool m_clear_vertex_selection_upon_mouse_button_up;
    
        //////////////////////////////////////////////////////////////////////////
        // mouse tracking vars
    
        // the position of the mouse at the most recent left mouse button press
        // in virtual screen coordinates
        FloatVector2 m_lmouse_pressed_position;
        // the position of the mouse at the most recent right mouse button press
        // in virtual screen coordinates
        FloatVector2 m_rmouse_pressed_position;
        // the position of the mouse at the most recent left mouse button press
        // in world coordinates
        FloatVector2 m_lmouse_pressed_world_position;
        // the position of the mouse at the most recent right mouse button press
        // in world coordinates
        FloatVector2 m_rmouse_pressed_world_position;
        // true if the mouse has moved since the left mouse button was pressed
        // (only applicable when the left mouse button is pressed)
        bool m_lmouse_dragged;
        // true if the mouse has moved since the right mouse button was pressed
        // (only applicable when the right mouse button is pressed)
        bool m_rmouse_dragged;
    
        //////////////////////////////////////////////////////////////////////////
        // object selection vars
    
        // if true, selection circles will select all object which touch it,
        // while if false, only objects which are completely inside the selection
        // circle will be selected.
        bool m_select_touching;
        // stores the configuration for which operation is performed for
        // each combination of the CTRL and ALT keys when selecting objects.
        // the size of the array (4) is the number of possible combinations
        // of two keys being pressed or not (ALT being the 1s bit, and
        // CTRL being the 10s bit).
        Object::SelectionOperation m_selection_operation_lookup[4];
    
        // the number of objects currently selected in the current object layer,
        // set by a signal sender in ObjectLayer
        Uint32 m_selected_object_count;
        // the object selection set origin, set by a signal sender in ObjectLayer.
        FloatVector2 m_object_selection_set_origin;
        // the object selection set scale factor (is reset back to 1.0 when the number
        // of selected objects changes)
        Float m_object_selection_set_scale;
        // the object selection set angle (is reset back to 0.0 when the number of
        // selected objects changes)
        Float m_object_selection_set_angle;
    
        // the number of entities currently selected in the current object layer,
        // set by a signal sender in ObjectLayer
        Uint32 m_selected_entity_count;
        // the object selection set first moment (total mass of the entities in
        // the object selection set)
        Float m_object_selection_set_mass;
        // the object selection set first moment (velocity of the single selected entity)
        FloatVector2 m_object_selection_set_velocity;
        // the object selection set second moment (second moment of the single
        // selected entity)
        Float m_object_selection_set_second_moment;
        // the object selection set angular velocity (angular velocity of the single
        // selected entity)
        Float m_object_selection_set_angular_velocity;
        // the object selection set elasticity (elasticity of the single
        // selected entity)
        Float m_object_selection_set_elasticity;
        // the object selection set density (density of the single
        // selected entity)
        Float m_object_selection_set_density;

        // number of selected Compounds (inside the object selection set)
        Uint32 m_selected_compound_count;
        
        //////////////////////////////////////////////////////////////////////////
        // view movement vars
    
        // speed coefficient for using the arrow keys to move the view
        Float m_key_movement_speed_factor;
    
        //////////////////////////////////////////////////////////////////////////
        // view zooming vars
    
        // accumulates weighted zoom in/out requests.  zooming in increases
        // this value by m_zoom_increment, zooming out decreases it by the same.
        Float m_zoom_accumulator;
        // the quanta for m_zoom_accumulator.
        Float m_zoom_increment;
        // the rate at which to zoom.
        Float m_zoom_speed;
    
        //////////////////////////////////////////////////////////////////////////
        // view rotation vars
    
        // accumulates weighted rotate requests.  rotating counterclockwise
        // increases this value by m_rotation_increment, while rotating clockwise
        // decreases it by the same.
        Float m_rotation_accumulator;
        // the quanta for m_rotation_accumulator.
        Float m_rotation_increment;
        // the rate at which to rotate.
        Float m_rotation_speed;
    
        //////////////////////////////////////////////////////////////////////////
        // origin cursor vars
    
        // origin cursor position, in world coordinates
        FloatVector2 m_origin_cursor_position;
        // radius of the origin cursor, in virtual-screen coords
        Float m_origin_cursor_radius;
        // the color of the origin cursor of the active transformation mode
        Color m_active_origin_cursor_color;
        // the color of the origin cursor of the inactive transformation mode
        Color m_inactive_origin_cursor_color;
            
        //////////////////////////////////////////////////////////////////////////
        // SignalSenders
    
        SignalSender1<std::string const &> m_sender_status_bar_message_text_changed;
        SignalSender1<std::string const &> m_sender_grid_scale_text_changed;
        SignalSender1<std::string const &> m_sender_polygon_tesselation_text_changed;
        SignalSender1<Object::TransformationMode> m_sender_transformation_mode_changed;
        SignalSender1<std::string const &> m_sender_transformation_mode_text_changed;
        SignalSender1<Object::MetricMode> m_sender_metric_editing_mode_changed;
        SignalSender1<std::string const &> m_sender_metric_editing_mode_text_changed;
    
        SignalSender1<bool> m_sender_no_objects_are_selected;
        SignalSender1<FloatVector2 const &> m_sender_object_selection_set_origin_changed;
        SignalSender1<Float> m_sender_object_selection_set_scale_changed;
        SignalSender1<Float> m_sender_object_selection_set_angle_changed;
    
        SignalSender1<bool> m_sender_no_entities_are_selected;
        SignalSender1<Uint32> m_sender_selected_entity_count_changed;
        SignalSender1<Float> m_sender_object_selection_set_mass_changed;
        SignalSender1<FloatVector2 const &> m_sender_object_selection_set_velocity_changed;
        SignalSender1<Float> m_sender_object_selection_set_second_moment_changed;
        SignalSender1<Float> m_sender_object_selection_set_angular_velocity_changed;
        SignalSender1<Float> m_sender_object_selection_set_elasticity_changed;
        SignalSender1<Float> m_sender_object_selection_set_density_changed;
        SignalSender1<bool> m_sender_per_entity_applies_gravity_assigned;
        SignalSender1<bool> m_sender_per_entity_reacts_to_gravity_assigned;
    
        //////////////////////////////////////////////////////////////////////////
        // SignalReceivers
    
        SignalReceiver1<Object::TransformationMode> m_receiver_set_transformation_mode;
        SignalReceiver1<Object::MetricMode> m_receiver_set_metric_editing_mode;
        SignalReceiver1<std::string const &> m_receiver_create_static_sprite;
        SignalReceiver1<std::string const &> m_receiver_create_sprite_entity;
    
        SignalReceiver1<Float> m_receiver_set_per_object_origin_x;
        SignalReceiver1<Float> m_receiver_set_per_object_origin_y;
        SignalReceiver1<Float> m_receiver_set_per_object_scale;
        SignalReceiver1<Float> m_receiver_set_per_object_angle;
    
        SignalReceiver1<Float> m_receiver_set_object_selection_set_origin_x;
        SignalReceiver1<Float> m_receiver_set_object_selection_set_origin_y;
        SignalReceiver1<Float> m_receiver_set_object_selection_set_scale;
        SignalReceiver1<Float> m_receiver_set_object_selection_set_angle;
    
        SignalReceiver1<Float> m_receiver_set_per_entity_mass;
        SignalReceiver1<Float> m_receiver_set_per_entity_velocity_x;
        SignalReceiver1<Float> m_receiver_set_per_entity_velocity_y;
        SignalReceiver1<Float> m_receiver_set_per_entity_speed;
        SignalReceiver1<Float> m_receiver_set_per_entity_velocity_angle;
        SignalReceiver1<Float> m_receiver_set_per_entity_second_moment;
        SignalReceiver1<Float> m_receiver_set_per_entity_angular_velocity;
        SignalReceiver1<Float> m_receiver_set_per_entity_elasticity;
        SignalReceiver1<Float> m_receiver_set_per_entity_density;
        SignalReceiver1<bool> m_receiver_set_per_entity_applies_gravity;
        SignalReceiver1<bool> m_receiver_set_per_entity_reacts_to_gravity;
    
        SignalReceiver1<Float> m_receiver_set_object_selection_set_mass;
        SignalReceiver1<Float> m_receiver_set_object_selection_set_velocity_x;
        SignalReceiver1<Float> m_receiver_set_object_selection_set_velocity_y;
        SignalReceiver1<Float> m_receiver_set_object_selection_set_speed;
        SignalReceiver1<Float> m_receiver_set_object_selection_set_velocity_angle;
        SignalReceiver1<Float> m_receiver_set_object_selection_set_second_moment;
        SignalReceiver1<Float> m_receiver_set_object_selection_set_angular_velocity;
        SignalReceiver1<Float> m_receiver_set_object_selection_set_elasticity;
        SignalReceiver1<Float> m_receiver_set_object_selection_set_density;
    
        SignalReceiver0 m_internal_receiver_update_main_object_layer_connections;
        SignalReceiver0 m_internal_receiver_change_object_selection_set;
    
        SignalReceiver1<Uint32> m_internal_receiver_set_selected_object_count;
        SignalReceiver1<FloatVector2 const &> m_internal_receiver_update_object_selection_set_origin;
    
        SignalReceiver1<Uint32> m_internal_receiver_set_selected_entity_count;
        SignalReceiver1<Float> m_internal_receiver_update_object_selection_set_mass;
        SignalReceiver1<FloatVector2 const &> m_internal_receiver_update_object_selection_set_velocity;
        SignalReceiver1<Float> m_internal_receiver_update_object_selection_set_second_moment;
        SignalReceiver1<Float> m_internal_receiver_update_object_selection_set_angular_velocity;
        SignalReceiver1<Float> m_internal_receiver_update_object_selection_set_elasticity;
        SignalReceiver1<Float> m_internal_receiver_update_object_selection_set_density;
    
        SignalReceiver1<Uint32> m_internal_receiver_set_selected_compound_count;
    }; // end of class MapEditor2::WorldView

} // end of namespace MapEditor2

} // end of namespace Xrb

#endif // !defined(_XRB_MAPEDITOR2_WORLDVIEW_HPP_)

