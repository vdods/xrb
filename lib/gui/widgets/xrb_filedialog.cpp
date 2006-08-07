// ///////////////////////////////////////////////////////////////////////////
// xrb_filedialog.cpp by Victor Dods, created 2005/02/21
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_filedialog.h"

#include "xrb_label.h"
#include "xrb_layout.h"
#include "xrb_lineedit.h"
#include "xrb_widgetbackground.h"

namespace Xrb
{

FileDialog::FileDialog (
    std::string const &title_text,
    FilePanel::Operation const file_operation,
    Widget *const parent,
    std::string const &name)
    :
    Dialog(DT_OK_CANCEL, parent, name),
    m_sender_submit_filename(this),
    m_sender_submit_filename_v(this),
    m_internal_receiver_filename_submitted(&FileDialog::InternalFilenameSubmitted, this)
{
    m_file_panel =
        new FilePanel(
            title_text,
            file_operation,
            GetDialogLayout());

    SignalHandler::Connect1(
        m_file_panel->SenderSubmitFilename(),
        &m_internal_receiver_filename_submitted);
}

void FileDialog::OKButtonActivated ()
{
    Dialog::OKButtonActivated();
    m_sender_submit_filename.Signal(GetFilename());
    m_sender_submit_filename_v.Signal(GetFilename());
}

void FileDialog::InternalFilenameSubmitted (std::string const &filename)
{
    ASSERT1(filename == GetFilename())
    OKButtonActivated();
}

} // end of namespace Xrb
