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
    m_greater_than_zero_validator(0, 1),
    m_internal_sender_resolution_x_changed(this),
    m_internal_sender_resolution_y_changed(this),
    m_internal_sender_fullscreen_changed(this),
    m_internal_receiver_set_resolution_x(&OptionsPanel::SetResolutionX, this),
    m_internal_receiver_set_resolution_y(&OptionsPanel::SetResolutionY, this),
    m_internal_receiver_set_fullscreen(&OptionsPanel::SetFullscreen, this)
{
    Label *l = NULL;

    Layout *main_layout = new Layout(HORIZONTAL, this);
    SetMainWidget(main_layout);

    Layout *left_side_layout = new Layout(VERTICAL, main_layout);

    {
        new Label("Video Options", left_side_layout);
        Layout *video_options_layout = new Layout(ROW, 2, left_side_layout);

        l = new Label("Horizontal Resolution:", video_options_layout);
        l->SetAlignment(Dim::X, RIGHT);
        ValueEdit<ScreenCoord> *resolution_x_edit = new ValueEdit<ScreenCoord>("%d", Util::TextToSint32, video_options_layout);
        resolution_x_edit->SetValidator(&m_greater_than_zero_validator);
        SignalHandler::Connect1(
            &m_internal_sender_resolution_x_changed,
            resolution_x_edit->ReceiverSetValue());
        SignalHandler::Connect1(
            resolution_x_edit->SenderValueUpdated(),
            &m_internal_receiver_set_resolution_x);

        l = new Label("Vertical Resolution:", video_options_layout);
        l->SetAlignment(Dim::X, RIGHT);
        ValueEdit<ScreenCoord> *resolution_y_edit = new ValueEdit<ScreenCoord>("%d", Util::TextToSint32, video_options_layout);
        resolution_y_edit->SetValidator(&m_greater_than_zero_validator);
        SignalHandler::Connect1(
            &m_internal_sender_resolution_y_changed,
            resolution_y_edit->ReceiverSetValue());
        SignalHandler::Connect1(
            resolution_y_edit->SenderValueUpdated(),
            &m_internal_receiver_set_resolution_y);

        l = new Label("Fullscreen:", video_options_layout);
        l->SetAlignment(Dim::X, RIGHT);
        CheckBox *fullscreen_checkbox = new CheckBox(video_options_layout);
        SignalHandler::Connect1(
            &m_internal_sender_fullscreen_changed,
            fullscreen_checkbox->ReceiverSetIsChecked());
        SignalHandler::Connect1(
            fullscreen_checkbox->SenderCheckedStateChanged(),
            &m_internal_receiver_set_fullscreen);
    }

    {
        new Label("Audio Options", left_side_layout);
    }

    Layout *right_side_layout = new Layout(VERTICAL, main_layout);

    {
        new Label("Input Options", right_side_layout);
//         Layout *input_options_layout = new Layout(ROW, 2, right_side_layout);
    }
}

OptionsPanel::~OptionsPanel ()
{
}

void OptionsPanel::SetResolutionX (ScreenCoord const resolution_x)
{
    if (m_resolution[Dim::X] != resolution_x)
    {
        m_resolution[Dim::X] = resolution_x;
        m_internal_sender_resolution_x_changed.Signal(m_resolution[Dim::X]);
    }
}

void OptionsPanel::SetResolutionY (ScreenCoord const resolution_y)
{
    if (m_resolution[Dim::Y] != resolution_y)
    {
        m_resolution[Dim::Y] = resolution_y;
        m_internal_sender_resolution_y_changed.Signal(m_resolution[Dim::Y]);
    }
}

void OptionsPanel::SetFullscreen (bool const fullscreen)
{
    if (m_fullscreen != fullscreen)
    {
        m_fullscreen = fullscreen;
        m_internal_sender_fullscreen_changed.Signal(m_fullscreen);
    }
}

} // end of namespace Dis

