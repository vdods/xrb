// ///////////////////////////////////////////////////////////////////////////
// xrb_datafilescanner.cpp by Victor Dods, created 2006/07/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_datafilescanner.hpp"

#include <iostream>

#include "xrb_datafilelocation.hpp"
#include "xrb_datafilevalue.hpp"
#include "xrb_math.hpp"
#include "xrb_util.hpp"

#undef FL
#define FL DataFileLocation(m_input_path, m_line_number)

namespace Xrb
{

inline bool IsWhitespace (char c) { return c == ' ' || c == '\t'; }
inline bool IsOperator (char c) { return c == '{' || c == '}' || c == '[' || c == ']' || c == ';' || c == ','; }
inline bool IsAlpha (char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
inline bool IsBinaryDigit (char c) { return c == '0' || c == '1'; }
inline bool IsOctalDigit (char c) { return c >= '0' && c <= '7'; }
inline bool IsDecimalDigit (char c) { return c >= '0' && c <= '9'; }
inline bool IsHexadecimalDigit (char c) { return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'); }

Uint32 HexadecimalDigitValue (char c)
{
    ASSERT1(IsHexadecimalDigit(c));
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return 0;
}

DataFileScanner::DataFileScanner ()
{
    m_line_number = 0;
    m_were_warnings_encountered = false;
    m_were_errors_encountered = false;
}

DataFileScanner::~DataFileScanner ()
{
    Close();
}

bool DataFileScanner::Open (std::string const &input_path)
{
    ASSERT1(!m_input.is_open() && "you must call Close() first");

    m_input.open(input_path.c_str());
    m_input.unsetf(std::ios_base::skipws);
    if (m_input.is_open())
        m_input_path = input_path;
    else
        m_input_path.clear();
    m_text.clear();
    m_line_number = 1;
    m_were_warnings_encountered = false;
    m_were_errors_encountered = false;
    return m_input.is_open();
}

void DataFileScanner::Close ()
{
    m_input_path.clear();
    if (m_input.is_open())
        m_input.close();
    m_text.clear();
    m_line_number = 0;
}

void DataFileScanner::EmitWarning (std::string const &message)
{
    ASSERT1(!m_input_path.empty());
    ASSERT1(m_input.is_open());
    std::cerr << m_input_path << ": warning: " << message << std::endl;
    m_were_warnings_encountered = true;
}

void DataFileScanner::EmitWarning (DataFileLocation const &file_location, std::string const &message)
{
    ASSERT1(!m_input_path.empty());
    ASSERT1(m_input.is_open());
    std::cerr << file_location << ": warning: " << message << std::endl;
    m_were_warnings_encountered = true;
}

void DataFileScanner::EmitError (std::string const &message)
{
    ASSERT1(!m_input_path.empty());
    ASSERT1(m_input.is_open());
    std::cerr << m_input_path << ": error: " << message << std::endl;
    m_were_errors_encountered = true;
}

void DataFileScanner::EmitError (DataFileLocation const &file_location, std::string const &message)
{
    ASSERT1(!m_input_path.empty());
    ASSERT1(m_input.is_open());
    std::cerr << file_location << ": error: " << message << std::endl;
    m_were_errors_encountered = true;
}

DataFileParser::Token DataFileScanner::Scan ()
{
    ASSERT1(!m_input_path.empty());
    ASSERT1(m_input.is_open());
    ASSERT1(m_line_number > 0);

    while (true)
    {
        m_text.clear();

        char c;

        while (!IsNextCharEOF(&c) && IsWhitespace(c))
            m_input >> c;

        m_input >> c;
        if (m_input.eof())
            return DataFileParser::Terminal::END_;
        m_text += c;

        if (IsAlpha(c) || c == '_')
            return ScanIdentifier();
        else if (IsOperator(c))
            return ScanOperator();
        else if (IsDecimalDigit(c) || c == '+' || c == '-')
            return ScanNumeric();
        else switch (c)
        {
            case '\'': return ScanCharacterLiteral();
            case '"': return ScanStringLiteral();
            case '\n': ++m_line_number; break;
            case '/':
                try
                {
                    ScanComment();
                }
                catch (DataFileParser::Token token)
                {
                    if (token.m_id == DataFileParser::Terminal::END_)
                        EmitWarning(FL, "unterminated comment");
                    return token;
                }
                break;
            default:
                return DataFileParser::Terminal::BAD_TOKEN;
        }
    }
}

DataFileParser::Token DataFileScanner::ScanIdentifier ()
{
    ASSERT1(!m_input.eof());

    char c;

    while (!IsNextCharEOF(&c) && (IsAlpha(c) || IsDecimalDigit(c) || c == '_'))
    {
        m_input >> c;
        m_text += c;
    }

    // check if it matches any keywords
    std::string lowercase_text(m_text);
    Util::MakeLowercase(&lowercase_text);
    if (lowercase_text == "true")
        return DataFileParser::Token(DataFileParser::Terminal::BOOLEAN, new DataFileBoolean(true));
    else if (lowercase_text == "false")
        return DataFileParser::Token(DataFileParser::Terminal::BOOLEAN, new DataFileBoolean(false));
    else
        return DataFileParser::Token(DataFileParser::Terminal::IDENTIFIER, new DataFileString(m_text));
}

DataFileParser::Token DataFileScanner::ScanOperator ()
{
    ASSERT1(!m_input.eof());
    return static_cast<DataFileParser::Token::Id>(m_text[0]);
}

DataFileParser::Token DataFileScanner::ScanNumeric ()
{
    ASSERT1(!m_input.eof());

    char c = m_text[0];

    ASSERT1(IsDecimalDigit(c) || c == '+' || c == '-');

    bool is_signed = false;
    bool is_positive = true;
    if (c == '+' || c == '-')
    {
        is_signed = true;
        is_positive = c == '+';

        if (IsNextCharEOF(&c) || !IsDecimalDigit(c))
        {
            ASSERT1(m_text.length() == 1);
            if (m_text[0] == '+')
                return DataFileParser::Token('+');

            EmitError(FL, "malformed numeric value");
            return DataFileParser::Terminal::BAD_TOKEN;
        }

        m_input >> c;
        m_text += c;
    }

    if (c == '0')
    {
        // check if it's a standalone 0
        if (IsNextCharEOF(&c) || (c != 'b' && !IsDecimalDigit(c) && c != 'x' && c != '.' && c != 'e' && c != 'E'))
        {
            if (is_signed)
                return DataFileParser::Token(DataFileParser::Terminal::SINT32, new DataFileSint32(0));
            else
                return DataFileParser::Token(DataFileParser::Terminal::UINT32, new DataFileUint32(0));
        }

        m_input >> c;
        m_text += c;

        if (c == 'b')
            return ScanBinaryNumeric(is_signed, is_positive);
        else if (IsOctalDigit(c))
            return ScanOctalNumeric(is_signed, is_positive, c);
        else if (c == 'x')
            return ScanHexadecimalNumeric(is_signed, is_positive);
        else if (c == '.' || c == 'e' || c == 'E')
            return ScanFloatingPointNumeric();
        else
            return DataFileParser::Terminal::BAD_TOKEN;
    }
    else
        return ScanDecimalNumeric(is_signed, is_positive, c);
}

DataFileParser::Token DataFileScanner::ScanBinaryNumeric (bool is_signed, bool is_positive)
{
    ASSERT1(!m_input.eof());

    char c;
    bool actually_read_digits = false;
    bool overflow = false;
    Uint32 value = 0;
    while (!IsNextCharEOF(&c) && IsBinaryDigit(c))
    {
        actually_read_digits = true;
        m_input >> c;
        m_text += c;
        // if the highest bit is about to be pushed off
        // the top, set the overflow flag.
        if ((value&0x80000000) != 0)
            overflow = true;
        value = (value << 1) + (c - '0');
    }

    if (!actually_read_digits)
    {
        EmitError(FL, std::string("malformed ") + (is_signed ? "signed" : "unsigned") + " binary value " + m_text);
        return DataFileParser::Terminal::BAD_TOKEN;
    }

    // if signed, check for signed overflow
    if (is_signed &&
        ((is_positive && value > static_cast<Uint32>(SINT32_UPPER_BOUND))
         ||
         (!is_positive && value > static_cast<Uint32>(SINT32_UPPER_BOUND)+1)))
    {
        overflow = true;
    }

    // an operator, whitespace, newline, or possible comment must follow a numeric
    if (!IsNextCharEOF(&c) && !IsOperator(c) && !IsWhitespace(c) && c != '\n' && c != '/')
        return DataFileParser::Terminal::BAD_TOKEN;

    if (overflow)
        EmitError(FL, std::string("overflow in ") + (is_signed ? "signed" : "unsigned") + " binary value " + m_text);

    if (is_signed)
        return DataFileParser::Token(
            DataFileParser::Terminal::SINT32,
            new DataFileSint32(is_positive ? static_cast<Sint32>(value) : -static_cast<Sint32>(value)));
    else
        return DataFileParser::Token(DataFileParser::Terminal::UINT32, new DataFileUint32(value));
}

DataFileParser::Token DataFileScanner::ScanOctalNumeric (bool is_signed, bool is_positive, char first_char)
{
    ASSERT1(!m_input.eof());

    char c;
    bool overflow = false;
    ASSERT1(first_char >= '0');
    Uint32 value = first_char - '0';
    while (!IsNextCharEOF(&c) && IsOctalDigit(c))
    {
        m_input >> c;
        m_text += c;
        // if any of the 3 highest bits are about to be
        // pushed off the top, set the overflow flag.
        if ((value&0xE0000000) != 0)
            overflow = true;
        value = (value << 3) + (c - '0');
    }

    // if signed, check for signed overflow
    if (is_signed &&
        ((is_positive && value > static_cast<Uint32>(SINT32_UPPER_BOUND))
         ||
         (!is_positive && value > static_cast<Uint32>(SINT32_UPPER_BOUND)+1)))
    {
        overflow = true;
    }

    // an operator, whitespace, newline, or possible comment must follow a numeric
    if (!IsNextCharEOF(&c) && !IsOperator(c) && !IsWhitespace(c) && c != '\n' && c != '/')
        return DataFileParser::Terminal::BAD_TOKEN;

    if (overflow)
        EmitError(FL, std::string("overflow in ") + (is_signed ? "signed" : "unsigned") + " octal value " + m_text);

    if (is_signed)
        return DataFileParser::Token(
            DataFileParser::Terminal::SINT32,
            new DataFileSint32(is_positive ? static_cast<Sint32>(value) : -static_cast<Sint32>(value)));
    else
        return DataFileParser::Token(DataFileParser::Terminal::UINT32, new DataFileUint32(value));
}

DataFileParser::Token DataFileScanner::ScanDecimalNumeric (bool is_signed, bool is_positive, char first_char)
{
    ASSERT1(!m_input.eof());

    char c;
    bool overflow = false;
    ASSERT1(first_char >= '0');
    Uint32 value = first_char - '0';
    while (!IsNextCharEOF(&c) && IsDecimalDigit(c))
    {
        m_input >> c;
        m_text += c;
        // if the value is above the maximum for which multiplying
        // by 10 does not overflow, or if the value is said maximum
        // and a value which would overflow the value is about to
        // be added, set the overflow flag.
        if (value > 429496729 || (value == 429496729 && c > '5'))
            overflow = true;
        value = 10 * value + (c - '0');
    }

    if (c == '.' || c == 'e' || c == 'E')
    {
        m_input >> c;
        m_text += c;
        return ScanFloatingPointNumeric();
    }

    // if signed, check for signed overflow
    if (is_signed &&
        ((is_positive && value > static_cast<Uint32>(SINT32_UPPER_BOUND))
         ||
         (!is_positive && value > static_cast<Uint32>(SINT32_UPPER_BOUND)+1)))
    {
        overflow = true;
    }

    // an operator, whitespace, newline, or possible comment must follow a numeric
    if (!IsNextCharEOF(&c) && !IsOperator(c) && !IsWhitespace(c) && c != '\n' && c != '/')
        return DataFileParser::Terminal::BAD_TOKEN;

    if (overflow)
        EmitError(FL, std::string("overflow in ") + (is_signed ? "signed" : "unsigned") + " decimal value " + m_text);

    if (is_signed)
        return DataFileParser::Token(
            DataFileParser::Terminal::SINT32,
            new DataFileSint32(is_positive ? static_cast<Sint32>(value) : -static_cast<Sint32>(value)));
    else
        return DataFileParser::Token(DataFileParser::Terminal::UINT32, new DataFileUint32(value));
}

DataFileParser::Token DataFileScanner::ScanHexadecimalNumeric (bool is_signed, bool is_positive)
{
    ASSERT1(!m_input.eof());

    char c;
    bool actually_read_digits = false;
    bool overflow = false;
    Uint32 value = 0;
    while (!IsNextCharEOF(&c) && IsHexadecimalDigit(c))
    {
        actually_read_digits = true;
        m_input >> c;
        m_text += c;
        if ((value&0xF0000000) != 0)
            overflow = true;
        value = (value << 4) + HexadecimalDigitValue(c);
    }

    if (!actually_read_digits)
    {
        EmitError(FL, std::string("malformed ") + (is_signed ? "signed" : "unsigned") + " binary value " + m_text);
        return DataFileParser::Terminal::BAD_TOKEN;
    }

    // if signed, check for signed overflow
    if (is_signed &&
        ((is_positive && value > static_cast<Uint32>(SINT32_UPPER_BOUND))
         ||
         (!is_positive && value > static_cast<Uint32>(SINT32_UPPER_BOUND)+1)))
    {
        overflow = true;
    }

    // an operator, whitespace, newline, or possible comment must follow a numeric
    if (!IsNextCharEOF(&c) && !IsOperator(c) && !IsWhitespace(c) && c != '\n' && c != '/')
        return DataFileParser::Terminal::BAD_TOKEN;

    if (overflow)
        EmitError(FL, std::string("overflow in ") + (is_signed ? "signed" : "unsigned") + " hexadecimal value " + m_text);

    if (is_signed)
        return DataFileParser::Token(
            DataFileParser::Terminal::SINT32,
            new DataFileSint32(is_positive ? static_cast<Sint32>(value) : -static_cast<Sint32>(value)));
    else
        return DataFileParser::Token(DataFileParser::Terminal::UINT32, new DataFileUint32(value));
}

DataFileParser::Token DataFileScanner::ScanFloatingPointNumeric ()
{
    ASSERT1(!m_input.eof());

    char c = *m_text.rbegin();
    ASSERT1(c == '.' || c == 'e' || c == 'E');

    if (c == '.')
    {
        // there must be at least one digit after the decimal point
        if (!IsNextCharEOF(&c) && IsDecimalDigit(c))
        {
            m_input >> c;
            m_text += c;
        }
        // if it was EOF or a non-decimal digit, return error
        if (m_input.eof() || !IsDecimalDigit(c))
            return DataFileParser::Terminal::BAD_TOKEN;
        // read the rest of the fractional part
        while (!IsNextCharEOF(&c) && IsDecimalDigit(c))
        {
            m_input >> c;
            m_text += c;
        }
        // read the optional exponent indicator
        if (!IsNextCharEOF(&c) && (c == 'e' || c == 'E'))
        {
            m_input >> c;
            m_text += c;
        }
    }

    if (c == 'e' || c == 'E')
    {
        // read the optional sign
        if (!IsNextCharEOF(&c) && (c == '+' || c == '-'))
        {
            m_input >> c;
            m_text += c;
        }
        // if it was EOF or (a non-sign and non-decimal digit), return error
        if (m_input.eof() || (c != '+' && c != '-' && !IsDecimalDigit(c)))
            return DataFileParser::Terminal::BAD_TOKEN;
        // read the minimum one exponent digit
        if (!IsNextCharEOF(&c) && IsDecimalDigit(c))
        {
            m_input >> c;
            m_text += c;
        }
        // if it was EOF or a non-decimal digit, return error
        if (m_input.eof() || !IsDecimalDigit(c))
            return DataFileParser::Terminal::BAD_TOKEN;
        // read the exponent
        while (!IsNextCharEOF(&c) && IsDecimalDigit(c))
        {
            m_input >> c;
            m_text += c;
        }
    }

    // an operator, whitespace, newline, or possible comment must follow a numeric
    if (!IsNextCharEOF(&c) && !IsOperator(c) && !IsWhitespace(c) && c != '\n' && c != '/')
        return DataFileParser::Terminal::BAD_TOKEN;

    Float value = Util::TextToFloat(m_text.c_str());
    if (!Math::IsFinite(value))
        EmitError(FL, std::string("overflow/underflow in floating point value ") + m_text);

    return DataFileParser::Token(DataFileParser::Terminal::FLOAT, new DataFileFloat(value));
}

DataFileParser::Token DataFileScanner::ScanCharacterLiteral ()
{
    ASSERT1(!m_input.eof());

    m_text.clear();
    m_text += '\'';

    char c;
    bool escaped_char = false;

    if (IsNextCharEOF(&c))
    {
        EmitError(FL, "unexpected end of file");
        return DataFileParser::Terminal::BAD_TOKEN;
    }

    if (c == '\\')
    {
        m_input >> c;
        m_text += c;
        escaped_char = true;

        if (IsNextCharEOF(&c))
        {
            EmitError(FL, "unexpected end of file");
            return DataFileParser::Terminal::BAD_TOKEN;
        }
    }

    m_input >> c;
    m_text += c;

    if (c == '\n' || (!escaped_char && c == '\''))
    {
        if (c == '\n')
            ++m_line_number;
        if (!escaped_char && c == '\'')
        {
            if (!IsNextCharEOF(&c) && c == '\'')
            {
                m_input >> c;
                m_text += c;
            }
        }

        EmitError(FL, "malformed character literal");
        return DataFileParser::Terminal::BAD_TOKEN;
    }

    if (IsNextCharEOF(&c))
    {
        EmitError(FL, "unexpected end of file");
        return DataFileParser::Terminal::BAD_TOKEN;
    }

    if (c == '\'')
    {
        m_input >> c;
        m_text += c;

        // an operator, whitespace, newline, or possible comment must follow a character literal
        if (!IsNextCharEOF(&c) && !IsOperator(c) && !IsWhitespace(c) && c != '\n' && c != '/')
            return DataFileParser::Terminal::BAD_TOKEN;

        c = m_text[m_text.length()-2];
        if (escaped_char)
            c = Util::EscapedChar(c);

        return DataFileParser::Token(DataFileParser::Terminal::CHARACTER, new DataFileCharacter(c));
    }
    else
    {
        EmitError(FL, "malformed character literal");
        return DataFileParser::Terminal::BAD_TOKEN;
    }
}

DataFileParser::Token DataFileScanner::ScanStringLiteral ()
{
    ASSERT1(!m_input.eof());

    char c;
    Uint32 starting_line = m_line_number;

    m_text.clear();

    while (!IsNextCharEOF(&c) && c != '"')
    {
        m_input >> c;

        if (c == '\\')
        {
            if (IsNextCharEOF(&c))
            {
                EmitError(DataFileLocation(m_input_path, starting_line), "unterminated string");
                return DataFileParser::Terminal::BAD_TOKEN;
            }
            else if (c == '\n')
            {
                ++m_line_number;
                // ignore this escaped newline
                m_input >> c;
            }
            else
            {
                m_input >> c;
                m_text += Util::EscapedChar(c);
            }
        }
        else
        {
            if (c == '\n')
                ++m_line_number;

            m_text += c;
        }
    }

    if (c == '"')
    {
        m_input >> c;

        // an operator, whitespace, newline, or possible comment must follow a string literal
        if (!IsNextCharEOF(&c) && !IsOperator(c) && !IsWhitespace(c) && c != '\n' && c != '/')
            return DataFileParser::Terminal::BAD_TOKEN;

        return DataFileParser::Token(DataFileParser::Terminal::STRING_FRAGMENT, new DataFileString(m_text));
    }
    else
    {
        EmitError(DataFileLocation(m_input_path, starting_line), "unterminated string");
        return DataFileParser::Terminal::BAD_TOKEN;
    }
}

void DataFileScanner::ScanComment ()
{
    ASSERT1(!m_input.eof());

    char c;

    if (IsNextCharEOF(&c))
        throw DataFileParser::Terminal::BAD_TOKEN;
    else if (c == '*')
    {
        m_input >> c;
        m_text += c;
        while (!IsNextCharEOF(&c))
        {
            m_input >> c;
            m_text += c;
            if (c == '*')
            {
                if (IsNextCharEOF(&c))
                    throw DataFileParser::Terminal::END_;
                else if (c == '/')
                {
                    m_input >> c;
                    m_text += c;
                    break;
                }
            }
            else if (c == '\n')
                ++m_line_number;
        }

        if (IsNextCharEOF())
            throw DataFileParser::Terminal::END_;
    }
    else if (c == '/')
    {
        m_input >> c;
        m_text += c;
        while (!IsNextCharEOF(&c) && c != '\n')
        {
            m_input >> c;
            m_text += c;
        }

        if (IsNextCharEOF())
            throw DataFileParser::Terminal::END_;

        m_input >> c;
        m_text += c;
        ++m_line_number;
    }
    else
        throw DataFileParser::Terminal::BAD_TOKEN;
}

} // end of namespace Xrb
