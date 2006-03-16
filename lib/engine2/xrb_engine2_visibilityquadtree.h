// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_visibilityquadtree.h by Victor Dods, created 2004/06/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_VISIBILITYQUADTREE_H_)
#define _XRB_ENGINE2_VISIBILITYQUADTREE_H_

#include "xrb.h"

#include <set>

#include "xrb_color.h"
#include "xrb_engine2_quadtree.h"
#include "xrb_math.h"
#include "xrb_matrix2.h"
#include "xrb_rendercontext.h"

namespace Xrb
{

class Screen;
class Serializer;

namespace Engine2
{

    // The VisibilityQuadTree class implements a 2D space-organizing structure known as
    // a quad tree.  The idea is to improve visibility/collision by eliminating
    // a large number of candidates quickly.  It should be noted that it
    // actually uses circle-intersections to determine containment, which will
    // give a boost in speed due to the ease of calculating radiuses (as opposed
    // to messier rectangle intersection calculations).
    class VisibilityQuadTree : public QuadTree
    {
    public:
    
        // the VisibilityQuadTree::DrawData class nicely packages up a bunch of
        // variables which are used in the recursive Draw function.
        // passing a reference to an instance of this class speeds up access,
        // because then using the variables contained within can all be done
        // using known offsets from a single pointer.
        class DrawData
        {
        public:
    
            DrawData (
                RenderContext const &render_context,
                FloatMatrix2 const &world_to_screen,
                Float pixels_in_view_radius,
                FloatVector2 const &view_center,
                Float view_radius);
            ~DrawData () { }
    
            inline Object::DrawData const &GetObjectDrawData () const
            {
                return m_object_draw_data;
            }
            inline FloatMatrix2 const &GetWorldToScreen () const
            {
                return m_object_draw_data.GetTransformation();
            }
            inline Float GetPixelsInViewRadius () const
            {
                return m_pixels_in_view_radius;
            }
            inline FloatVector2 const &GetViewCenter () const
            {
                return m_view_center;
            }
            inline Float GetViewRadius () const
            {
                return m_view_radius;
            }
    
            inline void SetWorldToScreen (FloatMatrix2 const &world_to_screen)
            {
                m_object_draw_data.SetTransformation(world_to_screen);
            }
            inline void SetViewCenter (FloatVector2 const view_center)
            {
                m_view_center = view_center;
            }
        
        private:
    
            Object::DrawData m_object_draw_data;
            Float m_pixels_in_view_radius;
            FloatVector2 m_view_center;
            Float m_view_radius;    
        }; // end of class Engine2::VisibilityQuadTree::DrawData
    
        VisibilityQuadTree (
            FloatVector2 const &center,
            Float half_side_length,
            Uint8 depth);
        virtual ~VisibilityQuadTree () { }
    
        static VisibilityQuadTree *Create (Serializer &serializer);
    
        virtual void ReadStructure (Serializer &serializer);
        void WriteStructure (Serializer &serializer) const;
        virtual void ReadObjects (
            Serializer &serializer,
            ObjectLayer *object_layer);
        Uint32 WriteObjects (Serializer &serializer) const;
        
        Uint32 Draw (VisibilityQuadTree::DrawData const &draw_data);
        Uint32 DrawWrapped (VisibilityQuadTree::DrawData draw_data);
    
        // draw lines where the bounds of this quadtree node are
        void DrawBounds (
            RenderContext const &render_context,
            Color const &color);
        // draw the entire quadtree's bounding lines
        void DrawTreeBounds (
            RenderContext const &render_context,
            Color const &color);
            
    protected:
    
        // for use in Create
        VisibilityQuadTree (VisibilityQuadTree *parent) : QuadTree(parent) { }
    }; // end of class Engine2::VisibilityQuadTree

} // end of namespace Engine2

} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_VISIBILITYQUADTREE_H_)
