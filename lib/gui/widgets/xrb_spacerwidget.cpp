// ///////////////////////////////////////////////////////////////////////////
// xrb_spacerwidget.cpp by Victor Dods, created 2005/04/16
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_spacerwidget.hpp"

namespace Xrb {

SpacerWidget::SpacerWidget (WidgetContext &context)
    :
    Widget(context, "SpacerWidget")
{
    SetBackgroundStyle(StyleSheet::BackgroundType::TRANSPARENT);
}

} // end of namespace Xrb
