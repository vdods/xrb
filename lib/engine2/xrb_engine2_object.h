// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_object.h by Victor Dods, created 2004/07/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_OBJECT_H_)
#define _XRB_ENGINE2_OBJECT_H_

#include "xrb.h"

#include "xrb_engine2_enums.h"
#include "xrb_engine2_types.h"
#include "xrb_transform2.h"
#include "xrb_vector.h"

namespace Xrb
{

class RenderContext;
class Serializer;

namespace Engine2
{

class Entity;
class ObjectLayer;
class QuadTree;
class World;

// base class for Sprite and Compound.  an Object which does not have an
// attached Entity will be referred to as a "static object" while an Object
// which has an attached Entity will be called a "dynamic object".
class Object
{
public:

    // the Object::DrawData class nicely packages up a bunch of
    // variables which are used in the frequently used Draw function.
    // passing a reference to an instance of this class speeds up access,
    // because then using the variables contained within can all be done
    // using known offsets from a single pointer.
    class DrawData
    {
    public:

        DrawData (
            RenderContext const &render_context,
            FloatMatrix2 const &transformation);
        ~DrawData () { }

        inline RenderContext const &GetRenderContext () const { return m_render_context; }
        inline FloatMatrix2 const &GetTransformation () const { return m_transformation; }

        inline void SetTransformation (FloatMatrix2 const &transformation) { m_transformation = transformation; }

    private:

        RenderContext const &m_render_context;
        FloatMatrix2 m_transformation;
    }; // end of class Engine2::Object::DrawData

    virtual ~Object ();

    // ///////////////////////////////////////////////////////////////////
    // public serialization functions
    // ///////////////////////////////////////////////////////////////////

    // creates a new Object from the serializer.  this is the main
    // multiplexing function which creates whatever type of object
    // subclass is actually stored in the serializer.
    static Object *Create (
        Serializer &serializer,
        CreateEntityFunction CreateEntity);
    // makes calls to WriteClassSpecific for this and all superclasses
    virtual void Write (Serializer &serializer) const;

    // ///////////////////////////////////////////////////////////////////
    // public interface methods
    // ///////////////////////////////////////////////////////////////////

    // draw this object on the SDL_Surface 'target', using the
    // specified transformation.
    virtual void Draw (
        DrawData const &draw_data,
        Float alpha_mask) const
    { }

    // ///////////////////////////////////////////////////////////////////
    // public accessors and modifiers
    // ///////////////////////////////////////////////////////////////////

    inline ObjectType GetObjectType () const { return m_object_type; }
    inline bool GetIsDynamic () const { return m_entity != NULL; }
    inline Entity *GetEntity () const { return m_entity; }
    inline Float GetRadius () const { CalculateTransform(); return m_radius; }
    inline Float GetRadiusSquared () const { CalculateTransform(); return m_radius*m_radius; }
    // returns the object_layer of this entity
    inline ObjectLayer *GetObjectLayer () const { return m_object_layer; }
    // returns the world this object resides in
    World *GetWorld () const;
    // returns true if this object has an owner quadtree
    inline bool GetHasOwnerQuadTree (QuadTreeType const quad_tree_type) const
    {
        ASSERT3(quad_tree_type <= QTT_COUNT)
        return m_owner_quad_tree[quad_tree_type] != NULL;
    }
    // returns the owner of this entity
    inline QuadTree *GetOwnerQuadTree (QuadTreeType const quad_tree_type) const
    {
        ASSERT3(quad_tree_type <= QTT_COUNT)
        return m_owner_quad_tree[quad_tree_type];
    }

    // imbues this object with a soul
    void SetEntity (Entity *entity);
    // sets the object_layer for this object
    inline void SetObjectLayer (ObjectLayer *const object_layer)
    {
        m_object_layer = object_layer;
    }
    // sets the owner for this object
    inline void SetOwnerQuadTree (
        QuadTreeType const quad_tree_type,
        QuadTree *const owner_quad_tree)
    {
        ASSERT3(quad_tree_type <= QTT_COUNT)
        m_owner_quad_tree[quad_tree_type] = owner_quad_tree;
    }

