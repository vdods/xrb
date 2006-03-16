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
#include "xrb_engine2_spriteentity.h"
#include "xrb_math.h"
#include "xrb_render.h"
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

Engine2::Object *Engine2::Object::Create (Serializer &serializer)
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

void Engine2::Object::Write (Serializer &serializer) const
{
    WriteSubType(serializer);
    // call WriteClassSpecific for this and all superclasses
    Object::WriteClassSpecific(serializer);
}

Engine2::World *Engine2::Object::GetWorld () const
{
    ASSERT1(m_object_layer != NULL)
    ASSERT1(m_object_layer->GetOwnerWorld() != NULL)
    return m_object_layer->GetOwnerWorld();
}

Engine2::Object::Object ()
    :
    m_transform(FloatTransform2::ms_identity, true)
{
    m_sub_type = ST_OBJECT;
    m_radius = 0.0;
    for (Uint32 i = 0; i < QTT_NUM_TYPES; ++i)
        m_owner_quad_tree[i] = NULL;
    m_object_layer = NULL;
    m_radius_needs_to_be_recalculated = true;
}

Engine2::Object::SubType Engine2::Object::ReadSubType (Serializer &serializer)
{
    ASSERT1(serializer.GetIODirection() == IOD_READ)
    SubType retval = static_cast<SubType>(serializer.ReadUint8());
    ASSERT1(retval >= ST_LOWEST_SUB_TYPE && retval <= ST_HIGHEST_SUB_TYPE)
    return retval;
}

void Engine2::Object::WriteSubType (Serializer &serializer) const
{
    ASSERT1(serializer.GetIODirection() == IOD_WRITE)
    ASSERT1(m_sub_type >= ST_LOWEST_SUB_TYPE && m_sub_type <= ST_HIGHEST_SUB_TYPE)
    serializer.WriteUint8(static_cast<Uint8>(m_sub_type));
}

void Engine2::Object::ReadClassSpecific (Serializer &serializer)
{
    ASSERT1(serializer.GetIODirection() == IOD_READ)
    
    // read in the guts
    m_transform = serializer.ReadFloatTransform2();
}

void Engine2::Object::WriteClassSpecific (Serializer &serializer) const
{
    ASSERT1(serializer.GetIODirection() == IOD_WRITE)
    
    // write out the guts
    serializer.WriteFloatTransform2(m_transform);
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
    /* original function code:
    if (m_transform.GetIsDirty() || m_radius_needs_to_be_recalculated)
    {
        m_transform.RecalculateTransformIfNecessary();
        CalculateRadius();
        m_radius_needs_to_be_recalculated = false;
    }
    */
    
    // optimized function code:
    if (m_transform.GetIsDirty())
    {
        if (m_transform.GetScalingAndRotationIsDirty())
        {
            m_transform.RecalculateTransform();
            CalculateRadius();
            m_radius_needs_to_be_recalculated = false;
        }
        else if (m_radius_needs_to_be_recalculated)
        {
            m_transform.RecalculateTransformWithoutScalingAndRotation();
            CalculateRadius();
            m_radius_needs_to_be_recalculated = false;
        }
        else
            m_transform.RecalculateTransformWithoutScalingAndRotation();
    }
    else if (m_radius_needs_to_be_recalculated)
    {
        CalculateRadius();
        m_radius_needs_to_be_recalculated = false;
    }
}

void Engine2::Object::IndicateRadiusNeedsToBeRecalculated ()
{
    m_radius_needs_to_be_recalculated = true;
}

} // end of namespace Xrb
