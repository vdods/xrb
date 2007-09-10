// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_object.cpp by Victor Dods, created 2005/07/03
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_mapeditor2_object.h"

#include "xrb_engine2_entityguts.h"
#include "xrb_mapeditor2_compound.h"
#include "xrb_mapeditor2_objectlayer.h"
#include "xrb_mapeditor2_sprite.h"
#include "xrb_mapeditor2_spriteentity.h"
#include "xrb_mapeditor2_visibilityquadtree.h"
#include "xrb_render.h"

namespace Xrb
{

Color MapEditor2::Object::ms_unselected_metrics_color(0.75f, 0.7f, 0.0f, 1.0f);
Color MapEditor2::Object::ms_selected_metrics_color(1.0f, 1.0f, 0.5f, 1.0f);

MapEditor2::Object::~Object ()
{
    DeleteAndNullify(m_saved_entity_guts);
}

MapEditor2::Object *MapEditor2::Object::Create (Serializer &serializer)
{
    Object *retval;

    SubType sub_type = ReadSubType(serializer);
    switch (sub_type)
    {
        case ST_OBJECT:
            ASSERT1(false && "Invalid object type -- Object is abstract")
            retval = NULL;
            break;

        case ST_SPRITE:
            retval = Sprite::Create(serializer);
            break;

        case ST_COMPOUND:
            retval = Compound::Create(serializer);
            break;
            
        case ST_ENTITY:
            retval = Entity::Create(serializer);
            break;

        case ST_SPRITE_ENTITY:
            retval = SpriteEntity::Create(serializer);
            break;

        default:
            ASSERT1(false && "Invalid object type")
            retval = NULL;
            break;
    }

    return retval;
}

MapEditor2::ObjectLayer *MapEditor2::Object::GetMapEditorObjectLayer () const
{
    if (GetObjectLayer() != NULL)
        ASSERT1(dynamic_cast<ObjectLayer *>(GetObjectLayer()) != NULL)
    return static_cast<ObjectLayer *>(GetObjectLayer());
}

MapEditor2::VisibilityQuadTree *MapEditor2::Object::GetOwnerMapEditorQuadTree () const
{
    if (GetOwnerQuadTree(Engine2::QTT_VISIBILITY) != NULL)
        ASSERT1(dynamic_cast<VisibilityQuadTree *>(GetOwnerQuadTree(Engine2::QTT_VISIBILITY)) != NULL)
    return static_cast<VisibilityQuadTree *>(GetOwnerQuadTree(Engine2::QTT_VISIBILITY));
}

Color const &MapEditor2::Object::GetUnselectedMetricsColor () const
{
    return Object::ms_unselected_metrics_color;
}

Color const &MapEditor2::Object::GetSelectedMetricsColor () const
{
    return Object::ms_selected_metrics_color;
}

void MapEditor2::Object::DrawMetrics (
    DrawData const &draw_data,
    Float const alpha_mask,
    MetricMode const metric_mode)
{
    Color color(GetIsSelected() ?
                GetSelectedMetricsColor() :
                GetUnselectedMetricsColor());
    color[Dim::A] *= alpha_mask;

    // draw the bounding circle
    if (metric_mode == MM_TRANSFORMATION ||
        metric_mode == MM_LINEAR_VELOCITY ||
        metric_mode == MM_ANGULAR_VELOCITY)
    {
        Render::DrawCircle(
            draw_data.GetRenderContext(),
            draw_data.GetTransformation(),
            GetTranslation(),
            GetVisibleRadius(),
            color);
    }

    switch (metric_mode)
    {
        case MM_TRANSFORMATION:
            // draw the two angle limit lines
            Render::DrawLine(
                draw_data.GetRenderContext(),
                GetTranslation(),
                GetTranslation() +
                1.25f * GetVisibleRadius() * FloatVector2(1.0f, 0.0f),
                color);
            Render::DrawLine(
                draw_data.GetRenderContext(),
                GetTranslation(),
                GetTranslation() +
                1.25f * GetVisibleRadius() *
                FloatVector2(
                    Math::Cos(GetAngle()),
                    Math::Sin(GetAngle())),
                color);
            // draw the arc connecting the two
            Render::DrawCircularArc(
                draw_data.GetRenderContext(),
                draw_data.GetTransformation(),
                GetTranslation(),
                1.25f*GetVisibleRadius(),
                0.0f,
                Math::GetCanonicalAngle(GetAngle()),
                color);
            // draw the little vertical bar across the origin
            Render::DrawLine(
                draw_data.GetRenderContext(),
                GetTranslation() + FloatVector2(0.0f, 0.125f*GetVisibleRadius()),
                GetTranslation() - FloatVector2(0.0f, 0.125f*GetVisibleRadius()),
                color);
            break;

        case MM_LINEAR_VELOCITY:
        case MM_ANGULAR_VELOCITY:
            // Object don't have these -- Entity does though
            break;

        case MM_POLYGONS:
        case MM_VERTICES:
            // Compound handles this.
            break;
            
        default:
            ASSERT0(false && "Invalid metric mode");
            break;
    }
}

void MapEditor2::Object::ToggleIsSelected ()
{
    if (m_is_selected)
        RemoveFromObjectSelectionSet();
    else
        AddToObjectSelectionSet();
}

void MapEditor2::Object::ApplyObjectSelectionOperation (
    SelectionOperation const selection_operation,
    bool const object_is_in_operand_set)
{
    if (object_is_in_operand_set)
    {
        switch (selection_operation)
        {
            case SO_EQUALS: SetIsSelected(true);  break;
            case SO_IOR:    SetIsSelected(true);  break;
            case SO_MINUS:  SetIsSelected(false); break;
            case SO_XOR:    ToggleIsSelected();   break;
            case SO_AND:                          break;
            default:
                ASSERT0(false && "Invalid selection operation");
                break;
        }
    }
    else
    {
        switch (selection_operation)
        {
            case SO_EQUALS: SetIsSelected(false); break;
            case SO_IOR:                          break;
            case SO_MINUS:                        break;
            case SO_XOR:                          break;
            case SO_AND:    SetIsSelected(false); break;
            default:
                ASSERT0(false && "Invalid selection operation");
                break;
        }
    }
}

void MapEditor2::Object::ObjectSelectionSetScale (
    Float const scale_factor,
    FloatVector2 transformation_origin,
    TransformationMode const transformation_mode)
{
    if (transformation_mode == TM_EACH_SELECTED_OBJECT_ORIGIN)
        transformation_origin = GetTranslation();

    // translate this object to the origin
    Translate(-transformation_origin);
    // scale this object on its own origin
    Scale(scale_factor);
    // scale the object's origin relative to the absolute origin
    SetTranslation(scale_factor * GetTranslation());
    // translate this object back to the original position
    Translate(transformation_origin);
}

void MapEditor2::Object::ObjectSelectionSetRotate (
    Float const angle_delta,
    FloatVector2 transformation_origin,
    TransformationMode const transformation_mode,
    FloatMatrix2 const &rotation_transformation)
{
    if (transformation_mode == TM_EACH_SELECTED_OBJECT_ORIGIN)
        transformation_origin = GetTranslation();

    // translate this object to the origin
    Translate(-transformation_origin);
    // rotate this object on its own origin
    Rotate(angle_delta);
    // rotate this object around the absolute origin
    SetTranslation(rotation_transformation * GetTranslation());
    // translate this object back to the original position
    Translate(transformation_origin);
}

void MapEditor2::Object::ReAddMapEditorObjectToQuadTree (
    Engine2::QuadTreeType const quad_tree_type)
{
    ASSERT1(GetOwnerQuadTree(quad_tree_type) != NULL)
    GetObjectLayer()->HandleContainmentOrWrapping(this);
    GetOwnerQuadTree(quad_tree_type)->ReAddObject(this);
}

void MapEditor2::Object::SetIsSelected (bool const is_selected)
{
    if (is_selected != m_is_selected)
    {
        if (is_selected)
            AddToObjectSelectionSet();
        else
            RemoveFromObjectSelectionSet();
    }
}

MapEditor2::Object::Object ()
    :
    Engine2::Object()
{
    m_is_selected = false;
    m_saved_entity_guts = NULL;
}

void MapEditor2::Object::AddToObjectSelectionSet ()
{
    ASSERT1(GetOwnerQuadTree(Engine2::QTT_VISIBILITY) != NULL)
    ASSERT1(GetObjectLayer() != NULL)
    ASSERT1(!m_is_selected)
    ObjectLayer *map_editor_object_layer = GetMapEditorObjectLayer();
    ASSERT1(map_editor_object_layer != NULL)
    DEBUG1_CODE(bool add_success =)
    map_editor_object_layer->AddObjectToObjectSelectionSet(this);
    ASSERT1(add_success)
    m_is_selected = true;
}

void MapEditor2::Object::RemoveFromObjectSelectionSet ()
{
    ASSERT1(GetOwnerQuadTree(Engine2::QTT_VISIBILITY) != NULL)
    ASSERT1(GetObjectLayer() != NULL)
    ASSERT1(m_is_selected)
    ObjectLayer *map_editor_object_layer = GetMapEditorObjectLayer();
    ASSERT1(map_editor_object_layer != NULL)
    DEBUG1_CODE(bool remove_success =)
    map_editor_object_layer->RemoveObjectFromObjectSelectionSet(this);
    ASSERT1(remove_success)
    m_is_selected = false;
}

} // end of namespace Xrb
