// ///////////////////////////////////////////////////////////////////////////
// dis_controlspanel.cpp by Victor Dods, created 2006/02/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_controlspanel.hpp"

#include "dis_config.hpp"
#include "xrb_cellpaddingwidget.hpp"
#include "xrb_checkbox.hpp"
#include "xrb_keyselectorbutton.hpp"
#include "xrb_label.hpp"
#include "xrb_layout.hpp"
#include "xrb_spacerwidget.hpp"
#include "xrb_valueedit.hpp"

using namespace Xrb;

namespace Dis {

ControlsPanel::ControlsPanel (ContainerWidget *parent)
    :
    ContainerWidget(parent, "ControlsPanel"),
    m_greater_than_zero_validator(0, 1)
{
    Label *l = NULL;

    Layout *main_layout = new Layout(HORIZONTAL, this);
    SetMainWidget(main_layout);

    Layout *left_side_layout = new Layout(VERTICAL, main_layout);

    new SpacerWidget(left_side_layout);

    {
        l = new Label("Video Controls", left_side_layout);
        l->SetIsHeightFixedToTextHeight(true);
        Layout *video_controls_layout = new Layout(ROW, 2, left_side_layout);

        l = new Label("Horizontal Resolution:", video_controls_layout);
        l->SetAlignment(Dim::X, RIGHT);
        m_resolution_x_edit = new ValueEdit<ScreenCoord>("%d", Util::TextToSint<ScreenCoord>, video_controls_layout, "resolution x ValueEdit");
        m_resolution_x_edit->SetValidator(&m_greater_than_zero_validator);
//         m_resolution_x_edit->SetIsMinWidthFixedToTextWidth(true);
        m_resolution_x_edit->SetSizeProperty(SizeProperties::MIN, Dim::X, 6*m_resolution_x_edit->GetFont()->PixelHeight());
        m_resolution_x_edit->SetSizePropertyEnabled(SizeProperties::MIN, Dim::X, true);

        l = new Label("Vertical Resolution:", video_controls_layout);
        l->SetAlignment(Dim::X, RIGHT);
        m_resolution_y_edit = new ValueEdit<ScreenCoord>("%d", Util::TextToSint<ScreenCoord>, video_controls_layout, "resolution y ValueEdit");
        m_resolution_y_edit->SetValidator(&m_greater_than_zero_validator);
//         m_resolution_y_edit->SetIsMinWidthFixedToTextWidth(true);
        m_resolution_y_edit->SetSizeProperty(SizeProperties::MIN, Dim::X, 6*m_resolution_y_edit->GetFont()->PixelHeight());
        m_resolution_y_edit->SetSizePropertyEnabled(SizeProperties::MIN, Dim::X, true);

        l = new Label("Fullscreen:", video_controls_layout);
        l->SetIsHeightFixedToTextHeight(true);
        l->SetAlignment(Dim::X, RIGHT);
        CellPaddingWidget *cpw = new CellPaddingWidget(video_controls_layout);
        cpw->SetAlignment(Dim::X, LEFT);
        m_fullscreen_checkbox = new CheckBox(cpw);

        l = new Label("Changes to the video controls will take effect after restarting.", left_side_layout);
        l->SetIsHeightFixedToTextHeight(true);
        l->SetWordWrap(true);
    }
/*
    {
        l = new Label("Audio Controls", left_side_layout);
        l->SetIsHeightFixedToTextHeight(true);
    }
*/

    new SpacerWidget(left_side_layout);

    {
        l = new Label("Game Difficulty", left_side_layout);
        l->SetIsHeightFixedToTextHeight(true);
        Layout *game_difficulty_layout = new Layout(ROW, 2, left_side_layout);

        for (Uint32 i = 0; i < DL_COUNT; ++i)
        {
            l = new Label(DifficultyLevelString(static_cast<DifficultyLevel>(i)), game_difficulty_layout);
            l->SetIsHeightFixedToTextHeight(true);
            l->SetAlignment(Dim::X, RIGHT);

            CellPaddingWidget *cpw = new CellPaddingWidget(game_difficulty_layout);
            cpw->SetAlignment(Dim::X, LEFT);
            new RadioButton<DifficultyLevel, DL_COUNT>(
                static_cast<DifficultyLevel>(i),
                &m_difficulty_level,
                cpw);
        }

        l = new Label("Changes to the game difficulty will take effect at the beginning of each new game.", left_side_layout);
        l->SetIsHeightFixedToTextHeight(true);
        l->SetWordWrap(true);
    }

    new SpacerWidget(left_side_layout);

    // input controls
    {
        Layout *right_side_layout = new Layout(VERTICAL, main_layout);

        new SpacerWidget(right_side_layout);

        l = new Label("Input Controls", right_side_layout);
        l->SetIsHeightFixedToTextHeight(true);

        Layout *two_columns_layout = new Layout(HORIZONTAL, right_side_layout);

        // left column
        Layout *input_controls_layout = new Layout(ROW, 2, two_columns_layout);
        for (Uint32 i = 0; i < KEY_INPUT_ACTION_COUNT / 2; ++i)
//         for (Uint32 i = 0; i < KEY_INPUT_ACTION_COUNT; ++i)
        {
            l = new Label(Config::ms_input_action_label[i], input_controls_layout);
            l->SetAlignment(Dim::X, RIGHT);
            m_input_action_button[i] = new KeySelectorButton(Config::ms_input_action_label[i], Key::INVALID, input_controls_layout);
            m_input_action_button[i]->SetIsHeightFixedToTextHeight(true);
            m_input_action_button[i]->SetIsMinWidthFixedToTextWidth(true);
        }
        // right column
        input_controls_layout = new Layout(ROW, 2, two_columns_layout);
        for (Uint32 i = KEY_INPUT_ACTION_COUNT / 2; i < KEY_INPUT_ACTION_COUNT; ++i)
        {
            l = new Label(Config::ms_input_action_label[i], input_controls_layout);
            l->SetAlignment(Dim::X, RIGHT);
            m_input_action_button[i] = new KeySelectorButton(Config::ms_input_action_label[i], Key::INVALID, input_controls_layout);
            m_input_action_button[i]->SetIsHeightFixedToTextHeight(true);
            m_input_action_button[i]->SetIsMinWidthFixedToTextWidth(true);
        }

        new SpacerWidget(right_side_layout);
    }
}

ScreenCoordVector2 ControlsPanel::Resolution () const
{
    ASSERT1(m_resolution_x_edit != NULL);
    ASSERT1(m_resolution_y_edit != NULL);
    return ScreenCoordVector2(m_resolution_x_edit->Value(), m_resolution_y_edit->Value());
}

bool ControlsPanel::Fullscreen () const
{
    ASSERT1(m_fullscreen_checkbox != NULL);
    return m_fullscreen_checkbox->IsChecked();
}

DifficultyLevel ControlsPanel::GetDifficultyLevel () const
{
    ASSERT1(m_difficulty_level.ID() >= DL_LOWEST);
    ASSERT1(m_difficulty_level.ID() <= DL_HIGHEST);
    return m_difficulty_level.ID();
}

Key::Code ControlsPanel::InputActionKeyCode (KeyInputAction const input_action) const
{
    ASSERT1(input_action < KEY_INPUT_ACTION_COUNT);
    ASSERT1(m_input_action_button[input_action] != NULL);
    return m_input_action_button[input_action]->KeyCode();
}

void ControlsPanel::SetResolutionX (ScreenCoord const resolution_x)
{
    ASSERT1(m_resolution_x_edit != NULL);
    m_resolution_x_edit->SetValue(resolution_x);
}

void ControlsPanel::SetResolutionY (ScreenCoord const resolution_y)
{
    ASSERT1(m_resolution_y_edit != NULL);
    m_resolution_y_edit->SetValue(resolution_y);
}

void ControlsPanel::SetFullscreen (bool const fullscreen)
{
    ASSERT1(m_fullscreen_checkbox != NULL);
    m_fullscreen_checkbox->SetIsChecked(fullscreen);
}

void ControlsPanel::SetDifficultyLevel (DifficultyLevel const difficulty_level)
{
    ASSERT1(difficulty_level >= DL_LOWEST);
    ASSERT1(difficulty_level <= DL_HIGHEST);
    m_difficulty_level.SetID(difficulty_level);
}

void ControlsPanel::SetInputActionKeyCode (
    KeyInputAction const input_action,
    Key::Code const key_code)
{
    ASSERT1(input_action < KEY_INPUT_ACTION_COUNT);
    ASSERT1(m_input_action_button[input_action] != NULL);
    m_input_action_button[input_action]->SetKeyCode(key_code);
}

void ControlsPanel::ReadValuesFromConfig (Config const &config)
{
    SetResolutionX(config.ResolutionX());
    SetResolutionY(config.ResolutionY());
    SetFullscreen(config.Boolean(VIDEO__FULLSCREEN));
    SetDifficultyLevel(config.GetDifficultyLevel());

    for (Uint32 i = 0; i < KEY_INPUT_ACTION_COUNT; ++i)
        SetInputActionKeyCode(
            static_cast<KeyInputAction>(i),
            config.InputAction(static_cast<KeyInputAction>(i)));
}

void ControlsPanel::WriteValuesToConfig (Config *const config)
{
    ASSERT1(config != NULL);

    config->SetResolutionX(Resolution()[Dim::X]);
    config->SetResolutionY(Resolution()[Dim::Y]);
    config->SetBoolean(VIDEO__FULLSCREEN, Fullscreen());
    config->SetDifficultyLevel(GetDifficultyLevel());

    for (Uint32 i = 0; i < KEY_INPUT_ACTION_COUNT; ++i)
        config->SetInputAction(
            static_cast<KeyInputAction>(i),
            InputActionKeyCode(static_cast<KeyInputAction>(i)));
}

} // end of namespace Dis

