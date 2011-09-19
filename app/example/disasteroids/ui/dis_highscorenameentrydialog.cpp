// ///////////////////////////////////////////////////////////////////////////
// dis_highscorenameentrydialog.cpp by Victor Dods, created 2006/03/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_highscorenameentrydialog.hpp"

#include "dis_util.hpp"
#include "xrb_label.hpp"
#include "xrb_layout.hpp"
#include "xrb_lineedit.hpp"
#include "xrb_util.hpp"

using namespace Xrb;

namespace Dis {

HighScoreNameEntryDialog::HighScoreNameEntryDialog (Uint32 points, Uint32 wave_count, WidgetContext &context)
    :
    Dialog(DT_OK, context, "HighScoreNameEntryDialog"),
    m_sender_submit_name(this),
    m_internal_receiver_name_submitted(&HighScoreNameEntryDialog::InternalNameSubmitted, this)
{
    Layout *main_layout = new Layout(VERTICAL, Context());
    {
        main_layout->AttachChild(new Label("NEW HIGH SCORE", Context()));

        Layout *score_layout = new Layout(HORIZONTAL, Context());
        {
            score_layout->AttachChild(new Label(Util::StringPrintf("WAVE %u", wave_count), Context()));
            score_layout->AttachChild(new Label(Util::StringPrintf("%u", points), Context()));
        }
        main_layout->AttachChild(score_layout);

        Layout *name_entry_layout = new Layout(HORIZONTAL, Context());
        {
            name_entry_layout->AttachChild(new Label("ENTER YOUR NAME", Context()));
            
            m_name_edit = new LineEdit(30, Context(), "name edit");
            name_entry_layout->AttachChild(m_name_edit);
        }
        main_layout->AttachChild(name_entry_layout);
    }
    DialogLayout()->AttachChild(main_layout);

    // set the LineEdit to be in focus
    m_name_edit->Focus();

    SignalHandler::Connect1(
        m_name_edit->SenderTextSetByEnterKey(),
        &m_internal_receiver_name_submitted);
}

void HighScoreNameEntryDialog::OKButtonActivated ()
{
    Dialog::OKButtonActivated();
    m_sender_submit_name.Signal(Name());
}

void HighScoreNameEntryDialog::InternalNameSubmitted (std::string const &name)
{
    ASSERT1(name == Name());
    OKButtonActivated();
}

} // end of namespace Dis
