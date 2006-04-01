// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_worldviewwidget.h by Victor Dods, created 2004/06/12
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_WORLDVIEWWIDGET_H_)
#define _XRB_ENGINE2_WORLDVIEWWIDGET_H_

#include "xrb.h"

#include "xrb_simpletransform2.h"
#include "xrb_vector.h"
#include "xrb_widget.h"

namespace Xrb
{

namespace Engine2
{
    
    class WorldView;
    
    // The WorldViewWidget class serves as the viewport-size-dependent intermediary
    // between WorldView and Screen which will manage drawing the WorldView in
    // the correct position (also allowing for multiple WorldViews in one
    // Screen).  This class also embodies the notion of a view frame
    // (rather than having a limitless view).
    class WorldViewWidget : public Widget
    {
    public:
    
        WorldViewWidget (
            WorldView *world_view,
            Widget *parent,
            std::string const &name = "Engine2::WorldViewWidget");
        virtual ~WorldViewWidget ();
    
        inline WorldView *GetWorldView ()
        {
            return m_world_view;
        }
        inline WorldView const *GetWorldView () const
        {
            return m_world_view;
        }
        inline FloatSimpleTransform2 const &GetTransformation () const
        {
            return m_transform;
        }
    
        // sets the world_view to the given one, deleting the current one if not null
        void SetWorldView (WorldView *world_view);
    
        // draws the contents of the world view
        virtual void Draw (RenderContext const &render_context) const;
        // overridden MoveBy so that m_dirty can be set and the transform
        // updated correctly
        virtual void MoveBy (ScreenCoordVector2 const &delta);
        // overridden Resize so that m_dirty can be set and the transform
        // updated correctly
        virtual ScreenCoordVector2 Resize (ScreenCoordVector2 const &size);
    
    protected:
    
        // this calculates one frame, called by the game loop
        virtual void ProcessFrameOverride ();
        // process a key event
        virtual bool ProcessKeyEvent (EventKey const *e);
        // process a mouse button event
        virtual bool ProcessMouseButtonEvent (EventMouseButton const *e);
        // process a mouse wheel event
        virtual bool ProcessMouseWheelEvent (EventMouseWheel const *e);
        // process a mouse motion event
        virtual bool ProcessMouseMotionEvent (EventMouseMotion const *e);

        virtual void HandleFocus ();
        virtual void HandleUnfocus ();
        
        // draws the inside border of the widget
        void DrawFocusFrame (RenderContext const &render_context) const;
        // recomputes the Transform2 with the new virtual rect size
        void ComputeTransform ();
    
        // indicates wether or not to draw the focus frame
        bool m_draw_focus_frame;
    
    private:
    
        // the attached world_view
        WorldView *m_world_view;
        // gives the transformation from WorldView coordinates to Screen coordinates.
        FloatSimpleTransform2 m_transform;
    }; // end of class Engine2::WorldViewWidget

} // end of namespace Engine2

} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_WORLDVIEWWIDGET_H_)