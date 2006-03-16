// ///////////////////////////////////////////////////////////////////////////
// dis_titlescreenwidget.cpp by Victor Dods, created 2006/02/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_titlescreenwidget.h"

#include "dis_optionspanel.h"
#include "xrb_button.h"
#include "xrb_cellpaddingwidget.h"
#include "xrb_dialog.h"
#include "xrb_label.h"
#include "xrb_layout.h"
#include "xrb_widgetbackground.h" // TEMP
#include "xrb_widgetstack.h"

using namespace Xrb;

namespace Dis
{

TitleScreenWidget::TitleScreenWidget (Widget *const parent)
    :
    Widget(parent, "TitleScreenWidget"),
    m_internal_receiver_go_to_options(&TitleScreenWidget::GoToOptions, this)
{
    Layout *main_layout = new Layout(VERTICAL, this, "main title screen layout");
    main_layout->SetIsUsingZeroedFrameMargins(true);
    main_layout->SetIsUsingZeroedLayoutSpacingMargins(true);

    CellPaddingWidget *logo_padding_widget =
        new CellPaddingWidget(main_layout, "logo padding widget");
    logo_padding_widget->FixHeightRatio(0.20f);
    
    m_logo_label = new Label("DISASTEROIDS", logo_padding_widget, "logo label");
    m_logo_label->SetFontHeightRatio(0.10f);

    WidgetStack *center_panel_widget_stack = new WidgetStack(main_layout, "center panel widget stack");
    
    m_game_widget_dummy = new Label("game demo widget", center_panel_widget_stack, "game widget dummy");
    m_game_widget_dummy->SetFontHeightRatio(0.10f);
    m_game_widget_dummy->SetBackground(new WidgetBackgroundColored(Color(1.0f, 0.0f, 0.0f, 1.0f)));

    m_high_scores_widget_dummy =
        new Label(
            "high scores widget\n"
            "high scores widget\n"
            "high scores widget\n"
            "high scores widget\n"
            "high scores widget\n"
            "high scores widget\n"
            "high scores widget\n",
            center_panel_widget_stack,
            "high scores widget dummy");
    m_high_scores_widget_dummy->Hide();

    Layout *stuff_layout = new Layout(VERTICAL, main_layout, "stuff layout");
    stuff_layout->FixHeightRatio(0.20f);
    stuff_layout->SetIsUsingZeroedFrameMargins(true);
    stuff_layout->SetIsUsingZeroedLayoutSpacingMargins(true);
        
    CellPaddingWidget *controls_padding_widget =
        new CellPaddingWidget(stuff_layout, "controls padding widget");

    Layout *controls_layout = new Layout(HORIZONTAL, controls_padding_widget, "controls layout");

    m_start_button = new Button("START", controls_layout, "start button");
    m_start_button->SetIsHeightFixedToTextHeight(true);
    
    m_options_button = new Button("OPTIONS", controls_layout, "options button");
    m_options_button->SetIsHeightFixedToTextHeight(true);
    
    m_quit_button = new Button("QUIT", controls_layout, "quit button");
    m_quit_button->SetIsHeightFixedToTextHeight(true);

    Layout *footnotes_layout = new Layout(HORIZONTAL, stuff_layout, "footnotes_layout layout");
    footnotes_layout->SetIsUsingZeroedFrameMargins(true);
    footnotes_layout->SetIsUsingZeroedLayoutSpacingMargins(true);

    Label *credits_label = new Label("(C)opyright 2004-2006 by Victor Dods", footnotes_layout, "credits label");
    credits_label->SetIsHeightFixedToTextHeight(true);
    credits_label->SetAlignment(Dim::X, LEFT);

    Label *library_label = new Label("Part of the XuqRijBuh Game Engine", footnotes_layout, "library label");
    library_label->SetIsHeightFixedToTextHeight(true);
    library_label->SetAlignment(Dim::X, RIGHT);

    SetMainWidget(main_layout);

    // ///////////////////////////////////////////////////////////////////////
    // hook up the button signals
    // ///////////////////////////////////////////////////////////////////////

    SignalHandler::Connect0(
        m_options_button->SenderReleased(),
        &m_internal_receiver_go_to_options);
}

TitleScreenWidget::~TitleScreenWidget ()
{
}

SignalSender0 const *TitleScreenWidget::SenderStartGame ()
{
    ASSERT1(m_start_button != NULL)
    return m_start_button->SenderReleased();
}

SignalSender0 const *TitleScreenWidget::SenderQuitGame ()
{
    ASSERT1(m_quit_button != NULL)
    return m_quit_button->SenderReleased();
}

void TitleScreenWidget::GoToOptions ()
{
    Dialog *options_dialog = new Dialog(Dialog::DT_OK_CANCEL, this, "options dialog");
    // TODO: save this OptionsPanel for later retreival of the options
    OptionsPanel *options_panel = new OptionsPanel(options_dialog->GetDialogLayout());
    // TODO: hook up the dialog result signal
    options_dialog->Resize(options_dialog->GetParent()->GetSize() * 4 / 5);
    options_dialog->CenterOnWidget(options_dialog->GetParent());
}

} // end of namespace Dis

