// ///////////////////////////////////////////////////////////////////////////
// xrb_filedialog.cpp by Victor Dods, created 2005/02/21
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_filedialog.hpp"

#include "xrb_label.hpp"
#include "xrb_layout.hpp"
#include "xrb_lineedit.hpp"
#include "xrb_widgetbackground.hpp"

namespace Xrb {

FileDialog::FileDialog (std::string const &title_text, FilePanel::Operation const file_operation, std::string const &name)
    :
    Dialog(DT_OK_CANCEL, name),
    m_sender_submit_path(this),
    m_sender_submit_path_v(this),
    m_internal_receiver_path_submitted(&FileDialog::InternalPathSubmitted, this)
{
    m_file_panel = new FilePanel(title_text, file_operation);
    DialogLayout()->AttachChild(m_file_panel);

    SignalHandler::Connect1(
        m_file_panel->SenderSubmitPath(),
        &m_internal_receiver_path_submitted);
}

void FileDialog::OKButtonActivated ()
{
    Dialog::OKButtonActivated();
    m_sender_submit_path.Signal(Path());
    m_sender_submit_path_v.Signal(Path());
}

void FileDialog::InternalPathSubmitted (std::string const &path)
{
    ASSERT1(path == Path());
    OKButtonActivated();
}

} // end of namespace Xrb
