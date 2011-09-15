// ///////////////////////////////////////////////////////////////////////////
// xrb_valueedit.hpp by Victor Dods, created 2005/02/23
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_VALUEEDIT_HPP_)
#define _XRB_VALUEEDIT_HPP_

#include "xrb.hpp"

#include "xrb_input_events.hpp"
#include "xrb_key.hpp"
#include "xrb_lineedit.hpp"
#include "xrb_util.hpp"
#include "xrb_validator.hpp"

namespace Xrb {

template <typename ValueType> struct Validator;

template <typename ValueType>
class ValueEdit : public LineEdit
{
public:

    typedef ValueType (*TextToValueFunctionType)(char const *);

    ValueEdit (std::string const &printf_format, TextToValueFunctionType text_to_value_function, std::string const &name = "ValueEdit");
    virtual ~ValueEdit () { }

    ValueType Value () const { return m_value; }
    Validator<ValueType> const *GetValidator () const { return m_validator; }
    std::string const &PrintfFormat () const { return m_printf_format; }
    TextToValueFunctionType TextToValueFunction () const { return m_text_to_value_function; }

    virtual void SetText (std::string const &text);
    void SetValue (ValueType value);
    void SetValidator (Validator<ValueType> const *validator);
    void SetPrintfFormat (std::string const &printf_format) { m_printf_format = printf_format; }
    void SetTextToValueFunction (TextToValueFunctionType text_to_value_function);

    SignalSender1<ValueType> const *SenderValueUpdated () { return &m_sender_value_updated; }
    SignalSender1<ValueType> const *SenderValueSetByEnterKey () { return &m_sender_value_set_by_enter_key; }

    SignalReceiver1<ValueType> const *ReceiverSetValue () { return &m_receiver_set_value; }

protected:

    // process a key event
    virtual bool ProcessKeyEvent (EventKey const *e);
    // the function that's called when this widget becomes unfocused
    virtual void HandleUnfocus ();

    virtual void SignalTextUpdated ();

private:

    void SetValueFromText ();
    void SignalValueUpdated ();

    // the actual value of this widget
    ValueType m_value;
    // the last value which was signaled
    ValueType m_last_value_update;
    // the value validator
    Validator<ValueType> const *m_validator;
    // the string passed to *printf as the format string
    std::string m_printf_format;
    // the text-to-value function pointer
    TextToValueFunctionType m_text_to_value_function;

    //////////////////////////////////////////////////////////////////////////
    // SignalSenders
    SignalSender1<ValueType> m_sender_value_updated;
    SignalSender1<ValueType> m_sender_value_set_by_enter_key;

    //////////////////////////////////////////////////////////////////////////
    // SignalReceivers
    SignalReceiver1<ValueType> m_receiver_set_value;
}; // end of class ValueEdit

template <typename ValueType>
ValueEdit<ValueType>::ValueEdit (std::string const &printf_format, TextToValueFunctionType text_to_value_function, std::string const &name)
    :
    LineEdit(20, name),
    m_sender_value_updated(this),
    m_sender_value_set_by_enter_key(this),
    m_receiver_set_value(&ValueEdit<ValueType>::SetValue, this)
{
    // set up the printf format string
    SetPrintfFormat(printf_format);

    // default is to not have any validation.
    m_validator = NULL;
    m_text_to_value_function = text_to_value_function;
}

template <typename ValueType>
void ValueEdit<ValueType>::SetText (std::string const &text)
{
    SetValue(m_text_to_value_function(text.c_str()));
}

template <typename ValueType>
void ValueEdit<ValueType>::SetValue (ValueType value)
{
    m_value = m_validator != NULL ?
              m_validator->Validate(value) :
              value;
    LineEdit::SetText(Util::StringPrintf(m_printf_format.c_str(), m_value));
    SignalValueUpdated();
}

template <typename ValueType>
void ValueEdit<ValueType>::SetValidator (Validator<ValueType> const *validator)
{
    m_validator = validator;
    SetValue(m_value);
}

template <typename ValueType>
void ValueEdit<ValueType>::SetTextToValueFunction (TextToValueFunctionType text_to_value_function)
{
    m_text_to_value_function = text_to_value_function;
    SetText(Text());
}

template <typename ValueType>
bool ValueEdit<ValueType>::ProcessKeyEvent (EventKey const *e)
{
    if (e->IsKeyDownEvent() || e->IsKeyRepeatEvent())
    {
        switch (e->KeyCode())
        {
            case Key::RETURN:
            case Key::KP_ENTER:
                // only set and signal the value if there is text
                if (LineEdit::Text()[0] != '\0')
                {
                    SetValueFromText();
                    m_sender_value_set_by_enter_key.Signal(Value());
                }
                break;

            default:
                break;
        }
    }

    return LineEdit::ProcessKeyEvent(e);
}

template <typename ValueType>
void ValueEdit<ValueType>::HandleUnfocus ()
{
    SignalValueUpdated();
    LineEdit::HandleUnfocus();
}

template <typename ValueType>
void ValueEdit<ValueType>::SignalTextUpdated ()
{
    SetValue(m_text_to_value_function(LineEdit::Text().c_str()));
    LineEdit::SignalTextUpdated();
    SignalValueUpdated();
}

template <typename ValueType>
void ValueEdit<ValueType>::SetValueFromText ()
{
    ValueType value = m_text_to_value_function(LineEdit::Text().c_str());
    if (m_validator != NULL)
        value = m_validator->Validate(value);
    if (m_value != value)
    {
        m_value = value;
        SignalValueUpdated();
    }
}

template <typename ValueType>
void ValueEdit<ValueType>::SignalValueUpdated ()
{
    if (m_last_value_update != Value())
    {
        m_last_value_update = Value();
        m_sender_value_updated.Signal(m_last_value_update);
    }
}

// convenience defines for printf formats
#define SIGNED_INTEGER_PRINTF_FORMAT      "%d"
#define UNSIGNED_INTEGER_PRINTF_FORMAT    "%u"
#define DECIMAL_NOTATION_PRINTF_FORMAT    "%g"
#define SCIENTIFIC_NOTATION_PRINTF_FORMAT "%e"

} // end of namespace Xrb

#endif // !defined(_XRB_VALUEEDIT_HPP_)
