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

namespace Xrb {
namespace Engine2 {

Object::~Object ()
{
    if (m_entity != NULL)
    {
        ASSERT1(m_entity->m_owner_object == this);
        ASSERT1(!m_entity->IsInWorld());
        m_entity->m_owner_object = NULL;
        Delete(m_entity);
    }
}

// constants which control the thresholds at which objects use
// alpha fading to fade away, when they become small enough.
Float const Object::ms_radius_limit_upper = 1.0f;
Float const Object::ms_radius_limit_lower = 0.5f;
Float const Object::ms_distance_fade_slope = 1.0f / (Object::ms_radius_limit_upper - Object::ms_radius_limit_lower);
Float const Object::ms_distance_fade_intercept = Object::ms_radius_limit_lower / (Object::ms_radius_limit_lower - Object::ms_radius_limit_upper);

Float Object::CalculateDistanceFade (Float object_radius)
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

Object *Object::Create (
    Serializer &serializer,
    CreateEntityFunction CreateEntity)
{
    ASSERT1(serializer.IsReadable());

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
    if (serializer.Read<bool>())
    {
        ASSERT1(CreateEntity != NULL);
        retval->m_entity = CreateEntity(serializer);
        ASSERT1(retval->m_entity != NULL);
    }

    return retval;
}

void Object::Write (Serializer &serializer) const
{
    WriteObjectType(serializer);
    // call WriteClassSpecific for this and all superclasses
    Object::WriteClassSpecific(serializer);

    // write true if there's an attached Entity, false if not.
    serializer.Write<bool>(m_entity != NULL);
    // if there's an Entity attached to this object, write it
    if (m_entity != NULL)
        m_entity->Write(serializer);
}

Object *Object::Clone () const
{
    Object *retval = new Object(m_object_type);
    retval->CloneProperties(*this);
    return retval;
}

World *Object::GetWorld () const
{
    ASSERT1(m_object_layer != NULL);
    ASSERT1(m_object_layer->OwnerWorld() != NULL);
    return m_object_layer->OwnerWorld();
}

void Object::SetEntity (Entity *const entity)
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

Object::Object (ObjectType object_type)
    :
    FloatTransform2(FloatTransform2::ms_identity, true),
    m_object_type(object_type),
    m_color_bias(Color::ms_identity_color_bias),
    m_color_mask(Color::ms_identity_color_mask)
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

ObjectType Object::ReadObjectType (Serializer &serializer)
{
    return static_cast<ObjectType>(serializer.Read<Uint8>());
}

void Object::WriteObjectType (Serializer &serializer) const
{
    serializer.Write<Uint8>(static_cast<Uint8>(m_object_type));
}

void Object::ReadClassSpecific (Serializer &serializer)
{
    serializer.ReadAggregate<FloatTransform2>(*this);
    serializer.ReadAggregate<Color>(m_color_bias);
    serializer.ReadAggregate<Color>(m_color_mask);
}

void Object::WriteClassSpecific (Serializer &serializer) const
{
    serializer.WriteAggregate<FloatTransform2>(*static_cast<FloatTransform2 const *>(this));
    serializer.WriteAggregate<Color>(m_color_bias);
    serializer.WriteAggregate<Color>(m_color_mask);
}

void Object::CalculateRadius (QuadTreeType quad_tree_type) const
{
    m_radius[quad_tree_type] = ScaleFactor();
}

void Object::CloneProperties (Object const &object)
{
    ASSERT1(GetObjectLayer() == NULL && "can't CloneProperties into an Object that is added to the world already");

    SetTranslation(object.Translation());
    SetScaleFactors(object.ScaleFactors());
    SetAngle(object.Angle());
    m_z_depth = object.m_z_depth;
    m_color_bias = object.m_color_bias;
    m_color_mask = object.m_color_mask;
    m_radii_need_to_be_recalculated = true;
    m_is_transparent = object.m_is_transparent;
}

void Object::CalculateTransform () const
{
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
/*
    // optimized function code: // NOTE: valgrind's callgrind seems to think this is slower than above
    if (IsDirty())
    {
        if (ScalingAndRotationIsDirty())
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
*/
}

} // end of namespace Engine2
} // end of namespace Xrb
