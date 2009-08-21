// ///////////////////////////////////////////////////////////////////////////
// xrb_filepanel.hpp by Victor Dods, created 2005/05/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_FILEPANEL_HPP_)
#define _XRB_FILEPANEL_HPP_

#include "xrb.hpp"

#include "xrb_containerwidget.hpp"

namespace Xrb
{

class Label;
class LineEdit;

class FilePanel : public ContainerWidget
{
public:

    enum Operation
    {
        OP_OPEN = 0,
        OP_SAVE
    }; // end of enum Operation

    FilePanel (
        std::string const &title_text,
        Operation file_operation,
        ContainerWidget *parent,
        std::string const &name = "FilePanel");
    virtual ~FilePanel () { }

    inline Operation FileOperation () const { return m_file_operation; }
    std::string const &Path () const;

    inline SignalSender1<std::string const &> const *SenderSubmitPath () { return &m_sender_submit_path; }
    inline SignalSender1<std::string> const *SenderSubmitPathV () { return &m_sender_submit_path_v; }

protected:

    virtual void UpdateRenderBackground ();

private:

    void InternalPathSetByEnterKey (std::string const &path);

    enum
    {
        LONGEST_FILESYSTEM_PATH_NAME = 512 // more or less arbitrary for now
    };

    // a "current directory" member might be necessary for asynchronously
    // used file panels.

    // a label which is a short description of this dialog
    Label *m_title_label;
    // indicates if this is an open or save dialog
    Operation m_file_operation;
    // the LineEdit to type in the path
    LineEdit *m_path_edit;

    SignalSender1<std::string const &> m_sender_submit_path;
    SignalSender1<std::string> m_sender_submit_path_v;

    SignalReceiver1<std::string const &> m_internal_receiver_path_set_by_enter_key;
}; // end of class FilePanel

} // end of namespace Xrb

#endif // !defined(_XRB_FILEPANEL_HPP_)
