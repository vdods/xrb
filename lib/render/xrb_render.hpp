// ///////////////////////////////////////////////////////////////////////////
// xrb_render.hpp by Victor Dods, created 2004/07/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_RENDER_HPP_)
#define _XRB_RENDER_HPP_

#include "xrb.hpp"

#include "xrb_color.hpp"
#include "xrb_matrix2.hpp"
#include "xrb_rect.hpp"
#include "xrb_rendercontext.hpp"
#include "xrb_vector.hpp"
#include "xrb_simpletransform2.hpp"

namespace Xrb
{

class Screen;
class GlTexture;

/** The rendering primitives are
  *     <ul>
  *     <li>Drawing a line.</li>
  *     <li>Drawing an arrow, with arrow's head size proportional to the arrow's length.</li>
  *     <li>Drawing a circle.</li>
  *     <li>Drawing a circular arc.</li>
  *     <li>Filling a rectangle with a solid color.</li>
  *     <li>Mapping a texture to a rectangle.</li>
  *     </ul>
  *
  * All functions requires a RenderContext (with a few exceptions).
  *
  * Some functions operate using the existing OpenGL modelview matrix, while
  * others reset it to the identity.
  *
  * None of the functions change the OpenGL projection matrix.  However, it
  * should be noted that each function depends on the projection matrix being
  * set up in a particular mode.  Specifically, there are two types of modes
  * currently: in-Widget rendering, and in-WorldView rendering.  In-Widget
  * rendering takes place when the projection matrix is set up to make the
  * modelview coordinates exactly coincide with the screen's pixel coordinates
  * (though in a right-handed coordinate system).  The in-WorldView rendering
  * mode assumes the projection matrix is set up so that the zooming, scaling,
  * and positioning of the view is taken into account, and so the modelview
  * coordinates represent actual world coordinates.
  *
  * @brief Houses all of the basic rendering primitives used by XuqRijBuh.
  */
namespace Render
{
    // ///////////////////////////////////////////////////////////////////////
    // in-WorldView rendering functions
    // ///////////////////////////////////////////////////////////////////////

    /** Obliterates the modelview matrix.
      * @brief Draws a line between @c from and @c to in the given color.
      * @param render_context The required RenderContext.
      * @param from One of the line's endpoints, in modelspace.
      * @param to The other of the line's endpoints, in modelspace.
      * @param color The color to use to render the line.
      */
    void DrawLine (
        RenderContext const &render_context,
        FloatVector2 const &from,
        FloatVector2 const &to,
        Color const &color);
    /** The size of the arrow's head is proportional to its length.
      *
      * Obliterates the modelview matrix.
      *
      * @brief Draws an arrow with tail at @c from and head at @c to, in
      *        the given color.
      * @param render_context The required RenderContext.
      * @param from The tail endpoint of the arrow, in modelspace.
      * @param to The head endpoint of the arrow, in modelspace.
      * @param color The color to use to render the arrow.
      */
    void DrawArrow (
        RenderContext const &render_context,
        FloatVector2 const &from,
        FloatVector2 const &to,
        Color const &color);
    /** The vertices of the polygon will be placed on the bounding circle
      * specified by @c center and @c radius.
      *
      * Obliterates the modelview matrix.
      *
      * @brief Draws a regular polygon.
      * @param render_context The required RenderContext.
      * @param center The center of the bounding circle that the polygon's
      *               vertices will lay upon, in modelspace.
      * @param radius The radius of the bounding circle that the polygon's
      *               vertices will lay upon, in modelspace.
      * @param angle The angle at which the first vertex will be placed upon
      *              the bounding circle.
      * @param color The color to use to render the polygon.
      * @param fill Specify true iff the polygon should be filled with the color.
      * @param vertex_count The number of vertices the polygon should have.
      */
    void DrawPolygon (
        RenderContext const &render_context,
        FloatVector2 const &center,
        Float radius,
        Float angle,
        Color const &color,
        bool fill,
        Uint32 vertex_count);
    /** The number of lines used to draw the circle is dependent on
      * @c transformation.  The larger the circle appears, the more
      * lines will be used to draw it.  No more than 30 lines and no
      * less than 6 lines will be used to draw the circle.
      *
      * Obliterates the modelview matrix.
      *
      * @brief Draws a circle with the given center and radius, in the
      *        given color.
      * @param render_context The required RenderContext.
      * @param transformation The world-to-screen transformation matrix
      *                       which is used to calculate the necessary number
      *                       of lines to use to draw the circle.
      * @param center The center of the circle, in modelspace.
      * @param radius The radius of the circle, in modelspace.
      * @param color The color to use to render the circle.
      * @param fill Specify true iff the circle should be filled with the color.
      */
    void DrawCircle (
        RenderContext const &render_context,
        FloatMatrix2 const &transformation,
        FloatVector2 const &center,
        Float radius,
        Color const &color,
        bool fill);
    /** The number of lines used to draw the arc is dependent on
      * @c transformation.  The larger the arc appears, the more
      * lines will be used to draw it.  No more than 30 lines per full
      * 360 degree arc, and no less than 6 lines per full 360 degree arc
      * will be used to draw the circular arc.
      *
      * Obliterates the modelview matrix.
      *
      * @brief Draws a circular arc with the given center and radius,
      *        start angle and end angle, in the given color.
      * @param render_context The required RenderContext.
      * @param transformation The world-to-screen transformation matrix
      *                       which is used to calculate the necessary number
      *                       of lines to use to draw the circular arc.
      * @param center The center of the circle that the arc lies on,
      *               in modelspace.
      * @param radius The radius of the circle that the arc lies on,
      *               in modelspace.
      * @param start_angle The angle at which to start the arc, in modelspace,
      *                    where 0 degrees is along the positive X axis, and
      *                    degrees increase to move counterclockwise.
      * @param end_angle The angle at which to end the arc, in modelspace,
      *                  where 0 degrees is along the positive X axis, and
      *                  degrees increase to move counterclockwise.  The total
      *                  angle spanned by @c end_angle @c - @c start_angle can
      *                  be greater than 360 if desired.
      * @param color The color to use to render the arc.
      */
    void DrawCircularArc (
        RenderContext const &render_context,
        FloatMatrix2 const &transformation,
        FloatVector2 const &center,
        Float radius,
        Float start_angle,
        Float end_angle,
        Color const &color);

    // ///////////////////////////////////////////////////////////////////////
    // in-Widget rendering functions
    // ///////////////////////////////////////////////////////////////////////

    /** Obliterates the modelview matrix.
      * @brief Fills the given rectangle with the given color.
      * @param render_context The required RenderContext.
      * @param color The color to use to render the rectangle.
      * @param screen_rect The screen-coordinate rectangle to fill.
      */
    void DrawScreenRect (
        RenderContext const &render_context,
        Color const &color,
        ScreenCoordRect const &screen_rect);
    /** Obliterates the modelview matrix.
      * @brief Maps the given texture to the given rectangle.
      * @param render_context The required RenderContext.
      * @param gltexture A pointer to the GlTexture which should be used.
      * @param screen_rect The screen rectangle to be filled with the texture.
      * @param transformation An optional transformation to the texture
      *                       coordinates (e.g. for flipping horizontally,
      *                       vertically, or scaling the texture).
      */
    void DrawScreenRectTexture (
        RenderContext const &render_context,
        GlTexture const &gltexture,
        ScreenCoordRect const &screen_rect,
        FloatSimpleTransform2 const &transformation =
            FloatSimpleTransform2::ms_identity);

} // end of namespace Render

} // end of namespace Xrb

#endif // !defined(_XRB_RENDER_HPP_)
