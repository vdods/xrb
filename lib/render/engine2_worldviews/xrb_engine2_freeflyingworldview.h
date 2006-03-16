// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_freeflyingworldview.h by Victor Dods, created 2005/06/18
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_FREEFLYINGWORLDVIEW_H_)
#define _XRB_ENGINE2_FREEFLYINGWORLDVIEW_H_

#include "xrb.h"

#include "xrb_engine2_worldview.h"

namespace Xrb
{

namespace Engine2
{

    class FreeFlyingWorldView : public WorldView
    {
    public:
    
        FreeFlyingWorldView (WorldViewWidget *parent_world_view_widget);
        virtual ~FreeFlyingWorldView ();
    
        // process a key event
        virtual bool ProcessKeyEvent (EventKey const *e);
        // process a mouse button event
        virtual bool ProcessMouseButtonEvent (EventMouseButton const *e);
        // process a mouse wheel event
        virtual bool ProcessMouseWheelEvent (EventMouseWheel const *e);
        // process a mouse motion event
        virtual bool ProcessMouseMotionEvent (EventMouseMotion const *e);
    
    protected:
    
        virtual void ProcessFrameOverride ();
    
    private:
    
        //////////////////////////////////////////////////////////////////////////
        // view movement vars
    
        // speed coefficient for using the arrow keys to move the view
        Float m_key_movement_speed_factor;
    
        //////////////////////////////////////////////////////////////////////////
        // view zooming vars
    
        // accumulates weighted zoom in/out requests.  zooming in increases
        // this value by m_zoom_increment, zooming out decreases it by the same.
        Float m_zoom_accumulator;
        // the quanta for m_zoom_accumulator.
        Float m_zoom_increment;
        // the rate at which to zoom.
        Float m_zoom_speed;
    
        //////////////////////////////////////////////////////////////////////////
        // view rotation vars
    
        // accumulates weighted rotate requests.  rotating counterclockwise
        // increases this value by m_rotation_increment, while rotating clockwise
        // decreases it by the same.
        Float m_rotation_accumulator;
        // the quanta for m_rotation_accumulator.
        Float m_rotation_increment;
        // the rate at which to rotate.
        Float m_rotation_speed;
    
    }; // end of class Engine2::FreeFlyingWorldView

} // end of namespace Engine2
    
} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_FREEFLYINGWORLDVIEW_H_)

