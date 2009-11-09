// ///////////////////////////////////////////////////////////////////////////
// xrb_datafilescanner.hpp by Victor Dods, created 2006/07/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_DATAFILESCANNER_HPP_)
#define _XRB_DATAFILESCANNER_HPP_

#include "xrb.hpp"

#include <fstream>
#include <string>

#include "xrb_datafileparser.hpp"

namespace Xrb
{

class DataFileLocation;
class DataFileValue;

class DataFileScanner
{
public:

    DataFileScanner ();
    ~DataFileScanner ();

    inline bool IsOpen () const { return m_input.is_open(); }
    inline std::string const &InputPath () const { return m_input_path; }
    inline Uint32 LineNumber () const { return m_line_number; }
    inline bool WereWarningsEncountered () const { return m_were_warnings_encountered; }
    inline bool WereErrorsEncountered () const { return m_were_errors_encountered; }

    bool Open (std::string const &input_path);
    void Close ();

    void EmitWarning (std::string const &message);
    void EmitWarning (DataFileLocation const &file_location, std::string const &message);

    void EmitError (std::string const &message);
    void EmitError (DataFileLocation const &file_location, std::string const &message);

    DataFileParser::Token Scan ();

private:

    DataFileParser::Token ScanIdentifier ();
    DataFileParser::Token ScanOperator ();
    DataFileParser::Token ScanNumeric ();
    DataFileParser::Token ScanBinaryNumeric (bool is_signed, bool is_positive);
    DataFileParser::Token ScanOctalNumeric (bool is_signed, bool is_positive, char first_char);
    DataFileParser::Token ScanDecimalNumeric (bool is_signed, bool is_positive, char first_char);
    DataFileParser::Token ScanHexadecimalNumeric (bool is_signed, bool is_positive);
    DataFileParser::Token ScanFloatingPointNumeric ();
    DataFileParser::Token ScanCharacterLiteral ();
    DataFileParser::Token ScanStringLiteral ();

    void ScanComment ();

    inline bool IsNextCharEOF (char *c = NULL)
    {
        if (c != NULL)
            *c = m_input.peek();
        return m_input.peek() == EOF;
    }

    std::string m_input_path;
    std::ifstream m_input;
    std::string m_text;
    Uint32 m_line_number;
    bool m_were_warnings_encountered;
    bool m_were_errors_encountered;
}; // end of class DataFileScanner

} // end of namespace Xrb

#endif // !defined(_XRB_DATAFILESCANNER_HPP_)
