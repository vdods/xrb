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

#include "xrb_widget.hpp"

namespace Xrb
{

void EventDeleteChildWidget::DeleteChildWidget () const
{
    ASSERT1(m_child_to_delete != NULL);
    DeleteAndNullify(m_child_to_delete);
}

} // end of namespace Xrb
