// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_worldview.hpp by Victor Dods, created 2004/06/04
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_WORLDVIEW_HPP_)
#define _XRB_ENGINE2_WORLDVIEW_HPP_

#include "xrb.hpp"

#include "xrb_color.hpp"
#include "xrb_engine2_types.hpp"
#include "xrb_framehandler.hpp"
#include "xrb_rendercontext.hpp"
#include "xrb_transform2.hpp"
#include "xrb_vector.hpp"

namespace Xrb
{

class EventKey;
class EventMouseButton;
class EventMouseWheel;
class EventMouseMotion;

namespace Engine2
{

    class WorldViewWidget;
    class World;
    class ObjectLayer;

    // The WorldView class implements all the view-specific details of drawing
    // the contents of what's in a World object.  WorldView inherits from
    // Transform2, and is used as the world-to-view transformation.
    // The WorldView is pre-translated.
    // WorldView inherits FloatTransform2 non-virtually.
    class WorldView : protected FloatTransform2, public FrameHandler
    {
    public:

        struct DrawInfo
        {
            // number of opaque Objects drawn this Draw()
            Uint32 m_drawn_opaque_object_count;
            // number of non-opaque Objects drawn this Draw()
            Uint32 m_drawn_transparent_object_count;

            void Reset ()
            {
                m_drawn_opaque_object_count = 0;
                m_drawn_transparent_object_count = 0;
            }
        }; // end of struct DrawInfo

        WorldView (WorldViewWidget *parent_world_view_widget);
        virtual ~WorldView ();

        inline WorldViewWidget *ParentWorldViewWidget () const { return m_parent_world_view_widget; }
        inline World *GetWorld () const { return m_world; }
        inline DrawInfo const &GetDrawInfo () const { return m_draw_info; }
        inline Float ZoomFactor () const { return m_zoom_factor; }
        inline FloatVector2 Center () const { return -Translation(); }
        inline Float Angle () const { return -FloatTransform2::Angle(); }
        inline bool IsViewLocked () const { return m_is_view_locked; }
        inline bool DrawBorderGridLines () const { return m_draw_border_grid_lines; }
        FloatMatrix2 CompoundTransformation () const;
        Float ViewDepth (ObjectLayer const *object_layer) const;
        inline Float ParallaxFactor (
            Float const view_depth,
            Float const layer_depth) const
        {
            ASSERT1(view_depth < layer_depth);
            return layer_depth - view_depth;
        }
        Float ParallaxedViewRadius (ObjectLayer const *object_layer) const;
        FloatMatrix2 ParallaxedTransformation (
            FloatMatrix2 const &world_to_view,
            FloatMatrix2 const &view_to_whatever,
            ObjectLayer const *object_layer) const;
        Float MinorAxisRadius () const;
        Float MajorAxisRadius () const;
        Float CornerRadius () const;
        inline bool IsTransformScalingBasedUponWidgetRadius () const
        {
            return m_is_transform_scaling_based_upon_widget_radius;
        }

        inline void SetIsViewLocked (bool const is_view_locked) { m_is_view_locked = is_view_locked; }
        inline void SetDrawBorderGridLines (bool const draw_border_grid_lines) { m_draw_border_grid_lines = draw_border_grid_lines; }
        // formalized ways to change the view's position/zoom/rotation
        void SetCenter (FloatVector2 const &position);
        void SetZoomFactor (Float zoom_factor);
        void SetAngle (Float angle);
        // modifiers for the zoom factor caps
        void SetMinZoomFactor (Float min_zoom_factor);
        void SetMaxZoomFactor (Float max_zoom_factor);
        // this method should only be called by World!
        inline void SetWorld (World *world)
        {
            m_world = world;
            HandleAttachedWorld();
        }
        void SetIsTransformScalingBasedUponWidgetRadius (bool is_transform_scaling_based_upon_widget_radius);

        inline void LockView ()
        {
            m_is_view_locked = true;
        }
        inline void UnlockView ()
        {
            m_is_view_locked = false;
        }
        // formalized ways to change the view's position/zoom/rotation
        void MoveView (FloatVector2 const &delta_position);
        void ZoomView (Float delta_zoom_factor);
        void RotateView (Float delta_angle);

        // detaches this view from the world it is connected to
        void DetachFromWorld ();

        // draws the contents of the view (bottom-most layer of the rendering)
        virtual void Draw (RenderContext const &render_context);

        // process a key event
        virtual bool ProcessKeyEvent (EventKey const *e);
        // process a mouse button event
        virtual bool ProcessMouseButtonEvent (EventMouseButton const *e);
        // process a mouse wheel event
        virtual bool ProcessMouseWheelEvent (EventMouseWheel const *e);
        // process a mouse motion event
        virtual bool ProcessMouseMotionEvent (EventMouseMotion const *e);

