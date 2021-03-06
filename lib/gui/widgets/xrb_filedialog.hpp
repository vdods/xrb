// ///////////////////////////////////////////////////////////////////////////
// xrb_filedialog.hpp by Victor Dods, created 2005/02/21
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_FILEDIALOG_HPP_)
#define _XRB_FILEDIALOG_HPP_

#include "xrb.hpp"

#include "xrb_dialog.hpp"
#include "xrb_filepanel.hpp"

namespace Xrb {

class FileDialog : public Dialog
{
public:

    FileDialog (std::string const &title_text, FilePanel::Operation file_operation, WidgetContext &context, std::string const &name = "FileDialog");
    virtual ~FileDialog () { }

    FilePanel::Operation FileOperation () const { return m_file_panel->FileOperation(); }
    std::string const &Path () const { return m_file_panel->Path(); }

    SignalSender1<std::string const &> const *SenderSubmitPath () { return &m_sender_submit_path; }
    SignalSender1<std::string> const *SenderSubmitPathV () { return &m_sender_submit_path_v; }

protected:

    virtual void OKButtonActivated ();

private:

    void InternalPathSubmitted (std::string const &path);

    FilePanel *m_file_panel;

    SignalSender1<std::string const &> m_sender_submit_path;
    SignalSender1<std::string> m_sender_submit_path_v;

    SignalReceiver1<std::string const &> m_internal_receiver_path_submitted;
}; // end of class FileDialog

} // end of namespace Xrb

#endif // !defined(_XRB_FILEDIALOG_HPP_)
