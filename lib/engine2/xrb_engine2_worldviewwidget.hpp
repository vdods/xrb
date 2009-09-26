// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_worldviewwidget.hpp by Victor Dods, created 2004/06/12
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_WORLDVIEWWIDGET_HPP_)
#define _XRB_ENGINE2_WORLDVIEWWIDGET_HPP_

#include "xrb.hpp"

#include "xrb_simpletransform2.hpp"
#include "xrb_vector.hpp"
#include "xrb_widget.hpp"

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
        ContainerWidget *parent,
        std::string const &name = "Engine2::WorldViewWidget");
    virtual ~WorldViewWidget ();

    WorldView *GetWorldView ()
    {
        return m_world_view;
    }
    WorldView const *GetWorldView () const
    {
        return m_world_view;
    }
    FloatSimpleTransform2 const &Transformation () const
    {
        return m_transform;
    }
    bool IsTransformScalingBasedUponWidgetRadius () const
    {
        return m_is_transform_scaling_based_upon_widget_radius;
    }

    // sets the world_view to the given one, deleting the current one if not null
    void SetWorldView (WorldView *world_view);
    void SetIsTransformScalingBasedUponWidgetRadius (bool is_transform_scaling_based_upon_widget_radius);

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
    virtual void HandleFrame ();
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
    // if true, indicates the WorldView -> WorldViewWidget transform scaling is
    // based upon the widget radius (1/2 of its diagonal length).  otherwise,
    // indicates said scaling is based upon the smaller of the widget's height
    // and width.
    bool m_is_transform_scaling_based_upon_widget_radius;
}; // end of class Engine2::WorldViewWidget

} // end of namespace Engine2

} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_WORLDVIEWWIDGET_HPP_)
