// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_events.cpp by Victor Dods, created 2005/11/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_events.h"

namespace Xrb
{

bool Engine2::MatchEntity (
    Event const *const event,
    Entity *const entity_to_match)
{
    ASSERT1(event != NULL);
    ASSERT1(entity_to_match != NULL);

    return dynamic_cast<EventEntity const *>(event) != NULL &&
           static_cast<EventEntity const *>(event)->GetEntity() == entity_to_match;
}

} // end of namespace Xrb
