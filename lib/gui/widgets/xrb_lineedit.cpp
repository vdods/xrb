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
#include "xrb_utf8.h"

namespace Xrb
{

LineEdit::LineEdit (
    Uint32 const character_limit,
    ContainerWidget *const parent,
    std::string const &name)
    :
    TextWidget("", parent, name),
    m_sender_text_updated(this),
    m_sender_text_updated_v(this),
    m_sender_text_set_by_enter_key(this),
    m_sender_text_set_by_enter_key_v(this)
{
    m_accepts_focus = true;

    m_character_limit = character_limit;
    m_text.reserve(m_character_limit);
    m_text_width = 0;
    m_text_offset = ScreenCoordVector2::ms_zero;
    m_alignment = LEFT;
    m_cursor_position = 0;
    m_does_cursor_overwrite = false;
    m_is_cursor_visible = false;
    m_cursor_blink_period = 0.5f;
    m_next_cursor_blink_time = 0.0f;
    m_character_filter = CharacterFilter(CharacterFilter::DENY, "");
    m_is_read_only = false;

    ASSERT1(m_text.empty())
    ASSERT1(m_last_text_update.empty())

    SetIsHeightFixedToTextHeight(true);

    LineEdit::UpdateRenderBackground();
}

void LineEdit::SetText (std::string const &text)
{
    ASSERT1(GetRenderFont().GetIsValid())

    // only do stuff if the text is different
    if (m_text != text)
    {
        AssignFilteredString(text);
        if (m_cursor_position > m_text.length())
            SetCursorPosition(m_text.length());
        SignalTextUpdated();
        UpdateMinAndMaxSizesFromText();
    }
}

void LineEdit::SetAlignment (Alignment const alignment)
{
    ASSERT1(alignment == LEFT || alignment == CENTER || alignment == RIGHT)
    m_alignment = alignment;
}

void LineEdit::Draw (RenderContext const &render_context) const
{
    ASSERT1(GetRenderFont().GetIsValid())

    // call the superclass Draw (for the background and such)
    Widget::Draw(render_context);

    ScreenCoordRect contents_rect(GetContentsRect());
    ScreenCoordVector2 initial_pen_position(GetInitialPenPositionX(), contents_rect.GetTop());

    // draw the text
    {
        if (contents_rect.GetIsValid())
        {
            // generate a render context for the string drawing function
            RenderContext string_render_context(render_context);
            // calculate the clip rect
            string_render_context.ApplyClipRect(contents_rect);
            // calculate the color mask
            string_render_context.ApplyColorMask(GetRenderTextColor());
            // set up the GL clip rect
            string_render_context.SetupGLClipRect();
            // draw the text
            GetRenderFont()->DrawString(
                string_render_context,
                initial_pen_position,
                m_text.c_str());
        }
    }

    // draw the cursor (if it's visible and the widget is not read-only)
    if (m_is_cursor_visible && !GetIsReadOnly())
    {
        initial_pen_position[Dim::Y] = contents_rect.GetBottom();
        // calculate the cursor rectangle
        ScreenCoordRect cursor_rect(
            ScreenCoordVector2(
                GetCursorWidth(m_cursor_position),
                GetRenderFont()->GetPixelHeight()));
        // calculate the cursor's position on screen
        ScreenCoordVector2 cursor_screen_position(initial_pen_position);
        cursor_screen_position += ScreenCoordVector2(GetCursorOffset(m_cursor_position), 0);
        // move the cursor rect by the calculated amount
        cursor_rect += cursor_screen_position;

        // draw the cursor
        Render::DrawScreenRect(
            render_context,
            GetRenderTextColor(),
            cursor_rect);
    }
}

void LineEdit::SetRenderFont (Resource<Font> const &render_font)
{
    TextWidget::SetRenderFont(render_font);
    UpdateTextWidth();
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

void LineEdit::HandleFrame ()
{
    Widget::HandleFrame();

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
                SetCursorPosition(0);
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
                    UpdateTextWidth();
                }
                MakeCursorVisible();
                break;

            case Key::DELETE:
                if (m_cursor_position < m_text.length())
                {
                    ShiftText(m_cursor_position + 1, -1);
                    UpdateTextWidth();
                }
                MakeCursorVisible();
                break;

            case Key::RETURN:
            case Key::KP_ENTER:
                SignalTextUpdated();
                m_sender_text_set_by_enter_key.Signal(m_text);
                m_sender_text_set_by_enter_key_v.Signal(m_text);
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
                    UpdateTextWidth();
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
    if (m_last_text_update != m_text)
    {
        m_last_text_update = m_text;
        m_sender_text_updated.Signal(m_last_text_update);
        m_sender_text_updated_v.Signal(m_last_text_update);
    }
}

void LineEdit::UpdateMinAndMaxSizesFromText ()
{
    m_is_min_width_fixed_to_text_width = false;
    m_is_max_width_fixed_to_text_width = false;
    m_is_min_height_fixed_to_text_height = true;
    m_is_max_height_fixed_to_text_height = true;
    TextWidget::UpdateMinAndMaxSizesFromText();
}

ScreenCoord LineEdit::GetInitialPenPositionX () const
{
    ScreenCoordRect contents_rect(GetContentsRect());
    ScreenCoord initial_pen_position_x;
    switch (m_alignment)
    {
        case LEFT:
            initial_pen_position_x = 0;
            break;
        case CENTER:
            initial_pen_position_x =
                m_text_width < contents_rect.GetWidth() ?
                (contents_rect.GetRight()-contents_rect.GetLeft()-m_text_width)/2 :
                0;
            break;
        case RIGHT:
            initial_pen_position_x =
                m_text_width < contents_rect.GetWidth() ?
                contents_rect.GetRight()-contents_rect.GetLeft()-m_text_width :
                0;
            break;
        default:
            ASSERT1(false && "Invalid Alignment")
            initial_pen_position_x = 0;
            break;
    }
    initial_pen_position_x += contents_rect.GetLeft();
    initial_pen_position_x += m_text_offset[Dim::X];
    return initial_pen_position_x;
}

ScreenCoord LineEdit::GetCursorOffset (Uint32 cursor_position) const
{
    ASSERT1(GetRenderFont().GetIsValid())

    ScreenCoordVector2 pen_position_26_6(ScreenCoordVector2::ms_zero);

    if (cursor_position < 0)
        cursor_position = 0;
    else if (cursor_position > m_text.length())
        cursor_position = m_text.length();

    char const *current_glyph = m_text.c_str();
    char const *next_glyph;
    for (Uint32 i = 0; i < cursor_position && current_glyph != '\0'; ++i)
    {
        next_glyph = UTF8::GetNextCharacter(current_glyph);
        GetRenderFont()->MoveThroughGlyph(
            &pen_position_26_6,
            ScreenCoordVector2::ms_zero,
            current_glyph,
            next_glyph);
        current_glyph = next_glyph;
    }

    return pen_position_26_6[Dim::X] >> 6;
}

ScreenCoord LineEdit::GetCursorWidth (Uint32 cursor_position) const
{
    if (cursor_position < 0)
        cursor_position = 0;
    else if (cursor_position > m_text.length())
        cursor_position = m_text.length();

    if (!m_does_cursor_overwrite)
        return Max(1, GetRenderFont()->GetPixelHeight()/6);
    else
        return (cursor_position == m_text.length()) ?
               GetRenderFont()->GetGlyphWidth("n") :
               GetRenderFont()->GetGlyphWidth(m_text.c_str()+cursor_position);
}

void LineEdit::SetCursorPosition (Uint32 cursor_position)
{
    if (cursor_position < 0)
        cursor_position = 0;
    else if (cursor_position > m_text.length())
        cursor_position = m_text.length();

    ScreenCoordRect contents_rect(GetContentsRect());
    ScreenCoord desired_cursor_offset = GetCursorOffset(cursor_position);
    ScreenCoord cursor_width = GetCursorWidth(cursor_position);

    if (desired_cursor_offset + m_text_offset[Dim::X] < 0)
        m_text_offset[Dim::X] = -desired_cursor_offset;
    else if (desired_cursor_offset + m_text_offset[Dim::X] > contents_rect.GetWidth() - cursor_width)
        m_text_offset[Dim::X] = -desired_cursor_offset + contents_rect.GetWidth() - cursor_width;

    m_cursor_position = cursor_position;
}

void LineEdit::UpdateTextWidth ()
{
    ScreenCoordRect contents_rect(GetContentsRect());
    m_text_width = GetRenderFont()->GetStringRect(m_text.c_str()).GetWidth();
    if (m_text_width <= contents_rect.GetWidth())
        m_text_offset = 0;
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
        }
        // otherwise just set the char and move the cursor
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
        char filtered_char = GetCharacterFilter().GetFilteredCharacter(string[string_index]);

        if (filtered_char != '\0' && filtered_char != '\n')
            m_text += filtered_char;

        ++text_size;
        ++string_index;
    }
    UpdateTextWidth();
}

} // end of namespace Xrb
