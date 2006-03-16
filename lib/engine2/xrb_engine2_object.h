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

#include "xrb_color.h"
#include "xrb_engine2_enums.h"
#include "xrb_rendercontext.h"
#include "xrb_transform2.h"
#include "xrb_vector.h"

namespace Xrb
{

class Serializer;

namespace Engine2
{

    class ObjectLayer;
    class QuadTree;
    class World;
    
    // abstract parent class for Entity and Sprite
    class Object
    {
    public:
    
        enum SubType
        {
            ST_INVALID = 0,
    
            ST_OBJECT,
            ST_SPRITE,
            ST_COMPOUND,
            ST_ENTITY,
            ST_SPRITE_ENTITY,
            ST_COMPOUND_ENTITY,
            // NOTE: if you add more enums, make sure to update ST_HIGEST_SUB_TYPE
    
            ST_NUM_SUB_TYPES,
    
            ST_LOWEST_SUB_TYPE = ST_OBJECT,
            ST_HIGHEST_SUB_TYPE = ST_COMPOUND_ENTITY
        }; // end of enum Engine2::Object::SubType

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
    
            inline RenderContext const &GetRenderContext () const
            {
                return m_render_context;
            }
            inline FloatMatrix2 const &GetTransformation () const
            {
                return m_transformation;
            }
    
            inline void SetTransformation (FloatMatrix2 const &transformation)
            {
                m_transformation = transformation;
            }
        
        private:
    
            RenderContext m_render_context;
            FloatMatrix2 m_transformation;
        }; // end of class Engine2::Object::DrawData

        virtual ~Object () { }
    
        // ///////////////////////////////////////////////////////////////////
        // public serialization functions
        // ///////////////////////////////////////////////////////////////////
        
        // creates a new Object from the serializer.  this is the main
        // multiplexing function which creates whatever type of object
        // subclass is actually stored in the serializer.
        static Object *Create (Serializer &serializer);
        // makes calls to WriteClassSpecific for this and all superclasses
        virtual void Write (Serializer &serializer) const;

        // ///////////////////////////////////////////////////////////////////
        // public overridable methods
        // ///////////////////////////////////////////////////////////////////
        
        // creates a cloned version of this object
        virtual Object *CreateClone () const = 0;
        // draw this object on the SDL_Surface 'target', using the
        // specified transformation.
        virtual void Draw (
            DrawData const &draw_data,
            Float alpha_mask) const = 0;
    
        // ///////////////////////////////////////////////////////////////////
        // frontends to FloatTransform2 functions (for m_transform)
        // ///////////////////////////////////////////////////////////////////
    
        inline FloatVector2 const &GetTranslation () const
        {
            return m_transform.GetTranslation();
        }
        inline FloatVector2 const &GetScaleFactors () const
        {
            return m_transform.GetScaleFactors();
        }
        inline Float GetScaleFactor () const
        {
            ASSERT1(m_transform.GetScaleFactors()[Dim::X] == m_transform.GetScaleFactors()[Dim::Y])
            return m_transform.GetScaleFactors()[Dim::X];
        }
        inline Float GetAngle () const
        {
            return m_transform.GetAngle();
        }
        inline FloatTransform2 &GetTransform ()
        {
            return m_transform;
        }
        inline FloatTransform2 const &GetTransform () const
        {
            return m_transform;
        }
        inline FloatMatrix2 const &GetTransformation () const
        {
            return m_transform.GetTransformation();
        }
        inline FloatMatrix2 GetTransformationInverse () const
        {
            return m_transform.GetTransformationInverse();
        }
        inline Float GetDeterminant () const
        {
            return m_transform.GetDeterminant();
        }
        inline void SetTranslation (FloatVector2 const &translation)
        {
            m_transform.SetTranslation(translation);
        }
        inline void SetTranslation (Float const x, Float const y)
        {
            m_transform.SetTranslation(x, y);
        }
        // these three scaling methods are virtual because the world/physics
        // handler needs to know when an Entity's scale factor(s) have changed.
        virtual void SetScaleFactors (FloatVector2 const &scale_factors)
        {
            m_transform.SetScaleFactors(scale_factors);
        }
        virtual void SetScaleFactors (Float r, Float s)
        {
            m_transform.SetScaleFactors(r, s);
        }
        virtual void SetScaleFactor (Float scale_factor)
        {
            m_transform.SetScaleFactor(scale_factor);
        }
        inline void SetAngle (Float const angle)
        {
            m_transform.SetAngle(angle);
        }
        inline void Translate (FloatVector2 const &translation)
        {
            m_transform.Translate(translation);
        }
        inline void Translate (Float const x, Float const y)
        {
            m_transform.Translate(x, y);
        }
        // these three scaling methods are virtual because the world/physics
        // handler needs to know when an Entity's scale factor(s) have changed.
        virtual void Scale (FloatVector2 const &scale_factors)
        {
            m_transform.Scale(scale_factors);
        }
        virtual void Scale (Float r, Float s)
        {
            m_transform.Scale(r, s);
        }
        virtual void Scale (Float scale_factor)
        {
            m_transform.Scale(scale_factor);
        }
        inline void Rotate (Float const angle)
        {
            m_transform.Rotate(angle);
        }
        inline void ResetTranslation ()
        {
            m_transform.ResetTranslation();
        }
        // this is virtual because the world/physics handler needs
        // to know when an Entity's scale factor(s) have changed.
        virtual void ResetScale ()
        {
            m_transform.ResetScale();
        }
        inline void ResetAngle ()
        {
            m_transform.ResetAngle();
        }
    
