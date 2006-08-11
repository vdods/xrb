// ///////////////////////////////////////////////////////////////////////////
// xrb_commandlineparser.h by Victor Dods, created 2006/02/18
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_COMMANDLINEPARSER_H_)
#define _XRB_COMMANDLINEPARSER_H_

#include "xrb.h"

#include <iostream>
#include <string>

// #if defined(ENABLE_COMMANDLINEPARSER_DEBUG_SPEW)
//     #define DEBUG_SPEW(x) cerr << x << endl
// #else // !defined(ENABLE_COMMANDLINEPARSER_DEBUG_SPEW)
    #define DEBUG_SPEW(x)
// #endif // !defined(ENABLE_COMMANDLINEPARSER_DEBUG_SPEW)

#define REQUIRES_A_PARAMETER true
#define NO_PARAMETER         false

#define OPTION_HEADER(text) { '\n', "", NO_PARAMETER, NULL, text }
#define OPTION_HANDLER(method) static_cast<void (CommandLineParser::*)(string const &)>(method)

namespace Xrb
{

struct CommandLineOption;

class CommandLineParser
{
public:

    typedef void (CommandLineParser::*HandlerMethod)(std::string const &);

    CommandLineParser (
        HandlerMethod non_option_argument_handler_method,
        CommandLineOption const *option,
        Uint32 option_count,
        std::string const &executable_filename,
        std::string const &program_description,
        std::string const &usage_message);
    virtual ~CommandLineParser () = 0;

    inline bool GetParseSucceeded () const { return m_parse_succeeded; }

    void PrintHelpMessage (std::ostream &stream) const;

protected:

    void Parse (Sint32 argc, char const *const *argv);

private:

    void PerformOptionConsistencyCheck () const;

    bool HandleShortNameOption (char const *arg, char const *next_argv);
    bool HandleLongNameOption (char const *arg, char const *next_argv);

    CommandLineOption const *FindOptionByShortName (char short_name) const;
    CommandLineOption const *FindOptionByLongName (char const *long_name) const;

    HandlerMethod const m_non_option_argument_handler_method;
    CommandLineOption const *const m_option;
    Uint32 const m_option_count;
    std::string const m_executable_filename;
    std::string const m_program_description;
    std::string const m_usage_message;
    bool m_parse_succeeded;
}; // end of class CommandLineParser

struct CommandLineOption
{
    typedef void (CommandLineParser::*HandlerMethod)(std::string const &);

    char m_short_name;
    std::string m_long_name;
    bool m_requires_a_parameter;
    HandlerMethod m_handler_method;
    std::string m_description;
}; // end of struct CommandLineOption

} // end of namespace Xrb

#endif // !defined(_XRB_COMMANDLINEPARSER_H_)
