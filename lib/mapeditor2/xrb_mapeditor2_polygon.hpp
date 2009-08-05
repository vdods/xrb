// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_polygon.hpp by Victor Dods, created 2005/07/03
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_MAPEDITOR2_POLYGON_HPP_)
#define _XRB_MAPEDITOR2_POLYGON_HPP_

#include "xrb.hpp"

#include <list>

#include "xrb_gltexture.hpp"
#include "xrb_mapeditor2_compoundvertex.hpp"
#include "xrb_reference.hpp"
#include "xrb_resourcelibrary.hpp"
#include "xrb_vector.hpp"

namespace Xrb
{

class Serializer;

namespace MapEditor2
{

    class Compound;

    struct Polygon
    {
        struct Vertex
        {
            Reference<CompoundVertex> m_compound_vertex;
            FloatVector2 m_texture_coordinate;
        }; // end of struct MapEditor2::Polygon::Vertex
    
        typedef std::list<Vertex> VertexList;
        typedef VertexList::iterator VertexListIterator;
        typedef VertexList::const_iterator VertexListConstIterator;
    
        // list of vertices of the polygon (ordered counterclockwise)
        VertexList m_vertex_list;
        // the texture to draw the polygon with (an invalid Resource indicates
        // that no texture will be drawn and the polygon is invisible).
        Resource<GLTexture> m_texture;
        // indicates iff this polygon is currently selected
        bool m_is_selected;
        // the "owner" Compound
        Compound *m_owner_compound;

        // ///////////////////////////////////////////////////////////////////
        // constructor
        // ///////////////////////////////////////////////////////////////////
        
        Polygon (Compound *owner_compound);

        // ///////////////////////////////////////////////////////////////////
        // other stuff
        // ///////////////////////////////////////////////////////////////////
        
        Polygon *CreateClone () const;

        inline Uint32 GetVertexCount () const
        {
            return m_vertex_list.size();
        }
        Uint32 GetSelectedVertexCount () const;
        FloatVector2 const &GetVertex (Uint32 index) const;
        Float Area () const;    
        bool IsPointInside (FloatVector2 const &point) const;
        bool IsCounterclockwise () const;
        bool IsConvex () const;
        inline bool IsDegenerate () const
        {
            return Area() == 0.0f;
        }
        bool AreSelectedVerticesContiguous () const;

        void WeldSelectedVertices (Instance<CompoundVertex> *welded_vertex_instance);
                        
        // draws this Polygon -- it is assumed that the GL modelview matrix
        // and the color mask is already correctly set up.
        void Draw () const;
        void DrawMetrics () const;
            
        void ReassignVertices (Compound const *owner_compound);
        bool ReplaceVertexWithNewVertex (
            Instance<CompoundVertex> *vertex_to_replace,
            Instance<CompoundVertex> *vertex_to_replace_with);

        void SetVertexSelectionStateFromSelectionOwnerPolygonCount (ObjectLayer *object_layer);
        
        void IncrementSelectedOwnerPolygonCount (ObjectLayer *object_layer);
        void DecrementSelectedOwnerPolygonCount (ObjectLayer *object_layer);

        void SetIsSelected (bool is_selected, ObjectLayer *object_layer);
        void ToggleIsSelected (ObjectLayer *object_layer);
        void AddToPolygonSelectionSet (ObjectLayer *object_layer);
        void RemoveFromPolygonSelectionSet (ObjectLayer *object_layer);

        // serialization functions
        void Read (Serializer &serializer);
        void Write (Serializer &serializer) const;
    }; // end of struct MapEditor2::Polygon

} // end of namespace MapEditor2

} // end of namespace Xrb

#endif // !defined(_XRB_MAPEDITOR2_POLYGON_HPP_)

