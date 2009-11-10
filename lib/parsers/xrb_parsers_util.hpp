// ///////////////////////////////////////////////////////////////////////////
// xrb_parsers_util.hpp by Victor Dods, created 2009/11/10
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_PARSERS_UTIL_HPP_)
#define _XRB_PARSERS_UTIL_HPP_

#include "xrb.hpp"

#include <string>

namespace Xrb {
// namespace Parsers {

enum EscapeStringReturnCode
{
    ESRC_SUCCESS = 0,
    // if there's a backslash immediately before the end of the string
    ESRC_UNEXPECTED_EOI,
    // if there's \x without a hex digit after it
    ESRC_MALFORMED_HEX_CHAR,
    // if the hex code's value exceeded 255
    ESRC_HEX_ESCAPE_SEQUENCE_OUT_OF_RANGE,
    // if the octal code's value exceeded 255
    ESRC_OCTAL_ESCAPE_SEQUENCE_OUT_OF_RANGE
}; // end of enum EscapeStringReturnCode

struct EscapeStringStatus
{
    EscapeStringReturnCode m_return_code;
    Uint32 m_line_number_offset;

    EscapeStringStatus (EscapeStringReturnCode return_code, Uint32 line_number_offset)
        :
        m_return_code(return_code),
        m_line_number_offset(line_number_offset)
    { }
}; // end of struct EscapeStringStatus

Uint8 SwitchCase (Uint8 c);
// in-place char-escaping
void EscapeChar (Uint8 &c);
Uint8 EscapedChar (Uint8 c);
// in-place string-escaping
EscapeStringStatus EscapeString (std::string &text);
std::string EscapedString (std::string const &text, EscapeStringReturnCode *escape_string_return_code = NULL);
std::string CharLiteral (Uint8 c, bool with_quotes = true);
std::string StringLiteral (std::string const &text, bool with_quotes = true);

Uint32 NewlineCount (std::string const &text);

// } // end of namespace Parsers
} // end of namespace Xrb

#endif // !defined(_XRB_PARSERS_UTIL_HPP_)
