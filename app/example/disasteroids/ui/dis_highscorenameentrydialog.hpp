// ///////////////////////////////////////////////////////////////////////////
// dis_highscorenameentrydialog.hpp by Victor Dods, created 2006/03/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_HIGHSCORENAMEENTRYDIALOG_HPP_)
#define _DIS_HIGHSCORENAMEENTRYDIALOG_HPP_

#include "xrb.hpp"

#include "xrb_dialog.hpp"
#include "xrb_lineedit.hpp"

using namespace Xrb;

namespace Dis {

class HighScoreNameEntryDialog : public Dialog
{
public:

    HighScoreNameEntryDialog (Uint32 points, Uint32 wave_count);
    virtual ~HighScoreNameEntryDialog () { }

    SignalSender1<std::string const &> const *SenderSubmitName () { return &m_sender_submit_name; }

    std::string const &Name () const { ASSERT1(m_name_edit != NULL); return m_name_edit->Text(); }

protected:

    virtual void OKButtonActivated ();

private:

    void InternalNameSubmitted (std::string const &name);

    LineEdit *m_name_edit;

    SignalSender1<std::string const &> m_sender_submit_name;

    SignalReceiver1<std::string const &> m_internal_receiver_name_submitted;
}; // end of class HighScoreNameEntryDialog

} // end of namespace Dis

#endif // !defined(_DIS_HIGHSCORENAMEENTRYDIALOG_HPP_)
