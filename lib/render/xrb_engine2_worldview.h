// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_worldview.h by Victor Dods, created 2004/06/04
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_WORLDVIEW_H_)
#define _XRB_ENGINE2_WORLDVIEW_H_

#include "xrb.h"

#include "xrb_color.h"
#include "xrb_framehandler.h"
#include "xrb_rendercontext.h"
#include "xrb_transform2.h"
#include "xrb_vector.h"

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
    class WorldView : public FrameHandler
    {
    public:
    
        struct DrawInfo
        {
            // number of sprites drawn this Draw()
            Uint32 m_drawn_sprite_count;
    
            inline void Reset () { m_drawn_sprite_count = 0; }
        }; // end of struct DrawInfo
    
        WorldView (WorldViewWidget *parent_virtual_view);
        virtual ~WorldView ();
    
        inline WorldViewWidget *GetParentWorldViewWidget () const { return m_parent_world_view_widget; }
        inline World *GetWorld () const { return m_world; }
        inline DrawInfo const &GetDrawInfo () const { return m_draw_info; }
        inline Float GetViewZoomFactor () const { return m_zoom_factor; }
        inline FloatVector2 GetViewCenter () const { return -GetTranslation(); }
        inline Float GetViewAngle () const { return -GetAngle(); }
        inline bool GetIsViewLocked () const { return m_is_view_locked; }
        inline bool GetDrawBorderGridLines () const { return m_draw_border_grid_lines; }
        FloatMatrix2 GetCompoundTransformation () const;
        Float GetViewDepth (ObjectLayer const *object_layer) const;
        inline Float GetParallaxFactor (
            Float const view_depth,
            Float const layer_depth) const
        {
            ASSERT1(view_depth < layer_depth)
            return layer_depth - view_depth;
        }
        Float GetParallaxedViewRadius (ObjectLayer const *object_layer) const;
        FloatMatrix2 GetParallaxedTransformation (
            FloatMatrix2 const &world_to_view,
            FloatMatrix2 const &view_to_whatever,
            ObjectLayer const *object_layer) const;
        Float GetMinorAxisRadius () const;
        Float GetMajorAxisRadius () const;
        Float GetCornerRadius () const;
    
        inline void SetIsViewLocked (bool const is_view_locked) { m_is_view_locked = is_view_locked; }
        inline void SetDrawBorderGridLines (bool const draw_border_grid_lines) { m_draw_border_grid_lines = draw_border_grid_lines; }
        // formalized ways to change the view's position/zoom/rotation
        void SetViewCenter (FloatVector2 const &position);
        void SetViewZoomFactor (Float zoom_factor);
        void SetViewAngle (Float angle);
        // this method should only be called by World!
        inline void SetWorld (World *world)
        {
            m_world = world;
            HandleAttachedWorld();
        }
    
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
    
        ObjectLayer *GetMainObjectLayer () const;
        Float GetGridScaleUnit (Uint32 grid_scale) const;
        // parallaxed transformation/view-and-object-layer-property accessors
        FloatMatrix2 const &GetParallaxedWorldToWorldView () const;
        FloatMatrix2 const &GetParallaxedWorldViewToWorld () const;
        FloatMatrix2 const &GetParallaxedWorldToScreen () const;
        FloatMatrix2 const &GetParallaxedScreenToWorld () const;

        // this is called in SetWorld, after m_world is assigned.
        virtual void HandleAttachedWorld () { }
            
        virtual void ProcessFrameOverride ();

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
    
            GR_NUM_GRID_LINE_TYPES
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
    
        // ///////////////////////////////////////////////////////////////////////
        // frontends to FloatTransform2 functions (for m_transform)
        // ///////////////////////////////////////////////////////////////////////

        inline FloatVector2 const &GetTranslation () const { return m_transform.GetTranslation(); }
        inline FloatVector2 const &GetScaleFactors () const { return m_transform.GetScaleFactors(); }
        inline Float GetAngle () const { return m_transform.GetAngle(); }
        inline FloatMatrix2 const &GetTransformation () const { return m_transform.GetTransformation(); }
        inline FloatMatrix2 GetTransformationInverse () const { return m_transform.GetTransformationInverse(); }
        inline Float GetDeterminant () const { return m_transform.GetDeterminant(); }
        inline void SetTranslation (FloatVector2 const &translation) { m_transform.SetTranslation(translation); }
        inline void SetTranslation (Float const x, Float const y) { m_transform.SetTranslation(x, y); }
        inline void SetScaleFactors (FloatVector2 const &scale_factors) { m_transform.SetScaleFactors(scale_factors); }
        inline void SetScaleFactors (Float const r, Float const s) { m_transform.SetScaleFactors(r, s); }
        inline void SetScaleFactor (Float const scale_factor) { m_transform.SetScaleFactor(scale_factor); }
        inline void SetAngle (Float const angle) { m_transform.SetAngle(angle); }
        inline void Translate (FloatVector2 const &translation) { m_transform.Translate(translation); }
        inline void Translate (Float const x, Float const y) { m_transform.Translate(x, y); }
        inline void Scale (FloatVector2 const &scale_factors) { m_transform.Scale(scale_factors); }
        inline void Scale (Float const r, Float const s) { m_transform.Scale(r, s); }
        inline void Scale (Float const scale_factor) { m_transform.Scale(scale_factor); }
        inline void Rotate (Float const angle) { m_transform.Rotate(-angle); }
        inline void ResetTranslation () { m_transform.ResetTranslation(); }
        inline void ResetScale () { m_transform.ResetScale(); }
        inline void ResetAngle () { m_transform.ResetAngle(); }

        // ///////////////////////////////////////////////////////////////////////
        // end of frontends to FloatTransform2 functions (for m_transform)
        // ///////////////////////////////////////////////////////////////////////

        // World-to-WorldView transformation
        FloatTransform2 m_transform;
    
        // view's zoom factor (this is decoupled from the transform
        // because of the parallax computations).  the higher this number
        // goes, the more zoomed in the view is.
        Float m_zoom_factor;
        // indicates if the view cannot be moved/zoomed/rotated (used when
        // certain editing modes are engaged which require a static reference
        // frame)
        bool m_is_view_locked;
        // used for safety checking for pushing/popping the projection matrix
        // (move this into a GL state singleton later)
        bool m_is_gl_projection_matrix_in_use;
        // indicates iff the border of the main object layer will be drawn
        bool m_draw_border_grid_lines;
    
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

#endif // !defined(_XRB_ENGINE2_WORLDVIEW_H_)

