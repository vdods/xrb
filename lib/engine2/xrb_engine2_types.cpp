// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_types.cpp by Victor Dods, created 2010/10/14
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_types.hpp"

#include "xrb_engine2_object.hpp"
#include "xrb_rendercontext.hpp"

namespace Xrb {
namespace Engine2 {

// ///////////////////////////////////////////////////////////////////////////
// DrawObjectOrder
// ///////////////////////////////////////////////////////////////////////////

bool DrawObjectOrder::operator () (DrawObject const &l, DrawObject const &r)
{
    ASSERT3(l.m_object != NULL);
    ASSERT3(r.m_object != NULL);

    // sort using these criteria, most important first:
    // - z depth (correctness of draw order)
    // - rendering params (to minimize the number of openGL calls)
    //     * texture atlas
    //     * color bias
    //     * color mask
    // - pointer values (wouldn't want this if we combined all wrapped rendering
    //                   into one std::sort.  see VisibilityQuadTree::DrawWrapped)

    Sint32 c;

    c = Compare(l.m_object->ZDepth(), r.m_object->ZDepth());
    if (c != 0)
        return c > 0; // prefer greater z depths.
    // otherwise they're equal, so continue.

    c = Compare(l.m_object->GlTextureAtlasHandle(), r.m_object->GlTextureAtlasHandle());
    if (c != 0)
        return c < 0;
    // otherwise they're equal, so continue.

    c = Compare(l.m_color_bias_rgba, r.m_color_bias_rgba);
    if (c != 0)
        return c < 0;
    // otherwise they're equal, so continue.

    c = Compare(l.m_color_mask_rgba, r.m_color_mask_rgba);
    if (c != 0)
        return c < 0;
    // otherwise they're equal, so continue.

    // the final test is pointer values.
    return l.m_object < r.m_object;
}

// ///////////////////////////////////////////////////////////////////////////
// DrawObjectCollector
// ///////////////////////////////////////////////////////////////////////////

DrawObjectCollector::DrawObjectCollector ()
    :
    m_render_context(NULL),
    m_pixels_in_view_radius(0.0f),
    m_view_center(FloatVector2::ms_zero),
    m_view_radius(0.0f)
{ }

void DrawObjectCollector::operator () (Object const *object)
{
    ASSERT3(object != NULL);
    ASSERT1(m_render_context != NULL);
    ASSERT1(m_pixels_in_view_radius > 0.0f);
    ASSERT1(m_view_radius > 0.0f);
    // calculate the object's pixel radius on screen
    Float object_radius = m_pixels_in_view_radius * object->Radius(QTT_VISIBILITY) / m_view_radius;
    // distance culling - don't draw objects that are below the ms_radius_limit_lower threshold
    if (object_radius >= Object::ms_radius_limit_lower)
    {
        // calculate the color bias
        Color color_bias(m_render_context->BlendedColorBias(object->ColorBias()));
        // calculate the color mask
        Color color_mask(m_render_context->MaskedColor(object->ColorMask()));
        color_mask[Dim::A] *= Object::CalculateDistanceFade(object_radius);
        // add the DrawObject
        m_draw_object.push_back(DrawObject(object, color_bias.Rgba(), color_mask.Rgba()));
    }
}

} // end of namespace Engine2
} // end of namespace Xrb
