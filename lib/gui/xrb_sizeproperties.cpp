// ///////////////////////////////////////////////////////////////////////////
// xrb_sizeproperties.cpp by Victor Dods, created 2005/06/12
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_sizeproperties.h"

namespace Xrb
{

SizeProperties::SizeProperties ()
    :
    m_min_size_enabled(false, false),
    m_min_size(GetDefaultMinSizeComponent(),
                GetDefaultMinSizeComponent()),
    m_max_size_enabled(false, false),
    m_max_size(GetDefaultMaxSizeComponent(),
                GetDefaultMaxSizeComponent())
{
    m_data = NULL;
}

ScreenCoord SizeProperties::GetAdjustedSizeComponent (
    Uint32 const index,
    ScreenCoord component) const
{
    ASSERT1(index <= 1)
    if (m_max_size_enabled.m[index] && component > m_max_size.m[index])
        component = m_max_size.m[index];
    // check min size second, because it takes precedence over max
    if (m_min_size_enabled.m[index] && component < m_min_size.m[index])
        component = m_min_size.m[index];
    return component;
}

ScreenCoordVector2 SizeProperties::GetAdjustedSize (
    ScreenCoordVector2 const &size) const
{
    return ScreenCoordVector2(
        GetAdjustedSizeComponent(Dim::X, size[Dim::X]),
        GetAdjustedSizeComponent(Dim::Y, size[Dim::Y]));
}

void SizeProperties::AdjustSizeComponent (
    Uint32 const index,
    ScreenCoord *const component) const
{
    ASSERT1(index <= 1)
    ASSERT1(component != NULL)
    if (m_max_size_enabled[index] && *component > m_max_size[index])
        *component = m_max_size[index];
    // check min size second, because it takes precedence over max
    if (m_min_size_enabled[index] && *component < m_min_size[index])
        *component = m_min_size[index];
}

void SizeProperties::AdjustSize (ScreenCoordVector2 *const size) const
{
    ASSERT1(size != NULL)
    AdjustSizeComponent(Dim::X, &size->m[Dim::X]);
    AdjustSizeComponent(Dim::Y, &size->m[Dim::Y]);
}

} // end of namespace Xrb
