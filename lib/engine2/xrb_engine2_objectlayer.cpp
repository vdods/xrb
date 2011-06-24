// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_objectlayer.cpp by Victor Dods, created 2004/07/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_objectlayer.hpp"

#include "xrb_engine2_entity.hpp"
#include "xrb_engine2_visibilityquadtree.hpp"
#include "xrb_rendercontext.hpp"
#include "xrb_serializer.hpp"

namespace Xrb {
namespace Engine2 {

ObjectLayer::~ObjectLayer ()
{
    Delete(m_quad_tree);
}

ObjectLayer *ObjectLayer::Create (
    World *const owner_world,
    bool const is_wrapped,
    Float const side_length,
    Uint32 const tree_depth,
    Float const z_depth,
    std::string const &name)
{
    ASSERT1(owner_world != NULL);
    ASSERT1(side_length > 0.0);
    ASSERT1(tree_depth > 0);

    ObjectLayer *retval =
        new ObjectLayer(
            owner_world,
            is_wrapped,
            side_length,
            z_depth,
            name);

    retval->m_quad_tree =
        new VisibilityQuadTree(
            FloatVector2::ms_zero,
            0.5f*side_length,
            tree_depth);

    return retval;
}

ObjectLayer *ObjectLayer::Create (
    Serializer &serializer,
    World *owner_world)
{
    ASSERT1(owner_world != NULL);

    Float side_length;
    Float z_depth;
    serializer.Read<Float>(side_length);
    serializer.Read<Float>(z_depth);
    bool is_wrapped = serializer.Read<bool>();
    ObjectLayer *retval =
        new ObjectLayer(
            owner_world,
            is_wrapped,
            side_length,
            z_depth,
            ""); // empty name

    // this call just constructs the quadtree nodes,
    retval->m_quad_tree = VisibilityQuadTree::Create(serializer);
    // the objects have to be added here
    retval->m_quad_tree->ReadObjects(serializer, retval);

    return retval;
}

Object *ObjectLayer::SmallestObjectTouchingPoint (
    FloatVector2 const &point) const
{
    return m_quad_tree->SmallestObjectTouchingPoint(point);
}

bool ObjectLayer::DoesAreaOverlapAnyObject (
    FloatVector2 const &area_center,
    Float area_radius) const
{
    ASSERT1(m_quad_tree != NULL);
    return m_quad_tree->DoesAreaOverlapAnyObject(
        area_center,
        area_radius,
        IsWrapped(),
        SideLength(),
        0.5f * SideLength());
}

FloatVector2 ObjectLayer::AdjustedDifference (FloatVector2 p, FloatVector2 const &q) const
{
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(p[Dim::X]));
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(p[Dim::Y]));
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(q[Dim::X]));
    ASSERT_NAN_SANITY_CHECK(Math::IsFinite(q[Dim::Y]));

    p -= q;
    if (IsWrapped())
    {
        p[Dim::X] = Math::Mod(p[Dim::X], m_side_length);
        p[Dim::Y] = Math::Mod(p[Dim::Y], m_side_length);
    }
    return p;
}

void ObjectLayer::Write (Serializer &serializer) const
{
    // write the ObjectLayer's overhead information
    serializer.Write<Float>(m_side_length);
    serializer.Write<Float>(m_z_depth);
    serializer.Write<bool>(m_is_wrapped);

    ASSERT1(m_quad_tree != NULL);
    // write the quad tree structure
    m_quad_tree->WriteStructure(serializer);
    // write the objects the quadtree contains
    DEBUG1_CODE(Uint32 written_static_object_count =)
    m_quad_tree->WriteObjects(serializer);
    ASSERT1(written_static_object_count == m_quad_tree->SubordinateStaticObjectCount());
}

Uint32 ObjectLayer::Draw (
    RenderContext const &render_context,
    FloatMatrix2 const &world_to_screen,
    DrawObjectCollector &draw_object_collector)
{
    ASSERT1(draw_object_collector.m_pixels_in_view_radius > 0.0f);
    ASSERT1(draw_object_collector.m_view_radius > 0.0f);

    if (m_is_wrapped)
        return m_quad_tree->DrawWrapped(render_context, world_to_screen, draw_object_collector);
    else
        return m_quad_tree->Draw(render_context, world_to_screen, draw_object_collector);
}

void ObjectLayer::AddObject (Object *const object)
{
    ASSERT1(object != NULL);
    ASSERT1(m_quad_tree != NULL);
    ASSERT1(object->OwnerQuadTree(QTT_VISIBILITY) == NULL);

    // set the object's layer
    object->SetObjectLayer(this);
    // make sure the object behaves within the bounds of the object layer.
    HandleContainmentOrWrapping(object);
    // attempt to add it to the quad tree
    DEBUG1_CODE(bool add_success =)
    m_quad_tree->AddObject(object);
    ASSERT1(add_success);

    ASSERT1(object->OwnerQuadTree(QTT_VISIBILITY) != NULL);
}

