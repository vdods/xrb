// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_object.cpp by Victor Dods, created 2004/07/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_object.h"

#include "xrb_engine2_compound.h"
#include "xrb_engine2_entity.h"
#include "xrb_engine2_objectlayer.h"
#include "xrb_engine2_sprite.h"
#include "xrb_serializer.h"

namespace Xrb
{

Engine2::Object::DrawData::DrawData (
    RenderContext const &render_context,
    FloatMatrix2 const &transformation)
    :
    m_render_context(render_context)
{
    // m_render_context is initialized above
    m_transformation = transformation;
}

Engine2::Object::~Object ()
{
    if (m_entity != NULL)
    {
        ASSERT1(m_entity->m_owner_object == this)
        ASSERT1(!m_entity->GetIsInWorld())
        m_entity->m_owner_object = NULL;
        Delete(m_entity);
    }
}

Engine2::Object *Engine2::Object::Create (
    Serializer &serializer,
    CreateEntityFunction CreateEntity)
{
    ASSERT1(serializer.GetIODirection() == IOD_READ)

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
            ASSERT0(false && "Invalid object type")
            retval = NULL;
            break;
    }

    // if there is an Entity attached to this object, read it
    if (serializer.ReadBool())
    {
        ASSERT1(CreateEntity != NULL)
        retval->m_entity = CreateEntity(serializer);
        ASSERT1(retval->m_entity != NULL)
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
    ASSERT1(m_object_layer != NULL)
    ASSERT1(m_object_layer->GetOwnerWorld() != NULL)
    return m_object_layer->GetOwnerWorld();
}

void Engine2::Object::SetEntity (Entity *const entity)
{
    if (m_entity != NULL)
    {
        ASSERT1(m_entity->m_owner_object == this)
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
    m_object_type(object_type)
{
    ASSERT1(m_object_type < OT_COUNT)
    m_object_layer = NULL;
    for (Uint32 i = 0; i < QTT_COUNT; ++i)
    {
        m_radius[i] = 0.0f;
        m_owner_quad_tree[i] = NULL;
    }
    m_entity = NULL;
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
    FloatTransform2::operator=(serializer.ReadFloatTransform2());
}

void Engine2::Object::WriteClassSpecific (Serializer &serializer) const
{
    serializer.WriteFloatTransform2(*static_cast<FloatTransform2 const *>(this));
}

void Engine2::Object::CloneProperties (Object const *const object)
{
    ASSERT1(object != NULL)

    SetTranslation(object->GetTranslation());
    SetScaleFactors(object->GetScaleFactors());
    SetAngle(object->GetAngle());
}

void Engine2::Object::CalculateTransform () const
{
    /*
    // original function code (kept around because this version does the
    // same thing but is much easier to read)
    if (GetIsDirty() || m_radii_need_to_be_recalculated)
    {
        RecalculateTransformIfNecessary();
        // recalculate all radii
        for (Uint32 i = 0; i < QTT_COUNT; ++i)
            CalculateRadius(static_cast<QuadTreeType>(i));
        m_radii_need_to_be_recalculated = false;
    }
    */
    
    // optimized function code:
    if (GetIsDirty())
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
