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

namespace Xrb {

class RenderContext;
class Serializer;

namespace Engine2 {

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

    virtual ~Object ();

    // ///////////////////////////////////////////////////////////////////
    // public values and methods used for distance fade calculation
    // ///////////////////////////////////////////////////////////////////

    static Float const ms_radius_limit_upper;
    static Float const ms_radius_limit_lower;
    static Float const ms_distance_fade_slope;
    static Float const ms_distance_fade_intercept;

    static Float CalculateDistanceFade (Float object_radius);

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

    struct DrawData
    {
        RenderContext const &m_render_context;
        FloatMatrix2 const &m_world_to_screen;
        Color m_color_bias;
        Color m_color_mask;

        DrawData (RenderContext const &render_context, FloatMatrix2 const &world_to_screen)
            :
            m_render_context(render_context),
            m_world_to_screen(world_to_screen)
        { }
    }; // end of struct Engine2::Object::DrawData

    // draw this object using the given alpha mask
    virtual void Draw (DrawData const &draw_data) const { }
    // create a clone of this object
    virtual Object *Clone () const;

    // ///////////////////////////////////////////////////////////////////
    // public accessors and modifiers
    // ///////////////////////////////////////////////////////////////////

    /// Should return the handle of the relevant texture (e.g. sprite
    /// texture, animated sprite frame texture, etc).
    virtual Uint32 GlTextureAtlasHandle () const { return 0; }

    ObjectType GetObjectType () const { return m_object_type; }
    Float ZDepth () const { return m_z_depth; }
    bool IsDynamic () const { return m_entity != NULL; }
    Entity *GetEntity () const { return m_entity; }
    Color const &ColorBias () const { return m_color_bias; }
    Color const &ColorMask () const { return m_color_mask; }
    // use this to change the color bias
    Color &ColorBias () { return m_color_bias; }
    // use this to change the color mask
    Color &ColorMask () { return m_color_mask; }
    bool IsTransparent () const { return m_is_transparent; }
    Float Radius (QuadTreeType quad_tree_type) const { ASSERT1(quad_tree_type < QTT_COUNT); CalculateTransform(); return m_radius[quad_tree_type]; }
    Float RadiusSquared (QuadTreeType quad_tree_type) const { ASSERT1(quad_tree_type < QTT_COUNT); CalculateTransform(); return m_radius[quad_tree_type]*m_radius[quad_tree_type]; }
    Float VisibleRadius () const { CalculateTransform(); return m_radius[QTT_VISIBILITY]; }
    Float VisibleRadiusSquared () const { CalculateTransform(); return m_radius[QTT_VISIBILITY]*m_radius[QTT_VISIBILITY]; }
    Float PhysicalRadius () const { CalculateTransform(); return m_radius[QTT_PHYSICS_HANDLER]; }
    Float PhysicalRadiusSquared () const { CalculateTransform(); return m_radius[QTT_PHYSICS_HANDLER]*m_radius[QTT_PHYSICS_HANDLER]; }
    // returns the object_layer of this entity
    ObjectLayer *GetObjectLayer () const { return m_object_layer; }
    // returns the world this object resides in
    World *GetWorld () const;
    // returns true if this object has an owner quadtree
    bool HasOwnerQuadTree (QuadTreeType const quad_tree_type) const
    {
        ASSERT3(quad_tree_type <= QTT_COUNT);
        return m_owner_quad_tree[quad_tree_type] != NULL;
    }
    // returns the owner of this entity
    QuadTree *OwnerQuadTree (QuadTreeType const quad_tree_type) const
    {
        ASSERT3(quad_tree_type <= QTT_COUNT);
        return m_owner_quad_tree[quad_tree_type];
    }

    // set the z depth (as used by the OpenGL depth buffer during drawing).
    // a lower value indicates closer to the viewpoint (and will be drawn
    // on top of things with higher z-depth values).
    void SetZDepth (Float z_depth)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(z_depth));
        m_z_depth = z_depth;
    }
    // imbues this object with a soul
    void SetEntity (Entity *entity);
    // sets the is-transparent property
    void SetIsTransparent (bool is_transparent) { m_is_transparent = is_transparent; }
    // sets the object_layer for this object
    void SetObjectLayer (ObjectLayer *object_layer)
    {
        m_object_layer = object_layer;
    }
    // sets the owner for this object
    void SetOwnerQuadTree (
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
    virtual void CalculateRadius (QuadTreeType quad_tree_type) const { m_radius[quad_tree_type] = ScaleFactor(); }

    // ///////////////////////////////////////////////////////////////////

    // copies the properties of the given object to this object
    void CloneProperties (Object const &object);
    // takes care of recalculating the transform if necessary, and
    // recalculating the radii if necessary.
    void CalculateTransform () const;
    // causes the m_radii_need_to_be_recalculated flag to be set
    void IndicateRadiiNeedToBeRecalculated () { m_radii_need_to_be_recalculated = true; }

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
    Color m_color_bias;
    // color mask
    Color m_color_mask;
    // indicates that the contents of the object have changed and the
    // visible and physical radii need to be recalculated
    mutable bool m_radii_need_to_be_recalculated;
    // indicates that this object is (partially) transparent and should always go
    // into the ObjectVector for sorted back-to-front drawing.
    bool m_is_transparent; // NOTE: deprecated
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
                         /
           AnimatedSprite

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

*/
