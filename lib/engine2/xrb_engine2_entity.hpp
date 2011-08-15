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

namespace Xrb {

class Serializer;

namespace Engine2 {

class AnimatedSprite;
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
    EntityWorldIndex WorldIndex () const { return m_world_index; }
    bool IsInWorld () const { return m_world_index != ENTITY_IS_NOT_IN_WORLD; }
    FloatVector2 const &WrappedOffset () const { return m_wrapped_offset; }
    FloatVector2 UnwrappedTranslation () const { return Translation() + m_wrapped_offset; }
    // the Object which this Entity is attached to
    Object const *OwnerObject () const { return m_owner_object; }
    Object *OwnerObject () { return m_owner_object; }
    // returns the owner object cast to a Sprite (doing appropriate
    // assert verification on its type)
    Sprite const *OwnerSprite () const;
    Sprite *OwnerSprite ();
    // returns the owner object cast to a AnimatedSprite (doing
    // appropriate assert verification on its type)
    AnimatedSprite const *OwnerAnimatedSprite () const;
    AnimatedSprite *OwnerAnimatedSprite () ;
    // returns the owner object cast to a Compound (doing appropriate
    // assert verification on its type)
    Compound const *OwnerCompound () const;
    Compound *OwnerCompound ();

    // ///////////////////////////////////////////////////////////////////
    // modifiers
    // ///////////////////////////////////////////////////////////////////

    // this should only be used by World
    void SetWorldIndex (EntityWorldIndex world_index) { m_world_index = world_index; }
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
    void ResetWorldIndex () { m_world_index = ENTITY_IS_NOT_IN_WORLD; }
    // efficiently re-adds this Entity's object back into the quadtree
    void ReAddToQuadTree (QuadTreeType quad_tree_type);

    // copies the properties of the given entity into this one
    // note: this entity must not be in the world during this call.
    void CloneProperties (Entity const &entity);

    // ///////////////////////////////////////////////////////////////////
    // event-scheduling procedures
    // ///////////////////////////////////////////////////////////////////

    void ScheduleForDeletion (Float time_delay);
    void ScheduleForRemovalFromWorld (Float time_delay);

    // ///////////////////////////////////////////////////////////////////
    // Object frontend methods
    // ///////////////////////////////////////////////////////////////////

    ObjectType GetObjectType () const { ASSERT3(m_owner_object != NULL); return m_owner_object->GetObjectType(); }
    Float Radius (QuadTreeType quad_tree_type) const { ASSERT3(m_owner_object != NULL); return m_owner_object->Radius(quad_tree_type); }
    Float VisibleRadius () const { ASSERT3(m_owner_object != NULL); return m_owner_object->VisibleRadius(); }
    Float PhysicalRadius () const { ASSERT3(m_owner_object != NULL); return m_owner_object->PhysicalRadius(); }
    Float Area (QuadTreeType quad_tree_type) const { ASSERT3(m_owner_object != NULL); return m_owner_object->Area(quad_tree_type); }
    Float VisibleArea () const { ASSERT3(m_owner_object != NULL); return m_owner_object->VisibleArea(); }
    Float PhysicalArea () const { ASSERT3(m_owner_object != NULL); return m_owner_object->PhysicalArea(); }
    ObjectLayer *GetObjectLayer () const { ASSERT3(m_owner_object != NULL); return m_owner_object->GetObjectLayer(); }
    World *GetWorld () const { ASSERT3(m_owner_object != NULL); return m_owner_object->GetWorld(); }
    bool HasOwnerQuadTree (QuadTreeType quad_tree_type) const { ASSERT3(m_owner_object != NULL); return m_owner_object->HasOwnerQuadTree(quad_tree_type); }
    QuadTree *OwnerQuadTree (QuadTreeType quad_tree_type) const { ASSERT3(m_owner_object != NULL); return m_owner_object->OwnerQuadTree(quad_tree_type); }

    FloatVector2 const &Translation () const { ASSERT3(m_owner_object != NULL); return m_owner_object->Translation(); }
    FloatVector2 const &ScaleFactors () const { ASSERT3(m_owner_object != NULL); return m_owner_object->ScaleFactors(); }
    Float ScaleFactor () const { ASSERT3(m_owner_object != NULL); return m_owner_object->ScaleFactor(); }
    Float Angle () const { ASSERT3(m_owner_object != NULL); return m_owner_object->Angle(); }
    FloatMatrix2 const &Transformation () const { ASSERT3(m_owner_object != NULL); return m_owner_object->Transformation(); }
    FloatMatrix2 TransformationInverse () const { ASSERT3(m_owner_object != NULL); return m_owner_object->TransformationInverse(); }
    Float Determinant () const { ASSERT3(m_owner_object != NULL); return m_owner_object->Determinant(); }
    void SetTranslation (FloatVector2 const &translation) { ASSERT3(m_owner_object != NULL); m_owner_object->SetTranslation(translation); ReAddToQuadTree(QTT_VISIBILITY); ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
    void SetScaleFactors (FloatVector2 const &scale_factors) { ASSERT3(m_owner_object != NULL); m_owner_object->SetScaleFactors(scale_factors); ReAddToQuadTree(QTT_VISIBILITY); ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
    void SetScaleFactor (Float scale_factor) { ASSERT3(m_owner_object != NULL); m_owner_object->SetScaleFactor(scale_factor); ReAddToQuadTree(QTT_VISIBILITY); ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
    void SetAngle (Float const angle) { ASSERT3(m_owner_object != NULL); m_owner_object->SetAngle(angle); }
    void Translate (FloatVector2 const &translation) { ASSERT3(m_owner_object != NULL); m_owner_object->Translate(translation); ReAddToQuadTree(QTT_VISIBILITY); ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
    void Scale (FloatVector2 const &scale_factors) { ASSERT3(m_owner_object != NULL); m_owner_object->Scale(scale_factors); ReAddToQuadTree(QTT_VISIBILITY); ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
    void Scale (Float scale_factor) { ASSERT3(m_owner_object != NULL); m_owner_object->Scale(scale_factor); ReAddToQuadTree(QTT_VISIBILITY); ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
    void Rotate (Float const angle) { ASSERT3(m_owner_object != NULL); m_owner_object->Rotate(angle); }
    void ResetTranslation () { ASSERT3(m_owner_object != NULL); m_owner_object->ResetTranslation(); ReAddToQuadTree(QTT_VISIBILITY); ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
    void ResetScale () { ASSERT3(m_owner_object != NULL); m_owner_object->ResetScale(); ReAddToQuadTree(QTT_VISIBILITY); ReAddToQuadTree(QTT_PHYSICS_HANDLER); }
    void ResetAngle () { ASSERT3(m_owner_object != NULL); m_owner_object->ResetAngle(); }

protected:

    // ///////////////////////////////////////////////////////////////////
    // protected interface methods
    // ///////////////////////////////////////////////////////////////////

    // will be called when this Entity is assigned to an Object
    virtual void HandleNewOwnerObject () = 0;

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