        virtual void HandleFocus () { }
        virtual void HandleUnfocus () { }

    protected:

        ObjectLayer *MainObjectLayer () const;
        Float GridScaleUnit (Uint32 grid_scale) const;
        // parallaxed transformation/view-and-object-layer-property accessors
        FloatMatrix2 const &ParallaxedWorldToWorldView () const;
        FloatMatrix2 const &ParallaxedWorldViewToWorld () const;
        FloatMatrix2 const &ParallaxedWorldToScreen () const;
        FloatMatrix2 const &ParallaxedScreenToWorld () const;

        // this is called in SetWorld, after m_world is assigned.
        virtual void HandleAttachedWorld () { }

        // FrameHandler interface method
        virtual void HandleFrame () { }

        void DrawGridLines (RenderContext const &render_context);
        void DrawGridLineSet (
            RenderContext const &render_context,
            Uint32 grid_scale,
            bool is_border_grid,
            bool is_wrapped,
            FloatVector2 const &view_center,
            Float view_radius,
            Color color);
        // internal calculation to get the radius of the circle which bounds
        // the visible rectangle
        Float CalculateViewRadius (
            FloatMatrix2 const &screen_to_world,
            ScreenCoordRect const &view_rect,
            FloatVector2 const &view_center) const;

        // sets up the world-to-screen coordinate transform in
        // the gl projection matrix.
        void PushParallaxedGLProjectionMatrix (
            RenderContext const &render_context,
            ObjectLayer const *object_layer);
        void PopGLProjectionMatrix ();

        enum GridLineType
        {
            GR_BELOW_MAIN_LAYER = 0,
            GR_NO_DRAW,
            GR_ABOVE_MAIN_LAYER,

            GR_COUNT
        }; // end of enum GridLineType

        // the WorldViewWidget that owns this view
        WorldViewWidget *m_parent_world_view_widget;
        // the world that this WorldView is attached to
        World *m_world;
        // draw info
        DrawInfo m_draw_info;

        //////////////////////////////////////////////////////////////////////////
        // grid line vars

        // draw grid lines below or above the main layer, or don't draw them.
        GridLineType m_grid_line_type;
        // the number of subdivisions inside each grid square
        Uint8 m_grid_number_base;
        // current grid scale (the scale is logarithmic, using m_grid_number_base
        // as the base, the largest being 0, going smaller as scale increases)
        Uint32 m_current_grid_scale;

    private:

        void DirtyAllParallaxedTransformations ();

        // view's zoom factor (this is decoupled from the transform
        // because of the parallax computations).  the higher this number
        // goes, the more zoomed in the view is.
        Float m_zoom_factor;
        // minimum allowable zoom factor value.  m_zoom_factor will be clamped
        // at this value if it is lower.
        Float m_min_zoom_factor;
        // maximum allowable zoom factor value.  m_zoom_factor will be clamped
        // at this value if it is higher.
        Float m_max_zoom_factor;
        // indicates if the view cannot be moved/zoomed/rotated (used when
        // certain editing modes are engaged which require a static reference
        // frame)
        bool m_is_view_locked;
        // used for safety checking for pushing/popping the projection matrix
        // (move this into a GL state singleton later)
        bool m_is_gl_projection_matrix_in_use;
        // indicates iff the border of the main object layer will be drawn
        bool m_draw_border_grid_lines;
        // if true, indicates the WorldView -> WorldViewWidget transform scaling is
        // based upon the widget radius (1/2 of its diagonal length).  otherwise,
        // indicates said scaling is based upon the smaller of the widget's height
        // and width.
        bool m_is_transform_scaling_based_upon_widget_radius;
        // used by Object::DrawLoopFunctor in ObjectLayer::Draw and
        // VisibilityQuadTree::Draw.
        TransparentObjectVector m_transparent_object_vector;

        //////////////////////////////////////////////////////////////////////////
        // cached parallaxed transformations/view-and-object-layer properties

        mutable bool m_is_parallaxed_world_to_view_dirty;
        mutable FloatMatrix2 m_parallaxed_world_to_view;
        mutable bool m_is_parallaxed_view_to_world_dirty;
        mutable FloatMatrix2 m_parallaxed_view_to_world;
        mutable bool m_is_parallaxed_world_to_screen_dirty;
        mutable FloatMatrix2 m_parallaxed_world_to_screen;
        mutable bool m_is_parallaxed_screen_to_world_dirty;
        mutable FloatMatrix2 m_parallaxed_screen_to_world;

    }; // end of class Engine2::WorldView

} // end of namespace Engine2

} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_WORLDVIEW_HPP_)
