// ///////////////////////////////////////////////////////////////////////////
// xrb_modalwidget.hpp by Victor Dods, created 2005/02/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_MODALWIDGET_HPP_)
#define _XRB_MODALWIDGET_HPP_

#include "xrb.hpp"

#include "xrb_containerwidget.hpp"

namespace Xrb {

/** This baseclass isn't strictly necessary to use modal widgets -- it just
  * provides a bit of functionality to make things more convenient.
  *
  * The Shutdown method is provided for ModalWidgets to delete themselves.
  * This is accomplished by sending an asynchronous delete child widget event
  * to its parent.
  *
  * The ModalWidget class has a default widget background, provided by
  * StyleSheet.  There are facilities to handle the necessary updating
  * automatically (necessary only if you want the ModalWidget background
  * provided by StyleSheet).
  *
  * @brief Baseclass for modal widgets.
  */
class ModalWidget : public ContainerWidget
{
public:

    /// Constructs a modal widget in the same manner as a Widget.
    ModalWidget (WidgetContext &context, std::string const &name = "ModalWidget");
    virtual ~ModalWidget () { }

    /// The widget makes itself not modal, and blocks all events.
    /// @brief Schedules this widget for deletion.
    void Shutdown ();

protected:

    /// Sets focus
    virtual void HandleActivate ();
}; // end of class ModalWidget

} // end of namespace Xrb

#endif // !defined(_XRB_MODALWIDGET_HPP_)

