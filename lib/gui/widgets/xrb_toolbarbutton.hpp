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

namespace Xrb {

template <typename T, T sentinel>
class ToolbarButton : public RadioButton<T, sentinel>
{
public:

    ToolbarButton (Resource<GlTexture> const &picture, T id, RadioButtonGroup<T, sentinel> *group, std::string const &name = "ToolbarButton");
    virtual ~ToolbarButton () { }

protected:

    virtual void HandleMouseoverOn ();
    virtual void HandleMouseoverOff ();

    virtual void UpdateRenderBackground ();
    virtual void UpdateRenderPicture ();

    // WidgetSkinHandler overrides
    virtual void HandleChangedWidgetSkin ();
}; // end of class ToolbarButton

// NOTE: the seemingly redundant "this->" is necessary because the C++
// standard specifies non-intuitive behavior for template definitions.
// see http://gcc.gnu.org/onlinedocs/gcc/Name-lookup.html for more info.

template <typename T, T sentinel>
ToolbarButton<T, sentinel>::ToolbarButton (Resource<GlTexture> const &picture, T id, RadioButtonGroup<T, sentinel> *group, std::string const &name)
    :
    RadioButton<T, sentinel>(picture, id, group, parent, name)
{
    ASSERT1(this->Picture().IsValid());
    ToolbarButton<T, sentinel>::UpdateRenderBackground();
    ToolbarButton<T, sentinel>::UpdateRenderPicture();
}

template <typename T, T sentinel>
void ToolbarButton<T, sentinel>::HandleMouseoverOn ()
{
    ASSERT1(this->IsMouseover());
    Button::HandleMouseoverOn();
    UpdateRenderBackground();
}

template <typename T, T sentinel>
void ToolbarButton<T, sentinel>::HandleMouseoverOff ()
{
    ASSERT1(!this->IsMouseover());
    Button::HandleMouseoverOff();
    UpdateRenderBackground();
}

template <typename T, T sentinel>
void ToolbarButton<T, sentinel>::UpdateRenderBackground ()
{
    // state priority: disabled, pressed, mouseover, checked, default
    if (!this->IsEnabled())
        SetRenderBackground(this->WidgetSkinWidgetBackground(WidgetSkin::TOOLBAR_BUTTON_BACKGROUND));
    else if (this->IsPressed())
        SetRenderBackground(this->WidgetSkinWidgetBackground(WidgetSkin::TOOLBAR_BUTTON_PRESSED_BACKGROUND));
    else if (this->IsMouseover() && this->AcceptsMouseover())
        SetRenderBackground(this->WidgetSkinWidgetBackground(WidgetSkin::TOOLBAR_BUTTON_MOUSEOVER_BACKGROUND));
    else if (this->IsChecked())
        SetRenderBackground(this->WidgetSkinWidgetBackground(WidgetSkin::TOOLBAR_BUTTON_CHECKED_BACKGROUND));
    else
        SetRenderBackground(this->WidgetSkinWidgetBackground(WidgetSkin::TOOLBAR_BUTTON_BACKGROUND));
}

template <typename T, T sentinel>
void ToolbarButton<T, sentinel>::UpdateRenderPicture ()
{
    SetRenderPicture(this->Picture());
}

template <typename T, T sentinel>
void ToolbarButton<T, sentinel>::HandleChangedWidgetSkin ()
{
    RadioButton<T, sentinel>::HandleChangedWidgetSkin();
    UpdateRenderBackground();
}

} // end of namespace Xrb

#endif // !defined(_XRB_TOOLBARBUTTON_HPP_)