void ObjectLayer::RemoveObject (Object *const object)
{
    ASSERT1(object != NULL);
    ASSERT1(object->GetObjectLayer() == this);
    ASSERT1(object->OwnerQuadTree(QTT_VISIBILITY) != NULL);
    object->OwnerQuadTree(QTT_VISIBILITY)->RemoveObject(object);
}

void ObjectLayer::HandleContainmentOrWrapping (Object *object)
{
    ASSERT1(object != NULL);
    ASSERT1(m_quad_tree != NULL);

    // make sure that it's inside the layer's bounds first
    Entity *entity = object->GetEntity();
    if (m_is_wrapped)
    {
        if (entity != NULL)
            WrapEntity(entity);
        else
            WrapTransform2(object);
    }
    else
    {
        if (entity != NULL)
            ContainEntity(entity);
        else
            ContainTransform2(object);
    }
}

void ObjectLayer::ContainVector2 (FloatVector2 *const vector) const
{
    ASSERT1(vector != NULL);

    for (Uint32 i = 0; i <= 1; ++i)
    {
        if (vector->m[i] > m_half_side_length)
            vector->m[i] = m_half_side_length;
        else if (vector->m[i] < -m_half_side_length)
            vector->m[i] = -m_half_side_length;
    }
}

void ObjectLayer::ContainTransform2 (FloatTransform2 *const transform) const
{
    ASSERT1(transform != NULL);
    ASSERT1(!IsWrapped());

    FloatVector2 translation(transform->Translation());
    ContainVector2(&translation);
    transform->SetTranslation(translation);
}

void ObjectLayer::ContainEntity (Entity *const entity) const
{
    ASSERT1(entity != NULL);
    ASSERT1(!IsWrapped());

    FloatVector2 translation(entity->Translation());
    bool component_x = false;
    bool component_y = false;

    if (translation[Dim::X] > m_half_side_length)
    {
        translation[Dim::X] = m_half_side_length;
        component_x = true;
    }
    else if (translation[Dim::X] < -m_half_side_length)
    {
        translation[Dim::X] = -m_half_side_length;
        component_x = true;
    }

    if (translation[Dim::Y] > m_half_side_length)
    {
        translation[Dim::Y] = m_half_side_length;
        component_y = true;
    }
    else if (translation[Dim::Y] < -m_half_side_length)
    {
        translation[Dim::Y] = -m_half_side_length;
        component_y = true;
    }

    ASSERT1(entity->OwnerObject() != NULL);
    entity->OwnerObject()->SetTranslation(translation);
    entity->HandleObjectLayerContainment(component_x, component_y);
}

void ObjectLayer::WrapVector2 (FloatVector2 *const vector) const
{
    ASSERT1(vector != NULL);

    for (Uint32 i = 0; i <= 1; ++i)
    {
        if (vector->m[i] > m_half_side_length)
            vector->m[i] -= m_side_length *
                            (int)((vector->m[i] + m_half_side_length) /
                            m_side_length);

        if (vector->m[i] < -m_half_side_length)
            vector->m[i] -= m_side_length *
                            (int)((vector->m[i] - m_half_side_length) /
                            m_side_length);
    }
}

void ObjectLayer::WrapTransform2 (FloatTransform2 *const transform) const
{
    ASSERT1(transform != NULL);
    ASSERT1(IsWrapped());

    FloatVector2 translation(transform->Translation());
    WrapVector2(&translation);
    transform->SetTranslation(translation);
}

void ObjectLayer::WrapEntity (Entity *const entity) const
{
    ASSERT1(entity != NULL);
    ASSERT1(IsWrapped());

    FloatVector2 previous_translation(entity->Translation());
    WrapTransform2(entity->OwnerObject());
    entity->AccumulateWrappedOffset(previous_translation - entity->Translation());
}

ObjectLayer::ObjectLayer (
    World *owner_world,
    bool is_wrapped,
    Float side_length,
    Float z_depth,
    std::string const &name)
{
    ASSERT1(owner_world != NULL);
    ASSERT1(side_length > 0.0f);

    m_owner_world = owner_world;
    m_is_wrapped = is_wrapped;
    m_side_length = side_length;
    m_half_side_length = 0.5f * side_length;
    m_z_depth = z_depth;
    m_quad_tree = NULL;
    m_name = name;
    m_background_color = Color::ms_transparent_black;
}

} // end of namespace Engine2
} // end of namespace Xrb
