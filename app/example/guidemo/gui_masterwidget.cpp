// ///////////////////////////////////////////////////////////////////////////
// gui_masterwidget.cpp by Victor Dods, created 2006/06/04
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "gui_masterwidget.hpp"

#include "xrb_label.hpp"
#include "xrb_layout.hpp"
#include "xrb_lineedit.hpp"
#include "xrb_widgetbackground.hpp" // temp?

using namespace Xrb;

MasterWidget::MasterWidget (ContainerWidget *const parent)
    :
    ContainerWidget(parent, "MasterWidget")
{
    Layout *main_layout = new Layout(ROW, 3, this, "main master widget layout");
    SetMainWidget(main_layout);

    Alignment2 const alignment[9] =
    {
        Alignment2(  LEFT,    TOP),
        Alignment2(CENTER,    TOP),
        Alignment2( RIGHT,    TOP),
        Alignment2(  LEFT, CENTER),
        Alignment2(CENTER, CENTER),
        Alignment2( RIGHT, CENTER),
        Alignment2(  LEFT, BOTTOM),
        Alignment2(CENTER, BOTTOM),
        Alignment2( RIGHT, BOTTOM)
    };

    {
        Label *label;
        std::string blah_text("I LIKE ZOMBIES.");

        for (Uint32 i = 0; i < 9; ++i)
        {
            label = new Label(blah_text, main_layout);
            label->SetIsSizeFixedToTextSize(false);
            label->SetAlignment(alignment[i]);
            label->SetBackground(new WidgetBackgroundColored(Color(1.0f, 0.0f, 0.0f, 1.0f)));
        }
    }

    Fprint(stderr, main_layout->ScreenRect());

    {
        LineEdit *line_edit;

        for (Uint32 i = 0; i < 3; ++i)
        {
            line_edit = new LineEdit(100, main_layout);
            line_edit->SetAlignment(alignment[i][Dim::X]);
            line_edit->SetText("I LIKE EDIT ZOMBIES.");
        }
    }

//     SetMainWidget(main_layout);
}

