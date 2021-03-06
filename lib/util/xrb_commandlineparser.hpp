// ///////////////////////////////////////////////////////////////////////////
// xrb_commandlineparser.hpp by Victor Dods, created 2006/02/18
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_COMMANDLINEPARSER_HPP_)
#define _XRB_COMMANDLINEPARSER_HPP_

#include "xrb.hpp"

#include <iostream>
#include <string>

namespace Xrb
{

struct CommandLineOption;

class CommandLineParser
{
public:

    typedef void (CommandLineParser::*HandlerMethodWithArgument)(std::string const &);
    typedef void (CommandLineParser::*HandlerMethodWithoutArgument)();

    template <typename CommandLineParserSubclass>
    CommandLineParser (
        void (CommandLineParserSubclass::*non_option_argument_handler_method)(std::string const &),
        CommandLineOption const *option,
        Uint32 option_count,
        std::string const &executable_path,
        std::string const &program_description,
        std::string const &usage_message)
        :
        m_non_option_argument_handler_method(static_cast<HandlerMethodWithArgument>(non_option_argument_handler_method)),
        m_option(option),
        m_option_count(option_count),
        m_executable_path(executable_path),
        m_program_description(program_description),
        m_usage_message(usage_message),
        m_parse_succeeded(true)
    {
        ASSERT0(m_non_option_argument_handler_method != NULL);
        ASSERT0(m_option != NULL);
        ASSERT0(m_option_count > 0);
        ASSERT1(!m_executable_path.empty());

        PerformOptionConsistencyCheck();
    }
    virtual ~CommandLineParser () = 0;

    inline bool ParseSucceeded () const { return m_parse_succeeded; }

    void Parse (Sint32 argc, char const *const *argv);
    void PrintHelpMessage (std::ostream &stream) const;

private:

    static bool IsAControlOption (CommandLineOption const &option);
    static bool IsAShortNameCollision (CommandLineOption const &option_0, CommandLineOption const &option_1);
    static bool IsALongNameCollision (CommandLineOption const &option_0, CommandLineOption const &option_1);

    void PerformOptionConsistencyCheck () const;

    // the return value indicates if next_arg was eaten up and should be skipped
    bool HandleShortNameOption (char const *arg, char const *next_arg);
    // the return value indicates if next_arg was eaten up and should be skipped
    bool HandleLongNameOption (char const *arg, char const *next_arg);

    CommandLineOption const *FindOptionByShortName (char short_name) const;
    CommandLineOption const *FindOptionByLongName (char const *long_name) const;

    HandlerMethodWithArgument const m_non_option_argument_handler_method;
    CommandLineOption const *const m_option;
    Uint32 const m_option_count;
    std::string const m_executable_path;
    std::string const m_program_description;
    std::string const m_usage_message;
    bool m_parse_succeeded;
}; // end of class CommandLineParser

struct CommandLineOption
{
public:

    CommandLineOption (std::string const &header_text)
        :
        m_short_name('\n'),
        m_long_name(),
        m_requires_an_argument(false),
        m_handler_method_with_argument(NULL),
        m_handler_method_without_argument(NULL),
        m_description(header_text)
    { }
    template <typename CommandLineParserSubclass>
    CommandLineOption (
        char short_name,
        void (CommandLineParserSubclass::*handler_method_with_argument)(std::string const &),
        std::string const &description)
        :
        m_short_name(short_name),
        m_long_name(),
        m_requires_an_argument(true),
        m_handler_method_with_argument(static_cast<HandlerMethodWithArgument>(handler_method_with_argument)),
        m_handler_method_without_argument(NULL),
        m_description(description)
    { }
    template <typename CommandLineParserSubclass>
    CommandLineOption (
        std::string const &long_name,
        void (CommandLineParserSubclass::*handler_method_with_argument)(std::string const &),
        std::string const &description)
        :
        m_short_name('\0'),
        m_long_name(long_name),
        m_requires_an_argument(true),
        m_handler_method_with_argument(static_cast<HandlerMethodWithArgument>(handler_method_with_argument)),
        m_handler_method_without_argument(NULL),
        m_description(description)
    { }
    template <typename CommandLineParserSubclass>
    CommandLineOption (
        char short_name,
        std::string const &long_name,
        void (CommandLineParserSubclass::*handler_method_with_argument)(std::string const &),
        std::string const &description)
        :
        m_short_name(short_name),
        m_long_name(long_name),
        m_requires_an_argument(true),
        m_handler_method_with_argument(static_cast<HandlerMethodWithArgument>(handler_method_with_argument)),
        m_handler_method_without_argument(NULL),
        m_description(description)
    { }
    template <typename CommandLineParserSubclass>
    CommandLineOption (
        char short_name,
        void (CommandLineParserSubclass::*handler_method_without_argument)(),
        std::string const &description)
        :
        m_short_name(short_name),
        m_long_name(),
        m_requires_an_argument(false),
        m_handler_method_with_argument(NULL),
        m_handler_method_without_argument(static_cast<HandlerMethodWithoutArgument>(handler_method_without_argument)),
        m_description(description)
    { }
    template <typename CommandLineParserSubclass>
    CommandLineOption (
        std::string const &long_name,
        void (CommandLineParserSubclass::*handler_method_without_argument)(),
        std::string const &description)
        :
        m_short_name('\0'),
        m_long_name(long_name),
        m_requires_an_argument(false),
        m_handler_method_with_argument(NULL),
        m_handler_method_without_argument(static_cast<HandlerMethodWithoutArgument>(handler_method_without_argument)),
        m_description(description)
    { }
    template <typename CommandLineParserSubclass>
    CommandLineOption (
        char short_name,
        std::string const &long_name,
        void (CommandLineParserSubclass::*handler_method_without_argument)(),
        std::string const &description)
        :
        m_short_name(short_name),
        m_long_name(long_name),
        m_requires_an_argument(false),
        m_handler_method_with_argument(NULL),
        m_handler_method_without_argument(static_cast<HandlerMethodWithoutArgument>(handler_method_without_argument)),
        m_description(description)
    { }

private:

    typedef void (CommandLineParser::*HandlerMethodWithArgument)(std::string const &);
    typedef void (CommandLineParser::*HandlerMethodWithoutArgument)();

    char const m_short_name;
    std::string const m_long_name;
    bool const m_requires_an_argument;
    HandlerMethodWithArgument const m_handler_method_with_argument;
    HandlerMethodWithoutArgument const m_handler_method_without_argument;
    std::string const m_description;

    // kludgey, but it's using this as a struct anyway
    friend class CommandLineParser;
}; // end of struct CommandLineOption

} // end of namespace Xrb

#endif // !defined(_XRB_COMMANDLINEPARSER_HPP_)
