// ///////////////////////////////////////////////////////////////////////////
// xrb_gui_events.cpp by Victor Dods, created 2005/11/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_gui_events.hpp"

#include "xrb_screen.hpp"
#include "xrb_widget.hpp"
#include "xrb_widgetcontext.hpp"

namespace Xrb {

void EventDetachAndDeleteChildWidget::DetachAndDeleteChildWidget () const
{
    ASSERT1(m_child_to_detach_and_delete != NULL);
    if (m_child_to_detach_and_delete->IsModal())
        m_child_to_detach_and_delete->Context().GetScreen().DetachAsModalChildWidget(*m_child_to_detach_and_delete);
    else
        m_child_to_detach_and_delete->DetachFromParent();
    DeleteAndNullify(m_child_to_detach_and_delete);
}

} // end of namespace Xrb
