// ///////////////////////////////////////////////////////////////////////////
// xrb_datafilescanner.cpp by Victor Dods, created 2006/07/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_datafilescanner.h"

#include <iostream>

#include "xrb_datafilelocation.h"
#include "xrb_datafilevalue.h"
#include "xrb_util.h"

#undef FL
#define FL DataFileLocation(m_input_filename, m_line_number)

namespace Xrb
{

inline bool IsWhitespace (char c) { return c == ' ' || c == '\t'; }
inline bool IsOperator (char c) { return c == '{' || c == '}' || c == '[' || c == ']' || c == ';' || c == ',' || c == '-'; }
inline bool IsAlpha (char c) { return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z'; }
inline bool IsBinaryDigit (char c) { return c == '0' || c == '1'; }
inline bool IsOctalDigit (char c) { return c >= '0' && c <= '7'; }
inline bool IsDecimalDigit (char c) { return c >= '0' && c <= '9'; }
inline bool IsHexidecimalDigit (char c) { return c >= '0' && c <= '9' || c >= 'A' && c <= 'F' || c >= 'a' && c <= 'f'; }
inline bool IsEscapableCharacter (char c) { return c == '0' || c == 'a' || c == 'b' || c == 't' || c == 'n' || c == 'v' || c == 'f' || c == 'r' || c == '\\' || c == '"'; }

Uint32 GetHexidecimalDigitValue (char c)
{
    ASSERT1(IsHexidecimalDigit(c))
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
    m_in_preamble = true;
    m_were_warnings_encountered = false;
    m_were_errors_encountered = false;
}

DataFileScanner::~DataFileScanner ()
{
    Close();
}

bool DataFileScanner::Open (std::string const &input_filename)
{
    ASSERT1(!m_input.is_open() && "you must call Close() first");

    m_input.open(input_filename.c_str());
    m_input.unsetf(std::ios_base::skipws);
    if (m_input.is_open())
        m_input_filename = input_filename;
    else
        m_input_filename.clear();
    m_text.clear();
    m_line_number = 1;
    m_in_preamble = true;
    m_were_warnings_encountered = false;
    m_were_errors_encountered = false;
    return m_input.is_open();
}

void DataFileScanner::Close ()
{
    m_input_filename.clear();
    if (m_input.is_open())
        m_input.close();
    m_text.clear();
    m_line_number = 0;
}

void DataFileScanner::EmitWarning (std::string const &message)
{
    ASSERT1(!m_input_filename.empty())
    ASSERT1(m_input.is_open())
    std::cerr << m_input_filename << ": warning: " << message << std::endl;
    m_were_warnings_encountered = true;
}

void DataFileScanner::EmitWarning (DataFileLocation const &file_location, std::string const &message)
{
    ASSERT1(!m_input_filename.empty())
    ASSERT1(m_input.is_open())
    std::cerr << file_location << ": warning: " << message << std::endl;
    m_were_warnings_encountered = true;
}

void DataFileScanner::EmitError (std::string const &message)
{
    ASSERT1(!m_input_filename.empty())
    ASSERT1(m_input.is_open())
    std::cerr << m_input_filename << ": error: " << message << std::endl;
    m_were_errors_encountered = true;
}

void DataFileScanner::EmitError (DataFileLocation const &file_location, std::string const &message)
{
    ASSERT1(!m_input_filename.empty())
    ASSERT1(m_input.is_open())
    std::cerr << file_location << ": error: " << message << std::endl;
    m_were_errors_encountered = true;
}

DataFileParser::Token::Type DataFileScanner::Scan (DataFileValue **const scanned_token)
{
    ASSERT1(!m_input_filename.empty())
    ASSERT1(m_input.is_open())
    ASSERT1(m_line_number > 0)

    ASSERT1(scanned_token != NULL);
    ASSERT1(*scanned_token == NULL);

    while (true)
    {
        m_text.clear();

        char c;

        while (!IsNextCharEOF(&c) && IsWhitespace(c))
            m_input >> c;

        m_input >> c;
        if (m_input.eof())
            return DataFileParser::Token::_END;
        m_text += c;

        if (IsAlpha(c) || c == '_')
            return ScanIdentifier(scanned_token);
        else if (IsOperator(c))
            return ScanOperator(scanned_token);
        else if (IsDecimalDigit(c))
            return ScanNumeric(scanned_token);
        else switch (c)
        {
            case '\'': return ScanCharacterLiteral(scanned_token);
            case '"': return ScanStringLiteral(scanned_token);
            case '\n': ++m_line_number; break;
            case '/':
                try
                {
                    ScanComment();
                }
                catch (DataFileParser::Token::Type token_type)
                {
                    if (token_type == DataFileParser::Token::_END)
                        EmitWarning(FL, "unterminated comment");
                    return token_type;
                }
                break;
            default:
                return DataFileParser::Token::ERROR;
        }
    }
}

DataFileParser::Token::Type DataFileScanner::ScanIdentifier (DataFileValue **const scanned_token)
{
    ASSERT1(scanned_token != NULL);
    ASSERT1(*scanned_token == NULL);
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
    if (lowercase_text == "true" || lowercase_text == "on" || lowercase_text == "yes")
    {
        *scanned_token = new DataFileBoolean(true);
        return DataFileParser::Token::BOOLEAN;
    }
    else if (lowercase_text == "false" || lowercase_text == "off" || lowercase_text == "no")
    {
        *scanned_token = new DataFileBoolean(false);
        return DataFileParser::Token::BOOLEAN;
    }

    *scanned_token = new DataFileString(m_text);
    return DataFileParser::Token::IDENTIFIER;
}

DataFileParser::Token::Type DataFileScanner::ScanOperator (DataFileValue **const scanned_token)
{
    ASSERT1(scanned_token != NULL);
    ASSERT1(*scanned_token == NULL);
    ASSERT1(!m_input.eof());

    return static_cast<DataFileParser::Token::Type>(m_text[0]);
}

DataFileParser::Token::Type DataFileScanner::ScanNumeric (DataFileValue **const scanned_token)
{
    ASSERT1(scanned_token != NULL);
    ASSERT1(*scanned_token == NULL);
    ASSERT1(!m_input.eof());

    char c;

    ASSERT1(IsDecimalDigit(m_text[0]))
    if (m_text[0] == '0')
    {
        // check if it's a standalone 0
        if (IsNextCharEOF(&c) || c != 'b' && !IsDecimalDigit(c) && c != 'x' && c != '.' && c != 'e' && c != 'E')
        {
            *scanned_token = new DataFileInteger(static_cast<Uint32>(0));
            return DataFileParser::Token::INTEGER;
        }

        m_input >> c;
        m_text += c;

        if (c == 'b')
            return ScanBinaryNumeric(scanned_token);
        else if (IsOctalDigit(c))
            return ScanOctalNumeric(scanned_token, c);
        else if (c == 'x')
            return ScanHexidecimalNumeric(scanned_token);
        else if (c == '.' || c == 'e' || c == 'E')
            return ScanFloatingPointNumeric(scanned_token);
        else
            return DataFileParser::Token::ERROR;
    }
    else
        return ScanDecimalNumeric(scanned_token, m_text[0]);
}

DataFileParser::Token::Type DataFileScanner::ScanBinaryNumeric (DataFileValue **const scanned_token)
{
    ASSERT1(scanned_token != NULL)
    ASSERT1(*scanned_token == NULL)
    ASSERT1(!m_input.eof())

    char c;
    Uint32 value = 0;
    while (!IsNextCharEOF(&c) && IsBinaryDigit(c))
    {
        m_input >> c;
        m_text += c;
        // TODO: check for overflow
        value = 2 * value + (c - '0');
    }

    // an operator, whitespace, newline, or possible comment must follow a numeric
    if (!IsNextCharEOF(&c) && !IsOperator(c) && !IsWhitespace(c) && c != '\n' && c != '/')
        return DataFileParser::Token::ERROR;

    *scanned_token = new DataFileInteger(value);
    return DataFileParser::Token::INTEGER;
}

DataFileParser::Token::Type DataFileScanner::ScanOctalNumeric (DataFileValue **const scanned_token, char const first_char)
{
    ASSERT1(scanned_token != NULL)
    ASSERT1(*scanned_token == NULL)
    ASSERT1(!m_input.eof())

    char c;
    Uint32 value = first_char - '0';
    while (!IsNextCharEOF(&c) && IsOctalDigit(c))
    {
        m_input >> c;
        m_text += c;
        // TODO: check for overflow
        value = 8 * value + (c - '0');
    }

    // an operator, whitespace, newline, or possible comment must follow a numeric
    if (!IsNextCharEOF(&c) && !IsOperator(c) && !IsWhitespace(c) && c != '\n' && c != '/')
        return DataFileParser::Token::ERROR;

    *scanned_token = new DataFileInteger(value);
    return DataFileParser::Token::INTEGER;
}

DataFileParser::Token::Type DataFileScanner::ScanDecimalNumeric (DataFileValue **const scanned_token, char const first_char)
{
    ASSERT1(scanned_token != NULL)
    ASSERT1(*scanned_token == NULL)
    ASSERT1(!m_input.eof())

    char c;
    Uint32 value = first_char - '0';
    while (!IsNextCharEOF(&c) && IsDecimalDigit(c))
    {
        m_input >> c;
        m_text += c;
        // TODO: check for overflow
        value = 10 * value + (c - '0');
    }

    if (c == '.' || c == 'e' || c == 'E')
    {
        m_input >> c;
        m_text += c;
        return ScanFloatingPointNumeric(scanned_token);
    }

    // an operator, whitespace, newline, or possible comment must follow a numeric
    if (!IsNextCharEOF(&c) && !IsOperator(c) && !IsWhitespace(c) && c != '\n' && c != '/')
        return DataFileParser::Token::ERROR;

    *scanned_token = new DataFileInteger(value);
    return DataFileParser::Token::INTEGER;
}

DataFileParser::Token::Type DataFileScanner::ScanHexidecimalNumeric (DataFileValue **const scanned_token)
{
    ASSERT1(scanned_token != NULL)
    ASSERT1(*scanned_token == NULL)
    ASSERT1(!m_input.eof())

    char c;
    Uint32 value = 0;
    while (!IsNextCharEOF(&c) && IsHexidecimalDigit(c))
    {
        m_input >> c;
        m_text += c;
        // TODO: check for overflow
        value = 16 * value + GetHexidecimalDigitValue(c);
    }

    // an operator, whitespace, newline, or possible comment must follow a numeric
    if (!IsNextCharEOF(&c) && !IsOperator(c) && !IsWhitespace(c) && c != '\n' && c != '/')
        return DataFileParser::Token::ERROR;

    *scanned_token = new DataFileInteger(value);
    return DataFileParser::Token::INTEGER;
}

DataFileParser::Token::Type DataFileScanner::ScanFloatingPointNumeric (DataFileValue **const scanned_token)
{
    ASSERT1(scanned_token != NULL)
    ASSERT1(*scanned_token == NULL)
    ASSERT1(!m_input.eof())

    char c = *m_text.rbegin();
    ASSERT1(c == '.' || c == 'e' || c == 'E')

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
            return DataFileParser::Token::ERROR;
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
            return DataFileParser::Token::ERROR;
        // read the minimum one exponent digit
        if (!IsNextCharEOF(&c) && IsDecimalDigit(c))
        {
            m_input >> c;
            m_text += c;
        }
        // if it was EOF or a non-decimal digit, return error
        if (m_input.eof() || !IsDecimalDigit(c))
            return DataFileParser::Token::ERROR;
        // read the exponent
        while (!IsNextCharEOF(&c) && IsDecimalDigit(c))
        {
            m_input >> c;
            m_text += c;
        }
    }

    // an operator, whitespace, newline, or possible comment must follow a numeric
    if (!IsNextCharEOF(&c) && !IsOperator(c) && !IsWhitespace(c) && c != '\n' && c != '/')
        return DataFileParser::Token::ERROR;

    *scanned_token = new DataFileFloat(Util::TextToFloat(m_text.c_str()));
    return DataFileParser::Token::FLOAT;
}

DataFileParser::Token::Type DataFileScanner::ScanCharacterLiteral (DataFileValue **const scanned_token)
{
    ASSERT1(scanned_token != NULL);
    ASSERT1(*scanned_token == NULL);
    ASSERT1(!m_input.eof());

    m_text.clear();
    m_text += '\'';

    char c;
    bool escaped_char = false;

    if (IsNextCharEOF(&c))
    {
        // TODO: emit fatal error
        return DataFileParser::Token::ERROR;
    }

    if (c == '\\')
    {
        m_input >> c;
        m_text += c;
        escaped_char = true;

        if (IsNextCharEOF(&c))
        {
            // TODO: emit fatal error
            return DataFileParser::Token::ERROR;
        }
    }

    if (c == '\n')
    {
        // TODO: emit fatal error
        return DataFileParser::Token::ERROR;
    }

    m_input >> c;
    m_text += c;

    if (IsNextCharEOF(&c))
    {
        // TODO: emit fatal error
        return DataFileParser::Token::ERROR;
    }

    if (c == '\'')
    {
        m_input >> c;
        m_text += c;

        // an operator, whitespace, newline, or possible comment must follow a character literal
        if (!IsNextCharEOF(&c) && !IsOperator(c) && !IsWhitespace(c) && c != '\n' && c != '/')
            return DataFileParser::Token::ERROR;

        if (escaped_char)
            *scanned_token = new DataFileCharacter(Util::GetEscapedCharacterBase(m_text[m_text.length()-2]));
        else
            *scanned_token = new DataFileCharacter(m_text[m_text.length()-2]);
        return DataFileParser::Token::CHARACTER;
    }
    else
    {
        // TODO: emit fatal error
        return DataFileParser::Token::ERROR;
    }
}

DataFileParser::Token::Type DataFileScanner::ScanStringLiteral (DataFileValue **const scanned_token)
{
    ASSERT1(scanned_token != NULL);
    ASSERT1(*scanned_token == NULL);
    ASSERT1(!m_input.eof());

    char c;

    m_text.clear();

    while (!IsNextCharEOF(&c) && c != '"')
    {
        m_input >> c;

        if (c == '\\')
        {
            if (IsNextCharEOF(&c))
            {
//                 EmitFatalError(FL, "unterminated std::string");
//                 // this is moot because EmitFatalError throws
                return DataFileParser::Token::ERROR;
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
                m_text += Util::GetEscapedCharacterBase(c);
            }
        }
        else
        {
            m_text += c;
        }
    }

    if (c == '"')
    {
        m_input >> c;

        // an operator, whitespace, newline, or possible comment must follow a string literal
        if (!IsNextCharEOF(&c) && !IsOperator(c) && !IsWhitespace(c) && c != '\n' && c != '/')
            return DataFileParser::Token::ERROR;

        *scanned_token = new DataFileString(m_text);
        return DataFileParser::Token::STRING_FRAGMENT;
    }
    else
    {
//         EmitFatalError(FL, "unterminated std::string");
//         // this is moot because EmitFatalError throws
        return DataFileParser::Token::ERROR;
    }
}

void DataFileScanner::ScanComment ()
{
    ASSERT1(!m_input.eof());

    char c;

    if (IsNextCharEOF(&c))
        throw DataFileParser::Token::ERROR;
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
                    throw DataFileParser::Token::_END;
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
            throw DataFileParser::Token::_END;
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
            throw DataFileParser::Token::_END;

        m_input >> c;
        m_text += c;
        ++m_line_number;
    }
    else
        throw DataFileParser::Token::ERROR;
}

} // end of namespace Xrb
