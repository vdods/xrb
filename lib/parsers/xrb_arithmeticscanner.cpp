// ///////////////////////////////////////////////////////////////////////////
// xrb_arithmeticscanner.cpp by Victor Dods, created 2006/08/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_arithmeticscanner.h"

#include "xrb_util.h"

namespace Xrb
{

// this namespace is used to make these functions private to this source file.
namespace
{
    inline bool IsWhitespace (char c) { return c == ' ' || c == '\t' || c == '\n'; }
    inline bool IsOperator (char c) { return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '(' || c == ')'; }
    inline bool IsDecimalDigit (char c) { return c >= '0' && c <= '9'; }
    inline Float GetDecimalDigitValue (char c) { return static_cast<Float>(c - '0'); }
}

ArithmeticScanner::ArithmeticScanner (std::string const &input_string)
    :
    m_input_string(input_string)
{
    m_it = m_input_string.begin();
    m_it_end = m_input_string.end();
}

ArithmeticParser::Token::Type ArithmeticScanner::Scan (Float *const scanned_token)
{
    // skip all whitespace
    while (m_it != m_it_end && IsWhitespace(*m_it))
        ++m_it;

    if (m_it == m_it_end)
        return ArithmeticParser::Token::END_;
    else if (IsOperator(*m_it))
        return static_cast<ArithmeticParser::Token::Type>(*m_it++);
    else if (IsDecimalDigit(*m_it) || *m_it == '.')
    {
        char const *end = NULL;
        *scanned_token = Util::TextToFloat(&*m_it, &end);
        ASSERT1(end != NULL);
        while (&*m_it != end)
        {
            ASSERT1(m_it != m_it_end);
            ++m_it;
        }
        return ArithmeticParser::Token::NUMERIC;
    }
    else
        return ArithmeticParser::Token::BAD_TOKEN;
}

} // end of namespace Xrb
