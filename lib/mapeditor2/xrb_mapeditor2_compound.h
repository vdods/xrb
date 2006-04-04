// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_compound.h by Victor Dods, created 2005/06/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_MAPEDITOR2_COMPOUND_H_)
#define _XRB_MAPEDITOR2_COMPOUND_H_

#include "xrb.h"

#include <list>

#include "xrb_engine2_compound.h"
#include "xrb_gltexture.h"
#include "xrb_mapeditor2_compoundvertex.h"
#include "xrb_mapeditor2_object.h"
#include "xrb_reference.h"
#include "xrb_resourcelibrary.h"

namespace Xrb
{

namespace MapEditor2
{

    class CompoundEntity;
    struct Polygon;
    
    class Compound :
        public virtual Object,
        public virtual Engine2::Compound
    {
    public:

        enum WeldReturnStatus
        {
            W_SUCCESSFUL = 0,           ///< The weld worked without any problems.
            W_WOULD_CAUSE_DEGENERATION, ///< At least one polygon would become degenerate.
            W_WOULD_CAUSE_PINCH,        ///< At least one polygon would become partially degenerate.

            W_RETURN_STATUS_COUNT       ///< The number of return status codes.
        }; // end of enum MapEditor2::Compound::WeldReturnStatus

        Compound (
            FloatVector2 const &polygon_center,
            Float polygon_radius,
            Float polygon_angle,
            Uint32 vertex_count,
            Resource<GLTexture> const &texture);

        virtual ~Compound ();

        // creates a non-entity version clone of the given compound entity.
        static Compound *CreateCompoundNonEntityClone (
            CompoundEntity const *compound_entity);
                            
        // ///////////////////////////////////////////////////////////////////
        // public serialization functions
        // ///////////////////////////////////////////////////////////////////
            
//         static Compound *Create (std::string const &prefab_filename);
        // creates a new Compound from data read in from the serializer
        static Compound *Create (Serializer &serializer);
        // makes calls to WriteClassSpecific for this and all superclasses
        virtual void Write (Serializer &serializer) const;
        
        // ///////////////////////////////////////////////////////////////////
        // public overridable methods
        // ///////////////////////////////////////////////////////////////////
        
        virtual Engine2::Object *CreateClone () const;
        // creates an entity version clone of this object, or NULL if
        // it either already is an entity, or if not applicable.
        virtual Entity *CreateEntityClone () const;
        // creates a non-entity version clone of this object, or NULL if
        // it either already is a non-entity, or if not applicable.
        virtual Object *CreateNonEntityClone () const
        {
            return NULL;
        }
        // Compound overrides Draw also because it doesn't render
        // out of Engine2::Compound's polygon array.
        virtual void Draw (
            DrawData const &draw_data,
            Float alpha_mask) const;
        // draws the lines and vertices
        virtual void DrawMetrics (
            DrawData const &draw_data,
            Float alpha_mask,
            MetricMode metric_mode);

        // ///////////////////////////////////////////////////////////////////
        // public accessors 
        // ///////////////////////////////////////////////////////////////////

        inline Uint32 GetPolygonCount () const { return m_polygon_list.size(); }
        Uint32 GetSelectedPolygonCount () const;
        Instance<CompoundVertex> *GetVertexInstance (Uint32 index) const;
        Polygon *GetSmallestPolygonTouchingPoint (
            FloatVector2 const &point) const;

        // ///////////////////////////////////////////////////////////////////
        // public procedures
        // ///////////////////////////////////////////////////////////////////

        void ComputeNearestVertex (
            FloatVector2 const &center,
            Float radius,
            Compound **compound_containing_nearest,
            CompoundVertex **nearest_vertex,
            Float *nearest_distance);
        WeldReturnStatus WeldSelectedVertices ();
        void UnweldSelectedVertices ();

        void AddDrawnPolygon (
            FloatVector2 const &polygon_center,
            Float polygon_radius,
            Float polygon_angle,
            Uint32 vertex_count,
            Resource<GLTexture> const &texture);
        void UnweldSelectedPolygons ();
        void DeleteSelectedPolygons ();

        void ApplyVertexSelectionOperation (
            FloatVector2 const &center,
            Float radius,
            Object::SelectionOperation selection_operation);
        void ApplyVertexSelectionOperation (
            CompoundVertex *vertex_to_select,
            Object::SelectionOperation selection_operation);
        void SelectAllVertices (bool toggle_selection);

        void TranslateVertex (
            CompoundVertex *vertex,
            FloatVector2 const &translation_delta);
        void ScaleVertex (
            CompoundVertex *vertex,
            Float scale_factor_delta,
            FloatVector2 const &transformation_origin);
        void RotateVertex (
            CompoundVertex *vertex,
            FloatMatrix2 const &rotation_transformation,
            FloatVector2 const &transformation_origin);

        void SelectAllPolygons (bool toggle_selection);
        void SetVertexSelectionStateFromSelectionOwnerPolygonCount ();
                                    
    protected:

        // protected constructor so you must use Create()
        Compound ();
    
        // ///////////////////////////////////////////////////////////////////
        // protected serialization functions
        // ///////////////////////////////////////////////////////////////////
        
        // does the guts of serializing reading for this class (doesn't read
        // the object subtype)
        void ReadClassSpecific (Serializer &serializer);
        // does the guts of serializing writing for this class (doesn't write
        // the object subtype)
        void WriteClassSpecific (Serializer &serializer) const;

        // ///////////////////////////////////////////////////////////////////
        // protected overridable methods
        // ///////////////////////////////////////////////////////////////////
        
        virtual void CalculateVisibleRadius () const;

        // ///////////////////////////////////////////////////////////////////
                        
        // clones the class-specific properties of the given object onto this.
        void CloneProperties (Engine2::Object const *object);
    
    private:
    
        inline Uint32 GetVertexCount () const
        {
            return m_vertex_list.size();
        }
        FloatVector2 const &GetVertex (Uint32 index) const;

        void AssignIndicesToVertices () const;
        void ResetVertexIndices () const;
        void ReplaceVertexWithNewVertex (
            Instance<CompoundVertex> *vertex_to_replace,
            Instance<CompoundVertex> *vertex_to_replace_with,
            bool use_unwelding_behavior);
    
        typedef std::list<Instance<CompoundVertex> *> VertexList;
        typedef VertexList::iterator VertexListIterator;
        typedef VertexList::const_iterator VertexListConstIterator;
    
        typedef std::list<Polygon *> PolygonList;
        typedef PolygonList::iterator PolygonListIterator;
        typedef PolygonList::const_iterator PolygonListConstIterator;

        VertexList m_vertex_list;
        PolygonList m_polygon_list;
    }; // end of class MapEditor2::Compound

} // end of namespace MapEditor2

} // end of namespace Xrb

#endif // !defined(_XRB_MAPEDITOR2_COMPOUND_H_)

