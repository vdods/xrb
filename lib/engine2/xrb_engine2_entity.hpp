// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_entity.hpp by Victor Dods, created 2004/07/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_ENTITY_HPP_)
#define _XRB_ENGINE2_ENTITY_HPP_

#include "xrb.hpp"

#include "xrb_engine2_enums.hpp"
#include "xrb_engine2_object.hpp"
#include "xrb_engine2_types.hpp"

namespace Xrb
{

class Serializer;

namespace Engine2
{

class Compound;
class Object;
class ObjectLayer;
class PhysicsHandler;
class QuadTree;
class Sprite;
class World;

class Entity
{
public:

    Entity ()
        :
        m_wrapped_offset(FloatVector2::ms_zero)
    {
        ResetWorldIndex();
        m_owner_object = NULL;
    }
    virtual ~Entity ()
    {
        ASSERT1(m_owner_object == NULL);
    }

    // ///////////////////////////////////////////////////////////////////
    // public interface methods
    // ///////////////////////////////////////////////////////////////////

    // writes game-specific Entity data
    virtual void Write (Serializer &serializer) const = 0;
    // used e.g. for setting the component velocity to 0 when the owner
    // object hits the side of its non-wrapped ObjectLayer.  if the owner
    // object exceeded the ObjectLayer's boundaries in the X dimension,
    // component_x will be true.  component_y will be set analogously.
    virtual void HandleObjectLayerContainment (bool component_x, bool component_y) = 0;

    // ///////////////////////////////////////////////////////////////////
    // accessors
    // ///////////////////////////////////////////////////////////////////

    // index in World's Entity array
    inline EntityWorldIndex GetWorldIndex () const { return m_world_index; }
    inline bool IsInWorld () const { return m_world_index != ENTITY_IS_NOT_IN_WORLD; }
    inline FloatVector2 const &GetWrappedOffset () const { return m_wrapped_offset; }
    inline FloatVector2 GetUnwrappedTranslation () const { return GetTranslation() + m_wrapped_offset; }
    // the Object which this Entity is attached to
    inline Object *GetOwnerObject () const { return m_owner_object; }
    // returns the owner object cast to a Sprite (but it asserts that
    // the owner object is actually a sprite).
    Sprite *GetOwnerSprite () const;
    // returns the owner object cast to a Compound (but it asserts that
    // the owner object is actually a compound).
    Compound *GetOwnerCompound () const;

    // ///////////////////////////////////////////////////////////////////
    // modifiers
    // ///////////////////////////////////////////////////////////////////

    // this should only be used by World
    inline void SetWorldIndex (EntityWorldIndex world_index) { m_world_index = world_index; }
    void SetWrappedOffset (FloatVector2 const &wrapped_offset);

    // ///////////////////////////////////////////////////////////////////
    // procedures
    // ///////////////////////////////////////////////////////////////////

    // for use in wrapped worlds
    void AccumulateWrappedOffset (FloatVector2 const &wrapped_offset_delta);
    // remove an entity from the world and physics handler (but don't delete it).
    void RemoveFromWorld ();
    // add a previously removed entity back into the world and object
    // layer it previously resided in.  must not already be in the world.
    void AddBackIntoWorld ();
    // resets the World's Entity index to the sentinel 'not in world'
    // value of -1.  should only be used by World
    inline void ResetWorldIndex () { m_world_index = ENTITY_IS_NOT_IN_WORLD; }
    // efficiently re-adds this Entity's object back into the quadtree
    void ReAddToQuadTree (QuadTreeType quad_tree_type);

    // ///////////////////////////////////////////////////////////////////
    // event-scheduling procedures
    // ///////////////////////////////////////////////////////////////////

    void ScheduleForDeletion (Float time_delay);
    void ScheduleForRemovalFromWorld (Float time_delay);

    // ///////////////////////////////////////////////////////////////////
    // Object frontend methods
    // ///////////////////////////////////////////////////////////////////

