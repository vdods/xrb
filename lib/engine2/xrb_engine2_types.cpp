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
    //     * color mask
    //     * color bias
    //     * texture atlas
    // - pointer values

    // NOTE: the color mask and color bias values are NOT the same
    // as the ones used while actually drawing, but it is sufficient
    // to distinguish the values at the Object level.

    Sint32 c;

    c = Compare(l.m_object->ZDepth(), r.m_object->ZDepth());
    if (c != 0)
        return c > 0; // prefer greater z depths.
    // otherwise they're equal, so continue.

    c = Compare(l.m_object->GlTextureAtlasHandle(), r.m_object->GlTextureAtlasHandle());
    if (c != 0)
        return c < 0;
    // otherwise they're equal, so continue.

    c = Compare(l.m_object->ColorBias(), r.m_object->ColorBias());
    if (c != 0)
        return c < 0;
    // otherwise they're equal, so continue.

    // TODO: account for distance fade here (need to first do above TODO re: render context)
    c = Compare(l.m_object->ColorMask(), r.m_object->ColorMask());
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
    m_pixels_in_view_radius(0.0f),
    m_view_center(FloatVector2::ms_zero),
    m_view_radius(0.0f)
{ }

void DrawObjectCollector::operator () (Object const *object)
{
    ASSERT3(object != NULL);
    ASSERT3(m_pixels_in_view_radius > 0.0f);
    ASSERT3(m_view_radius > 0.0f);
    // calculate the object's pixel radius on screen
    Float object_radius = m_pixels_in_view_radius * object->Radius(QTT_VISIBILITY) / m_view_radius;
    // distance culling - don't draw objects that are below the
    // gs_radius_limit_lower threshold
    if (object_radius >= Object::ms_radius_limit_lower)
        m_draw_object.push_back(DrawObject(object, Object::CalculateDistanceFade(object_radius)));
}

} // end of namespace Engine2
} // end of namespace Xrb
