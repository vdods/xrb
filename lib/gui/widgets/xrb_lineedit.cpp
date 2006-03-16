// ///////////////////////////////////////////////////////////////////////////
// xrb_lineedit.cpp by Victor Dods, created 2005/02/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_lineedit.h"

#include "xrb_input_events.h"
#include "xrb_render.h"
#include "xrb_screen.h"

namespace Xrb
{

LineEdit::LineEdit (
    Uint32 const character_limit,
    Widget *const parent,
    std::string const &name)
    :
    Label("", parent, name),
    m_sender_text_updated(this),
    m_sender_text_set_by_enter_key(this)
{
    m_accepts_focus = true;

    m_character_limit = character_limit;
    m_text.reserve(m_character_limit);
    SetAlignment(Alignment2(LEFT, CENTER));

    m_cursor_position = 0;
    m_does_cursor_overwrite = false;

    m_is_cursor_visible = false;
    m_cursor_blink_period = 0.5;
    m_next_cursor_blink_time = 0.0;

    // default filter denies nothing (allows anything), except for newlines
    // which are always filtered
    m_character_filter = CharacterFilter(CharacterFilter::DENY, "");

    m_is_read_only = false;

    ASSERT1(m_last_text_update.empty())

    SetIsHeightFixedToTextHeight(true);

    LineEdit::UpdateRenderBackground();
}

void LineEdit::SetText (std::string const &text)
{
    // only do stuff if the text is different
    if (m_text != text)
    {
        AssignFilteredString(text);

        if (m_cursor_position > m_text.length())
            SetCursorPosition(m_text.length());

        SignalTextUpdated();

        // this is necessary so that the Label text stuff is updated
        DirtyTextFormatting();
    }
}

void LineEdit::SetWordWrap (bool const word_wrap)
{
    ASSERT1(!word_wrap && "You can't turn word wrapping on in a LineEdit")
}

void LineEdit::Draw (RenderContext const &render_context) const
{
    ASSERT1(!GetWordWrap())
    // call the superclass Draw (for the background and such)
    Widget::Draw(render_context);
    // have Label draw the text
    Label::DrawText(render_context);

    // the clip rect from the call to Label::DrawTextInternal will be used

    // draw the cursor (if it's visible and the widget is not read-only)
    if (m_is_cursor_visible && !GetIsReadOnly())
    {
        // figure out where the cursor is in relation to the text
        ScreenCoord cursor_offset = GetCursorOffset(m_cursor_position);
        // calculate the cursor's position on screen
        ScreenCoordVector2 cursor_screen_position(
            GetPosition() +
            GetFrameMargins() +
            GetContentMargins());
        cursor_screen_position +=
            ScreenCoordVector2(cursor_offset + m_text_offset[Dim::X], 0);
        // calculate the width (based on if overwrite mode is set)
        ScreenCoord cursor_width = GetCursorWidth(m_cursor_position);
        // calculate the cursor rectangle
        ScreenCoordRect cursor_rect(
            ScreenCoordVector2(
                cursor_width,
                GetHeight() -
                2 * (GetFrameMargins()[Dim::Y] + GetContentMargins()[Dim::Y])));
        cursor_rect += cursor_screen_position;

        // do the drawing call
        Render::DrawScreenRect(
            render_context,
            GetTextColor(),
            cursor_rect);
    }
}

void LineEdit::UpdateRenderBackground ()
{
    SetRenderBackground(
        GetWidgetSkinWidgetBackground(WidgetSkin::LINE_EDIT_BACKGROUND));
}

void LineEdit::HandleChangedWidgetSkinWidgetBackground (
    WidgetSkin::WidgetBackgroundType const widget_background_type)
{
    if (widget_background_type == WidgetSkin::LINE_EDIT_BACKGROUND)
        UpdateRenderBackground();
}

void LineEdit::ProcessFrameOverride ()
{
    // blink the cursor if it's time
    if (GetIsFocused() && GetFrameTime() >= m_next_cursor_blink_time)
    {
        m_is_cursor_visible = !m_is_cursor_visible;
        m_next_cursor_blink_time = GetFrameTime() + 0.5f * m_cursor_blink_period;
    }
}

bool LineEdit::ProcessKeyEvent (EventKey const *const e)
{
    // read only LineEdits don't allow input
    if (GetIsReadOnly())
        return false;

    if (e->GetIsKeyDownEvent() || e->GetIsKeyRepeatEvent())
    {
        switch (e->GetKeyCode())
        {
            case Key::LEFT:
                MoveCursorLeft();
                MakeCursorVisible();
                break;

            case Key::RIGHT:
                MoveCursorRight();
                MakeCursorVisible();
                break;

            case Key::HOME:
                m_cursor_position = 0;
                MakeCursorVisible();
                break;

            case Key::END:
                SetCursorPosition(m_text.length());
                MakeCursorVisible();
                break;

            case Key::INSERT:
                m_does_cursor_overwrite = !m_does_cursor_overwrite;
                break;

            case Key::BACKSPACE:
                if (m_cursor_position > 0)
                {
                    bool shift_text_worked = ShiftText(m_cursor_position, -1);
                    if (shift_text_worked)
                        MoveCursorLeft();
                }
                MakeCursorVisible();
                break;

            case Key::DELETE:
                if (m_cursor_position < m_text.length())
                    ShiftText(m_cursor_position + 1, -1);
                MakeCursorVisible();
                break;

            case Key::RETURN:
            case Key::KP_ENTER:
                SignalTextUpdated();
                m_sender_text_set_by_enter_key.Signal(GetText());
                break;

            default:
                // type the char into the text buffer, it will
                // handle inserting/overwriting
                char c =
                    GetCharacterFilter().
                        GetFilteredCharacter(
                            e->GetModifiedAscii());
                if (c != '\0')
                {
                    TypeCharacter(c);
                    MakeCursorVisible();
                }
                break;
        }
    }

    return true;
}

bool LineEdit::ProcessMouseButtonEvent (EventMouseButton const *const e)
{
    return e->GetIsMouseButtonDownEvent() && !GetIsReadOnly();
}

void LineEdit::HandleFocus ()
{
    MakeCursorVisible();
}

void LineEdit::HandleUnfocus ()
{
    m_is_cursor_visible = false;
    SignalTextUpdated();
}

void LineEdit::SignalTextUpdated ()
{
    if (m_last_text_update != GetText())
    {
        m_last_text_update = GetText();
        m_sender_text_updated.Signal(m_last_text_update);
    }
}

ScreenCoord LineEdit::GetCursorOffset (Uint32 cursor_position) const
{
    ScreenCoord retval = 0;

    if (cursor_position < 0)
        cursor_position = 0;
    else if (cursor_position > m_text.length())
        cursor_position = m_text.length();

    for (Uint32 i = 0; i < cursor_position; ++i)
        retval += GetRenderFont()->GetGlyphPixelAdvance(m_text[i]);

    return retval;
}

ScreenCoord LineEdit::GetCursorWidth (Uint32 cursor_position) const
{
    if (cursor_position < 0)
        cursor_position = 0;
    else if (cursor_position > m_text.length())
        cursor_position = m_text.length();

    return m_does_cursor_overwrite ?
           GetRenderFont()->GetGlyphPixelAdvance(m_text[cursor_position]) :
           static_cast<ScreenCoord>(GetTopLevelParent()->GetSizeRatioBasis() * 0.003f);
}

void LineEdit::SetCursorPosition (Uint32 cursor_position)
{
    if (cursor_position < 0)
        cursor_position = 0;
    else if (cursor_position > m_text.length())
        cursor_position = m_text.length();

    ScreenCoord desired_cursor_offset = GetCursorOffset(cursor_position);
    ScreenCoord text_space = GetWidth() - 2 * GetFrameMargins()[Dim::X];
    ScreenCoord cursor_width = GetCursorWidth(cursor_position);

    if (desired_cursor_offset + m_text_offset[Dim::X] < 0)
        m_text_offset[Dim::X] = -desired_cursor_offset;
    else if (desired_cursor_offset + m_text_offset[Dim::X] > text_space - cursor_width)
        m_text_offset[Dim::X] = -desired_cursor_offset - cursor_width + text_space;

    m_cursor_position = cursor_position;
}

void LineEdit::MakeCursorVisible ()
{
    m_is_cursor_visible = true;
    m_next_cursor_blink_time = GetMostRecentFrameTime() + 0.5f * m_cursor_blink_period;
}

void LineEdit::MoveCursorLeft ()
{
    if (m_cursor_position > 0)
        SetCursorPosition(m_cursor_position - 1);
}

void LineEdit::MoveCursorRight ()
{
    if (m_cursor_position < m_text.length())
        SetCursorPosition(m_cursor_position + 1);
}

void LineEdit::TypeCharacter (char const c)
{
    if (c == '\0' || c == '\n')
        return;

    ASSERT1(m_cursor_position <= m_text.length())

    if (m_does_cursor_overwrite)
    {
        // if the cursor was at the end of the line, shift (the null),
        // add the char and move the cursor
        if (m_cursor_position == m_text.length())
        {
            bool shift_text_worked = ShiftText(m_cursor_position, 1);
            if (shift_text_worked)
            {
                m_text[m_cursor_position] = c;
                MoveCursorRight();
            }

        // otherwise just set the char and move the cursor
        }
        else
        {
            m_text[m_cursor_position] = c;
            MoveCursorRight();
        }
    }
    else
    {
        bool shift_text_worked = ShiftText(m_cursor_position, 1);
        if (shift_text_worked)
        {
            m_text[m_cursor_position] = c;
            MoveCursorRight();
        }
    }
}

bool LineEdit::ShiftText (Uint32 const position, Sint32 const offset)
{
    ASSERT1(offset != 0)
    ASSERT1(position <= m_text.length())

    if (m_text.length() + offset > m_character_limit)
        return false;

    if (offset < 0)
        m_text.erase(position + offset, -offset);
    else
        m_text.insert(position, offset, '~');

    return true;
}

void LineEdit::AssignFilteredString (std::string const &string)
{
    Uint32 text_size = 0;
    Uint32 string_index = 0;

    m_text.clear();
    while (string_index < string.length() && text_size < m_character_limit)
    {
        char filtered_char =
            GetCharacterFilter().
                GetFilteredCharacter(
                    string[string_index]);

        if (filtered_char != '\0' && filtered_char != '\n')
            m_text += filtered_char;

        ++text_size;
        ++string_index;
    }
}

} // end of namespace Xrb
