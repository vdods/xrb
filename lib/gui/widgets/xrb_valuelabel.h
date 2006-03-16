// ///////////////////////////////////////////////////////////////////////////
// xrb_valuelabel.h by Victor Dods, created 2005/02/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_VALUELABEL_H_)
#define _XRB_VALUELABEL_H_

#include "xrb.h"

#include "xrb_label.h"
#include "xrb_util.h"

namespace Xrb
{

template <typename ValueType>
class ValueLabel : public Label
{
public:

    typedef ValueType (*TextToValueFunctionType)(char const *);

    ValueLabel (
        std::string const &printf_format,
        TextToValueFunctionType text_to_value_function,
        Widget *parent,
        std::string const &name = "ValueLabel");
    virtual ~ValueLabel () { }

    inline ValueType GetValue () const
    {
        return m_value;
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
    inline void SetPrintfFormat (std::string const &printf_format)
    {
        m_printf_format = printf_format;
    }
    void SetTextToValueFunction (TextToValueFunctionType text_to_value_function);

    inline SignalReceiver1<ValueType> const *ReceiverSetValue ()
    {
        return &m_receiver_set_value;
    }

protected:

    // the actual value of this widget
    ValueType m_value;

private:

    // the string passed to *printf as the format string
    std::string m_printf_format;
    // the text-to-value function pointer
    TextToValueFunctionType m_text_to_value_function;

    //////////////////////////////////////////////////////////////////////////
    // SignalReceivers
    SignalReceiver1<ValueType> m_receiver_set_value;
}; // end of class ValueLabel

// function definitions for ValueLabel
#include "xrb_valuelabel.tcpp"

// convenience defines for printf formats
#define SIGNED_INTEGER_PRINTF_FORMAT      "%d"
#define UNSIGNED_INTEGER_PRINTF_FORMAT    "%u"
#define DECIMAL_NOTATION_PRINTF_FORMAT    "%g"
#define SCIENTIFIC_NOTATION_PRINTF_FORMAT "%e"

} // end of namespace Xrb

#endif // !defined(_XRB_VALUELABEL_H_)
