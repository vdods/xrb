// ///////////////////////////////////////////////////////////////////////////
// dis_optionspanel.h by Victor Dods, created 2006/02/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_OPTIONSPANEL_H_)
#define _DIS_OPTIONSPANEL_H_

#include "xrb_containerwidget.h"

#include "dis_config.h"
#include "xrb_validator.h"

using namespace Xrb;

namespace Xrb
{
class CheckBox;
template <typename T> class ValueEdit;
} // end of namespace Xrb

namespace Dis
{

/*
video:
- screen resolution X/Y
- fullscreen
audio:
- none yet
input:
- forward
- left
- back
- right
- primary weapon fire
- secondary weapon fire
- tractor mode
- engine brake
*/

class OptionsPanel : public ContainerWidget
{
public:

    OptionsPanel (ContainerWidget *parent);
    virtual ~OptionsPanel ();

    ScreenCoordVector2 GetResolution () const;
    bool GetFullscreen () const;
//     std::string const &GetInputActionKeyName (Config::InputAction input_action) const;

    void SetResolutionX (ScreenCoord resolution_x);
    void SetResolutionY (ScreenCoord resolution_y);
    void SetFullscreen (bool fullscreen);

private:

    GreaterThanValidator<ScreenCoord> m_greater_than_zero_validator;

    ValueEdit<ScreenCoord> *m_resolution_x_edit;
    ValueEdit<ScreenCoord> *m_resolution_y_edit;
    CheckBox *m_fullscreen_checkbox;
}; // end of class OptionsPanel

} // end of namespace Dis

#endif // !defined(_DIS_OPTIONSPANEL_H_)

