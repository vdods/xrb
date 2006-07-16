// ///////////////////////////////////////////////////////////////////////////
// xrb_datafilescanner.h by Victor Dods, created 2006/07/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_DATAFILESCANNER_H_)
#define _XRB_DATAFILESCANNER_H_

#include "xrb.h"

#include <fstream>
#include <string>

#include "xrb_datafileparser.h"

namespace Xrb
{

class DataFileValue;

class DataFileScanner
{
public:

    DataFileScanner ();
    ~DataFileScanner ();

    inline std::string const &GetInputFilename () const { return m_input_filename; }
    inline uint GetLineNumber () const { return m_line_number; }

    bool Open (std::string const &input_filename);
    void Close ();
    DataFileParser::Token::Type Scan (DataFileValue **scanned_token);

private:

    DataFileParser::Token::Type ScanIdentifier (DataFileValue **scanned_token);
    DataFileParser::Token::Type ScanOperator (DataFileValue **scanned_token);
    DataFileParser::Token::Type ScanNumeric (DataFileValue **scanned_token);
    DataFileParser::Token::Type ScanBinaryNumeric (DataFileValue **scanned_token);
    DataFileParser::Token::Type ScanOctalNumeric (DataFileValue **scanned_token, char first_char);
    DataFileParser::Token::Type ScanDecimalNumeric (DataFileValue **scanned_token, char first_char);
    DataFileParser::Token::Type ScanHexidecimalNumeric (DataFileValue **scanned_token);
    DataFileParser::Token::Type ScanFloatingPointNumeric (DataFileValue **scanned_token);
    DataFileParser::Token::Type ScanCharacterLiteral (DataFileValue **scanned_token);
    DataFileParser::Token::Type ScanStringLiteral (DataFileValue **scanned_token);

    void ScanComment ();

    inline bool IsNextCharEOF (char *c = NULL)
    {
        if (c != NULL)
            *c = m_input.peek();
        return m_input.peek() == EOF;
    }

    std::string m_input_filename;
    std::ifstream m_input;
    std::string m_text;
    uint m_line_number;
    bool m_in_preamble;
}; // end of class DataFileScanner

} // end of namespace Xrb

#endif // !defined(_XRB_DATAFILESCANNER_H_)