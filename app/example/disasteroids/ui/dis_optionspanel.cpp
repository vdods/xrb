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

#include "dis_config.h"
#include "xrb_cellpaddingwidget.h"
#include "xrb_checkbox.h"
#include "xrb_keyselectorbutton.h"
#include "xrb_label.h"
#include "xrb_layout.h"
#include "xrb_spacerwidget.h"
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

    // possibly temporary -- to space out the left side nicely
    new SpacerWidget(left_side_layout);

    {
        l = new Label("Video Options", left_side_layout);
        l->SetIsHeightFixedToTextHeight(true);
        Layout *video_options_layout = new Layout(ROW, 2, left_side_layout);

        l = new Label("Horizontal Resolution:", video_options_layout);
        l->SetAlignment(Dim::X, RIGHT);
        m_resolution_x_edit = new ValueEdit<ScreenCoord>("%d", Util::TextToSint32, video_options_layout);
        m_resolution_x_edit->SetValidator(&m_greater_than_zero_validator);
        m_resolution_x_edit->SetSizePropertyEnabled(SizeProperties::MIN, Dim::X, true);
        m_resolution_x_edit->SetSizeProperty(SizeProperties::MIN, Dim::X, 4*m_resolution_x_edit->GetFont()->GetPixelHeight());

        l = new Label("Vertical Resolution:", video_options_layout);
        l->SetAlignment(Dim::X, RIGHT);
        m_resolution_y_edit = new ValueEdit<ScreenCoord>("%d", Util::TextToSint32, video_options_layout);
        m_resolution_y_edit->SetValidator(&m_greater_than_zero_validator);
        m_resolution_y_edit->SetSizePropertyEnabled(SizeProperties::MIN, Dim::X, true);
        m_resolution_y_edit->SetSizeProperty(SizeProperties::MIN, Dim::X, 4*m_resolution_y_edit->GetFont()->GetPixelHeight());

        l = new Label("Fullscreen:", video_options_layout);
        l->SetIsHeightFixedToTextHeight(true);
        l->SetAlignment(Dim::X, RIGHT);
        CellPaddingWidget *cpw = new CellPaddingWidget(video_options_layout);
        cpw->SetAlignment(Dim::X, LEFT);
        m_fullscreen_checkbox = new CheckBox(cpw);

        l = new Label("Changes to the video options will take effect after restarting.", left_side_layout);
        l->SetIsHeightFixedToTextHeight(true);
        l->SetWordWrap(true);
    }
/*
    {
        l = new Label("Audio Options", left_side_layout);
        l->SetIsHeightFixedToTextHeight(true);
    }
*/

    // possibly temporary -- to space out the left side nicely
    new SpacerWidget(left_side_layout);

    {
        l = new Label("Game Difficulty", left_side_layout);
        l->SetIsHeightFixedToTextHeight(true);
        Layout *game_difficulty_layout = new Layout(ROW, 2, left_side_layout);

        for (Uint32 i = 0; i < DL_COUNT; ++i)
        {
            l = new Label(GetDifficultyLevelString(static_cast<DifficultyLevel>(i)), game_difficulty_layout);
            l->SetIsHeightFixedToTextHeight(true);
            l->SetAlignment(Dim::X, RIGHT);

            CellPaddingWidget *cpw = new CellPaddingWidget(game_difficulty_layout);
            cpw->SetAlignment(Dim::X, LEFT);
            new RadioButton<DifficultyLevel, DL_COUNT>(
                static_cast<DifficultyLevel>(i),
                &m_difficulty_level,
                cpw);
        }
    }

    // possibly temporary -- to space out the left side nicely
    new SpacerWidget(left_side_layout);

    Layout *right_side_layout = new Layout(VERTICAL, main_layout);

    {
        l = new Label("Input Options", right_side_layout);
        l->SetIsHeightFixedToTextHeight(true);
        Layout *input_options_layout = new Layout(ROW, 2, right_side_layout);

        for (Uint32 i = 0; i < Config::IA_COUNT; ++i)
        {
            l = new Label(Config::ms_input_action_name[i], input_options_layout);
            l->SetAlignment(Dim::X, RIGHT);
            m_input_action_button[i] = new KeySelectorButton( Config::ms_input_action_name[i], Key::INVALID, input_options_layout);
            m_input_action_button[i]->SetIsHeightFixedToTextHeight(true);
        }
    }
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

DifficultyLevel OptionsPanel::GetDifficultyLevel () const
{
    ASSERT1(m_difficulty_level.GetID() >= DL_LOWEST)
    ASSERT1(m_difficulty_level.GetID() <= DL_HIGHEST)
    return m_difficulty_level.GetID();
}

Key::Code OptionsPanel::GetInputActionKeyCode (Config::InputAction const input_action) const
{
    ASSERT1(input_action < Config::IA_COUNT)
    ASSERT1(m_input_action_button[input_action] != NULL)
    return m_input_action_button[input_action]->GetKeyCode();
}

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

void OptionsPanel::SetDifficultyLevel (DifficultyLevel const difficulty_level)
{
    ASSERT1(difficulty_level >= DL_LOWEST)
    ASSERT1(difficulty_level <= DL_HIGHEST)
    m_difficulty_level.SetID(difficulty_level);
}

void OptionsPanel::SetInputActionKeyCode (
    Config::InputAction const input_action,
    Key::Code const key_code)
{
    ASSERT1(input_action < Config::IA_COUNT)
    ASSERT1(m_input_action_button[input_action] != NULL)
    m_input_action_button[input_action]->SetKeyCode(key_code);
}

void OptionsPanel::ReadValuesFromConfig (Config const &config)
{
    SetResolutionX(config.GetResolution()[Dim::X]);
    SetResolutionY(config.GetResolution()[Dim::Y]);
    SetFullscreen(config.GetFullscreen());
    SetDifficultyLevel(config.GetDifficultyLevel());

    for (Uint32 i = 0; i < Config::IA_COUNT; ++i)
        SetInputActionKeyCode(
            static_cast<Config::InputAction>(i),
            config.GetInputActionKeyCode(static_cast<Config::InputAction>(i)));
}

void OptionsPanel::WriteValuesToConfig (Config *const config)
{
    ASSERT1(config != NULL)

    config->SetResolutionX(GetResolution()[Dim::X]);
    config->SetResolutionY(GetResolution()[Dim::Y]);
    config->SetFullscreen(GetFullscreen());
    config->SetDifficultyLevel(GetDifficultyLevel());

    for (Uint32 i = 0; i < Config::IA_COUNT; ++i)
        config->SetInputActionKeyCode(
            static_cast<Config::InputAction>(i),
            GetInputActionKeyCode(static_cast<Config::InputAction>(i)));
}

} // end of namespace Dis

