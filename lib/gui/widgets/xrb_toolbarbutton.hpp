// ///////////////////////////////////////////////////////////////////////////
// xrb_toolbarbutton.hpp by Victor Dods, created 2005/02/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_TOOLBARBUTTON_HPP_)
#define _XRB_TOOLBARBUTTON_HPP_

#include "xrb.hpp"

#include "xrb_radiobutton.hpp"

namespace Xrb
{

template <typename T, T sentinel>
class ToolbarButton : public RadioButton<T, sentinel>
{
public:

    ToolbarButton (
        Resource<GLTexture> const &picture,
        T id,
        RadioButtonGroup<T, sentinel> *group,
        ContainerWidget *parent,
        std::string const &name = "ToolbarButton");
    virtual ~ToolbarButton () { }

protected:

    virtual void HandleMouseoverOn ();
    virtual void HandleMouseoverOff ();

    virtual void UpdateRenderBackground ();
    virtual void UpdateRenderPicture ();

    // WidgetSkinHandler overrides
    virtual void HandleChangedWidgetSkinWidgetBackground (
        WidgetSkin::WidgetBackgroundType widget_background_type);
}; // end of class ToolbarButton

// function definitions for ToolbarButton
#include "xrb_toolbarbutton.tcpp"

} // end of namespace Xrb

#endif // !defined(_XRB_TOOLBARBUTTON_HPP_)
