// ///////////////////////////////////////////////////////////////////////////
// dis_optionspanel.hpp by Victor Dods, created 2006/02/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_OPTIONSPANEL_HPP_)
#define _DIS_OPTIONSPANEL_HPP_

#include "xrb_containerwidget.hpp"

#include "dis_config.hpp"
#include "xrb_radiobutton.hpp"
#include "xrb_validator.hpp"

using namespace Xrb;

namespace Xrb
{
class CheckBox;
class Config;
class KeySelectorButton;
template <typename T> class ValueEdit;
} // end of namespace Xrb

namespace Dis
{

class OptionsPanel : public ContainerWidget
{
public:

    OptionsPanel (ContainerWidget *parent);
    virtual ~OptionsPanel () { }

    ScreenCoordVector2 GetResolution () const;
    bool GetFullscreen () const;
    DifficultyLevel GetDifficultyLevel () const;
    Key::Code GetInputActionKeyCode (KeyInputAction input_action) const;

    void SetResolutionX (ScreenCoord resolution_x);
    void SetResolutionY (ScreenCoord resolution_y);
    void SetFullscreen (bool fullscreen);
    void SetDifficultyLevel (DifficultyLevel difficulty_level);
    void SetInputActionKeyCode (KeyInputAction input_action, Key::Code key_code);

    void ReadValuesFromConfig (Config const &config);
    void WriteValuesToConfig (Config *config);

private:

    GreaterThanValidator<ScreenCoord> m_greater_than_zero_validator;

    ValueEdit<ScreenCoord> *m_resolution_x_edit;
    ValueEdit<ScreenCoord> *m_resolution_y_edit;
    CheckBox *m_fullscreen_checkbox;
    RadioButtonGroup<DifficultyLevel, DL_COUNT> m_difficulty_level;
    KeySelectorButton *m_input_action_button[KEY_INPUT_ACTION_COUNT];
}; // end of class OptionsPanel

} // end of namespace Dis

#endif // !defined(_DIS_OPTIONSPANEL_HPP_)

