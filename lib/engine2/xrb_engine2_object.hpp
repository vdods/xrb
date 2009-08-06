// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_object.hpp by Victor Dods, created 2004/07/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_OBJECT_HPP_)
#define _XRB_ENGINE2_OBJECT_HPP_

#include "xrb.hpp"

#include "xrb_color.hpp"
#include "xrb_engine2_enums.hpp"
#include "xrb_engine2_types.hpp"
#include "xrb_transform2.hpp"
#include "xrb_vector.hpp"

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
// Object inherits FloatTransform2 non-virtually.
class Object : public FloatTransform2
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
            FloatMatrix2 const &transformation)
            :
            m_render_context(render_context),
            m_transformation(transformation)
        { }
        ~DrawData () { }

        inline RenderContext const &GetRenderContext () const { return m_render_context; }
        inline FloatMatrix2 const &GetTransformation () const { return m_transformation; }

        inline void SetTransformation (FloatMatrix2 const &transformation) { m_transformation = transformation; }

    private:

        RenderContext const &m_render_context;
        FloatMatrix2 m_transformation;
    }; // end of class Engine2::Object::DrawData

    struct TransparentObjectOrder
    {
        bool operator () (Object const *t0, Object const *t1)
        {
            return t0->GetZDepth() > t1->GetZDepth()
                   ||
                   (t0->GetZDepth() == t1->GetZDepth() && t0 < t1);
        }
    }; // end of struct Engine2::Object::TransparentObjectOrder

    // the Engine2::Object::DrawLoopFunctor class nicely packages up a
    // bunch of variables which are used in the recursive Draw function.
    // passing a reference to an instance of this class speeds up access,
    // because then using the variables contained within can all be done
    // using known offsets from a single pointer.  It also behaves as a
    // functor for use in a std::for_each call on each quadtree node's
    // object set.
    class DrawLoopFunctor
    {
    public:

        // constants which control the thresholds at which objects use
        // alpha fading to fade away, when they become small enough.
        static Float const ms_radius_limit_upper;
        static Float const ms_radius_limit_lower;
        static Float const ms_distance_fade_slope;
        static Float const ms_distance_fade_intercept;

        DrawLoopFunctor (
            RenderContext const &render_context,
            FloatMatrix2 const &world_to_screen,
            Float pixels_in_view_radius,
            FloatVector2 const &view_center,
            Float view_radius,
            bool is_collect_transparent_object_pass,
            TransparentObjectVector *transparent_object_vector,
            QuadTreeType quad_tree_type);
        ~DrawLoopFunctor () { }

        inline Object::DrawData const &ObjectDrawData () const { return m_object_draw_data; }
        inline FloatMatrix2 const &GetWorldToScreen () const { return m_object_draw_data.GetTransformation(); }
        inline Float GetPixelsInViewRadius () const { return m_pixels_in_view_radius; }
        inline FloatVector2 const &GetViewCenter () const { return m_view_center; }
        inline Float GetViewRadius () const { return m_view_radius; }
        inline bool IsCollectTransparentObjectPass () const { return m_is_collect_transparent_object_pass; }
        inline TransparentObjectVector *GetTransparentObjectVector () const { return m_transparent_object_vector; }
        inline Uint32 DrawnOpaqueObjectCount () const { return m_drawn_opaque_object_count; }
        inline Uint32 DrawnTransparentObjectCount () const { return m_drawn_transparent_object_count; }

        inline void SetWorldToScreen (FloatMatrix2 const &world_to_screen) { m_object_draw_data.SetTransformation(world_to_screen); }
        inline void SetViewCenter (FloatVector2 view_center) { m_view_center = view_center; }
        inline void SetIsCollectTransparentObjectPass (bool is_collect_transparent_object_pass) { m_is_collect_transparent_object_pass = is_collect_transparent_object_pass; }

        // this method is what std::for_each will use to draw each object.
        void operator () (Engine2::Object const *object);

    private:

        Float CalculateDistanceFade (Float object_radius);

        Object::DrawData m_object_draw_data;
        Float m_pixels_in_view_radius;
        FloatVector2 m_view_center;
        Float m_view_radius;
        bool m_is_collect_transparent_object_pass;
        TransparentObjectVector *const m_transparent_object_vector;
        QuadTreeType m_quad_tree_type;
        mutable Uint32 m_drawn_opaque_object_count;
        mutable Uint32 m_drawn_transparent_object_count;
    }; // end of class Engine2::Object::DrawLoopFunctor

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
    inline Float GetZDepth () const { return m_z_depth; }
    inline bool IsDynamic () const { return m_entity != NULL; }
    inline Entity *GetEntity () const { return m_entity; }
    inline Color const &BiasColor () const { return m_bias_color; }
    inline Color const &ColorMask () const { return m_color_mask; }
    inline bool IsTransparent () const { return m_is_transparent; }
    inline Float GetRadius (QuadTreeType quad_tree_type) const { ASSERT1(quad_tree_type < QTT_COUNT); CalculateTransform(); return m_radius[quad_tree_type]; }
    inline Float GetRadiusSquared (QuadTreeType quad_tree_type) const { ASSERT1(quad_tree_type < QTT_COUNT); CalculateTransform(); return m_radius[quad_tree_type]*m_radius[quad_tree_type]; }
    inline Float GetVisibleRadius () const { CalculateTransform(); return m_radius[QTT_VISIBILITY]; }
    inline Float GetVisibleRadiusSquared () const { CalculateTransform(); return m_radius[QTT_VISIBILITY]*m_radius[QTT_VISIBILITY]; }
    inline Float GetPhysicalRadius () const { CalculateTransform(); return m_radius[QTT_PHYSICS_HANDLER]; }
    inline Float GetPhysicalRadiusSquared () const { CalculateTransform(); return m_radius[QTT_PHYSICS_HANDLER]*m_radius[QTT_PHYSICS_HANDLER]; }
    // returns the object_layer of this entity
    inline ObjectLayer *GetObjectLayer () const { return m_object_layer; }
    // returns the world this object resides in
    World *GetWorld () const;
    // returns true if this object has an owner quadtree
    inline bool HasOwnerQuadTree (QuadTreeType const quad_tree_type) const
    {
        ASSERT3(quad_tree_type <= QTT_COUNT);
        return m_owner_quad_tree[quad_tree_type] != NULL;
    }
    // returns the owner of this entity
    inline QuadTree *OwnerQuadTree (QuadTreeType const quad_tree_type) const
    {
        ASSERT3(quad_tree_type <= QTT_COUNT);
        return m_owner_quad_tree[quad_tree_type];
    }

    // set the z depth (as used by the OpenGL depth buffer during drawing).
    // a lower value indicates closer to the viewpoint (and will be drawn
    // on top of things with higher z-depth values).
    inline void SetZDepth (Float z_depth)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(z_depth));
        m_z_depth = z_depth;
    }
    // imbues this object with a soul
    void SetEntity (Entity *entity);
    // sets the color bias
    inline void SetBiasColor (Color const &bias_color) { m_bias_color = bias_color; }
    // sets the color mask
    inline void SetColorMask (Color const &color_mask) { m_color_mask = color_mask; }
    // sets the is-transparent property
    inline void SetIsTransparent (bool is_transparent) { m_is_transparent = is_transparent; }
    // sets the object_layer for this object
    inline void SetObjectLayer (ObjectLayer *object_layer)
    {
        m_object_layer = object_layer;
    }
    // sets the owner for this object
    inline void SetOwnerQuadTree (
        QuadTreeType const quad_tree_type,
        QuadTree *const owner_quad_tree)
    {
        ASSERT3(quad_tree_type <= QTT_COUNT);
        m_owner_quad_tree[quad_tree_type] = owner_quad_tree;
    }

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

    // recalculates the radii (visible, physical) of the object.
    // this should be called if/when the object's transformation matrix
    // changes, setting m_radius[quad_tree_type] to the calculated value.
    virtual void CalculateRadius (QuadTreeType quad_tree_type) const { m_radius[quad_tree_type] = GetScaleFactor(); }

    // ///////////////////////////////////////////////////////////////////

    // copies the properties of the given object to this object
    void CloneProperties (Object const *object);
    // takes care of recalculating the transform if necessary, and
    // recalculating the radii if necessary.
    void CalculateTransform () const;
    // causes the m_radii_need_to_be_recalculated flag to be set
    inline void IndicateRadiiNeedToBeRecalculated () { m_radii_need_to_be_recalculated = true; }

    // the radii of this object, one for each QuadTreeType (visible and physical)
    mutable Float m_radius[QTT_COUNT];
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
    // the z-depth value -- as used by the OpenGL depth buffer while drawing.
    // a lower value indicates closer to the viewpoint (and will be drawn
    // on top of things with higher z-depth values).
    Float m_z_depth;
    // a pointer to the optional Entity object which can "imbue this
    // object with a soul".
    Entity *m_entity;
    // color bias -- its alpha channel indicates the bias towards its RGB
    Color m_bias_color;
    // color mask
    Color m_color_mask;
    // indicates that the contents of the object have changed and the
    // visible and physical radii need to be recalculated
    mutable bool m_radii_need_to_be_recalculated;
    // indicates that this object is (partially) transparent and should always go
    // into the TransparentObjectVector for sorted back-to-front drawing.
    bool m_is_transparent;
}; // end of class Engine2::Object

} // end of namespace Engine2

} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_OBJECT_HPP_)

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
