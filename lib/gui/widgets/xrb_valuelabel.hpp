// ///////////////////////////////////////////////////////////////////////////
// xrb_valuelabel.hpp by Victor Dods, created 2005/02/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_VALUELABEL_HPP_)
#define _XRB_VALUELABEL_HPP_

#include "xrb.hpp"

#include "xrb_label.hpp"
#include "xrb_util.hpp"

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
        ContainerWidget *parent,
        std::string const &name = "ValueLabel");
    virtual ~ValueLabel () { }

    inline ValueType Value () const
    {
        return m_value;
    }
    inline std::string const &PrintfFormat () const
    {
        return m_printf_format;
    }
    inline TextToValueFunctionType TextToValueFunction () const
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

template <typename ValueType>
ValueLabel<ValueType>::ValueLabel (
    std::string const &printf_format,
    TextToValueFunctionType text_to_value_function,
    ContainerWidget *const parent,
    std::string const &name)
    :
    Label("", parent, name),
    m_receiver_set_value(&ValueLabel<ValueType>::SetValue, this)
{
    // set up the printf format string
    SetPrintfFormat(printf_format);

    m_text_to_value_function = text_to_value_function;

    SetIsHeightFixedToTextHeight(true);
}

template <typename ValueType>
void ValueLabel<ValueType>::SetText (std::string const &text)
{
    SetValue(TextToValueFunction()(text.c_str()));
}

template <typename ValueType>
void ValueLabel<ValueType>::SetValue (ValueType const value)
{
    m_value = value;
    Label::SetText(Util::StringPrintf(PrintfFormat().c_str(), m_value));
}

template <typename ValueType>
void ValueLabel<ValueType>::SetTextToValueFunction (
    TextToValueFunctionType text_to_value_function)
{
    m_text_to_value_function = text_to_value_function;
    SetText(Text());
}

// convenience defines for printf formats
#define SIGNED_INTEGER_PRINTF_FORMAT      "%d"
#define UNSIGNED_INTEGER_PRINTF_FORMAT    "%u"
#define DECIMAL_NOTATION_PRINTF_FORMAT    "%g"
#define SCIENTIFIC_NOTATION_PRINTF_FORMAT "%e"

} // end of namespace Xrb

#endif // !defined(_XRB_VALUELABEL_HPP_)