    inline ObjectType GetObjectType () const { ASSERT3(m_owner_object != NULL); return m_owner_object->GetObjectType(); }
    inline Float GetRadius (QuadTreeType quad_tree_type) const { ASSERT3(m_owner_object != NULL); return m_owner_object->GetRadius(quad_tree_type); }
    inline Float GetRadiusSquared (QuadTreeType quad_tree_type) const { ASSERT3(m_owner_object != NULL); return m_owner_object->GetRadiusSquared(quad_tree_type); }
    inline Float GetVisibleRadius () const { ASSERT3(m_owner_object != NULL); return m_owner_object->GetVisibleRadius(); }
    inline Float GetVisibleRadiusSquared () const { ASSERT3(m_owner_object != NULL); return m_owner_object->GetVisibleRadiusSquared(); }
    inline Float GetPhysicalRadius () const { ASSERT3(m_owner_object != NULL); return m_owner_object->GetPhysicalRadius(); }
    inline Float GetPhysicalRadiusSquared () const { ASSERT3(m_owner_object != NULL); return m_owner_object->GetPhysicalRadiusSquared(); }
    inline ObjectLayer *GetObjectLayer () const { ASSERT3(m_owner_object != NULL); return m_owner_object->GetObjectLayer(); }
    inline bool GetHasOwnerQuadTree (QuadTreeType quad_tree_type) const { ASSERT3(m_owner_object != NULL); return m_owner_object->GetHasOwnerQuadTree(quad_tree_type); }
    inline QuadTree *GetOwnerQuadTree (QuadTreeType quad_tree_type) const { ASSERT3(m_owner_object != NULL); return m_owner_object->GetOwnerQuadTree(quad_tree_type); }

    inline FloatVector2 const &GetTranslation () const { ASSERT3(m_owner_object != NULL); return m_owner_object->GetTranslation(); }
    inline FloatVector2 const &GetScaleFactors () const { ASSERT3(m_owner_object != NULL); return m_owner_object->GetScaleFactors(); }
    inline Float GetScaleFactor () const { ASSERT3(m_owner_object != NULL); return m_owner_object->GetScaleFactor(); }
    inline Float Angle () const { ASSERT3(m_owner_object != NULL); return m_owner_object->Angle(); }
    inline FloatMatrix2 const &GetTransformation () const { ASSERT3(m_owner_object != NULL); return m_owner_object->GetTransformation(); }
    inline FloatMatrix2 GetTransformationInverse () const { ASSERT3(m_owner_object != NULL); return m_owner_object->GetTransformationInverse(); }
    inline Float Determinant () const { ASSERT3(m_owner_object != NULL); return m_owner_object->Determinant(); }
    inline void SetTranslation (FloatVector2 const &translation) { ASSERT3(m_owner_object != NULL); m_owner_object->SetTranslation(translation); ReAddToQuadTree(QTT_VISIBILITY); ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
    inline void SetScaleFactors (FloatVector2 const &scale_factors) { ASSERT3(m_owner_object != NULL); m_owner_object->SetScaleFactors(scale_factors); ReAddToQuadTree(QTT_VISIBILITY); ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
    inline void SetScaleFactor (Float scale_factor) { ASSERT3(m_owner_object != NULL); m_owner_object->SetScaleFactor(scale_factor); ReAddToQuadTree(QTT_VISIBILITY); ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
    inline void SetAngle (Float const angle) { ASSERT3(m_owner_object != NULL); m_owner_object->SetAngle(angle); }
    inline void Translate (FloatVector2 const &translation) { ASSERT3(m_owner_object != NULL); m_owner_object->Translate(translation); ReAddToQuadTree(QTT_VISIBILITY); ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
    inline void Scale (FloatVector2 const &scale_factors) { ASSERT3(m_owner_object != NULL); m_owner_object->Scale(scale_factors); ReAddToQuadTree(QTT_VISIBILITY); ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
    inline void Scale (Float scale_factor) { ASSERT3(m_owner_object != NULL); m_owner_object->Scale(scale_factor); ReAddToQuadTree(QTT_VISIBILITY); ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
    inline void Rotate (Float const angle) { ASSERT3(m_owner_object != NULL); m_owner_object->Rotate(angle); }
    inline void ResetTranslation () { ASSERT3(m_owner_object != NULL); m_owner_object->ResetTranslation(); ReAddToQuadTree(QTT_VISIBILITY); ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
    inline void ResetScale () { ASSERT3(m_owner_object != NULL); m_owner_object->ResetScale(); ReAddToQuadTree(QTT_VISIBILITY); ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
    inline void ResetAngle () { ASSERT3(m_owner_object != NULL); m_owner_object->ResetAngle(); }

protected:

    // ///////////////////////////////////////////////////////////////////
    // protected interface methods
    // ///////////////////////////////////////////////////////////////////

    // will be called when this Entity is assigned to an Object
    virtual void HandleNewOwnerObject () = 0;
    // duplicates the given Entity's properties onto this one
    virtual void CloneProperties (Entity const *entity) = 0;

private:

    void ReadClassSpecific (Serializer &serializer);
    void WriteClassSpecific (Serializer &serializer) const;

    void HandleChangedRadius (Float old_radius, Float new_radius);

    EntityWorldIndex m_world_index;
    // (TODO: should this be implemented by the Entity subclass?)
    FloatVector2 m_wrapped_offset;
    Object *m_owner_object;

    friend class Object;
};

} // end of namespace Engine2

} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_ENTITY_HPP_)