        // ///////////////////////////////////////////////////////////////////
        // public accessors and modifiers
        // ///////////////////////////////////////////////////////////////////

        inline SubType GetSubType () const
        {
            return m_sub_type;
        }    
        inline Float GetRadius () const
        {
            CalculateTransform();
            return m_radius;
        }
        inline Float GetRadiusSquared () const
        {
            CalculateTransform();
            return m_radius*m_radius;
        }
        // returns true if this object has an owner
        inline bool GetHasOwner (QuadTreeType const quad_tree_type) const
        {
            ASSERT3(quad_tree_type <= QTT_NUM_TYPES)
            return m_owner_quad_tree[quad_tree_type] != NULL;
        }
        // returns the owner of this entity
        inline QuadTree *GetOwnerQuadTree (QuadTreeType const quad_tree_type) const
        {
            ASSERT3(quad_tree_type <= QTT_NUM_TYPES)
            return m_owner_quad_tree[quad_tree_type];
        }
        // returns the object_layer of this entity
        inline ObjectLayer *GetObjectLayer () const
        {
            return m_object_layer;
        }
        // returns the world this object resides in
        World *GetWorld () const;
    
        // sets the owner for this object
        inline void SetOwnerQuadTree (
            QuadTreeType const quad_tree_type,
            QuadTree *const owner_quad_tree)
        {
            ASSERT3(quad_tree_type <= QTT_NUM_TYPES)
            m_owner_quad_tree[quad_tree_type] = owner_quad_tree;
        }
        // sets the object_layer for this object
        inline void SetObjectLayer (ObjectLayer *const object_layer)
        {
            m_object_layer = object_layer;
        }

    protected:

        // protected constructor so you must use Create()        
        Object ();

        // ///////////////////////////////////////////////////////////////////
        // protected serialization functions
        // ///////////////////////////////////////////////////////////////////
        
        // reads the Object subclass type in order to know what subclass
        // of Object to create
        static SubType ReadSubType (Serializer &serializer);
        // writes out the Object subclass type which is used by the reading
        // function to know what object to create while reading
        void WriteSubType (Serializer &serializer) const;
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
        virtual void CalculateRadius () const = 0;

        // ///////////////////////////////////////////////////////////////////
        
        // copies the properties of the given object to this object
        void CloneProperties (Object const *object);
        // takes care of recalculating the transform if necessary, and
        // recalculating the radius if necessary.
        void CalculateTransform () const;    
        // causes the m_radius_needs_to_be_recalculated flag to be set
        void IndicateRadiusNeedsToBeRecalculated ();
    
        // the type of object
        SubType m_sub_type;
        // "radius" of the object
        mutable Float m_radius;
        // points to the quadtree nodes which currently 'own' this object.
        // (this is to make removal from the quadtrees faster)
        // there is one array entry for each type of quadtree (the visibility
        // quadtree in each object layer, and the physics handler quadtree)
        QuadTree *m_owner_quad_tree[QTT_NUM_TYPES];
        // points to the ObjectLayer which this object exists in
        ObjectLayer *m_object_layer;
    
    private:

        // this object's spatial transform
        FloatTransform2 m_transform;
        // indicates that the contents of the object have changed and the
        // radius needs to be recalculated
        mutable bool m_radius_needs_to_be_recalculated;
    }; // end of class Engine2::Object

} // end of namespace Engine2

} // end of namespace Xrb
    
#endif // !defined(_XRB_ENGINE2_OBJECT_H_)
