// ///////////////////////////////////////////////////////////////////////////
// xrb_filepanel.cpp by Victor Dods, created 2005/05/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_filepanel.h"

#include "xrb_label.h"
#include "xrb_layout.h"
#include "xrb_lineedit.h"
#include "xrb_widgetbackground.h"

namespace Xrb
{

FilePanel::FilePanel (
    std::string const &title_text,
    Operation const file_operation,
    Widget *const parent,
    std::string const &name)
    :
    Widget(parent, name),
    m_sender_submit_filename(this),
    m_internal_receiver_filename_set_by_enter_key(
        &FilePanel::InternalFilenameSetByEnterKey, this)
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
    label = new Label("Filename:", sub_control_layout);
    label->SetIsWidthFixedToTextWidth(true);
    // create the line edit to type the filename into
    m_filename_edit =
        new LineEdit(
            LONGEST_FILESYSTEM_PATH_NAME,
            sub_control_layout,
            "filename edit");
    m_filename_edit->Focus();

    SetMainWidget(main_control_layout);

    // connect the filename edit's enter key signal to this file panel's
    // filename set by enter key receiver
    SignalHandler::Connect1(
        m_filename_edit->SenderTextSetByEnterKey(),
        &m_internal_receiver_filename_set_by_enter_key);

    FilePanel::UpdateRenderBackground();
}

std::string const &FilePanel::GetFilename () const
{
    ASSERT1(m_filename_edit != NULL)
    return m_filename_edit->GetText();
}

void FilePanel::UpdateRenderBackground ()
{
    SetRenderBackground(NULL);
}

void FilePanel::InternalFilenameSetByEnterKey (std::string const &filename)
{
    m_sender_submit_filename.Signal(filename);
}

} // end of namespace Xrb
