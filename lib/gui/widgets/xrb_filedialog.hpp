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

namespace Xrb
{

class FileDialog : public Dialog
{
public:

    FileDialog (
        std::string const &title_text,
        FilePanel::Operation file_operation,
        ContainerWidget *parent,
        std::string const &name = "FileDialog");
    virtual ~FileDialog () { }

    inline FilePanel::Operation FileOperation () const { return m_file_panel->FileOperation(); }
    inline std::string const &Filename () const { return m_file_panel->Filename(); }

    inline SignalSender1<std::string const &> const *SenderSubmitFilename () { return &m_sender_submit_filename; }
    inline SignalSender1<std::string> const *SenderSubmitFilenameV () { return &m_sender_submit_filename_v; }

protected:

    virtual void OKButtonActivated ();

private:

    void InternalFilenameSubmitted (std::string const &filename);

    FilePanel *m_file_panel;

    SignalSender1<std::string const &> m_sender_submit_filename;
    SignalSender1<std::string> m_sender_submit_filename_v;

    SignalReceiver1<std::string const &> m_internal_receiver_filename_submitted;
}; // end of class FileDialog

} // end of namespace Xrb

#endif // !defined(_XRB_FILEDIALOG_HPP_)
