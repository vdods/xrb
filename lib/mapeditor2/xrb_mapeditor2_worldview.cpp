// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_worldview.cpp by Victor Dods, created 2005/01/30
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_mapeditor2_worldview.h"

#include "xrb_engine2_objectlayer.h"
#include "xrb_engine2_physicshandler.h" // temp
#include "xrb_engine2_world.h"
#include "xrb_engine2_worldviewwidget.h"
#include "xrb_filedialog.h"
#include "xrb_input_events.h"
#include "xrb_keybinds.h"
#include "xrb_mapeditor2_compound.h"
#include "xrb_mapeditor2_objectlayer.h"
#include "xrb_mapeditor2_sprite.h"
#include "xrb_mapeditor2_spriteentity.h"
#include "xrb_mapeditor2_world.h"
#include "xrb_math.h"
#include "xrb_render.h"
#include "xrb_serializer.h" // temp
#include "xrb_screen.h"
#include "xrb_util.h"
#include "xrb_widgetbackground.h"

namespace Xrb
{

MapEditor2::WorldView::WorldView (Engine2::WorldViewWidget *const parent_view_widget)
    :
    Engine2::WorldView(parent_view_widget),
    m_sender_status_bar_message_text_changed(this),
    m_sender_grid_scale_text_changed(this),
    m_sender_polygon_tesselation_text_changed(this),
    m_sender_transformation_mode_changed(this),
    m_sender_transformation_mode_text_changed(this),
    m_sender_metric_editing_mode_changed(this),
    m_sender_metric_editing_mode_text_changed(this),
    m_sender_no_objects_are_selected(this),
    m_sender_object_selection_set_origin_changed(this),
    m_sender_object_selection_set_scale_changed(this),
    m_sender_object_selection_set_angle_changed(this),
    m_sender_no_entities_are_selected(this),
    m_sender_selected_entity_count_changed(this),
    m_sender_object_selection_set_first_moment_changed(this),
    m_sender_object_selection_set_velocity_changed(this),
    m_sender_object_selection_set_second_moment_changed(this),
    m_sender_object_selection_set_angular_velocity_changed(this),
    m_sender_object_selection_set_elasticity_changed(this),
    m_sender_object_selection_set_density_changed(this),
    m_sender_per_entity_applies_gravity_assigned(this),
    m_sender_per_entity_reacts_to_gravity_assigned(this),
    m_receiver_set_transformation_mode(
        &MapEditor2::WorldView::SetTransformationMode, this),
    m_receiver_set_metric_editing_mode(
        &MapEditor2::WorldView::SetMetricEditingMode, this),
    m_receiver_create_static_sprite(
        &MapEditor2::WorldView::CreateStaticSprite, this),
    m_receiver_create_sprite_entity(
        &MapEditor2::WorldView::CreateSpriteEntity, this),
    m_receiver_set_per_object_origin_x(
        &MapEditor2::WorldView::SetPerObjectOriginX, this),
    m_receiver_set_per_object_origin_y(
        &MapEditor2::WorldView::SetPerObjectOriginY, this),
    m_receiver_set_per_object_scale(
        &MapEditor2::WorldView::SetPerObjectScale, this),
    m_receiver_set_per_object_angle(
        &MapEditor2::WorldView::SetPerObjectAngle, this),
    m_receiver_set_object_selection_set_origin_x(
        &MapEditor2::WorldView::SetObjectSelectionSetOriginX, this),
    m_receiver_set_object_selection_set_origin_y(
        &MapEditor2::WorldView::SetObjectSelectionSetOriginY, this),
    m_receiver_set_object_selection_set_scale(
        &MapEditor2::WorldView::SetObjectSelectionSetScale, this),
    m_receiver_set_object_selection_set_angle(
        &MapEditor2::WorldView::SetObjectSelectionSetAngle, this),
    m_receiver_set_per_entity_first_moment(
        &MapEditor2::WorldView::SetPerEntityFirstMoment, this),
    m_receiver_set_per_entity_velocity_x(
        &MapEditor2::WorldView::SetPerEntityVelocityX, this),
    m_receiver_set_per_entity_velocity_y(
        &MapEditor2::WorldView::SetPerEntityVelocityY, this),
    m_receiver_set_per_entity_speed(
        &MapEditor2::WorldView::SetPerEntitySpeed, this),
    m_receiver_set_per_entity_velocity_angle(
        &MapEditor2::WorldView::SetPerEntityVelocityAngle, this),
    m_receiver_set_per_entity_second_moment(
        &MapEditor2::WorldView::SetPerEntitySecondMoment, this),
    m_receiver_set_per_entity_angular_velocity(
        &MapEditor2::WorldView::SetPerEntityAngularVelocity, this),
    m_receiver_set_per_entity_elasticity(
        &MapEditor2::WorldView::SetPerEntityElasticity, this),
    m_receiver_set_per_entity_density(
        &MapEditor2::WorldView::SetPerEntityDensity, this),
    m_receiver_set_per_entity_applies_gravity(
        &MapEditor2::WorldView::SetPerEntityAppliesGravity, this),
    m_receiver_set_per_entity_reacts_to_gravity(
        &MapEditor2::WorldView::SetPerEntityReactsToGravity, this),
    m_receiver_set_object_selection_set_first_moment(
        &MapEditor2::WorldView::SetObjectSelectionSetFirstMoment, this),
    m_receiver_set_object_selection_set_velocity_x(
        &MapEditor2::WorldView::SetObjectSelectionSetVelocityX, this),
    m_receiver_set_object_selection_set_velocity_y(
        &MapEditor2::WorldView::SetObjectSelectionSetVelocityY, this),
    m_receiver_set_object_selection_set_speed(
        &MapEditor2::WorldView::SetObjectSelectionSetSpeed, this),
    m_receiver_set_object_selection_set_velocity_angle(
        &MapEditor2::WorldView::SetObjectSelectionSetVelocityAngle, this),
    m_receiver_set_object_selection_set_second_moment(
        &MapEditor2::WorldView::SetObjectSelectionSetSecondMoment, this),
    m_receiver_set_object_selection_set_angular_velocity(
        &MapEditor2::WorldView::SetObjectSelectionSetAngularVelocity, this),
    m_receiver_set_object_selection_set_elasticity(
        &MapEditor2::WorldView::SetObjectSelectionSetElasticity, this),
    m_receiver_set_object_selection_set_density(
        &MapEditor2::WorldView::SetObjectSelectionSetDensity, this),
    m_internal_receiver_update_main_object_layer_connections(
        &MapEditor2::WorldView::UpdateMainObjectLayerConnections, this),
    m_internal_receiver_change_object_selection_set(
        &MapEditor2::WorldView::ChangeObjectSelectionSet, this),
    m_internal_receiver_set_selected_object_count(
        &MapEditor2::WorldView::SetSelectedObjectCount, this),
    m_internal_receiver_update_object_selection_set_origin(
        &MapEditor2::WorldView::UpdateObjectSelectionSetOrigin, this),
    m_internal_receiver_set_selected_entity_count(
        &MapEditor2::WorldView::SetSelectedEntityCount, this),
    m_internal_receiver_update_object_selection_set_first_moment(
        &MapEditor2::WorldView::UpdateObjectSelectionSetFirstMoment, this),
    m_internal_receiver_update_object_selection_set_velocity(
        &MapEditor2::WorldView::UpdateObjectSelectionSetVelocity, this),
    m_internal_receiver_update_object_selection_set_second_moment(
        &MapEditor2::WorldView::UpdateObjectSelectionSetSecondMoment, this),
    m_internal_receiver_update_object_selection_set_angular_velocity(
        &MapEditor2::WorldView::UpdateObjectSelectionSetAngularVelocity, this),
    m_internal_receiver_update_object_selection_set_elasticity(
        &MapEditor2::WorldView::UpdateObjectSelectionSetElasticity, this),
    m_internal_receiver_update_object_selection_set_density(
        &MapEditor2::WorldView::UpdateObjectSelectionSetDensity, this),
    m_internal_receiver_set_selected_compound_count(
        &MapEditor2::WorldView::SetSelectedCompoundCount, this)
{
    m_view_owned_world = NULL;

    m_draw_object_metrics = true;

    m_transformation_mode = Object::TM_NUM_TRANSFORMATION_MODES;
    m_metric_editing_mode = Object::MM_NUM_METRIC_MODES;
    m_editing_sub_mode = ESM_EDITING_SUB_MODE_COUNT;
    m_saved_metric_editing_mode = Object::MM_NUM_METRIC_MODES;
    ResetEditDeltas();
    m_polygon_tesselation = 4;
    m_clear_object_selection_upon_mouse_button_up = false;
    m_clear_polygon_selection_upon_mouse_button_up = false;
    m_clear_vertex_selection_upon_mouse_button_up = false;

    m_lmouse_dragged = false;
    m_rmouse_dragged = false;

    m_select_touching = true;
    m_selection_operation_lookup[0] = Object::SO_EQUALS;
    m_selection_operation_lookup[1] = Object::SO_MINUS;
    m_selection_operation_lookup[2] = Object::SO_IOR;
    m_selection_operation_lookup[3] = Object::SO_XOR;

    m_grid_line_type = WorldView::GR_ABOVE_MAIN_LAYER;
//     m_grid_number_base = 10;
//     m_current_grid_scale = 1;

    m_key_movement_speed_factor = 0.5f;

    m_zoom_accumulator = 0.0f;
    m_zoom_increment = Math::Pow(2.0f, 0.25f);
    m_zoom_speed = 10.0f;

    m_rotation_accumulator = 0.0f;
    m_rotation_increment = 15.0f;
    m_rotation_speed = 120.0f;

    m_origin_cursor_position = FloatVector2::ms_zero;
    m_origin_cursor_radius = 0.05f;
    m_active_origin_cursor_color = Color(0.55f, 0.55f, 1.0f, 1.0f);
    m_inactive_origin_cursor_color = Color(0.0f, 0.0f, 0.6f, 1.0f);

    // these must be set to a ridiculous value so that when
    // they set to the real values in ConnectSignals, the proper
    // things happen.
    m_selected_object_count = UINT32_UPPER_BOUND;
    m_selected_entity_count = UINT32_UPPER_BOUND;
    m_selected_compound_count = UINT32_UPPER_BOUND;

    // we want to draw the border grid lines
    SetDrawBorderGridLines(true);
    
    SetTransformationMode(Object::TM_GLOBAL_ORIGIN_CURSOR);
    SetMetricEditingMode(Object::MM_TRANSFORMATION);
    SetEditingSubMode(ESM_DEFAULT);
}

MapEditor2::WorldView::~WorldView ()
{
    Delete(m_view_owned_world);
}

MapEditor2::ObjectLayer *MapEditor2::WorldView::GetSavedMainObjectLayer () const
{
    return DStaticCast<World *>(m_world)->GetSavedMainObjectLayer();
}

MapEditor2::World *MapEditor2::WorldView::GetMapEditorWorld () const
{
    return DStaticCast<World *>(m_world);
}

std::string MapEditor2::WorldView::GetCurrentGridScaleText () const
{
    ASSERT1(m_grid_number_base != 0)
    Float numerator = 0.5f * GetMainObjectLayer()->GetSideLength();
    Float denominator = 
        Math::Pow(
            static_cast<Float>(m_grid_number_base), 
            static_cast<Float>(m_current_grid_scale + 1));
    ASSERT1(numerator != 0.0f)
    ASSERT1(denominator != 0.0f)
    if (numerator >= denominator)
    {
        numerator /= denominator;
        return Util::StringPrintf("%g", numerator);
    }
    else
    {
        denominator /= numerator;
        return Util::StringPrintf("1 / %g", denominator);
    }
}

std::string MapEditor2::WorldView::GetPolygonTesselationText () const
{
    ASSERT1(m_polygon_tesselation >= 3)
    return Util::StringPrintf("%u", m_polygon_tesselation);
}

FloatVector2 const &MapEditor2::WorldView::GetOriginCursor () const
{
    switch (m_transformation_mode)
    {
        case Object::TM_GLOBAL_ORIGIN_CURSOR:
            return m_origin_cursor_position;

        case Object::TM_SELECTION_SET_ORIGIN:
            return GetMainMapEditorObjectLayer()->GetObjectSelectionSetOrigin();

        case Object::TM_EACH_SELECTED_OBJECT_ORIGIN:
            return GetMainMapEditorObjectLayer()->GetObjectSelectionSetOrigin();

        default:
            ASSERT0(false && "Invalid transformation type")
            return m_origin_cursor_position;
    }
}

bool MapEditor2::WorldView::GetAreNoObjectsSelected () const
{
    return GetMainMapEditorObjectLayer()->GetSelectedObjectCount() == 0;
}

FloatVector2 const &MapEditor2::WorldView::GetObjectSelectionSetOrigin () const
{
    return GetMainMapEditorObjectLayer()->GetObjectSelectionSetOrigin();
}

bool MapEditor2::WorldView::GetAreNoEntitiesSelected () const
{
    return GetMainMapEditorObjectLayer()->GetSelectedEntityCount() == 0;
}

void MapEditor2::WorldView::SetTransformationMode (
    Object::TransformationMode const transformation_mode)
{
    if (m_transformation_mode != transformation_mode)
    {
        m_transformation_mode = transformation_mode;
        switch (m_transformation_mode)
        {
            case Object::TM_GLOBAL_ORIGIN_CURSOR:
                m_transformation_mode_text = "Global Origin";
                break;

            case Object::TM_SELECTION_SET_ORIGIN:
                m_transformation_mode_text = "Selection Set Origin";
                break;

            case Object::TM_EACH_SELECTED_OBJECT_ORIGIN:
                m_transformation_mode_text = "Selected Object Origin";
                break;

            default:
                ASSERT1(false && "Invalid Object::TransformationMode")
                m_transformation_mode_text = "Invalid Mode";
                break;
        }
        m_sender_transformation_mode_changed.Signal(m_transformation_mode);
        m_sender_transformation_mode_text_changed.Signal(m_transformation_mode_text);
    }
}

void MapEditor2::WorldView::SetMetricEditingMode (
    Object::MetricMode const metric_editing_mode)
{
    if (m_metric_editing_mode != metric_editing_mode)
    {
        m_metric_editing_mode = metric_editing_mode;
        ResetEditDeltas();
        switch (m_metric_editing_mode)
        {
            case Object::MM_TRANSFORMATION:
                m_metric_editing_mode_text = "Transformation";
                break;

            case Object::MM_LINEAR_VELOCITY:
                m_metric_editing_mode_text = "Linear Velocity";
                break;

            case Object::MM_ANGULAR_VELOCITY:
                m_metric_editing_mode_text = "Angular Velocity";
                break;

            case Object::MM_POLYGONS:
                if (GetMainMapEditorObjectLayer()->GetSelectedObjectCount() > 0)
                    GetMainMapEditorObjectLayer()->MaskSelectedPolygonsByObjectSelectionSet();
                GetMainMapEditorObjectLayer()->SetVertexSelectionStateFromSelectionOwnerPolygonCount();
                m_metric_editing_mode_text = "Polygons";
                break;
                
            case Object::MM_VERTICES:
                if (GetMainMapEditorObjectLayer()->GetSelectedObjectCount() > 0)
                    GetMainMapEditorObjectLayer()->MaskSelectedVerticesByObjectSelectionSet();
                m_metric_editing_mode_text = "Vertices";
                break;
                
            default:
                ASSERT1(false && "Invalid Object::MetricMode")
                m_metric_editing_mode_text = "Invalid Mode";
                break;
        }
        m_sender_metric_editing_mode_changed.Signal(m_metric_editing_mode);
        m_sender_metric_editing_mode_text_changed.Signal(m_metric_editing_mode_text);
    }
}

void MapEditor2::WorldView::SetEditingSubMode (
    EditingSubMode const editing_sub_mode)
{
    if (m_editing_sub_mode != editing_sub_mode)
    {
        switch (editing_sub_mode)
        {
            case ESM_DEFAULT:
                fprintf(stderr, "setting ESM_DEFAULT\n");
                m_editing_sub_mode = editing_sub_mode;
                m_sender_status_bar_message_text_changed.Signal("Default Editing Mode");
                break;

            case ESM_DRAW_POLYGON:
                fprintf(stderr, "setting ESM_DRAW_POLYGON\n");
                // must have exactly 0 Objects or 1 Compound selected
                if (GetMainMapEditorObjectLayer()->GetSelectedObjectCount() == 0 ||
                    GetMainMapEditorObjectLayer()->GetSelectedCompoundCount() == 1)
                {
                    m_saved_metric_editing_mode = m_metric_editing_mode;
                    SetMetricEditingMode(Object::MM_POLYGONS);
                    m_editing_sub_mode = editing_sub_mode;
                    m_sender_status_bar_message_text_changed.Signal(
                        "Draw Polygon - Left-click and drag out a "
                        "polygon, or press ESCAPE to cancel.");
                }
                else
                {
                    m_sender_status_bar_message_text_changed.Signal(
                        "Draw Polygon - Error: You must have "
                        "exactly 0 objects or 1 compound selected.");
                }
                break;

            case ESM_POSITION_GLOBAL_ORIGIN_CURSOR:
                fprintf(stderr, "setting ESM_POSITION_GLOBAL_ORIGIN_CURSOR\n");
                m_editing_sub_mode = editing_sub_mode;
                m_sender_status_bar_message_text_changed.Signal(
                    "Position Global Origin Cursor - Left click to place.");
                break;

            case ESM_ADD_SPRITE:
                fprintf(stderr, "setting ESM_ADD_SPRITE\n");
                m_editing_sub_mode = editing_sub_mode;
                m_sender_status_bar_message_text_changed.Signal(
                    "Add Sprite - Left click to place.");
                break;
                
            default:
                ASSERT1(false && "Invalid EditingSubMode")
                break;
        }
    }
}

void MapEditor2::WorldView::CreateStaticSprite (std::string const &filename)
{
    // calculate the position to stick the new sprite
    FloatVector2 position(GetParallaxedWorldViewToWorld() * FloatVector2::ms_zero);
    // create the sprite
    Sprite *sprite = Sprite::Create(filename);
    // position the sprite
    sprite->SetTranslation(position);
    // TODO: set the scale to be proportionate to the view area

    // add the sprite to the world in the current object layer
    fprintf(stderr, "MapEditor2::WorldView::CreateStaticSprite(); creating a static sprite from filename \"%s\" at ", filename.c_str());
    FprintVector(stderr, position);
    GetWorld()->AddObject(sprite, GetMainObjectLayer());
    sprite->SetIsSelected(true);
}

void MapEditor2::WorldView::CreateSpriteEntity (std::string const &filename)
{
    // calculate the position to stick the new sprite
    FloatVector2 position(GetParallaxedWorldViewToWorld() * FloatVector2::ms_zero);
    // create the sprite
    SpriteEntity *sprite = SpriteEntity::Create(filename);
    // position the sprite
    sprite->SetTranslation(position);
    // TODO: set the scale to be proportionate to the view area

    // add the sprite to the world in the current object layer
    fprintf(stderr, "MapEditor2::WorldView::CreateSpriteEntity(); creating a sprite entity from filename \"%s\" at ", filename.c_str());
    FprintVector(stderr, position);
    GetWorld()->AddEntity(sprite, GetMainObjectLayer());
    sprite->SetIsSelected(true);
}

void MapEditor2::WorldView::SetCurrentGridScale (Uint32 const current_grid_scale)
{
    if (m_current_grid_scale != current_grid_scale)
    {
        m_current_grid_scale = current_grid_scale;
        m_sender_grid_scale_text_changed.Signal(GetCurrentGridScaleText());
    }
}

void MapEditor2::WorldView::SetPolygonTesselation (Uint32 polygon_tesselation)
{
    if (polygon_tesselation < 3)
        polygon_tesselation = 3;
        
    if (m_polygon_tesselation != polygon_tesselation)
    {
        m_polygon_tesselation = polygon_tesselation;
        m_sender_polygon_tesselation_text_changed.Signal(GetPolygonTesselationText());
    }
}

void MapEditor2::WorldView::SetPerObjectOriginX (Float const origin_x)
{
    fprintf(stderr, "MapEditor2::WorldView::SetPerObjectOriginX(%g);\n", origin_x);
    GetMainMapEditorObjectLayer()->ObjectSelectionSetAssignPerObjectTranslationX(origin_x);
}

void MapEditor2::WorldView::SetPerObjectOriginY (Float const origin_y)
{
    fprintf(stderr, "MapEditor2::WorldView::SetPerObjectOriginY(%g);\n", origin_y);
    GetMainMapEditorObjectLayer()->ObjectSelectionSetAssignPerObjectTranslationY(origin_y);
}

void MapEditor2::WorldView::SetPerObjectScale (Float const scale)
{
    fprintf(stderr, "MapEditor2::WorldView::SetPerObjectScale(%g);\n", scale);
    GetMainMapEditorObjectLayer()->ObjectSelectionSetAssignPerObjectScale(scale);
    UpdateObjectSelectionSetScale(scale);
}

void MapEditor2::WorldView::SetPerObjectAngle (Float const angle)
{
    fprintf(stderr, "MapEditor2::WorldView::SetPerObjectAngle(%g);\n", angle);
    GetMainMapEditorObjectLayer()->ObjectSelectionSetAssignPerObjectRotation(angle);
    UpdateObjectSelectionSetAngle(angle);
}

void MapEditor2::WorldView::SetObjectSelectionSetOriginX (Float const origin_x)
{
    FloatVector2 origin_delta =
        FloatVector2(origin_x - m_object_selection_set_origin[Dim::X], 0.0f);
    fprintf(stderr, "MapEditor2::WorldView::SetObjectSelectionSetOriginX(%g); origin_x_delta = %g\n", origin_x, origin_delta[Dim::X]);
    GetMainMapEditorObjectLayer()->ObjectSelectionSetTranslate(origin_delta);
}

void MapEditor2::WorldView::SetObjectSelectionSetOriginY (Float const origin_y)
{
    FloatVector2 origin_delta =
        FloatVector2(0.0f, origin_y - m_object_selection_set_origin[Dim::Y]);
    fprintf(stderr, "MapEditor2::WorldView::SetObjectSelectionSetOriginY(%g); origin_y_delta = %g\n", origin_y, origin_delta[Dim::Y]);
    GetMainMapEditorObjectLayer()->ObjectSelectionSetTranslate(origin_delta);
}

void MapEditor2::WorldView::SetObjectSelectionSetScale (Float const scale)
{
    Float scale_factor = scale / m_object_selection_set_scale;
    fprintf(stderr, "MapEditor2::WorldView::SetObjectSelectionSetScale(%g); scale_factor = %g\n", scale, scale_factor);
    // scale and rotate the object selection set
    GetMainMapEditorObjectLayer()->ObjectSelectionSetScale(
        scale_factor,
        GetOriginCursor(),
        GetTransformationMode());
    UpdateObjectSelectionSetScale(scale);
}

void MapEditor2::WorldView::SetObjectSelectionSetAngle (Float const angle)
{
    Float angle_delta = angle - m_object_selection_set_angle;
    fprintf(stderr, "MapEditor2::WorldView::SetObjectSelectionSetAngle(%g); angle_delta = %g\n", angle, angle_delta);
    GetMainMapEditorObjectLayer()->ObjectSelectionSetRotate(
        angle_delta,
        GetOriginCursor(),
        GetTransformationMode());
    UpdateObjectSelectionSetAngle(angle);
}

void MapEditor2::WorldView::SetPerEntityFirstMoment (Float const first_moment)
{
    fprintf(stderr, "MapEditor2::WorldView::SetPerEntityFirstMoment(%g);\n", first_moment);
    GetMainMapEditorObjectLayer()->
        ObjectSelectionSetAssignPerEntityFirstMoment(first_moment);
}

void MapEditor2::WorldView::SetPerEntityVelocityX (Float const velocity_x)
{
    fprintf(stderr, "MapEditor2::WorldView::SetPerEntityVelocityX(%g);\n", velocity_x);
    GetMainMapEditorObjectLayer()->
        ObjectSelectionSetAssignPerEntityVelocityX(velocity_x);
}

void MapEditor2::WorldView::SetPerEntityVelocityY (Float const velocity_y)
{
    fprintf(stderr, "MapEditor2::WorldView::SetPerEntityVelocityY(%g);\n", velocity_y);
    GetMainMapEditorObjectLayer()->
        ObjectSelectionSetAssignPerEntityVelocityY(velocity_y);
}

void MapEditor2::WorldView::SetPerEntitySpeed (Float const speed)
{
    fprintf(stderr, "MapEditor2::WorldView::SetPerEntitySpeed(%g);\n", speed);
    GetMainMapEditorObjectLayer()->
        ObjectSelectionSetAssignPerEntitySpeed(speed);
}

void MapEditor2::WorldView::SetPerEntityVelocityAngle (Float const velocity_angle)
{
    fprintf(stderr, "MapEditor2::WorldView::SetPerEntityVelocityAngle(%g);\n", velocity_angle);
    GetMainMapEditorObjectLayer()->
        ObjectSelectionSetAssignPerEntityVelocityAngle(velocity_angle);
}

void MapEditor2::WorldView::SetPerEntitySecondMoment (Float const second_moment)
{
    fprintf(stderr, "MapEditor2::WorldView::SetPerEntitySecondMoment(%g);\n", second_moment);
    GetMainMapEditorObjectLayer()->
        ObjectSelectionSetAssignPerEntitySecondMoment(second_moment);
}

void MapEditor2::WorldView::SetPerEntityAngularVelocity (Float const angular_velocity)
{
    fprintf(stderr, "MapEditor2::WorldView::SetPerEntityAngularVelocity(%g);\n", angular_velocity);
    GetMainMapEditorObjectLayer()->
        ObjectSelectionSetAssignPerEntityAngularVelocity(angular_velocity);
}

void MapEditor2::WorldView::SetPerEntityElasticity (Float const elasticity)
{
    fprintf(stderr, "MapEditor2::WorldView::SetPerEntityElasticity(%g);\n", elasticity);
    GetMainMapEditorObjectLayer()->
        ObjectSelectionSetAssignPerEntityElasticity(elasticity);
}

void MapEditor2::WorldView::SetPerEntityDensity (Float const density)
{
    fprintf(stderr, "MapEditor2::WorldView::SetPerEntityDensity(%g);\n", density);
    GetMainMapEditorObjectLayer()->
        ObjectSelectionSetAssignPerEntityDensity(density);
}

void MapEditor2::WorldView::SetPerEntityAppliesGravity (bool const applies_gravity)
{
    fprintf(stderr, "MapEditor2::WorldView::SetPerEntityAppliesGravity(%s);\n", BOOL_TO_STRING(applies_gravity));
    GetMainMapEditorObjectLayer()->
        ObjectSelectionSetAssignPerEntityAppliesGravity(applies_gravity);
}

void MapEditor2::WorldView::SetPerEntityReactsToGravity (bool const reacts_to_gravity)
{
    fprintf(stderr, "MapEditor2::WorldView::SetPerEntityReactsToGravity(%s);\n", BOOL_TO_STRING(reacts_to_gravity));
    GetMainMapEditorObjectLayer()->
        ObjectSelectionSetAssignPerEntityReactsToGravity(reacts_to_gravity);
}

void MapEditor2::WorldView::SetObjectSelectionSetFirstMoment (Float const first_moment)
{
    ASSERT1(first_moment > 0.0f)
    ASSERT1(m_object_selection_set_first_moment > 0.0f)
    Float first_moment_scale_factor =
        first_moment / m_object_selection_set_first_moment;
    fprintf(stderr, "MapEditor2::WorldView::SetObjectSelectionSetFirstMoment(%g); first_moment_scale_factor = %g\n", first_moment, first_moment_scale_factor);
    GetMainMapEditorObjectLayer()->
        ObjectSelectionSetScaleFirstMoment(first_moment_scale_factor);
}

void MapEditor2::WorldView::SetObjectSelectionSetVelocityX (Float const velocity_x)
{
    fprintf(stderr, "MapEditor2::WorldView::SetObjectSelectionSetVelocityX(%g);\n", velocity_x);
    // this action applies only to the single selected entity,
    // so it's the same as the corresponding per-entity function
    GetMainMapEditorObjectLayer()->
        ObjectSelectionSetAssignPerEntityVelocityX(velocity_x);
}

void MapEditor2::WorldView::SetObjectSelectionSetVelocityY (Float const velocity_y)
{
    fprintf(stderr, "MapEditor2::WorldView::SetObjectSelectionSetVelocityY(%g);\n", velocity_y);
    // this action applies only to the single selected entity,
    // so it's the same as the corresponding per-entity function
    GetMainMapEditorObjectLayer()->
        ObjectSelectionSetAssignPerEntityVelocityY(velocity_y);
}

void MapEditor2::WorldView::SetObjectSelectionSetSpeed (Float const speed)
{
    fprintf(stderr, "MapEditor2::WorldView::SetObjectSelectionSetSpeed(%g);\n", speed);
    // this action applies only to the single selected entity,
    // so it's the same as the corresponding per-entity function
    GetMainMapEditorObjectLayer()->
        ObjectSelectionSetAssignPerEntitySpeed(speed);
}

void MapEditor2::WorldView::SetObjectSelectionSetVelocityAngle (Float const velocity_angle)
{
    fprintf(stderr, "MapEditor2::WorldView::SetObjectSelectionSetVelocityAngle(%g);\n", velocity_angle);
    // this action applies only to the single selected entity,
    // so it's the same as the corresponding per-entity function
    GetMainMapEditorObjectLayer()->
        ObjectSelectionSetAssignPerEntityVelocityAngle(velocity_angle);
}

void MapEditor2::WorldView::SetObjectSelectionSetSecondMoment (Float const second_moment)
{
    fprintf(stderr, "MapEditor2::WorldView::SetObjectSelectionSetSecondMoment(%g);\n", second_moment);
    // this action applies only to the single selected entity,
    // so it's the same as the corresponding per-entity function
    GetMainMapEditorObjectLayer()->
        ObjectSelectionSetAssignPerEntitySecondMoment(second_moment);
}

void MapEditor2::WorldView::SetObjectSelectionSetAngularVelocity (Float const angular_velocity)
{
    fprintf(stderr, "MapEditor2::WorldView::SetObjectSelectionSetAngularVelocity(%g);\n", angular_velocity);
    // this action applies only to the single selected entity,
    // so it's the same as the corresponding per-entity function
    GetMainMapEditorObjectLayer()->
        ObjectSelectionSetAssignPerEntityAngularVelocity(angular_velocity);
}

void MapEditor2::WorldView::SetObjectSelectionSetElasticity (Float const elasticity)
{
    fprintf(stderr, "MapEditor2::WorldView::SetObjectSelectionSetElasticity(%g);\n", elasticity);
    // this action applies only to the single selected entity,
    // so it's the same as the corresponding per-entity function
    GetMainMapEditorObjectLayer()->
        ObjectSelectionSetAssignPerEntityElasticity(elasticity);
}

void MapEditor2::WorldView::SetObjectSelectionSetDensity (Float const density)
{
    fprintf(stderr, "MapEditor2::WorldView::SetObjectSelectionSetDensity(%g);\n", density);
    // this action applies only to the single selected entity,
    // so it's the same as the corresponding per-entity function
    GetMainMapEditorObjectLayer()->
        ObjectSelectionSetAssignPerEntityDensity(density);
}

void MapEditor2::WorldView::Draw (RenderContext const &render_context)
{
    // draw the normal view first
    Engine2::WorldView::Draw(render_context);

    PushParallaxedGLProjectionMatrix(
        render_context,
        GetMainMapEditorObjectLayer());

    FloatVector2 const transformed_mouse_position(
        GetParallaxedScreenToWorld() *
        GetParentWorldViewWidget()->GetLastMousePosition().StaticCast<Float>());
        
    // calculate the number of pixels in the view radius
    Float pixels_in_view_radius =
        0.5f * render_context.GetClipRect().GetSize().StaticCast<Float>().GetLength();
    // get the parallaxed view radius for the main object layer
    Float parallaxed_view_radius = GetParallaxedViewRadius(NULL);

    // draw the bounding circles for the objects
    if (m_draw_object_metrics)
    {
        // draw the object bounding circles in the correct colors
        GetMainMapEditorObjectLayer()->DrawMetrics(
            render_context,
            GetParallaxedWorldToScreen(),
            pixels_in_view_radius,
            GetViewCenter(),
            parallaxed_view_radius,
            m_metric_editing_mode);
    }

    // draw the selection circle (if appropriate)
    if (Singletons::KeyBinds()->GetIsKeyPressed(Key::RMOUSE) &&
        Singletons::KeyBinds()->GetIsEitherShiftKeyPressed() &&
        m_rmouse_dragged &&
        m_editing_sub_mode == ESM_DEFAULT)
    {
        Float selection_circle_radius =
            (transformed_mouse_position -
             m_rmouse_pressed_world_position).GetLength();
        Color selection_circle_color(0.5, 0.5, 1.0, 1.0);

        // draw the selection circle
        Render::DrawCircle(
            render_context,
            GetParallaxedWorldToScreen(),
            m_rmouse_pressed_world_position,
            selection_circle_radius,
            selection_circle_color);
    }

    // draw the polygon-creating circle/polygon (if appropriate)
    if (m_editing_sub_mode == ESM_DRAW_POLYGON &&
        Singletons::KeyBinds()->GetIsKeyPressed(Key::LMOUSE) &&
        m_lmouse_dragged)
    {
        ASSERT1(GetMainMapEditorObjectLayer()->GetSelectedObjectCount() == 0 ||
                GetMainMapEditorObjectLayer()->GetSelectedCompoundCount() == 1)
    
        FloatVector2 bounding_circle_ray(transformed_mouse_position);
        bounding_circle_ray -= m_lmouse_pressed_world_position;
        Float selection_circle_radius = bounding_circle_ray.GetLength();
        Color selection_circle_color(1.0f, 0.5f, 0.7f, 1.0f);

        // draw the selection circle
        Float polygon_angle = Math::Atan(bounding_circle_ray);
        Render::DrawPolygon(
            render_context,
            m_lmouse_pressed_world_position,
            selection_circle_radius,
            polygon_angle,
            selection_circle_color,
            m_polygon_tesselation);
    }

    // the gl projection matrix is popped before drawing the origin
    // cursors because the origin cursors must be drawn at a constant
    // size with respect to the screen.
    PopGLProjectionMatrix();

    // draw the origin cursors
    if (!GetMapEditorWorld()->GetIsRunning())
    {
        // draw the object selection set origin cursor
        if (!GetMainMapEditorObjectLayer()->GetIsObjectSelectionSetEmpty())
            DrawOriginCursor(
                render_context,
                GetMainMapEditorObjectLayer()->GetObjectSelectionSetOrigin(),
                Object::TM_SELECTION_SET_ORIGIN);
        // draw the global origin cursor
        DrawOriginCursor(
            render_context,
            m_origin_cursor_position,
            Object::TM_GLOBAL_ORIGIN_CURSOR);
    }

    // send the signals to update the status bar
    SignalEditDeltaText();
}

bool MapEditor2::WorldView::ProcessKeyEvent (EventKey const *const e)
{
    // handle all the editing-sub-mode-independent keys here
    if (e->GetIsKeyDownEvent())
    {
        switch (e->GetKeyCode())
        {
            case Key::HOME:
                SetPolygonTesselation(m_polygon_tesselation + 1);
                return true;

            case Key::END:
                SetPolygonTesselation(m_polygon_tesselation - 1);
                return true;
            
            case Key::PAGEUP:
                if (e->GetIsEitherAltKeyPressed())
                    GetMapEditorWorld()->IncrementMainObjectLayer();
                else
                    if (m_current_grid_scale < UINT32_UPPER_BOUND)
                        SetCurrentGridScale(m_current_grid_scale + 1);
                return true;

            case Key::PAGEDOWN:
                if (e->GetIsEitherAltKeyPressed())
                    GetMapEditorWorld()->DecrementMainObjectLayer();
                else
                    if (m_current_grid_scale > UINT32_LOWER_BOUND)
                        SetCurrentGridScale(m_current_grid_scale - 1);
                return true;

            case Key::KP_DIVIDE:
                if (m_rotation_accumulator < 0.0)
                    m_rotation_accumulator -=
                        m_rotation_increment *
                        static_cast<Sint32>(
                            m_rotation_accumulator / m_rotation_increment);
                m_rotation_accumulator += m_rotation_increment;
                return true;

            case Key::KP_MULTIPLY:
                if (m_rotation_accumulator > 0.0)
                    m_rotation_accumulator -=
                        m_rotation_increment *
                        static_cast<Sint32>(
                            m_rotation_accumulator / m_rotation_increment);
                m_rotation_accumulator -= m_rotation_increment;
                return true;

            case Key::KP_PLUS:
                if (m_zoom_accumulator < 0.0)
                    m_zoom_accumulator -= static_cast<Sint32>(m_zoom_accumulator);
                m_zoom_accumulator += 1.0;
                return true;

            case Key::KP_MINUS:
                if (m_zoom_accumulator > 0.0)
                    m_zoom_accumulator -= static_cast<Sint32>(m_zoom_accumulator);
                m_zoom_accumulator -= 1.0;
                return true;

            case Key::ESCAPE:
                SetEditingSubMode(ESM_DEFAULT);
                m_saved_metric_editing_mode = Object::MM_NUM_METRIC_MODES;
                return true;
                
            case Key::ONE:
                if (e->GetIsEitherAltKeyPressed())
                    SetTransformationMode(Object::TM_GLOBAL_ORIGIN_CURSOR);
                return true;

            case Key::TWO:
                if (e->GetIsEitherAltKeyPressed())
                    SetTransformationMode(Object::TM_SELECTION_SET_ORIGIN);
                return true;

            case Key::THREE:
                if (e->GetIsEitherAltKeyPressed())
                    SetTransformationMode(Object::TM_EACH_SELECTED_OBJECT_ORIGIN);
                return true;

            case Key::L:
                m_grid_line_type = static_cast<GridLineType>(
                    (static_cast<Uint32>(m_grid_line_type) + 1) %
                     GR_NUM_GRID_LINE_TYPES);
                return true;

            case Key::M:
                m_draw_object_metrics = !m_draw_object_metrics;
                return true;

            case Key::T:
                m_select_touching = !m_select_touching;
                return true;

            case Key::KP_ENTER:
                GetMapEditorWorld()->SetSavedMainObjectLayer(GetMainMapEditorObjectLayer());
                return true;

            default:
                break;
        }
    }

    // handle all the ESM_DEFAULT keys here
    if (e->GetIsKeyDownEvent() && m_editing_sub_mode == ESM_DEFAULT)
    {
        switch (e->GetKeyCode())
        {
            case Key::F12:
            {
                bool new_running_state = !GetMapEditorWorld()->GetIsRunning();
                // make sure to skip the time 'burp' to avoid physics unpleasantness
                if (new_running_state)
                    GetMapEditorWorld()->GetPhysicsHandler()->SetSkipTime(true);
                else
                    GetMainMapEditorObjectLayer()->ForceObjectSelectionSetSignals();
                GetMapEditorWorld()->SetIsRunning(new_running_state);
                return true;
            }

            case Key::SPACE:
                // TODO: make polygon version of this (cloning)
                if (GetMapEditorWorld()->GetIsRunning())
                    return true;
                if (!GetMainMapEditorObjectLayer()->GetIsObjectSelectionSetEmpty())
                {
                    FloatVector2 position_offset =
                        GetParallaxedScreenToWorld() * FloatVector2(0.1f, 0.1f) -
                        GetParallaxedScreenToWorld() * FloatVector2::ms_zero;
                    GetMainMapEditorObjectLayer()->ObjectSelectionSetClone(
                        position_offset);
                }
                /*
                else
                {
                    FileDialog *dialog;
                    if (e->GetIsEitherAltKeyPressed())
                    {
                        dialog = new FileDialog(
                            "Load image as static sprite",
                            FilePanel::OP_OPEN,
                            GetParentWorldViewWidget(),
                            "static sprite creation dialog");
                        dialog->Resize(dialog->GetParent()->GetSize() * 4 / 5);
                        dialog->CenterOnWidget(dialog->GetParent());
                        SignalHandler::Connect1(
                            dialog->SenderSubmitFilename(),
                            ReceiverCreateStaticSprite());
                    }
                    else
                    {
                        dialog = new FileDialog(
                            "Load image as sprite entity",
                            FilePanel::OP_OPEN,
                            GetParentWorldViewWidget(),
                            "sprite entity creation dialog");
                        dialog->Resize(dialog->GetParent()->GetSize() * 4 / 5);
                        dialog->CenterOnWidget(dialog->GetParent());
                        SignalHandler::Connect1(
                            dialog->SenderSubmitFilename(),
                            ReceiverCreateSpriteEntity());
                    }
                }
                */
                return true;

            case Key::BACKSPACE:
                if (!GetMapEditorWorld()->GetIsRunning())
                {
                    switch (m_metric_editing_mode)
                    {
                        case Object::MM_TRANSFORMATION:
                        case Object::MM_LINEAR_VELOCITY:
                        case Object::MM_ANGULAR_VELOCITY:
                            GetMainMapEditorObjectLayer()->ObjectSelectionSetDelete();
                            break;

                        case Object::MM_POLYGONS:
                            GetMainMapEditorObjectLayer()->DeleteSelectedPolygons();
                            break;

                        case Object::MM_VERTICES:
                            // TODO: delete selected vertices
                            break;

                        default:
                            ASSERT1(false && "Invalid Object::MetricMode")
                            break;
                    }
                }
                return true;

            case Key::F1:
                SetMetricEditingMode(Object::MM_TRANSFORMATION);
                return true;

            case Key::F2:
                SetMetricEditingMode(Object::MM_LINEAR_VELOCITY);
                return true;

            case Key::F3:
                SetMetricEditingMode(Object::MM_ANGULAR_VELOCITY);
                return true;

            case Key::F4:
                SetMetricEditingMode(Object::MM_POLYGONS);
                return true;
                
            case Key::F5:
                SetMetricEditingMode(Object::MM_VERTICES);
                return true;
                                                                
            case Key::A:
                switch (m_metric_editing_mode)
                {
                    case Object::MM_TRANSFORMATION:
                    case Object::MM_LINEAR_VELOCITY:
                    case Object::MM_ANGULAR_VELOCITY:
                        GetMainMapEditorObjectLayer()->SelectAllObjects();
                        break;

                    case Object::MM_POLYGONS:
                        {
                            bool mask_by_object_selection_set =
                                GetMainMapEditorObjectLayer()->GetSelectedCompoundCount() > 0;
                            GetMainMapEditorObjectLayer()->SelectAllPolygons(
                                mask_by_object_selection_set);
                        }
                        break;

                    case Object::MM_VERTICES:
                        {
                            bool mask_by_object_selection_set =
                                GetMainMapEditorObjectLayer()->GetSelectedCompoundCount() > 0;
                            GetMainMapEditorObjectLayer()->SelectAllVertices(
                                mask_by_object_selection_set);
                        }
                        break;

                    default:
                        ASSERT1(false && "Invalid Object::MetricMode")
                        break;
                }
                return true;

            case Key::E:
                if (GetMainMapEditorObjectLayer()->GetSelectedNonEntityCount() == 1)
                {
                    // clone the single selected non-entity as an entity
                    Object *object = GetMainMapEditorObjectLayer()->GetSingleSelectedNonEntity();
                    Entity *entity = object->CreateEntityClone();
                    ASSERT1(entity != NULL)
                    // move the object's saved entity guts over to the entity
                    entity->SetEntityGuts(object->GetSavedEntityGuts());
                    object->SetSavedEntityGuts(NULL);
                    // delete the old non-entity and add the entity to the world                    
                    GetMainMapEditorObjectLayer()->ObjectSelectionSetDelete();
                    GetWorld()->AddObject(entity, GetMainObjectLayer());
                    entity->SetIsSelected(true);
                    m_sender_status_bar_message_text_changed.Signal(
                        "Object converted into entity.");
                }
                else
                    m_sender_status_bar_message_text_changed.Signal(
                        "You must have exactly one selected non-entity in "
                        "order to perform an object-to-entity conversion.");
                return true;
                
            case Key::G:
                // set the global origin cursor's position
                SetEditingSubMode(ESM_POSITION_GLOBAL_ORIGIN_CURSOR);
                return true;
                
            case Key::I:
                switch (m_metric_editing_mode)
                {
                    case Object::MM_TRANSFORMATION:
                    case Object::MM_LINEAR_VELOCITY:
                    case Object::MM_ANGULAR_VELOCITY:
                        GetMainMapEditorObjectLayer()->InvertObjectSelectionSet();
                        break;

                    case Object::MM_POLYGONS:
                        {
                            bool mask_by_object_selection_set =
                                GetMainMapEditorObjectLayer()->GetSelectedCompoundCount() > 0;
                            GetMainMapEditorObjectLayer()->InvertPolygonSelectionSet(
                                mask_by_object_selection_set);
                        }
                        break;

                    case Object::MM_VERTICES:
                        {
                            bool mask_by_object_selection_set =
                                GetMainMapEditorObjectLayer()->GetSelectedCompoundCount() > 0;
                            GetMainMapEditorObjectLayer()->InvertVertexSelectionSet(
                                mask_by_object_selection_set);
                        }
                        break;

                    default:
                        ASSERT1(false && "Invalid Object::MetricMode")
                        break;
                }
                return true;

            case Key::N:
                if (e->GetIsEitherShiftKeyPressed())
                {
                    GetMainMapEditorObjectLayer()->ClearObjectSelectionSet();
                    GetMainMapEditorObjectLayer()->ClearPolygonSelectionSet();
                    GetMainMapEditorObjectLayer()->ClearVertexSelectionSet();
                }
                else
                {
                    switch (m_metric_editing_mode)
                    {
                        case Object::MM_TRANSFORMATION:
                        case Object::MM_LINEAR_VELOCITY:
                        case Object::MM_ANGULAR_VELOCITY:
                            GetMainMapEditorObjectLayer()->ClearObjectSelectionSet();
                            break;
                            
                        case Object::MM_POLYGONS:
                            GetMainMapEditorObjectLayer()->ClearPolygonSelectionSet();
                            break;
    
                        case Object::MM_VERTICES:
                            GetMainMapEditorObjectLayer()->ClearVertexSelectionSet();
                            break;
    
                        default:
                            ASSERT1(false && "Invalid Object::MetricMode")
                            break;
                    }
                }
                return true;

            case Key::O:
                if (GetMainMapEditorObjectLayer()->GetSelectedEntityCount() == 1)
                {
                    // clone the single selected entity as a non-entity
                    Entity *entity = GetMainMapEditorObjectLayer()->GetSingleSelectedEntity();
                    Object *object = entity->CreateNonEntityClone();
                    ASSERT1(object != NULL)
                    // move the entity's guts over to the non-entity's saved guts
                    object->SetSavedEntityGuts(entity->GetEntityGuts());
                    entity->SetEntityGuts(NULL);
                    // delete the old entity and add the non-entity to the world
                    GetMainMapEditorObjectLayer()->ObjectSelectionSetDelete();
                    GetWorld()->AddObject(object, GetMainObjectLayer());
                    object->SetIsSelected(true);
                    m_sender_status_bar_message_text_changed.Signal(
                        "Entity converted into object.");
                }
                else
                    m_sender_status_bar_message_text_changed.Signal(
                        "You must have exactly one selected entity in "
                        "order to perform an entity-to-object conversion.");
                return true;
                
            case Key::P:
                SetEditingSubMode(ESM_DRAW_POLYGON);
                return true;

            case Key::S:
                SetEditingSubMode(ESM_ADD_SPRITE);
                return true;
                
            case Key::W:
                if (GetMainMapEditorObjectLayer()->GetSelectedCompoundCount() == 1 &&
                    GetMainMapEditorObjectLayer()->GetSelectedVertexCount() >= 2)
                {
                    Compound *compound =
                        GetMainMapEditorObjectLayer()->GetSingleSelectedCompound();
                    ASSERT1(compound != NULL)
                    Compound::WeldReturnStatus status = compound->WeldSelectedVertices();
                    switch (status)
                    {
                        case Compound::W_SUCCESSFUL:
                            m_sender_status_bar_message_text_changed.Signal(
                                "Weld Vertices - Successful.");
                            break;
                            
                        case Compound::W_WOULD_CAUSE_DEGENERATION:
                            m_sender_status_bar_message_text_changed.Signal(
                                "Weld Vertices - Error: That weld would cause "
                                "polygon degeneration.");
                            break;
                            
                        case Compound::W_WOULD_CAUSE_PINCH:
                            m_sender_status_bar_message_text_changed.Signal(
                                "Weld Vertices - Error: That weld would cause "
                                "partial polygon degeneration.");
                            break;
                            
                        default:
                            ASSERT1(false && "Invalid Compound::WeldReturnStatus")
                            break;
                    }
                }
                else
                {
                    m_sender_status_bar_message_text_changed.Signal(
                        "Weld Vertices - Error: Must have exactly one "
                        "compound and at least two vertices selected.");
                }
                return true;

            case Key::U:
                switch (m_metric_editing_mode)
                {
                    case Object::MM_TRANSFORMATION:
                    case Object::MM_LINEAR_VELOCITY:
                    case Object::MM_ANGULAR_VELOCITY:
                        // this does nothing
                        break;

                    case Object::MM_POLYGONS:
                        // unweld the vertices of the selected polygons
                        GetMainMapEditorObjectLayer()->UnweldSelectedPolygons();
                        m_sender_status_bar_message_text_changed.Signal(
                            "Unwelded Selected Polygons.");
                        break;

                    case Object::MM_VERTICES:
                        // unweld the selected vertices
                        GetMainMapEditorObjectLayer()->UnweldSelectedVertices();
                        m_sender_status_bar_message_text_changed.Signal(
                            "Unwelded Selected Vertices.");
                        break;

                    default:
                        ASSERT1(false && "Invalid Object::MetricMode")
                        break;
                }
                return true;
                
            default:
                break;
        }
    }
    // handle all the ESM_DRAW_POLYGON keys here
    else if (e->GetIsKeyDownEvent() && m_editing_sub_mode == ESM_DRAW_POLYGON)
    {
        // currently no keys
//         switch (e->GetKeyCode())
//         {
//             default:
//                 break;
//         }
    }
    // handle all the ESM_POSITION_GLOBAL_ORIGIN_CURSOR keys here
    else if (e->GetIsKeyDownEvent() && m_editing_sub_mode == ESM_POSITION_GLOBAL_ORIGIN_CURSOR)
    {
        // currently no keys
//         switch (e->GetKeyCode())
//         {
//             default:
//                 break;
//         }
    }
    // handle all the ESM_ADD_SPRITE keys here
    else if (e->GetIsKeyDownEvent() && m_editing_sub_mode == ESM_ADD_SPRITE)
    {
        // currently no keys
//         switch (e->GetKeyCode())
//         {
//             default:
//                 break;
//         }
    }

    return true;
}

bool MapEditor2::WorldView::ProcessMouseButtonEvent (EventMouseButton const *const e)
{
    FloatVector2 const transformed_mouse_event_position(
        GetParallaxedScreenToWorld() * e->GetPosition().StaticCast<Float>());

    // track the lmouse/rmouse pressed positions, resetting the dragged
    // properties if appropriate
    if (e->GetIsMouseButtonDownEvent())
    {
        if (e->GetButtonCode() == Key::LMOUSE)
        {
            m_lmouse_pressed_position = e->GetPosition().StaticCast<Float>();
            m_lmouse_pressed_world_position = transformed_mouse_event_position;
            m_lmouse_dragged = false;
        }
        else if (e->GetButtonCode() == Key::RMOUSE)
        {
            m_rmouse_pressed_position = e->GetPosition().StaticCast<Float>();
            m_rmouse_pressed_world_position = transformed_mouse_event_position;
            m_rmouse_dragged = false;
        }
    }

    // if m_clear_object_selection_upon_mouse_button_up is set, clear it
    // and clear the object selection set
    if (m_clear_object_selection_upon_mouse_button_up)
    {
        m_clear_object_selection_upon_mouse_button_up = false;
        GetMainMapEditorObjectLayer()->ClearObjectSelectionSet();
    }
    
    // if m_clear_polygon_selection_upon_mouse_button_up is set, clear it
    // and clear the polygon selection set
    if (m_clear_polygon_selection_upon_mouse_button_up)
    {
        m_clear_polygon_selection_upon_mouse_button_up = false;
        GetMainMapEditorObjectLayer()->ClearPolygonSelectionSet();
    }
    
    // if m_clear_vertex_selection_upon_mouse_button_up is set, clear it
    // and clear the vertex selection set
    if (m_clear_vertex_selection_upon_mouse_button_up)
    {
        m_clear_vertex_selection_upon_mouse_button_up = false;
        GetMainMapEditorObjectLayer()->ClearVertexSelectionSet();
    }
    
    // right mouse button up event + shift key is held down
    if (e->GetIsMouseButtonUpEvent() &&
        e->GetButtonCode() == Key::RMOUSE &&
        e->GetIsEitherShiftKeyPressed())
    {
        Object::SelectionOperation selection_operation = GetSelectionOperation(e);

        switch (m_metric_editing_mode)
        {
            case Object::MM_TRANSFORMATION:
            case Object::MM_LINEAR_VELOCITY:
            case Object::MM_ANGULAR_VELOCITY:
                // clear the object selection set if we're using 'equals' selection
                if (selection_operation == Object::SO_EQUALS)
                    GetMainMapEditorObjectLayer()->ClearObjectSelectionSet();
        
                if (m_rmouse_dragged)
                {
                    // circle selection
                    Float selection_circle_radius =
                        (transformed_mouse_event_position -
                         m_rmouse_pressed_world_position).GetLength();
        
                    GetMainMapEditorObjectLayer()->SelectObjectsByCircle(
                        m_rmouse_pressed_world_position,
                        selection_circle_radius,
                        selection_operation,
                        m_select_touching);
                }
                else
                {
                    // single object selection
        
                    // apply the operation to the object at the point, if it exists.
                    GetMainMapEditorObjectLayer()->SelectSmallestObjectTouchingPoint(
                        transformed_mouse_event_position,
                        selection_operation);
                }                
                break;
                
            case Object::MM_POLYGONS:
                // polygons may only be selected by stationary clicks (for now)
                if (!m_rmouse_dragged)
                {
                    if (selection_operation == Object::SO_EQUALS)
                        GetMainMapEditorObjectLayer()->ClearPolygonSelectionSet();
                        
                    GetMainMapEditorObjectLayer()->
                        SelectSmallestPolygonTouchingPoint(
                            m_rmouse_pressed_world_position,
                            selection_operation);
                }
                break;

            case Object::MM_VERTICES:
                if (m_rmouse_dragged)
                {
                    // select vertices by drawn out circles
                    if (selection_operation == Object::SO_EQUALS)
                        GetMainMapEditorObjectLayer()->ClearVertexSelectionSet();
                    
                    Float selection_circle_radius =
                        (transformed_mouse_event_position -
                         m_rmouse_pressed_world_position).GetLength();
        
                    GetMainMapEditorObjectLayer()->SelectVerticesByCircle(
                        m_rmouse_pressed_world_position,
                        selection_circle_radius,
                        selection_operation);                
                }
                else
                {
                    // select nearest vertex
                    if (selection_operation == Object::SO_EQUALS)
                        GetMainMapEditorObjectLayer()->ClearVertexSelectionSet();
                    
                    Float parallaxed_view_radius = GetParallaxedViewRadius(NULL);
                    GetMainMapEditorObjectLayer()->SelectNearestVertex(
                        transformed_mouse_event_position,
                        0.02f * parallaxed_view_radius,
                        selection_operation);
                }
                break;

            default:
                ASSERT1(false && "Invalid Object::MetricMode")
                break;
        }
    }

    if (m_editing_sub_mode == ESM_DRAW_POLYGON &&
        m_lmouse_dragged &&
        e->GetIsMouseButtonUpEvent() &&
        e->GetButtonCode() == Key::LMOUSE)
    {
        ASSERT1(m_metric_editing_mode == Object::MM_POLYGONS)
        ASSERT1(GetMainMapEditorObjectLayer()->GetSelectedObjectCount() == 0 ||
                GetMainMapEditorObjectLayer()->GetSelectedCompoundCount() == 1)
        ASSERT1(m_saved_metric_editing_mode != Object::MM_NUM_METRIC_MODES)
    
        // create the polygon that has been dragged out
        FloatVector2 bounding_circle_ray(transformed_mouse_event_position);
        bounding_circle_ray -= m_lmouse_pressed_world_position;
        Float polygon_angle = Math::Atan(bounding_circle_ray);

        // if there are no selected Compounds, create a new compound from
        // the drawn out polygon and add it to the object layer
        if (GetMainMapEditorObjectLayer()->GetSelectedObjectCount() == 0)
        {
            fprintf(stderr, "Drawing polygon into new compound\n");
            Compound *compound =
                new Compound(
                    m_lmouse_pressed_world_position,
                    bounding_circle_ray.GetLength(),
                    polygon_angle,
                    m_polygon_tesselation,
                    Singletons::ResourceLibrary()->
                        LoadFilename<GLTexture>(
                            GLTexture::Create,
                            "resources/texture1.png"));
            
            GetWorld()->AddObject(compound, GetMainObjectLayer());
            if (e->GetIsEitherShiftKeyPressed())
                compound->SetIsSelected(true);
        }
        // otherwise, add it to the single selected compound
        else
        {
            fprintf(stderr, "Drawing polygon into existing selected compound\n");
            Compound *compound =
                GetMainMapEditorObjectLayer()->GetSingleSelectedCompound();
            ASSERT1(compound != NULL)
            compound->AddDrawnPolygon(
                m_lmouse_pressed_world_position,
                bounding_circle_ray.GetLength(),
                polygon_angle,
                m_polygon_tesselation,
                Singletons::ResourceLibrary()->
                    LoadFilename<GLTexture>(
                        GLTexture::Create,
                        "resources/texture1.png"));
        }

        // reset the editing sub mode back to default
        SetEditingSubMode(ESM_DEFAULT);
        SetMetricEditingMode(m_saved_metric_editing_mode);
        m_saved_metric_editing_mode = Object::MM_NUM_METRIC_MODES;
    }

    if (m_editing_sub_mode == ESM_POSITION_GLOBAL_ORIGIN_CURSOR &&
        e->GetIsMouseButtonUpEvent() &&
        e->GetButtonCode() == Key::LMOUSE)
    {
        ASSERT1(m_saved_metric_editing_mode == Object::MM_NUM_METRIC_MODES)

        // set the global origin cursor's position
        m_origin_cursor_position = transformed_mouse_event_position;
        if (e->GetIsEitherShiftKeyPressed())
            SetTransformationMode(Object::TM_GLOBAL_ORIGIN_CURSOR);
        SetEditingSubMode(ESM_DEFAULT);
    }

    if (m_editing_sub_mode == ESM_ADD_SPRITE &&
        e->GetIsMouseButtonUpEvent() &&
        e->GetButtonCode() == Key::LMOUSE)
    {
        ASSERT1(m_saved_metric_editing_mode == Object::MM_NUM_METRIC_MODES)

        GetMainMapEditorObjectLayer()->ClearObjectSelectionSet();
        
        // create the sprite
        Sprite *sprite = Sprite::Create("resources/mars.png");
        // position the sprite
        sprite->SetTranslation(transformed_mouse_event_position);
        // TODO: set the scale to be proportionate to the view area?

        // add the sprite to the world in the current object layer
        GetWorld()->AddObject(sprite, GetMainObjectLayer());
        sprite->SetIsSelected(true);
        
        SetEditingSubMode(ESM_DEFAULT);
    }

    if ((m_metric_editing_mode == Object::MM_TRANSFORMATION ||
         m_metric_editing_mode == Object::MM_LINEAR_VELOCITY ||
         m_metric_editing_mode == Object::MM_ANGULAR_VELOCITY)
        &&
        m_editing_sub_mode == ESM_DEFAULT &&
        GetMainMapEditorObjectLayer()->GetSelectedObjectCount() == 0 &&
        e->GetIsMouseButtonDownEvent() &&
        e->GetButtonCode() == Key::LMOUSE &&
        !e->GetIsEitherAltKeyPressed() &&
        !e->GetIsEitherControlKeyPressed())
    {
        // select the smallest object that is under the mouse cursor.
        GetMainMapEditorObjectLayer()->SelectSmallestObjectTouchingPoint(
            transformed_mouse_event_position,
            Object::SO_EQUALS);
        m_clear_object_selection_upon_mouse_button_up = true;
    }
        
    if (m_metric_editing_mode == Object::MM_POLYGONS &&
        m_editing_sub_mode == ESM_DEFAULT &&
        GetMainMapEditorObjectLayer()->GetSelectedPolygonCount() == 0 &&
        e->GetIsMouseButtonDownEvent() &&
        e->GetButtonCode() == Key::LMOUSE &&
        !e->GetIsEitherAltKeyPressed() &&
        !e->GetIsEitherControlKeyPressed())
    {
        // select the smallest polygon under the cursor
        GetMainMapEditorObjectLayer()->
            SelectSmallestPolygonTouchingPoint(
                transformed_mouse_event_position,
                Object::SO_EQUALS);
        m_clear_polygon_selection_upon_mouse_button_up = true;
    }
        
    if (m_metric_editing_mode == Object::MM_VERTICES &&
        m_editing_sub_mode == ESM_DEFAULT &&
        GetMainMapEditorObjectLayer()->GetSelectedVertexCount() == 0 &&
        e->GetIsMouseButtonDownEvent() &&
        e->GetButtonCode() == Key::LMOUSE &&
        !e->GetIsEitherAltKeyPressed() &&
        !e->GetIsEitherControlKeyPressed())
    {
        // select the closest vertex that's within 2% of the radius of the view
        Float parallaxed_view_radius = GetParallaxedViewRadius(NULL);
        GetMainMapEditorObjectLayer()->SelectNearestVertex(
            transformed_mouse_event_position,
            0.02f * parallaxed_view_radius,
            Object::SO_EQUALS);
        m_clear_vertex_selection_upon_mouse_button_up = true;
    }
        
    return true;
}

bool MapEditor2::WorldView::ProcessMouseWheelEvent (EventMouseWheel const *const e)
{
    // don't allow mouse wheel input while the widget is not focused
    if (!GetParentWorldViewWidget()->GetIsFocused())
        return false;

    if (e->GetIsEitherAltKeyPressed())
    {
        // when the alt key is held down, change the view's rotation

        if (e->GetButtonCode() == Key::MOUSEWHEELUP)
        {
            if (m_rotation_accumulator > 0.0)
                m_rotation_accumulator -=
                    m_rotation_increment *
                    (Sint32)(m_rotation_accumulator / m_rotation_increment);
            m_rotation_accumulator -= m_rotation_increment;
        }
        else if (e->GetButtonCode() == Key::MOUSEWHEELDOWN)
        {
            if (m_rotation_accumulator < 0.0)
                m_rotation_accumulator -=
                    m_rotation_increment *
                    (Sint32)(m_rotation_accumulator / m_rotation_increment);
            m_rotation_accumulator += m_rotation_increment;
        }
        else
            ASSERT0(false && "Invalid mouse wheel event (button code not recognized)")
    }
    else
    {
        // otherwise, change the view's zoom

        if (e->GetButtonCode() == Key::MOUSEWHEELUP)
        {
            if (m_zoom_accumulator < 0.0)
                m_zoom_accumulator -= (Sint32)m_zoom_accumulator;
            m_zoom_accumulator += 1.0;
        }
        else if (e->GetButtonCode() == Key::MOUSEWHEELDOWN)
        {
            if (m_zoom_accumulator > 0.0)
                m_zoom_accumulator -= (Sint32)m_zoom_accumulator;
            m_zoom_accumulator -= 1.0;
        }
        else
            ASSERT0(false && "Invalid mouse wheel event (button code not recognized)")
    }

    return true;
}

bool MapEditor2::WorldView::ProcessMouseMotionEvent (EventMouseMotion const *const e)
{
    // track the lmouse/rmouse dragged properties
    if (e->GetIsLeftMouseButtonPressed())
        m_lmouse_dragged = true;
    if (e->GetIsRightMouseButtonPressed())
        m_rmouse_dragged = true;

    // get the mouse movement's delta, in world coordinates
    FloatVector2 position_delta(
        GetParallaxedScreenToWorld() * FloatVector2::ms_zero -
        GetParallaxedScreenToWorld() * e->GetDelta().StaticCast<Float>());
    FloatVector2 last_mouse_position(
        GetParallaxedScreenToWorld() *
        GetParentWorldViewWidget()->GetLastMousePosition().StaticCast<Float>());
    FloatVector2 current_mouse_position(
        GetParallaxedScreenToWorld() * e->GetPosition().StaticCast<Float>());

    // the right mouse is being held and neither shift key is pressed, drag the view
    if (e->GetIsRightMouseButtonPressed() &&
        !e->GetIsEitherShiftKeyPressed())
    {
        // translate the view by the transformed delta
        MoveView(position_delta);
    }

    // left mouse dragging is for editing object properties
    // (the alt key can not be held down during this)
    if (!GetMapEditorWorld()->GetIsRunning() &&
        m_editing_sub_mode == ESM_DEFAULT &&
        e->GetIsLeftMouseButtonPressed() &&
        !e->GetIsEitherAltKeyPressed())
    {
        // the delta is reversed from the direction we want, negate it.
        position_delta = -position_delta;

        switch (m_metric_editing_mode)
        {
            case Object::MM_TRANSFORMATION:
                TransformationEdit(
                    last_mouse_position,
                    current_mouse_position,
                    position_delta,
                    e);
                break;

            case Object::MM_LINEAR_VELOCITY:
                LinearVelocityEdit(
                    last_mouse_position,
                    current_mouse_position,
                    position_delta,
                    e);
                break;

            case Object::MM_ANGULAR_VELOCITY:
                AngularVelocityEdit(
                    last_mouse_position,
                    current_mouse_position,
                    e);
                break;

            case Object::MM_POLYGONS:
                // selecting a polygon secretly selects its vertices,
                // so that it can use vertex editing code for transformations.
            case Object::MM_VERTICES:
                VerticesEdit(
                    last_mouse_position,
                    current_mouse_position,
                    position_delta,
                    e);
                break;
                
            default:
                ASSERT0(false && "Invalid metric mode")
                break;
        }
    }
    else
    {
        // if we're not currently editing something with the left mouse,
        // reset the deltas which display the editing values
        ResetEditDeltas();
    }

    return true;
}

void MapEditor2::WorldView::HandleUnfocus ()
{
    SetEditingSubMode(ESM_DEFAULT);
}

void MapEditor2::WorldView::HandleAttachedWorld ()
{
    DeleteAndNullify(m_view_owned_world);
    if (m_world != NULL)
        ConnectSignals();
    m_view_owned_world = DStaticCast<World *>(m_world);
}

void MapEditor2::WorldView::ProcessFrameOverride ()
{
    // don't do anything if this view is hidden
    if (GetParentWorldViewWidget()->GetIsHidden())
        return;

    // handle view zooming (from accumulated mouse wheel events)
    {
        Float zoom_by_power;

        if (m_zoom_accumulator > 0)
            zoom_by_power = Min(m_zoom_accumulator, m_zoom_speed*GetFrameDT());
        else if (m_zoom_accumulator < 0)
            zoom_by_power = Max(m_zoom_accumulator, -m_zoom_speed*GetFrameDT());
        else
            zoom_by_power = 0.0;

        ZoomView(Math::Pow(m_zoom_increment, zoom_by_power));
        m_zoom_accumulator -= zoom_by_power;
    }

    // handle view rotation (from acculumated mouse motion events)
    {
        Float rotate_by_angle;

        if (m_rotation_accumulator > 0)
            rotate_by_angle = Min(m_rotation_accumulator,
                                  m_rotation_speed*GetFrameDT());
        else if (m_rotation_accumulator < 0)
            rotate_by_angle = Max(m_rotation_accumulator,
                                  -m_rotation_speed*GetFrameDT());
        else
            rotate_by_angle = 0.0;

        RotateView(rotate_by_angle);
        m_rotation_accumulator -= rotate_by_angle;
    }

    // handle view movement (from the arrow keys)
    if (GetParentWorldViewWidget()->GetIsFocused())
    {
        // movement basis vectors
        FloatVector2 origin(
            GetParallaxedWorldViewToWorld() * FloatVector2::ms_zero);
        FloatVector2 right(
            GetParallaxedWorldViewToWorld() * FloatVector2(1.0, 0.0) - origin);
        FloatVector2 up(
            GetParallaxedWorldViewToWorld() * FloatVector2(0.0, 1.0) - origin);

        Sint8 left_right_input =
            static_cast<Sint8>(Singletons::KeyBinds()->GetIsKeyPressed(Key::RIGHT)) -
            static_cast<Sint8>(Singletons::KeyBinds()->GetIsKeyPressed(Key::LEFT));
        Sint8 up_down_input =
            static_cast<Sint8>(Singletons::KeyBinds()->GetIsKeyPressed(Key::UP)) -
            static_cast<Sint8>(Singletons::KeyBinds()->GetIsKeyPressed(Key::DOWN));

        right *= (Float)left_right_input;
        up *= (Float)up_down_input;

        MoveView(GetFrameDT() * m_key_movement_speed_factor * (right + up));
    }

    // WorldView owns the world it views, so it's in charge
    // of calling the world's ProcessFrame.
    GetMapEditorWorld()->ProcessFrame(GetFrameTime());
}

MapEditor2::ObjectLayer *MapEditor2::WorldView::GetMainMapEditorObjectLayer () const
{
    return DStaticCast<ObjectLayer *>(GetMainObjectLayer());
}

MapEditor2::Object::SelectionOperation MapEditor2::WorldView::GetSelectionOperation (
    EventMouseButton const *const e) const
{
    Sint8 lookup_index = (e->GetIsEitherControlKeyPressed() ? 2 : 0) +
                         (e->GetIsEitherAltKeyPressed() ? 1 : 0);
    return m_selection_operation_lookup[lookup_index];
}

void MapEditor2::WorldView::SetSelectedObjectCount (
    Uint32 const selected_object_count)
{
    if (m_selected_object_count != selected_object_count)
    {
        m_selected_object_count = selected_object_count;
        m_sender_no_objects_are_selected.Signal(m_selected_object_count == 0);

        ChangeSelectionObjects();
    }
}

void MapEditor2::WorldView::UpdateObjectSelectionSetOrigin (
    FloatVector2 const &object_selection_set_origin)
{
    if (m_object_selection_set_origin != object_selection_set_origin)
    {
        m_object_selection_set_origin = object_selection_set_origin;
        m_sender_object_selection_set_origin_changed.Signal(
            m_object_selection_set_origin);
    }
}

void MapEditor2::WorldView::UpdateObjectSelectionSetScale (
    Float const object_selection_set_scale)
{
    if (m_object_selection_set_scale != object_selection_set_scale)
    {
        m_object_selection_set_scale = object_selection_set_scale;
        m_sender_object_selection_set_scale_changed.Signal(m_object_selection_set_scale);
    }
}

void MapEditor2::WorldView::UpdateObjectSelectionSetAngle (
    Float const object_selection_set_angle)
{
    Float canonical_angle = Math::GetCanonicalAngle(object_selection_set_angle);
    if (m_object_selection_set_angle != canonical_angle)
    {
        m_object_selection_set_angle = canonical_angle;
        m_sender_object_selection_set_angle_changed.Signal(m_object_selection_set_angle);
    }
}

void MapEditor2::WorldView::SetSelectedEntityCount (
    Uint32 const selected_entity_count)
{
    if (m_selected_entity_count != selected_entity_count)
    {
        m_selected_entity_count = selected_entity_count;
        m_sender_no_entities_are_selected.Signal(m_selected_entity_count == 0);
        m_sender_selected_entity_count_changed.Signal(m_selected_entity_count);

        ChangeSelectionEntities();
    }
}

void MapEditor2::WorldView::UpdateObjectSelectionSetFirstMoment (
    Float const object_selection_set_first_moment)
{
    if (m_object_selection_set_first_moment != object_selection_set_first_moment)
    {
        m_object_selection_set_first_moment = object_selection_set_first_moment;
        m_sender_object_selection_set_first_moment_changed.Signal(
            m_object_selection_set_first_moment);
    }
}

void MapEditor2::WorldView::UpdateObjectSelectionSetVelocity (
    FloatVector2 const &object_selection_set_velocity)
{
    if (m_object_selection_set_velocity != object_selection_set_velocity)
    {
        m_object_selection_set_velocity = object_selection_set_velocity;
        m_sender_object_selection_set_velocity_changed.Signal(
            m_object_selection_set_velocity);
    }
}

void MapEditor2::WorldView::UpdateObjectSelectionSetSecondMoment (
    Float const object_selection_set_second_moment)
{
    if (m_object_selection_set_second_moment != object_selection_set_second_moment)
    {
        m_object_selection_set_second_moment = object_selection_set_second_moment;
        m_sender_object_selection_set_second_moment_changed.Signal(
            m_object_selection_set_second_moment);
    }
}

void MapEditor2::WorldView::UpdateObjectSelectionSetAngularVelocity (
    Float const object_selection_set_angular_velocity)
{
    if (m_object_selection_set_angular_velocity != object_selection_set_angular_velocity)
    {
        m_object_selection_set_angular_velocity = object_selection_set_angular_velocity;
        m_sender_object_selection_set_angular_velocity_changed.Signal(
            m_object_selection_set_angular_velocity);
    }
}

void MapEditor2::WorldView::UpdateObjectSelectionSetElasticity (
    Float const object_selection_set_elasticity)
{
    if (m_object_selection_set_elasticity != object_selection_set_elasticity)
    {
        m_object_selection_set_elasticity = object_selection_set_elasticity;
        m_sender_object_selection_set_elasticity_changed.Signal(
            m_object_selection_set_elasticity);
    }
}

void MapEditor2::WorldView::UpdateObjectSelectionSetDensity (
    Float const object_selection_set_density)
{
    if (m_object_selection_set_density != object_selection_set_density)
    {
        m_object_selection_set_density = object_selection_set_density;
        m_sender_object_selection_set_density_changed.Signal(
            m_object_selection_set_density);
    }
}

void MapEditor2::WorldView::SetSelectedCompoundCount (
    Uint32 const selected_compound_count)
{
    if (m_selected_compound_count != selected_compound_count)
    {
        m_selected_compound_count = selected_compound_count;
    }        
}

void MapEditor2::WorldView::UpdateMainObjectLayerConnections ()
{
    // disconnect the signals that are connected to the old main object layer
    m_internal_receiver_change_object_selection_set.DetachAll();

    m_internal_receiver_set_selected_object_count.DetachAll();
    m_internal_receiver_update_object_selection_set_origin.DetachAll();

    m_internal_receiver_set_selected_entity_count.DetachAll();

    // connect the new main object layer's signals
    SignalHandler::Connect0(
        GetMainMapEditorObjectLayer()->SenderObjectSelectionSetChanged(),
        &m_internal_receiver_change_object_selection_set);

    SignalHandler::Connect1(
        GetMainMapEditorObjectLayer()->SenderSelectedObjectCountChanged(),
        &m_internal_receiver_set_selected_object_count);
    SignalHandler::Connect1(
        GetMainMapEditorObjectLayer()->SenderObjectSelectionSetOriginChanged(),
        &m_internal_receiver_update_object_selection_set_origin);

    SignalHandler::Connect1(
        GetMainMapEditorObjectLayer()->SenderSelectedEntityCountChanged(),
        &m_internal_receiver_set_selected_entity_count);
    SignalHandler::Connect1(
        GetMainMapEditorObjectLayer()->SenderObjectSelectionSetFirstMomentChanged(),
        &m_internal_receiver_update_object_selection_set_first_moment);
    SignalHandler::Connect1(
        GetMainMapEditorObjectLayer()->SenderObjectSelectionSetVelocityChanged(),
        &m_internal_receiver_update_object_selection_set_velocity);
    SignalHandler::Connect1(
        GetMainMapEditorObjectLayer()->SenderObjectSelectionSetSecondMomentChanged(),
        &m_internal_receiver_update_object_selection_set_second_moment);
    SignalHandler::Connect1(
        GetMainMapEditorObjectLayer()->SenderObjectSelectionSetAngularVelocityChanged(),
        &m_internal_receiver_update_object_selection_set_angular_velocity);
    SignalHandler::Connect1(
        GetMainMapEditorObjectLayer()->SenderObjectSelectionSetElasticityChanged(),
        &m_internal_receiver_update_object_selection_set_elasticity);
    SignalHandler::Connect1(
        GetMainMapEditorObjectLayer()->SenderObjectSelectionSetDensityChanged(),
        &m_internal_receiver_update_object_selection_set_density);

    SignalHandler::Connect1(
        GetMainMapEditorObjectLayer()->SenderSelectedCompoundCountChanged(),
        &m_internal_receiver_set_selected_compound_count);
        
    // initialize the values it deals with
    SetSelectedObjectCount(
        GetMainMapEditorObjectLayer()->GetSelectedObjectCount());
    UpdateObjectSelectionSetOrigin(
        GetMainMapEditorObjectLayer()->GetObjectSelectionSetOrigin());

    SetSelectedEntityCount(
        GetMainMapEditorObjectLayer()->GetSelectedEntityCount());
    UpdateObjectSelectionSetFirstMoment(
        GetMainMapEditorObjectLayer()->GetObjectSelectionSetFirstMoment());
    UpdateObjectSelectionSetVelocity(
        GetMainMapEditorObjectLayer()->GetObjectSelectionSetVelocity());
    UpdateObjectSelectionSetSecondMoment(
        GetMainMapEditorObjectLayer()->GetObjectSelectionSetSecondMoment());
    UpdateObjectSelectionSetAngularVelocity(
        GetMainMapEditorObjectLayer()->GetObjectSelectionSetAngularVelocity());
    UpdateObjectSelectionSetElasticity(
        GetMainMapEditorObjectLayer()->GetObjectSelectionSetElasticity());
    UpdateObjectSelectionSetDensity(
        GetMainMapEditorObjectLayer()->GetObjectSelectionSetDensity());

    SetSelectedCompoundCount(
        GetMainMapEditorObjectLayer()->GetSelectedCompoundCount());
}

void MapEditor2::WorldView::ChangeObjectSelectionSet ()
{
    ChangeSelectionObjects();
    ChangeSelectionEntities();
}

void MapEditor2::WorldView::ChangeSelectionObjects ()
{
    if (m_selected_object_count == 1)
    {
        Object *object =
            GetMainMapEditorObjectLayer()->GetSingleSelectedObject();
        // the scale and angle will be the single selected object's
        UpdateObjectSelectionSetScale(object->GetScaleFactors().m[0]); // TODO: x/y scale
        UpdateObjectSelectionSetAngle(object->GetAngle());
    }
    else
    {
        // reset the object selection set scale and angle
        ResetObjectSelectionSetScale();
        ResetObjectSelectionSetAngle();
    }
}

void MapEditor2::WorldView::ChangeSelectionEntities ()
{
    if (m_selected_entity_count == 1)
    {
        Entity *entity =
            GetMainMapEditorObjectLayer()->GetSingleSelectedEntity();
        m_sender_per_entity_applies_gravity_assigned.Signal(
            entity->GetAppliesGravity());
        m_sender_per_entity_reacts_to_gravity_assigned.Signal(
            entity->GetReactsToGravity());
    }
    else
    {

    }
}

void MapEditor2::WorldView::DrawOriginCursor (
    RenderContext const &render_context,
    FloatVector2 const &origin,
    MapEditor2::Object::TransformationMode const transformation_mode)
{
    FloatVector2 screen_origin_cursor_position(
        GetParallaxedWorldToScreen() * origin);

    Float radius;
    Color color;
    Float inactive_cursor_size_factor = 0.5;

    // determine the color and radius, depending on if this cursor
    // matches the current transformation mode.
    if (m_transformation_mode == transformation_mode)
    {
        radius = m_origin_cursor_radius;
        color = m_active_origin_cursor_color;
    }
    else
    {
        radius = inactive_cursor_size_factor * m_origin_cursor_radius;
        color = m_inactive_origin_cursor_color;
    }

    // special case for TM_EACH_SELECTED_OBJECT_ORIGIN for the
    // object selection set local cursor
    if (m_transformation_mode == Object::TM_EACH_SELECTED_OBJECT_ORIGIN &&
        transformation_mode == Object::TM_SELECTION_SET_ORIGIN)
    {
        radius = m_origin_cursor_radius;
        color = m_active_origin_cursor_color;
    }

    radius *= GetParentWorldViewWidget()->GetTopLevelParent()->GetSizeRatioBasis();

    FloatMatrix2 draw_circle_transform = FloatMatrix2::ms_identity;
    draw_circle_transform.Scale(2.0f);

    // draw 2 concentric circles around it
    Render::DrawCircle(
        render_context,
        draw_circle_transform,
        screen_origin_cursor_position,
        radius,
        color);
    Render::DrawCircle(
        render_context,
        draw_circle_transform,
        screen_origin_cursor_position,
        0.9f*radius,
        color);
    // draw the X going through the center
    Render::DrawLine(
        render_context,
        screen_origin_cursor_position - FloatVector2(radius, radius),
        screen_origin_cursor_position + FloatVector2(radius, radius),
        color);
    Render::DrawLine(
        render_context,
        screen_origin_cursor_position - FloatVector2(-radius, radius),
        screen_origin_cursor_position + FloatVector2(-radius, radius),
        color);
}

void MapEditor2::WorldView::TransformationEdit (
    FloatVector2 const &last_mouse_position,
    FloatVector2 const &current_mouse_position,
    FloatVector2 const &position_delta,
    EventMouseMotion const *const e)
{
    FloatVector2 old_vector = last_mouse_position - GetOriginCursor();
    FloatVector2 new_vector = current_mouse_position - GetOriginCursor();
    Float old_vector_length = old_vector.GetLength();
    Float new_vector_length = new_vector.GetLength();
    Float vector_length_product = old_vector_length * new_vector_length;
    Float scale_factor =
        old_vector_length != 0.0f ?
        new_vector_length / old_vector_length :
        0.0f;
    Float angle_delta =
        vector_length_product != 0.0f ?
        Math::Asin((old_vector & new_vector) / vector_length_product) :
        0.0f;

    // check for each combination of shift and control keys
    if (e->GetIsEitherShiftKeyPressed())
    {
        if (e->GetIsEitherControlKeyPressed())
        {
            // scale and rotate the object selection set
            GetMainMapEditorObjectLayer()->ObjectSelectionSetScale(
                scale_factor,
                GetOriginCursor(),
                GetTransformationMode());
            GetMainMapEditorObjectLayer()->ObjectSelectionSetRotate(
                angle_delta,
                GetOriginCursor(),
                GetTransformationMode());
            // update the edit deltas
            AccumulateScaleAndAngleEditDeltas(scale_factor, angle_delta);
            // update the object selection set scale and angle
            AccumulateObjectSelectionSetScale(scale_factor);
            AccumulateObjectSelectionSetAngle(angle_delta);
        }
        else
        {
            // scale the object selection set
            GetMainMapEditorObjectLayer()->ObjectSelectionSetScale(
                scale_factor,
                GetOriginCursor(),
                GetTransformationMode());
            // update the edit deltas
            AccumulateScaleEditDelta(scale_factor);
            // update the object selection set scale
            AccumulateObjectSelectionSetScale(scale_factor);
        }
    }
    else
    {
        if (e->GetIsEitherControlKeyPressed())
        {
            // rotate the object selection set
            GetMainMapEditorObjectLayer()->ObjectSelectionSetRotate(
                angle_delta,
                GetOriginCursor(),
                GetTransformationMode());
            // update the edit deltas
            AccumulateAngleEditDelta(angle_delta);
            // update the object selection set angle
            AccumulateObjectSelectionSetAngle(angle_delta);
        }
        else
        {
            // translate the object selection set
            GetMainMapEditorObjectLayer()->ObjectSelectionSetTranslate(position_delta);
            // update the edit deltas
            AccumulateTranslationEditDelta(position_delta);
        }
    }
}

void MapEditor2::WorldView::LinearVelocityEdit (
    FloatVector2 const &last_mouse_position,
    FloatVector2 const &current_mouse_position,
    FloatVector2 const &position_delta,
    EventMouseMotion const *const e)
{
    FloatVector2 old_vector = last_mouse_position - GetOriginCursor();
    FloatVector2 new_vector = current_mouse_position - GetOriginCursor();
    Float old_vector_length = old_vector.GetLength();
    Float new_vector_length = new_vector.GetLength();
    Float vector_length_product = old_vector_length * new_vector_length;
    Float scale_factor =
        old_vector_length != 0.0f ?
        new_vector_length / old_vector_length :
        0.0f;
    Float angle_delta =
        vector_length_product != 0.0f ?
        Math::Asin((old_vector & new_vector) / vector_length_product) :
        0.0f;

    // check for each combination of shift and control keys
    if (e->GetIsEitherShiftKeyPressed())
    {
        if (e->GetIsEitherControlKeyPressed())
        {
            // scale and rotate the object selection set
            GetMainMapEditorObjectLayer()->ObjectSelectionSetScaleVelocity(
                scale_factor,
                GetOriginCursor(),
                GetTransformationMode());
            GetMainMapEditorObjectLayer()->ObjectSelectionSetRotateVelocity(
                angle_delta,
                GetOriginCursor(),
                GetTransformationMode());
            // update the edit deltas
            AccumulateScaleAndAngleEditDeltas(scale_factor, angle_delta);
        }
        else
        {
            // scale the object selection set
            GetMainMapEditorObjectLayer()->ObjectSelectionSetScaleVelocity(
                scale_factor,
                GetOriginCursor(),
                GetTransformationMode());
            // update the edit deltas
            AccumulateScaleEditDelta(scale_factor);
        }
    }
    else
    {
        if (e->GetIsEitherControlKeyPressed())
        {
            // rotate the object selection set
            GetMainMapEditorObjectLayer()->ObjectSelectionSetRotateVelocity(
                angle_delta,
                GetOriginCursor(),
                GetTransformationMode());
            // update the edit deltas
            AccumulateAngleEditDelta(angle_delta);
        }
        else
        {
            // translate the object selection set
            GetMainMapEditorObjectLayer()->ObjectSelectionSetTranslateVelocity(
                position_delta);
            // update the edit deltas
            AccumulateTranslationEditDelta(position_delta);
        }
    }
}

void MapEditor2::WorldView::AngularVelocityEdit (
    FloatVector2 const &last_mouse_position,
    FloatVector2 const &current_mouse_position,
    EventMouseMotion const *const e)
{
    FloatVector2 old_vector = last_mouse_position - GetOriginCursor();
    FloatVector2 new_vector = current_mouse_position - GetOriginCursor();
    Float old_vector_length = old_vector.GetLength();
    Float new_vector_length = new_vector.GetLength();
    Float vector_length_product = old_vector_length * new_vector_length;
    Float scale_factor =
        old_vector_length != 0.0f ?
        new_vector_length / old_vector_length :
        0.0f;
    Float angle_delta =
        vector_length_product != 0.0f ?
        Math::Asin((old_vector & new_vector) / vector_length_product) :
        0.0f;

    // check for each combination of shift and control keys
    if (e->GetIsEitherShiftKeyPressed())
    {
        if (e->GetIsEitherControlKeyPressed())
        {
            // do nothing (for now)
        }
        else
        {
            // scale the object selection set
            GetMainMapEditorObjectLayer()->ObjectSelectionSetScaleAngularVelocity(
                scale_factor);
            // update the edit deltas
            AccumulateScaleEditDelta(scale_factor);
        }
    }
    else
    {
        if (e->GetIsEitherControlKeyPressed())
        {
            // do nothing (for now)
        }
        else
        {
            // angle-accumulate the object selection set
            GetMainMapEditorObjectLayer()->ObjectSelectionSetAccumulateAngularVelocity(
                angle_delta);
            // update the edit deltas
            AccumulateAngleEditDelta(angle_delta);
        }
    }
}

void MapEditor2::WorldView::VerticesEdit (
    FloatVector2 const &last_mouse_position,
    FloatVector2 const &current_mouse_position,
    FloatVector2 const &position_delta,
    EventMouseMotion const *const e)
{
    FloatVector2 old_vector = last_mouse_position - GetOriginCursor();
    FloatVector2 new_vector = current_mouse_position - GetOriginCursor();
    Float old_vector_length = old_vector.GetLength();
    Float new_vector_length = new_vector.GetLength();
    Float vector_length_product = old_vector_length * new_vector_length;
    Float scale_factor =
        old_vector_length != 0.0f ?
        new_vector_length / old_vector_length :
        0.0f;
    Float angle_delta =
        vector_length_product != 0.0f ?
        Math::Asin((old_vector & new_vector) / vector_length_product) :
        0.0f;

    // check for each combination of shift and control keys
    if (e->GetIsEitherShiftKeyPressed())
    {
        if (e->GetIsEitherControlKeyPressed())
        {
            // scale and rotate the vertex selection set
            GetMainMapEditorObjectLayer()->VertexSelectionSetScale(
                scale_factor,
                GetOriginCursor(),
                GetTransformationMode());
            GetMainMapEditorObjectLayer()->VertexSelectionSetRotate(
                angle_delta,
                GetOriginCursor(),
                GetTransformationMode());
            // update the edit deltas
            AccumulateScaleAndAngleEditDeltas(scale_factor, angle_delta);
        /*
            // update the object selection set scale and angle
            AccumulateObjectSelectionSetScale(scale_factor);
            AccumulateObjectSelectionSetAngle(angle_delta);
        */
        }
        else
        {
            // scale the vertex selection set
            GetMainMapEditorObjectLayer()->VertexSelectionSetScale(
                scale_factor,
                GetOriginCursor(),
                GetTransformationMode());
            // update the edit deltas
            AccumulateScaleEditDelta(scale_factor);
        /*
            // update the object selection set scale
            AccumulateObjectSelectionSetScale(scale_factor);
        */
        }
    }
    else
    {
        if (e->GetIsEitherControlKeyPressed())
        {
            // rotate the vertex selection set
            GetMainMapEditorObjectLayer()->VertexSelectionSetRotate(
                angle_delta,
                GetOriginCursor(),
                GetTransformationMode());
            // update the edit deltas
            AccumulateAngleEditDelta(angle_delta);
        /*
            // update the object selection set angle
            AccumulateObjectSelectionSetAngle(angle_delta);
        */
        }
        else
        {
            // translate the vertex selection set
            GetMainMapEditorObjectLayer()->VertexSelectionSetTranslate(position_delta);
            // update the edit deltas
            AccumulateTranslationEditDelta(position_delta);
        }
    }
}

void MapEditor2::WorldView::ResetEditDeltas ()
{
    m_is_translation_edit_in_progress = false;
    m_translation_delta = FloatVector2::ms_zero;
    m_is_scale_edit_in_progress = false;
    m_scale_delta = 1.0;
    m_is_angle_edit_in_progress = false;
    m_angle_delta = 0.0;
}

void MapEditor2::WorldView::SignalEditDeltaText ()
{
    if (m_is_translation_edit_in_progress)
    {
        ASSERT1(!m_is_scale_edit_in_progress)
        ASSERT1(!m_is_angle_edit_in_progress)
        m_sender_status_bar_message_text_changed.Signal(
            Util::StringPrintf(
                "dp = (%f, %f)",
                m_translation_delta[Dim::X],
                m_translation_delta[Dim::Y]));
    }
    else
    {
        if (m_is_scale_edit_in_progress)
        {
            if (m_is_angle_edit_in_progress)
                m_sender_status_bar_message_text_changed.Signal(
                    Util::StringPrintf(
                        "ds = %f, da = %f",
                        m_scale_delta,
                        m_angle_delta));
            else
                m_sender_status_bar_message_text_changed.Signal(
                    Util::StringPrintf(
                        "ds = %f",
                        m_scale_delta));
        }
        else
        {
            if (m_is_angle_edit_in_progress)
                m_sender_status_bar_message_text_changed.Signal(
                    Util::StringPrintf(
                        "da = %f",
                        m_angle_delta));
        }
    }
}

void MapEditor2::WorldView::AccumulateTranslationEditDelta (
    FloatVector2 const &translation_delta)
{
    // reset the non-translating edit values
    m_is_scale_edit_in_progress = false;
    m_scale_delta = 1.0;
    m_is_angle_edit_in_progress = false;
    m_angle_delta = 0.0;
    // update the translation delta
    m_is_translation_edit_in_progress = true;
    m_translation_delta += translation_delta;
}

void MapEditor2::WorldView::AccumulateScaleEditDelta (Float const scale_delta)
{
    // reset the non-scaling edit values
    m_is_translation_edit_in_progress = false;
    m_translation_delta = FloatVector2::ms_zero;
    m_is_angle_edit_in_progress = false;
    m_angle_delta = 0.0;
    // update the scale delta
    m_is_scale_edit_in_progress = true;
    m_scale_delta *= scale_delta;
}

void MapEditor2::WorldView::AccumulateAngleEditDelta (Float const angle_delta)
{
    // reset the non-angle edit values
    m_is_translation_edit_in_progress = false;
    m_translation_delta = FloatVector2::ms_zero;
    m_is_scale_edit_in_progress = false;
    m_scale_delta = 1.0;
    // update the angle delta
    m_is_angle_edit_in_progress = true;
    m_angle_delta += angle_delta;
    // constrain the angle if appropriate
    if (m_metric_editing_mode != Object::MM_ANGULAR_VELOCITY)
        m_angle_delta = Math::GetCanonicalAngle(m_angle_delta);
}

void MapEditor2::WorldView::AccumulateScaleAndAngleEditDeltas (
    Float const scale_delta,
    Float const angle_delta)
{
    // reset the translation edit values
    m_is_translation_edit_in_progress = false;
    m_translation_delta = FloatVector2::ms_zero;
    // update the scale and angle deltas
    m_is_scale_edit_in_progress = true;
    m_scale_delta *= scale_delta;
    m_is_angle_edit_in_progress = true;
    m_angle_delta += angle_delta;
    // constrain the angle if appropriate
    if (m_metric_editing_mode != Object::MM_ANGULAR_VELOCITY)
        m_angle_delta = Math::GetCanonicalAngle(m_angle_delta);
}

void MapEditor2::WorldView::ResetObjectSelectionSetScale ()
{
    m_object_selection_set_scale = 1.0;
    m_sender_object_selection_set_scale_changed.Signal(m_object_selection_set_scale);
}

void MapEditor2::WorldView::ResetObjectSelectionSetAngle ()
{
    m_object_selection_set_angle = 0.0;
    m_sender_object_selection_set_angle_changed.Signal(m_object_selection_set_angle);
}

void MapEditor2::WorldView::AccumulateObjectSelectionSetScale (Float const scale_delta)
{
    if (scale_delta != 1.0)
    {
        m_object_selection_set_scale *= scale_delta;
        m_sender_object_selection_set_scale_changed.Signal(m_object_selection_set_scale);
    }
}

void MapEditor2::WorldView::AccumulateObjectSelectionSetAngle (Float const angle_delta)
{
    if (angle_delta != 0.0)
    {
        m_object_selection_set_angle += angle_delta;
        m_sender_object_selection_set_angle_changed.Signal(m_object_selection_set_angle);
    }
}

void MapEditor2::WorldView::ConnectSignals ()
{
    SignalHandler::Connect0(
        GetMapEditorWorld()->SenderMainObjectLayerChanged(),
        &m_internal_receiver_update_main_object_layer_connections);
    UpdateMainObjectLayerConnections();
}

} // end of namespace Xrb