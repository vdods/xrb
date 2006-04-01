// ///////////////////////////////////////////////////////////////////////////
// dis_highscorenameentrydialog.cpp by Victor Dods, created 2006/03/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_highscorenameentrydialog.h"

#include "dis_util.h"
#include "xrb_label.h"
#include "xrb_layout.h"
#include "xrb_lineedit.h"
#include "xrb_util.h"

using namespace Xrb;

namespace Dis
{

HighScoreNameEntryDialog::HighScoreNameEntryDialog (
    Uint32 const points,
    Float const time_alive,
    Widget *const parent)
    :
    Dialog(DT_OK, parent, "HighScoreNameEntryDialog"),
    m_sender_submit_name(this),
    m_internal_receiver_name_submitted(&HighScoreNameEntryDialog::InternalNameSubmitted, this)
{
    Layout *main_layout = new Layout(VERTICAL, GetDialogLayout());
    {
        new Label("NEW HIGH SCORE", main_layout);

        Layout *score_layout = new Layout(HORIZONTAL, main_layout);
        {
            new Label(GetFormattedTimeString(time_alive), score_layout);
    
            new Label(Util::StringPrintf("%u", points), score_layout);
        }
                
        Layout *name_entry_layout = new Layout(HORIZONTAL, main_layout);
        {
            new Label("ENTER YOUR NAME", name_entry_layout);

            m_name_edit = new LineEdit(30, name_entry_layout, "name edit");
        }
    }

    // set the LineEdit to be in focus
    m_name_edit->Focus();
    
    SignalHandler::Connect1(
        m_name_edit->SenderTextSetByEnterKey(),
        &m_internal_receiver_name_submitted);
}

void HighScoreNameEntryDialog::OKButtonActivated ()
{
    Dialog::OKButtonActivated();
    m_sender_submit_name.Signal(GetName());
}

void HighScoreNameEntryDialog::InternalNameSubmitted (std::string const &name)
{
    ASSERT1(name == GetName())
    OKButtonActivated();
}

} // end of namespace Dis