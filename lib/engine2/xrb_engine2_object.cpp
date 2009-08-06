// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_object.cpp by Victor Dods, created 2004/07/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_object.hpp"

#include "xrb_engine2_compound.hpp"
#include "xrb_engine2_entity.hpp"
#include "xrb_engine2_objectlayer.hpp"
#include "xrb_engine2_sprite.hpp"
#include "xrb_rendercontext.hpp"
#include "xrb_serializer.hpp"

namespace Xrb
{

// ///////////////////////////////////////////////////////////////////////////
// Engine2::Object::DrawLoopFunctor
// ///////////////////////////////////////////////////////////////////////////

// constants which control the thresholds at which objects use
// alpha fading to fade away, when they become small enough.
Float const Engine2::Object::DrawLoopFunctor::ms_radius_limit_upper = 1.0f;
Float const Engine2::Object::DrawLoopFunctor::ms_radius_limit_lower = 0.5f;
Float const Engine2::Object::DrawLoopFunctor::ms_distance_fade_slope =
    1.0f /
    (Engine2::Object::DrawLoopFunctor::ms_radius_limit_upper - Engine2::Object::DrawLoopFunctor::ms_radius_limit_lower);
Float const Engine2::Object::DrawLoopFunctor::ms_distance_fade_intercept =
    Engine2::Object::DrawLoopFunctor::ms_radius_limit_lower /
    (Engine2::Object::DrawLoopFunctor::ms_radius_limit_lower - Engine2::Object::DrawLoopFunctor::ms_radius_limit_upper);

Engine2::Object::DrawLoopFunctor::DrawLoopFunctor (
    RenderContext const &render_context,
    FloatMatrix2 const &world_to_screen,
    Float const pixels_in_view_radius,
    FloatVector2 const &view_center,
    Float const view_radius,
    bool const is_collect_transparent_object_pass,
    TransparentObjectVector *const transparent_object_vector,
    QuadTreeType const quad_tree_type)
    :
    m_object_draw_data(render_context, world_to_screen),
    m_transparent_object_vector(transparent_object_vector)
{
    m_pixels_in_view_radius = pixels_in_view_radius;
    m_view_center = view_center;
    m_view_radius = view_radius;
    m_is_collect_transparent_object_pass = is_collect_transparent_object_pass;
    m_quad_tree_type = quad_tree_type;
    m_drawn_opaque_object_count = 0;
    m_drawn_transparent_object_count = 0;
}

void Engine2::Object::DrawLoopFunctor::operator () (Engine2::Object const *object)
{
    ASSERT3(m_transparent_object_vector != NULL);

    // calculate the object's pixel radius on screen
    Float object_radius = m_pixels_in_view_radius * object->GetRadius(m_quad_tree_type) / m_view_radius;
    // distance culling - don't draw objects that are below the
    // gs_radius_limit_lower threshold
    if (object_radius >= ms_radius_limit_lower)
    {
        Float distance_fade;
        if (m_is_collect_transparent_object_pass)
        {
            ASSERT3(m_object_draw_data.GetRenderContext().ColorMask()[Dim::A] <= 1.0f);
            ASSERT3(object->ColorMask()[Dim::A] <= 1.0f);
            // if it's a transparent object and the transparent object vector
            // exists, add it to the transparent object vector.
            if (object->IsTransparent() ||
                object->ColorMask()[Dim::A] < 1.0f ||
                (distance_fade = CalculateDistanceFade(object_radius)) < 1.0f ||
                m_object_draw_data.GetRenderContext().ColorMask()[Dim::A] < 1.0f)
            {
                m_transparent_object_vector->push_back(object);
                // no need to do anything else, so return
                return;
            }
        }
        else
            distance_fade = CalculateDistanceFade(object_radius);

        // if we got this far, draw it now
        object->Draw(m_object_draw_data, distance_fade);
        // increment the appropriate drawn object count
        if (m_is_collect_transparent_object_pass)
            ++m_drawn_opaque_object_count;
        else
            ++m_drawn_transparent_object_count;
    }
}

Float Engine2::Object::DrawLoopFunctor::CalculateDistanceFade (Float const object_radius)
{
    // calculate the alpha value of the object due to its distance.
    // sprites with radii between ms_radius_limit_lower and
    // gs_radius_limit_upper will be partially transparent, fading away
    // once they get to ms_radius_limit_lower.  this gives a very
    // nice smooth transition for when the objects are not drawn
    // because they are below the lower radius threshold.
    Float distance_fade =
        (object_radius > ms_radius_limit_upper) ?
        1.0f :
        (ms_distance_fade_slope * object_radius + ms_distance_fade_intercept);
    ASSERT1(distance_fade <= 1.0f);
    return distance_fade;
}

// ///////////////////////////////////////////////////////////////////////////
// Engine2::Object
// ///////////////////////////////////////////////////////////////////////////

Engine2::Object::~Object ()
{
    if (m_entity != NULL)
    {
        ASSERT1(m_entity->m_owner_object == this);
        ASSERT1(!m_entity->IsInWorld());
        m_entity->m_owner_object = NULL;
        Delete(m_entity);
    }
}

Engine2::Object *Engine2::Object::Create (
    Serializer &serializer,
    CreateEntityFunction CreateEntity)
{
    ASSERT1(serializer.GetIODirection() == IOD_READ);

    Object *retval = NULL;
    switch (ReadObjectType(serializer))
    {
        case OT_OBJECT:
            retval = new Object(OT_OBJECT);
            retval->Object::ReadClassSpecific(serializer);
            break;

        case OT_SPRITE:
            retval = Sprite::Create(serializer);
            break;

        case OT_COMPOUND:
            retval = Compound::Create(serializer);
            break;

        default:
            ASSERT0(false && "Invalid object type");
            retval = NULL;
            break;
    }

    // if there is an Entity attached to this object, read it
    if (serializer.ReadBool())
    {
        ASSERT1(CreateEntity != NULL);
        retval->m_entity = CreateEntity(serializer);
        ASSERT1(retval->m_entity != NULL);
    }

    return retval;
}

void Engine2::Object::Write (Serializer &serializer) const
{
    WriteObjectType(serializer);
    // call WriteClassSpecific for this and all superclasses
    Object::WriteClassSpecific(serializer);

    // write true if there's an attached Entity, false if not.
    serializer.WriteBool(m_entity != NULL);
    // if there's an Entity attached to this object, write it
    if (m_entity != NULL)
        m_entity->Write(serializer);
}

Engine2::World *Engine2::Object::GetWorld () const
{
    ASSERT1(m_object_layer != NULL);
    ASSERT1(m_object_layer->OwnerWorld() != NULL);
    return m_object_layer->OwnerWorld();
}

void Engine2::Object::SetEntity (Entity *const entity)
{
    if (m_entity != NULL)
    {
        ASSERT1(m_entity->m_owner_object == this);
        m_entity->m_owner_object = NULL;
    }

    m_entity = entity;

    if (m_entity != NULL)
    {
        m_entity->m_owner_object = this;
        m_entity->HandleNewOwnerObject();
    }
}

Engine2::Object::Object (ObjectType object_type)
    :
    FloatTransform2(FloatTransform2::ms_identity, true),
    m_object_type(object_type),
    m_bias_color(Color::ms_identity_bias_color),
    m_color_mask(Color::ms_identity_mask_color)
{
    ASSERT1(m_object_type < OT_COUNT);
    m_z_depth = 0.0f;
    m_object_layer = NULL;
    for (Uint32 i = 0; i < QTT_COUNT; ++i)
    {
        m_radius[i] = 0.0f;
        m_owner_quad_tree[i] = NULL;
    }
    m_entity = NULL;
    m_is_transparent = false;
    m_radii_need_to_be_recalculated = true;
}

Engine2::ObjectType Engine2::Object::ReadObjectType (Serializer &serializer)
{
    return static_cast<ObjectType>(serializer.ReadUint8());
}

void Engine2::Object::WriteObjectType (Serializer &serializer) const
{
    serializer.WriteUint8(static_cast<Uint8>(m_object_type));
}

void Engine2::Object::ReadClassSpecific (Serializer &serializer)
{
    serializer.ReadFloatTransform2(this);
    serializer.ReadColor(&m_bias_color);
    serializer.ReadColor(&m_color_mask);
}

void Engine2::Object::WriteClassSpecific (Serializer &serializer) const
{
    serializer.WriteFloatTransform2(*static_cast<FloatTransform2 const *>(this));
    serializer.WriteColor(m_bias_color);
    serializer.WriteColor(m_color_mask);
}

void Engine2::Object::CloneProperties (Object const *const object)
{
    ASSERT1(object != NULL);

    SetTranslation(object->GetTranslation());
    SetScaleFactors(object->GetScaleFactors());
    SetAngle(object->Angle());
    m_bias_color = object->m_bias_color;
    m_color_mask = object->m_color_mask;
}

void Engine2::Object::CalculateTransform () const
{
    /*
    // original function code (kept around because this version does the
    // same thing but is much easier to read)
    if (IsDirty() || m_radii_need_to_be_recalculated)
    {
        RecalculateTransformIfNecessary();
        // recalculate all radii
        for (Uint32 i = 0; i < QTT_COUNT; ++i)
            CalculateRadius(static_cast<QuadTreeType>(i));
        m_radii_need_to_be_recalculated = false;
    }
    */

    // optimized function code:
    if (IsDirty())
    {
        if (GetScalingAndRotationIsDirty())
        {
            RecalculateTransform();
            // recalculate all radii
            for (Uint32 i = 0; i < QTT_COUNT; ++i)
                CalculateRadius(static_cast<QuadTreeType>(i));
            m_radii_need_to_be_recalculated = false;
        }
        else if (m_radii_need_to_be_recalculated)
        {
            RecalculateTransformWithoutScalingAndRotation();
            // recalculate all radii
            for (Uint32 i = 0; i < QTT_COUNT; ++i)
                CalculateRadius(static_cast<QuadTreeType>(i));
            m_radii_need_to_be_recalculated = false;
        }
        else
            RecalculateTransformWithoutScalingAndRotation();
    }
    else if (m_radii_need_to_be_recalculated)
    {
        // recalculate all radii
        for (Uint32 i = 0; i < QTT_COUNT; ++i)
            CalculateRadius(static_cast<QuadTreeType>(i));
        m_radii_need_to_be_recalculated = false;
    }
}

} // end of namespace Xrb
