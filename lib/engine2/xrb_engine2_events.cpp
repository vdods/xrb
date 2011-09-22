// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_events.cpp by Victor Dods, created 2005/11/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_events.hpp"

namespace Xrb {
namespace Engine2 {

bool MatchEntity (Event const &event, Entity *entity_to_match)
{
    ASSERT1(entity_to_match != NULL);
    EventEntity const *event_entity = dynamic_cast<EventEntity const *>(&event);
    return event_entity != NULL && event_entity->GetEntity() == entity_to_match;
}

} // end of namespace Engine2
} // end of namespace Xrb
