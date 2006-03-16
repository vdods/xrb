// ///////////////////////////////////////////////////////////////////////////
// xrb_valueedit.h by Victor Dods, created 2005/02/23
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_VALUEEDIT_H_)
#define _XRB_VALUEEDIT_H_

#include "xrb.h"

#include "xrb_input_events.h"
#include "xrb_key.h"
#include "xrb_lineedit.h"
#include "xrb_util.h"
#include "xrb_validator.h"

namespace Xrb
{

template <typename ValueType> struct Validator;

template <typename ValueType>
class ValueEdit : public LineEdit
{
public:

    typedef ValueType (*TextToValueFunctionType)(char const *);

    ValueEdit (
        std::string const &printf_format,
        typename ValueEdit<ValueType>::TextToValueFunctionType text_to_value_function,
        Widget *parent,
        std::string const &name = "ValueEdit");
    virtual ~ValueEdit () { }

    inline ValueType GetValue () const
    {
        return m_value;
    }
    inline Validator<ValueType> const *GetValidator () const
    {
        return m_validator;
    }
    inline std::string const &GetPrintfFormat () const
    {
        return m_printf_format;
    }
    inline TextToValueFunctionType GetTextToValueFunction () const
    {
        return m_text_to_value_function;
    }

    virtual void SetText (std::string const &text);
    void SetValue (ValueType value);
    void SetValidator (Validator<ValueType> const *validator);
    inline void SetPrintfFormat (std::string const &printf_format)
    {
        m_printf_format = printf_format;
    }
    void SetTextToValueFunction (TextToValueFunctionType text_to_value_function);

    inline SignalSender1<ValueType> const *SenderValueUpdated ()
    {
        return &m_sender_value_updated;
    }
    inline SignalSender1<ValueType> const *SenderValueSetByEnterKey ()
    {
        return &m_sender_value_set_by_enter_key;
    }

    inline SignalReceiver1<ValueType> const *ReceiverSetValue ()
    {
        return &m_receiver_set_value;
    }

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

// function definitions for ValueEdit
#include "xrb_valueedit.tcpp"

// convenience defines for printf formats
#define SIGNED_INTEGER_PRINTF_FORMAT      "%d"
#define UNSIGNED_INTEGER_PRINTF_FORMAT    "%u"
#define DECIMAL_NOTATION_PRINTF_FORMAT    "%g"
#define SCIENTIFIC_NOTATION_PRINTF_FORMAT "%e"

} // end of namespace Xrb

#endif // !defined(_XRB_VALUEEDIT_H_)
