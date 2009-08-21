// ///////////////////////////////////////////////////////////////////////////
// xrb_filepanel.cpp by Victor Dods, created 2005/05/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_filepanel.hpp"

#include "xrb_label.hpp"
#include "xrb_layout.hpp"
#include "xrb_lineedit.hpp"
#include "xrb_widgetbackground.hpp"

namespace Xrb
{

FilePanel::FilePanel (
    std::string const &title_text,
    Operation const file_operation,
    ContainerWidget *const parent,
    std::string const &name)
    :
    ContainerWidget(parent, name),
    m_sender_submit_path(this),
    m_sender_submit_path_v(this),
    m_internal_receiver_path_set_by_enter_key(
        &FilePanel::InternalPathSetByEnterKey, this)
{
    Layout *main_control_layout =
        new Layout(
            VERTICAL,
            this,
            "main control layout");

    Label *label;

    // the title text label
    label = m_title_label =
        new Label(
            title_text,
            main_control_layout,
            "title label");
    label->SetIsHeightFixedToTextHeight(true);

    // the layout for the dialog's controls
    Layout *sub_control_layout =
        new Layout(
            HORIZONTAL,
            main_control_layout,
            "sub control layout");
    // create a text label to indicate what the lineedit is for
    m_file_operation = file_operation;
    label = new Label("Path:", sub_control_layout);
    label->SetIsWidthFixedToTextWidth(true);
    // create the line edit to type the path into
    m_path_edit =
        new LineEdit(
            LONGEST_FILESYSTEM_PATH_NAME,
            sub_control_layout,
            "path edit");
    m_path_edit->Focus();

    SetMainWidget(main_control_layout);

    // connect the path edit's enter key signal to this file panel's
    // path set by enter key receiver
    SignalHandler::Connect1(
        m_path_edit->SenderTextSetByEnterKey(),
        &m_internal_receiver_path_set_by_enter_key);

    FilePanel::UpdateRenderBackground();
}

std::string const &FilePanel::Path () const
{
    ASSERT1(m_path_edit != NULL);
    return m_path_edit->Text();
}

void FilePanel::UpdateRenderBackground ()
{
    SetRenderBackground(NULL);
}

void FilePanel::InternalPathSetByEnterKey (std::string const &path)
{
    m_sender_submit_path.Signal(path);
    m_sender_submit_path_v.Signal(path);
}

} // end of namespace Xrb
