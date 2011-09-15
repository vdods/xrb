// ///////////////////////////////////////////////////////////////////////////
// xrb_spacerwidget.hpp by Victor Dods, created 2005/04/16
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_SPACERWIDGET_HPP_)
#define _XRB_SPACERWIDGET_HPP_

#include "xrb.hpp"

#include "xrb_widget.hpp"

namespace Xrb {

/** This lovely little widget does absolutely nothing.  It is here just so
  * layouts can be set up with gaps in certain slots, where necessary.
  * @brief A blank widget to be placed in layouts where one needs a gap.
  */
class SpacerWidget : public Widget
{
public:

    /** @brief Constructs a SpacerWidget, attaching it to parent.
      */
    SpacerWidget ();
    /** @brief Boring destructor.  Does nothing.
      */
    virtual ~SpacerWidget () { }
}; // end of class SpacerWidget

} // end of namespace Xrb

#endif // !defined(_XRB_SPACERWIDGET_HPP_)
