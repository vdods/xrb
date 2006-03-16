// ///////////////////////////////////////////////////////////////////////////
// xrb_linecalculator.cpp by Victor Dods, created 2005/07/21
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_linecalculator.h"

// function prototypes for the scanner
int LineCalculator_lex_init (void **yyscanner);
int LineCalculator_lex_destroy (void *yyscanner);
void LineCalculator_buffer_state_initialize (
    char const *string,
    void *buffer_state,
    void *line_calculator_scanner);
void LineCalculator_buffer_state_shutdown (
    void *buffer_state,
    void *line_calculator_scanner);

// function prototype for the parser
int LineCalculator_parse (void *yyscanner, Xrb::Float *parsed_value);

namespace Xrb
{

Float LineCalculator::Parse (char const *string)
{
// this is not yet supported in WIN32
#if defined(WIN32)
    return 0.0f;
#else // !defined(WIN32)
    void *line_calculator_scanner;
    void *buffer_state;
    Float parsed_value;

    // set up the scanner
    LineCalculator_lex_init(&line_calculator_scanner);
    // set the scanner to read from the string instead of a file
    LineCalculator_buffer_state_initialize(
        string,
        &buffer_state,
        line_calculator_scanner);
    // parse the file
    int parse_exit_code =
        LineCalculator_parse(
            line_calculator_scanner,
            &parsed_value);
    // shut down the string scanning
    LineCalculator_buffer_state_shutdown(
        buffer_state,
        line_calculator_scanner);
    // shut down the scanner
    LineCalculator_lex_destroy(line_calculator_scanner);

    // if the string was parsed correctly, return the parsed value
    if (parse_exit_code == 0)
        return parsed_value;
    // otherwise throw an error
    else
    {
        throw "parse error";
        return static_cast<Float>(0);
    }
#endif // !defined(WIN32)
}

} // end of namespace Xrb

