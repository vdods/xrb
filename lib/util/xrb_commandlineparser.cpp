// ///////////////////////////////////////////////////////////////////////////
// xrb_commandlineparser.cpp by Victor Dods, created 2006/02/18
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_commandlineparser.h"

using namespace std;

namespace Xrb
{

CommandLineParser::CommandLineParser (
    HandlerFunction non_option_argument_handler_function,
    CommandLineOption const *const option,
    Uint32 const option_count,
    std::string const &executable_name,
    string const &program_description,
    string const &usage_message)
    :
    m_non_option_argument_handler_function(non_option_argument_handler_function),
    m_option(option),
    m_option_count(option_count),
    m_executable_name(executable_name),
    m_program_description(program_description),
    m_usage_message(usage_message),
    m_parse_succeeded(true)
{
    // m_non_option_argument_handler_function may be NULL
    ASSERT1(m_option != NULL);
    ASSERT1(m_option_count > 0);
    ASSERT1(!m_executable_name.empty());

    PerformOptionConsistencyCheck();
}

CommandLineParser::~CommandLineParser () { }

void CommandLineParser::PrintHelpMessage (ostream &stream) const
{
    if (!m_program_description.empty())
        stream << m_program_description << "\n\n";

    stream << "usage: " << m_executable_name << " " << m_usage_message << "\n";

    if (m_option[0].m_short_name != '\n')
        stream << "\n";

    for (CommandLineOption const *option = m_option,
                                 *option_end = m_option + m_option_count;
         option != option_end;
         ++option)
    {
        if (option->m_short_name == '\n')
        {
            if (option->m_description.empty())
                stream << endl;
            else
                stream << "\n" << option->m_description << "\n" << endl;
        }
        else
        {
            if (option->m_short_name == '\0')
                stream << "   ";
            else
            {
                stream << "-" << option->m_short_name;
                if (!option->m_long_name.empty())
                    stream << ",";
            }

            if (!option->m_long_name.empty())
                stream << "--" << option->m_long_name;
            if (option->m_requires_a_parameter)
                stream << "=<parameter>";

            if (!option->m_description.empty())
                stream << "\n" << option->m_description;

            stream << endl;
        }
    }
}

void CommandLineParser::Parse (Sint32 argc, char const *const *argv)
{
    ASSERT1(argc >= 1);
    ASSERT1(argv != NULL);

    try
    {
        while (++argv, --argc > 0)
        {
            ASSERT1(*argv != NULL);
            char const *arg = *argv;
            if (*arg == '-')
            {
                ++arg;

                char const *next_argv = (argc == 1) ? NULL : *(argv + 1);

                if (*arg == '-')
                {
                    if (HandleLongNameOption(++arg, next_argv))
                        ++argv, --argc;
                }
                else
                {
                    if (HandleShortNameOption(arg, next_argv))
                        ++argv, --argc;
                }
            }
            else if (m_non_option_argument_handler_function != NULL)
            {
                (this->*m_non_option_argument_handler_function)(arg);
            }
        }
    }
    catch (string const &exception)
    {
        cerr << exception << "\n";
        m_parse_succeeded = false;
    }
}

void CommandLineParser::PerformOptionConsistencyCheck () const
{
    // make sure if a special short name is specified, then the
    // long name isn't specified.
    for (CommandLineOption const *option = m_option,
                                 *option_end = m_option + m_option_count;
         option != option_end;
         ++option)
    {
        if (GetIsASpecialShortName(option->m_short_name))
        {
            ASSERT1(option->m_long_name.empty());
            ASSERT1(option->m_requires_a_parameter == false);
            ASSERT1(option->m_handler_function == NULL);
        }
        else
        {
            ASSERT1(option->m_handler_function != NULL);
        }
    }

    // make sure that there are no short/long option name collisions.
    for (CommandLineOption const *option_0 = m_option,
                                 *option_end_0 = m_option + m_option_count;
         option_0 != option_end_0;
         ++option_0)
    {
        // loop over all options after this one in the array, so that
        // every option is compared with every other option.
        for (CommandLineOption const *option_1 = option_0 + 1;
             option_1 != option_end_0;
             ++option_1)
        {
            // there can only be multiple instances of the "special" short names
            if (!GetIsASpecialShortName(option_0->m_short_name) && !GetIsASpecialShortName(option_1->m_short_name))
                ASSERT1(option_0->m_short_name != option_1->m_short_name && "option short-name collision");

            // there can only be multiple instances of empty long names
            if (!option_0->m_long_name.empty() && !option_1->m_long_name.empty())
                ASSERT1(option_0->m_long_name != option_1->m_long_name && "option long-name collision");
        }
    }
}

bool CommandLineParser::HandleShortNameOption (
    char const *arg,
    char const *const next_argv)
{
    ASSERT1(arg != NULL);
    // next_argv may be NULL

    if (*arg == '\0')
        throw string("error: no option letter specified after \"-\"");

    CommandLineOption const *option = FindOptionByShortName(*arg);
    if (option == NULL)
        throw string("error: no such option: -") + *arg;

    ++arg;

    if (*arg != '\0')
    {
        // if the option doesn't take a parameter, then this argument
        // is a concatenated string of short options
        if (!option->m_requires_a_parameter)
        {
            // iterate through the concatenated short options, processing
            // each one, but throwing an error if one requires a parameter
            while (true)
            {
                // call the option handler function with an empty argument
                DEBUG_SPEW("short option -" << option->m_short_name);
                ASSERT1(option->m_handler_function != NULL);
                (this->*(option->m_handler_function))(string());

                if (*arg == '\0')
                    break;

                option = FindOptionByShortName(*arg);
                if (option == NULL)
                    throw string("error: no such option: -") + *arg;
                else if (option->m_requires_a_parameter)
                    throw string("error: may not concatenate short options which require a parameter (option -") + *arg + ")";

                ++arg;
            }
        }
        else
        {
            // call the option handler function with the argument
            DEBUG_SPEW("short option -" << option->m_short_name << " with arg \"" << arg << "\"");
            ASSERT1(option->m_handler_function != NULL);
            (this->*(option->m_handler_function))(string(arg));
        }

        return false;
    }
    else
    {
        if (option->m_requires_a_parameter)
        {
            if (next_argv == NULL)
                throw string("error: option -") + option->m_short_name + " requires a parameter";

            // call the option handler function with the argument
            DEBUG_SPEW("short option -" << option->m_short_name << " with arg \"" << next_argv << "\"");
            ASSERT1(option->m_handler_function != NULL);
            (this->*(option->m_handler_function))(string(next_argv));

            return true;
        }
        else
        {
            // call the option handler function with an empty argument
            DEBUG_SPEW("short option -" << option->m_short_name);
            ASSERT1(option->m_handler_function != NULL);
            (this->*(option->m_handler_function))(string());

            return false;
        }
    }
}

bool CommandLineParser::HandleLongNameOption (
    char const *arg,
    char const *const next_argv)
{
    ASSERT1(arg != NULL);
    // next_argv may be NULL

    if (*arg == '\0')
        throw string("error: no option name specified after \"--\"");

    CommandLineOption const *option = FindOptionByLongName(arg);
    if (option == NULL)
        throw string("error: no such option --") + arg;

    arg += option->m_long_name.length();

    if (*arg != '\0')
    {
        if (!option->m_requires_a_parameter)
            throw string("error: option --") + option->m_long_name + " does not take a parameter";

        ASSERT1(*arg == '=');
        ++arg;

        // call the option handler function with the argument
        DEBUG_SPEW("long option --" << option->m_long_name << " with arg \"" << arg << "\"");
        ASSERT1(option->m_handler_function != NULL);
        (this->*(option->m_handler_function))(string(arg));

        return false;
    }
    else
    {
        if (option->m_requires_a_parameter)
        {
            if (next_argv == NULL)
                throw string("error: option --") + option->m_long_name + " requires a parameter";

            // call the option handler function with the argument
            DEBUG_SPEW("long option --" << option->m_long_name << " with arg \"" << next_argv << "\"");
            ASSERT1(option->m_handler_function != NULL);
            (this->*(option->m_handler_function))(string(next_argv));

            return true;
        }
        else
        {
            // call the option handler function with an empty argument
            DEBUG_SPEW("long option --" << option->m_long_name);
            ASSERT1(option->m_handler_function != NULL);
            (this->*(option->m_handler_function))(string());

            return false;
        }
    }
}

CommandLineOption const *CommandLineParser::FindOptionByShortName (char const short_name) const
{
    ASSERT1(short_name != '\0');

    for (CommandLineOption const *option = m_option,
                                 *option_end = m_option + m_option_count;
         option != option_end;
         ++option)
    {
        if (option->m_short_name != '\0' && short_name == option->m_short_name)
            return option;
    }

    return NULL;
}

CommandLineOption const *CommandLineParser::FindOptionByLongName (char const *const long_name) const
{
    ASSERT1(long_name != NULL);
    ASSERT1(*long_name != '\0');

    string option_name = long_name;
    if (option_name.find_first_of('=') >= 0)
        option_name = option_name.substr(0, option_name.find_first_of('='));

    for (CommandLineOption const *option = m_option,
                                 *option_end = m_option + m_option_count;
         option != option_end;
         ++option)
    {
        if (!option->m_long_name.empty() && option_name == option->m_long_name)
            return option;
    }

    return NULL;
}

} // end of namespace Xrb