    // ///////////////////////////////////////////////////////////////////
    // frontends to FloatTransform2 functions (for m_transform)
    // ///////////////////////////////////////////////////////////////////

    inline FloatVector2 const &GetTranslation () const { return m_transform.GetTranslation(); }
    inline FloatVector2 const &GetScaleFactors () const { return m_transform.GetScaleFactors(); }
    inline Float GetScaleFactor () const { ASSERT1(m_transform.GetScaleFactors()[Dim::X] == m_transform.GetScaleFactors()[Dim::Y]) return m_transform.GetScaleFactors()[Dim::X]; }
    inline Float GetAngle () const { return m_transform.GetAngle(); }
    inline FloatTransform2 &GetTransform () { return m_transform; }
    inline FloatTransform2 const &GetTransform () const { return m_transform; }
    inline FloatMatrix2 const &GetTransformation () const { return m_transform.GetTransformation(); }
    inline FloatMatrix2 GetTransformationInverse () const { return m_transform.GetTransformationInverse(); }
    inline Float GetDeterminant () const { return m_transform.GetDeterminant(); }
    
    inline void SetTranslation (FloatVector2 const &translation)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(translation[Dim::X]))
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(translation[Dim::Y]))
        m_transform.SetTranslation(translation);
    }
    inline void SetTranslation (Float const x, Float const y)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(x))
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(y))
        m_transform.SetTranslation(x, y);
    }
    inline void SetScaleFactors (FloatVector2 const &scale_factors)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factors[Dim::X]))
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factors[Dim::Y]))
        m_transform.SetScaleFactors(scale_factors);
    }
    inline void SetScaleFactors (Float r, Float s)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(r))
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(s))
        m_transform.SetScaleFactors(r, s);
    }
    inline void SetScaleFactor (Float scale_factor)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factor))
        m_transform.SetScaleFactor(scale_factor);
    }
    inline void SetAngle (Float const angle)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(angle))
        m_transform.SetAngle(angle);
    }
    
    inline void Translate (FloatVector2 const &translation)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(translation[Dim::X]))
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(translation[Dim::Y]))
        m_transform.Translate(translation);
    }
    inline void Translate (Float const x, Float const y)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(x))
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(y))
        m_transform.Translate(x, y);
    }
    inline void Scale (FloatVector2 const &scale_factors)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factors[Dim::X]))
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factors[Dim::Y]))
        m_transform.Scale(scale_factors);
    }
    inline void Scale (Float r, Float s)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(r))
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(s))
        m_transform.Scale(r, s);
    }
    inline void Scale (Float scale_factor)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(scale_factor))
        m_transform.Scale(scale_factor);
    }
    inline void Rotate (Float const angle)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(angle))
        m_transform.Rotate(angle);
    }
    
    inline void ResetTranslation () { m_transform.ResetTranslation(); }
    inline void ResetScale () { m_transform.ResetScale(); }
    inline void ResetAngle () { m_transform.ResetAngle(); }

