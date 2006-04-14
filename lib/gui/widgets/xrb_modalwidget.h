// ///////////////////////////////////////////////////////////////////////////
// xrb_modalwidget.h by Victor Dods, created 2005/02/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_MODALWIDGET_H_)
#define _XRB_MODALWIDGET_H_

#include "xrb.h"

#include "xrb_widget.h"

namespace Xrb
{

/** This baseclass isn't strictly necessary to use modal widgets -- it just
  * provides a bit of functionality to make things more convenient.
  *
  * The Shutdown method is provided for ModalWidgets to delete themselves.
  * This is accomplished by sending an asynchronous delete child widget event
  * to its parent.
  *
  * The ModalWidget class has a default widget background, provided by
  * WidgetSkin.  There are facilities to handle the necessary updating
  * automatically (necessary only if you want the ModalWidget background
  * provided by WidgetSkin).
  *
  * @brief Baseclass for modal widgets.
  */
class ModalWidget : public Widget
{
public:

    /** @brief Constructs a modal widget in the same manner as a Widget.
      */
    ModalWidget (
        Widget *parent,
        std::string const &name = "ModalWidget");
    /** @brief Boring destructor.
      */
    virtual ~ModalWidget () { }

    /** The widget makes itself not modal, and blocks all events.
      * @brief Schedules this widget for deletion.
      */
    void Shutdown ();

protected:

    /** @brief Sets the render background to the WidgetSkin's
      *        ModalWidget background.
      */
    virtual void UpdateRenderBackground ();
    /** Simply calls UpdateRenderBackground.
      * @brief Called by ModalWidget when specifically the ModalWidget
      *        background changes.
      */
    virtual void HandleChangedModalWidgetBackground ();
    /** If the changed background type is MODAL_WIDGET_BACKGROUND,
      * HandleChangedModalWidgetBackground is called.
      * @brief Used to catch when the modal widget background is changed.
      */
    virtual void HandleChangedWidgetSkinWidgetBackground (
        WidgetSkin::WidgetBackgroundType widget_background_type);
}; // end of class ModalWidget

} // end of namespace Xrb

#endif // !defined(_XRB_MODALWIDGET_H_)

