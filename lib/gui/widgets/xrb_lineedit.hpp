// ///////////////////////////////////////////////////////////////////////////
// xrb_lineedit.hpp by Victor Dods, created 2005/02/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_LINEEDIT_HPP_)
#define _XRB_LINEEDIT_HPP_

#include "xrb.hpp"

#include "xrb_characterfilter.hpp"
#include "xrb_textwidget.hpp"

namespace Xrb {

class LineEdit : public TextWidget
{
public:

    LineEdit (Uint32 character_limit, WidgetContext &context, std::string const &name = "LineEdit");
    virtual ~LineEdit () { }

    Alignment GetAlignment () const { return m_alignment; }
    Uint32 CharacterLimit () const { return m_character_limit; }
    CharacterFilter &GetCharacterFilter () { return m_character_filter; }
    bool IsReadOnly () const { return m_is_read_only; }

    virtual void SetText (std::string const &text);
    void SetAlignment (Alignment alignment);
    void SetCharacterFilter (CharacterFilter const &character_filter) { m_character_filter = character_filter; }
    void SetIsReadOnly (bool is_read_only) { m_is_read_only = is_read_only; }

    SignalSender1<std::string const &> const *SenderTextUpdated () { return &m_sender_text_updated; }
    SignalSender1<std::string> const *SenderTextUpdatedV () { return &m_sender_text_updated_v; }
    SignalSender1<std::string const &> const *SenderTextSetByEnterKey () { return &m_sender_text_set_by_enter_key; }
    SignalSender1<std::string> const *SenderTextSetByEnterKeyV () { return &m_sender_text_set_by_enter_key_v; }

    virtual void Draw (RenderContext const &render_context) const;

protected:

    virtual void SetRenderFont (Resource<Font> const &render_font);

    virtual void HandleChangedWidgetSkin ();
    virtual void UpdateRenderBackground ();

    // process one computation frame
    virtual void HandleFrame ();
    // process a key event
    virtual bool ProcessKeyEvent (EventKey const *e);
    // process a mouse button event
    virtual bool ProcessMouseButtonEvent (EventMouseButton const *e);

    // the function that's called when this widget becomes focused
    virtual void HandleFocus ();
    // the function that's called when this widget becomes unfocused
    virtual void HandleUnfocus ();

    virtual void SignalTextUpdated ();

    virtual void UpdateMinAndMaxSizesFromText ();

private:

    ScreenCoord InitialPenPositionX () const;
    ScreenCoord CursorOffset (Uint32 cursor_position) const;
    ScreenCoord CursorWidth (Uint32 cursor_position) const;

    // sets the cursor position and takes care of making sure the
    // line edit scrolls properly
    void SetCursorPosition (Uint32 cursor_position);

    void UpdateTextWidth ();
    void MakeCursorVisible ();
    void MoveCursorLeft ();
    void MoveCursorRight ();
    void TypeCharacter (char c);
    bool ShiftText (Uint32 position, Sint32 offset);
    void AssignFilteredString (std::string const &string);

    // for alignment
    ScreenCoord m_text_width;
    // for scrolling
    ScreenCoordVector2 m_text_offset;
    // horizontal text alignment (may only be LEFT, CENTER or RIGHT)
    Alignment m_alignment;
    // the maximum number of characters that can be typed into this widget
    Uint32 m_character_limit;
    // the current cursor position in the contents string
    Uint32 m_cursor_position;
    // indicates if the cursor should overwrite characters or not
    bool m_does_cursor_overwrite;
    // indicates if the cursor is visible right now
    bool m_is_cursor_visible;
    // the speed at which the cursor blinks (a full cycle)
    Float m_cursor_blink_period;
    // the next time the cursor's visibility should be toggled
    Float m_next_cursor_blink_time;
    // the last string which was signaled using m_sender_text_updated
    std::string m_last_text_update;
    // the character filter for this LineEdit
    CharacterFilter m_character_filter;
    // indicates if this LineEdit is read-only
    bool m_is_read_only;

    // ///////////////////////////////////////////////////////////////////////
    // SignalSenders
    // ///////////////////////////////////////////////////////////////////////

    SignalSender1<std::string const &> m_sender_text_updated;
    SignalSender1<std::string> m_sender_text_updated_v;
    SignalSender1<std::string const &> m_sender_text_set_by_enter_key;
    SignalSender1<std::string> m_sender_text_set_by_enter_key_v;
}; // end of class LineEdit

} // end of namespace Xrb

#endif // !defined(_LINEEDITBOX_HPP_)