protected:

    // protected constructor so you must use Create()
    Object (ObjectType object_type);

    // ///////////////////////////////////////////////////////////////////
    // protected serialization functions
    // ///////////////////////////////////////////////////////////////////

    // reads the Object subclass type in order to know what subclass
    // of Object to create
    static ObjectType ReadObjectType (Serializer &serializer);
    // writes out the Object subclass type which is used by the reading
    // function to know what object to create while reading
    void WriteObjectType (Serializer &serializer) const;
    // does the guts of serializing reading for this class (doesn't read
    // the object subtype)
    void ReadClassSpecific (Serializer &serializer);
    // does the guts of serializing writing for this class (doesn't write
    // the object subtype)
    void WriteClassSpecific (Serializer &serializer) const;

    // ///////////////////////////////////////////////////////////////////
    // protected overridable methods
    // ///////////////////////////////////////////////////////////////////

    // recalculates the radius (this should be called if/when the object's
    // transformation matrix changes, setting m_radius to the calculated
    // value.
    virtual void CalculateRadius () const { m_radius = GetScaleFactor(); }

    // ///////////////////////////////////////////////////////////////////

    // copies the properties of the given object to this object
    void CloneProperties (Object const *object);
    // takes care of recalculating the transform if necessary, and
    // recalculating the radius if necessary.
    void CalculateTransform () const;
    // causes the m_radius_needs_to_be_recalculated flag to be set
    inline void IndicateRadiusNeedsToBeRecalculated () { m_radius_needs_to_be_recalculated = true; }

    // "radius" of the object
    mutable Float m_radius;
    // points to the ObjectLayer which this object exists in
    ObjectLayer *m_object_layer;
    // points to the quadtree nodes which currently 'own' this object.
    // (this is to make removal from the quadtrees faster)
    // there is one array entry for each type of quadtree (the visibility
    // quadtree in each object layer, and the physics handler quadtree)
    QuadTree *m_owner_quad_tree[QTT_COUNT];

private:

    // the type of object
    ObjectType const m_object_type;
    // this object's spatial transform
    FloatTransform2 m_transform;
    // a pointer to the optional Entity object which can "imbue this
    // object with a soul".
    Entity *m_entity;
    // indicates that the contents of the object have changed and the
    // radius needs to be recalculated
    mutable bool m_radius_needs_to_be_recalculated;
}; // end of class Engine2::Object

} // end of namespace Engine2

} // end of namespace Xrb
    
#endif // !defined(_XRB_ENGINE2_OBJECT_H_)

/*
//////////////////////////////////////////////////////////////////////////////
// Object/Entity redesign
//////////////////////////////////////////////////////////////////////////////

// Object hierarchy

Avoid complex and unnecessary (multiple virtual) inheritance.  Object will
be the sole base for physical items.  The specializations of Object are
Sprite (a simple single circle/square) and Compound (one or more polygons).

                                 Object
                                /      \
                          Sprite        Compound
    
Object can now be "imbued with a soul" by means of giving it a pointer to
an Entity (which is no longer part of the Object class hierarchy).  Entity
is now analogous to what EntityGuts was (an interface for the game-specific
data for each game object).  Each game will subclass the Entity interface
to specify its own game-specific data.

// World considerations

World will still accept adding Objects or Entities, but they will be kept
strictly separate.  An Object must not be imbued with an Entity while it is
owned by World (the Entity must be added before the Object is added to the
World).

// MapEditor2 classes

Instead of an ultra multiple-virtual inherited class hierarchy, a single
interface class will be used to provide the necessary functionality for
the map editor.  The object hierarchy is:

                                 Object
                                /      \
    MapEditor2::Object    Sprite        Compound    MapEditor2::Object
                      \  /                      \  /
               MapEditor2::Sprite       MapEditor2::Compound

// Specifying MapEditor2 GUI controls for Entity subclasses.

The goal here is to allow the map editor to edit Entity subclass data in a
robust and intuitive way (like how the built-in Object editing works).

The obvious type of controls will be Buttons, CheckBoxes, ValueEdits
and such, but there will also be primitives for controlling values via the
mapeditor's world itself (e.g. dragging tangible vector arrows around, etc).
Control primitives:

- Object-center-based vector (arrow)
- World coordinate position vector (something like the global origin cursor)
- Angle (like the Object editing mode's angle display)
- Scale (like the Object editing mode's scale display)

Only one of these tangible controls will be visible/controllable at a time,
and the active one will be set by picking one of the enumerated IDs that will
be attached to each specified control.

*** Think about how object selection set value editing should work (and if
the Entity controls should provide that)



*/
