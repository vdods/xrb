// ///////////////////////////////////////////////////////////////////////////
// dis_optionspanel.cpp by Victor Dods, created 2006/02/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_optionspanel.h"

#include "xrb_checkbox.h"
#include "xrb_label.h"
#include "xrb_layout.h"
#include "xrb_valueedit.h"

using namespace Xrb;

namespace Dis
{

OptionsPanel::OptionsPanel (ContainerWidget *const parent)
    :
    ContainerWidget(parent, "OptionsPanel"),
    m_greater_than_zero_validator(0, 1)
{
    Label *l = NULL;

    Layout *main_layout = new Layout(HORIZONTAL, this);
    SetMainWidget(main_layout);

    Layout *left_side_layout = new Layout(VERTICAL, main_layout);

    {
        l = new Label("Video Options", left_side_layout);
        l->SetIsHeightFixedToTextHeight(true);
        Layout *video_options_layout = new Layout(ROW, 2, left_side_layout);

        l = new Label("Horizontal Resolution:", video_options_layout);
        l->SetAlignment(Dim::X, RIGHT);
        m_resolution_x_edit = new ValueEdit<ScreenCoord>("%d", Util::TextToSint32, video_options_layout);
        m_resolution_x_edit->SetValidator(&m_greater_than_zero_validator);

        l = new Label("Vertical Resolution:", video_options_layout);
        l->SetAlignment(Dim::X, RIGHT);
        m_resolution_y_edit = new ValueEdit<ScreenCoord>("%d", Util::TextToSint32, video_options_layout);
        m_resolution_y_edit->SetValidator(&m_greater_than_zero_validator);

        l = new Label("Fullscreen:", video_options_layout);
        l->SetAlignment(Dim::X, RIGHT);
        m_fullscreen_checkbox = new CheckBox(video_options_layout);

        l = new Label("Changes to the video options will take effect after restarting.", left_side_layout);
        l->SetIsHeightFixedToTextHeight(true);
        l->SetWordWrap(true);
    }
/*
    {
        new Label("Audio Options", left_side_layout);
    }
*/
    Layout *right_side_layout = new Layout(VERTICAL, main_layout);

    {
        l = new Label("Input Options", right_side_layout);
        l->SetIsHeightFixedToTextHeight(true);
        Layout *input_options_layout = new Layout(ROW, 2, right_side_layout);

        l = new Label("Move Forward:", input_options_layout);
        l->SetAlignment(Dim::X, RIGHT);
//         LineEdit *move_forward_edit = new LineEdit(30, input_options_layout);

    }
}

OptionsPanel::~OptionsPanel ()
{
}

ScreenCoordVector2 OptionsPanel::GetResolution () const
{
    ASSERT1(m_resolution_x_edit != NULL)
    ASSERT1(m_resolution_y_edit != NULL)
    return ScreenCoordVector2(m_resolution_x_edit->GetValue(), m_resolution_y_edit->GetValue());
}

bool OptionsPanel::GetFullscreen () const
{
    ASSERT1(m_fullscreen_checkbox != NULL)
    return m_fullscreen_checkbox->GetIsChecked();
}

// std::string const &OptionsPanel::GetInputActionKeyName (Config::InputAction input_action) const
// {
// }

void OptionsPanel::SetResolutionX (ScreenCoord const resolution_x)
{
    ASSERT1(m_resolution_x_edit != NULL)
    m_resolution_x_edit->SetValue(resolution_x);
}

void OptionsPanel::SetResolutionY (ScreenCoord const resolution_y)
{
    ASSERT1(m_resolution_y_edit != NULL)
    m_resolution_y_edit->SetValue(resolution_y);
}

void OptionsPanel::SetFullscreen (bool const fullscreen)
{
    ASSERT1(m_fullscreen_checkbox != NULL)
    m_fullscreen_checkbox->SetIsChecked(fullscreen);
}

// void OptionsPanel::SetInputActionKeyName (Config::InputAction input_action, std::string const &key_name);

} // end of namespace Dis

